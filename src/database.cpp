#include "database.h"

#include <QDebug>

#include <QResource>

namespace UnqliteQt
{

// int output_callback(const void* pOutput, unsigned int nLen, void* pUserData)
// {
// 	QString output = QString::fromUtf8(reinterpret_cast<const char*>(pOutput), nLen);
// 	qDebug() << output;
// 	return UNQLITE_OK;
// }

// int walk_callback(unqlite_value* pKey, unqlite_value* pValue, void *pUserData)
// {

// 	int nLen;
// 	const char* pKeyBytes = unqlite_value_to_string(pKey, &nLen);
// 	QString key = QString::fromUtf8(pKeyBytes, nLen);
// 	qDebug() << "Array key:" << key; 	

// 	if (pUserData) {
// 		Database* p = reinterpret_cast<Database*>(pUserData);
// 	}


// 	return UNQLITE_OK;
// }


Database::Database(QObject* parent)
	: QObject(parent), pDb(0)
{

}

Database::~Database()
{
	if (pDb)
		close();
}

bool Database::open(const QString& path, OpenMode openMode)
{
	int rc = unqlite_open(&pDb, path.toLocal8Bit().data(), static_cast<unsigned int>(openMode));

	qDebug() << "Opened DB, result code: " << rc;

	setErrorCode(rc);

	return rc == UNQLITE_OK;
}

void Database::close()
{	
	qDebug() << "Closing database";
	unqlite_close(pDb);
	pDb = NULL;
}

bool Database::begin()
{
	Q_ASSERT(pDb != NULL);
	int rc = unqlite_begin(pDb);
	setErrorCode(rc);
	return rc == UNQLITE_OK;
}

bool Database::commit() 
{
	Q_ASSERT(pDb != NULL);
	int rc = unqlite_commit(pDb);
	setErrorCode(rc);
	return rc == UNQLITE_OK;
}

bool Database::rollback()
{
	Q_ASSERT(pDb != NULL);
	int rc = unqlite_rollback(pDb);
	setErrorCode(rc);
	return rc == UNQLITE_OK;
}

bool Database::store(const QString& key, const QString& value)
{
	QByteArray keyBytes = key.toUtf8();
	QByteArray valueBytes = value.toUtf8();
	int rc = unqlite_kv_store(pDb, keyBytes.data(), keyBytes.length(), valueBytes.data(), valueBytes.length());
	setErrorCode(rc);
	return rc == UNQLITE_OK;
}

QString Database::fetch(const QString& key) 
{
	QByteArray keyBytes = key.toUtf8();
	unqlite_int64 bufLen;
	int rc = unqlite_kv_fetch(pDb, keyBytes.data(), keyBytes.length(), NULL, &bufLen);	
	if (rc != UNQLITE_OK) {
		setErrorCode(rc);
		return QString();
	}
	char *buf = reinterpret_cast<char*>(malloc(bufLen));
	rc = unqlite_kv_fetch(pDb, keyBytes.data(), keyBytes.length(), buf, &bufLen);
	setErrorCode(rc);
	if (rc != UNQLITE_OK) {
		free(buf);
		return QString();
	}
	QString returned = QString::fromUtf8(buf, bufLen);
	free(buf);
	return returned;	
}


ErrorCode Database::lastError() {
	return errorCode;
}

void Database::setErrorCode(int rc)
{
	errorCode = static_cast<ErrorCode>(rc);
}

VirtualMachine* Database::compile(const QString& bytecode)
{
    unqlite_vm* pVm = NULL;
    QByteArray bytes = bytecode.toUtf8();
    int rc = unqlite_compile(pDb, bytes.data(), bytes.length(), &pVm);
    setErrorCode(rc);
    if (rc == UNQLITE_OK) {
        return new VirtualMachine(pVm, this);
    }
    return NULL;
} 

}
