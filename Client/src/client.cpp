#include"bclient.hpp"
#include <QCryptographicHash>
#include <QDebug>
#include<QJsonDocument>
#include<QTimer>
#include"nodeConnection.hpp"
#include "encoding/qbech32.hpp"
#include "qwallet.hpp"

using namespace qiota::qblocks;

using namespace qiota;

Server::Server(QString account,c_array outId, const quint64 pph, const c_array payAddress, const QJsonArray bookings,
               const float latitude, const float longitude, const float score, const quint8 occupied, QObject *parent )
    :QObject(parent),m_account(account),m_outId(outId),m_pph(new Qml64(pph,this)),m_payAddress(payAddress),
    m_daymodel(new Day_model(this)),m_latitude(latitude),m_longitude(longitude),m_score(score),m_occupied(occupied)
{
    m_daymodel->add_booking(bookings);
}
Book_Client::Book_Client(QObject *parent):QAbstractListModel(parent)
{
    connect(Wallet::instance(),&Wallet::resetted,this,[=](){
        resetData();
        getServerList();});
}
void Book_Client::getServerList(void)
{
    //*****Testing***************//
    auto nserver=new Server("rms1ddr","sdsdsdsds",1000,"payAddress",{},41.902916,12.453389,3.0,50,this);
    int i = m_servers.size();
    beginInsertRows(QModelIndex(), i, i);
    m_servers.append(nserver);
    emit countChanged(count());
    endInsertRows();
    //*****Testing***************//

    auto info=NodeConnection::instance()->rest()->get_api_core_v2_info();
    connect(info,&Node_info::finished,this,[=]( ){
        auto nodeOutputs=NodeConnection::instance()->rest()->get_outputs<Output::Basic_typ>
                           ("tag="+fl_array<quint8>("stateDLockers").toHexString());
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
void Book_Client::addServer(QString account)
{
    auto info=NodeConnection::instance()->rest()->get_api_core_v2_info();
    connect(info,&Node_info::finished,this,[=]( ){
        auto nodeOutputs=NodeConnection::instance()->rest()->get_outputs<Output::Basic_typ>
                           ("address="+account+"&sender="+account+"&hasStorageDepositReturn=false&hasTimelock=false&hasExpiration=false&tag="+fl_array<quint8>("stateDLockers").toHexString());
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
void Book_Client::getServer(std::shared_ptr<const Output> output,c_array outId, QString hrp)
{
    if(output->type()==qblocks::Output::Basic_typ)
    {
        const auto addrunlock=output->get_unlock_(qblocks::Unlock_Condition::Address_typ)->address()->addr();
        const auto sendfea=output->get_feature_(qblocks::Feature::Sender_typ);
        const auto tagfeau=output->get_feature_(Feature::Tag_typ);
        const auto metfeau=output->get_feature_(Feature::Metadata_typ);
        if(sendfea&&tagfeau&&
            std::static_pointer_cast<const Sender_Feature>(sendfea)->sender()->addr()==addrunlock&&
            std::static_pointer_cast<const Tag_Feature>(tagfeau)->tag()==fl_array<quint8>("stateDLockers")&&metfeau)
        {
            const auto account=qencoding::qbech32::Iota::encode(
                hrp,std::static_pointer_cast<const Sender_Feature>(sendfea)->sender()->addr());
            auto metadata_feature=std::static_pointer_cast<const Metadata_Feature>(metfeau);
            auto metadata=metadata_feature->data();
            deserializeState(account,outId,metadata);
        }
    }
}

void Book_Client::deserializeState(const QString account,const c_array outId,const QByteArray state)
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
    const float longitude=coords.at(0).toDouble();

    const auto score=5.0;
    const auto occupied=0;

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
                v->dayModel()->add_booking(bookarray);
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
        auto resp2=NodeConnection::instance()->mqtt()->get_outputs_outputId(outId.toHexString());
        QObject::connect(resp2,&ResponseMqtt::returned,nserver,[=](QJsonValue data){
            const auto node_output=Node_output(data);
            if(node_output.metadata().is_spent_)
            {
                rmServer(outId);
                resp2->deleteLater();
            }
        });
        int i = m_servers.size();
        beginInsertRows(QModelIndex(), i, i);
        m_servers.append(nserver);
        emit countChanged(count());
        endInsertRows();

    }

}
void Book_Client::resetData(void)
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
void Book_Client::rmServer(c_array outId)
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

int Book_Client::idToIndex(c_array outId)
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
        const auto books=m_daymodel->getNewBookings();
        auto metadata=Booking::create_new_bookings_metadata(books);
        auto metFea=Feature::Metadata(metadata);
        const auto retAddr=Wallet::instance()->addresses().begin()->second->getAddress();
        auto sendFea=Feature::Sender(retAddr);
        auto until_when=QDateTime::currentDateTime().addSecs(60).toSecsSinceEpoch();

        auto expirationUnlock=Unlock_Condition::Expiration(until_when,retAddr);

        auto payToAddress=Address::from_array(m_payAddress);

        auto addrUnlcon=Unlock_Condition::Address(payToAddress);
        auto mineUnlock=Unlock_Condition::Address(retAddr);

        auto price=Booking::calculate_price(books,m_pph->getValue());

        auto mindeposit=Client::get_deposit(Output::Basic(0,{addrUnlcon}),info);

        auto storUnlo=Unlock_Condition::Storage_Deposit_Return(retAddr,mindeposit);

        auto BaOut=Output::Basic(0,{addrUnlcon,storUnlo,expirationUnlock},{},{sendFea,metFea});
        auto storagedeposit=Client::get_deposit(BaOut,info);
        auto requiredAmount=mindeposit+price;
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

        if(consumedAmount>=stateAmount+requiredAmount+mindeposit)
        {
            auto BaOut2=Output::Basic(consumedAmount-stateAmount-requiredAmount,{mineUnlock});
            theOutputs.push_back(BaOut2);

            auto payloadusedids=Wallet::instance()->createTransaction(inputSet,info,theOutputs);
            auto block=Block(payloadusedids.first);

            auto transactionid=payloadusedids.first->get_id();
            NodeConnection::instance()->rest()->send_block(block);

            m_daymodel->add_booking(books,transactionid.toHexString());
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

                    if(!var["bookings"].isUndefined()&&var["bookings"].isArray())
                    {
                        const auto bookarray=var["bookings"].toArray();
                        const auto vec=Booking::from_Array(bookarray);

                        for(const auto& f:vec)
                        {
                            if(f["start"].toInteger()<=now.toSecsSinceEpoch()&&f["finish"].toInteger()>now.toSecsSinceEpoch())
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

QVariant Book_Client::data(const QModelIndex &index, int role) const
{
    return m_servers[index.row()]->property(roleNames().value(role));
}
bool Book_Client::setData(const QModelIndex &index, const QVariant &value, int role )
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
bool Book_Client::setProperty(int i,QString role,const QVariant value)
{
    const auto ind=index(i);
    const auto rol=roleNames().keys(role.toUtf8());
    return setData(ind,value,rol.front());
}
QHash<int, QByteArray> Book_Client::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[scoreRole] = "score";
    roles[occupiedRole] = "occupied";
    roles[latitudeRole] = "latitude";
    roles[longitudeRole] = "longitude";
    roles[pphRole] = "pph";
    roles[DayModelRole] = "DayModel";
    roles[accountRole]= "account";
    return roles;
}
