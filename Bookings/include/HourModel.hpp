#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QtQml/qqmlregistration.h>
#include "Booking.hpp"
#include <QJsonObject>

class BOOKI_EXPORT Hour_box : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString hour READ hour CONSTANT)
    Q_PROPERTY(bool booked READ booked WRITE set_booked NOTIFY booked_changed)
    Q_PROPERTY(bool selected READ selected WRITE set_selected NOTIFY selected_changed)
    Q_PROPERTY(bool sentbook READ sentbook WRITE set_sentbook NOTIFY sentbook_changed)
    Q_PROPERTY(QString outId READ outId WRITE setOutId NOTIFY outIdChanged)
    QML_ELEMENT


public:
    Hour_box(QString hour_,bool booked_,bool selected_,int hour_m,QObject *parent):QObject(parent),hour_m(hour_),
        booked_m(booked_),selected_m(selected_),hour_(hour_m),sentbook_m(false)
    {};
    const int hour_;
    QString hour() const{return hour_m;}
    bool booked() const{return booked_m;}
    bool selected() const{return selected_m;}
    bool sentbook() const{return sentbook_m;}
    QString outId()const{return outId_;}
    void setOutId(QString id_m){if(id_m!=outId_){outId_=id_m;emit outIdChanged();}}
	
    void set_booked(bool booked_){if(booked_!=booked_m){booked_m=booked_;emit booked_changed();}};
    void set_selected(bool selected_){if(selected_!=selected_m){selected_m=selected_;emit selected_changed();}};
    void set_sentbook(bool sentbook_){if(sentbook_!=sentbook_m){sentbook_m=sentbook_;emit sentbook_changed();}};

signals:
    void booked_changed(void);
    void selected_changed(void);
    void sentbook_changed(void);
    void outIdChanged(void);

private:
    QString hour_m;
    bool booked_m,selected_m,sentbook_m;
    QString outId_;

};

class BOOKI_EXPORT Hour_model : public QAbstractListModel
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
    explicit Hour_model(int hstart,QObject *parent = nullptr);
    Q_INVOKABLE bool setProperty(int i, QString role, const QVariant value);

    void add_booked_hours(const QString id, const std::vector<int>& booked_hours);
    void rm_sent_booked_hours(const std::vector<int>& booked_hours);
    void getBookingsFromSelected(QDate day,QJsonArray& var);

    void pop_front(void);
    void update_list(void);

    int rowCount(const QModelIndex &p) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
    QHash<int, QByteArray> roleNames() const;

signals:
    void countChanged(int count);
    void total_selected_changed(int selecteds);

private:
    int m_count;
    QList<Hour_box*> m_hours;
};

