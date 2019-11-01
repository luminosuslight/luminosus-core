#include "OneInputBlock.h"

#include "core/connections/Nodes.h"  // for QPointer<Node>

OneInputBlock::OneInputBlock(CoreController* controller, QString uid)
	: BlockBase(controller, uid)
{
	// prepare nodes:
	m_inputNode = createInputNode("inputNode");
}
