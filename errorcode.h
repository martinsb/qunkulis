#ifndef ERRORCODE_H
#define ERRORCODE_H

extern "C"
{
#include "unqlite-db/unqlite.h"
}

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
};

#endif //ERRORCODE_H
