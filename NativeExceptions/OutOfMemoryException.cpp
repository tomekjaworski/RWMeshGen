#include "StdAfx.h"
#include "NativeExceptions.hpp"

using namespace NativeExceptions;

OutOfMemoryException::OutOfMemoryException(const TCHAR * message, size_t requested_memory)
	: Exception(message)
{
	this->SetExceptionClassName(_T("OutOfMemoryException"));
	this->RequestedMemory = requested_memory;

	this->Message = message;
	if (this->RequestedMemory != 0)
	{
		TCHAR err[1024];
		wsprintf(err, _T(" (Requested memory=%d bytes)"), this->RequestedMemory);
		this->Message += err;
	}

}

OutOfMemoryException::~OutOfMemoryException(void)
{
}

const TCHAR * OutOfMemoryException::ToString(void)
{
	Exception::ToString();
	return this->ToStringBuffer.c_str();
}