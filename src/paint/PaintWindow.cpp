#include "PaintWindow.h"

#include "wx/dcbuffer.h"
#include "wx/graphics.h"
#include "wx/wx.h"

#include "Macros.h"
#include "MathUtils.h"
#include "ScaledImageDrawer.h"

void logMatrix(wxAffineMatrix2D);
void logRange(OffsetRange);

PaintWindow::PaintWindow(wxWindow *parent, const wxSize size)
    : wxWindow(parent, wxID_ANY, wxDefaultPosition, size, wxEXPAND) {
  SetBackgroundStyle(wxBG_STYLE_PAINT);
  SetBackgroundColour(parent->GetBackgroundColour());

  Bind(wxEVT_PAINT, &PaintWindow::OnPaint, this);
  Bind(wxEVT_RIGHT_DOWN, &PaintWindow::OnMouseRightDown, this);
  Bind(wxEVT_MOTION, &PaintWindow::OnMouseMove, this);
  Bind(wxEVT_RIGHT_UP, &PaintWindow::OnMouseRightUp, this);
  Bind(wxEVT_MOUSEWHEEL, &PaintWindow::OnMouseWheel, this);
  Bind(wxEVT_SIZE, &PaintWindow::OnSize, this);

  Reset();
}

PaintWindow::~PaintWindow() {
  delete drawer;
}

void PaintWindow::Reset() {
  minScale = 0.;
  drawRange = {0, 1, 0, 1};
  lastGrabbedOrigin = {0, 0};
  relOffset.m_x = 0.;
  relOffset.m_y = 0.;
  shouldDrag = false;
  shouldZoom = false;
  interpolate = true;
  scale = 1.;

  delete drawer;
  drawer = nullptr;
}

void PaintWindow::SetImage(wxImage &img) {
  LOG("SetImage()");

  delete drawer;
  drawer = new ScaledImageDrawer(img, wxGraphicsRenderer::GetDefaultRenderer());

  auto imgSize = img.GetSize();
  int longerLength = std::max(imgSize.GetX(), imgSize.GetY());
  minScale = Ratio(MIN_LENGTH, longerLength);

  LOG("minScale: %.4f", minScale);

  updateMinOffset();
}

void PaintWindow::CenterImage() {
  LOG("Center()");
  int winX = GetSize().GetX();
  int winY = GetSize().GetY();
  int imgX = drawer->GetScaledSize().x;
  int imgY = drawer->GetScaledSize().y;

  drawOffset = {(winX - imgX) / 2, (winY - imgY) / 2};
  updateRelOffset();

  Refresh();
}

void PaintWindow::FitImage() {
  int winX = GetSize().GetX();
  int winY = GetSize().GetY();
  int imgX = drawer->GetScaledSize().x;
  int imgY = drawer->GetScaledSize().y;

  double windowRatio = Ratio(winY, winX);
  double imgRatio = Ratio(imgY, imgX);

  bool fitByWidth = imgRatio < windowRatio;
  double fitScale = fitByWidth ? Ratio(winX, drawer->originalSize.GetX())
                               : Ratio(winY, drawer->originalSize.GetY());

  if (fitScale > MAX_SCALE) {
    drawer->Scale(MAX_SCALE);
    scale = MAX_SCALE;
  } else if (fitByWidth) {
    drawer->ScaleToWidth(winX);
    scale = fitScale;
  } else {
    drawer->ScaleToHeight(winY);
    scale = fitScale;
  }

  LOG("Fit() -- scale: %.3f", scale);

  updateMinOffset();

  Refresh();
}

void PaintWindow::OnSize(wxSizeEvent &evt) {
  (void)evt;
  updateMaxOffset();

  int xRange = drawRange.maxX - drawRange.minX;
  int yRange = drawRange.maxY - drawRange.minY;
  drawOffset =
      clampOffset({roundToInt(relOffset.m_x * xRange) + drawRange.minX,
                   roundToInt(relOffset.m_y * yRange) + drawRange.minY});

  LOG("OnSize() -- win size:[%d,%d]  x:[%d,%d]  y:[%d,%d]  relPos:[%.3f,%.3f]",
      GetSize().GetX(), GetSize().GetY(), drawRange.minX, drawRange.maxX,
      drawRange.minY, drawRange.maxY, relOffset.m_x, relOffset.m_y);

  Refresh();
}

