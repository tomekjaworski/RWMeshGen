#include "StdAfx.h"
#include "NativeExceptions.hpp"

using namespace NativeExceptions;

ModelException::ModelException(const TCHAR * message)
	: Exception(message)
{
	this->SetExceptionClassName(_T("ModelException"));
}

ModelException::~ModelException(void)
{
}
