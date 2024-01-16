#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QtQml/qqmlregistration.h>
#include "Booking.hpp"
#include <QJsonObject>

class BOOKI_EXPORT HourBox : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint8 hour READ hour CONSTANT)
    Q_PROPERTY(bool booked MEMBER m_booked NOTIFY bookedChanged)
    Q_PROPERTY(bool selected MEMBER m_selected NOTIFY selectedChanged)
    Q_PROPERTY(bool sentbook MEMBER m_sentbook NOTIFY sentbookChanged)
    Q_PROPERTY(QString outId READ outId WRITE setOutId NOTIFY outIdChanged)
    QML_ELEMENT


public:
    HourBox(quint8 hour,bool booked,bool selected,QObject *parent):QObject(parent),m_hour(hour),
        m_booked(booked),m_selected(selected),m_sentbook(false)
    {};
    quint8 hour() const{return m_hour;}
    bool booked() const{return m_booked;}
    bool selected() const{return m_selected;}
    bool sentbook() const{return m_sentbook;}
    QString outId()const{return m_outId;}
    void setOutId(QString outId){if(outId!=m_outId){m_outId=outId;emit outIdChanged();}}
	
    void set_booked(bool booked){if(booked!=m_booked){m_booked=booked;emit bookedChanged();}};
    void set_selected(bool selected){if(selected!=m_selected){m_selected=selected;emit selectedChanged();}};
    void set_sentbook(bool sentbook){if(sentbook!=m_sentbook){m_sentbook=sentbook;emit sentbookChanged();}};

signals:
    void bookedChanged(void);
    void selectedChanged(void);
    void sentbookChanged(void);
    void outIdChanged(void);

private:
    quint8 m_hour;
    bool m_booked,m_selected,m_sentbook;
    QString m_outId;

};

class BOOKI_EXPORT HourModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    QML_ELEMENT

public:
    enum ModelRoles {
        hourRole = Qt::UserRole + 1,
        bookedRole,selectedRole,sentbookRole,outIdRole
    };
    int count() const;
    explicit HourModel(int hstart,QObject *parent = nullptr);
    Q_INVOKABLE bool setProperty(int i, QString role, const QVariant value);

    void addBookedHours(const QString id, const std::vector<int>& booked_hours);
    void rmSentBookedHours(const std::vector<int>& booked_hours);
    void getBookingsFromSelected(QDate day,QJsonArray& var);


    void updateList(void);

    int rowCount(const QModelIndex &p) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
    QHash<int, QByteArray> roleNames() const;

signals:
    void countChanged(int count);
    void totalSelectedChanged(int selecteds);

private:
    void popFront(void);
    int m_count;
    QList<HourBox*> m_hours;
};

