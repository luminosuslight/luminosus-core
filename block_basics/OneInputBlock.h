#ifndef ONEINPUTBLOCK_H
#define ONEINPUTBLOCK_H

#include "BlockBase.h"
#include <QPointer>

// forward declaration to reduce dependencies
class NodeBase;

/**
 * @brief The OneInputBlock class is the base of all simple blocks with one input node.
 */
class OneInputBlock : public BlockBase {

    Q_OBJECT

public:

	/**
	 * @brief OneInputBlock creates a Block instance with one input node
	 * @param controller a pointer to the main controller
     * @param uid of this block or nothing if this block is new
	 */
    explicit OneInputBlock(CoreController* controller, QString uid);

protected:
	/**
	 * @brief m_inputNode is a pointer to the created input node
	 */
    QPointer<NodeBase> m_inputNode;

};


#endif // ONEINPUTBLOCK_H
