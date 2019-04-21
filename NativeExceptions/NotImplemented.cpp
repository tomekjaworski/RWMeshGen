#include "StdAfx.h"
#include "NativeExceptions.hpp"

using namespace NativeExceptions;

NotImplementedException::NotImplementedException(const TCHAR * message)
	: Exception(message)
{
	this->SetExceptionClassName(_T("NotImplementedException"));
}

NotImplementedException::~NotImplementedException(void)
{
}
