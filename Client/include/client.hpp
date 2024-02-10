#pragma once

#define USE_QML

#include<QObject>
#include<QString>
#include <QtQml/qqmlregistration.h>
#include "block/carray.hpp"
#include "block/qoutputs.hpp"
#include "qaddr_bundle.hpp"
#include "DayModel.hpp"
#include <QHash>

using namespace qiota::qblocks;
using namespace qiota;



class Server : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString account READ account CONSTANT)
    Q_PROPERTY(float score READ score NOTIFY scoreChanged)
    Q_PROPERTY(quint8 occupied READ occupied NOTIFY occupiedChanged)
    Q_PROPERTY(float latitude READ latitude NOTIFY latitudeChanged)
    Q_PROPERTY(float longitude READ longitude NOTIFY longitudeChanged)
    Q_PROPERTY(Qml64*  price READ price CONSTANT)
    Q_PROPERTY(DayModel* dayModel  READ dayModel CONSTANT)
    QML_UNCREATABLE("")
    QML_ELEMENT

public:
    Server(QString account,
           const c_array outId,
           const quint64 pph,
           const c_array payAddress,
           const QJsonArray bookings,
           const float latitude,
           const float longitude,
           const float score=0,
           const quint8 occupied=0,
           QObject *parent = nullptr);

    void setScore(float score){if(score!=m_score){m_score=score; emit scoreChanged();}}
    void setOccupied(quint8 occupied){if(occupied!=m_occupied){m_occupied=occupied; emit occupiedChanged();}}
    void setLatitude(float latitude){if(latitude!=m_latitude){m_latitude=latitude; emit latitudeChanged();}}
    void setLongitude(float longitude){if(longitude!=m_longitude){m_longitude=longitude; emit longitudeChanged();}}
    void setPph(quint64 pph){if(pph!=m_pph){
            m_pph=pph;
            m_price->setValue(m_pph*m_dayModel->totalSelected());
        }
    }
    void setPayAddress(const c_array payAddress){if(payAddress!=m_payAddress){m_payAddress=payAddress;}}
    void setOutId(c_array outId){if(m_outId!=outId){m_outId=outId;}}
    QString account()const{return m_account;}
    c_array outId()const{return m_outId;}
    quint8 score()const{return m_score;}
    quint8 occupied()const{return m_occupied;}
    float latitude()const{return m_latitude;}
    float longitude()const{return m_longitude;}
    Qml64* price()const{return m_price;}
    DayModel* dayModel(void)const{return m_dayModel;}

    void sendBookings();
    void presentNft(const QString address);

signals:
    void scoreChanged();
    void occupiedChanged();
    void latitudeChanged();
    void longitudeChanged();
    void stateChanged();
    void gotNewBooking(QJsonArray books);
    void noValidNftFound();

private:

    void sendNFT(c_array id,c_array addressArray)const;
    float m_latitude,m_longitude,m_score;
    quint8 m_occupied;
    c_array m_payAddress,m_outId;
    QString m_account;
    quint64 m_pph;
    Qml64* m_price;
    DayModel* m_dayModel;
};

class BookClient : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int selected READ selected NOTIFY selectedChanged)
    QML_ELEMENT
    QML_SINGLETON

public:
    enum ModelRoles {
        scoreRole = Qt::UserRole + 1, accountRole,
        occupiedRole,latitudeRole,longitudeRole,priceRole,dayModelRole};
    int count() const
    {
        return m_servers.size();
    }
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Q_INVOKABLE bool setProperty(int i, QString role, const QVariant value);
    Q_INVOKABLE void setSelected(int ind);
    int selected(void)const{return m_selected;}
    Q_INVOKABLE void sendBookings(int index)
    {
        if(index>-1&&index<m_servers.size())
            m_servers.at(index)->sendBookings();
    };
    Q_INVOKABLE void presentNFT(int index,QString address)
    {
        if(index>-1&&index<m_servers.size())
            m_servers.at(index)->presentNft(address);
    };

    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex &p) const{
        Q_UNUSED(p)
        return m_servers.size();
    }
    QVariant data(const QModelIndex &index, int role) const;

    BookClient(QObject *parent = nullptr);


signals:
    void countChanged(int);
    void selectedChanged();

private:
    void resetData();
    void getBookings();
    void checkNFTforBook(std::shared_ptr<const qblocks::Output> output);
    void rmServer(c_array outId);
    int idToIndex(c_array outId);
    void deserializeState(const QString account, const c_array outId, const QByteArray state);
    void getServerList(void);
    void addServer(QString account);
    void getServer(std::shared_ptr<const Output>, c_array outId, QString hrp);
    int m_selected;
    QList<Server*> m_servers;
};
