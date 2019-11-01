#ifndef CONVERSATIONACTIONINTERFACE_H
#define CONVERSATIONACTIONINTERFACE_H

#include "core/conversation/Command.h"
#include "core/conversation/UserInput.h"

#include <QString>

class ConversationActionInterface;


struct Suitability {
    Suitability(double s, int sp, QString mc)
        : suitability(s)
        , specificity(sp)
        , matchedPattern(mc)
    {}

    double suitability = 0.0;
    int specificity = 0;
    QString matchedPattern = "";
};


class ConversationActionInterface {

public:
    virtual ~ConversationActionInterface() {}

    virtual QString actionDescription() const = 0;

    virtual bool actionIsAvailable() const = 0;

    virtual Suitability actionSuitability(const Command* command) const = 0;

    virtual void performAction(Command* command) = 0;

    virtual void addMissingSynonym(QString phrase) = 0;
};

#endif // CONVERSATIONACTIONINTERFACE_H
