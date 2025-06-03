import QtQuick 2.15

Rectangle {
    id: menu
    anchors {
        top: parent.top
        bottom: bottomBar.top
        left: parent.left
        right: parent.right
    }

    color: "#2d2d2d"

    SubMenu {
        id: subMenu
    }

    MenuList {
        id: menuList
    }
}
