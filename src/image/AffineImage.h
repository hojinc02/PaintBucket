#ifndef IMAGE_AFFINEIMAGE_H_
#define IMAGE_AFFINEIMAGE_H_

#include "wx/graphics.h"
#include "wx/wx.h"

#include "MathUtils.h"

class AffineImage {
 public:
  wxImage &originalImage;      // original image - no copying
  wxAffineMatrix2D transform;  // define as M = S T

  AffineImage(wxImage &img)
      : originalImage(img),
        width(static_cast<double>(img.GetSize().GetX())),
        height(static_cast<double>(img.GetSize().GetY())),
        ratio(Ratio(height, width)),
        imageBitmap(
            wxGraphicsRenderer::GetDefaultRenderer()->CreateBitmapFromImage(
                img)) {}

  void Draw(wxGraphicsContext *gc) {
    if (gc) {
      gc->SetTransform(gc->CreateMatrix(transform));
      gc->DrawBitmap(imageBitmap, 0, 0, width, height);
    }
  }

  wxPoint2DDouble GetScaledSize() {
    return transform.TransformDistance(
        {static_cast<double>(width), static_cast<double>(height)});
  }

  const double width;
  const double height;
  const double ratio;

 private:
  wxGraphicsBitmap imageBitmap;  // bitmap
};

#endif