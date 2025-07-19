#include "log.h"

#include "wx/wx.h"

LogWindow::LogWindow(wxWindow* parent, const wxString& title)
    : wxFrame(parent, wxID_ANY, title) {
  const auto parentPos = parent->GetPosition();
  logTextCtrl = new wxTextCtrl(
      this, wxID_ANY, "",
      wxPoint(parentPos.x - LOG_WIDTH, parentPos.y - LOG_HEIGHT),
      wxSize(LOG_WIDTH, LOG_HEIGHT),
      wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH | wxBORDER_NONE);
  logTextCtrl->SetBackgroundColour(wxColour(180, 180, 180));
  logger = new wxLogTextCtrl(logTextCtrl);
  wxLog::SetActiveTarget(logger);
}

LogWindow::~LogWindow() {
  wxLog::SetActiveTarget(nullptr);
  delete logTextCtrl;
  delete logger;
}
