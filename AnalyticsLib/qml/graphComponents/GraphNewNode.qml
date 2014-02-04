import QtQuick 1.0

Rectangle {
    width: 0
    height: 0
    property string name: "newNode"
    property int currentX: 0
    property int currentY: 0
    property int previousX: 0
    property int previousY: 0
    property string lineColor: "white"
    property string nodeColor: "green"
    property string xLabel: ""
    property string yLabel: ""
    color: "transparent"

    GraphNode {
        id: node
        z: 5
        x: parent.currentX
        y: parent.height - parent.currentY
        color: parent.nodeColor
        borderColor: parent.lineColor
        xLabel: parent.xLabel
        yLabel: parent.yLabel
        name: parent.name
    }
    GraphLine {
        id: line
        z: 0
        lineColor: parent.lineColor
        currentX: parent.currentX
        currentY: parent.height - parent.currentY
        previousX: parent.previousX
        previousY: parent.height - parent.previousY
    }
}
