#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "Qrimageprovider.hpp"


int main(int argc, char *argv[])
{
	QGuiApplication app(argc, argv);

	QQmlApplicationEngine engine;
	engine.addImportPath("qrc:/esterVtech.com/imports");
    engine.addImageProvider(QLatin1String("qrcode"), new QRImageProvider(1));
    const QUrl url(u"qrc:/esterVtech.com/imports/Esterv/DLockers/Server/qml/window.qml"_qs);
	engine.load(url);
	return app.exec();
}
