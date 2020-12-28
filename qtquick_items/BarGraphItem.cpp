#include "BarGraphItem.h"

#include <QtQuick/qsgnode.h>
#include <QtQuick/qsgflatcolormaterial.h>
#include <QQuickWindow>
#include <QGuiApplication>
#include <QScreen>
#include <cmath>


BarGraphItem::BarGraphItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_color("blue")
    , m_lineWidth(3)
    , m_device_pixel_ratio(QGuiApplication::primaryScreen()->devicePixelRatio())
{
    setFlag(ItemHasContents, true);
}

BarGraphItem::~BarGraphItem() {
}

void BarGraphItem::setColor(const QColor &color) {
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged(color);
    update();
}

void BarGraphItem::setLineWidth(double width) {
    if (m_lineWidth == width) return;

    m_lineWidth = width;
    emit lineWidthChanged(width);
    update();
}

void BarGraphItem::setPoints(const QVector<double> value) {
    m_points = value;
    emit pointsChanged();
    update();
}

QPointF BarGraphItem::normalFromTangent(const QPointF& tangent) {
    // returns a normalized normal vector given a tangent vector
    if (tangent.manhattanLength() == 0.0) return QPointF(0, 0);
    QPointF n(tangent.y(), tangent.x() * (-1));
    n /= double(QVector2D(n).length());
    return n;
}

QSGNode* BarGraphItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) {
    if (!isVisible()) return oldNode;

    const int pointCount = m_points.size();
    if (pointCount < 2) return oldNode;

    // -------------------- Prepare QSG Nodes:
    QSGNode* parentNode = nullptr;
    if (oldNode) {
        parentNode = static_cast<QSGNode*>(oldNode);
    } else {
        parentNode = new QSGNode;
    }
    // adapt child count:
    int childCount = parentNode->childCount();
    if (childCount != 2) {
        parentNode->removeAllChildNodes();
        QSGGeometryNode* node = new QSGGeometryNode;
        QSGGeometry* geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 3);
        geometry->setDrawingMode(QSGGeometry::DrawTriangleStrip);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        QSGFlatColorMaterial* material = new QSGFlatColorMaterial;
        material->setColor("#000");
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
        parentNode->appendChildNode(node);

        // Attention: TODO: colors are swapped!

        node = new QSGGeometryNode;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 3);
        geometry->setDrawingMode(QSGGeometry::DrawTriangleStrip);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        material = new QSGFlatColorMaterial;
        material->setColor(m_color);
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
        parentNode->appendChildNode(node);
    }

    QSGGeometryNode* const qsgNode = static_cast<QSGGeometryNode*>(parentNode->childAtIndex(0));
    QSGGeometryNode* const qsgNodeOutline = static_cast<QSGGeometryNode*>(parentNode->childAtIndex(1));
    if (!qsgNode || !qsgNodeOutline) {
        qCritical() << "[SpectrumItem] Could not get QSG Node.";
        return nullptr;
    }
    QSGGeometry* const geometry = qsgNode->geometry();
    QSGGeometry* const geometryOutline = qsgNodeOutline->geometry();
    if (!geometry || !geometryOutline) {
        qCritical() << "[SpectrumItem] Could not get QSG Geometry.";
        return nullptr;
    }

    const int verticesCount = pointCount * 7;
    const int outlineVerticesCount = pointCount * 7;

    geometry->allocate(verticesCount);
    geometryOutline->allocate(outlineVerticesCount);
    QSGGeometry::Point2D* const vertices = geometry->vertexDataAsPoint2D();
    QSGGeometry::Point2D* const verticesOutline = geometryOutline->vertexDataAsPoint2D();

    if (!vertices || !verticesOutline) {
        qCritical() << "[SpectrumItem] Could not get QSG vertices.";
        return nullptr;
    }

    const float itemWidth = float(width());
    const float itemHeight = float(height());
    const float barWidth = (itemWidth / pointCount) * 0.4f;
    const float spaceWidth = (itemWidth / pointCount) * 0.6f;
    const float endLineHeight = itemHeight / 100.0f;

    // draw spectrum:
    for (int i = 0; i < pointCount; ++i) {
        const float x = itemWidth * (i / float(pointCount - 1));
        const float y = itemHeight * (1 - float(m_points[i]));

        vertices[i*7].set(x, itemHeight);
        vertices[i*7+1].set(x, y);
        vertices[i*7+2].set(x + barWidth, itemHeight);
        vertices[i*7+3].set(x + barWidth, y);
        vertices[i*7+4].set(x + barWidth, itemHeight);
        vertices[i*7+5].set(x + barWidth, itemHeight);
        vertices[i*7+6].set(x + barWidth + spaceWidth, itemHeight);

        const float y2 = std::min(itemHeight, itemHeight * (1 - float(m_points[i])) + endLineHeight);

        verticesOutline[i*7].set(x, itemHeight);
        verticesOutline[i*7+1].set(x, y2);
        verticesOutline[i*7+2].set(x + barWidth, itemHeight);
        verticesOutline[i*7+3].set(x + barWidth, y2);
        verticesOutline[i*7+4].set(x + barWidth, itemHeight);
        verticesOutline[i*7+5].set(x + barWidth, itemHeight);
        verticesOutline[i*7+6].set(x + barWidth + spaceWidth, itemHeight);
    }

    // tell Scene Graph that this items needs to be drawn:
    qsgNode->markDirty(QSGNode::DirtyGeometry);
    qsgNodeOutline->markDirty(QSGNode::DirtyGeometry);

    return parentNode;
}
