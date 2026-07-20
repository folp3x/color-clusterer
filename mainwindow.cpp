#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGraphicsPixmapItem>
#include <QPainter>

#include <QFileDialog>
#include <QStandardPaths>
#include <QTimer>
#include <QVBoxLayout>

#include "color_clusterer/color_clusterer.h"
#include "color_converter/color_converter.h"
#include "color_spaces/rgb.h"
#include "color_spaces/rgba.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  setWindowTitle("Генератор цветовых палитр");

  connect(ui->btnChooseImg, &QPushButton::clicked, this,
          &MainWindow::onBtnChooseImgClicked);

  QTimer::singleShot(0, loadExample);
}

MainWindow::~MainWindow() { delete ui; }

QList<Lab> MainWindow::getPixels(const QImage &img) {
  QImage rgbaImg = img.convertToFormat(QImage::Format_RGBA8888);

  // выборка пикселей
  int pixelsCount = rgbaImg.height() * rgbaImg.width();
  int maxCount = 600000;
  int step = 1;
  if (pixelsCount > maxCount) {
    step = std::ceil(std::sqrt(static_cast<double>(pixelsCount) / maxCount));
  }

  uchar *bits = rgbaImg.bits();
  QList<Lab> pixels{};
  // перебор байтов для каждого пикселя
  for (int y = 0; y < rgbaImg.height(); y += step) {
    for (int x = 0; x < rgbaImg.width(); x += step) {
      uchar *pixel = bits + y * rgbaImg.bytesPerLine() + x * 4;
      uchar alpha = pixel[3];

      if (alpha != 0) {
        Rgba rgbaPixel = {pixel[0], pixel[1], pixel[2], alpha};
        Rgb rgbPixel = ColorConverter::removeAlpha(rgbaPixel);
        Lab labPixel = ColorConverter::rgbToLab(rgbPixel);
        pixels.push_back(labPixel);
      }
    }
  }

  return pixels;
}

void MainWindow::showPallete(const QList<Lab> &palette,
                             const QList<size_t> &occurrences,
                             size_t pixelCount) {
  clearPaletteLayout();

  if (palette.size() != occurrences.size()) {
    return;
  }

  size_t curRow = 0;
  size_t curCol = 0;
  for (int i = 0; i < palette.size(); ++i) {
    QWidget *paletteElem = new QWidget();
    paletteElem->setMinimumWidth(90);

    QVBoxLayout *paletteElemLayout = new QVBoxLayout(paletteElem);
    paletteElemLayout->setSpacing(2);
    paletteElemLayout->setContentsMargins(4, 4, 4, 4);

    Rgb rgb = ColorConverter::labToRgb(palette[i]);
    QString hex = ColorConverter::rgbToHex(rgb);

    QLabel *colorLabel = new QLabel();
    colorLabel->setStyleSheet("background-color: " + hex);
    colorLabel->setMinimumHeight(40);
    paletteElemLayout->addWidget(colorLabel);

    QLabel *hexLabel = new QLabel();
    hexLabel->setText(hex);
    hexLabel->setAlignment(Qt::AlignCenter);
    hexLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QFont hexLabelFont = hexLabel->font();
    hexLabelFont.setPointSize(11);
    hexLabel->setFont(hexLabelFont);

    paletteElemLayout->addWidget(hexLabel);

    QLabel *percentageLabel = new QLabel();
    double percentage = static_cast<double>(occurrences[i]) / pixelCount * 100;
    QString percentageStr = QString("%1\%").arg(percentage, 0, 'g', 2);
    percentageLabel->setText(percentageStr);
    percentageLabel->setAlignment(Qt::AlignCenter);

    QFont percentageLabelFont = percentageLabel->font();
    percentageLabelFont.setPointSize(10);
    percentageLabelFont.setBold(true);
    percentageLabel->setFont(percentageLabelFont);

    paletteElemLayout->addWidget(percentageLabel);

    if (curCol >= MaxPaletteLayoutCols) {
      curCol = 0;
      curRow++;
    }

    ui->paletteLayout->addWidget(paletteElem, curRow, curCol);
    curCol++;
  }

  ui->paletteLayout->setVerticalSpacing(0);
}

void MainWindow::showImage(const QImage &img) {
  // создание объекта для вывода изображения
  QPixmap pixmap = QPixmap::fromImage(img);

  // изображение рисуется на белом фоне
  QPixmap result(pixmap.size());
  result.fill(Qt::white);
  QPainter painter(&result);

  // улучшение качества изображения
  painter.setRenderHint(QPainter::Antialiasing, false);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  painter.drawPixmap(0, 0, pixmap);
  painter.end();
  pixmap = result;

  // создание сцены для вывода изображения
  if (!m_scene) {
    m_scene = new QGraphicsScene(this);
  } else {
    m_scene->clear();
  }
  QGraphicsPixmapItem *item = m_scene->addPixmap(pixmap);
  item->setPos(item->boundingRect().center());

  // установка сцены и настройка параметров
  ui->image->setScene(m_scene);
  ui->image->setRenderHint(QPainter::Antialiasing);
  ui->image->setRenderHint(QPainter::SmoothPixmapTransform);
  ui->image->fitInView(item, Qt::KeepAspectRatio);
}

void MainWindow::loadExample() {
  QImage img(ExampleImgFilePath);
  if (!img.isNull()) {
    QList<Lab> pixels = getPixels(img);

    ColorClusterer clusterer{};
    clusterer.kMeans(pixels);
    clusterer.sortByClusterSize();

    showImage(img);
    showPallete(clusterer.getCenters(), clusterer.getClusterSizes(),
                clusterer.getTotalPoints());
  }
}

void MainWindow::clearPaletteLayout() {
  while (QLayoutItem *paletteItem = ui->paletteLayout->takeAt(0)) {
    if (QWidget *paletteWidget = paletteItem->widget()) {
      paletteWidget->deleteLater();
    }
  }
}

void MainWindow::onBtnChooseImgClicked() {
  QString filePath = QFileDialog::getOpenFileName(
      this, "Выберите файл",
      QStandardPaths::writableLocation(QStandardPaths::DownloadLocation),
      "Изображения (*.bmp *.jpg *.jpeg *.png)");

  if (!filePath.isEmpty()) {
    QImage img(filePath);
    if (!img.isNull()) {
      ui->labelExampleTitle->clear();

      QList<Lab> pixels = getPixels(img);

      ColorClusterer clusterer{};
      clusterer.kMeans(pixels);
      clusterer.sortByClusterSize();

      showImage(img);
      showPallete(clusterer.getCenters(), clusterer.getClusterSizes(),
                  clusterer.getTotalPoints());
    }
  }
}
