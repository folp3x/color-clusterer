#ifndef PALETTE_COLOR_H
#define PALETTE_COLOR_H

#include <QString>

struct PaletteColor {
  Q_GADGET

  Q_PROPERTY(QString hexCode MEMBER hexCode)
  Q_PROPERTY(double percentage MEMBER percentage)

public:
  QString hexCode = "";
  double percentage = 0;
};

Q_DECLARE_METATYPE(PaletteColor)

#endif // PALETTE_COLOR_H
