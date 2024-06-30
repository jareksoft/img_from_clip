cmake_minimum_required(VERSION 3.21)

project(
  img_from_clip
  VERSION 0.6
  LANGUAGES CXX C)

if(APPLE)
  enable_language(OBJCXX)
endif()

include(CheckCXXCompilerFlag)
include(cmake/CPM.cmake)

set(APP_ID com.jpelczar.img_from_clip)

if(APPLE OR WIN32)
  set(APP_NAME ImgFromClip)
else()
  set(APP_NAME appimg_from_clip)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_OSX_DEPLOYMENT_TARGET 12)

option(FLATPAK_DIST "Enable flatpak specific options" OFF)
option(SNAP_DIST "Enable snap specific options" OFF)
option(OSX_BUNDLE "Use app bundle on macOS" ON)
option(ENABLE_LUA "Enable Lua based scripting of the app" ON)

find_package(
  Qt6 6.5 REQUIRED
  COMPONENTS Quick
             Gui
             Widgets
             Svg
             LinguistTools
             Concurrent
             QuickControls2)

qt_standard_project_setup(REQUIRES 6.5)
qt_add_executable(${APP_NAME} main.cpp)

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

checkandaddoption(-Wall HAVE_WALL)
checkandaddoption(-Werror HAVE_WERROR)
checkandaddoption(-Wextra HAVE_WEXTRA)
checkandaddoption(-Wconversion HAVE_WCONVERSION)

if(WIN32)
  list(APPEND PLATFORM_SOURCES
       ${CMAKE_CURRENT_SOURCE_DIR}/shared/windows/icon.rc)
endif()

find_package(PkgConfig)
find_package(Lua)

if(NOT LUA_FOUND)
  if(EXISTS external/lua/onelua.c)
    set(lua_sources external/lua/onelua.c)
  else()
    cpmaddpackage(
      NAME
      lua
      GIT_REPOSITORY
      https://github.com/lua/lua.git
      VERSION
      5.4
      DOWNLOAD_ONLY
      YES)

    file(GLOB lua_sources ${lua_SOURCE_DIR}/onelua.c)
  endif()

  add_library(lua_import STATIC ${lua_sources})
  target_include_directories(lua_import SYSTEM
                             PUBLIC $<BUILD_INTERFACE:${lua_SOURCE_DIR}>)
  target_compile_definitions(lua_import PRIVATE MAKE_LIB=1)
  set(LUA_FOUND ON)
else()
  add_library(lua_import INTERFACE)
  target_link_libraries(lua_import INTERFACE ${LUA_LIBRARIES})
  target_include_directories(lua_import INTERFACE ${LUA_INCLUDE_DIR})
endif()

if(LUA_FOUND)
  find_package(sol2)

  if(NOT sol2_FOUND)
    if(EXISTS external/sol2/CMakeLists.txt)
      cpmaddpackage(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/sol2
                    VERSION 3.3.1 OPTIONS "SOL2_BUILD_LUA=OFF")
    else()
      cpmaddpackage(GITHUB_REPOSITORY ThePhD/sol2 VERSION 3.3.1 OPTIONS
                    "SOL2_BUILD_LUA=OFF")
    endif()
  endif()
endif()

if(PKG_CONFIG_FOUND)
  pkg_check_modules(RSVG IMPORTED_TARGET librsvg-2.0)
  pkg_check_modules(CAIRO IMPORTED_TARGET cairo)
  pkg_check_modules(GDK_PIXBUF IMPORTED_TARGET gdk-pixbuf-2.0)
  if(RSVG_FOUND
     AND CAIRO_FOUND
     AND GDK_PIXBUF_FOUND)
    set(USE_LIBRSVG_RENDERER ON)
  endif()
endif()

configure_file(config.h.in config.h)

qt_add_qml_module(
  ${APP_NAME}
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
  ${PLATFORM_SOURCES}
  SOURCES
  clipboardsource.h
  clipboardsource.cpp
  SOURCES
  clipboardcontents.h
  clipboardcontents.cpp
  SOURCES
  rsvg_helper.h
  SOURCES
  rsvg_helper.cpp
  QML_FILES
  StorePreferences.qml
  SOURCES
  renderconfiguration.h
  renderconfiguration.cpp)

