#include "ConversationLogicBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"
#include "block_implementations/Conversation/LogicResultBlock.h"
#include "conversation/Command.h"


ConversationLogicBlock::ConversationLogicBlock(MainController *controller, QString uid)
    : InOutBlock(controller, uid)
{

}

void ConversationLogicBlock::addLogicResult(QString id, QString description) {
    m_logicResults.push_back({id, description});
}

void ConversationLogicBlock::sendLogicResult(QString id, Command* command) {
    if (!command) {
        qWarning() << "ConversationLogicBlock::sendLogicResult: missing command";
        return;
    }
    UserInput& input = command->currentInput();
    input.m_logicResultId = id;
    m_outputNode->sendCommand(command);
}

void ConversationLogicBlock::createLogicResultBlocks() {
    for (auto message: m_logicResults) {
        m_outputNode->onTouched();
        LogicResultBlock* block = qobject_cast<LogicResultBlock*>(m_controller->blockManager()->addNewBlock("Logic Result"));
        if (!block) {
            qWarning() << "ConversationLogicBlock: Could not create Logic Result Block.";
            return;
        }
        block->m_id = message.first;
        block->m_description = message.second;
    }
}
