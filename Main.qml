// SPDX-License-Identifier: BSD-2-Clause
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtCore
import img_from_clip
import Qt.labs.platform as Labs

ApplicationWindow {
    id: mainWindow
    width: 360
    height: 500
    visible: true
    title: qsTr("Image from Clipboard")
    minimumWidth: 180
    minimumHeight: 100

    property bool askedAboutNotifications: false
    property bool htmlAllowed: false
    property real htmlRenderWidth: 800

    Labs.MenuBar {
        id: menuBar

        Labs.Menu {
            id: fileMenu
            title: qsTr("File")

            Labs.MenuItem {
                text: qsTr("&About")
                onTriggered: {
                    sysTray.aboutApp()
                }
            }
        }
    }

    Settings {
        id: appSettings
        property string defaultWritePath
        property bool activeOnStart: false
        property string savePattern
        property int saveMode
        property alias askedAboutNotifications: mainWindow.askedAboutNotifications
        property alias allowHtml: mainWindow.htmlAllowed
        property alias htmlRenderWidth: mainWindow.htmlRenderWidth

        Component.onCompleted: {
            if (defaultWritePath === "") {
                defaultWritePath = StandardPaths.standardLocations(
                            StandardPaths.PicturesLocation)[0]
            }

            if (savePattern === "") {
                savePattern = "{timestamp}"
            }
        }
    }

    PlatformSupport {
        id: platformSupport
        onPermissionStateFetchCompletion: {
            console.log("Permission check complete")
            if (platformSupport.requiresNotificationPermission
                    && (platformSupport.notificationsAllowed < 0
                        || !askedAboutNotifications)) {
                permissionDrawer.open()
            }
        }
        onGenericNotifyError: (title, body) => {
                                  sysTray.notifyError(title, body)
                              }
        onGenericNotifyInfo: (title, body) => {
                                 sysTray.notifyInfo(title, body)
                             }
    }

    SysTray {
        id: sysTray
        isActive: clipMonitor.active
        iconPath: ":/qt/qml/img_from_clip/appicon64.png"
        notificationsAllowed: platformSupport.notificationsAllowed > 0
    }

    ClipMonitor {
        id: clipMonitor
        savePath: appSettings.defaultWritePath
        active: appSettings.activeOnStart
        savePattern: appSettings.savePattern
        saveMode: appSettings.saveMode
        htmlAllowed: appSettings.allowHtml
        renderWidth: appSettings.htmlRenderWidth
        onSaveFailed: function (reason) {
            platformSupport.notify(PlatformSupport.Error,
                                   qsTr("Capture failed"), reason)
            captureDrawer.notifyInfo(qsTr("Could not capture image"))
        }
        onNotifyCapture: function (lastPath) {
            platformSupport.notifyWithImage(PlatformSupport.Info,
                                            qsTr("Image captured"),
                                            qsTr("Saved file %1").arg(
                                                lastPath), lastPath)
            captureDrawer.notifyInfoAndImage(qsTr("Saved file %1").arg(
                                                 lastPath), lastPath)
        }
    }

    FolderDialog {
        id: pathBrowser
        currentFolder: appSettings.defaultWritePath
        onAccepted: {
            appSettings.defaultWritePath = selectedFolder
        }
    }

    MessageDialog {
        id: notificationMessageDialog
    }

    ScrollView {
        anchors.fill: parent
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn

        Flickable {
            width: parent.width
            leftMargin: 20
            rightMargin: 20
            contentHeight: mainRow.implicitHeight

            ColumnLayout {
                id: mainRow
                width: parent.width
                spacing: 16

                ColumnLayout {
                    Layout.maximumWidth: parent.width
                    Layout.preferredWidth: parent.width
                }

                Label {
                    Layout.maximumWidth: parent.width
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: qsTr("Simple tool which automatically captures contents of clipboard and saves it as image")
                }

                GroupBox {
                    Layout.maximumWidth: parent.width
                    Layout.preferredWidth: parent.width
                    title: qsTr("Save location")

                    RowLayout {
                        width: parent.width
                        spacing: 16

                        TextField {
                            id: savePath
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                            text: appSettings.defaultWritePath
                        }

                        Button {
                            text: qsTr("...")
                            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                            onClicked: {
                                pathBrowser.open()
                            }
                        }
                    }
                }

                GroupBox {
                    Layout.maximumWidth: parent.width
                    Layout.preferredWidth: parent.width
                    title: qsTr("Save mode")

                    ColumnLayout {
                        width: parent.width

                        RadioButton {
                            Layout.maximumWidth: parent.width
                            checked: clipMonitor.saveMode == ClipMonitor.SVG
                            text: qsTr("SVG")
                            onClicked: {
                                appSettings.saveMode = ClipMonitor.SVG
                            }
                        }

                        RadioButton {
                            Layout.maximumWidth: parent.width
                            checked: clipMonitor.saveMode == ClipMonitor.PNG
                            text: qsTr("PNG")
                            onClicked: {
                                appSettings.saveMode = ClipMonitor.PNG
                            }
                        }

                        RadioButton {
                            Layout.maximumWidth: parent.width
                            checked: clipMonitor.saveMode == ClipMonitor.JPG
                            text: qsTr("JPG")
                            onClicked: {
                                appSettings.saveMode = ClipMonitor.JPG
                            }
                        }
                    }
                }

                GroupBox {
                    Layout.maximumWidth: parent.width
                    Layout.preferredWidth: parent.width
                    title: qsTr("Render HTML")
                    ColumnLayout {
                        width: parent.width

                        CheckBox {
                            id: allowHtmlCheckBox
                            text: qsTr("Enable HTML rendering")
                            checked: appSettings.allowHtml
                            onCheckedChanged: {
                                appSettings.allowHtml = checked
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            Layout.maximumWidth: parent.width
                            spacing: 16
                            Label {
                                text: qsTr("Page width:")
                            }
                            TextField {
                                text: mainWindow.htmlRenderWidth
                                validator: IntValidator {
                                    bottom: 100
                                    top: 8192
                                }
                                onAccepted: {
                                    mainWindow.htmlRenderWidth = text
                                }
                            }
                        }

                        Label {
                            Layout.maximumWidth: parent.width
                            wrapMode: Text.WordWrap
                            text: qsTr("Note: This feature uses Qt rendering engine, so results may not be perfect")
                        }
                    }
                }

                Button {
                    id: activateButton
                    checkable: true
                    text: checked ? qsTr("Deactivate") : qsTr("Activate")

                    Binding {
                        appSettings.activeOnStart: activateButton.checked
                    }

                    Component.onCompleted: {
                        checked = appSettings.activeOnStart
                    }
                }
            }
        }
    }

    Drawer {
        id: permissionDrawer
        width: mainWindow.width
        x: (mainWindow.width - permissionDrawer.width) / 2
        edge: Qt.TopEdge
        modal: false

        Pane {
            horizontalPadding: 32
            verticalPadding: 32
            id: drawerPane
            width: parent.width
            ColumnLayout {
                width: parent.width
                spacing: 16
                RowLayout {
                    Layout.maximumWidth: parent.width
                    spacing: 16

                    Image {
                        source: Qt.resolvedUrl("appicon64.png")
                        Layout.alignment: Qt.AlignVCenter
                    }

                    Label {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: qsTr("Application uses notifications to display information about captured screenshots")
                        Layout.alignment: Qt.AlignTop
                    }
                }
                RowLayout {
                    Layout.maximumWidth: parent.width
                    Button {
                        text: qsTr("Request")
                        onClicked: {
                            permissionDrawer.close()
                            platformSupport.requestNotifications()
                            mainWindow.askedAboutNotifications = true
                        }
                    }
                    Button {
                        text: qsTr("Don't ask")
                        onClicked: {
                            permissionDrawer.close()
                            mainWindow.askedAboutNotifications = true
                        }
                    }
                }
            }
        }
    }

    Drawer {
        id: captureDrawer
        width: mainWindow.width * 0.9
        x: (mainWindow.width - captureDrawer.width) / 2
        edge: Qt.BottomEdge
        modal: false

        property string lastCaptureName

        contentWidth: captureDrawerPane.implicitWidth
        contentHeight: captureDrawerPane.implicitHeight

        Pane {
            id: captureDrawerPane
            horizontalPadding: 32
            verticalPadding: 32
            width: parent.width
            RowLayout {
                width: parent.width
                spacing: 8

                Image {
                    id: capImage
                    fillMode: Image.PreserveAspectFit
                    Layout.maximumWidth: 80
                    Layout.maximumHeight: 80
                }

                Label {
                    text: captureDrawer.lastCaptureName
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }
        }

        MouseArea {
            id: theMouse
            anchors.fill: parent
            hoverEnabled: true
        }

        Timer {
            id: captureTimer
            running: captureDrawer.visible && !theMouse.containsMouse
            onTriggered: captureDrawer.close()
            interval: 5000
            repeat: false
        }

        function notifyInfo(info) {
            captureDrawer.lastCaptureName = info
            capImage.source = ""
            captureDrawer.open()
        }

        function notifyInfoAndImage(info, path) {
            captureDrawer.lastCaptureName = info
            capImage.source = "file://" + path
            captureDrawer.open()
        }
    }
}
