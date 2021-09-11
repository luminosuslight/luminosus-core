#include "ColoredPointsItem.h"

#include <QSGNode>
#include <QSGVertexColorMaterial>
#include <QGuiApplication>
#include <QScreen>


ColoredPointsItem::ColoredPointsItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_color1("blue")
    , m_color2("white")
    , m_pointSize(3)
    , m_devicePixelRatio(QGuiApplication::primaryScreen()->devicePixelRatio())
{
    setFlag(ItemHasContents, true);
}

ColoredPointsItem::~ColoredPointsItem() {
}

void ColoredPointsItem::setColor1(const QColor& color) {
    if (m_color1 == color)
        return;

    m_color1 = color;
    emit color1Changed(color);
    update();
}

void ColoredPointsItem::setColor2(const QColor& color) {
    if (m_color2 == color)
        return;

    m_color2 = color;
    emit color2Changed(color);
    update();
}

void ColoredPointsItem::setPointSize(double width) {
    if (m_pointSize == width) return;

    m_pointSize = width;
    emit pointSizeChanged(width);
    update();
}

void ColoredPointsItem::setGamma(double value) {
    if (m_gamma == value) return;

    m_gamma = value;
    emit gammaChanged(value);
    update();
}

void ColoredPointsItem::setXPositions(const QVector<double>& values) {
    m_xPositions = values;
    emit positionsChanged();
    update();
}

void ColoredPointsItem::setYPositions(const QVector<double>& values) {
    m_yPositions = values;
    emit positionsChanged();
    // TODO: check if update is batched / delayed to not update twice
    update();
}

void ColoredPointsItem::setColorValues(const QVector<double>& values) {
    m_colorValues = values;
    emit colorValuesChanged();
    update();
}

QSGNode* ColoredPointsItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) {
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
        QSGGeometry* geometry = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), std::max(1, verticesCount));
        geometry->setDrawingMode(QSGGeometry::DrawPoints);
        geometry->setLineWidth(float(m_pointSize * m_devicePixelRatio));
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        QSGVertexColorMaterial* material = new QSGVertexColorMaterial;
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
        parentNode->appendChildNode(node);
    }

    QSGGeometryNode* const qsgNode = static_cast<QSGGeometryNode*>(parentNode->childAtIndex(0));
    if (!qsgNode) {
        qCritical() << "[ColoredPointsItem] Could not get QSG Node.";
        return nullptr;
    }
    QSGGeometry* const geometry = qsgNode->geometry();
    if (!geometry) {
        qCritical() << "[ColoredPointsItem] Could not get QSG Geometry.";
        return nullptr;
    }
    geometry->setLineWidth(float(m_pointSize * m_devicePixelRatio));

    geometry->allocate(verticesCount);
    QSGGeometry::ColoredPoint2D* const vertices = geometry->vertexDataAsColoredPoint2D();

    if (!vertices) {
        qCritical() << "[ColoredPointsItem] Could not get QSG vertices.";
        return nullptr;
    }

    const float itemWidth = float(width());
    const float itemHeight = float(height());

    const double s1 = m_color1.saturationF() * m_color1.alphaF();
    const double h1 = s1 > 0.0 ? m_color1.hueF() * m_color1.alphaF() : m_color2.hueF() * m_color1.alphaF();
    const double v1 = m_color1.valueF() * m_color1.alphaF();
    const double a1 = m_color1.alphaF();

    const double s2 = m_color2.saturationF() * m_color2.alphaF();
    const double h2 = s2 > 0.0 ? m_color2.hueF() * m_color2.alphaF() : m_color1.hueF() * m_color2.alphaF();
    const double v2 = m_color2.valueF() * m_color2.alphaF();
    const double a2 = m_color2.alphaF();

    // draw points:
    for (int i = 0; i < pointCount; ++i) {
        const float x = itemWidth * float(m_xPositions[i]);
        const float y = itemHeight * float(m_yPositions[i]);

        // c is a ratio between 0-1 to define the position between the two colors
        // c == 0.0 means color1, 1.0 means color2 and 0.5 means exactly in the middle
        // of a HSV fade between both
        const double c = std::pow(m_colorValues[i], m_gamma);
        const double cInv = 1.0 - c;

        QColor pointColor = QColor::fromHsvF(h1 * cInv + h2 * c,
                                         s1 * cInv + s2 * c,
                                         v1 * cInv + v2 * c,
                                         a1 * cInv + a2 * c);

        vertices[i].set(x, y, uchar(pointColor.red()),
                        uchar(pointColor.green()),
                        uchar(pointColor.blue()),
                        uchar(pointColor.alpha()));
    }

    // tell Scene Graph that this items needs to be drawn:
    qsgNode->markDirty(QSGNode::DirtyGeometry);

    return parentNode;
}
