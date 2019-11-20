#include "PointsItem.h"

#include <QtQuick/qsgnode.h>
#include <QtQuick/qsgflatcolormaterial.h>
#include <QGuiApplication>
#include <QScreen>
#include <cmath>


PointsItem::PointsItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_color("blue")
    , m_pointSize(3)
    , m_devicePixelRatio(QGuiApplication::primaryScreen()->devicePixelRatio())
{
    setFlag(ItemHasContents, true);
}

PointsItem::~PointsItem() {
}

void PointsItem::setColor(const QColor &color) {
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged(color);
    update();
}

void PointsItem::setPointSize(double width) {
    if (m_pointSize == width) return;

    m_pointSize = width;
    emit pointSizeChanged(width);
    update();
}

void PointsItem::setXPositions(const QVector<double>& values) {
    m_xPositions = values;
    emit positionsChanged();
    update();
}

void PointsItem::setYPositions(const QVector<double>& values) {
    m_yPositions = values;
    emit positionsChanged();
    // TODO: check if update is batched / delayed to not update twice
    update();
}

QSGNode* PointsItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) {
    if (!isVisible()) return oldNode;

    const int pointCount = std::min(m_xPositions.size(), m_yPositions.size());
    const int verticesCount = pointCount;

    // -------------------- Prepare QSG Nodes:
    QSGNode* parentNode = nullptr;
    if (oldNode) {
        parentNode = static_cast<QSGNode*>(oldNode);
    } else {
        parentNode = new QSGNode;
    }
    // adapt child count:
    int childCount = parentNode->childCount();
    if (childCount != 1) {
        parentNode->removeAllChildNodes();
        QSGGeometryNode* node = new QSGGeometryNode;
        QSGGeometry* geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), verticesCount);
        geometry->setDrawingMode(QSGGeometry::DrawPoints);
        geometry->setLineWidth(float(m_pointSize * m_devicePixelRatio));
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        QSGFlatColorMaterial* material = new QSGFlatColorMaterial;
        material->setColor(m_color);
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
        parentNode->appendChildNode(node);
    }

    QSGGeometryNode* const qsgNode = static_cast<QSGGeometryNode*>(parentNode->childAtIndex(0));
    if (!qsgNode) {
        qCritical() << "[PointsItem] Could not get QSG Node.";
        return nullptr;
    }

    QSGFlatColorMaterial* const material = static_cast<QSGFlatColorMaterial*>(qsgNode->material());
    if (!material) {
        qCritical() << "[PointsItem] Could not get QSG Node.";
        return nullptr;
    }
    material->setColor(m_color);
    QSGGeometry* const geometry = qsgNode->geometry();
    if (!geometry) {
        qCritical() << "[PointsItem] Could not get QSG Geometry.";
        return nullptr;
    }

    geometry->allocate(verticesCount);
    QSGGeometry::Point2D* const vertices = geometry->vertexDataAsPoint2D();

    if (!vertices) {
        qCritical() << "[PointsItem] Could not get QSG vertices.";
        return nullptr;
    }

    const float itemWidth = float(width());
    const float itemHeight = float(height());

    // draw points:
    for (int i = 0; i < pointCount; ++i) {
        const float x = itemWidth * float(m_xPositions[i]);
        const float y = itemHeight * float(m_yPositions[i]);

        vertices[i].set(x, y);
    }

    // tell Scene Graph that this items needs to be drawn:
    qsgNode->markDirty(QSGNode::DirtyGeometry);

    return parentNode;
}
