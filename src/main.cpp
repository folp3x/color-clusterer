#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "app/app.h"

int main(int argc, char *argv[]) {
  qputenv("QSG_RHI_BACKEND", "opengl");

  QGuiApplication app(argc, argv);
  app.setWindowIcon(QIcon(":/icons/app_icon.png"));

  App coreApp{};

  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty("app", &coreApp);

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
  engine.loadFromModule("clusterer", "Main");

  return QGuiApplication::exec();
}
