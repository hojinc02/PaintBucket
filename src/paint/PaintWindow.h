#ifndef PAINT_PAINTWINDOW_H_
#define PAINT_PAINTWINDOW_H_

#include "wx/wx.h"

#include "ScaledImageDrawer.h"

struct OffsetRange {
  int minX;
  int maxX;
  int minY;
  int maxY;
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
  void updateRelOffset();
  wxPoint clampOffset(wxPoint offset);

  const int MIN_VISIBLE_PIXELS = FromDIP(60);
  const int MIN_LENGTH = FromDIP(60);
  static constexpr double SCALE_MULT = 1.15;
  static constexpr double INTERPOLATION_THRESHOLD = 5.8;
  static constexpr double MAX_SCALE = 50.0;

  double scale;     // current scale
  double minScale;  // updated on load

  ScaledImageDrawer *drawer = nullptr;

  OffsetRange drawRange;

  wxPoint drawOffset;
  wxPoint lastGrabbedOrigin;
  wxPoint2DDouble relOffset;

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