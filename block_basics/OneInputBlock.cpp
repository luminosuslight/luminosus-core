#include "OneInputBlock.h"

#include "core/Nodes.h"  // for QPointer<Node>

OneInputBlock::OneInputBlock(MainController* controller, QString uid)
	: BlockBase(controller, uid)
{
	// prepare nodes:
	m_inputNode = createInputNode("inputNode");
}
