#include "objtomap.hpp"
#include <QGuiApplication>

#if QT_CONFIG(permissions)
#include <QPermission>
#endif

LocationPermisioner::LocationPermisioner(QObject *parent):QObject(parent),m_granted(false)
{

}

void LocationPermisioner::getPermissions(void)
{
#if QT_CONFIG(permissions)
    QLocationPermission lPermission;
    lPermission.setAccuracy(QLocationPermission::Precise);
    lPermission.setAvailability(QLocationPermission::WhenInUse);

    switch (qApp->checkPermission(lPermission)) {
    case Qt::PermissionStatus::Undetermined:
        qApp->requestPermission(lPermission, this,
                                &LocationPermisioner::getPermissions);
        return;
    case Qt::PermissionStatus::Denied:
        setGranted(false);
        return;
    case Qt::PermissionStatus::Granted:
        setGranted(true);
        return;
    }
#else
    setGranted(true);
#endif

}

