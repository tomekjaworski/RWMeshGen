// Exceptions.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "NativeExceptions.hpp"

using namespace NativeExceptions;


Exception::Exception()
{
	this->InitException();
}

Exception::Exception(const TCHAR * message)
{
	this->InitException();
	this->Message = message;
}

Exception::Exception(const Exception & inner_exception)
{
	this->InitException();
	this->SetInnerException(inner_exception);
}

Exception::Exception(const TCHAR * message, Exception & inner_exception)
{
	this->InitException();
	this->Message = message;
	this->SetInnerException(inner_exception);
}

Exception::~Exception()
{
	if (this->InnerException != NULL)
		delete this->InnerException;
}

void Exception::InitException(void)
{
	this->Message = _T("");
	this->InnerException = NULL;

	this->SetExceptionClassName(_T("Exception"));

}

void Exception::SetExceptionClassName(const TCHAR * class_name)
{
	//TODO: czy nie bedzie z tym problemu?
	this->exception_class_name = class_name;
}

const TCHAR * Exception::GetExceptionClassName(void)
{
	return this->exception_class_name.c_str();
}

void Exception::SetInnerException(const Exception & e)
{
	this->InnerException = new Exception();
	this->InnerException->Message = e.Message;

	if (e.InnerException != NULL)
		this->InnerException->SetInnerException(*e.InnerException);
}

const TCHAR * Exception::ToString(void)
{
	/*
	this->ToStringBuffer = this->exception_class_name;
	if (this->Message.length() > 0)
	{
		this->ToStringBuffer += ": ";
		this->ToStringBuffer += this->Message;
	}
	*/

	if (this->Message.length() > 0)
		this->ToStringBuffer = this->Message;
	else
		this->ToStringBuffer = this->exception_class_name;

	return this->ToStringBuffer.c_str();
}