import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import QtQuick.Dialogs
import QtCore

ApplicationWindow {
    readonly property int fontMdPx: 18
    readonly property int fontSmPx: 16
    readonly property int fontXsPx: 14

    width: 600
    minimumWidth: 375
    height: 520
    minimumHeight: 450
    visible: true

    title: qsTr("Генератор цветовых палитр")

    Component.onCompleted: {
        app.clusterImage(image.source, paletteColorCountSlider.value);
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20

        Label {
            id: exampleTitle
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: 10

            text: qsTr("Пример палитры изображения")
            font.pixelSize: fontMdPx
        }

        Image {
            id: image

            Layout.fillHeight: true
            Layout.maximumWidth: parent.width
            Layout.alignment: Qt.AlignHCenter

            source: "qrc:/examples/example.jpg"
            fillMode: Image.PreserveAspectFit
            mipmap: true
            visible: !app.isClustering
        }

        BusyIndicator {
            Layout.fillHeight: true
            Layout.maximumWidth: parent.width
            Layout.alignment: Qt.AlignHCenter

            running: app.isClustering
            visible: running
        }

        ScrollView {
            readonly property int showedRows: 1
            readonly property int rowGap: 14
            readonly property int rowHeight: 60

            id: paletteScroll
            visible: !app.isClustering

            Layout.minimumHeight: rowHeight + rowGap * 2
            Layout.maximumHeight: rowHeight * showedRows + rowGap * (showedRows + 1)
            Layout.topMargin: 20
            Layout.fillWidth: true
            contentHeight: paletteContainer.height
            leftPadding: paletteScrollBar.width
            rightPadding: paletteScrollBar.width

            Component.onCompleted: {
                contentItem.boundsBehavior = Flickable.StopAtBounds
            }

            ScrollBar.vertical : ScrollBar {
                id: paletteScrollBar

                anchors.right: parent.right
                height: parent.height

                policy: ScrollBar.AlwaysOn
                visible: size < 1
            }

            Item {
                width: parent.width

                Grid {
                    readonly property int colWidth: 120
                    readonly property int maxCols: ((
                        parent.width + columnSpacing) /
                        (colWidth + columnSpacing) | 0
                    )

                    id: paletteContainer

                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter

                    columnSpacing: 20
                    rowSpacing: paletteScroll.rowGap

                    columns: Math.min(paletteElems.count, maxCols)

                    Repeater {
                        id: paletteElems

                        model: app.palette

                        ColumnLayout {
                            width: paletteContainer.colWidth
                            height: paletteScroll.rowHeight

                            Rectangle {
                                Layout.maximumWidth: parent.width
                                Layout.maximumHeight: parent.height
                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                color: modelData.hexCode
                            }

                            RowLayout {
                                Layout.alignment: Qt.AlignHCenter

                                spacing: 4

                                TextEdit {
                                    Layout.alignment: Qt.AlignBaseline

                                    text: modelData.hexCode
                                    font.pixelSize: fontSmPx
                                    readOnly: true
                                    selectByMouse: true
                                    color: ApplicationWindow.window.palette.text

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.IBeamCursor
                                        acceptedButtons: Qt.NoButton
                                    }
                                }

                                Label {
                                    Layout.alignment: Qt.AlignBaseline

                                    text: "(" +
                                        parseFloat(
                                            (modelData.percentage * 100).toFixed(2)
                                        ) +
                                    "%)"
                                    font.pixelSize: fontXsPx
                                }
                            }
                        }
                    }
                }
            }
        }

        RowLayout {
            Layout.maximumWidth: 400
            Layout.fillWidth: true
            Layout.topMargin: 10
            Layout.alignment: Qt.AlignHCenter

            spacing: 14

            Label {
                text: qsTr("Количество цветов:")
                font.pixelSize: fontSmPx
            }

            Slider {
                id: paletteColorCountSlider

                Layout.fillWidth: true

                from: 2
                to: 16
                stepSize: 1
                value: 6
                snapMode: Slider.SnapAlways
            }
        }

        Label {
            id: paletteColorCountText
            parent: paletteColorCountSlider.handle
            anchors.top: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter

            text: paletteColorCountSlider.value
            font.pixelSize: fontXsPx
        }

        Button {
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: paletteColorCountText.height + 6

            text: qsTr("Выбрать изображение")
            font.pixelSize: fontSmPx
            enabled: !app.isClustering

            onClicked: {
                fileDialog.open();
            }
        }

        FileDialog {
            id: fileDialog
            title: "Выберите файл для открытия"
            currentFolder: StandardPaths.writableLocation(StandardPaths.DownloadLocation)
            nameFilters: "Изображения (*.bmp *.jpg *.jpeg *.png)"

            onAccepted: {
                exampleTitle.text = "";

                var filePath = fileDialog.selectedFile;
                image.source = filePath;
                app.clusterImage(filePath, paletteColorCountSlider.value);
            }
        }
    }
}
