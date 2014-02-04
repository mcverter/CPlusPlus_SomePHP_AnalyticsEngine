import QtQuick 1.0

Rectangle {
    id: node
    z: 1
    property string name: "node"
    property string borderColor: "white"
    property string xLabel: ""
    property string yLabel: ""
    property int plotPointDuration: 1000

    opacity: 0
    Behavior on opacity { PropertyAnimation { duration: node.plotPointDuration } }
    Component.onCompleted: { node.opacity = 1 }

    x: 0; y: 0
    width: 12; height: width; radius: width/2
    color: "green"
    border.width: 1
    border.color: node.borderColor

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            showNodePopup(node.x,
                           node.y,
                           node.name + " (" + node.xLabel + ", " + node.yLabel + ")");
        }
        onExited: {
            hideNodePopup();
        }
    }

}
