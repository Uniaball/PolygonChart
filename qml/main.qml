import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import Qt.labs.platform 1.1 as Platform
import PolygonChart

ApplicationWindow {
    id: root
    width: 480
    height: 800
    visible: true
    title: "多边形成绩图表"

    Material.theme: Material.Light
    Material.accent: Material.Blue

    RadarChartModel {
        id: chartModel
        onDataChanged: {
            for (var i = 0; i < count; ++i) {
                if (i < nameInputs.length) {
                    nameInputs[i].text = chartModel.names[i]
                    scoreInputs[i].value = chartModel.scoreAt(i)
                    maxInputs[i].value = chartModel.maxScoreAt(i)
                }
            }
        }
    }

    RadarChartRenderer {
        id: chartRenderer
        anchors.fill: parent
        anchors.bottomMargin: controlPanel.height
        model: chartModel
    }

    Rectangle {
        id: controlPanel
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: Math.min(parent.height * 0.45, 420)
        color: Material.backgroundColor
        elevation: 4
        clip: true

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 8

            RowLayout {
                spacing: 12
                Label { text: "项目数:" }
                SpinBox {
                    id: projectCountSpin
                    from: 3
                    to: 12
                    value: chartModel.count
                    onValueChanged: {
                        var oldCount = chartModel.count
                        if (value === oldCount) return
                        var names = []
                        var scores = []
                        var maxScores = []
                        for (var i = 0; i < value; ++i) {
                            if (i < oldCount) {
                                names.push(chartModel.names[i])
                                scores.push(chartModel.scoreAt(i))
                                maxScores.push(chartModel.maxScoreAt(i))
                            } else {
                                names.push("项目" + (i+1))
                                scores.push(100.0)
                                maxScores.push(100.0)
                            }
                        }
                        chartModel.setData(names, scores, maxScores)
                    }
                }
                Item { Layout.fillWidth: true }
            }

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true

                Column {
                    id: inputColumn
                    spacing: 6
                    width: parent.width - 10

                    Repeater {
                        id: repeater
                        model: chartModel.count
                        
                        RowLayout {
                            spacing: 6
                            width: inputColumn.width

                            property int idx: index

                            Label {
                                text: (index+1) + ":"
                                Layout.preferredWidth: 24
                            }
                            TextField {
                                id: nameField
                                text: chartModel.names[index]
                                Layout.minimumWidth: 60
                                onEditingFinished: {
                                    if (text !== chartModel.names[index])
                                        chartModel.setName(index, text)
                                }
                            }
                            DoubleSpinBox {
                                id: scoreField
                                value: chartModel.scoreAt(index)
                                from: 0
                                to: 1000000
                                decimals: 1
                                stepSize: 1
                                Layout.minimumWidth: 70
                                onValueChanged: {
                                    if (!activeFocus) return
                                    if (!qFuzzyCompare(value, chartModel.scoreAt(index)))
                                        chartModel.setScore(index, value)
                                }
                            }
                            Label { text: "上限" }
                            DoubleSpinBox {
                                id: maxField
                                value: chartModel.maxScoreAt(index)
                                from: 0.01
                                to: 1000000
                                decimals: 2
                                stepSize: 10
                                Layout.minimumWidth: 70
                                onValueChanged: {
                                    if (!activeFocus) return
                                    if (!qFuzzyCompare(value, chartModel.maxScoreAt(index)))
                                        chartModel.setMaxScore(index, value)
                                }
                            }
                        }
                    }
                }
            }

            RowLayout {
                spacing: 8
                Button {
                    text: "保存图片"
                    onClicked: fileDialogSaveImage.open()
                }
                Button {
                    text: "保存 JSON"
                    onClicked: fileDialogSaveJson.open()
                }
                Button {
                    text: "打开 JSON"
                    onClicked: fileDialogLoadJson.open()
                }
            }
        }
    }

    Platform.FileDialog {
        id: fileDialogSaveImage
        title: "保存雷达图"
        currentFile: "radar_chart.png"
        fileTypes: ["PNG 图片 (*.png)"]
        onAccepted: {
            var img = chartRenderer.grabToImage()
            img.imageSaved.connect(function() {
                // 可选提示
            })
            img.saveToFile(file)
        }
    }

    Platform.FileDialog {
        id: fileDialogSaveJson
        title: "保存数据"
        currentFile: "radar_data.json"
        fileTypes: ["JSON 文件 (*.json)"]
        onAccepted: {
            if (chartModel.saveToJson(file)) {
                // 成功，可显示 toast，为简化不处理
            }
        }
    }

    Platform.FileDialog {
        id: fileDialogLoadJson
        title: "打开 JSON 数据"
        fileTypes: ["JSON 文件 (*.json)"]
        onAccepted: {
            if (chartModel.loadFromJson(file)) {
                projectCountSpin.value = chartModel.count
            } else {
                errorMessage.visible = true
            }
        }
    }

    Dialog {
        id: errorMessage
        title: "加载失败"
        standardButtons: Dialog.Ok
        modal: true
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        Label {
            text: "JSON 格式无效或项目数不在 3～12 之间"
            wrapMode: Text.WordWrap
            width: 250
        }
        onAccepted: close()
    }
}