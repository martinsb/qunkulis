#include "executionresult.h"

namespace UnqliteQt
{

ErrorCode ExecutionResult::errorCode() const
{
    return Ok;
}

QVariant ExecutionResult::extractVariable(const QString& name)
{
    return QVariant();
}

}
