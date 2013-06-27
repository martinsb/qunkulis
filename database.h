#ifndef DATABASE_H
#define DATABASE_H

extern "C" 
{
#include "unqlite-db/unqlite.h"
}

#include "errorcode.h"
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

	bool open(const QString& path);
	void close();



    bool begin();

    bool commit();

    bool rollback();

    bool store(const QString& key, const QString& value);

    long long int store(const QString& key, const QMap<QString, QVariant>& document);

    QString fetch(const QString& key);

    bool hasCollection(const QString& name);

    ErrorCode lastError();

    QVariant fetch(const QString& collection, long long int id);
    QList<QVariant> fetchAll(const QString& collection);

    VirtualMachine* compile(const QString& bytecode);


private: 
    void setErrorCode(int rc);

    unqlite_value* createUnqlite(unqlite_vm* pVm, const QVariant& var);

    QVariant createVariant(unqlite_value* pVal);

    unqlite_vm* getVM(const QString& key);

private:
	unqlite *pDb;

    ErrorCode errorCode;

    QMap<QString, unqlite_vm*> bytecodeCache;
};

}

#endif //DATABASE_H
