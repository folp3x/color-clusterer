#include "app.h"

#include <QElapsedTimer>
#include <QFile>
#include <QImage>
#include <QQmlFile>

#include <QtConcurrent/QtConcurrent>

#include "core/color_clusterer/color_clusterer.h"
#include "core/color_converter/color_converter.h"
#include "logging/logger.h"

App::App(QObject *parent) : QObject{parent} {}

void App::clusterImage(const QUrl &fileUrl, int colorCount) {
  QString filePath = QQmlFile::urlToLocalFileOrQrc(fileUrl);

  QImage img(filePath);
  if (!img.isNull()) {
    setIsProcessingImage(true);

    (void)QtConcurrent::run([this, img, colorCount]() {
      QList<Lab> pixels = getPixels(img);

      ColorClusterer clusterer{colorCount};

      QElapsedTimer timer;
      timer.start();

      clusterer.kMeans(pixels);

      int elapsedMs = timer.elapsed();
      Logger::instance().log(QString("KMeans finished in %1ms").arg(elapsedMs));

      clusterer.sortByClusterSize();

      size_t pixelCount = pixels.size();
      QMetaObject::invokeMethod(this, [this, clusterer, pixelCount]() {
        setPalette(clusterer.getClusterCenters(), clusterer.getClusterSizes(),
                   pixelCount);
        setIsProcessingImage(false);
      });
    });
  }
}

void App::savePalette(const QUrl &fileUrl) const {
  QString filePath = QQmlFile::urlToLocalFileOrQrc(fileUrl);

  QFile file(filePath);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    for (const auto &variant : m_palette) {
      PaletteColor color = variant.value<PaletteColor>();
      file.write(color.hexCode.toUtf8());
    }

    file.close();
  }
}

QVariantList App::palette() const { return m_palette; }

bool App::isProcessingImage() const { return m_isProcessingImage; }

void App::setPalette(const QList<Lab> &colors,
                     const QList<size_t> colorPixelCounts,
                     size_t totalPixelCount) {
  if (colors.size() != colorPixelCounts.size()) {
    return;
  }

  m_palette.clear();
  m_palette.reserve(colors.size());
  for (int i = 0; i < colors.size(); ++i) {
    Rgb rgb = ColorConverter::labToRgb(colors[i]);
    QString hexCode = ColorConverter::rgbToHex(rgb);
    double percentage =
        static_cast<double>(colorPixelCounts[i]) / totalPixelCount;
    PaletteColor color(hexCode, percentage);

    m_palette.push_back(QVariant::fromValue(color));
  }

  emit paletteChanged();
}

void App::setIsProcessingImage(bool isProcessingImage) {
  if (m_isProcessingImage != isProcessingImage) {
    m_isProcessingImage = isProcessingImage;
    emit isProcessingImageChanged();
  }
}

QList<Lab> App::getPixels(const QImage &img) {
  static constexpr int MaxChosenPixels = 800'000;
  static constexpr size_t RgbaChannelCount = 4;

  QImage rgbaImg = img.convertToFormat(QImage::Format_RGBA8888);

  int h = rgbaImg.height();
  int w = rgbaImg.width();
  int pixelCount = h * w;

  int stepX = 1;
  int stepY = 1;
  size_t chosenCount = pixelCount;
  if (pixelCount > MaxChosenPixels) {
    stepX = std::ceil(w / std::sqrt(MaxChosenPixels));
    double relDiff = static_cast<double>(h) / w;
    stepY = std::ceil(relDiff * stepX);
    chosenCount = (h / stepY) * (w / stepX);
  }

#ifdef QT_DEBUG
  Logger::instance().log(QString("Chosen %1/%2 pixels, stepX = %3, stepY = %4)")
                             .arg(chosenCount)
                             .arg(pixelCount)
                             .arg(stepX)
                             .arg(stepY));
#endif

  uchar *bitsPtr = rgbaImg.bits();
  QList<Lab> chosen{};
  chosen.reserve(chosenCount);
  for (int y = 0; y < h; y += stepY) {
    for (int x = 0; x < w; x += stepX) {
      uchar *pixelPtr =
          bitsPtr + y * rgbaImg.bytesPerLine() + x * RgbaChannelCount;
      uchar alpha = pixelPtr[3];

      if (alpha != 0) {
        Rgba rgba = {pixelPtr[0], pixelPtr[1], pixelPtr[2], alpha};
        Rgb rgb = ColorConverter::removeAlpha(rgba);
        Lab lab = ColorConverter::rgbToLab(rgb);
        chosen.push_back(lab);
      }
    }
  }

  return chosen;
}