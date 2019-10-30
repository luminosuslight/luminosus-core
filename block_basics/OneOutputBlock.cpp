#include "OneOutputBlock.h"

#include "core/Nodes.h"


OneOutputBlock::OneOutputBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
    , m_value(0.0)
{
	// prepare nodes:
	m_outputNode = createOutputNode("outputNode");

    connect(m_outputNode, SIGNAL(requestedSizeChanged()), this, SLOT(forceUpdateOutput()));
}

void OneOutputBlock::getAdditionalState(QJsonObject& state) const {
    state["value"] = getValue();
}

void OneOutputBlock::setAdditionalState(const QJsonObject& state) {
	setValue(state["value"].toDouble());
}

double OneOutputBlock::getValue() const {
    return m_value;
}

void OneOutputBlock::setValue(double value) {
    m_value = value;
    emit valueChanged();
    m_outputNode->setValue(m_value);
}

void OneOutputBlock::forceUpdateOutput() {
    m_outputNode->setHsv(0, 0, m_value);
}
