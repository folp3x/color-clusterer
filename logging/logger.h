#ifndef LOGGER_H
#define LOGGER_H

#include <QDebug>

#include "core/color_spaces/lab.h"

class Logger {
private:
  Logger() = default;

  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

public:
  static Logger &instance();

  void logPixels(const QList<Lab> &pixels) const;

  void log(const QString &text) const;
  void log() const;
};

#endif // LOGGER_H
