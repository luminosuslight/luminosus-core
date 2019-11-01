#include "KineticEffect.h"

#include <QDebug>


KineticEffect::KineticEffect(QAbstractAnimation* parent)
    : QAbstractAnimation(parent)
	, m_history(KineticEffectConstants::DEFAULT_HISTORY_SIZE)
	, m_isManual(false)
	, m_origin(0.0)
	, m_velocity(0.0)
    , m_value(0.0)
    , m_minVelocity(3.0)  // min 3 pixel per second
    , m_friction(0.03)
	, m_lastUpdate(HighResTime::now())
	, m_minValue(std::numeric_limits<qreal>::lowest())
    , m_maxValue(std::numeric_limits<qreal>::max())
{
}

void KineticEffect::start(qreal value) {
	m_isManual = true;
	m_velocity = 0.0;
	m_origin = value;
	MoveEvent moveEvent {value, HighResTime::now()};
	m_history.push_back(moveEvent);
}

void KineticEffect::update(qreal value) {
	qreal lastValue = m_history.last().value;
	qreal distance = value - lastValue;
	applyDistance(distance);
	MoveEvent moveEvent {value, HighResTime::now()};
	m_history.push_back(moveEvent);
	emit moving(m_value, m_origin, m_isManual);
}

void KineticEffect::stop(qreal value) {
	m_isManual = false;
	qreal distance = value - m_history.last().value;
	applyDistance(distance);

	MoveEvent newestSample {value, HighResTime::now()};

	// find oldest sample in the last 10/60s:
	int index = 0;
	double shortTimeInSec = (10. / 60.);
	MoveEvent oldestSampleInShortTime = m_history[index];
	while (HighResTime::diff(newestSample.time, oldestSampleInShortTime.time) > shortTimeInSec) {
		if (index >= m_history.size() - 1) {
			// reached end of history
			break;
		}
		++index;
		oldestSampleInShortTime = m_history[index];
	}
	qreal totalDistance = newestSample.value - oldestSampleInShortTime.value;
	qreal durationInSec = HighResTime::diff(newestSample.time, oldestSampleInShortTime.time);
    m_velocity = totalDistance / qMax(durationInSec, 0.0001);
    // m_velocity unit is pixels per second
    // limit it to 8000 pixels per second initial velocity:
    m_velocity = limit(-8000, m_velocity, 8000);
	m_lastUpdate = newestSample.time;
	triggerVelocityUpdate();
}

void KineticEffect::cancel() {
	m_isManual = false;
	m_lastUpdate = m_history.last().time;
	triggerVelocityUpdate();
}

void KineticEffect::setValue(qreal value) {
    m_value = value;
}

void KineticEffect::updateCurrentTime(int /*currentTime*/)
{
    updateVelocity();
}

void KineticEffect::applyDistance(qreal distance) {
	m_value += distance;
	// check bounds:
	if (m_value < m_minValue) {
		m_value = m_minValue;
		m_velocity = 0.0;
	} else if (m_value > m_maxValue) {
		m_value = m_maxValue;
		m_velocity = 0.0;
	}
}

void KineticEffect::updateVelocity() {
	if (qAbs(m_velocity) <= m_minVelocity) {
		m_velocity = 0.0;
        QAbstractAnimation::stop();
		return;
	}
    double timeSinceLastUpdateInSec = HighResTime::getElapsedSecAndUpdate(m_lastUpdate);
    m_velocity -= m_velocity * m_friction * (timeSinceLastUpdateInSec / (1.0/50));
    applyDistance(m_velocity * timeSinceLastUpdateInSec);
    emit velocityChanged();
	emit moving(m_value, m_origin, m_isManual);
}

void KineticEffect::triggerVelocityUpdate() {
    QAbstractAnimation::start();
}
