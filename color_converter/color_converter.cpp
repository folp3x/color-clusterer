#include "color_converter.h"

uchar ColorConverter::denormalizeRgbChannel(double channel) {
  double rounded = std::round(channel * MaxRgbChannelValue);
  return static_cast<uchar>(std::min(MaxRgbChannelValue, rounded));
}

double ColorConverter::normalizeRgbChannel(uchar channel) {
  return static_cast<double>(channel) / MaxRgbChannelValue;
}

uchar ColorConverter::blendRgbChannel(uchar pixelChannel, uchar bgChannel,
                                      double alpha) {
  double normalizedPixel = normalizeRgbChannel(pixelChannel);
  double normalizedBg = normalizeRgbChannel(bgChannel);

  return denormalizeRgbChannel(normalizedPixel * alpha +
                               normalizedBg * (1 - alpha));
}

Rgb ColorConverter::removeAlpha(const Rgba &pixel) {
  Rgb whiteBg = {255, 255, 255};
  if (pixel.a == NonTransparentAlpha) {
    return Rgb{pixel.r, pixel.g, pixel.b};
  } else if (pixel.a > TransparentAlpha) {
    double alpha = normalizeRgbChannel(pixel.a);

    Rgb result{};
    result.r = blendRgbChannel(pixel.r, whiteBg.r, alpha);
    result.g = blendRgbChannel(pixel.g, whiteBg.g, alpha);
    result.b = blendRgbChannel(pixel.b, whiteBg.b, alpha);
    return result;
  } else {
    return whiteBg;
  }
}

Lab ColorConverter::rgbToLab(const Rgb &pixel) {
  double r = normalizeRgbChannel(pixel.r);
  double g = normalizeRgbChannel(pixel.g);
  double b = normalizeRgbChannel(pixel.b);

  // переход от sRGB к линейному цвету
  if (r > 0.04045) {
    r = std::pow((r + 0.055) / 1.055, 2.4);
  } else {
    r /= 12.92;
  }
  if (g > 0.04045) {
    g = std::pow((g + 0.055) / 1.055, 2.4);
  } else {
    g /= 12.92;
  }
  if (b > 0.04045) {
    b = std::pow((b + 0.055) / 1.055, 2.4);
  } else {
    b /= 12.92;
  }

  // масштабирование под белую точку D65
  r *= 100;
  g *= 100;
  b *= 100;

  // перевод в XYZ
  double x = r * 0.4124 + g * 0.3576 + b * 0.1805;
  double y = r * 0.2126 + g * 0.7152 + b * 0.0722;
  double z = r * 0.0193 + g * 0.1192 + b * 0.9505;

  // нормировка относительно белой точки
  x /= 95.047;
  y /= 100.000;
  z /= 108.883;

  // коррекция XYZ
  if (x > 0.008856) {
    x = std::pow(x, 1.0 / 3.0);
  } else {
    x = 7.787 * x + 16.0 / 116.0;
  }
  if (y > 0.008856) {
    y = std::pow(y, 1.0 / 3.0);
  } else {
    y = 7.787 * y + 16.0 / 116.0;
  }
  if (z > 0.008856) {
    z = std::pow(z, 1.0 / 3.0);
  } else {
    z = 7.787 * z + 16.0 / 116.0;
  }

  // перевод в Lab
  Lab result{};
  result.l = (116 * y) - 16;
  result.a = 500 * (x - y);
  result.b = 200 * (y - z);

  return result;
}

Rgb ColorConverter::labToRgb(const Lab &pixel) {
  // перевод в XYZ
  double y = (pixel.l + 16) / 116.0;
  double x = pixel.a / 500.0 + y;
  double z = y - pixel.b / 200.0;

  // обратное преобразование XYZ
  if (std::pow(y, 3) > 0.008856) {
    y = std::pow(y, 3);
  } else {
    y = (y - 16.0 / 116.0) / 7.787;
  }
  if (std::pow(x, 3) > 0.008856) {
    x = std::pow(x, 3);
  } else {
    x = (x - 16.0 / 116.0) / 7.787;
  }
  if (std::pow(z, 3) > 0.008856) {
    z = std::pow(z, 3);
  } else {
    z = (z - 16.0 / 116.0) / 7.787;
  }

  // обратное преобразование относительно белой точки
  x *= 95.047;
  y *= 100.000;
  z *= 108.883;

  // нормировка XYZ
  x /= 100.0;
  y /= 100.0;
  z /= 100.0;

  // перевод в RGB
  double r, g, b;
  r = x * 3.2406 + y * (-1.5372) + z * (-0.4986);
  g = x * (-0.9689) + y * 1.8758 + z * 0.0415;
  b = x * 0.0557 + y * (-0.2040) + z * 1.0570;

  // переход от линейного цвета к sRGB
  if (r > 0.0031308) {
    r = 1.055 * pow(r, 1 / 2.4) - 0.055;
  } else {
    r *= 12.92;
  }
  if (g > 0.0031308) {
    g = 1.055 * pow(g, 1 / 2.4) - 0.055;
  } else {
    g *= 12.92;
  }
  if (b > 0.0031308) {
    b = 1.055 * pow(b, 1 / 2.4) - 0.055;
  } else {
    b *= 12.92;
  }

  Rgb result = {};
  result.r = denormalizeRgbChannel(r);
  result.g = denormalizeRgbChannel(g);
  result.b = denormalizeRgbChannel(b);

  return result;
}

QString ColorConverter::rgbToHex(const Rgb &pixel) {
  return QString::asprintf("#%02X%02X%02X", pixel.r, pixel.g, pixel.b);
}
