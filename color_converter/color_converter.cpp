#include "color_converter.h"

Rgb ColorConverter::removeAlpha(const Rgba &pixel) {
  Rgb result = {};
  if (pixel.a == 255) {
    // если пиксель непрозрачный
    result.r = static_cast<double>(pixel.r) / 255.0;
    result.g = static_cast<double>(pixel.g) / 255.0;
    result.b = static_cast<double>(pixel.b) / 255.0;
  } else if (pixel.a > 0) {
    // если пиксель полупрозрачный
    Rgb background = {255, 255, 255};
    double norm_a = static_cast<double>(pixel.a) / 255.0;
    result.r = (pixel.r * norm_a + background.r * (1 - norm_a)) / 255.0;
    result.g = (pixel.g * norm_a + background.g * (1 - norm_a)) / 255.0;
    result.b = (pixel.b * norm_a + background.b * (1 - norm_a)) / 255.0;
  }

  return result;
}

Lab ColorConverter::rgbToLab(const Rgb &pixel) {
  double r = pixel.r;
  double g = pixel.g;
  double b = pixel.b;

  // переход от sRGB к линейному цвету
  if (r > 0.04045) {
    r = pow((r + 0.055) / 1.055, 2.4);
  } else {
    r /= 12.92;
  }
  if (g > 0.04045) {
    g = pow((g + 0.055) / 1.055, 2.4);
  } else {
    g /= 12.92;
  }
  if (b > 0.04045) {
    b = pow((b + 0.055) / 1.055, 2.4);
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
    x = pow(x, 1.0 / 3.0);
  } else {
    x = 7.787 * x + 16.0 / 116.0;
  }
  if (y > 0.008856) {
    y = pow(y, 1.0 / 3.0);
  } else {
    y = 7.787 * y + 16.0 / 116.0;
  }
  if (z > 0.008856) {
    z = pow(z, 1.0 / 3.0);
  } else {
    z = 7.787 * z + 16.0 / 116.0;
  }

  // перевод в Lab
  Lab result;
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
  if (pow(y, 3) > 0.008856) {
    y = pow(y, 3);
  } else {
    y = (y - 16.0 / 116.0) / 7.787;
  }
  if (pow(x, 3) > 0.008856) {
    x = pow(x, 3);
  } else {
    x = (x - 16.0 / 116.0) / 7.787;
  }
  if (pow(z, 3) > 0.008856) {
    z = pow(z, 3);
  } else {
    z = (z - 16.0 / 116.0) / 7.787;
  }

  // обратное преобразование относительно белой точки
  x *= 95.047;
  y *= 100.000;
  z *= 108.883;

  // нормализация XYZ
  x /= 100.0;
  y /= 100.0;
  z /= 100.0;

  // перевод в RGB
  Rgb result = {};
  result.r = x * 3.2406 + y * (-1.5372) + z * (-0.4986);
  result.g = x * (-0.9689) + y * 1.8758 + z * 0.0415;
  result.b = x * 0.0557 + y * (-0.2040) + z * 1.0570;

  // переход от линейного цвета к sRGB
  if (result.r > 0.0031308) {
    result.r = 1.055 * pow(result.r, 1 / 2.4) - 0.055;
  } else {
    result.r *= 12.92;
  }
  if (result.g > 0.0031308) {
    result.g = 1.055 * pow(result.g, 1 / 2.4) - 0.055;
  } else {
    result.g *= 12.92;
  }
  if (result.b > 0.0031308) {
    result.b = 1.055 * pow(result.b, 1 / 2.4) - 0.055;
  } else {
    result.b *= 12.92;
  }

  return result;
}

QString ColorConverter::rgbToHex(const Rgb &pixel) {
  // приведение к диапазону 0-255
  uchar r = static_cast<uchar>(std::min(255.0, round((pixel.r * 255))));
  uchar g = static_cast<uchar>(std::min(255.0, round((pixel.g * 255))));
  uchar b = static_cast<uchar>(std::min(255.0, round((pixel.b * 255))));

  // перевод в 16-ричную систему
  QString result = QString::asprintf("#%02X%02X%02X", r, g, b);
  return result;
}