void PaintWindow::OnPaint(wxPaintEvent &evt) {
  (void)evt;
  wxAutoBufferedPaintDC dc(this);
  dc.Clear();
  wxGraphicsContext *gc = wxGraphicsContext::Create(dc);

  if (!interpolate || scale > INTERPOLATION_THRESHOLD) {
    gc->SetInterpolationQuality(wxINTERPOLATION_NONE);
  } else {
    gc->SetInterpolationQuality(wxINTERPOLATION_DEFAULT);
  }

  drawer->DrawAt(gc, drawOffset);

  delete gc;
}

void PaintWindow::OnMouseWheel(wxMouseEvent &evt) {
  (void)evt;
  wxPoint mousePos = evt.GetPosition();
  bool zoomingIn = evt.GetWheelRotation() > 0;
  double newScale = zoomingIn ? (scale * SCALE_MULT) : (scale / SCALE_MULT);
  newScale = std::clamp(newScale, minScale, MAX_SCALE);
  LOG("OnMouseWheel() -- scale: %.4f -> %.4f", scale, newScale);

  wxPoint newOffset = mousePos - newScale / scale * (mousePos - drawOffset);
  drawOffset = clampOffset(newOffset);
  updateRelOffset();

  scale = newScale;
  drawer->Scale(scale);
  updateMinOffset();

  Refresh();
}

void PaintWindow::OnMouseMove(wxMouseEvent &evt) {
  if (!shouldDrag) { return; }
  wxPoint dragVector = evt.GetPosition() - lastGrabbedOrigin;
  if (dragVector == wxPoint(0, 0)) { return; }
  interpolate = false;

  drawOffset = clampOffset(drawOffset + dragVector);
  updateRelOffset();

  lastGrabbedOrigin = evt.GetPosition();
  Refresh();
}

void PaintWindow::OnMouseRightDown(wxMouseEvent &evt) {
  shouldDrag = true;
  shouldZoom = false;
  lastGrabbedOrigin = evt.GetPosition();
  CaptureMouse();
  LOG("OnMouseRightDown() -- relPos:[%.3f,%.3f]", relOffset.m_x, relOffset.m_y);
}

void PaintWindow::OnMouseRightUp(wxMouseEvent &evt) {
  (void)evt;
  shouldDrag = false;
  shouldZoom = true;

  ReleaseMouse();
  LOG("OnMouseRightUp() -- relPos:[%.3f,%.3f]", relOffset.m_x, relOffset.m_y);

  if (!interpolate) {
    interpolate = true;
    Refresh();
  }
}

wxPoint PaintWindow::clampOffset(wxPoint offset) {
  return {std::min(drawRange.maxX, std::max(drawRange.minX, offset.x)),
          std::min(drawRange.maxY, std::max(drawRange.minY, offset.y))};
}

void PaintWindow::updateMinOffset() {
  auto scaledSize = drawer->GetScaledSize();
  drawRange.minX = MIN_VISIBLE_PIXELS - scaledSize.GetX();
  drawRange.minY = MIN_VISIBLE_PIXELS - scaledSize.GetY();
  logRange(drawRange);
}

void PaintWindow::updateMaxOffset() {
  auto winSize = this->GetSize();
  drawRange.maxX = winSize.GetX() - MIN_VISIBLE_PIXELS;
  drawRange.maxY = winSize.GetY() - MIN_VISIBLE_PIXELS;
  logRange(drawRange);
}

void PaintWindow::updateRelOffset() {
  double relX =
      Ratio(drawOffset.x - drawRange.minX, drawRange.maxX - drawRange.minX);
  double relY =
      Ratio(drawOffset.y - drawRange.minY, drawRange.maxY - drawRange.minY);
  relOffset = {relX, relY};
}

void logRange(OffsetRange r) {
  LOG("Offset Range -- x: [%d,%d]  y: [%d,%d]", r.minX, r.maxX, r.minY, r.maxY);
}