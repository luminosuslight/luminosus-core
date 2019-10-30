#ifndef CONVERSATIONLOGICBLOCK_H
#define CONVERSATIONLOGICBLOCK_H

#include "core/block_basics/InOutBlock.h"

#include <vector>

struct Command;


class ConversationLogicBlock : public InOutBlock {

    Q_OBJECT

public:
    explicit ConversationLogicBlock(MainController* controller, QString uid);

public slots:

    void addLogicResult(QString id, QString description="");
    void sendLogicResult(QString id, Command* command);
    void createLogicResultBlocks();

protected:
    std::vector<std::pair<QString, QString>> m_logicResults;
};

#endif // CONVERSATIONLOGICBLOCK_H
