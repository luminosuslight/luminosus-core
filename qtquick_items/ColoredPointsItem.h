#ifndef COLOREDPOINTSITEM_H
#define COLOREDPOINTSITEM_H

#include <QtQuick/QQuickItem>


class ColoredPointsItem : public QQuickItem {

    Q_OBJECT

    Q_PROPERTY(QColor color1 READ color1 WRITE setColor1 NOTIFY color1Changed)
    Q_PROPERTY(QColor color2 READ color2 WRITE setColor2 NOTIFY color2Changed)
    Q_PROPERTY(double pointSize READ pointSize WRITE setPointSize NOTIFY pointSizeChanged)
    Q_PROPERTY(const QVector<double>& xPositions READ xPositions WRITE setXPositions NOTIFY positionsChanged)
    Q_PROPERTY(const QVector<double>& yPositions READ yPositions WRITE setYPositions NOTIFY positionsChanged)
    Q_PROPERTY(const QVector<double>& colorValues READ colorValues WRITE setColorValues NOTIFY colorValuesChanged)

public:
    explicit ColoredPointsItem(QQuickItem* parent = nullptr);
    ~ColoredPointsItem();

    QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*);

signals:
    void color1Changed(const QColor& color);
    void color2Changed(const QColor& color);
    void pointSizeChanged(double width);
    void positionsChanged();
    void colorValuesChanged();

public slots:
    void setColor1(const QColor& color);
    QColor color1() const { return m_color1; }

    void setColor2(const QColor& color);
    QColor color2() const { return m_color2; }

    double pointSize() const { return m_pointSize; }
    const QVector<double>& xPositions() const { return m_xPositions; }
    const QVector<double>& yPositions() const { return m_yPositions; }
    const QVector<double>& colorValues() const { return m_colorValues; }

    void setPointSize(double width);
    void setXPositions(const QVector<double>& values);
    void setYPositions(const QVector<double>& values);
    void setColorValues(const QVector<double>& values);

private:
    QColor m_color1;
    QColor m_color2;
    double m_pointSize;
    QVector<double> m_xPositions;
    QVector<double> m_yPositions;
    QVector<double> m_colorValues;

    const double m_devicePixelRatio;
};

#endif // COLOREDPOINTSITEM_H
