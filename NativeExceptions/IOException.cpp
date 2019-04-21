#include "StdAfx.h"
#include "NativeExceptions.hpp"

using namespace NativeExceptions;

struct ERROR_ENTRY
{
	int code;
	const TCHAR * const_name;
	const TCHAR * desc;
};

// constant names and error descriptions taken from http://msdn.microsoft.com/en-us/library/t3ayayh1.aspx
struct ERROR_ENTRY error_description_english[] = 
{
	{EPERM,			_T("EPERM"),		_T("Operation not permitted")},
	{ENOENT,		_T("ENOENT"),		_T("No such file or directory")},
	{ESRCH,			_T("ESRCH"),		_T("No such process")},
	{EINTR,			_T("EINTR"),		_T("Interrupted function")},
	{EIO,			_T("EIO"),			_T("I/O error")},
	{ENXIO,			_T("ENXIO"),		_T("No such device or address")},
	{E2BIG,			_T("E2BIG"),		_T("Argument list too long")},
	{ENOEXEC,		_T("ENOEXEC"),		_T("Exec format error")},
	{EBADF,			_T("EBADF"),		_T("Bad file number")},
	{ECHILD,		_T("ECHILD"),		_T("No spawned processes")},
	{EAGAIN,		_T("EAGAIN"),		_T("No more processes or not enough memory or maximum nesting level reached")},
	{ENOMEM,		_T("ENOMEM"),		_T("Not enough memory")},
	{EACCES,		_T("EACCES"),		_T("Permission denied")},
	{EFAULT,		_T("EFAULT"),		_T("Bad address")},
	{EBUSY,			_T("EBUSY"),		_T("Device or resource busy")},
	{EEXIST,		_T("EEXIST"),		_T("File exists")},
	{EXDEV,			_T("EXDEV"),		_T("Cross-device link")},
	{ENODEV,		_T("ENODEV"),		_T("No such device")},
	{ENOTDIR,		_T("ENOTDIR"),		_T("Not a directory")},
	{EISDIR,		_T("EISDIR"),		_T("Is a directory")},
	{EINVAL,		_T("EINVAL"),		_T("Invalid argument")},
	{ENFILE,		_T("ENFILE"),		_T("Too many files open in system")},
	{EMFILE,		_T("EMFILE"),		_T("Too many open files")},
	{ENOTTY,		_T("ENOTTY"),		_T("Inappropriate I/O control operation")},
	{EFBIG,			_T("EFBIG"),		_T("File too large")},
	{ENOSPC,		_T("ENOSPC"),		_T("No space left on device")},
	{ESPIPE,		_T("ESPIPE"),		_T("Invalid seek")},
	{EROFS,			_T("EROFS"),		_T("Read-only file system")},
	{EMLINK,		_T("EMLINK"),		_T("Too many links")},
	{EPIPE,			_T("EPIPE"),		_T("Broken pipe")},
	{EDOM,			_T("EDOM"),			_T("Math argument")},
	{ERANGE,		_T("ERANGE"),		_T("Result too large")},
	{EDEADLK,		_T("EDEADLK"),		_T("Resource deadlock would occur")},
	{EDEADLOCK,		_T("EDEADLOCK"),	_T("Same as EDEADLK for compatibility with older Microsoft C versions")},
	{ENAMETOOLONG,	_T("ENAMETOOLONG"),	_T("Filename too long")},
	{ENOLCK,		_T("ENOLCK"),		_T("No locks available")},
	{ENOSYS,		_T("ENOSYS"),		_T("Function not supported")},
	{ENOTEMPTY,		_T("ENOTEMPTY"),	_T("Directory not empty")},
	{EILSEQ,		_T("EILSEQ"),		_T("Illegal byte sequence")},
	{STRUNCATE,		_T("STRUNCATE"),	_T("String was truncated_T(")},
	{0,				NULL,			NULL}
};

