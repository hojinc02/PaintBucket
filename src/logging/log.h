#ifndef LOGGING_LOG_H_
#define LOGGING_LOG_H_

#include "wx/wx.h"

class LogWindow : public wxFrame {
 private:
  wxTextCtrl* logTextCtrl;
  wxLog* logger;
  const int LOG_WIDTH = FromDIP(600);
  const int LOG_HEIGHT = FromDIP(300);

 public:
  LogWindow(wxWindow* parent, const wxString& title);
  ~LogWindow();
};

#endif