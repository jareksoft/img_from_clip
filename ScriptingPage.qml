import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import img_from_clip

Item {
    property ScriptingSupport scriptingSupport

    id: page

    property RenderConfiguration renderConfiguration

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
                            onTextChanged: {
                                scriptingSupport.scriptText = text
                            }
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

                    RowLayout {
                        Button {
                            text: qsTr("Clear")
                            onClicked: editor.clear()
                        }
                    }
                }
            }
        }
    }
}
