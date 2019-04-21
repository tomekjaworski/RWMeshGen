#include "StdAfx.h"
#include "NativeExceptions.hpp"

using namespace NativeExceptions;

RangeException::RangeException(const TCHAR * message, double value)
	: Exception(message)
{
	this->SetExceptionClassName(_T("CRangeException"));
	this->RangeValue = value;

	this->Message = message;
	if (this->RangeValue != 0.0)
	{
		TCHAR err[1024];
		wsprintf(err, _T(" (Value out-of-range = %.6f)"), this->RangeValue);
		this->Message += err;
	}

}

RangeException::~RangeException(void)
{
}

const TCHAR * RangeException::ToString(void)
{
	Exception::ToString();
	return this->ToStringBuffer.c_str();
}