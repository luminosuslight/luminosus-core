#include <QtGui>
#include <QGuiApplication>
#include <QtQuick>

class CoreController;

namespace Luminosus {

void preQApplicationCreation();

void prepareQmlEngine(QQmlApplicationEngine& engine);

void setupDpProperty(QQmlApplicationEngine& engine);

void registerQtQuickItems();

void preparePauseAndShutdown(QGuiApplication& app, QQmlApplicationEngine& engine, CoreController& controller);

}
