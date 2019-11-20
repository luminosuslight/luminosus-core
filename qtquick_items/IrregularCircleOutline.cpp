#include "IrregularCircleOutline.h"

#include <QSGNode>
#include <QSGFlatColorMaterial>
#include <QGuiApplication>
#include <QScreen>
#include <cmath>


IrregularCircleOutline::IrregularCircleOutline(QQuickItem *parent)
    : QQuickItem(parent)
    , m_color(Qt::red)
    , m_lineWidth(2.0)
    , m_devicePixelRatio(QGuiApplication::primaryScreen()->devicePixelRatio())
{
    setFlag(ItemHasContents, true);
}

IrregularCircleOutline::~IrregularCircleOutline() {
}

void IrregularCircleOutline::setColor(const QColor& color) {
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged(color);
    update();
}

void IrregularCircleOutline::setRadii(const QVector<float>& radii) {
    m_radii = radii;
    emit radiiChanged();
    update();
}

void IrregularCircleOutline::setLineWidth(double width) {
    if (m_lineWidth == width)
        return;

    m_lineWidth = width;
    emit lineWidthChanged();
    update();
}

QSGNode* IrregularCircleOutline::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) {
    if (!isVisible()) return oldNode;

    const int pointCount = m_radii.size();
    if (pointCount < 3) return oldNode;

    const int interpolationFactor = 4;
    const int interpolatedRadiusCount = pointCount * interpolationFactor;
    const int verticesCount = interpolatedRadiusCount;

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
        geometry->setDrawingMode(QSGGeometry::DrawLineLoop);
        geometry->setLineWidth(float(m_lineWidth * m_devicePixelRatio));
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        QSGFlatColorMaterial* material = new QSGFlatColorMaterial;
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
        parentNode->appendChildNode(node);
    }

    QSGGeometryNode* const qsgNode = static_cast<QSGGeometryNode*>(parentNode->childAtIndex(0));
    Q_ASSERT_X(qsgNode, "IrregularCircleOutline::updatePaintNode", "Could not get QSG Node.");

    QSGFlatColorMaterial* const material = static_cast<QSGFlatColorMaterial*>(qsgNode->material());
    Q_ASSERT_X(material, "IrregularCircleOutline::updatePaintNode", "Could not get QSG Material.");
    material->setColor(m_color);

    QSGGeometry* const geometry = qsgNode->geometry();
    Q_ASSERT_X(geometry, "IrregularCircleOutline::updatePaintNode", "Could not get QSG Geometry.");
    geometry->setLineWidth(float(m_lineWidth * m_devicePixelRatio));

    geometry->allocate(verticesCount);
    QSGGeometry::Point2D* const vertices = geometry->vertexDataAsPoint2D();

    Q_ASSERT_X(vertices, "IrregularCircleOutline::updatePaintNode", "Could not get QSG vertices.");

    const float itemWidth = float(width());
    const float itemRadius = itemWidth / 2.0f;

    // draw irregular circle like this:
    // start at the bottom, then anti-clockwise

    for (int i = 0; i < pointCount; ++i) {
        for (int j = 0; j < interpolationFactor; ++j) {
            const float pos = float(j) / interpolationFactor;
            const float currentRadius = float((1-pos) * m_radii[i] + pos * m_radii[(i+1) % pointCount]);
            const int interpolatedRadiusIndex = i * interpolationFactor + j;
            const float angle = 2 * float(M_PI) * (float(interpolatedRadiusIndex) / interpolatedRadiusCount);
            const float x = currentRadius * itemRadius * std::sin(angle);
            const float y = currentRadius * itemRadius * std::cos(angle);

            const int idx = (i * interpolationFactor) + j;
            vertices[idx].set(itemRadius + x, itemRadius + y);
        }
    }

    // tell Scene Graph that this items needs to be drawn:
    qsgNode->markDirty(QSGNode::DirtyGeometry);

    return parentNode;
}
