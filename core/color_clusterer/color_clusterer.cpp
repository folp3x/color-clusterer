#include "color_clusterer.h"

#include "logging/logger.h"

void ColorClusterer::initCenters(const QList<Lab> &points) {
  QList<double> minDistances(points.size(), std::numeric_limits<double>::max());

  m_centers.clear();
  m_centers.reserve(m_k);

  // случайный выбор первого центра
  int index = m_gen.bounded(points.size());
  m_centers.push_back(points[index]);

  for (int i = 1; i < m_k; ++i) {
    double totalSum = 0;
    // обновление квадратов расстояния до последнего выбранного центра
    for (int j = 0; j < points.size(); ++j) {
      double d = distanceSquared(points[j], m_centers.back());
      if (d < minDistances[j]) {
        minDistances[j] = d;
      }
      totalSum += minDistances[j];
    }

    double targetSum = m_gen.generateDouble() * totalSum;
    double cumulativeSum = 0;

    // выбор следующего центра с вероятностью, пропорциональной d^2
    for (int j = 0; j < points.size(); ++j) {
      cumulativeSum += minDistances[j];
      if (cumulativeSum >= targetSum) {
        m_centers.push_back(points[j]);
        break;
      }
    }
  }
}

void ColorClusterer::sortByClusterSize(bool descending) {
  for (int i = 0; i < m_centers.size() - 1; ++i) {
    int swappedInd = i;

    for (int j = i; j < m_centers.size(); ++j) {
      double cur = m_clusterSizes[j];

      bool isSwappedAsc = !descending && (cur > m_clusterSizes[swappedInd]);
      bool isSwappedDesc = descending && (cur < m_clusterSizes[swappedInd]);

      if (isSwappedAsc || isSwappedDesc) {
        swappedInd = j;
      }
    }

    std::swap(m_clusterSizes[i], m_clusterSizes[swappedInd]);
    std::swap(m_centers[i], m_centers[swappedInd]);
  }
}

double ColorClusterer::distanceSquared(const Lab &p1, const Lab &p2) {
  double distance = std::pow(p1.l - p2.l, 2) + std::pow(p1.a - p2.a, 2) +
                    std::pow(p1.b - p2.b, 2);
  return distance;
}

double ColorClusterer::distance(const Lab &p1, const Lab &p2) {
  return std::sqrt(distanceSquared(p1, p2));
}

ColorClusterer::ColorClusterer(int k, int maxIter, double eps)
    : m_k{k}, m_maxIter{maxIter}, m_eps{eps} {}

void ColorClusterer::kMeans(const QList<Lab> &points) {
  m_totalPoints = points.size();

  initCenters(points);

  bool clustersFound = false;
  int curIter = 1;

  while (!clustersFound && (curIter < m_maxIter)) {
    std::vector<double> sumL(m_k, 0);
    std::vector<double> sumA(m_k, 0);
    std::vector<double> sumB(m_k, 0);
    m_clusterSizes.assign(m_k, 0);

    for (const auto &p : points) {
      // поиск кластера для точки
      int minIndex = 0;
      double minDistance = distanceSquared(p, m_centers[0]);

      for (int i = 1; i < m_k; ++i) {
        double d = distanceSquared(p, m_centers[i]);
        if (d < minDistance) {
          minDistance = d;
          minIndex = i;
        }
      }

      sumL[minIndex] += p.l;
      sumA[minIndex] += p.a;
      sumB[minIndex] += p.b;
      m_clusterSizes[minIndex]++;
    }

    QList<Lab> newCenters(m_k, Lab{});
    clustersFound = true;

#ifdef QT_DEBUG
    double minDistance = std::numeric_limits<double>::max();
    double distanceSum = 0;
#endif

    // вычисление новых центров для каждого кластера
    for (int i = 0; i < m_k; ++i) {
      if (m_clusterSizes[i] == 0) {
        int index = m_gen.bounded(points.size());
        newCenters[i] = points[index];
        clustersFound = false;
      } else {
        newCenters[i].l = sumL[i] / m_clusterSizes[i];
        newCenters[i].a = sumA[i] / m_clusterSizes[i];
        newCenters[i].b = sumB[i] / m_clusterSizes[i];

        double d = distance(m_centers[i], newCenters[i]);
        if (d > m_eps) {
          clustersFound = false;
        }

#ifdef QT_DEBUG
        distanceSum += d;
        if (d < minDistance) {
          minDistance = d;
        }
#endif
      }
    }

#ifdef QT_DEBUG
    Logger::instance().log(QString("Iter %1 - min dist=%2, dist sum = %3:")
                               .arg(curIter)
                               .arg(minDistance)
                               .arg(distanceSum));
    Logger::instance().logPixels(newCenters);
    Logger::instance().log();
#endif

    m_centers = std::move(newCenters);
    curIter++;
  }
}

QList<Lab> ColorClusterer::getCenters() const { return m_centers; }

QList<size_t> ColorClusterer::getClusterSizes() const { return m_clusterSizes; }

size_t ColorClusterer::getTotalPoints() const { return m_totalPoints; }
