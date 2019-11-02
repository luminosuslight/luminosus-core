import QtQuick 2.5
import "."

CheckBox {
    property QtObject attr
    active: attr.val
    onActiveChanged: {
        if (active !== attr.val) {
            attr.val = active
        }
    }
    mappingID: attr.block().getUid ? attr.block().getUid() + attr.name() : ""
}
