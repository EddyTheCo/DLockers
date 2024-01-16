#include<QObject>
#include <QtQml/qqmlregistration.h>


class  LocationPermisioner: public QObject
{
    bool m_granted;
    void setGranted(bool granted){if(granted!=m_granted){m_granted=granted;emit grantedChanged();}}
    Q_OBJECT
    Q_PROPERTY(bool isGranted READ isGranted NOTIFY grantedChanged)
    QML_ELEMENT
    QML_SINGLETON



signals:
    void grantedChanged(void);
public:
    LocationPermisioner(QObject *parent = nullptr);
    bool isGranted(void)const{return m_granted;}
    Q_INVOKABLE void getPermissions();
};
