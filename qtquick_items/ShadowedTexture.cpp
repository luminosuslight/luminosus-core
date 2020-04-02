/*
 *  SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ShadowedTexture.h"

#include <QQuickWindow>
#include <QSGRendererInterface>
#include <QSGRectangleNode>

#include "scenegraph/shadowedtexturenode.h"

ShadowedTexture::ShadowedTexture(QQuickItem *parentItem)
    : ShadowedRectangle(parentItem)
{
}

ShadowedTexture::~ShadowedTexture()
{
}

QQuickItem *ShadowedTexture::source() const
{
    return m_source;
}

void ShadowedTexture::setSource(QQuickItem *newSource)
{
    if (newSource == m_source) {
        return;
    }

    m_source = newSource;
    if (!m_source->parentItem()) {
        m_source->setParentItem(this);
    }

    update();
    Q_EMIT sourceChanged();
}

QSGNode *ShadowedTexture::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *data)
{
    Q_UNUSED(data)

    if (!node) {
        node = new ShadowedTextureNode{};
    }

    auto shadowNode = static_cast<ShadowedTextureNode*>(node);
    shadowNode->setBorderEnabled(border()->isEnabled());
    shadowNode->setRect(boundingRect());
    shadowNode->setSize(shadow()->size());
    shadowNode->setRadius(corners()->toVector4D(radius()));
    shadowNode->setOffset(QVector2D{float(shadow()->xOffset()), float(shadow()->yOffset())});
    shadowNode->setColor(color());
    shadowNode->setShadowColor(shadow()->color());
    shadowNode->setBorderWidth(border()->width());
    shadowNode->setBorderColor(border()->color());

    if (m_source) {
        shadowNode->setTextureSource(m_source->textureProvider());
    }

    shadowNode->updateGeometry();
    return shadowNode;
}
