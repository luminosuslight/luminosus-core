#include "IrregularCircleOutlineTwoColored.h"

#include <QSGNode>
#include <QSGFlatColorMaterial>
#include <QSGVertexColorMaterial>
#include <QGuiApplication>
#include <QScreen>
#include <cmath>


IrregularCircleOutlineTwoColored::IrregularCircleOutlineTwoColored(QQuickItem *parent)
    : QQuickItem(parent)
    , m_firstColor(Qt::red)
    , m_secondColor(Qt::blue)
    , m_lineWidth(2.0)
    , m_devicePixelRatio(QGuiApplication::primaryScreen()->devicePixelRatio())
{
    setFlag(ItemHasContents, true);
}

IrregularCircleOutlineTwoColored::~IrregularCircleOutlineTwoColored() {
}

void IrregularCircleOutlineTwoColored::setFirstColor(const QColor& color) {
    if (m_firstColor == color)
        return;

    m_firstColor = color;
    emit innerColorChanged(color);
    update();
}

void IrregularCircleOutlineTwoColored::setSecondColor(const QColor& color) {
    if (m_secondColor == color)
        return;

    m_secondColor = color;
    emit outerColorChanged(color);
    update();
}

void IrregularCircleOutlineTwoColored::setRadii(const QVector<float>& radii) {
    m_radii = radii;
    emit radiiChanged();
    update();
}

void IrregularCircleOutlineTwoColored::setLineWidth(double width) {
    if (m_lineWidth == width)
        return;

    m_lineWidth = width;
    emit lineWidthChanged();
    update();
}

QSGNode* IrregularCircleOutlineTwoColored::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) {
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
        geometry->setDrawingMode(QSGGeometry::DrawLineLoop);
        geometry->setLineWidth(float(m_lineWidth * m_devicePixelRatio));
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
    const int verticesCount = interpolatedRadiusCount;

    geometry->allocate(verticesCount);
    QSGGeometry::ColoredPoint2D* const vertices = geometry->vertexDataAsColoredPoint2D();

    if (!vertices) {
        qCritical() << "[IrregularCircle] Could not get QSG vertices.";
        return nullptr;
    }

    const float itemWidth = float(width());
    const float itemRadius = itemWidth / 2.0f;

    // draw irregular circle like this:
    // start at the bottom, then anti-clockwise

    const uchar r = uchar(m_secondColor.red() * m_secondColor.alphaF());
    const uchar g = uchar(m_secondColor.green() * m_secondColor.alphaF());
    const uchar b = uchar(m_secondColor.blue() * m_secondColor.alphaF());
    const uchar a = uchar(m_secondColor.alpha());

    const uchar r2 = uchar(m_firstColor.red() * m_firstColor.alphaF());
    const uchar g2 = uchar(m_firstColor.green() * m_firstColor.alphaF());
    const uchar b2 = uchar(m_firstColor.blue() * m_firstColor.alphaF());
    const uchar a2 = uchar(m_firstColor.alpha());

    for (int i = 0; i < pointCount; ++i) {
        for (int j = 0; j < interpolationFactor; ++j) {
            const float pos = float(j) / interpolationFactor;
            const float currentRadius = float((1-pos) * m_radii[i] + pos * m_radii[(i+1) % pointCount]);
            const int interpolatedRadiusIndex = i * interpolationFactor + j;
            const float angle = 2 * float(M_PI) * (float(interpolatedRadiusIndex) / interpolatedRadiusCount);
            const float x = currentRadius * itemRadius * std::sin(angle);
            const float y = currentRadius * itemRadius * std::cos(angle);

            const int idx = (i * interpolationFactor) + j;
            if ((idx / 3) % 2) {
                vertices[idx].set(itemRadius + x, itemRadius + y, r, g, b, a);
            } else {
                vertices[idx].set(itemRadius + x, itemRadius + y, r2, g2, b2, a2);
            }
        }
    }

    // tell Scene Graph that this items needs to be drawn:
    qsgNode->markDirty(QSGNode::DirtyGeometry);

    return parentNode;
}
