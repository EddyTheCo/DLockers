#include"server.hpp"
#include <QCryptographicHash>
#include <QDebug>
#include<QJsonDocument>
#include<QTimer>
#include <QRandomGenerator>

#include "qwallet.hpp"
#include"nodeConnection.hpp"
#include "account.hpp"
#include"lgpio.h"


#include <QGuiApplication>
#if QT_CONFIG(permissions)
#include <QPermission>
#endif


void BookServer::checkStateOutput(c_array outId, QString payAddress,std::shared_ptr<const qblocks::Output> output)
{
    auto metfeau=output->get_feature_(Feature::Metadata_typ);
    if(metfeau)
    {
        auto metadata_feature=std::static_pointer_cast<const Metadata_Feature>(metfeau);
        auto metadata=metadata_feature->data();
        const auto var=QJsonDocument::fromJson(metadata).object();
        if(!var["b"].isUndefined()&&var["b"].isArray())
        {
            const auto bookarray=var["b"].toArray();
            const auto vec=Booking::fromArray(bookarray);
            for(const auto& v:vec)
            {
                m_books.insert(v);
            }
            m_dayModel->addBooking(HourBox::Occupied,bookarray);
            m_pubId=outId;
            monitorPayments(payAddress);
        }
    }
}
void BookServer::monitorPayments(QString payAddress)
{
    qDebug()<<"BookServer::monitorPayments:"<<payAddress;
    connect(this,&BookServer::stateChanged,this,[=](){
        qDebug()<<"BookServer::stateChanged:"<<m_state;
        while (m_state==Ready&&!queue.empty()) {
            const auto nout=queue.front();
            handleNewBook(nout.metadata().outputid_,nout.output());
            queue.pop();
        }
    });

    auto resp=NodeConnection::instance()->mqtt()->
                get_outputs_unlock_condition_address("address/"+payAddress);
    QObject::connect(resp,&ResponseMqtt::returned,this,[=](QJsonValue data){
        qDebug()<<"ResponseMqtt::returned:"<<m_state;
        if(m_state==Ready)
        {
            const auto nout=Node_output(data);
            handleNewBook(nout.metadata().outputid_,nout.output());
        }
        else
        {
            queue.push(Node_output(data));
        }
    });

}
void BookServer::restart(void)
{
    auto info=NodeConnection::instance()->rest()->get_api_core_v2_info();
    connect(info,&Node_info::finished,this,[=]( ){

        const auto serverId=Wallet::instance()->addresses().begin()->second->getAddressBech32();
        const auto payAddress=(++Wallet::instance()->addresses().begin())->second->getAddressBech32();
        auto nodeOutputs=NodeConnection::instance()->rest()->
                           get_outputs<Output::Basic_typ>("address="+serverId+
                                                          "&hasStorageDepositReturn=false&hasTimelock=false&hasExpiration=false&sender="+
                                                          serverId+"&tag="+fl_array<quint8>("DLockers").toHexString());

        connect(nodeOutputs,&Node_outputs::finished,this,[=]( ){
            qDebug()<<"Node_outputs::finished:"<<nodeOutputs->outs_.size();
            if(nodeOutputs->outs_.size())
            {
                const auto nout=nodeOutputs->outs_.front();
                checkStateOutput(nout.metadata().outputid_,payAddress,nout.output());

            }
            else
            {
                if(Wallet::instance()->amount())
                {
                    handleInitFunds(payAddress);
                }
                else
                {
                    connect(Wallet::instance(),&Wallet::amountChanged,this,[=]()
                            {
                                handleInitFunds(payAddress);
                            });
                }
            }
            nodeOutputs->deleteLater();
        });
        info->deleteLater();
    });
}


