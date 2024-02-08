#include"client.hpp"
#include <QCryptographicHash>
#include <QDebug>
#include<QJsonDocument>
#include<QTimer>
#include"nodeConnection.hpp"
#include "encoding/qbech32.hpp"
#include "qwallet.hpp"
#include "account.hpp"
using namespace qiota::qblocks;

using namespace qiota;

Server::Server(QString account,c_array outId, const quint64 pph, const c_array payAddress, const QJsonArray bookings,
               const float latitude, const float longitude, const float score, const quint8 occupied, QObject *parent )
    :QObject(parent),m_account(account),m_outId(outId),m_pph(pph),m_price(new Qml64(0,this)),m_payAddress(payAddress),
    m_dayModel(new DayModel(this)),m_latitude(latitude),m_longitude(longitude),m_score(score),m_occupied(occupied)
{
    m_dayModel->addBooking(HourBox::Occupied,bookings);
    connect(m_dayModel,&DayModel::totalSelectedChanged,m_price,[=](){
        m_price->setValue(m_pph*m_dayModel->totalSelected());
    });

}
BookClient::BookClient(QObject *parent):QAbstractListModel(parent),m_selected(-1)
{
    Account::instance()->setVaultFile(
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)+"/dlockerClient/qvault.bin");

    connect(Wallet::instance(),&Wallet::synced,this,[=](){
        resetData();
        getServerList();

    });

}
void BookClient::getBookings()
{
    const auto nfts=Wallet::instance()->getNFTs();
    for(const auto& v: nfts)
    {
        checkNFTforBook(v.second);
    }
    connect(Wallet::instance(),&Wallet::inputAdded,this,[=](c_array id){
        checkNFTforBook(Wallet::instance()->getInput(id).output);
    });
}
void BookClient::checkNFTforBook(std::shared_ptr<const qblocks::Output> output)
{

    if(output->type()==Output::NFT_typ)
    {
        const auto issuerfeature=output->get_immutable_feature_(Feature::Issuer_typ);
        if(issuerfeature)
        {
            const auto issuer=std::static_pointer_cast<const Issuer_Feature>(issuerfeature)->issuer()->addr();
            const auto metfeature=output->get_immutable_feature_(Feature::Metadata_typ);
            if(metfeature)
            {
                const auto metadata=std::static_pointer_cast<const Metadata_Feature>(metfeature)->data();
                auto books=Booking::metadataToBookings(metadata);
                if(!books.isEmpty())
                {
                    for(auto& v:m_servers)
                    {
                        const auto addr_pair=qencoding::qbech32::Iota::decode(v->account());
                        if(addr_pair.second==issuer)
                        {
                            v->dayModel()->addBooking(HourBox::Booked,books);
                        }
                    }
                }


            }
        }
    }
}
void BookClient::setSelected(int ind)
{
    if(ind<m_servers.size())
    {
        m_selected=ind;
        emit selectedChanged();
    }

}
void BookClient::getServerList(void)
{
    //*****Testing***************//
    /*auto nserver=new Server("rms1qqzpht5ur3m43ynz88rqeny83zzjueq77phgm97geyqwcmmju6keyq37s7s",
                              c_array::fromHexString("0x4623856d6b79407a52052a65729f907588d30f7e0ad2a28b4efd66e2945435440000"),
                              1000,
                              c_array::fromHexString("0x007111d514f02adb5155cdf460afd3c39e565bd3fd1c22d3bd4201a373ad76130d"),
                              {},41.902916,12.453389,3.0,50,this);
    int i = m_servers.size();
    beginInsertRows(QModelIndex(), i, i);
    m_servers.append(nserver);
    emit countChanged(count());
    endInsertRows();
*/
    //*****Testing***************//

    auto info=NodeConnection::instance()->rest()->get_api_core_v2_info();
    connect(info,&Node_info::finished,this,[=]( ){
        auto nodeOutputs=NodeConnection::instance()->rest()->get_outputs<Output::Basic_typ>
                           ("tag="+fl_array<quint8>("DLockers").toHexString()+"&createdAfter="+QString::number(QDateTime::currentSecsSinceEpoch()-60*60));
        const auto hrp=info->bech32Hrp;
        connect(nodeOutputs,&Node_outputs::finished,this,[=]( ){
            for(const auto & v:nodeOutputs->outs_)
            {
                const auto output=v.output();
                getServer(output,v.metadata().outputid_,hrp);
            }
            getBookings();
            nodeOutputs->deleteLater();
        });
        info->deleteLater();
    });
}
void BookClient::addServer(QString account)
{
    auto info=NodeConnection::instance()->rest()->get_api_core_v2_info();
    connect(info,&Node_info::finished,this,[=]( ){
        auto nodeOutputs=NodeConnection::instance()->rest()->get_outputs<Output::Basic_typ>
                           ("address="+account+"&sender="+account+"&hasStorageDepositReturn=false&hasTimelock=false&hasExpiration=false&tag="+fl_array<quint8>("DLockers").toHexString());
        const auto hrp=info->bech32Hrp;
        connect(nodeOutputs,&Node_outputs::finished,this,[=]( ){
            for(const auto & v:nodeOutputs->outs_)
            {
                const auto output_=v.output();
                getServer(output_,v.metadata().outputid_,hrp);
            }
            nodeOutputs->deleteLater();
        });
        info->deleteLater();
    });
}
void BookClient::getServer(std::shared_ptr<const Output> output,c_array outId, QString hrp)
{
    if(output->type()==qblocks::Output::Basic_typ)
    {
        const auto addrunlock=output->get_unlock_(qblocks::Unlock_Condition::Address_typ)->address()->addr();
        const auto sendfea=output->get_feature_(qblocks::Feature::Sender_typ);
        const auto tagfeau=output->get_feature_(Feature::Tag_typ);
        const auto metfeau=output->get_feature_(Feature::Metadata_typ);
        if(sendfea&&tagfeau&&
            std::static_pointer_cast<const Sender_Feature>(sendfea)->sender()->addr()==addrunlock&&
            std::static_pointer_cast<const Tag_Feature>(tagfeau)->tag()==fl_array<quint8>("DLockers")&&metfeau)
        {
            const auto account=qencoding::qbech32::Iota::encode(
                hrp,std::static_pointer_cast<const Sender_Feature>(sendfea)->sender()->addr());
            auto metadata_feature=std::static_pointer_cast<const Metadata_Feature>(metfeau);
            auto metadata=metadata_feature->data();
            deserializeState(account,outId,metadata);
        }
    }
}

