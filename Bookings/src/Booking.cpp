#include "Booking.hpp"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

Booking::Booking(const qint64 &start,const qint64 &end):
    m_start(QDateTime::fromSecsSinceEpoch(start)),m_end(QDateTime::fromSecsSinceEpoch(end))
{

}
bool Booking::checkValidity(const QDateTime & ref)const
{
    return ref<m_end;
}
bool Booking::isValid()const
{
    return m_end>m_start;
}
bool Booking::contains(const QDateTime & ref)const
{
    return m_start<=ref&&m_end>ref;
}
std::vector<QDate> Booking::getDays(void)const
{
    std::vector<QDate> days;
    auto startday=m_start.date();
    const auto finishday=m_end.date();
    days.push_back(startday);

    for(auto i=0;i<startday.daysTo(finishday);i++)
    {
        startday=startday.addDays(1);
        days.push_back(startday);
    }

    return days;
}
std::vector<int> Booking::getHours(const QDate& day)const
{
    std::vector<int> booked_hours;

    auto st=(day==m_start.date())?
                  m_start.time():QTime(0,0);
    auto fi=(day==m_end.date())?
                  m_end.time():QTime(23,59,59,59);
    auto interval=(st.secsTo(fi)+1)/60/60;
    for(auto i =0;i<interval;i++)
    {
        booked_hours.push_back(st.hour()+i);
    }
    return booked_hours;
}

quint64 Booking::price(quint64 per_hour)const
{
    const auto hours=(m_end-m_start);

    return per_hour*(hours.count()+1000)/60/60/1000;
}
std::vector<Booking> Booking::fromArray(const QJsonArray &books)
{
    std::vector<Booking> var;
    for(auto i=0;i<books.size();i++)
    {
        if(i%2&&books.at(i-1).toInteger()>0&&books.at(i).toInteger()>0)
        {
            auto b=Booking(books.at(i-1).toInteger(),books.at(i).toInteger());
            var.push_back(b);
        }

    }
    return var;
}
quint64 Booking::price(const QJsonArray &books,quint64 per_hour)
{
    quint64 total=0;
    auto vec=fromArray(books);
    for (const auto &v:vec)total+=v.price(per_hour);
    return total;
}

QJsonArray Booking::metadataToBookings(const QByteArray metadata)
{
    const auto var=QJsonDocument::fromJson(metadata).object();
    QJsonArray nbooks;
    if(!var["b"].isUndefined()&&var["b"].isArray())
    {
        auto books=var["b"].toArray();
        quint64 start=0;
        const auto now=QDateTime::currentDateTime().toSecsSinceEpoch();
        for(auto i=0;i<books.size();i++)
        {

            if(i%2)
            {
                if(books.at(i).toInteger()>books.at(i-1).toInteger()&&books.at(i).toInteger()>now)
                {
                    nbooks.push_back(books.at(i-1));
                    nbooks.push_back(books.at(i));
                }
            }

        }
        return nbooks;
    }
    return QJsonArray();
}
std::shared_ptr<const qiota::qblocks::Feature> Booking::bookingsToMetadata(const QJsonArray &books)
{
    const auto booking=QJsonObject({{"b",books}});
    return qiota::qblocks::Feature::Metadata(QJsonDocument(booking).toJson());
}
