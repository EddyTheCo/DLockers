#pragma once

#include<QByteArray>
#include<QObject>
#include<QString>
#include<QDateTime>
#include<QJsonValue>
#include<QJsonObject>
#include "block/qfeatures.hpp"

#include <QtCore/QtGlobal>
#if defined(WINDOWS_BOOKI)
# define BOOKI_EXPORT Q_DECL_EXPORT
#else
#define BOOKI_EXPORT Q_DECL_IMPORT
#endif

class BOOKI_EXPORT Booking
{
	public:
    Booking(const qint64 &start,const qint64 & end);
        std::vector<QDate> getDays(void)const;
        std::vector<int> getHours(const QDate& day)const;
        bool checkValidity(const QDateTime & ref)const;
        bool isValid()const;
        bool contains(const QDateTime & ref)const;
        QDateTime start()const{return m_start;}
        QDateTime end()const{return m_end;}
		friend bool operator < (const Booking& b1, const Booking& b2)
		{
            return(b1.m_start<b2.m_start&&b1.m_end<b2.m_end);
		}
		friend bool operator < (const Booking& b1, const QDateTime& b2)
		{
            return(b1.m_end<b2);
		}

        quint64 price(quint64 per_hour)const;
        static quint64 price(const QJsonArray &books,quint64 per_hour);
        static std::vector<Booking> fromArray(const QJsonArray &books);
        static QJsonArray metadataToBookings(const QByteArray metadata);
        static std::shared_ptr<const qiota::qblocks::Feature> bookingsToMetadata(const QJsonArray &books);

    private:
        QDateTime m_start,m_end;

};

