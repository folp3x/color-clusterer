#include "color_clusterer.h"

#include <cmath>

#include <QRandomGenerator>

/*
 * Функция задает начальные центры кластеров с помощью алгоритма k-means++
 * Входные данные: points - набор точек, k - число кластеров
 * Выходные данные: centers - центры кластеров
 */
std::vector<Lab> ColorClusterer::initCenters(const std::vector<Lab> &points,
                                             int k) {
  std::vector<Lab> centers;
  std::vector<double> minDistances(points.size(),
                                   std::numeric_limits<double>::max());

  // случайный выбор первого центра
  centers.push_back(points[QRandomGenerator::global()->bounded(points.size())]);
  for (int i = 1; i < k; i++) {
    double totalSum = 0;
    // обновление квадратов расстояния до последнего выбранного центра
    for (int j = 0; j < points.size(); j++) {
      double d = ColorClusterer::distanceSquared(points[j], centers.back());
      if (d < minDistances[j]) {
        minDistances[j] = d;
      }
      totalSum += minDistances[j];
    }
    // генерация случайного числа в диапазоне от 0 до totalSum
    double target = QRandomGenerator::global()->generateDouble() * totalSum;
    double cumulativeSum = 0;
    // выбор следующего центра с вероятностью, пропорциональной d^2
    for (int j = 0; j < points.size(); j++) {
      cumulativeSum += minDistances[j];
      if (cumulativeSum >= target) {
        centers.push_back(points[j]);
        break;
      }
    }
  }

  return centers;
}

/*
 * Функция вычисляет квадрат евклидова расстояния между двумя точками
 * Входные данные: p1 - первая точка, p2 - вторая точка
 * Выходные данные: distance - вычисленной расстойние
 */
double ColorClusterer::distanceSquared(const Lab &p1, const Lab &p2) {
  double distance =
      pow(p1.l - p2.l, 2) + pow(p1.a - p2.a, 2) + pow(p1.b - p2.b, 2);
  return distance;
}

/*
 * Функция разделяет точки на кластеры с помощью алгоритма k-средних
 * Входные данные: points - набор точек, k - число кластеров,
 *  maxIter - максимальное число итераций для цикла, eps - точность
 * Выходные данные: centers - центры найденных кластеров
 */
std::vector<Lab> ColorClusterer::kMeans(const std::vector<Lab> &points, int k,
                                        int maxIter, double eps) {
  // инициализация начальных центров
  std::vector<Lab> centers = ColorClusterer::initCenters(points, k);

  std::vector<int> clusterSizes = {};
  bool clustersFound = false;
  int iter = 1;
  while (!clustersFound && (iter < maxIter)) {
    std::vector<double> sumL(k, 0);
    std::vector<double> sumA(k, 0);
    std::vector<double> sumB(k, 0);
    clusterSizes.assign(k, 0);

    for (const auto &p : points) {
      // поиск кластера для точки
      int minIndex = 0;
      double minDistance = ColorClusterer::distanceSquared(p, centers[0]);
      for (int i = 1; i < k; i++) {
        double d = ColorClusterer::distanceSquared(p, centers[i]);
        if (d < minDistance) {
          minDistance = d;
          minIndex = i;
        }
      }

      // обновление параметров кластера, к которому ближе текущая точка
      sumL[minIndex] += p.l;
      sumA[minIndex] += p.a;
      sumB[minIndex] += p.b;
      clusterSizes[minIndex]++;
    }

    std::vector<Lab> newCenters(k, Lab{});
    clustersFound = true;
    // вычисление новых центров для каждого кластера
    for (int i = 0; i < k; i++) {
      if (clusterSizes[i] == 0) {
        // выбор случайно точки если кластер пустой
        newCenters[i] =
            points[QRandomGenerator::global()->bounded(points.size())];
        clustersFound = false;
      } else {
        // вычисление координат нового центра
        newCenters[i].l = sumL[i] / clusterSizes[i];
        newCenters[i].a = sumA[i] / clusterSizes[i];
        newCenters[i].b = sumB[i] / clusterSizes[i];
        if (sqrt(ColorClusterer::distanceSquared(centers[i], newCenters[i])) >
            eps) {
          clustersFound = false;
        }
      }
    }

    centers = newCenters;
    iter++;
  }
  // сортировка центров по убыванию размеров кластеров
  for (int i = 0; i < centers.size() - 1; i++) {
    int maxInd = i;
    for (int j = i; j < centers.size(); j++) {
      if (clusterSizes[j] > clusterSizes[maxInd]) {
        maxInd = j;
      }
    }
    std::swap(clusterSizes[i], clusterSizes[maxInd]);
    std::swap(centers[i], centers[maxInd]);
  }

  return centers;
}
