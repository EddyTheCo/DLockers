#include "DayModel.hpp"

Day_model::Day_model(QObject *parent)
    :QAbstractListModel(parent),m_count(7),timer_m(new QTimer(this)),total_selected_(0)
{

    QDate today=QDate::currentDate();
    for(auto i=0;i<m_count;i++)
    {
        Day_box* day_box;
        if(i==0)
        {
            auto zero=QTime(0,0);
            auto init=zero.secsTo(QTime::currentTime())/60/60;
            day_box=new Day_box(today,new Hour_model(init,this),this);

        }
        else
        {
            day_box=new Day_box(today,new Hour_model(0,this),this);
        }
        connect(day_box->hour_model(),&Hour_model::total_selected_changed,this, &Day_model::add_to_total_selected);

        m_days.push_back(day_box);
        today=today.addDays(1);
    }
    connect(timer_m, &QTimer::timeout, this, &Day_model::update_list);
    timer_m->start(1800000);



}
void Day_model::update_list(void)
{
    m_days.front()->hour_model()->update_list();
    if(QDate::currentDate()>m_days.front()->day())
    {
        pop_front();
        auto next_day=m_days.back()->day().addDays(1);
        append(new Day_box(next_day,new Hour_model(0,this),this));
    }
}
int Day_model::count() const
{
    return m_count;
}


int Day_model::rowCount(const QModelIndex &p) const
{
    Q_UNUSED(p)
    return m_days.size();
}
QHash<int, QByteArray> Day_model::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[dayRole] = "day";
    roles[hour_modelRole] = "hour_model";
    return roles;
}
QVariant Day_model::data(const QModelIndex &index, int role) const
{
    return m_days[index.row()]->property(roleNames().value(role));
}

void Day_model::append(Day_box *o) {

    int i = m_days.size();
    beginInsertRows(QModelIndex(), i, i);
    m_days.append(o);
    emit countChanged(count());
    endInsertRows();

}

void Day_model::pop_front(void) {

    if(m_days.size())
    {
        beginRemoveRows(QModelIndex(),0,0);
        m_days.front()->deleteLater();
        m_days.pop_front();
        emit countChanged(count());
        endRemoveRows();
    }
}
void Day_model::remove_sent_booking(const QString& outid)
{
    auto it=sentBookings_.find(outid);
    if(it!=sentBookings_.end())
    {
        auto vec=Booking::from_Array(it.value());
        for(const auto & nbook:vec)
        {
            auto days=nbook.get_days();
            auto list_start_day=m_days.front()->day();
            for(auto d:days)
            {
                auto ind=list_start_day.daysTo(d);
                auto booked_hours=nbook.get_hours(d);
                m_days.at(ind)->hour_model()->rm_sent_booked_hours(booked_hours); //Fix this. you are writing to hour model and broke the binding to the underlying model
            }
        }
        sentBookings_.remove(outid);
    }

}

void Day_model::add_booking(const QJsonArray& books,QString id)
{
    auto vec=Booking::from_Array(books);

    for(const auto & nbook:vec)
    {
        auto days=nbook.get_days();
        auto list_start_day=m_days.front()->day();

        for(auto d:days)
        {
            auto ind=list_start_day.daysTo(d);
            if(ind>=0)
            {
                auto booked_hours=nbook.get_hours(d);
                m_days.at(ind)->hour_model()->add_booked_hours(id,booked_hours);
                if(!id.isEmpty())sentBookings_.insert(id,books);
            }
        }
    }

}
QJsonArray Day_model::getNewBookings(void)
{
    QJsonArray var;
    for(auto v:m_days)
    {
        v->hour_model()->getBookingsFromSelected(v->day(),var);
    }
    return var;
}
