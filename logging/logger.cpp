#include "logger.h"

Logger &Logger::instance() {
  static Logger instance;
  return instance;
}

void Logger::logPixels(const QList<Lab> &pixels) const {
  if (pixels.empty()) {
    log("[]");
  }

  for (int i = 0; i < pixels.size() - 1; ++i) {
    log(pixels[i].toString() + ", ");
  }
  log(pixels.back().toString());
}

void Logger::log(const QString &text) const {
  qDebug().nospace().noquote() << text;
}

void Logger::log() const { log(""); }
