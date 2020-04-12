#include "ConnectionCycleBlock.h"

#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"


bool ConnectionCycleBlock::s_registered = BlockList::getInstance().addBlock(ConnectionCycleBlock::info());

ConnectionCycleBlock::ConnectionCycleBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
{
    connect(m_inputNode, &NodeBase::dataChanged, this, [this]() {
        m_outputNode->setValue(m_inputNode->getValue());
    });
}
