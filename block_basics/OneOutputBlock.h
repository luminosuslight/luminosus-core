#ifndef ONEOUTPUTBLOCK_H
#define ONEOUTPUTBLOCK_H

#include "BlockBase.h"
#include <QPointer>

// forward declaration to reduce dependencies
class NodeBase;

/**
 * @brief The OneOutputBlock class is the base of all simple blocks with one output node.
 * It has a "value" property because most of these blocks generate a single value.
 */
class OneOutputBlock : public BlockBase {

    Q_OBJECT

    Q_PROPERTY(double value READ getValue WRITE setValue NOTIFY valueChanged)

public:

	/**
	 * @brief OneOutputBlock creates a Block instance with one output node
	 * @param controller a pointer to the main controller
     * @param uid of this block or nothing if this block is new
	 */
    explicit OneOutputBlock(CoreController* controller, QString uid);

	virtual void getAdditionalState(QCborMap& state) const override;

	virtual void setAdditionalState(const QCborMap& state) override;

signals:
	/**
	 * @brief valueChanged is emitted when the value changes
	 */
    void valueChanged();

public slots:
	/**
	 * @brief getValue returns the value (stored in the output node)
	 * @return the value [0...1]
	 */
	double getValue() const;
	/**
	 * @brief setValue sets the value (it is stored in the output node)
	 * @param value to set
	 */
	void setValue(double value);

    /**
     * @brief forceUpdateOutput forces updating the values in the output node
     * even if value is identical (but matrices could be invalid due to rescale)
     */
    void forceUpdateOutput();

protected:
	/**
	 * @brief m_outputNode is a pointer to the created output node
	 */
    QPointer<NodeBase> m_outputNode;

    double m_value;

};

#endif // ONEOUTPUTBLOCK_H
