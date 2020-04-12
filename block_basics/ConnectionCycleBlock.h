#ifndef CONNECTIONCYCLEBLOCK_H
#define CONNECTIONCYCLEBLOCK_H

#include "core/block_basics/InOutBlock.h"


class ConnectionCycleBlock: public InOutBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Connection Cycle";
        info.category << "General" << "Groups / Projects";
        info.qmlFile = "qrc:/core/ui/blocks/ConnectionCycleBlock.qml";
        info.helpText = "Lets you create an intentional connection cycle\n"
                        "It overrides the cycle detection, so please ensure to not create "
                        "endless update loops.\n\n"
                        "It only forwards simple scalar values at the moment.";
        info.orderHint = 0 + 500 + 2;
        info.complete<ConnectionCycleBlock>();
        return info;
    }

    explicit ConnectionCycleBlock(CoreController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }
};

#endif // CONNECTIONCYCLEBLOCK_H
