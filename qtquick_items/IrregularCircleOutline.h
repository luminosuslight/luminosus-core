#ifndef IRREGULARCIRCLEOUTLINE_H
#define IRREGULARCIRCLEOUTLINE_H

#include <QQuickItem>


class IrregularCircleOutline : public QQuickItem {

    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(const QVector<double>& radii READ radii WRITE setRadii NOTIFY radiiChanged)
    Q_PROPERTY(double lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)

public:
    explicit IrregularCircleOutline(QQuickItem* parent = nullptr);
    ~IrregularCircleOutline();

    QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*);

signals:
    void colorChanged(const QColor& color);
    void radiiChanged();
    void lineWidthChanged();

public slots:
    void setColor(const QColor& color);
    QColor color() const { return m_color; }

    void setRadii(const QVector<double>& radii);
    const QVector<double>& radii() const { return m_radii; }

    void setLineWidth(const double width);
    double lineWidth() const { return m_lineWidth; }

private:
    QColor m_color;
    QVector<double> m_radii;
    double m_lineWidth;

    const double m_devicePixelRatio;
};

#endif // IRREGULARCIRCLEOUTLINE_H
