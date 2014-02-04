import QtQuick 1.0

Rectangle {
    id: rect
    width: 0
    height: 0
    color: "#CCC"
    radius: 10
    border.width: 1
    border.color: "#999"
    smooth: true
    clip: true
    property string graphTitle: "graph title here"
    property string xName: "x axis title here"
    property string yName: "y axis title here"
    property variant xAxis: []
    property variant yAxis: []
    property string lineColor: []
    property int popupFontSize: 11
    property list<Point> listOfPoints
    property string xMin: ""
    property string xMax: ""

    gradient: Gradient {
        GradientStop { position: 0.0; color: "#eee" }
        GradientStop { position: 1.0; color: "#aaa" }
    }

    GraphPoints {
        id: graphElement
        width: parent.width - 100; height: parent.height - 100
        z: parent.z + 1
        anchors.centerIn: parent
        lineColor: rect.lineColor
        xAxis: rect.xAxis
        yAxis: rect.yAxis


        Rectangle {
            id: popup
            color: "black"
            x: 0; y: 0
            width: popupText.width + 10;
            height: popupText.height + 5;
            radius: 5
            z: parent.z + 1
            clip: true
            smooth: true
            border.width: 1
            border.color: "white"
            opacity: 0
            Behavior on opacity { PropertyAnimation {duration: 250} }
            Text {
                id: popupText
                //text: node.name + " (" + node.xLabel + ", " + node.yLabel + ")"
                color: "white"
                font.pixelSize: rect.popupFontSize
                anchors.centerIn: parent
            }
        }
    }

    Rectangle {
        id: title
        anchors.top: parent.top
        height: (parent.height - graphElement.height) / 2
        width: parent.width
        color: "transparent"
        Text {
            anchors.centerIn: parent
            text: rect.graphTitle
            font.pixelSize: 22
            font.weight: Font.Bold
            font.letterSpacing: 1
        }
    }

    Rectangle {
        id: xAxisName
        anchors.bottom: parent.bottom
        height: (parent.height - graphElement.height) / 2
        width: parent.width
        color: "transparent"
        Text {
            anchors.centerIn: parent
            text: rect.xName
            font.pixelSize: 16
        }
        Text {
            anchors.left: parent.left
            anchors.leftMargin: (rect.width - graphElement.width) / 2
            text: rect.xMin
            font.pixelSize: 12
        }
        Text {
            anchors.right: parent.right
            anchors.rightMargin: ((rect.width - graphElement.width) / 2) + graphElement.intervalX
            text: rect.xMax
            font.pixelSize: 12
        }
    }

    Rectangle {
        id: yAxisName
        anchors.left: parent.left
        height: parent.height
        width: (parent.width - graphElement.width) / 2
        color: "transparent"
        Text {
            anchors.centerIn: parent
            text: rect.yName
            font.pixelSize: 16
            rotation: 270
        }
    }

    function setAxis(axisType, axisArray) {
        if(axisType == "x") {
            rect.xAxis = axisArray;
            console.log("Graph.qml: x: " + rect.xAxis)
        }
        else if(axisType == "y") {
            rect.yAxis = axisArray;
            console.log("Graph.qml: y: " + rect.yAxis)
        }
    }

    function setDimension(width, height){
        rect.width = width;
        rect.height = height;
    }

    function createGraphPoint(pname, pcolor, pxLabel, pyLabel) {
        var component = Qt.createComponent("Point.qml");
        if(component.status == Component.Ready) {
            var newPoint = component.createObject(graphElement);
            newPoint.name = pname;
            newPoint.color = pcolor;
            newPoint.xLabel = pxLabel;
            newPoint.yLabel = pyLabel;
            console.log(pname + "," + pcolor + "," + pxLabel + "," + pyLabel);
        }
    }

    function plotPoints() {
        graphElement.plotPoints();
    }

    signal showNodePopup(int nodeX, int nodeY, string nodeText)
    signal hideNodePopup
    onShowNodePopup: {
        popupText.text = nodeText;
        popup.x = nodeX - popupText.width/2;
        popup.y = nodeY - popupText.height;
        popup.opacity = 1;
    }
    onHideNodePopup: {
        popup.opacity = 0;
    }


}