void BookClient::deserializeState(const QString account,const c_array outId,const QByteArray state)
{
    const auto var=QJsonDocument::fromJson(state).object();
    if(var["pph"].isUndefined())return;
    const auto pricePerHour=var["pph"].toString().toULongLong();
    if(var["pt"].isUndefined())return;
    const auto payAddress=c_array::fromHexString(var["pt"].toString());
    if(var["c"].isUndefined()||!var["c"].isArray())return;
    const auto coords=var["c"].toArray();

    if(coords.size()<2||!coords.at(0).isDouble()||!coords.at(1).isDouble())return;
    const float latitude=coords.at(0).toDouble();
    const float longitude=coords.at(1).toDouble();

    const auto score=5.0;
    
    const quint8 occupied=(var["o"].isUndefined())?0:var["o"].toInt();

    if(!var["b"].isUndefined()&&var["b"].isArray())
    {
        const auto bookarray=var["b"].toArray();
        bool exist=false;
        for(const auto& v:m_servers)
        {
            if(v->account()==account)
            {
                v->setOutId(outId);
                v->setPph(pricePerHour);
                v->setPayAddress(payAddress);
                v->setScore(score);
                v->dayModel()->addBooking(HourBox::Occupied,bookarray);
                v->setOccupied(occupied);
                v->setLatitude(latitude);
                v->setLongitude(longitude);
                return;
            }
        }
        auto nserver=new Server(account,outId,pricePerHour,payAddress,bookarray,latitude,longitude,score,occupied,this);

        connect(nserver,&Server::scoreChanged,this,[=]{
            const auto ind=idToIndex(nserver->outId());
            if(ind>-1)
                emit dataChanged(index(ind),index(ind),QList<int>{ModelRoles::scoreRole});
        });
        connect(nserver,&Server::occupiedChanged,this,[=]{
            const auto ind=idToIndex(nserver->outId());
            if(ind>-1)
                emit dataChanged(index(ind),index(ind),QList<int>{ModelRoles::occupiedRole});
        });
        connect(nserver,&Server::latitudeChanged,this,[=]{
            const auto ind=idToIndex(nserver->outId());
            if(ind>-1)
                emit dataChanged(index(ind),index(ind),QList<int>{ModelRoles::latitudeRole});
        });
        connect(nserver,&Server::longitudeChanged,this,[=]{
            const auto ind=idToIndex(nserver->outId());
            if(ind>-1)
                emit dataChanged(index(ind),index(ind),QList<int>{ModelRoles::longitudeRole});
        });

        auto resp=NodeConnection::instance()->mqtt()->
                    get_outputs_unlock_condition_address("address/"+account);
        QObject::connect(resp,&ResponseMqtt::returned,nserver,[=](QJsonValue data){
            const auto nodeOutput=Node_output(data);
            getServer(nodeOutput.output(),nodeOutput.metadata().outputid_,qencoding::qbech32::Iota::decode(account).first);
        });

        int i = m_servers.size();
        beginInsertRows(QModelIndex(), i, i);
        m_servers.append(nserver);
        emit countChanged(count());
        endInsertRows();

    }

}
void BookClient::resetData(void)
{
    for(auto ind=0;ind<count();ind++)
    {
        m_servers[ind]->deleteLater();
        m_servers.remove(ind);
    }
    beginRemoveRows(QModelIndex(),0,count());
    emit countChanged(count());
    endRemoveRows();
}
void BookClient::rmServer(c_array outId)
{
    const auto ind=idToIndex(outId);
    if(ind>-1)
    {
        beginRemoveRows(QModelIndex(),ind,ind);
        m_servers[ind]->deleteLater();
        m_servers.remove(ind);
        emit countChanged(count());
        endRemoveRows();
    }

}

