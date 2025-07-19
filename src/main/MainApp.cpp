#include <iostream>

#include "wx/display.h"
#include "wx/image.h"
#include "wx/wx.h"

#include "MainFrame.h"

#ifdef LOGGING_ENABLED
#include "log.h"
#endif

class MainApp : public wxApp {
 public:
  virtual bool OnInit();
};

wxIMPLEMENT_APP(MainApp);

bool MainApp::OnInit() {
#ifndef LOGGING_ENABLED
  wxLog::EnableLogging(false);
#endif

  wxDisplay *display = new wxDisplay(0u);  // primary display
  const wxRect clientSize = display->GetClientArea();

  wxInitAllImageHandlers();

  MainFrame *frame = new MainFrame("Paint");

  // Position frame to center of client
  const wxSize frameSize = frame->GetSize();
  const int framePosX = (clientSize.width - frameSize.x) / 2;
  const int framePosY = (clientSize.height - frameSize.y) / 2;

  frame->SetPosition(wxPoint(framePosX, framePosY));

  SetTopWindow(frame);
  frame->Show(true);

  return true;
}