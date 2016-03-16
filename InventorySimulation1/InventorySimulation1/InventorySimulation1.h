
// InventorySimulation1.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CInventorySimulation1App:
// See InventorySimulation1.cpp for the implementation of this class
//

class CInventorySimulation1App : public CWinApp
{
public:
	CInventorySimulation1App();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CInventorySimulation1App theApp;