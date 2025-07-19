#ifndef IMAGE_SCALEDIMAGEDRAWER_H_
#define IMAGE_SCALEDIMAGEDRAWER_H_

#include "wx/graphics.h"
#include "wx/wx.h"

class ScaledImageDrawer {
 public:
  const wxSize originalSize;
  ScaledImageDrawer(const wxImage &img, wxGraphicsRenderer *renderer);
  void DrawAt(wxGraphicsContext *gc, wxPoint p);
  void Scale(double scale);
  void ScaleToWidth(int width);
  void ScaleToHeight(int height);
  wxSize GetScaledSize();

 private:
  wxSize scaledSize;
  const double ratio;
  wxGraphicsBitmap imageBitmap;  // bitmap
};

#endif