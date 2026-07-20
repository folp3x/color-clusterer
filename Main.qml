import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

ApplicationWindow {
    readonly property int defaultFontSizePx: 18

    width: 580
    height: 520
    visible: true

    title: qsTr("Генератор цветовых палитр")
    Material.theme: Material.Dark
    Material.accent: Material.Blue

    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: 10
        anchors.rightMargin: 20
        anchors.leftMargin: 20
        anchors.bottomMargin: 10

        Label {
            id: exampleImageTitle
            Layout.alignment: Qt.AlignHCenter

            text: qsTr("Пример палитры изображения")
            font.pixelSize: defaultFontSizePx
        }

        Image {
            id: image
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 10
            Layout.fillHeight: true
            Layout.maximumWidth: parent.width

            source: "qrc:/examples/example.jpg"
            fillMode: Image.PreserveAspectFit
            mipmap: true
            asynchronous: true
        }

        ScrollView {
            readonly property int showedRows: 2

            id: paletteScroll
            Layout.preferredHeight: 120
            Layout.topMargin: 20
            Layout.maximumWidth: parent.width
            Layout.fillWidth: true
            contentHeight: paletteContainer.height

            Component.onCompleted: {
                paletteScroll.contentItem.boundsBehavior = Flickable.StopAtBounds
            }

            Item {
                id: paletteWrapper
                width: paletteScroll.availableWidth
                height: paletteScroll.contentHeight

                Grid {
                    readonly property int elemWidth: 80

                    id: paletteContainer
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top

                    columnSpacing: 20
                    rowSpacing: 14

                    columns: (parent.width + columnSpacing) / (elemWidth + columnSpacing) | 0

                    Repeater {
                        model: 12

                        Rectangle {
                            width: paletteContainer.elemWidth
                            height: (
                                paletteScroll.height / paletteScroll.showedRows
                                - paletteContainer.columnSpacing * (paletteScroll.showedRows - 1)
                            )
                            color: "blue"

                            Label {
                                anchors.centerIn: parent

                                text: index
                                font.pixelSize: defaultFontSizePx
                            }
                        }
                    }
                }
            }
        }
    }
}