if(APPLE)
  target_compile_options(${APP_NAME}
                         PRIVATE $<$<COMPILE_LANGUAGE:OBJCXX>:-fobjc-arc>)
  target_link_libraries(${APP_NAME} PRIVATE "-framework
  UserNotifications")
  set_target_properties(
    ${APP_NAME}
    PROPERTIES XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
               MACOSX_BUNDLE_BUNDLE_NAME "ImgFromClip"
               MACOSX_BUNDLE_BUNDLE_VERSION
               "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
               MACOSX_BUNDLE_SHORT_VERSION_STRING
               "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
               XCODE_ATTRIBUTE_STRIP_INSTALLED_PRODUCT YES
               XCODE_ATTRIBUTE_STRIP_STYLE YES
               MACOSX_BUNDLE_ICON_FILE ${APP_NAME})
  if(NOT IOS)
    find_program(SIPS sips REQUIRED)
    find_program(ICONUTIL iconutil REQUIRED)

    file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}.iconset)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}.iconset)

    foreach(img_size 16 32 64 128 256 512)
      math(EXPR size_2 "${img_size}*2")

      execute_process(
        COMMAND
          ${SIPS} -z ${img_size} ${img_size}
          ${CMAKE_CURRENT_SOURCE_DIR}/appicon512.png -s format png --out
          ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}.iconset/icon_${img_size}x${img_size}.png
          COMMAND_ERROR_IS_FATAL ANY)

      execute_process(
        COMMAND
          ${SIPS} -z ${size_2} ${size_2}
          ${CMAKE_CURRENT_SOURCE_DIR}/appicon512.png -s format png --out
          ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}.iconset/icon_${size_2}x${size_2}@2x.png
          COMMAND_ERROR_IS_FATAL ANY)
    endforeach()

    execute_process(
      COMMAND ${ICONUTIL} -c icns ${APP_NAME}.iconset
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMAND_ERROR_IS_FATAL ANY)

    target_sources(${APP_NAME}
                   PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}.icns)
    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}.icns
                                PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set_target_properties(
      ${APP_NAME} PROPERTIES INSTALL_RPATH "@executable_path/../Frameworks")

    set(CMAKE_MACOSX_RPATH TRUE)
  endif()
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1. If
# you are developing for iOS or macOS you should consider setting an explicit,
# fixed bundle identifier manually though.
set_target_properties(
  ${APP_NAME}
  PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_ID}
             MACOSX_BUNDLE TRUE
             WIN32_EXECUTABLE TRUE)

target_link_libraries(
  ${APP_NAME}
  PRIVATE Qt6::Quick
          Qt6::Gui
          Qt6::Widgets
          Qt6::Svg
          Qt6::Concurrent
          compile_options_if
          Qt6::QuickControls2)

if(USE_DBUS_BACKEND)
  target_link_libraries(${APP_NAME} PRIVATE Qt6::DBus)
endif()

if(USE_LIBRSVG_RENDERER)
  target_link_libraries(${APP_NAME} PRIVATE PkgConfig::RSVG PkgConfig::CAIRO
                                            PkgConfig::GDK_PIXBUF)
endif()

target_include_directories(${APP_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(CMAKE_GENERATOR MATCHES "Xcode")
  target_link_options(${APP_NAME} PRIVATE -s)
endif()

qt_add_translations(${APP_NAME} TS_FILES appimg_from_clip_en.ts
                    appimg_from_clip_pl.ts appimg_from_clip_ko.ts)

include(GNUInstallDirs)

add_subdirectory(shared)

install(
  TARGETS ${APP_NAME}
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(APPLE
   AND OSX_BUNDLE
   AND NOT IOS)
  set(CPACK_GENERATOR "DragNDrop")
  set(CPACK_DMG_FORMAT "UDBZ")
  set(CPACK_DMG_VOLUME_NAME "ImgFromClip")
  set(CPACK_SYSTEM_NAME "OSX")
  set(CPACK_STRIP_FILES ON)
  set(CPACK_PACKAGE_FILE_NAME
      "${APP_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
  ")
  include(CPack)
endif()
