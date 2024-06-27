import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import img_from_clip

ScrollView {
    id: mainView
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
    ScrollBar.vertical.policy: ScrollBar.AsNeeded

    property RenderConfiguration renderConfiguration

    Flickable {
        id: flickable
        width: parent.width
        leftMargin: 20
        rightMargin: 20
        bottomMargin: 32
        contentHeight: mainRow.implicitHeight

        ColumnLayout {
            id: mainRow
            width: parent.width
            spacing: 16

            GroupBox {
                title: qsTr("PNG scale")
                Layout.maximumWidth: parent.width
                Layout.preferredWidth: parent.width

                ColumnLayout {
                    width: parent.width
                    spacing: 8

                    Label {
                        wrapMode: Text.WordWrap
                        text: qsTr("Select at which scales PNG files should be saved. This is useful e.g. when exporting images for iOS")
                        Layout.fillWidth: true
                    }

                    CheckBox {
                        Layout.fillWidth: true
                        text: qsTr("1.5x")
                        checked: renderConfiguration.scale15
                        onCheckedChanged: renderConfiguration.scale15 = checked
                    }

                    CheckBox {
                        Layout.fillWidth: true
                        text: qsTr("2x")
                        checked: renderConfiguration.scale2
                        onCheckedChanged: renderConfiguration.scale2 = checked
                    }

                    CheckBox {
                        Layout.fillWidth: true
                        text: qsTr("3x")
                        checked: renderConfiguration.scale3
                        onCheckedChanged: renderConfiguration.scale3 = checked
                    }

                    CheckBox {
                        Layout.fillWidth: true
                        text: qsTr("4x")
                        checked: renderConfiguration.scale4
                        onCheckedChanged: renderConfiguration.scale4 = checked
                    }
                }
            }
        }
    }
}
