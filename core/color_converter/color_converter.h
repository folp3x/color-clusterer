#ifndef COLORCONVERTER_H
#define COLORCONVERTER_H

#include "core/color_spaces/lab.h"
#include "core/color_spaces/rgb.h"
#include "core/color_spaces/rgba.h"

class ColorConverter {
private:
  static constexpr uchar TransparentAlpha = 0;
  static constexpr uchar NonTransparentAlpha = 255;

  static constexpr double MaxRgbChannelValue = 255;

  static uchar denormalizeRgbChannel(double channel);

  static double normalizeRgbChannel(uchar channel);

  static uchar blendRgbChannel(uchar pixelChannel, uchar bgChannel,
                               double alpha);

public:
  static Rgb removeAlpha(const Rgba &pixel);

  static Lab rgbToLab(const Rgb &pixel);
  static Rgb labToRgb(const Lab &pixel);

  static QString rgbToHex(const Rgb &pixel);
};

#endif // COLORCONVERTER_H
