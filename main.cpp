#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "backend/backend.h"

int main(int argc, char *argv[]) {
  qputenv("QSG_RHI_BACKEND", "opengl");

  QGuiApplication app(argc, argv);

  Backend backend{};

  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty("backend", &backend);

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
  engine.loadFromModule("clusterer", "Main");

  return QGuiApplication::exec();
}
