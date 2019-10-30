#ifndef SPECTRUMITEM_H
#define SPECTRUMITEM_H

#include <QtQuick/QQuickItem>
#include <QVector2D>
#include <QPointer>


class SpectrumItem : public QQuickItem {

    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(double lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)
    Q_PROPERTY(const QVector<double>& points READ points WRITE setPoints NOTIFY pointsChanged)

public:
    explicit SpectrumItem(QQuickItem* parent = nullptr);
    ~SpectrumItem();

    QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*);

signals:
    void colorChanged(const QColor& color);
    void lineWidthChanged(double width);
    void pointsChanged();

public slots:
    QColor color() const { return m_color; }
    double lineWidth() const { return m_lineWidth; }
    const QVector<double>& points() const { return m_points; }

    void setColor(const QColor& color);
    void setLineWidth(double width);
    void setPoints(const QVector<double> value);

private:
    static QPointF normalFromTangent(const QPointF& tangent);

    QColor m_color;
    double m_lineWidth;
    QVector<double> m_points;

    const double m_device_pixel_ratio;
};

#endif // SPECTRUMITEM_H
