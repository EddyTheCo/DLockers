#pragma once

#include<QObject>
#include<QString>
#include <QtQml/qqmlregistration.h>
#include<set>
#include<queue>

#if defined(RPI_SERVER)
#include <QGeoPositionInfoSource>
#endif

#include "DayModel.hpp"
#include "block/qoutputs.hpp"
#include "client/qnode_outputs.hpp"

using namespace qiota;
using namespace qiota::qblocks;

class BookServer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool  open READ isOpen NOTIFY openChanged)
    Q_PROPERTY(float latitude READ latitude NOTIFY latitudeChanged)
    Q_PROPERTY(float longitude READ longitude NOTIFY longitudeChanged)
    Q_PROPERTY(DayModel* dayModel  READ dayModel CONSTANT)
    Q_PROPERTY(State  state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString  openAddress READ getOpenAddress NOTIFY openAddressChanged)
    QML_ELEMENT
    QML_SINGLETON

public:
    BookServer(QObject *parent = nullptr);
    enum State {
        Publishing = 0,
        Ready
    };
    Q_ENUM(State)
    static BookServer* instance(){return m_instance;}
    DayModel* dayModel(void)const{return m_dayModel;}
    qreal longitude()const{return m_longitude;}
    qreal latitude()const{return m_latitude;}
    void setCoords(float lat,float lon)
    {
        m_latitude=lat;
        m_longitude=lon;
	emit latitudeChanged();
	emit longitudeChanged();
    }
    void setOpen(bool op){if(op!=m_open){m_open=op;emit openChanged();}}
    bool isOpen()const{return m_open;}

    Q_INVOKABLE void getNewOpenAddress();
    QString getOpenAddress()const;

    State state(void)const{return m_state;}
    void setState(State state){if(state!=m_state){m_state=state;emit stateChanged(); }}


signals:
    void latitudeChanged();
    void longitudeChanged();
    void stateChanged();
    void openChanged();
    void openAddressChanged();

private:
    void publishState();
    void checkNftToOpen(std::shared_ptr<const qblocks::Output> output);
    QByteArray serializeState(void)const;
    void monitorPayments();
    void restart();
    void checkStateOutput(c_array outId, std::shared_ptr<const qblocks::Output> output);
    void handleNewBook(c_array bookId,std::shared_ptr<const qblocks::Output> bookOut);
    void handleInitFunds();
    std::shared_ptr<qblocks::Output> getPublishOutput()const;
    void cleanState(void);


    std::set<Booking> m_books;
    std::queue<Node_output> queue;

    quint64 m_pph;
    void checkLPermission();
    void initGPS();

    qreal m_latitude,m_longitude;
    DayModel* m_dayModel;
    State m_state;
    c_array  m_pubId;
    bool m_open;
    QString m_openAddress;
    QTimer * m_timer;
    QObject* receiver;
#if defined(RPI_SERVER)
    void openBox(void);
    QGeoPositionInfoSource *PosSource;
#endif
    static BookServer* m_instance;
};



