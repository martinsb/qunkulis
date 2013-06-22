#ifndef DATABASE_H
#define DATABASE_H

extern "C" 
{
#include "unqlite-db/unqlite.h"
}

#include <QObject>
#include <QString>
#include <QMap>
#include <QList>

namespace UnqliteQt
{

enum ErrorCode
{
	Ok              = UNQLITE_OK,
    NoMemory        = UNQLITE_NOMEM,
    Abort           = UNQLITE_ABORT,
    IoError         = UNQLITE_IOERR,
    CorruptPointer  = UNQLITE_CORRUPT,
    Locked          = UNQLITE_LOCKED,
    Busy            = UNQLITE_BUSY,
    Done            = UNQLITE_DONE,
    PermissionError = UNQLITE_PERM,
    NotImplemented  = UNQLITE_NOTIMPLEMENTED,
    NotFound        = UNQLITE_NOTFOUND,
    NoSuchFunction  = UNQLITE_NOOP,
    Invalid         = UNQLITE_INVALID,
    EndOfInput      = UNQLITE_EOF,
    UnknownError    = UNQLITE_UNKNOWN,
    Limit           = UNQLITE_LIMIT,
    Exists          = UNQLITE_EXISTS,
    Empty           = UNQLITE_EMPTY,
    CompileError    = UNQLITE_COMPILE_ERR,
    VMError         = UNQLITE_VM_ERR,
    Full            = UNQLITE_FULL,
    CannotOpen      = UNQLITE_CANTOPEN,
    IsReadOnly      = UNQLITE_READ_ONLY,
    LockingError    = UNQLITE_LOCKERR
};

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