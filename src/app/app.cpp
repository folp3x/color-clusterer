#include "app.h"

#include <QDebug>

App::App(QObject *parent) : QObject{parent} {}

void App::clusterImage() const { qDebug().noquote() << "Clustering..."; }