BookServer::BookServer(QObject *parent):QObject(parent),m_pph(10000),m_GeoCoord(41.852229,12.45100),m_state(Ready),m_open(false),m_dayModel(new DayModel(this))
{

    //Account::instance()->setSeed("outer broccoli mango tourist hungry venue industry twin grass pluck culture alley crane shy differ cage annual fit also diagram salmon spring original wild");
    Wallet::instance()->setAddressRange(2);
    checkLPermission();
    connect(Wallet::instance(),&Wallet::synced,this,[=](){
        if(Wallet::instance()->nRootAddresses()==2)
        {
            disconnect(Wallet::instance(),&Wallet::synced,this,nullptr);
            restart();
        }
    });


}
void BookServer::initGPS(void)
{
    PosSource = QGeoPositionInfoSource::createSource("nmea", {std::make_pair("nmea.source",SERIAL_PORT_NAME)}, this);
    if(!PosSource)PosSource=QGeoPositionInfoSource::createDefaultSource(this);
    if (PosSource) {
        connect(PosSource,&QGeoPositionInfoSource::positionUpdated,
                this, [=](const QGeoPositionInfo &update){
                    m_GeoCoord=update.coordinate();
                    emit geoCoordChanged();
                });
        connect(PosSource,&QGeoPositionInfoSource::errorOccurred,
                this, [=](QGeoPositionInfoSource::Error _t1){
                    qDebug()<<"Position Error:"<<_t1;
                });
        PosSource->setUpdateInterval(30000);
        PosSource->requestUpdate();
        PosSource->startUpdates();
    }
}
void BookServer::checkLPermission(void)
{

#if QT_CONFIG(permissions)

    QLocationPermission lPermission;
    lPermission.setAccuracy(QLocationPermission::Precise);
    switch (qApp->checkPermission(lPermission)) {
    case Qt::PermissionStatus::Undetermined:
        qApp->requestPermission(lPermission, this,
                                &BookServer::checkLPermission);
        return;
    case Qt::PermissionStatus::Denied:
        return;
    case Qt::PermissionStatus::Granted:
        //initGPS();
        return;
    }
#else
    //initGPS();
#endif

}

void BookServer::cleanState(void)
{
    qDebug()<<"BookServer::cleanState";
    const auto now=QDateTime::currentDateTime();
    const auto nBook=Booking(now.toSecsSinceEpoch(),now.toSecsSinceEpoch());
    auto lbound=m_books.lower_bound(nBook);
    m_books.erase(m_books.begin(),lbound);
}

std::shared_ptr<qblocks::Output> BookServer::getPublishOutput()const
{

    const fl_array<quint8> tag("DLockers");
    const auto state=serializeState();
    const auto eddAddr=Wallet::instance()->addresses().begin()->second->getAddress();
    auto sendFea=Feature::Sender(eddAddr);
    auto tagFea=Feature::Tag(tag);

    auto metFea=Feature::Metadata(state);

    auto addUnlcon=Unlock_Condition::Address(eddAddr);
    return Output::Basic(0,{addUnlcon},{},{sendFea,metFea,tagFea});
}
QString BookServer::getOpenAddress()const{return m_openAddress;}
void BookServer::getNewOpenAddress()
{
    static ResponseMqtt* resp=nullptr;
    if(resp)resp->deleteLater();
    setOpen(false);
    m_openAddress.clear();
    emit openAddressChanged();

    auto info=NodeConnection::instance()->rest()->get_api_core_v2_info();
    connect(info,&Node_info::finished,this,[=]( ){

        quint32 r1 = QRandomGenerator::global()->generate();
        quint32 r2 = QRandomGenerator::global()->generate();

        m_openAddress=Account::instance()->getAddrBech32({100,r1,r2},info->bech32Hrp);

        emit openAddressChanged();
        resp=NodeConnection::instance()->mqtt()->
               get_outputs_unlock_condition_address("address/"+m_openAddress);

        QObject::connect(resp,&ResponseMqtt::returned,this,[=](QJsonValue data){
            checkNftToOpen(Node_output(data).output());
            resp->deleteLater();
            resp=nullptr;
        });
        info->deleteLater();
    });
}
void BookServer::checkNftToOpen(std::shared_ptr<const qblocks::Output> output)
{
    if(output->type()==Output::NFT_typ)
    {
        const auto issuerfeature=output->get_immutable_feature_(Feature::Issuer_typ);
        if(issuerfeature)
        {
            const auto issuer=std::static_pointer_cast<const Issuer_Feature>(issuerfeature)->issuer()->addr();
            if(issuer==Wallet::instance()->addresses().begin()->second->getAddress()->addr())
            {
                const auto metfeature=output->get_immutable_feature_(Feature::Metadata_typ);
                if(metfeature)
                {
                    const auto metadata=std::static_pointer_cast<const Metadata_Feature>(metfeature)->data();

                    auto books=Booking::metadataToBookings(metadata);

                    auto vec=Booking::fromArray(books);
                    auto now=QDateTime::currentDateTime();

                    for(const auto& book: vec )
                    {
                        if(book.contains(now))
                        {
                            openBox();
                            m_open=true;
                            emit openChanged();
                            break;
                        }
                    }

                }

            }
        }
    }
}
void BookServer::openBox(void)
{
    auto chip = lgGpiochipOpen(0);
    lgGpioSetUser(chip, "esterv");
    auto err = lgGpioClaimOutput(chip, 0, GPIO_NUMBER, 0);
    if (err) qDebug()<<"Set out err"<<err;
    lgGpioWrite(chip, GPIO_NUMBER, 1);
    qDebug()<<"opening gpio:"<<GPIO_NUMBER;
    QTimer::singleShot(2000,this,[=](){
        lgGpioWrite(chip, GPIO_NUMBER, 0);
        lgGpiochipClose(chip);
    });

}



