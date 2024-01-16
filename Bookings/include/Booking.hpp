#pragma once

#include<QByteArray>
#include<QObject>
#include<QString>
#include<QDateTime>
#include<QJsonValue>
#include<QJsonObject>
#include<set>

#include <QtCore/QtGlobal>
#if defined(WINDOWS_BOOKI)
# define BOOKI_EXPORT Q_DECL_EXPORT
#else
#define BOOKI_EXPORT Q_DECL_IMPORT
#endif
class BOOKI_EXPORT Booking : public QJsonObject
{
	public:
		using QJsonObject::QJsonObject;
		Booking(const QJsonObject &var=QJsonObject()):QJsonObject(var){};
		std::vector<QDate> get_days(void)const;
		std::vector<int> get_hours(const QDate& day)const;
		bool check_validity(const QDateTime & ref)const;

		friend bool operator < (const Booking& b1, const Booking& b2)
		{
			return(b1["start"].toInteger()<b2["start"].toInteger()&&b1["finish"].toInteger()<b2["finish"].toInteger());
		}
		friend bool operator < (const Booking& b1, const QDateTime& b2)
		{
			return(QDateTime::fromSecsSinceEpoch(b1["finish"].toInteger())<b2);
		}

		quint64 calculate_price(quint64 per_hour)const;
		static quint64 calculate_price(const QJsonArray &books,quint64 per_hour);
		static std::vector<Booking> from_Array(const QJsonArray &books);
		static QJsonArray get_new_bookings_from_metadata(const QByteArray& metadata);
		static QByteArray create_new_bookings_metadata(const QJsonArray &books);

};

