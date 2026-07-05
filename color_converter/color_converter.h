#ifndef COLORCONVERTER_H
#define COLORCONVERTER_H

#include <QString>

#include "color_spaces/lab.h"
#include "color_spaces/rgb.h"
#include "color_spaces/rgba.h"

class ColorConverter {
public:
  static Rgb removeAlpha(const Rgba &pixel);
  static Lab rgbToLab(const Rgb &pixel);
  static Rgb labToRgb(const Lab &pixel);
  static QString rgbToHex(const Rgb &pixel);
};

#endif // COLORCONVERTER_H
