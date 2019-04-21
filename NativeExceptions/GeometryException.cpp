#include "StdAfx.h"
#include "NativeExceptions.hpp"

using namespace NativeExceptions;

GeometryException::GeometryException(const TCHAR * message)
	: Exception(message)
{
	this->SetExceptionClassName(_T("GeometryException"));
}

GeometryException::~GeometryException(void)
{
}
