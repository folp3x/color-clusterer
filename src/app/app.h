#ifndef APP_H
#define APP_H

#include <QObject>
#include <QVariantList>

#include "core/color_spaces/lab.h"
#include "core/palette_color/palette_color.h"

class App : public QObject {
  Q_OBJECT
  Q_PROPERTY(QVariantList palette READ palette NOTIFY paletteChanged)
  Q_PROPERTY(bool isProcessingImage READ isProcessingImage NOTIFY
                 isProcessingImageChanged)

private:
  QVariantList m_palette{};

  bool m_isProcessingImage = false;

  static QList<Lab> getPixels(const QImage &img);

  void setPalette(const QList<Lab> &colors,
                  const QList<size_t> colorPixelCounts, size_t totalPixelCount);

  void setIsProcessingImage(bool isProcessingImage);

public:
  explicit App(QObject *parent = nullptr);

  Q_INVOKABLE void clusterImage(const QUrl &fileUrl, int colorCount);

  Q_INVOKABLE void savePalette(const QUrl &fileUrl) const;

  QVariantList palette() const;

  bool isProcessingImage() const;

signals:
  void paletteChanged();
  void isProcessingImageChanged();
};

#endif // APP_H
