import QtQuick
import QtQuick.Window

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("calculator")
    color: "#1a1a1a"

    // --- Calculator Logic ---
    property string expression: ""
    property bool isCalculated: false

    function handleInput(buttonValue) {
        if (buttonValue === "C") {
            expression = "";
        } else if (buttonValue === "=") {
            try {
                // Replace fancy symbols with real JS operators
                let formattedExpr = expression.replace(/×/g, "*").replace(/÷/g, "/");
                expression = eval(formattedExpr).toString();
                isCalculated = true;
            } catch (e) {
                expression = "Error";
            }
        } else {
            if (isCalculated) {
                expression = buttonValue; // Start fresh if typing after an "="
                isCalculated = false;
            } else {
                expression += buttonValue;
            }
        }
    }

    // --- Display Area ---
    Rectangle {
        id: displayID
        width: parent.width * .97
        height: parent.height * .25
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 10 // Added margin so it doesn't touch the top edge
        radius: 20
        color: "black"
        border.color: "#333333"
        border.width: 2

        Text {
            id: displayText
            text: expression === "" ? "0" : expression
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 20
            color: "white"
            font.pixelSize: 60
            font.weight: Font.Light
        }
    }

    // --- Button Area ---
    Rectangle {
        id: buttonArea
        width: parent.width * .9
        height: parent.height * .6
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#1a1a1a"

        Grid {
            anchors.fill: parent
            columns: 4
            spacing: 10

            // ROW 1
            Rectangle {
                id: btn7
                width: parent.width / 4 - 10
                height: parent.height / 4 - 10
                radius: 20
                color: "#333333"
                Text { text: "7"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("7") }
            }
            Rectangle {
                id: btn8
                width: parent.width / 4 - 10
                height: parent.height / 4 - 10
                radius: 20
                color: "#333333"
                Text { text: "8"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("8") }
            }
            Rectangle {
                id: btn9
                width: parent.width / 4 - 10
                height: parent.height / 4 - 10
                radius: 20
                color: "#333333"
                Text { text: "9"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("9") }
            }
            Rectangle {
                id: btnDivide
                width: parent.width / 4 - 10
                height: parent.height / 4 - 10
                radius: 20
                color: "#ff9500" // Fancy Orange
                Text { text: "÷"; anchors.centerIn: parent; color: "white"; font.pixelSize: 40; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("÷") }
            }

            // ROW 2
            Rectangle {
                id: btn4
                width: parent.width / 4 - 10
                height: parent.height / 4 - 10
                radius: 20
                color: "#333333"
                Text { text: "4"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("4") }
            }
            Rectangle {
                id: btn5
                width: parent.width / 4 - 10
                height: parent.height / 4 - 10
                radius: 20
                color: "#333333"
                Text { text: "5"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("5") }
            }
            Rectangle {
                id: btn6
                width: parent.width / 4 - 10
                height: parent.height / 4 - 10
                radius: 20
                color: "#333333"
                Text { text: "6"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("6") }
            }
            Rectangle {
                id: btnMultiply
                width: parent.width / 4 - 10
                height: parent.height / 4 - 10
                radius: 20
                color: "#ff9500"
                Text { text: "×"; anchors.centerIn: parent; color: "white"; font.pixelSize: 40; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("×") }
            }

            // ROW 3
            Rectangle {
                id: btn1
                width: parent.width / 4 - 10
                height: parent.height / 4 - 10
                radius: 20
                color: "#333333"
                Text { text: "1"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("1") }
            }
            Rectangle {
                id: btn2
                width: parent.width / 4 - 10
                height: parent.height / 4 - 10
                radius: 20
                color: "#333333"
                Text { text: "2"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("2") }
            }
            Rectangle {
                id: btn3
                width: parent.width / 4 - 10
                height: parent.height / 4 - 10
                radius: 20
                color: "#333333"
                Text { text: "3"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("3") }
            }
            Rectangle {
                id: btnMinus
                width: parent.width / 4 - 10
                height: parent.height / 4 - 10
                radius: 20
                color: "#ff9500"
                Text { text: "-"; anchors.centerIn: parent; color: "white"; font.pixelSize: 45; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("-") }
            }

            // ROW 4
            Rectangle {
                id: btnClear
                width: parent.width / 4 - 10
                height: parent.height / 4 - 10
                radius: 20
                color: "#ff5555" // Fancy Red
                Text { text: "C"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("C") }
            }
            Rectangle {
                id: btn0
                width: parent.width / 4 - 10
                height: parent.height / 4 - 10
                radius: 20
                color: "#333333"
                Text { text: "0"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("0") }
            }
            Rectangle {
                id: equalId
                width: parent.width / 4 - 10
                height: parent.height / 4 - 10
                radius: 20
                color: "#2ecc71" // Fancy Green
                Text { text: "="; anchors.centerIn: parent; color: "white"; font.pixelSize: 45; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("=") }
            }
            Rectangle {
                id: btnPlus
                width: parent.width / 4 - 10
                height: parent.height / 4 - 10
                radius: 20
                color: "#ff9500"
                Text { text: "+"; anchors.centerIn: parent; color: "white"; font.pixelSize: 40; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("+") }
            }
        }
    }
}
