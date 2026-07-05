#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPainter>

#include <QFileDialog>
#include <QStandardPaths>
#include <QTimer>

#include "color_clusterer/color_clusterer.h"
#include "color_converter/color_converter.h"
#include "color_spaces/rgb.h"
#include "color_spaces/rgba.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  setWindowTitle("Генератор цветовых палитр");
  // вывод примера палитры после загрузки приложения
  QTimer::singleShot(0, this, &MainWindow::loadExample);
}

MainWindow::~MainWindow() { delete ui; }

/*
 * Функция создает палитру цветов изображения
 * Входные данные: img - изображение
 * Выходные данные: palette - созданная палитра
 */
std::vector<Lab> MainWindow::getPallete(const QImage &img) {
  QImage rgbaImg = img.convertToFormat(QImage::Format_RGBA8888);

  // выборка пикселей
  int pixelsCount = rgbaImg.height() * rgbaImg.width();
  int maxCount = 600000;
  int step = 1;
  if (pixelsCount > maxCount) {
    step = std::ceil(std::sqrt(static_cast<double>(pixelsCount) / maxCount));
  }

  uchar *bits = rgbaImg.bits();
  std::vector<Lab> pixels = {};
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

  // создание палитры цветов изображения
  std::vector<Lab> palette = ColorClusterer::kMeans(pixels, 6, 20, 1);

  return palette;
}

/*
 * Функция выводит палитру цветов изображения
 * Входные данные: palette - палитра
 * Выходные данные: отсутствует
 */
void MainWindow::showPallete(const std::vector<Lab> &palette) {
  for (int i = 0; i < palette.size(); i++) {
    Rgb rgbColor = ColorConverter::labToRgb(palette[i]);
    QString hex = ColorConverter::rgbToHex(rgbColor);

    // вывод цвета
    QLayoutItem *colorItem = ui->gridLayout->itemAtPosition(0, i);
    QLabel *colorLabel = qobject_cast<QLabel *>(colorItem->widget());
    colorLabel->setStyleSheet("background-color: " + hex);

    // вывод 16-ричного кода
    QLayoutItem *hexItem = ui->gridLayout->itemAtPosition(1, i);
    QLabel *hexLabel = qobject_cast<QLabel *>(hexItem->widget());
    hexLabel->setText(hex);
  }
}

/*
 * Функция выводит изображение на белом фоне
 * Входные данные: img - изображение
 * Выходные данные: отсутствует
 */
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
  QGraphicsScene *scene = new QGraphicsScene(this);
  scene->clear();
  QGraphicsPixmapItem *item = scene->addPixmap(pixmap);
  item->setPos(item->boundingRect().center());

  // установка сцены и настройка параметров
  ui->graphicsView->setScene(scene);
  ui->graphicsView->setRenderHint(QPainter::Antialiasing);
  ui->graphicsView->setRenderHint(QPainter::SmoothPixmapTransform);
  ui->graphicsView->fitInView(item, Qt::KeepAspectRatio);
}

/*
 * Функция создает и выводит пример палитры
 * Входные данные: отсутствуют
 * Выходные данные: отсутствует
 */
void MainWindow::loadExample() {
  QString filePath = ":/examples/example.jpg";
  QImage img(filePath);
  if (!img.isNull()) {
    // если файл корректно загружен
    std::vector<Lab> palette = getPallete(img);
    showImage(img);
    showPallete(palette);
  }
}

void MainWindow::on_pushButton_clicked() {
  QString filePath = QFileDialog::getOpenFileName(
      this, tr("Выберите файл"),
      QStandardPaths::writableLocation(QStandardPaths::DownloadLocation),
      tr("Изображения (*.bmp *.jpg *.jpeg *.png)"));
  if (!filePath.isEmpty()) {
    // если файл был выбран
    QImage img(filePath);
    if (!img.isNull()) {
      // если файл корректно загружен
      std::vector<Lab> palette = getPallete(img);
      showImage(img);
      showPallete(palette);
      ui->labelExampleTitle->clear();
    }
  }
}
