#include "Engine.h"


Engine::Engine(QObject* parent, int fps)
	: QObject(parent)
	, m_timer(this)
	, m_fps(fps)
{
	m_lastFrameTime = HighResTime::now();
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
}

void Engine::start() {
    m_timer.start(1000 / m_fps);
}

void Engine::stop() {
    m_timer.stop();
}

void Engine::tick() {
    // calculate time once last frame:
    const double timeSinceLastFrame = HighResTime::getElapsedSecAndUpdate(m_lastFrameTime);

	// call signals in logical order:
	emit updateBlocks(timeSinceLastFrame);
	emit updateOutput(timeSinceLastFrame);
}
