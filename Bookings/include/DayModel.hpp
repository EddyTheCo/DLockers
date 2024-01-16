#pragma once

#include <QObject>
#include <QAbstractListModel>
#include <QTimer>
#include <QtQml/qqmlregistration.h>
#include "HourModel.hpp"
#include <QJsonArray>


class BOOKI_EXPORT DayBox : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDate day READ day CONSTANT)
    Q_PROPERTY(HourModel* hourModel  READ hourModel CONSTANT)
    QML_ELEMENT

public:

    HourModel* hourModel(void)const{return m_hourModel;}
    QDate day(void){return m_day;}

    DayBox(QDate day,HourModel * hourModel,QObject *parent):QObject(parent),m_day(day),
        m_hourModel(hourModel){
        m_hourModel->setParent(this);
    };

private:
    HourModel *  m_hourModel;
    QDate m_day;

};

class BOOKI_EXPORT DayModel : public QAbstractListModel
{
    Q_OBJECT    
    Q_PROPERTY(int count READ count CONSTANT)
    Q_PROPERTY(int totalSelected  READ totalSelected  NOTIFY totalSelectedChanged)
    QML_ELEMENT

public:

    explicit DayModel(QObject *parent = nullptr);
    enum ModelRoles {
        dayRole = Qt::UserRole + 1,
        hourModelRole
    };

    QJsonArray getNewBookings(void);
    void addBooking(const QJsonArray &books, QString id="" );

    void removeSentBooking(const QString &outid);

    void addToTotalSelected(int sel){m_totalSelected+=sel; emit totalSelectedChanged(m_totalSelected);}

    int totalSelected(void)const{return m_totalSelected;}

    void append(DayBox* o);
    void popFront(void);
    void updateList(void);

    int count() const;
    int rowCount(const QModelIndex &p) const;
    QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;



signals:
    void countChanged(int count);
    void totalSelectedChanged(int sele);

private:
    int m_count;
    QList<DayBox*> m_days;
    QTimer *m_timer;
    int m_totalSelected;
    QHash <QString,QJsonArray> m_sentBookings;
};

