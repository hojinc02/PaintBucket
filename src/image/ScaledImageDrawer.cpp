#include "ScaledImageDrawer.h"

#include "wx/graphics.h"
#include "wx/wx.h"

#include "MathUtils.h"

ScaledImageDrawer::ScaledImageDrawer(const wxImage &img,
                                     wxGraphicsRenderer *renderer)
    : originalSize(img.GetSize()),
      scaledSize(img.GetSize()),
      ratio(Ratio(originalSize.GetY(), originalSize.GetX())),
      imageBitmap(renderer->CreateBitmapFromImage(img)) {}

void ScaledImageDrawer::DrawAt(wxGraphicsContext *gc, wxPoint p) {
  if (gc) {
    gc->DrawBitmap(imageBitmap, p.x, p.y, scaledSize.GetX(), scaledSize.GetY());
  }
}

void ScaledImageDrawer::Scale(double scale) {
  scaledSize = scale * originalSize;
}

void ScaledImageDrawer::ScaleToWidth(int width) {
  scaledSize = {width, roundToInt(ratio * width)};
}

void ScaledImageDrawer::ScaleToHeight(int height) {
  scaledSize = {roundToInt(height / ratio), height};
}

wxSize ScaledImageDrawer::GetScaledSize() {
  return scaledSize;
}