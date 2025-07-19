#ifndef UTIL_MATHUTILS_H_
#define UTIL_MATHUTILS_H_

#include <cmath>

inline double Ratio(int a, int b) {
  double r = static_cast<double>(a) / b;

  double eps = 0.00001;
  if (r > 0 - eps && r < 0 + eps) { return 0.0; }
  if (r > 1 - eps && r < 1 + eps) { return 1.0; }

  return r;
}

inline double Ratio(double a, double b) {
  double r = a / b;

  double eps = 0.00001;
  if (r > 0 - eps && r < 0 + eps) { return 0.0; }
  if (r > 1 - eps && r < 1 + eps) { return 1.0; }

  return r;
}

inline int roundToInt(double f) {
  return static_cast<int>(std::round(f));
}

#endif