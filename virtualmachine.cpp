#include "virtualmachine.h"


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

const ExecutionResult* VirtualMachine::exec(const QMap<QString, QVariant>& params)
{
    return NULL;
}

bool VirtualMachine::reset() 
{
    Q_ASSERT(m_pVm);
    int rc = unqlite_vm_reset(m_pVm);
    return rc == UNQLITE_OK;
}

}
