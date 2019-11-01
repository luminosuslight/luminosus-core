#include <QtGui>
#include <QApplication>
#include <QtQuick>

class CoreController;

namespace Luminosus {

void preQApplicationCreation();

void prepareQmlEngine(QQmlApplicationEngine& engine);

void setupDpProperty(QQmlApplicationEngine& engine);

void registerQtQuickItems();

void preparePauseAndShutdown(QApplication& app, QQmlApplicationEngine& engine, CoreController& controller);

}
