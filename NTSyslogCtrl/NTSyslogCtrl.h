// NTSyslogCtrl.h : main header file for the NTSYSLOGCTRL application
//

#if !defined(AFX_NTSYSLOGCTRL_H__9FB33EE5_E0E8_11D5_B306_0040055338AF__INCLUDED_)
#define AFX_NTSYSLOGCTRL_H__9FB33EE5_E0E8_11D5_B306_0040055338AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#define NTSYSLOG_SERVICE_NAME	_T("NTSyslog")
#define NTSYSLOG_DISPLAY_NAME	_T("Syslog NT Service")

#define NTSYSLOG_SOFTWARE_KEY	_T( "SOFTWARE\\SaberNet")
#define NTSYSLOG_SYSLOG_KEY	_T( "Syslog")
#define	NTSYSLOG_REGISTRY_KEY	_T( "SOFTWARE\\SaberNet\\Syslog" )
#define APPLICATION_SECTION	_T( "Application")
#define SECURITY_SECTION	_T( "Security")
#define SYSTEM_SECTION		_T( "System")
#define EVENTLOG_REG_PATH	_T( "System\\CurrentControlSet\\Services\\EventLog")

#define PRIMARY_SYSLOGD_ENTRY	_T( "Syslog")
#define BACKUP_SYSLOGD_ENTRY	_T( "Syslog1")

#define INFORMATION_ENTRY	_T( "Information")
#define INFORMATION_PRIORITY	_T( "Information Priority" )
#define WARNING_ENTRY		_T( "Warning")
#define WARNING_PRIORITY	_T( "Warning Priority" )
#define ERROR_ENTRY		_T( "Error")
#define ERROR_PRIORITY		_T( "Error Priority" )
#define AUDIT_SUCCESS_ENTRY	_T( "Audit Success")
#define AUDIT_SUCCESS_PRIORITY	_T( "Audit Success Priority" )
#define AUDIT_FAILURE_ENTRY	_T( "Audit Failure")
#define AUDIT_FAILURE_PRIORITY	_T( "Audit Failure Priority" )

#define CHECK_NOT_ENABLED	0x00
#define CHECK_INFORMATION	0x01
#define CHECK_WARNING		0x02
#define CHECK_ERROR		0x04
#define CHECK_AUDIT_SUCCESS	0x08
#define CHECK_AUDIT_FAILURE	0x10

#define DEFAULT_CHECKS (CHECK_WARNING + CHECK_ERROR + CHECK_AUDIT_FAILURE)
#define ALL_CHECKS (CHECK_INFORMATION + CHECK_WARNING + CHECK_ERROR \
				+ CHECK_AUDIT_SUCCESS + CHECK_AUDIT_FAILURE)

/////////////////////////////////////////////////////////////////////////////
// CNTSyslogCtrlApp:
// See NTSyslogCtrl.cpp for the implementation of this class
//

class CNTSyslogCtrlApp : public CWinApp
{
public:
	CNTSyslogCtrlApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNTSyslogCtrlApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CNTSyslogCtrlApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NTSYSLOGCTRL_H__9FB33EE5_E0E8_11D5_B306_0040055338AF__INCLUDED_)
