#include<QObject>
#include <QtQml/qqmlregistration.h>
#include <QJsonObject>

class  ObjectMapView: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QJsonObject Layout MEMBER m_layout NOTIFY layoutChanged)
    Q_PROPERTY(bool visible MEMBER m_visible NOTIFY visibleChanged)

    QML_ELEMENT
signals:
    void layoutChanged(void);
    void visibleChanged(void);
public:
    ObjectMapView(QObject *parent = nullptr);

private:
    bool m_visible;
    QJsonObject m_layout;
};
