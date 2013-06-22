#include "database.h"

#include <QDebug>
#include <QVariant>
#include <QList>

using namespace UnqliteQt;

int main(int argc, char** argv)
{
	Database db;

	if (!db.open("test.db"))
		qDebug() << "Could not open database";

	if (!db.store("rofl", QString("copter")))
		qDebug() << "Could not store a simple Key-Value record";

	QString value = db.fetch("rofl");
	qDebug() << "Fetched the same value back:" << value;

	if (db.begin()) {
		QMap<QString, QVariant> map;
		map["foo"] = 1337;

		long long int storedId = db.store("bar", map);
		if (storedId > -1) {
			qDebug() << "Stored a document with id: " << storedId;		
		}
		else {
			qDebug() << "Could not store a variant";		
		}
		if (!db.commit()) {
			db.rollback();
		}

		QList<QVariant> arr;
		arr.append("one");
		arr.append("two");
		arr.append("three");

		map["arr"] = QVariant(arr);

		storedId = db.store("bar", map);
		if (storedId > -1) {
			qDebug() << "Stored a document with id: " << storedId;		
		}
		QVariant rec = db.fetch("bar", storedId);

		qDebug() << "Database has collection 'bar':" << db.hasCollection("bar");
		qDebug() << "Database has collection 'foo':" << db.hasCollection("foo");
	}

	db.close();

	return 0;
}