#include "PaintWindow.h"

#include "wx/affinematrix2d.h"
#include "wx/dcbuffer.h"
#include "wx/graphics.h"
#include "wx/wx.h"

#include "AffineImage.h"
#include "Macros.h"
#include "MathUtils.h"

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
  delete affineDrawer;
}

void PaintWindow::Reset() {
  minScale = 0.;
  drawRange = {0., 1., 0., 1.};
  lastGrabbedOrigin = {0., 0.};
  relativeX = 0.;
  relativeY = 0.;
  shouldDrag = false;
  shouldZoom = false;
  scale = 1.;

  delete affineDrawer;
  affineDrawer = nullptr;
}

void PaintWindow::SetImage(wxImage &img) {
  LOG("SetImage()");

  delete affineDrawer;
  affineDrawer = new AffineImage(img);

  minScale = Ratio(MIN_LENGTH, std::max(img.GetSize().GetX(), img.GetSize().GetY()));

  LOG("minScale: %.4f", minScale);

  updateMinOffset();
}

void PaintWindow::CenterImage() {
  LOG("Center()");
  int winX = GetSize().GetX();
  int winY = GetSize().GetY();
  int imgX = affineDrawer->width;
  int imgY = affineDrawer->height;

  affineDrawer->transform.Translate((winX - imgX) / 2, (winY - imgY) / 2);
}

void PaintWindow::FitImage() {
  LOG("Fit()");
  /*
  int winX = GetSize().GetX();
  int winY = GetSize().GetY();
  int imgX = imageDrawer->GetScaledSize().x;
  int imgY = imageDrawer->GetScaledSize().y;

  double windowRatio = Ratio(winY, winX);
  double imgRatio = Ratio(imgY, imgX);

  if (imgRatio < windowRatio) {
    imageDrawer->ScaleToWidth(winX);
  } else {
    imageDrawer->ScaleToHeight(winY);
  }
    */

  updateMinOffset();
}

void PaintWindow::OnSize(wxSizeEvent &evt) {
  (void)evt;
  updateMaxOffset();

  LOG("OnSize() -- win size:[%d,%d]  x:[%.1f,%.1f]  y:[%.1f,%.1f]  "
      "relPos:[%.3f,%.3f]",
      GetSize().GetX(), GetSize().GetY(), drawRange.minX, drawRange.maxX, drawRange.minY,
      drawRange.maxY, relativeX, relativeY);

  clampToRange();
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

  affineDrawer->Draw(gc);

  delete gc;
}

double PaintWindow::getNewScale(int wheelMovement) {
  double newScale;
  if (wheelMovement > 0) {
    newScale = scale * SCALE_MULT;
    newScale = std::max(newScale, minScale);
  } else {
    newScale = scale / SCALE_MULT;
    newScale = std::min(newScale, MAX_SCALE);
  }
  return newScale;
}

void PaintWindow::OnMouseWheel(wxMouseEvent &evt) {
  (void)evt;
  double newScale = getNewScale(evt.GetWheelRotation());
  wxPoint2DDouble mousePos = evt.GetPosition();

  affineDrawer->transform.Invert();
  wxPoint2DDouble mousePosOrig = affineDrawer->transform.TransformPoint(mousePos);
  wxPoint2DDouble newOffset = mousePos - newScale * mousePosOrig;
  affineDrawer->transform = wxAffineMatrix2D();
  affineDrawer->transform.Translate(newOffset.m_x, newOffset.m_y);
  affineDrawer->transform.Scale(newScale, newScale);

  LOG("OnMouseWheel() -- scale: %.4f -> %.4f", scale, newScale);

  scale = newScale;
  updateMinOffset();

  clampToRange();

  Refresh();
}

void PaintWindow::OnMouseRightDown(wxMouseEvent &evt) {
  shouldDrag = true;
  shouldZoom = false;
  lastGrabbedOrigin = evt.GetPosition();
  CaptureMouse();
  LOG("OnMouseRightDown() -- relPos:[%.3f,%.3f]", relativeX, relativeY);
}

void PaintWindow::OnMouseMove(wxMouseEvent &evt) {
  if (!shouldDrag) { return; }
  wxPoint2DDouble dragVector = evt.GetPosition() - lastGrabbedOrigin;
  if (dragVector == wxPoint2DDouble(0.0, 0.0)) { return; }
  interpolate = false;

  wxAffineMatrix2D inv = affineDrawer->transform;
  inv.Invert();
  dragVector = inv.TransformDistance(dragVector);
  affineDrawer->transform.Translate(dragVector.m_x, dragVector.m_y);

  clampToRange();

  lastGrabbedOrigin = evt.GetPosition();
  Refresh();
}

void PaintWindow::OnMouseRightUp(wxMouseEvent &evt) {
  (void)evt;
  shouldDrag = false;
  shouldZoom = true;

  ReleaseMouse();
  LOG("OnMouseRightUp() -- relPos:[%.3f,%.3f]", relativeX, relativeY);

  if (!interpolate) {
    interpolate = true;
    Refresh();
  }
}

void PaintWindow::clampToRange() {
  auto topLeft = affineDrawer->transform.TransformPoint({0., 0.});
  auto bottomRight =
      affineDrawer->transform.TransformPoint({affineDrawer->width, affineDrawer->height});
  wxPoint2DDouble tr = {0., 0.};
  if (bottomRight.m_x < drawRange.minX) { tr.m_x = drawRange.minX - bottomRight.m_x; }
  if (bottomRight.m_y < drawRange.minY) { tr.m_y = drawRange.minY - bottomRight.m_y; }
  if (topLeft.m_x > drawRange.maxX) { tr.m_x = drawRange.maxX - topLeft.m_x; }
  if (topLeft.m_y > drawRange.maxY) { tr.m_y = drawRange.maxY - topLeft.m_y; }
  tr = affineDrawer->transform.TransformDistance(tr);
  affineDrawer->transform.Translate(tr.m_x, tr.m_y);
}

void PaintWindow::updateMinOffset() {
  auto scaledSize = affineDrawer->GetScaledSize();
  drawRange.minX = MIN_VISIBLE_PIXELS - scaledSize.m_x;
  drawRange.minY = MIN_VISIBLE_PIXELS - scaledSize.m_y;
  logRange(drawRange);
}

void PaintWindow::updateMaxOffset() {
  drawRange.maxX = GetSize().GetX() - MIN_VISIBLE_PIXELS;
  drawRange.maxY = GetSize().GetY() - MIN_VISIBLE_PIXELS;
  logRange(drawRange);
}

void logMatrix(const wxAffineMatrix2D &m, wxString name) {
  wxMatrix2D lin;
  wxPoint2DDouble tr;
  m.Get(&lin, &tr);
  LOG("%s: [[%.2f, %.2f], [%.2f, %.2f]] [%.2f, %.2f]", name, lin.m_11, lin.m_12, lin.m_21,
      lin.m_22, tr.m_x, tr.m_y);
}

void logRange(OffsetRange r) {
  LOG("Offset Range -- x: [%.1f,%.1f]  y: [%.1f,%.1f]", r.minX, r.maxX, r.minY, r.maxY);
}