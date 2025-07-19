#ifndef UTIL_MACROS_H_
#define UTIL_MACROS_H_

#define LOG(format, ...) wxLogMessage(wxString::Format(format, ##__VA_ARGS__))

#endif