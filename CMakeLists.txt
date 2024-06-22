cmake_minimum_required(VERSION 3.16)

project(
  img_from_clip
  VERSION 0.1
  LANGUAGES CXX)

if(APPLE)
  enable_language(OBJCXX)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick Gui Widgets Svg)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appimg_from_clip main.cpp)

configure_file(config.h.in config.h)

unset(PLATFORM_SOURCES)

if(APPLE)
  list(APPEND PLATFORM_SOURCES platformsupport_apple.mm)
else()
  list(APPEND PLATFORM_SOURCES platformsupport_generic.cpp)
endif()

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
                                               Qt6::Svg)
target_include_directories(appimg_from_clip PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

include(GNUInstallDirs)
install(
  TARGETS appimg_from_clip
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
