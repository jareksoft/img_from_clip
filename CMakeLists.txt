cmake_minimum_required(VERSION 3.21)

project(
  img_from_clip
  VERSION 0.1
  LANGUAGES CXX)

if(APPLE)
  enable_language(OBJCXX)
endif()

include(CheckCXXCompilerFlag)

set(APP_ID com.jpelczar.img_from_clip)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

option(FLATPAK_DIST "Enable flatpak specific options" OFF)
option(SNAP_DIST "Enable snap specific options" OFF)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick Gui Widgets Svg LinguistTools)

qt_standard_project_setup(REQUIRES 6.5)
qt_add_executable(appimg_from_clip main.cpp)

unset(PLATFORM_SOURCES)

if(APPLE)
  list(APPEND PLATFORM_SOURCES platformsupport_apple.mm)
else()
  if(UNIX
     AND NOT APPLE
     AND NOT ANDROID)
    find_package(Qt6 COMPONENTS DBus)
    if(TARGET Qt6::DBus)
      set(USE_DBUS_BACKEND ON)
    endif()
  endif()

  if(USE_DBUS_BACKEND)
    list(APPEND PLATFORM_SOURCES platformsupport_dbus.cpp)
  else()
    list(APPEND PLATFORM_SOURCES platformsupport_generic.cpp)
  endif()
endif()

macro(CheckAndAddOption Flag Var)
  check_cxx_compiler_flag(${Flag} ${Var})
  if(${Var})
    target_compile_options(compile_options_if
                           INTERFACE $<$<COMPILE_LANGUAGE:CXX>:${Flag}>)
  endif()
endmacro()

add_library(compile_options_if INTERFACE)

configure_file(config.h.in config.h)

checkandaddoption(-Wall HAVE_WALL)
checkandaddoption(-Werror HAVE_WERROR)
checkandaddoption(-Wextra HAVE_WEXTRA)
checkandaddoption(-Wconversion HAVE_WCONVERSION)

qt_add_qml_module(
  appimg_from_clip
  URI
  img_from_clip
  VERSION
  1.0
  QML_FILES
  Main.qml
  SOURCES
  clipmonitor.h
  clipmonitor.cpp
  systray.h
  systray.cpp
  RESOURCES
  appicon512.png
  appicon256.png
  appicon128.png
  appicon64.png
  appicon24.png
  SOURCES
  platformsupport.h
  platformsupport.cpp
  ${PLATFORM_SOURCES})

if(APPLE)
  target_compile_options(appimg_from_clip
                         PRIVATE $<$<COMPILE_LANGUAGE:OBJCXX>:-fobjc-arc>)
  target_link_libraries(appimg_from_clip PRIVATE "-framework UserNotifications")
  set_target_properties(appimg_from_clip
                        PROPERTIES XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES)
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1. If
# you are developing for iOS or macOS you should consider setting an explicit,
# fixed bundle identifier manually though.
set_target_properties(
  appimg_from_clip
  PROPERTIES # MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appimg_from_clip
             MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
             MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
             MACOSX_BUNDLE TRUE
             WIN32_EXECUTABLE TRUE)

target_link_libraries(appimg_from_clip PRIVATE Qt6::Quick Qt6::Gui Qt6::Widgets
                                               Qt6::Svg compile_options_if)

if(USE_DBUS_BACKEND)
  target_link_libraries(appimg_from_clip PRIVATE Qt6::DBus)
endif()

target_include_directories(appimg_from_clip PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

qt_add_translations(appimg_from_clip TS_FILES appimg_from_clip_en.ts
                    appimg_from_clip_pl.ts)

include(GNUInstallDirs)

add_subdirectory(shared)

install(
  TARGETS appimg_from_clip
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
