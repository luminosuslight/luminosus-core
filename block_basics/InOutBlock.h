#ifndef INOUTBLOCK_H
#define INOUTBLOCK_H

#include "BlockBase.h"
#include <QPointer>

// forward declaration to reduce dependencies
class NodeBase;

/**
 * @brief The InOutBlock class is the base of all simple blocks with one in- and one output node.
 */
class InOutBlock : public BlockBase {

    Q_OBJECT

public:

	/**
	 * @brief InOutBlock creates a Block instance with one in- and one output node
	 * @param controller a pointer to the main controller
     * @param uid of this block or nothing if this block is new
	 */
    explicit InOutBlock(MainController* controller, QString uid);

protected:
	/**
	 * @brief m_outputNode is a pointer to the created output node
	 */
    QPointer<NodeBase> m_outputNode;
	/**
	 * @brief m_inputNode is a pointer to the created input node
	 */
    QPointer<NodeBase> m_inputNode;

};

#endif // INOUTBLOCK_H
