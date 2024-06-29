// This file is licensed under either of licenses:
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-License-Identifier: BSD-2-Clause
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import img_from_clip

Item {
    property ScriptingSupport scriptingSupport

    id: page

    property RenderConfiguration renderConfiguration
    property string compilationResult: ""
    property bool compileSuccess: false

    Drawer {
        id: compileResultDrawer
        edge: Drawer.Top
        width: page.width
        modal: true
        closePolicy: Popup.CloseOnPressOutside
        RowLayout {
            width: parent.width - 32
            x: 16
            spacing: 16

            Image {
                source: compileSuccess ? Qt.resolvedUrl(
                                             "success.png") : Qt.resolvedUrl(
                                             "error-icon.png")
                Layout.preferredWidth: 64
                Layout.preferredHeight: 64
                Layout.topMargin: 8
                Layout.alignment: Qt.AlignTop
            }

            Label {
                Layout.topMargin: 8
                Layout.bottomMargin: 8
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop
                wrapMode: Text.WordWrap
                text: compilationResult
            }
        }
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent
        anchors.bottomMargin: 16

        Flickable {
            anchors.fill: parent
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            contentHeight: column.implicitHeight

            ColumnLayout {
                id: column
                width: parent.width

                ColumnLayout {
                    Layout.preferredWidth: parent.width
                    Layout.maximumWidth: parent.width
                    Layout.minimumHeight: scrollView.height
                    spacing: 8

                    Label {
                        text: qsTr("Insert your Lua script. It will be invoked on each capture")
                        Layout.preferredWidth: parent.width
                        Layout.maximumWidth: parent.width
                        wrapMode: Text.WordWrap
                    }

                    ScrollView {
                        id: view
                        Layout.preferredWidth: parent.width
                        Layout.maximumWidth: parent.width
                        Layout.minimumHeight: 200
                        Layout.fillHeight: true
                        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                        ScrollBar.vertical.policy: ScrollBar.AlwaysOn

                        TextArea {
                            id: editor
                            font.pixelSize: 18
                            placeholderText: qsTr("Insert your Lua script")
                            text: scriptingSupport.scriptText
                            background: Rectangle {
                                color: "white"
                                border.width: 1
                                border.color: "black"
                            }
                            color: "black"
                            placeholderTextColor: "grey"
                            wrapMode: Text.WordWrap
                        }
                    }

                    Flow {
                        Layout.maximumWidth: parent.width
                        Layout.preferredWidth: parent.width
                        spacing: 16

                        Button {
                            text: qsTr("Clear")
                            spacing: 8
                            onClicked: editor.clear()
                            icon.source: Qt.resolvedUrl("dust.png")
                            icon.width: 32
                            icon.height: 32
                            icon.color: "transparent"
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        }

                        Button {
                            text: qsTr("Compile script")
                            icon.source: Qt.resolvedUrl("compiler.png")
                            icon.width: 32
                            icon.height: 32
                            icon.color: "transparent"
                            spacing: 8
                            onClicked: {
                                console.clear()
                                var result = scriptingSupport.tryCompile(
                                            editor.text)

                                if (result.length > 0) {
                                    compilationResult = qsTr(
                                                "Error:\n%1").arg(result)
                                    compileSuccess = false
                                } else {
                                    compilationResult = qsTr("No errors found")
                                    compileSuccess = true
                                }

                                compileResultDrawer.open()
                            }
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        }

                        Button {
                            text: qsTr("Try run")
                            spacing: 8
                            icon.source: Qt.resolvedUrl("compiler.png")
                            icon.width: 32
                            icon.height: 32
                            icon.color: "transparent"
                            onClicked: {
                                console.clear()
                                var result = scriptingSupport.tryRun(
                                            editor.text)

                                if (result.length > 0) {
                                    compilationResult = qsTr(
                                                "Error:\n%1").arg(result)
                                    compileSuccess = false
                                } else {
                                    compilationResult = qsTr("Success")
                                    compileSuccess = true
                                }

                                compileResultDrawer.open()
                            }
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        }

                        Button {
                            text: qsTr("Save")
                            onClicked: {
                                scriptingSupport.scriptText = editor.text
                            }
                        }
                    }

                    Label {
                        Layout.preferredWidth: parent.width
                        Layout.maximumWidth: parent.width
                        text: qsTr("Lua output:")
                    }

                    ScrollView {
                        Layout.preferredWidth: parent.width
                        Layout.maximumWidth: parent.width
                        Layout.minimumHeight: 100
                        Layout.fillHeight: false
                        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                        ScrollBar.vertical.policy: ScrollBar.AlwaysOn

                        TextArea {
                            id: console
                            wrapMode: Text.WordWrap
                            textFormat: Text.PlainText
                            readOnly: true

                            Connections {
                                target: scriptingSupport
                                function onOutputPrint(line) {
                                    console.append(line)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
