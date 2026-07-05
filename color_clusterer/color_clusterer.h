#ifndef COLOR_CLUSTERER_H
#define COLOR_CLUSTERER_H

#include <vector>

#include "color_spaces/lab.h"

class ColorClusterer {
public:
  static std::vector<Lab> initCenters(const std::vector<Lab> &points, int k);
  static double distanceSquared(const Lab &p1, const Lab &p2);
  static std::vector<Lab> kMeans(const std::vector<Lab> &points, int k,
                                 int maxIter, double eps);
};

#endif // COLOR_CLUSTERER_H
