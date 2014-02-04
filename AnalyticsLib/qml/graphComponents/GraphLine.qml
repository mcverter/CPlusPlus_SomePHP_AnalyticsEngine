import QtQuick 1.0

Rectangle {
    id: line
    z: 0
    property string name: "line"
    property int currentX: 0
    property int currentY: 0
    property int previousX: 0
    property int previousY: 0
    property int coordinateOffset: 6
    property string lineColor: "white"
    property int plotPointDuration: 1000
    color: line.lineColor

    opacity: 0
    Behavior on opacity { PropertyAnimation { duration: line.plotPointDuration } }
    Component.onCompleted: { line.opacity = 1 }

    x: line.currentX+coordinateOffset; y: line.currentY+coordinateOffset
    property int deltaX: line.currentX - line.previousX;
    property int deltaY: line.currentY - line.previousY;
    width: 1;
    height: Math.sqrt(Math.pow(line.deltaX,2) + Math.pow(line.deltaY,2));
    smooth: true;

    transform: Rotation {
        id: lineRotation
        origin.x: 0; origin.y: 0
        angle: 90 + (Math.atan2(line.deltaY,line.deltaX) * 180 / Math.PI)
//        angle: Math.atan(line.deltaY/line.deltaX) * 180 / Math.PI
    }

//    Component.onCompleted: console.log(deltaX + "," + deltaY + "," + lineRotation.angle)

}
