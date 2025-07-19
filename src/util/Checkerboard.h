#ifndef UTIL_CHECKERBOARD_H_
#define UTIL_CHECKERBOARD_H_

#include "wx/dcbuffer.h"
#include "wx/graphics.h"
#include "wx/wx.h"

class Checkerboard {
 public:
  Checkerboard(const int& rowCount, const int& colCount, const double& size)
      : ROW_COUNT(rowCount), COL_COUNT(colCount), DEFAULT_TILE_SIZE(size) {}

  void Draw(wxGraphicsContext* gc, const int& x, const int& y) {
    if (gc) {
      double ts = getTileSize();
      for (int r = 0; r < ROW_COUNT; r++) {
        for (int c = 0; c < COL_COUNT; c++) {
          if (r % 2 == c % 2) {
            gc->SetBrush(*wxWHITE);
          } else {
            gc->SetBrush(*wxBLACK);
          }
          gc->DrawRectangle(x + c * ts, y + r * ts, ts, ts);
        }
      }
    }
  }

  double getTileSize() { return DEFAULT_TILE_SIZE * scale; }

  wxSize getSize() {
    double ts = getTileSize();
    return wxSize(COL_COUNT * ts, ROW_COUNT * ts);
  }

  void setScale(double newScale) { scale = newScale; }

  double getScale() { return scale; }

 private:
  const int ROW_COUNT, COL_COUNT;
  double DEFAULT_TILE_SIZE;
  double scale = 1.0;
};

#endif