

#include <QtGui>
#include <QApplication>
#include <QtQuick>

namespace Luminosus {

void preQApplicationCreation();

void prepareQmlEngine(QQmlApplicationEngine& engine);

void setupDpProperty(QQmlApplicationEngine& engine);

void registerQtQuickItems();

}