void BookServer::handleNewBook(c_array bookId,std::shared_ptr<const qblocks::Output> bookOut)
{
    qDebug()<<"BookServer::handleNewBook";
    setState(Publishing);
    QTimer::singleShot(15000,this,[=](){setState(Ready);});
    if(bookOut->type()==Output::Basic_typ)
    {
        auto info=NodeConnection::instance()->rest()->get_api_core_v2_info();
        connect(info,&Node_info::finished,this,[=]( ){
            const auto metFea=bookOut->get_feature_(Feature::Metadata_typ);
            if(metFea)
            {
                auto metadata=std::static_pointer_cast<const Metadata_Feature>(metFea)->data();
                const auto sendfeature=bookOut->get_feature_(Feature::Sender_typ);
                if(sendfeature)
                {
                    auto sender=std::static_pointer_cast<const Sender_Feature>(sendfeature)->sender();

                    auto newBooks=Booking::metadataToBookings(metadata);
                    if(!newBooks.isEmpty())
                    {
                        QJsonArray varBooks;
                        auto vec=Booking::fromArray(newBooks);
                        quint64 price=0;
                        quint64 maxB=0;

                        InputSet inputSet;
                        StateOutputs stateOutputs;
                        quint64 consumedAmount=0;
                        quint64 stateAmount=0;
                        consumedAmount+=Wallet::instance()->
                                          consume(inputSet,stateOutputs,0,{Output::Basic_typ},{bookId});
                        qDebug()<<"consumedAmount1:"<<consumedAmount;
                        for(auto i=0;i<vec.size();i++)
                        {
                            if(vec.at(i).isValid()&&vec.at(i).end()<QDateTime::currentDateTime().addDays(7))
                            {
                                auto pair=m_books.insert(vec.at(i));
                                if(pair.second)
                                {
                                    price+=vec.at(i).price(m_pph);

                                    if(consumedAmount<price)
                                    {
                                        m_books.erase(vec.at(i));
                                        price-=vec.at(i).price(m_pph);
                                        break;
                                    }
                                    varBooks.push_back(newBooks.at(2*i));
                                    varBooks.push_back(newBooks.at(2*i+1));
                                    if(newBooks.at(2*i+1).toInteger()>maxB)
                                        maxB=newBooks.at(2*i+1).toInteger();
                                }
                            }
                        }


                        if(!varBooks.isEmpty())
                        {


                            const auto eddAddr=Wallet::instance()->addresses().begin()->second->getAddress();
                            const auto issuerFea=Feature::Issuer(eddAddr);

                            auto addUnlcon=Unlock_Condition::Address(sender);
                            auto expirUnloc=Unlock_Condition::Expiration(maxB+100,eddAddr);
                            auto varretUlock=Unlock_Condition::Storage_Deposit_Return(eddAddr,0);
                            auto metFea=Booking::bookingsToMetadata(varBooks);
                            auto varNftOut= Output::NFT(0,{addUnlcon,varretUlock,expirUnloc},{},{issuerFea,metFea});
                            auto ret_amount=Client::get_deposit(varNftOut,info);

                            auto retUlock=Unlock_Condition::Storage_Deposit_Return(eddAddr,ret_amount);
                            auto NftOut= Output::NFT(0,{addUnlcon,retUlock,expirUnloc},{},{issuerFea,metFea});
                            NftOut->amount_=Client::get_deposit(NftOut,info)+consumedAmount-price;

                            consumedAmount+=Wallet::instance()->
                                              consume(inputSet,stateOutputs,0,{Output::Basic_typ},{m_pubId});
                            cleanState();
                            const auto pubOut=getPublishOutput();
                            qDebug()<<"consumedAmount2:"<<consumedAmount;
                            qDebug()<<"NftOut->amount_:"<<NftOut->amount_;
                            if(consumedAmount>=NftOut->amount_+Client::get_deposit(pubOut,info))
                            {
                                pubOut->amount_=consumedAmount-NftOut->amount_;
                                pvector<const Output> theOutputs{pubOut,NftOut};

                                auto payloadusedids=Wallet::instance()->createTransaction(inputSet,info,theOutputs);
                                auto block=Block(payloadusedids.first);

                                auto transactionid=payloadusedids.first->get_id();

                                auto resp=NodeConnection::instance()->mqtt()->get_subscription("transactions/"+transactionid.toHexString() +"/included-block");
                                connect(resp,&ResponseMqtt::returned,this,[=](auto var){
                                    setState(Ready);
                                    resp->deleteLater();
                                });
                                transactionid.append(quint16(0));
                                m_pubId=transactionid;
                                qDebug()<<"outId:"<<m_pubId.toHexString();
                                NodeConnection::instance()->rest()->send_block(block);

                                m_dayModel->addBooking(HourBox::Occupied,varBooks);

                                qDebug()<<"finished";
                            }
                        }
                    }
                }


            }


            info->deleteLater();
        });


    }
}

