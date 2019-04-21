// MatrixInterface.h : main header file for the MatrixInterface DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMatrixInterfaceApp
// See MatrixInterface.cpp for the implementation of this class
//

class CMatrixInterfaceApp : public CWinApp
{
public:
	CMatrixInterfaceApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
