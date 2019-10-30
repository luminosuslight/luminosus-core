#include "IrregularCircleItem.h"

#include <QSGNode>
#include <QSGFlatColorMaterial>
#include <QSGVertexColorMaterial>
#include <QGuiApplication>
#include <QScreen>
#include <cmath>


IrregularCircleItem::IrregularCircleItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_innerColor(Qt::red)
    , m_outerColor(Qt::blue)
    , m_devicePixelRatio(QGuiApplication::primaryScreen()->devicePixelRatio())
{
    setFlag(ItemHasContents, true);
}

IrregularCircleItem::~IrregularCircleItem() {
}

void IrregularCircleItem::setInnerColor(const QColor& color) {
    if (m_innerColor == color)
        return;

    m_innerColor = color;
    emit innerColorChanged(color);
    update();
}

void IrregularCircleItem::setOuterColor(const QColor& color) {
    if (m_outerColor == color)
        return;

    m_outerColor = color;
    emit outerColorChanged(color);
    update();
}

void IrregularCircleItem::setRadii(const QVector<float>& radii) {
    m_radii = radii;
    emit radiiChanged();
    update();
}

QSGNode* IrregularCircleItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) {
    if (!isVisible()) return oldNode;

    const int pointCount = m_radii.size();
    if (pointCount < 3) return oldNode;

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
        QSGGeometry* geometry = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), 3);
        geometry->setDrawingMode(QSGGeometry::DrawTriangleFan);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        QSGVertexColorMaterial* material = new QSGVertexColorMaterial;
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
        parentNode->appendChildNode(node);
    }

    QSGGeometryNode* const qsgNode = static_cast<QSGGeometryNode*>(parentNode->childAtIndex(0));
    if (!qsgNode) {
        qCritical() << "[IrregularCircle] Could not get QSG Node.";
        return nullptr;
    }
    QSGGeometry* const geometry = qsgNode->geometry();
    if (!geometry) {
        qCritical() << "[IrregularCircle] Could not get QSG Geometry.";
        return nullptr;
    }

    const int interpolationFactor = 4;
    const int interpolatedRadiusCount = pointCount * interpolationFactor;
    const int verticesCount = interpolatedRadiusCount + 2;

    geometry->allocate(verticesCount);
    QSGGeometry::ColoredPoint2D* const vertices = geometry->vertexDataAsColoredPoint2D();

    if (!vertices) {
        qCritical() << "[IrregularCircle] Could not get QSG vertices.";
        return nullptr;
    }

    const float itemWidth = float(width());
    const float itemHeight = float(height());
    const float itemRadius = itemWidth / 2.0f;

    // draw irregular circle like this:
    // start at center, then bottom, then anti-clockwise, then repeat first radius to close
    vertices[0].set(itemWidth / 2, itemHeight / 2,
                    uchar(m_innerColor.red()), uchar(m_innerColor.green()), uchar(m_innerColor.blue()), uchar(m_innerColor.alpha()));

    const uchar r = uchar(m_outerColor.red() * m_outerColor.alphaF());
    const uchar g = uchar(m_outerColor.green() * m_outerColor.alphaF());
    const uchar b = uchar(m_outerColor.blue() * m_outerColor.alphaF());
    const uchar a = uchar(m_outerColor.alpha());

    for (int i = 0; i < pointCount; ++i) {
        for (int j = 0; j < interpolationFactor; ++j) {
            const float pos = float(j) / interpolationFactor;
            const float currentRadius = float((1-pos) * m_radii[i] + pos * m_radii[(i+1) % pointCount]);
            const int interpolatedRadiusIndex = i * interpolationFactor + j;
            const float angle = 2 * float(M_PI) * (float(interpolatedRadiusIndex) / interpolatedRadiusCount);
            const float x = currentRadius * itemRadius * std::sin(angle);
            const float y = currentRadius * itemRadius * std::cos(angle);

            vertices[1 + (i * interpolationFactor) + j].set(itemRadius + x, itemRadius + y, r, g, b, a);
        }
    }
    vertices[interpolatedRadiusCount + 1] = vertices[1];

    // tell Scene Graph that this items needs to be drawn:
    qsgNode->markDirty(QSGNode::DirtyGeometry);

    return parentNode;
}
