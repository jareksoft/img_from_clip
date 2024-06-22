import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtCore
import img_from_clip

ApplicationWindow {
    id: mainWindow
    width: 360
    height: 480
    visible: true
    title: qsTr("Image from Clipboard")
    minimumWidth: 180
    minimumHeight: 100

    Settings {
        id: appSettings
        property string defaultWritePath
        property bool activeOnStart: false
        property string savePattern
        property int saveMode

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

    SysTray {
        id: sysTray
        isActive: clipMonitor.active
        iconPath: ":/qt/qml/img_from_clip/appicon24.png"
    }

    ClipMonitor {
        id: clipMonitor
        savePath: appSettings.defaultWritePath
        active: appSettings.activeOnStart
        savePattern: appSettings.savePattern
        saveMode: appSettings.saveMode
    }

    FolderDialog {
        id: pathBrowser
        currentFolder: appSettings.defaultWritePath
        onAccepted: {
            appSettings.defaultWritePath = selectedFolder
        }
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
                        }

                        RadioButton {
                            Layout.maximumWidth: parent.width
                            checked: clipMonitor.saveMode == ClipMonitor.PNG
                            text: qsTr("PNG")
                        }

                        RadioButton {
                            Layout.maximumWidth: parent.width
                            checked: clipMonitor.saveMode == ClipMonitor.JPG
                            text: qsTr("JPG")
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
}
