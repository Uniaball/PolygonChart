#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuickControls2/QQuickStyle>
#include "RadarChartModel.h"
#include "RadarChartRenderer.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Material");

    qmlRegisterType<RadarChartRenderer>("PolygonChart", 1, 0, "RadarChartRenderer");
    qmlRegisterType<RadarChartModel>("PolygonChart", 1, 0, "RadarChartModel");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/PolygonChart/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}