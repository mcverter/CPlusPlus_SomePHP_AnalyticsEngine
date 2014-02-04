import QtQuick 1.0

Rectangle {
    id: rect
    width: 0
    height: 0
    color: Qt.lighter("#333")
    smooth: true
    property variant xAxis: []
    property variant yAxis: []
    property string lineColor: "black"
    property int startX: 0
    property int startY: 0
    property double intervalX: (rect.width - 10) / rect.xAxis.length
    property double intervalY: (rect.height - 10) / rect.yAxis.length
    property int xOffset: -5
    property int yOffset: 5

    border.width: 1
    border.color: "white"

    gradient: Gradient {
        GradientStop { position: 0.0; color: Qt.lighter("#222") }
        GradientStop { position: 1.0; color: Qt.lighter("#666") }
    }

    function plotPoints() {
        console.log("GraphPoints.qml: " + rect.width + "," + rect.height
                    + "  " + rect.intervalX + "," + rect.intervalY)
        plotTimer.start();
    }

    property int plotCounter: 0;
    property int plotPointDuration: 2000
    property int plotTimerDuration: 150
    Timer {
        id: plotTimer
        interval: rect.plotTimerDuration
        repeat: true
        onTriggered: {
            if(plotCounter < rect.children.length)
            {
                var prevXindex, prevYindex, currXindex, currYindex;
                if(rect.children[plotCounter].type == "graphPoint")
                {
                    //console.log(plotCounter + " " + rect.children[plotCounter].name + " found at " + rect.children[plotCounter].xLabel + "," + rect.children[plotCounter].yLabel)
                    currXindex = xAxis.indexOf(rect.children[plotCounter].xLabel);
                    currYindex = yAxis.indexOf(rect.children[plotCounter].yLabel);
                    if(plotCounter == 0) {
                        createNewNode(rect.children[plotCounter].name, rect.children[plotCounter].color,
                                      (currXindex) * intervalX + rect.xOffset, (currYindex) * intervalY + rect.yOffset,
                                      rect.xOffset, rect.yOffset,
                                      rect.children[plotCounter].xLabel, rect.children[plotCounter].yLabel)
                    }
                    else {
                        prevXindex = xAxis.indexOf(rect.children[plotCounter-1].xLabel);
                        prevYindex = yAxis.indexOf(rect.children[plotCounter-1].yLabel);
                        createNewNode(rect.children[plotCounter].name, rect.children[plotCounter].color,
                                      (currXindex) * intervalX + rect.xOffset, (currYindex) * intervalY + rect.yOffset,
                                      (prevXindex) * intervalX + rect.xOffset, (prevYindex) * intervalY + rect.yOffset,
                                      rect.children[plotCounter].xLabel, rect.children[plotCounter].yLabel)
                    }
                    plotCounter++;
                    plotTimer.restart();
                }
                else { plotTimer.stop(); }
            }
        }
    }

    function createNewNode(name, color, currX, currY, prevX, prevY, xLabel, yLabel)
    {
//        console.log(name + "," + color + "," + currX + "," + currY + "," + prevX + "," + prevY)
        var component;
        component = Qt.createComponent("GraphNode.qml");
        if(component.status == Component.Ready) {
            var newNode = component.createObject(rect);
            newNode.name = name;
            newNode.color = color;
            newNode.x = currX;
            newNode.y = rect.height - currY;
            newNode.xLabel = xLabel;
            newNode.yLabel = yLabel;
            newNode.plotPointDuration = rect.plotPointDuration;
        }
        component = Qt.createComponent("GraphLine.qml");
        if(component.status == Component.Ready) {
            var newLine = component.createObject(rect);
            newLine.name = name;
            newLine.lineColor = rect.lineColor;
            newLine.currentX = currX;
            newLine.currentY = rect.height - currY;
            newLine.previousX = prevX;
            newLine.previousY = rect.height - prevY;
            newNode.plotPointDuration = rect.plotPointDuration;
        }
    }

}
