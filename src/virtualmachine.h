#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H


#include <QObject>
#include <QString>
#include <QVariant>

extern "C"
{
#include "unqlite-db/unqlite.h"
}

#include "database.h"

namespace UnqliteQt
{

class Database;


class VirtualMachine : public QObject
{
    Q_OBJECT

public:    
    virtual ~VirtualMachine();

    VirtualMachine& addArgument(const QString& name);

    VirtualMachine& setVariable(const QString& name, const QVariant& value);

    QVariant extractVariable(const QString& name);    

    bool execute();
    bool reset();
    
signals:
    
public slots:

private:
    explicit VirtualMachine(unqlite_vm *pVm, Database *parent = 0);
    friend class Database;

private:
	unqlite_value* createUnqlite(const QVariant& var);
	QVariant createVariant(unqlite_value* pVal);
	void updateLastError(int rc);

private:
	ErrorCode m_rc;
    unqlite_vm* m_pVm;
    
};

}

#endif // VIRTUALMACHINE_H
