#ifndef APP_H
#define APP_H

#include <QObject>
#include <QVariantList>

#include "core/color_spaces/lab.h"
#include "core/palette_color/palette_color.h"

class App : public QObject {
  Q_OBJECT
  Q_PROPERTY(QVariantList palette READ palette NOTIFY paletteChanged)
  Q_PROPERTY(bool isClustering READ isClustering NOTIFY isClusteringChanged)

private:
  QVariantList m_palette{};

  bool m_isClustering = false;

  static QList<Lab> getPixels(const QImage &img);

  void setPalette(const QList<Lab> &colors, const QList<size_t> occurancies,
                  size_t totalCount);

  void setIsClustering(bool isClustering);

public:
  explicit App(QObject *parent = nullptr);

  Q_INVOKABLE void clusterImage(const QUrl &fileUrl, int colorCount);

  QVariantList palette() const;

  bool isClustering() const;

signals:
  void paletteChanged();

  void isClusteringChanged();
};

#endif // APP_H
