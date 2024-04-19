#include <QApplication>
#include <QQmlApplicationEngine>

#include <QQmlContext>
#include "appcore.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    AppCore appcore;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("appcore", &appcore);

    const QUrl url(u"qrc:/MeasuringObserver/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
