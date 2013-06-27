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
#include "executionresult.h"

namespace UnqliteQt
{

class Database;


class VirtualMachine : public QObject
{
    Q_OBJECT

public:    
    virtual ~VirtualMachine();

    const ExecutionResult* exec(const QMap<QString, QVariant>& params);
    bool reset();
    
signals:
    
public slots:

private:
    explicit VirtualMachine(unqlite_vm *pVm, Database *parent = 0);
    friend class Database;

private:
    unqlite_vm* m_pVm;
    
};

}

#endif // VIRTUALMACHINE_H
