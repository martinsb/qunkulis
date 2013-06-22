#include "database.h"

#include <QDebug>

#include <QResource>

namespace UnqliteQt
{

int output_callback(const void* pOutput, unsigned int nLen, void* pUserData)
{
	QString output = QString::fromUtf8(reinterpret_cast<const char*>(pOutput), nLen);
	qDebug() << output;
	return UNQLITE_OK;
}

int walk_callback(unqlite_value* pKey, unqlite_value* pValue, void *pUserData)
{

	int nLen;
	const char* pKeyBytes = unqlite_value_to_string(pKey, &nLen);
	QString key = QString::fromUtf8(pKeyBytes, nLen);
	qDebug() << "Array key:" << key; 	

	if (pUserData) {
		Database* p = reinterpret_cast<Database*>(pUserData);
	}


	return UNQLITE_OK;
}


Database::Database(QObject* parent)
	: QObject(parent), pDb(0)
{

}

Database::~Database()
{
	if (pDb)
		close();
}

bool Database::open(const QString& path)
{
	int rc = unqlite_open(&pDb, path.toLocal8Bit().data(), UNQLITE_OPEN_CREATE);

	qDebug() << "Opened DB, result code: " << rc;

	setErrorCode(rc);

	return rc == UNQLITE_OK;
}

void Database::close()
{
	qDebug() << "Destroying cached VM bytecodes";
	QMapIterator<QString, unqlite_vm*> it(bytecodeCache);
	while (it.hasNext()) {
		it.next();
		unqlite_vm_release(it.value());
	}
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

bool Database::hasCollection(const QString& name)
{
	Q_ASSERT(pDb != NULL);

	unqlite_vm *pVm = getVM(":/jx9/exists.jx9");

	unqlite_value *pCol = createUnqlite(pVm, QVariant(name));
	if (!pCol) {
		qDebug() << "Failed to create collection name variable";
		//TODO what to do here? throw an exception?
		return false;
	}

	int rc = unqlite_vm_config(pVm, UNQLITE_VM_CONFIG_CREATE_VAR, "zCol", pCol);	

	unqlite_vm_release_value(pVm, pCol);

	setErrorCode(rc);

	if (rc != UNQLITE_OK) {
		qDebug() << "Failed to assign the collection name variable to the VM";
		return false;
	}

	unqlite_vm_config(pVm, UNQLITE_VM_CONFIG_OUTPUT, output_callback, NULL);

	rc = unqlite_vm_exec(pVm);

	setErrorCode(rc);

	if (rc != UNQLITE_OK) {
		qDebug() << "Failed to execute VM" << rc;
		return false;
	}

	unqlite_value* pExists = unqlite_vm_extract_variable(pVm, "exists");
	if (!pExists) {
		qDebug() << "Failed to extract the return variable";
		//TODO throw?
		return false;
	}

	int exists = unqlite_value_to_bool(pExists);

	unqlite_vm_release_value(pVm, pExists);

	return exists != 0;
}

bool Database::store(const QString& key, const QString& value)
{
	QByteArray keyBytes = key.toUtf8();
	QByteArray valueBytes = value.toUtf8();
	int rc = unqlite_kv_store(pDb, keyBytes.data(), keyBytes.length(), valueBytes.data(), valueBytes.length());
	setErrorCode(rc);
	return rc == UNQLITE_OK;
}

long long int Database::store(const QString& key, const QMap<QString, QVariant>& document)
{
	int rc;
	unqlite_vm *pVm = NULL;
	unqlite_value* pTableName = NULL;
	unqlite_value* pVal = NULL;
	long long int ret = -1LL;
	try {
		pVm = getVM(":/jx9/store.jx9");
		if (!pVm) {
			qDebug() << "Failed to compile script";
			return false;
		}

		pTableName = unqlite_vm_new_scalar(pVm);
		if (!pTableName) {			
			qDebug() << "Could not create unqlite scalar value";
			return false;		
		}

		QByteArray keyBytes = key.toUtf8();
		rc = unqlite_value_string(pTableName, keyBytes.data(), keyBytes.length());
		if (rc != UNQLITE_OK) {
			qDebug() << "Could not write value to the table name";			
			throw rc;
		}

		unqlite_value* pVal = createUnqlite(pVm, QVariant(document));	
		if (!pVal) {
			qDebug() << "Failed to create unqlite value";
			throw 1;
		}

		rc = unqlite_vm_config(pVm, UNQLITE_VM_CONFIG_OUTPUT, output_callback, NULL);
		if (rc != UNQLITE_OK) {
			qDebug() << "Failed to install an output callback";
			throw rc;
		}

		rc = unqlite_vm_config(pVm, UNQLITE_VM_CONFIG_CREATE_VAR, "zRec", pVal);
		if (rc != UNQLITE_OK) {
			qDebug() << "Failed to add record variable";
			throw rc;
		}		

		rc = unqlite_vm_config(pVm, UNQLITE_VM_CONFIG_CREATE_VAR, "zTable", pTableName);
		if (rc != UNQLITE_OK) {
			qDebug() << "Failed to add collection name variable";
			throw rc;
		}

		rc = unqlite_vm_exec(pVm);
		if (rc != UNQLITE_OK) {
			qDebug() << "Failed to exec the script";
			throw rc;
		}

		unqlite_value* pLastId = unqlite_vm_extract_variable(pVm, "last_id");
		if (pLastId) {
			unqlite_int64 id = unqlite_value_to_int64(pLastId);
			unqlite_vm_release_value(pVm, pLastId);
			ret = static_cast<long long int>(id);
		}
	}
	catch (int caught) {
		if (caught <= 0) {
			setErrorCode(caught);
		}
		//do nothing, just clean up afterwards
	}

	//cleanup
	if (pTableName)
		unqlite_vm_release_value(pVm, pTableName);
	if (pVal)
		unqlite_vm_release_value(pVm, pVal);

	return ret;
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

QVariant Database::fetch(const QString& collection, long long int id)
{
	unqlite_value* pCol = NULL;
	unqlite_value* pId = NULL;
	unqlite_vm* pVm = getVM(":/jx9/fetch-by-id.jx9");
	if (!pVm) {
		qDebug() << "Could not create execution VM";
		return QVariant();
	}

	try {
		pCol = unqlite_vm_new_scalar(pVm);
		if (!pCol) {
			qDebug() << "Could not create collection name argument";
			throw UNQLITE_NOMEM;			
		}
		QByteArray collBytes = collection.toUtf8();
		int rc = unqlite_value_string(pCol, collBytes.data(), collBytes.length());
		if (rc != UNQLITE_OK) {
			qDebug() << "Could not set value for the collection name argument";
			throw rc;
		}

		pId = unqlite_vm_new_scalar(pVm);
		if (!pId) {
			qDebug() << "Could not create record id argument";
			throw UNQLITE_NOMEM;
		}
		rc = unqlite_value_int64(pId, static_cast<unqlite_int64>(id));
		if (rc != UNQLITE_OK) {
			qDebug() << "Could not set value for the record id argument";
			throw rc;
		}

		rc = unqlite_vm_config(pVm, UNQLITE_VM_CONFIG_CREATE_VAR, "zCol", pCol);
		if (rc != UNQLITE_OK) {
			qDebug() << "Could not assign column name argument to the VM";
			throw rc;
		}

		rc = unqlite_vm_config(pVm, UNQLITE_VM_CONFIG_CREATE_VAR, "id", pId);
		if (rc != UNQLITE_OK) {
			qDebug() << "Could not assign record id argument to the VM";
			throw rc;
		}

		rc = unqlite_vm_exec(pVm);
		if (rc != UNQLITE_OK) {
			qDebug() << "Could not execute VM";
			throw rc;
		}		

		unqlite_value* pRec = unqlite_vm_extract_variable(pVm, "zRec");
		if (pRec) {
			QVariant var = createVariant(pRec);
			unqlite_vm_release_value(pVm, pRec);
		}
	}
	catch (int caught) {
		if (caught <= 0) {
			setErrorCode(caught);
		}
	}

	if (pId)
		unqlite_vm_release_value(pVm, pId);

	if (pCol)
		unqlite_vm_release_value(pVm, pCol);

	return QVariant();
}

QList<QVariant> Database::fetchAll(const QString& collection) 
{
	QList<QVariant> list;
	return list;
}

ErrorCode Database::lastError() {
	return errorCode;
}

void Database::setErrorCode(int rc)
{
	errorCode = static_cast<ErrorCode>(rc);
}

unqlite_vm* Database::getVM(const QString& key)
{		
	if (bytecodeCache.contains(key)) {
		unqlite_vm* pVm = bytecodeCache[key];
		/*Most likely, it has been executed anyway*/
		unqlite_vm_reset(pVm);
		return pVm;
	}
	QResource r(key);

	unqlite_vm* pVm = NULL;
	int rc = unqlite_compile(pDb, reinterpret_cast<const char*>(r.data()), r.size(), &pVm);
	setErrorCode(rc);
	if (rc == UNQLITE_OK) {
		bytecodeCache[key] = pVm;
	}
	return pVm;
}

QVariant Database::createVariant(unqlite_value* pVal)
{
	if (unqlite_value_is_bool(pVal)) {
		int val = unqlite_value_to_bool(pVal);
		return QVariant(val != 0 ? true : false);
	}
	else if (unqlite_value_is_numeric(pVal)) {
		return QVariant(unqlite_value_to_int(pVal));
	}
	else if (unqlite_value_is_string(pVal)) {
		int nLen;
		const char* zVal = unqlite_value_to_string(pVal, &nLen);
		return QVariant(QString::fromUtf8(zVal, nLen));
	}
	else if (unqlite_value_is_json_array(pVal)) {
		unqlite_array_walk(pVal, walk_callback, this);			
	}
	return QVariant();
}

unqlite_value* Database::createUnqlite(unqlite_vm* pVm, const QVariant& var)
{
	unqlite_value* retval = NULL;
	QVariant::Type type = var.type();
	if (type == QVariant::Map) {
		retval = unqlite_vm_new_array(pVm);
		QMap<QString, QVariant> map = var.toMap();
		QMapIterator<QString, QVariant> it(map);
		while (it.hasNext()) {
			it.next();						
			unqlite_value* pVal = createUnqlite(pVm, it.value());
			if (pVal) {			
				QString key = it.key();
				const char* pKey = key.toUtf8().data();	
				unqlite_array_add_strkey_elem(retval, pKey, pVal);
				unqlite_vm_release_value(pVm, pVal);				
			}
			else {
				qDebug() << "Could not create unqlite value for map";
			}
		}
	}
	else if (type == QVariant::List) {
		retval = unqlite_vm_new_array(pVm);
		QList<QVariant> list = var.toList();
		QListIterator<QVariant> it(list);
		while (it.hasNext()) {
			unqlite_value* pVal = createUnqlite(pVm, it.next());
			if (pVal) {
				unqlite_array_add_elem(retval, NULL, pVal);
				unqlite_vm_release_value(pVm, pVal);
			}
			else {
				qDebug() << "Could not create unqlite value for list";
			}
		}
	}
	else {
		retval = unqlite_vm_new_scalar(pVm);
		if (type == QVariant::Int || type == QVariant::UInt) {
			unqlite_value_int(retval, var.toInt());
		}
		else if (type == QVariant::Double) {
			unqlite_value_double(retval, var.toDouble());
		}
		else if (type == QVariant::Bool) {
			unqlite_value_bool(retval, var.toBool());
		}
		else if (type == QVariant::String) {
			QByteArray bytes = var.toString().toUtf8();
			unqlite_value_string(retval, bytes.data(), bytes.length());
		}
		else {
			qDebug() << "Could not determine type of the given QVariant" << var;
		}
	}
	return retval;
}
}
