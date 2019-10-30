#ifndef POINTSITEM_H
#define POINTSITEM_H

#include <QtQuick/QQuickItem>


class PointsItem : public QQuickItem {

    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(double pointSize READ pointSize WRITE setPointSize NOTIFY pointSizeChanged)
    Q_PROPERTY(const QVector<double>& xPositions READ xPositions WRITE setXPositions NOTIFY positionsChanged)
    Q_PROPERTY(const QVector<double>& yPositions READ yPositions WRITE setYPositions NOTIFY positionsChanged)

public:
    explicit PointsItem(QQuickItem* parent = nullptr);
    ~PointsItem();

    QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*);

signals:
    void colorChanged(const QColor& color);
    void pointSizeChanged(double width);
    void positionsChanged();

public slots:
    QColor color() const { return m_color; }
    double pointSize() const { return m_pointSize; }
    const QVector<double>& xPositions() const { return m_xPositions; }
    const QVector<double>& yPositions() const { return m_yPositions; }

    void setColor(const QColor& color);
    void setPointSize(double width);
    void setXPositions(const QVector<double>& values);
    void setYPositions(const QVector<double>& values);

private:
    QColor m_color;
    double m_pointSize;
    QVector<double> m_xPositions;
    QVector<double> m_yPositions;

    const double m_devicePixelRatio;
};

#endif // POINTSITEM_H
