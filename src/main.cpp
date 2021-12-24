#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>
#include <iio.h>
#include "pluto.h"
#include "waterfall.h"

int main(int argc, char *argv[])
{
    pluto pluto;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);
    qmlRegisterType<Waterfall>("HamTools", 1, 0, "Waterfall");

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("pluto", QVariant::fromValue(&pluto));

    const QUrl url(QStringLiteral("qrc:/src/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
        QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
