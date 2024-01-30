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
    Q_PROPERTY(State state MEMBER m_state NOTIFY stateChanged)
    QML_UNCREATABLE("")
    QML_ELEMENT


public:
    HourBox(quint8 hour,QObject *parent):QObject(parent),m_hour(hour),
        m_state(Free)
    {};
    enum State {
        Free = 0,
        Occupied,
        Selected,
        Booking,
        Booked
    };
    Q_ENUM(State)
    quint8 hour() const{return m_hour;}
    State state() const{return m_state;}


signals:
    void stateChanged(void);
    void outIdChanged(void);

private:
    quint8 m_hour;
    State m_state;

};

class BOOKI_EXPORT HourModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    QML_UNCREATABLE("")
    QML_ELEMENT

public:
    enum ModelRoles {
        hourRole = Qt::UserRole + 1,
        stateRole
    };
    int count() const;
    void clean();
    explicit HourModel(int hstart,QObject *parent = nullptr);
    Q_INVOKABLE bool setProperty(int i, QString role, const QVariant value);

    void addBookedHours(const HourBox::State state, const std::vector<int>& booked_hours);
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

