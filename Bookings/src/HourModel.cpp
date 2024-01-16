#include "HourModel.hpp"
#include <QJsonArray>

HourModel::HourModel(int hstart, QObject *parent)
    : QAbstractListModel(parent),m_count(24)
{
    for(auto i=hstart;i<m_count;i++)
    {
        m_hours.push_back(new HourBox(i,false,false,this));
    }

}

int HourModel::count() const
{
    return m_hours.size();
}


int HourModel::rowCount(const QModelIndex &p) const
{
    Q_UNUSED(p)
    return m_hours.size();
}
QHash<int, QByteArray> HourModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[hourRole] = "hour";
    roles[bookedRole] = "booked";
    roles[selectedRole] = "selected";
    roles[sentbookRole] = "sentbook";
    roles[outIdRole] = "outId";
    return roles;
}
QVariant HourModel::data(const QModelIndex &index, int role) const
{
    return m_hours[index.row()]->property(roleNames().value(role));
}
bool HourModel::setData(const QModelIndex &index, const QVariant &value, int role )
{

    const auto re=m_hours[index.row()]->setProperty(roleNames().value(role),value);

    if(re)
    {
        emit dataChanged(index,index,QList<int>{role});

        if(roleNames().value(role)=="selected")
        {
            emit totalSelectedChanged((value.toBool())?1:-1);
        }
        return true;
    }
    return false;
}
bool HourModel::setProperty(int i,QString role,const QVariant value)
{
    const auto ind=index(i);
    const auto rol=roleNames().keys(role.toUtf8());
    return setData(ind,value,rol.front());
}
void HourModel::rmSentBookedHours(const std::vector<int>& booked_hours)
{
    for(auto v:booked_hours)
    {
        auto ind=v-m_hours.front()->hour();
        if(ind>=0)
        {
            setProperty(ind,"sentbook",false);
        }
    }

}
void HourModel::addBookedHours(const QString id, const std::vector<int>& booked_hours)
{
    for(auto v:booked_hours)
    {
        auto ind=v-m_hours.front()->hour();
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
QModelIndex HourModel::index(int row, int column , const QModelIndex &parent ) const
{
    return createIndex(row,column);
}
void HourModel::getBookingsFromSelected(QDate day,QJsonArray& var)
{
    bool init=false;
    auto tstime=QTime(m_hours.front()->hour(),0);
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
void HourModel::popFront(void) {

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
void HourModel::updateList(void)
{
    if(QTime::currentTime().hour()>m_hours.front()->hour())
    {
        popFront();
    }
}
