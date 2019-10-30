#include "InOutBlock.h"

#include "core/connections/Nodes.h"  // for QPointer<NodeBase>

InOutBlock::InOutBlock(MainController* controller, QString uid)
	: BlockBase(controller, uid)
{
	// prepare nodes:
	m_outputNode = createOutputNode("outputNode");
	m_inputNode = createInputNode("inputNode");
}