void BookServer::handleInitFunds(QString payAddress)
{

    auto info=NodeConnection::instance()->rest()->get_api_core_v2_info();
    QObject::connect(info,&Node_info::finished,this,[=]( ){

        qDebug()<<"BookServer::handleInitFunds";
        cleanState();
        auto pubOut=getPublishOutput();
        const auto minOutputPub=Client::get_deposit(pubOut,info);

        InputSet inputSet;
        StateOutputs stateOutputs;
        quint64 consumedAmount=0;
        quint64 stateAmount=0;
        consumedAmount+=Wallet::instance()->
                          consume(inputSet,stateOutputs,0,{Output::Basic_typ});
        pubOut->amount_=consumedAmount;

        auto Addrpub=Wallet::instance()->addresses().begin()->second->getAddress();

        auto PubUnlcon=Unlock_Condition::Address(Addrpub);
        if(consumedAmount>=minOutputPub)
        {

            pvector<const Output> theOutputs{pubOut};
            auto payloadusedids=Wallet::instance()->createTransaction(inputSet,info,theOutputs);
            auto block=Block(payloadusedids.first);
            qDebug()<<block.get_Json();
            auto transactionid=payloadusedids.first->get_id();
            qDebug()<<"transactionid:"<<transactionid.toHexString();
            auto resp=NodeConnection::instance()->mqtt()->get_subscription("transactions/"+transactionid.toHexString() +"/included-block");
            connect(resp,&ResponseMqtt::returned,this,[=](auto var){
                setState(Ready);
                resp->deleteLater();
            });
            setState(Publishing);
            transactionid.append(quint16(0));
            m_pubId=transactionid;
            NodeConnection::instance()->rest()->send_block(block);
            disconnect(Wallet::instance(),&Wallet::amountChanged,this,nullptr);
            monitorPayments(payAddress);
        }

        info->deleteLater();

    });

}

QByteArray BookServer::serializeState()const
{

    QJsonObject var;
    QJsonArray bookings;
    quint64 duration=0;
    for(const auto& v: m_books)
    {
        bookings.append(v.start().toSecsSinceEpoch());
        bookings.append(v.end().toSecsSinceEpoch());
        duration+=(v.end()-v.start()).count();
    }
    const quint8 occupied=100.0*duration/(7*24*60*60*1000);

    var.insert("o",occupied);
    var.insert("b",bookings);
    var.insert("pph",QString::number(m_pph));
    var.insert("pt",(++Wallet::instance()->addresses().begin())->second->getAddressHash());
    if(m_GeoCoord.isValid())var.insert("c",QJsonArray({m_GeoCoord.latitude(),m_GeoCoord.longitude()}));
    auto state = QJsonDocument(var);
    return state.toJson();
}
