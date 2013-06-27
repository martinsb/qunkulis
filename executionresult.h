#ifndef _EXECUTIONRESULT_H
#define _EXECUTIONRESULT_H

#include "errorcode.h"

#include <QObject>
#include <QString>
#include <QVariant>

namespace UnqliteQt
{

class ExecutionResult : public QObject
{
    Q_OBJECT

public:
        ErrorCode errorCode() const;
        QVariant extractVariable(const QString& name);
private:

};

}

#endif //_EXECUTIONRESULT_H
