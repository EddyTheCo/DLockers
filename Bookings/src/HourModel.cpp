#include "HourModel.hpp"
#include <QJsonArray>

Hour_model::Hour_model(int hstart, QObject *parent)
    : QAbstractListModel(parent),m_count(24)
{
    for(auto i=hstart;i<m_count;i++)
    {
        m_hours.push_back(new Hour_box(((i>12)?QString::number(i-12):QString::number((i==0)?12:i))
                                       +((i<12)?" AM":" PM"),false,false,i,this));
    }

}

int Hour_model::count() const
{
    return m_hours.size();
}


int Hour_model::rowCount(const QModelIndex &p) const
{
    Q_UNUSED(p)
    return m_hours.size();
}
QHash<int, QByteArray> Hour_model::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[hourRole] = "hour";
    roles[bookedRole] = "booked";
    roles[selectedRole] = "selected";
    roles[sentbookRole] = "sentbook";
    roles[outIdRole] = "outId";
    return roles;
}
QVariant Hour_model::data(const QModelIndex &index, int role) const
{
    return m_hours[index.row()]->property(roleNames().value(role));
}
bool Hour_model::setData(const QModelIndex &index, const QVariant &value, int role )
{

    const auto re=m_hours[index.row()]->setProperty(roleNames().value(role),value);

    if(re)
    {
        emit dataChanged(index,index,QList<int>{role});

        if(roleNames().value(role)=="selected")
        {
            emit total_selected_changed((value.toBool())?1:-1);
        }
        return true;
    }
    return false;
}
bool Hour_model::setProperty(int i,QString role,const QVariant value)
{
    const auto ind=index(i);
    const auto rol=roleNames().keys(role.toUtf8());
    return setData(ind,value,rol.front());
}
void Hour_model::rm_sent_booked_hours(const std::vector<int>& booked_hours)
{
    for(auto v:booked_hours)
    {
        auto ind=v-m_hours.front()->hour_;
        if(ind>=0)
        {
            setProperty(ind,"sentbook",false);
        }
    }

}
void Hour_model::add_booked_hours(const QString id, const std::vector<int>& booked_hours)
{
    for(auto v:booked_hours)
    {
        auto ind=v-m_hours.front()->hour_;
        if(ind>=0)
        {
            if(id.isEmpty())
            {
                setProperty(ind,"booked",true);
            }
            else
            {
                setProperty(ind,"sentbook",true);
                setProperty(ind,"outId",id);
            }
                if(m_hours.at(ind)->selected())
                {
                    setProperty(ind,"selected",false);
                }
        }
    }

}
QModelIndex Hour_model::index(int row, int column , const QModelIndex &parent ) const
{
    return createIndex(row,column);
}
void Hour_model::getBookingsFromSelected(QDate day,QJsonArray& var)
{

    bool init=false;
    auto tstime=QTime(m_hours.front()->hour_,0);
    QTime start_t;
    QTime finish_t;

    for(auto v:m_hours)
    {
        if(v->selected())
        {
            if(!init)
            {
                start_t=tstime;
                init=!init;
            }
        }
        else
        {
            if(init)
            {
                finish_t=tstime.addSecs(-1);
                init=!init;
                var.push_back(QDateTime(day,start_t).toSecsSinceEpoch());
                var.push_back(QDateTime(day,finish_t).toSecsSinceEpoch());
            }
        }
        tstime=tstime.addSecs(60*60);
    }
    if(init)
    {
        if(m_hours.size()!=1)
        {
            finish_t=tstime.addSecs(-1*60*60);
            finish_t=tstime.addSecs(-1);
        }
        var.push_back(QDateTime(day,start_t).toSecsSinceEpoch());
        var.push_back(QDateTime(day,finish_t).toSecsSinceEpoch());
    }

}
void Hour_model::pop_front(void) {

    if(m_hours.size())
    {
        if(m_hours.front()->selected())setProperty(0,"selected",false);
        beginRemoveRows(QModelIndex(),0,0);
        m_hours.front()->deleteLater();
        m_hours.pop_front();
        emit countChanged(count());
        endRemoveRows();
    }
}
void Hour_model::update_list(void)
{
    if(QTime::currentTime().msecsSinceStartOfDay()/60/60/1000>m_hours.front()->hour_)
    {
        pop_front();
    }
}
