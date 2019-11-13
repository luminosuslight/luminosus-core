#ifndef IrregularCircleOutlineTwoColoredTWOCOLORED_H
#define IrregularCircleOutlineTwoColoredTWOCOLORED_H

#include <QQuickItem>


class IrregularCircleOutlineTwoColored : public QQuickItem {

    Q_OBJECT

    Q_PROPERTY(QColor firstColor READ firstColor WRITE setFirstColor NOTIFY innerColorChanged)
    Q_PROPERTY(QColor secondColor READ secondColor WRITE setSecondColor NOTIFY outerColorChanged)
    Q_PROPERTY(const QVector<float>& radii READ radii WRITE setRadii NOTIFY radiiChanged)
    Q_PROPERTY(double lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)

public:
    explicit IrregularCircleOutlineTwoColored(QQuickItem* parent = nullptr);
    ~IrregularCircleOutlineTwoColored();

    QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*);

signals:
    void innerColorChanged(const QColor& color);
    void outerColorChanged(const QColor& color);
    void radiiChanged();
    void lineWidthChanged();

public slots:
    void setFirstColor(const QColor& color);
    QColor firstColor() const { return m_firstColor; }

    void setSecondColor(const QColor& color);
    QColor secondColor() const { return m_secondColor; }

    void setRadii(const QVector<float>& radii);
    const QVector<float>& radii() const { return m_radii; }

    void setLineWidth(const double width);
    double lineWidth() const { return m_lineWidth; }

private:
    QColor m_firstColor;
    QColor m_secondColor;
    QVector<float> m_radii;
    double m_lineWidth;

    const double m_devicePixelRatio;
};

#endif // IrregularCircleOutlineTwoColoredTWOCOLORED_H
