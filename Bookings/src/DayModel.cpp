#include "DayModel.hpp"

DayModel::DayModel(QObject *parent)
    :QAbstractListModel(parent),m_count(7),m_timer(new QTimer(this)),m_totalSelected(0)
{

    QDate today=QDate::currentDate();
    for(auto i=0;i<m_count;i++)
    {
        DayBox* dayBox;
        if(i==0)
        {
            auto zero=QTime(0,0);
            auto init=zero.secsTo(QTime::currentTime())/60/60;
            dayBox=new DayBox(today,new HourModel(init,this),this);

        }
        else
        {
            dayBox=new DayBox(today,new HourModel(0,this),this);
        }
        append(dayBox);
        today=today.addDays(1);
    }
    connect(m_timer, &QTimer::timeout, this, &DayModel::updateList);
    m_timer->start(1800000);

}
void DayModel::updateList(void)
{
    m_days.front()->hourModel()->updateList();
    if(QDate::currentDate()>m_days.front()->day())
    {
        popFront();
        auto nextDay=m_days.back()->day().addDays(1);
        append(new DayBox(nextDay,new HourModel(0,this),this));
    }
}
int DayModel::count() const
{
    return m_count;
}

int DayModel::rowCount(const QModelIndex &p) const
{
    Q_UNUSED(p)
    return m_days.size();
}
QHash<int, QByteArray> DayModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[dayRole] = "day";
    roles[hourModelRole] = "hourModel";
    return roles;
}
QVariant DayModel::data(const QModelIndex &index, int role) const
{
    return m_days[index.row()]->property(roleNames().value(role));
}

void DayModel::append(DayBox *o) {
    connect(o->hourModel(),&HourModel::totalSelectedChanged,this, &DayModel::addToTotalSelected);
    int i = m_days.size();
    beginInsertRows(QModelIndex(), i, i);
    m_days.append(o);
    emit countChanged(count());
    endInsertRows();
}

void DayModel::popFront(void) {

    if(m_days.size())
    {
        beginRemoveRows(QModelIndex(),0,0);
        m_days.front()->deleteLater();
        m_days.pop_front();
        emit countChanged(count());
        endRemoveRows();
    }
}
void DayModel::removeSentBooking(const std::vector<Booking>& bookings)
{

    for(const auto & nbook:bookings)
    {
        auto days=nbook.getDays();
        auto listStartDay=m_days.front()->day();
        for(auto d:days)
        {
            auto ind=listStartDay.daysTo(d);
            auto booked_hours=nbook.getHours(d);
            m_days.at(ind)->hourModel()->rmSentBookedHours(booked_hours);
        }
    }

}

void DayModel::addBooking(const HourBox::State state, const QJsonArray& books)
{
    auto vec=Booking::fromArray(books);

    for(const auto & nbook:vec)
    {
        auto days=nbook.getDays();
        auto list_start_day=m_days.front()->day();

        for(auto d:days)
        {
            auto ind=list_start_day.daysTo(d);
            if(ind>=0)
            {
                auto booked_hours=nbook.getHours(d);
                m_days.at(ind)->hourModel()->addBookedHours(state,booked_hours);
            }
        }
    }

}
QJsonArray DayModel::getNewBookings(void)
{
    QJsonArray var;
    for(auto v:m_days)
    {
        v->hourModel()->getBookingsFromSelected(v->day(),var);
    }
    return var;
}
