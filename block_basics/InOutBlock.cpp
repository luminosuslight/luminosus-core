#include "InOutBlock.h"

#include "core/connections/Nodes.h"  // for QPointer<NodeBase>

InOutBlock::InOutBlock(CoreController* controller, QString uid)
	: BlockBase(controller, uid)
{
	// prepare nodes:
	m_outputNode = createOutputNode("outputNode");
	m_inputNode = createInputNode("inputNode");
}
