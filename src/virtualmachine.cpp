#include "virtualmachine.h"

#include <QDebug>


namespace UnqliteQt
{

VirtualMachine::VirtualMachine(unqlite_vm* pVm, Database *parent) :
    QObject(parent)
{
    m_pVm = pVm;
}

VirtualMachine::~VirtualMachine() 
{
   if (m_pVm) {
       unqlite_vm_release(m_pVm);
   }
}

VirtualMachine& VirtualMachine::addArgument(const QString& arg)
{
    QByteArray argBytes = arg.toUtf8();
    int rc = unqlite_vm_config(m_pVm, UNQLITE_VM_CONFIG_ARGV_ENTRY, argBytes.data());
    updateLastError(rc);
    return *this;
}

VirtualMachine& VirtualMachine::setVariable(const QString& name, const QVariant& value)
{	
	unqlite_value* pVal = createUnqlite(value);
	if (pVal) {
		QByteArray nameBytes = name.toUtf8();
		int rc = unqlite_vm_config(m_pVm, UNQLITE_VM_CONFIG_CREATE_VAR, nameBytes.data(), pVal);
		unqlite_value_release(pVal);
		updateLastError(rc);
	}
	return *this;
}

QVariant VirtualMachine::extractVariable(const QString& name)
{
	QByteArray nameBytes = name.toUtf8();
	unqlite_value* pVal = unqlite_vm_extract_variable(m_pVm, nameBytes.data());	
	if (!pVal)
		return QVariant();
	QVariant var = createVariant(pVal);
	unqlite_value_release(pVal);
	return var;
}

bool VirtualMachine::execute()
{
	int rc = unqlite_vm_exec(m_pVm);
	updateLastError(rc);
    return rc == UNQLITE_OK;
}

bool VirtualMachine::reset() 
{
    Q_ASSERT(m_pVm);
    int rc = unqlite_vm_reset(m_pVm);
    updateLastError(rc);
    return rc == UNQLITE_OK;
}


unqlite_value* VirtualMachine::createUnqlite(const QVariant& var)
{
	unqlite_value* retval = NULL;
	QVariant::Type type = var.type();
	if (type == QVariant::Map) {
		retval = unqlite_vm_new_array(m_pVm);
		QMap<QString, QVariant> map = var.toMap();
		QMapIterator<QString, QVariant> it(map);
		while (it.hasNext()) {
			it.next();						
			unqlite_value* pVal = createUnqlite(it.value());
			if (pVal) {			
				QString key = it.key();
				const char* pKey = key.toUtf8().data();	
				unqlite_array_add_strkey_elem(retval, pKey, pVal);
				unqlite_vm_release_value(m_pVm, pVal);				
			}
			else {
				qDebug() << "Could not create unqlite value for map";
			}
		}
	}
	else if (type == QVariant::List) {
		retval = unqlite_vm_new_array(m_pVm);
		QList<QVariant> list = var.toList();
		QListIterator<QVariant> it(list);
		while (it.hasNext()) {
			unqlite_value* pVal = createUnqlite(it.next());
			if (pVal) {
				unqlite_array_add_elem(retval, NULL, pVal);
				unqlite_vm_release_value(m_pVm, pVal);
			}
			else {
				qDebug() << "Could not create unqlite value for list";
			}
		}
	}
	else {
		retval = unqlite_vm_new_scalar(m_pVm);
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

QVariant VirtualMachine::createVariant(unqlite_value* pVal)
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
	// else if (unqlite_value_is_json_array(pVal)) {
	// 	unqlite_array_walk(pVal, walk_callback, this);			
	// }
	return QVariant();
}

void VirtualMachine::updateLastError(int rc) {
	m_rc = static_cast<ErrorCode>(rc);
}


}
