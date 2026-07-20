#ifndef LAB_H
#define LAB_H

#include <QString>

struct Lab {
  double l;
  double a;
  double b;

  static QString channelToStr(double channel) {
    return QString("%1").arg(channel, 0, 'f', 4);
  }

  QString toString() const {
    return QString("{l=%1, a=%2, b=%3}")
        .arg(channelToStr(l))
        .arg(channelToStr(a))
        .arg(channelToStr(b));
  }
};

#endif // LAB_H
