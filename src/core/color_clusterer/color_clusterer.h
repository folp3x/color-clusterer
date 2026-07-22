#ifndef COLOR_CLUSTERER_H
#define COLOR_CLUSTERER_H

#include <QList>
#include <QMutex>
#include <QRandomGenerator>

#include "core/color_spaces/lab.h"

class ColorClusterer {
private:
  static constexpr int RandomSeed = 42;

  QRandomGenerator m_gen{RandomSeed};

  int m_k;
  int m_maxIter;
  double m_eps;

  QList<Lab> m_clusterCenters{};
  QList<size_t> m_clusterSizes{};

  static double distanceSquared(const Lab &p1, const Lab &p2);

  static double distance(const Lab &p1, const Lab &p2);

  void initCenters(const QList<Lab> &points);

public:
  explicit ColorClusterer(int k = 6, int maxIter = 20, double eps = 1);

  void kMeans(const QList<Lab> &points);

  void sortByClusterSize(bool descending = false);

  QList<Lab> getClusterCenters() const;
  QList<size_t> getClusterSizes() const;
};

#endif // COLOR_CLUSTERER_H
