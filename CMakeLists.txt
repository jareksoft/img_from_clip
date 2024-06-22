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
set(CMAKE_OSX_DEPLOYMENT_TARGET 12)

option(FLATPAK_DIST "Enable flatpak specific options" OFF)
option(SNAP_DIST "Enable snap specific options" OFF)
option(OSX_BUNDLE "Use app bundle on macOS" ON)

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
                        PROPERTIES XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
                        MACOSX_BUNDLE_BUNDLE_NAME "ImgFromClip"
                        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
                        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
                        XCODE_ATTRIBUTE_STRIP_INSTALLED_PRODUCT YES
                        XCODE_ATTRIBUTE_STRIP_STYLE YES
                        MACOSX_BUNDLE_ICON_FILE appimg_from_clip)
    if (NOT IOS)
        find_program(SIPS sips REQUIRED)
        find_program(ICONUTIL iconutil REQUIRED)

        file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/appimg_from_clip.iconset)
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/appimg_from_clip.iconset)

        foreach(img_size 16 32 64 128 256 512)
            math(EXPR size_2 "${img_size}*2")


            execute_process(
                COMMAND
                    ${SIPS} -z ${img_size} ${img_size} ${CMAKE_CURRENT_SOURCE_DIR}/appicon512.png -s format
                    png --out ${CMAKE_CURRENT_BINARY_DIR}/appimg_from_clip.iconset/icon_${img_size}x${img_size}.png
                    COMMAND_ERROR_IS_FATAL ANY)

            execute_process(
                COMMAND
                    ${SIPS} -z ${size_2} ${size_2} ${CMAKE_CURRENT_SOURCE_DIR}/appicon512.png -s format png
                    --out ${CMAKE_CURRENT_BINARY_DIR}/appimg_from_clip.iconset/icon_${size_2}x${size_2}@2x.png
                    COMMAND_ERROR_IS_FATAL ANY)
           endforeach()

           execute_process(COMMAND ${ICONUTIL} -c icns appimg_from_clip.iconset WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                           COMMAND_ERROR_IS_FATAL ANY)

          target_sources(appimg_from_clip PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/appimg_from_clip.icns)
          set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/appimg_from_clip.icns PROPERTIES MACOSX_PACKAGE_LOCATION
                                                                                              "Resources")
          set_target_properties(appimg_from_clip PROPERTIES INSTALL_RPATH "@executable_path/../Frameworks")

          set(CMAKE_MACOSX_RPATH TRUE)
    ENDIF()
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1. If
# you are developing for iOS or macOS you should consider setting an explicit,
# fixed bundle identifier manually though.
set_target_properties(
  appimg_from_clip
  PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_ID}
             MACOSX_BUNDLE TRUE
             WIN32_EXECUTABLE TRUE)

target_link_libraries(appimg_from_clip PRIVATE Qt6::Quick Qt6::Gui Qt6::Widgets
                                               Qt6::Svg compile_options_if)

if(USE_DBUS_BACKEND)
  target_link_libraries(appimg_from_clip PRIVATE Qt6::DBus)
endif()

target_include_directories(appimg_from_clip PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

IF(CMAKE_GENERATOR MATCHES "Xcode")
    target_link_options(appimg_from_clip PRIVATE -s)
ENDIF()

qt_add_translations(appimg_from_clip TS_FILES appimg_from_clip_en.ts
                    appimg_from_clip_pl.ts)

include(GNUInstallDirs)

add_subdirectory(shared)

install(
  TARGETS appimg_from_clip
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if (APPLE AND OSX_BUNDLE AND NOT IOS)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_FORMAT "UDBZ")
    set(CPACK_DMG_VOLUME_NAME "ImgFromClip")
    set(CPACK_SYSTEM_NAME "OSX")
    set(CPACK_STRIP_FILES ON)
    set(CPACK_PACKAGE_FILE_NAME "ImgFromClip-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
    include(CPack)
endif()
