/*
*  SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
*
*  SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick 2.12
import CustomElements 1.0

Item {
property alias color: shadowRectangle.color
property alias radius: shadowRectangle.radius
property alias shadow: shadowRectangle.shadow
property alias border: shadowRectangle.border
property alias corners: shadowRectangle.corners
property alias source: image.source

ShadowedTexture {
    id: shadowRectangle
    anchors.fill: parent

    source: Image {
        id: image
        visible: false
    }
}
}
