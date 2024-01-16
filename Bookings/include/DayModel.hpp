#pragma once

#include <QObject>
#include <QAbstractListModel>
#include <QTimer>
#include <QtQml/qqmlregistration.h>
#include "HourModel.hpp"
#include <QJsonArray>


class BOOKI_EXPORT Day_box : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDate day READ day CONSTANT)
    Q_PROPERTY(Hour_model* hour_model  READ hour_model CONSTANT)
    QML_ELEMENT

public:

    Hour_model* hour_model(void)const{return hour_model_m;}
    QDate day(void){return day_m;}

    Day_box(QDate day_,Hour_model * hour_model_,QObject *parent):QObject(parent),day_m(day_),hour_model_m(hour_model_){
        hour_model_->setParent(this);
    };

private:
    Hour_model *  hour_model_m;
    QDate day_m;

};

class BOOKI_EXPORT Day_model : public QAbstractListModel
{
    Q_OBJECT    
    Q_PROPERTY(int count READ count CONSTANT)
    Q_PROPERTY(int total_selected  READ total_selected  NOTIFY total_selected_changed)
    QML_ELEMENT

public:

    explicit Day_model(QObject *parent = nullptr);
    enum ModelRoles {
        dayRole = Qt::UserRole + 1,
        hour_modelRole
    };

    QJsonArray getNewBookings(void);
    Q_INVOKABLE void add_booking(const QJsonArray &books, QString id="" );

    Q_INVOKABLE void remove_sent_booking(const QString &outid);

    void add_to_total_selected(int sel){total_selected_+=sel; emit total_selected_changed(total_selected_);}

    int total_selected(void)const{return total_selected_;}




    void append(Day_box* o);
    void pop_front(void);
    void update_list(void);

    int count() const;
    int rowCount(const QModelIndex &p) const;
    QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;



signals:
    void countChanged(int count);
    void total_selected_changed(int sele);
    void hasnewbooks(QJsonArray);


private:
    int m_count;
    QList<Day_box*> m_days;
    QTimer *timer_m;
    int total_selected_;
    QHash <QString,QJsonArray> sentBookings_;
};

