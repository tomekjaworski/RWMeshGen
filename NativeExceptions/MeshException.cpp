#include "StdAfx.h"
#include "NativeExceptions.hpp"

using namespace NativeExceptions;

MeshException::MeshException(const TCHAR * message)
	: Exception(message)
{
	this->SetExceptionClassName(_T("MeshException"));
}

MeshException::~MeshException(void)
{
}
