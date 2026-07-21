#ifndef COLOR_CLUSTERER_H
#define COLOR_CLUSTERER_H

#include <QList>
#include <QRandomGenerator>

#include "core/color_spaces/lab.h"

class ColorClusterer {
private:
  static constexpr int RandomSeed = 42;

  QRandomGenerator m_gen{RandomSeed};

  int m_k;
  int m_maxIter;
  double m_eps;

  QList<Lab> m_centers{};
  QList<size_t> m_clusterSizes{};
  size_t m_totalPoints;

  static double distanceSquared(const Lab &p1, const Lab &p2);

  static double distance(const Lab &p1, const Lab &p2);

  void initCenters(const QList<Lab> &points);

public:
  explicit ColorClusterer(int k = 6, int maxIter = 20, double eps = 1);

  void kMeans(const QList<Lab> &points);

  void sortByClusterSize(bool descending = false);

  QList<Lab> getCenters() const;
  QList<size_t> getClusterSizes() const;
  size_t getTotalPoints() const;
};

#endif // COLOR_CLUSTERER_H
