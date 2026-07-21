#include "app.h"

#include <QImage>
#include <QQmlFile>
#include <QtConcurrent/QtConcurrent>

#include "core/color_clusterer/color_clusterer.h"
#include "core/color_converter/color_converter.h"

App::App(QObject *parent) : QObject{parent} {}

void App::clusterImage(const QUrl &fileUrl, int colorCount) {
  QString filePath = QQmlFile::urlToLocalFileOrQrc(fileUrl);

  QImage img(filePath);
  if (!img.isNull()) {
    setIsClustering(true);

    (void)QtConcurrent::run([this, img, colorCount]() {
      QList<Lab> pixels = getPixels(img);

      ColorClusterer clusterer{colorCount};
      clusterer.kMeans(pixels);
      clusterer.sortByClusterSize();

      QMetaObject::invokeMethod(
          this,
          [this, clusterer]() {
            setPalette(clusterer.getCenters(), clusterer.getClusterSizes(),
                       clusterer.getTotalPoints());
            setIsClustering(false);
          },
          Qt::QueuedConnection);
    });
  }
}

QVariantList App::palette() const { return m_palette; }

bool App::isClustering() const { return m_isClustering; }

void App::setPalette(const QList<Lab> &colors, const QList<size_t> occurancies,
                     size_t totalCount) {
  if (colors.size() != occurancies.size()) {
    return;
  }

  m_palette.clear();
  m_palette.reserve(colors.size());
  for (int i = 0; i < colors.size(); ++i) {
    Rgb rgb = ColorConverter::labToRgb(colors[i]);
    QString hexCode = ColorConverter::rgbToHex(rgb);
    double percentage = static_cast<double>(occurancies[i]) / totalCount;
    PaletteColor color(hexCode, percentage);

    m_palette.push_back(QVariant::fromValue(color));
  }

  emit paletteChanged();
}

void App::setIsClustering(bool isClustering) {
  if (m_isClustering != isClustering) {
    m_isClustering = isClustering;
    emit isClusteringChanged();
  }
}

QList<Lab> App::getPixels(const QImage &img) {
  static constexpr int MaxChosenPixels = 600'000;

  QImage rgbaImg = img.convertToFormat(QImage::Format_RGBA8888);

  int pixelCount = rgbaImg.height() * rgbaImg.width();
  int step = 1;
  if (pixelCount > MaxChosenPixels) {
    double count = static_cast<double>(pixelCount);
    step = std::ceil(std::sqrt(count / MaxChosenPixels));
  }

  uchar *bits = rgbaImg.bits();
  QList<Lab> pixels{};

  for (int y = 0; y < rgbaImg.height(); y += step) {
    for (int x = 0; x < rgbaImg.width(); x += step) {
      uchar *pixel = bits + y * rgbaImg.bytesPerLine() + x * 4;
      uchar alpha = pixel[3];

      if (alpha != 0) {
        Rgba rgba = {pixel[0], pixel[1], pixel[2], alpha};
        Rgb rgb = ColorConverter::removeAlpha(rgba);
        Lab lab = ColorConverter::rgbToLab(rgb);
        pixels.push_back(lab);
      }
    }
  }

  return pixels;
}