#ifndef PAINT_PAINTWINDOW_H_
#define PAINT_PAINTWINDOW_H_

#include "wx/wx.h"

#include "AffineImage.h"

struct OffsetRange {
  double minX;
  double maxX;
  double minY;
  double maxY;
};

class PaintWindow : public wxWindow {
 private:
  void OnPaint(wxPaintEvent &evt);
  void OnMouseRightDown(wxMouseEvent &evt);
  void OnMouseMove(wxMouseEvent &evt);
  void OnMouseRightUp(wxMouseEvent &evt);
  void OnMouseWheel(wxMouseEvent &evt);
  void OnSize(wxSizeEvent &evt);

  void updateMinOffset();
  void updateMaxOffset();
  void clampToRange();
  double getNewScale(int wheelMovement);

  const int MIN_VISIBLE_PIXELS = FromDIP(60);
  const int MIN_LENGTH = FromDIP(60);
  static constexpr double MAX_SCALE =
      40.0;  // scale is scaledLength:originalLength
  static constexpr double SCALE_MULT = 1.15;
  static constexpr double INTERPOLATION_THRESHOLD = 5.8;

  double scale;     // current scale
  double minScale;  // updated on load

  AffineImage *affineDrawer = nullptr;

  OffsetRange drawRange;

  wxPoint2DDouble lastGrabbedOrigin;
  double relativeX;
  double relativeY;
  bool interpolate;
  bool shouldDrag;
  bool shouldZoom;

 public:
  PaintWindow(wxWindow *parent, const wxSize size = wxDefaultSize);
  ~PaintWindow();
  void Reset();
  void SetImage(wxImage &img);
  void FitImage();
  void CenterImage();
};

#endif