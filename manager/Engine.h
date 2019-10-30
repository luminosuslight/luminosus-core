#ifndef ENGINE_H
#define ENGINE_H

#include "utils.h"

#include <QObject>
#include <QTimer>
#include <chrono>


/**
 * @brief The Engine class defines an engine that is responsible to trigger all non-GUI actions
 * that have to be done regulary (i.e. block logic and data output).
 * It generates different signals with a specified FPS rate.
 *
 * It is independent from the GUI. Actions that affect the GUI should be triggered by QTimer.
 */
class Engine : public QObject
{
	Q_OBJECT

public:
	/**
	 * @brief Engine creates an engine instance
	 * @param fps the amount of frames per second to generate
	 * @param parent QObject parent
	 */
    explicit Engine(QObject* parent = 0, int fps = 50);

signals:
	/**
	 * @brief updateBlocks is emitted every frame when the block logic should update its values
	 * @param timeSinceLastFrame is the time in seconds since the last call of this signal
	 */
    void updateBlocks(double timeSinceLastFrame);
	/**
	 * @brief updateOutput is emitted every frame after the blocks have been updated
	 * and the output (i.e. ArtNet, sACN) should be updated
	 * @param timeSinceLastFrame is the time in seconds since the last call of this signal
	 */
    void updateOutput(double timeSinceLastFrame);

public slots:

	/**
	 * @brief start starts the engine, signals will then be emitted every frame
	 */
    void start();

	/**
	 * @brief stop stops the engine, no more signals will be emitted
	 */
    void stop();

private slots:

	/**
	 * @brief tick is the internal function called every frame
	 * and emits the signals in the correct order
	 */
	void tick();

private:
	/**
	 * @brief m_timer is the timer that triggers the tick() function
	 */
	QTimer m_timer;
	/**
	 * @brief m_fps is the amount of frames per second to be generated
	 */
	int m_fps;
	/**
	 * @brief m_lastFrameTime is the time of the last generated frame
	 */
	HighResTime::time_point_t m_lastFrameTime;

};

#endif // ENGINE_H
