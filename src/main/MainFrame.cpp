#include "MainFrame.h"

#include "wx/gbsizer.h"
#include "wx/wx.h"

#include "Macros.h"
#include "PaintWindow.h"

#ifdef LOGGING_ENABLED
#include "log.h"
#endif

MainFrame::MainFrame(const wxString &title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize) {

#ifdef LOGGING_ENABLED
  LogWindow *logWindow = new LogWindow(this, "Log");
  LOG("Logging frame attached to main frame");
  logWindow->Show(true);
#endif

  mainPanel = new wxPanel(this, wxID_ANY);
  mainSizer = new wxBoxSizer(wxVERTICAL);
  bagSizer = new wxGridBagSizer(MARGIN, MARGIN);

  this->SetBackgroundColour(BG_COLOR);
  mainPanel->SetBackgroundColour(this->GetBackgroundColour());

  for (const auto &details : GB_ITEMS) {
    auto panel =
        new wxPanel(mainPanel, wxID_ANY, wxDefaultPosition, details.size);
    panel->SetBackgroundColour(PANEL_COLOR);
    bagSizer->Add(panel, details.pos, details.span, wxEXPAND);
  }

  for (const auto &item : GROWABLE_ROWS) {
    bagSizer->AddGrowableRow(item.first, item.second);
  }
  for (const auto &item : GROWABLE_COLS) {
    bagSizer->AddGrowableCol(item.first, item.second);
  }

  bagSizer->SetMinSize({MAIN_WIDTH, MAIN_HEIGHT});

  paintWindow = bagSizer->FindItemAtPosition(pwGBDetails.pos)->GetWindow();
  // Initial text box window to show instead of paint window
  auto initText = new wxStaticText(paintWindow, wxID_ANY, "Open an image",
                                   wxDefaultPosition, wxDefaultSize,
                                   wxTE_READONLY | wxTE_CENTER | wxBORDER_NONE);
  initText->SetBackgroundColour(PANEL_COLOR);
  initText->SetForegroundColour(INITTEXT_COLOR);
  initText->SetFont(initText->GetFont().Scale(INITTEXT_FONT_SCALE));
  centerItem(initText, paintWindow, wxALIGN_CENTER);

  mainPanel->SetSizer(bagSizer);
  mainSizer->Add(mainPanel, 1, wxEXPAND | wxALL, MARGIN);
  this->SetSizerAndFit(mainSizer);

  auto loadButton = addButton({0, 0}, "Load");
  loadButton->Bind(wxEVT_BUTTON, &MainFrame::OnLoadButton, this);

  auto centerButton = addButton({1, 0}, "Center");
  centerButton->Bind(wxEVT_BUTTON, &MainFrame::OnCenterButton, this);

  auto fitButton = addButton({1, 1}, "Fit");
  fitButton->Bind(wxEVT_BUTTON, &MainFrame::OnFitButton, this);
}

void MainFrame::OnLoadButton(wxCommandEvent &evt) {
  (void)evt;
  LOG("load");
  wxFileDialog dialog =
      wxFileDialog(this, "Open Image", "", "",
                   "Image File Types "
                   "(*.png;*.jpg;*.jpeg;*.bmp;*.tiff;*.tga)|*.png;*.jpg;*.jpeg;"
                   "*.bmp;*.tiff;*.tga",
                   wxFD_OPEN | wxFD_FILE_MUST_EXIST);

  if (dialog.ShowModal() == wxID_CANCEL) { return; }

  if (!img.LoadFile(dialog.GetPath())) {
    wxMessageBox("Image load failed", "Error", wxOK | wxICON_ERROR);
    return;
  }

  PaintWindow *pw = nullptr;
  if (!(pw = dynamic_cast<PaintWindow *>(paintWindow))) {
    bagSizer->Detach(paintWindow);
    paintWindow->Destroy();
    paintWindow = new PaintWindow(mainPanel);
    bagSizer->Add(paintWindow, pwGBDetails.pos, pwGBDetails.span, wxEXPAND);
  }

  pw = dynamic_cast<PaintWindow *>(paintWindow);
  pw->Reset();
  pw->SetImage(img);

  mainPanel->Layout();

  pw->FitImage();
  pw->CenterImage();
  pw->Refresh();
}

void MainFrame::OnCenterButton(wxCommandEvent &evt) {
  (void)evt;
  LOG("center");
  if (auto win = dynamic_cast<PaintWindow *>(paintWindow)) {
    win->CenterImage();
    win->Refresh();
  }
}

void MainFrame::OnFitButton(wxCommandEvent &evt) {
  (void)evt;
  LOG("fit");
  if (auto win = dynamic_cast<PaintWindow *>(paintWindow)) {
    win->FitImage();
    win->CenterImage();
    win->Refresh();
  }
}