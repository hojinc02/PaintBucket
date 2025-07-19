# Library linking rules for wxWidgets
find_package(wxWidgets COMPONENTS core base REQUIRED)

include(${wxWidgets_USE_FILE})

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
  set(wxWidgets_CONFIGURATION mswud)
elseif(${CMAKE_BUILD_TYPE} STREQUAL Release)
  set(wxWidgets_CONFIGURATION mswu)
endif()