struct ERROR_ENTRY error_description_polish[] = 
{
	{EPERM,			_T("EPERM"),		_T("Operacja jest niedozwolona")},
	{ENOENT,		_T("ENOENT"),		_T("Brak podanego pliku lub katalogu")},
	{ESRCH,			_T("ESRCH"),		_T("Brak podanego procesu")},
	{EINTR,			_T("EINTR"),		_T("Przerwanie")},
	{EIO,			_T("EIO"),			_T("B³¹d WE/WY")},
	{ENXIO,			_T("ENXIO"),		_T("Brak takiego urz¹dzenia lub adresu")},
	{E2BIG,			_T("E2BIG"),		_T("Lista parametrów jest zbyt d³uga")},
	{ENOEXEC,		_T("ENOEXEC"),		_T("B³¹d formatu wykonywalnego")},
	{EBADF,			_T("EBADF"),		_T("Niew³aœciwy uchwyt pliku")},
	{ECHILD,		_T("ECHILD"),		_T("No spawned processes")},
	{EAGAIN,		_T("EAGAIN"),		_T("No more processes or not enough memory or maximum nesting level reached")},
	{ENOMEM,		_T("ENOMEM"),		_T("Brak pamiêci")},
	{EACCES,		_T("EACCES"),		_T("Brak praw dostêpu")},
	{EFAULT,		_T("EFAULT"),		_T("Bad address")},
	{EBUSY,			_T("EBUSY"),		_T("Urz¹dzenie lub zasób systemowy jest aktualnie w u¿yciu")},
	{EEXIST,		_T("EEXIST"),		_T("Plik o podanej nazwie istnieje")},
	{EXDEV,			_T("EXDEV"),		_T("Cross-device link")},
	{ENODEV,		_T("ENODEV"),		_T("Brak podanego urz¹dzenia")},
	{ENOTDIR,		_T("ENOTDIR"),		_T("Not a directory")},
	{EISDIR,		_T("EISDIR"),		_T("Is a directory")},
	{EINVAL,		_T("EINVAL"),		_T("Invalid argument")},
	{ENFILE,		_T("ENFILE"),		_T("Too many files open in system")},
	{EMFILE,		_T("EMFILE"),		_T("Zbyt du¿o otwartych plików")},
	{ENOTTY,		_T("ENOTTY"),		_T("Inappropriate I/O control operation")},
	{EFBIG,			_T("EFBIG"),		_T("File too large")},
	{ENOSPC,		_T("ENOSPC"),		_T("Brak miejsca na nosniku/urz¹dzeniu docelowym")},
	{ESPIPE,		_T("ESPIPE"),		_T("Invalid seek")},
	{EROFS,			_T("EROFS"),		_T("System plików jest w trybie tylko-do-odczytu")},
	{EMLINK,		_T("EMLINK"),		_T("Too many links")},
	{EPIPE,			_T("EPIPE"),		_T("Broken pipe")},
	{EDOM,			_T("EDOM"),			_T("Math argument")},
	{ERANGE,		_T("ERANGE"),		_T("Result too large")},
	{EDEADLK,		_T("EDEADLK"),		_T("Resource deadlock would occur")},
	{EDEADLOCK,		_T("EDEADLOCK"),	_T("Same as EDEADLK for compatibility with older Microsoft C versions")},
	{ENAMETOOLONG,	_T("ENAMETOOLONG"),	_T("Nazwa pliku jest zbyt d³uga")},
	{ENOLCK,		_T("ENOLCK"),		_T("No locks available")},
	{ENOSYS,		_T("ENOSYS"),		_T("Funkcja nie jest obs³ugiwana")},
	{ENOTEMPTY,		_T("ENOTEMPTY"),	_T("Katalog nie jest pusty")},
	{EILSEQ,		_T("EILSEQ"),		_T("Illegal byte sequence")},
	{STRUNCATE,		_T("STRUNCATE"),	_T("String was truncated")},
	{0,				NULL,			NULL}
};

const TCHAR * GetErrorConstant(const int error_code)
{
	ERROR_ENTRY * entry = error_description_english;
	while(true)
	{
		// end of array?
		if (entry->const_name == NULL)
			return NULL;

		if (entry->code == error_code)
			return entry->const_name;

		entry++;
	}
}

const TCHAR * GetErrorDescription(const int error_code)
{
	ERROR_ENTRY * entry = error_description_english;
	while(true)
	{
		// end of array?
		if (entry->const_name == NULL)
			return NULL;

		if (entry->code == error_code)
			return entry->desc;

		entry++;
	}
}

IOException::IOException(const TCHAR * file_name, int error_code, const TCHAR * message)
	: Exception(message)
{
	Exception::SetExceptionClassName(_T("CFileIOException"));

	this->ErrorCode = error_code;
	this->FileName = file_name;

	TCHAR err[200];
	wsprintf(err, _T("[0x%08X, %s]: %s)"), this->ErrorCode, GetErrorConstant(this->ErrorCode), GetErrorDescription(this->ErrorCode));

	this->Message = message;
	this->Message += _T(" (FileName=\"") + this->FileName + _T("\", ErrorCode=") + err;
	

}

IOException::~IOException(void)
{
}

const TCHAR * IOException::ToString(void)
{
	Exception::ToString();
	return this->ToStringBuffer.c_str();
}