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

    readonly property int btnMdW: 200
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

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumWidth: parent.width
            Layout.alignment: Qt.AlignHCenter

            Image {
                id: image
                anchors.fill: parent

                source: "qrc:/examples/example.jpg"
                fillMode: Image.PreserveAspectFit
                mipmap: true
                visible: !app.isProcessingImage
            }

            Rectangle {
                anchors.centerIn: image
                width: image.paintedWidth
                height: image.paintedHeight
                z: -1

                visible: !app.isProcessingImage
                color: "#FFFFFF"
            }
        }

        BusyIndicator {
            Layout.fillHeight: true
            Layout.maximumWidth: parent.width
            Layout.alignment: Qt.AlignHCenter

            running: app.isProcessingImage
            visible: running
        }

        ScrollView {
            readonly property int showedRows: 1
            readonly property int rowGap: 14
            readonly property int rowHeight: 60

            id: paletteScroll
            visible: !app.isProcessingImage

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
            Layout.fillWidth: true
            Layout.maximumWidth: 400
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

        Flow {
            readonly property int elemWidth: btnMdW

            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: paletteColorCountText.height + 6

            spacing: 6

            leftPadding: {
                let countInRow = ((width + spacing) / (elemWidth + spacing)) | 0;
                let totalWidth = (elemWidth * countInRow) + spacing * (countInRow - 1);
                if (width >= totalWidth) {
                    return (width - totalWidth) / 2;
                } else {
                    return (width - elemWidth) / 2;
                }
            }

            Button {
                width: parent.elemWidth

                text: qsTr("Выбрать изображение")
                font.pixelSize: fontSmPx
                enabled: !app.isProcessingImage

                onClicked: {
                    chooseImgDialog.open();
                }
            }

            Button {
                width: parent.elemWidth

                text: qsTr("Сохранить палитру")
                font.pixelSize: fontSmPx
                enabled: !app.isProcessingImage

                onClicked: {
                    savePaletteDialog.open();
                }
            }
        }
    }

    FileDialog {
        id: chooseImgDialog

        title: "Выберите файл для открытия"
        currentFolder: StandardPaths.writableLocation(StandardPaths.DownloadLocation)
        nameFilters: "Изображения (*.bmp *.jpg *.jpeg *.png)"

        onAccepted: {
            exampleTitle.text = "";

            let filePath = selectedFile;
            image.source = filePath;
            app.clusterImage(filePath, paletteColorCountSlider.value);
        }
    }

    FileDialog {
        id: savePaletteDialog

        title: "Сохранить палитру"
        fileMode: FileDialog.SaveFile
        currentFolder: StandardPaths.writableLocation(StandardPaths.DownloadLocation)
        nameFilters: ["Текстовые файлы (*.txt)"]

        onAccepted: {
            app.savePalette(selectedFile);
            savePalleteInfo.open();
        }
    }

    Dialog {
        id: savePalleteInfo
        title: qsTr("Успех")
        anchors.centerIn: parent
        modal: true
        padding: 40

        standardButtons: Dialog.Ok

        Label {
            text: qsTr("Палитра сохранена")
            font.pixelSize: fontSmPx
        }
    }
}
