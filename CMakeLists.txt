cmake_minimum_required(VERSION 3.16...3.25)
project(obs-browser LANGUAGES C CXX)

if(NOT DEFINED OS_WINDOWS)
  set(OS_WINDOWS OFF)
endif()
if(NOT DEFINED OS_MACOS)
  set(OS_MACOS OFF)
endif()
if(NOT DEFINED OS_LINUX)
  set(OS_LINUX OFF)
endif()

if(WIN32)
  set(OS_WINDOWS ON)
elseif(APPLE)
  set(OS_MACOS ON)
elseif(UNIX)
  set(OS_LINUX ON)
endif()

option(ENABLE_BROWSER "Enable browser source plugin (required Chromium Embedded Framework)" OFF)

if(NOT ENABLE_BROWSER)
  target_disable(obs-browser)
  target_disable_feature(obs-browser "Browser sources are not enabled by default (set CEF_ROOT_DIR and ENABLE_BROWSER)")

  return()
endif()

find_package(CEF 95 REQUIRED)
find_package(nlohmann_json 3.11 REQUIRED)
find_package(PkgConfig QUIET)

add_library(obs-browser MODULE)
add_library(OBS::browser ALIAS obs-browser)

option(ENABLE_BROWSER_PANELS "Enable Qt web browser panel support" ON)
mark_as_advanced(ENABLE_BROWSER_PANELS)

target_sources(
  obs-browser
  PRIVATE # cmake-format: sortable
          browser-app.cpp
          browser-app.hpp
          browser-client.cpp
          browser-client.hpp
          browser-scheme.cpp
          browser-scheme.hpp
          browser-version.h
          cef-headers.hpp
          deps/base64/base64.cpp
          deps/base64/base64.hpp
          deps/ip-string.hpp
          deps/signal-restore.cpp
          deps/signal-restore.hpp
          deps/wide-string.cpp
          deps/wide-string.hpp
          obs-browser-plugin.cpp
          obs-browser-source.cpp
          obs-browser-source.hpp)

target_include_directories(obs-browser PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/deps")
if(EXISTS "/usr/include/cef")
  target_include_directories(obs-browser PRIVATE "/usr/include/cef")
endif()

target_compile_features(obs-browser PRIVATE cxx_std_20)

set(OBS_BROWSER_LIBS nlohmann_json::nlohmann_json)
if(TARGET OBS::libobs AND TARGET OBS::frontend-api)
  list(APPEND OBS_BROWSER_LIBS OBS::libobs OBS::frontend-api)
  if(TARGET OBS::websocket-api)
    list(APPEND OBS_BROWSER_LIBS OBS::websocket-api)
  endif()
elseif(PkgConfig_FOUND)
  pkg_check_modules(LIBOBS REQUIRED libobs)
  pkg_check_modules(OBS_FRONTEND REQUIRED obs-frontend-api)

  target_include_directories(obs-browser PRIVATE ${LIBOBS_INCLUDE_DIRS} ${OBS_FRONTEND_INCLUDE_DIRS})
  if(EXISTS "/usr/include/obs")
    target_include_directories(obs-browser PRIVATE "/usr/include/obs")
  endif()
  target_link_directories(obs-browser PRIVATE ${LIBOBS_LIBRARY_DIRS} ${OBS_FRONTEND_LIBRARY_DIRS})
  list(APPEND OBS_BROWSER_LIBS ${LIBOBS_LIBRARIES} ${OBS_FRONTEND_LIBRARIES})
else()
  message(FATAL_ERROR "Missing OBS CMake targets and pkg-config fallback is unavailable")
endif()

target_link_libraries(obs-browser PRIVATE ${OBS_BROWSER_LIBS})

if(OS_WINDOWS)
  include(cmake/os-windows.cmake)
elseif(OS_MACOS)
  include(cmake/os-macos.cmake)
elseif(OS_LINUX)
  include(cmake/os-linux.cmake)
endif()

if(ENABLE_BROWSER_PANELS)
  include(cmake/feature-panels.cmake)
endif()

if(COMMAND set_target_properties_obs)
  set_target_properties_obs(obs-browser PROPERTIES FOLDER plugins/obs-browser PREFIX "")
else()
  set_target_properties(obs-browser PROPERTIES PREFIX "")
endif()
