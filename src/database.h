#ifndef DATABASE_H
#define DATABASE_H

extern "C" 
{
#include "unqlite-db/unqlite.h"
}

#include "constants.h"
#include "virtualmachine.h"

#include <QObject>
#include <QString>
#include <QMap>
#include <QList>

namespace UnqliteQt
{
class VirtualMachine; //forward

class Database: public QObject 
{
public:
	Database(QObject* parent = NULL);
	virtual ~Database();

	bool open(const QString& path, OpenMode openMode = Create);
	void close();



    bool begin();

    bool commit();

    bool rollback();

    bool store(const QString& key, const QString& value);

    QString fetch(const QString& key);

    ErrorCode lastError();

    VirtualMachine* compile(const QString& bytecode);


private: 
    void setErrorCode(int rc);

private:
	unqlite *pDb;

    ErrorCode errorCode;
};

}

#endif //DATABASE_H
