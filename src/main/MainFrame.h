#ifndef MAIN_MAINFRAME_H_
#define MAIN_MAINFRAME_H_

#include <array>
#include <vector>

#include "wx/gbsizer.h"
#include "wx/wx.h"

#include "PaintWindow.h"

struct GBDetails {
  wxGBPosition pos = {-1, -1};
  wxGBSpan span = {-1, -1};
  wxSize size = {-1, -1};
  GBDetails(wxGBPosition p, wxGBSpan s, int ux, int uy, int unitWidth,
            int unitHeight)
      : pos(p), span(s) {
    if (ux > 0) { size.SetWidth(ux * unitWidth); }
    if (uy > 0) { size.SetHeight(uy * unitHeight); }
  }
};

class MainFrame : public wxFrame {
 public:
  MainFrame(const wxString &title);

 private:
  const wxSize sizeM = this->GetTextExtent("M");
  const int MARGIN = sizeM.GetX();
  const int UNIT_WIDTH = sizeM.GetX() * 5;
  const int UNIT_HEIGHT = sizeM.GetY() * 4;
  const int MAIN_WIDTH = UNIT_WIDTH * 12;
  const int MAIN_HEIGHT = UNIT_HEIGHT * 7;
  const int BORDER_SIZE = sizeM.GetX() / 3;

  static constexpr double INITTEXT_FONT_SCALE = 2.0;
  static const inline wxColour INITTEXT_COLOR = wxColour(250u, 250u, 250u);
  static const inline wxColour BG_COLOR = wxColour(180u, 180u, 180u);
  static const inline wxColour PANEL_COLOR = wxColour(80u, 80u, 80u);

  wxPanel *mainPanel;
  wxBoxSizer *mainSizer;
  wxGridBagSizer *bagSizer;

  const GBDetails pwGBDetails =
      GBDetails({0, 2}, {3, 1}, -1, -1, UNIT_WIDTH, UNIT_HEIGHT);
  const std::array<GBDetails, 6> GB_ITEMS = {
      GBDetails({0, 0}, {1, 2}, 2, 1, UNIT_WIDTH, UNIT_HEIGHT),
      GBDetails({1, 0}, {1, 1}, 1, 1, UNIT_WIDTH, UNIT_HEIGHT),
      GBDetails({1, 1}, {1, 1}, 1, 1, UNIT_WIDTH, UNIT_HEIGHT),
      GBDetails({2, 0}, {1, 2}, 2, -1, UNIT_WIDTH, UNIT_HEIGHT),
      GBDetails({3, 0}, {1, 3}, -1, 1, UNIT_WIDTH, UNIT_HEIGHT),
      pwGBDetails};

  static constexpr std::array<std::pair<int, int>, 1> GROWABLE_ROWS = {
      {{2, 1}}};
  static constexpr std::array<std::pair<int, int>, 1> GROWABLE_COLS = {
      {{2, 1}}};

  wxImage img;
  wxWindow *paintWindow = nullptr;

  void OnCenterButton(wxCommandEvent &evt);
  void OnFitButton(wxCommandEvent &evt);
  void OnLoadButton(wxCommandEvent &evt);

  // Helper function for adding button to panels
  wxButton *addButton(const wxGBPosition &pos, const wxString &name) {
    auto win = bagSizer->FindItemAtPosition(pos)->GetWindow();
    wxButton *button =
        new wxButton(win, wxID_ANY, name, wxDefaultPosition, wxDefaultSize);
    centerItem(button, win, wxEXPAND | wxALL, sizeM.GetX() / 3);
    return button;
  }

  // Helper function for centering items to panels
  static void centerItem(wxWindow *item, wxWindow *panel, int flags = 0,
                         int margin = 0) {
    auto xSizer = new wxBoxSizer(wxHORIZONTAL);
    auto ySizer = new wxBoxSizer(wxVERTICAL);
    xSizer->Add(item, 1, flags, margin);
    ySizer->Add(xSizer, 1, flags, margin);
    panel->SetSizerAndFit(ySizer);
  }
};

#endif