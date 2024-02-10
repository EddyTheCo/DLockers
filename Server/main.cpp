#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "Qrimageprovider.hpp"
#if defined(FORCE_STYLE)
#include <QQuickStyle>
#endif

int main(int argc, char *argv[])
{
	QGuiApplication app(argc, argv);
#if defined(FORCE_STYLE)
    QQuickStyle::setStyle(FORCE_STYLE);
#endif

	QQmlApplicationEngine engine;
	engine.addImportPath("qrc:/esterVtech.com/imports");
    engine.addImageProvider(QLatin1String("qrcode"), new QRImageProvider(1));
    const QUrl url(u"qrc:/esterVtech.com/imports/Esterv/DLockers/Server/qml/window.qml"_qs);
	engine.load(url);
	return app.exec();
}
