if(UNIX AND NOT APPLE)
  if(FLATPAK_DIST)
    set(ICON_NAME ${APP_ID})
  else()
    set(ICON_NAME appimg_from_clip)
  endif()

  configure_file(linux/com.jpelczar.img_from_clip.desktop.in
                 linux/${APP_ID}.desktop @ONLY)

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/linux/${APP_ID}.desktop
          DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
  install(FILES linux/${APP_ID}.appdata.xml
          DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)

  function(_inst_icon Res File)
    set(ICON_DEST ${CMAKE_INSTALL_DATADIR}/icons/hicolor/${Res}/apps)
    install(
      FILES ${File}
      DESTINATION ${ICON_DEST}
      RENAME ${ICON_NAME}.png)
  endfunction()

  _inst_icon(64x64 ${CMAKE_CURRENT_SOURCE_DIR}/../appicon64.png)
  _inst_icon(128x128 ${CMAKE_CURRENT_SOURCE_DIR}/../appicon128.png)
  _inst_icon(256x256 ${CMAKE_CURRENT_SOURCE_DIR}/../appicon256.png)
  _inst_icon(512x512 ${CMAKE_CURRENT_SOURCE_DIR}/../appicon512.png)
endif()