int BookClient::idToIndex(c_array outId)
{
    for(auto i=0;i<m_servers.size();i++)
    {
        if(m_servers[i]->outId()==outId)
        {
            return i;
        }
    }
    return -1;
}
void Server::sendBookings()
{
    auto info=NodeConnection::instance()->rest()->get_api_core_v2_info();
    QObject::connect(info,&Node_info::finished,this,[=]( ){
        const auto books=m_dayModel->getNewBookings();

        auto metFea=Booking::bookingsToMetadata(books);

        const auto retAddr=Wallet::instance()->addresses().begin()->second->getAddress();
        auto sendFea=Feature::Sender(retAddr);

        auto untilWhen=QDateTime::currentDateTime().addSecs(60).toSecsSinceEpoch();

        auto expirationUnlock=Unlock_Condition::Expiration(untilWhen,retAddr);

        auto payToAddress=Address::Ed25519(m_payAddress);


        auto addrUnlcon=Unlock_Condition::Address(payToAddress);

        auto mineUnlock=Unlock_Condition::Address(retAddr);

        auto price=Booking::price(books,m_pph);

        auto mindeposit=Client::get_deposit(Output::Basic(0,{addrUnlcon}),info);

        auto storUnlo=Unlock_Condition::Storage_Deposit_Return(retAddr,mindeposit);

        auto BaOut=Output::Basic(0,{addrUnlcon,storUnlo,expirationUnlock},{},{sendFea,metFea});
        auto storagedeposit=Client::get_deposit(BaOut,info);
        auto requiredAmount=mindeposit+price;

        qDebug()<<"price:"<<price;
        qDebug()<<"mindeposit:"<<mindeposit;
        qDebug()<<"storagedeposit:"<<storagedeposit;
        BaOut->amount_=requiredAmount;
        if(requiredAmount<storagedeposit)
        {
            requiredAmount=storagedeposit;
            storUnlo=Unlock_Condition::Storage_Deposit_Return(retAddr,requiredAmount-price);
            BaOut=Output::Basic(requiredAmount,{addrUnlcon,storUnlo,expirationUnlock},{},{sendFea,metFea});
        }

        InputSet inputSet;
        StateOutputs stateOutputs1;
        pvector<const Output> theOutputs{BaOut};

        auto consumedAmount=Wallet::instance()->
                              consume(inputSet,stateOutputs1,0,{Output::Basic_typ});
        quint64 stateAmount=0;
        for (auto i = stateOutputs1.cbegin(), end = stateOutputs1.cend(); i != end; ++i)
        {
            auto out=i.value().output->clone();
            out->consume();
            out->unlock_conditions_={mineUnlock};    //Fix: Add state transition is if alias

            const auto cdep=NodeConnection::instance()->rest()->get_deposit(out,info);
            out->amount_=cdep;
            stateAmount+=cdep;
            theOutputs.push_back(out);
        }
        qDebug()<<"consumedAmount:"<<consumedAmount;
        qDebug()<<"stateAmount:"<<stateAmount;
        if(consumedAmount>=stateAmount+requiredAmount+mindeposit)
        {
            auto BaOut2=Output::Basic(consumedAmount-stateAmount-requiredAmount,{mineUnlock});
            theOutputs.push_back(BaOut2);

            auto payloadusedids=Wallet::instance()->createTransaction(inputSet,info,theOutputs);
            auto block=Block(payloadusedids.first);
            m_dayModel->addBooking(HourBox::Booking,books);
            auto transactionid=payloadusedids.first->get_id();
            NodeConnection::instance()->rest()->send_block(block);


        }
        info->deleteLater();

    });

}
void Server::presentNft(const QString address)
{
    const auto presentAddrArray=qencoding::qbech32::Iota::decode(address);

    if(presentAddrArray.second.size())
    {
        const auto accountAddrArray=qencoding::qbech32::Iota::decode(m_account);
        auto nfts=Wallet::instance()->getNFTs();
        c_array nftid;
        const auto now=QDateTime::currentDateTime();
        for(const auto&v: nfts)
        {
            auto issuerfea=v.second->get_immutable_feature_(Feature::Issuer_typ);
            if(std::static_pointer_cast<const Issuer_Feature>(issuerfea)->issuer()->addr()==accountAddrArray.second)
            {
                const auto metfeau=v.second->get_immutable_feature_(Feature::Metadata_typ);

                if(metfeau)
                {
                    auto metadata_feature=std::static_pointer_cast<const Metadata_Feature>(metfeau);
                    auto metadata=metadata_feature->data();

                    const auto var=QJsonDocument::fromJson(metadata).object();

                    if(!var["b"].isUndefined()&&var["b"].isArray())
                    {
                        const auto bookarray=var["b"].toArray();
                        const auto vec=Booking::fromArray(bookarray);

                        for(const auto& f:vec)
                        {
                            if(f.contains(now))
                            {
                                sendNFT(v.first,presentAddrArray.second);
                                return;
                            }
                        }
                    }

                }
            }
        }
        emit noValidNftFound();

    }
}
void Server::sendNFT(c_array id,c_array addressArray)const
{
    auto sendToAddress=Address::from_array(addressArray);

    auto info=NodeConnection::instance()->rest()->get_api_core_v2_info();
    connect(info,&Node_info::finished,this,[=]( ){

        const auto mineAddr=Wallet::instance()->addresses().begin()->second->getAddress();
        const auto mineUnlock=Unlock_Condition::Address(mineAddr);
        const auto expirunlock=Unlock_Condition::Expiration(1000,mineAddr);
        const auto sendUnlock=Unlock_Condition::Address(sendToAddress);

        InputSet inputSet;
        StateOutputs stateOutputs1;
        auto consumedAmount=Wallet::instance()->
                              consume(inputSet,stateOutputs1,0,{Output::NFT_typ},{id});
        quint64 stateAmount=0;
        pvector<const Output> theOutputs;
        for (auto i = stateOutputs1.cbegin(), end = stateOutputs1.cend(); i != end; ++i)
        {
            auto out=i.value().output->clone();
            out->consume();
            if(i.key()==id)
            {
                out->unlock_conditions_={sendUnlock,expirunlock};    //Fix: Add state transition is if alias
            }
            else
            {
                out->unlock_conditions_={mineUnlock};    //Fix: Add state transition is if alias
            }
            const auto cdep=NodeConnection::instance()->rest()->get_deposit(out,info);
            out->amount_=cdep;
            stateAmount+=cdep;
            theOutputs.push_back(out);
        }
        auto BaOut=Output::Basic(0,{mineUnlock});
        auto mindeposit=Client::get_deposit(BaOut,info);
        if(consumedAmount<stateAmount+mindeposit)
        {
            StateOutputs stateOutputs2;
            consumedAmount+=Wallet::instance()->
                              consume(inputSet,stateOutputs2,0,{Output::All_typ});
            for (auto i = stateOutputs2.cbegin(), end = stateOutputs2.cend(); i != end; ++i)
            {
                auto out=i.value().output->clone();
                out->consume();
                out->unlock_conditions_={mineUnlock};    //Fix: Add state transition is if alias
                const auto cdep=NodeConnection::instance()->rest()->get_deposit(out,info);
                out->amount_=cdep;
                stateAmount+=cdep;
                theOutputs.push_back(out);
            }
        }
        if(consumedAmount>=stateAmount+mindeposit)
        {
            BaOut->amount_=consumedAmount-stateAmount;
            theOutputs.push_back(BaOut);
            auto payloadusedids=Wallet::instance()->createTransaction(inputSet,info,theOutputs);
            auto block=Block(payloadusedids.first);

            auto transactionid=payloadusedids.first->get_id();
            NodeConnection::instance()->rest()->send_block(block);
        }
    });
}

QVariant BookClient::data(const QModelIndex &index, int role) const
{
    return m_servers[index.row()]->property(roleNames().value(role));
}
bool BookClient::setData(const QModelIndex &index, const QVariant &value, int role )
{
    if(index.row()<m_servers.size())
    {
        const auto re=m_servers[index.row()]->setProperty(roleNames().value(role),value);

        if(re)
        {
            emit dataChanged(index,index,QList<int>{role});
            return true;
        }
    }
    return false;
}

bool BookClient::setProperty(int i,QString role,const QVariant value)
{
    const auto ind=index(i);
    const auto rol=roleNames().keys(role.toUtf8());
    return setData(ind,value,rol.front());
}
QHash<int, QByteArray> BookClient::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[scoreRole] = "score";
    roles[occupiedRole] = "occupied";
    roles[latitudeRole] = "latitude";
    roles[longitudeRole] = "longitude";
    roles[priceRole] = "price";
    roles[dayModelRole] = "dayModel";
    roles[accountRole]= "account";
    return roles;
}
