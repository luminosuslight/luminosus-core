#ifndef IRREGULARCIRCLEITEM_H
#define IRREGULARCIRCLEITEM_H

#include <QtQuick/QQuickItem>


class IrregularCircleItem : public QQuickItem {

    Q_OBJECT

    Q_PROPERTY(QColor innerColor READ innerColor WRITE setInnerColor NOTIFY innerColorChanged)
    Q_PROPERTY(QColor outerColor READ outerColor WRITE setOuterColor NOTIFY outerColorChanged)
    Q_PROPERTY(const QVector<double>& radii READ radii WRITE setRadii NOTIFY radiiChanged)

public:
    explicit IrregularCircleItem(QQuickItem* parent = nullptr);
    ~IrregularCircleItem();

    QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*);

signals:
    void innerColorChanged(const QColor& color);
    void outerColorChanged(const QColor& color);
    void radiiChanged();

public slots:
    void setInnerColor(const QColor& color);
    QColor innerColor() const { return m_innerColor; }

    void setOuterColor(const QColor& color);
    QColor outerColor() const { return m_outerColor; }

    void setRadii(const QVector<double>& radii);
    const QVector<double>& radii() const { return m_radii; }

private:
    QColor m_innerColor;
    QColor m_outerColor;
    QVector<double> m_radii;

    const double m_devicePixelRatio;
};

#endif // IRREGULARCIRCLEITEM_H
