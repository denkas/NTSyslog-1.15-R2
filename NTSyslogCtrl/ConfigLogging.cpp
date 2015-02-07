// ConfigLogging.cpp : implementation file
//

#include "stdafx.h"
#include "NTSyslogCtrl.h"
#include "NTService.h"
#include "ConfigLogging.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigLogging dialog

// Constructor.  Set up sensible values.
CConfigLogging::CConfigLogging(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigLogging::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigLogging)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	//--OLD--m_idTitle = -1;
	//--OLD--m_idRegpath = -1;

	m_uCurrentState = CHECK_NOT_ENABLED;
	this->SetDefaultPriorities();
}

void CConfigLogging::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigLogging)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigLogging, CDialog)
	//{{AFX_MSG_MAP(CConfigLogging)
	ON_BN_CLICKED(IDC_DEFAULTS, OnDefaults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigLogging message handlers

BOOL CConfigLogging::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CWaitCursor	cWait;
	HKEY		hKeyRemote,
				hKey;
	DWORD		dwSize,
				dwType,
				dwValue;
	CString		csKeyName;

	// Check to see we were set up properly...
	//--OLD--if(m_idTitle == NULL || m_idRegpath == NULL)
	if(m_idTitle.IsEmpty() || m_idRegpath.IsEmpty() ) //NEW
	{
			AfxMessageBox("Program error: Dialog incorrectly set up.  This "
					"message should never appear.", MB_ICONSTOP);
			CDialog::OnCancel();
			return TRUE;
	}

	// Get the human readable title, and load it in to the right places in
	// the dialog...
	//////////////////////////////////////////////////////////////////////////////////////
	//Need to change this to accept CStrings instead of type INT to allow for dynamic
	//processing of custom EventLogs because type INT won't do it the way
	//I want it to
	//--OLD--CString csTitle((LPCTSTR)m_idTitle);
	CString csTitle = m_idTitle; //NEW
	CString csFmt;
	CString csUpdate;

	GetWindowText(csFmt);
	csUpdate.Format(csFmt, (LPCSTR)csTitle);
	SetWindowText(csUpdate);

	CWnd *pLabel = GetDlgItem(IDC_LABEL);
	pLabel->GetWindowText(csFmt);
	csUpdate.Format(csFmt, (LPCTSTR)csTitle);
	pLabel->SetWindowText(csUpdate);

	// Set everything up with empty, default values.
	m_uCurrentState = CHECK_NOT_ENABLED;
	this->SetDefaultPriorities();

	// Load the name of the key we're working from.
	//--OLD--CString csRegPath((LPCTSTR)m_idRegpath);
	CString csRegPath = m_idRegpath; //NEW
	
	// Open the registry on HKLM
	//Not sure if this is necessary anymore because registry connection
	//is verified upon selecting the server in the select server dialog
	if (RegConnectRegistry( (char*)((LPCTSTR)m_csComputer), HKEY_LOCAL_MACHINE, &hKeyRemote) != ERROR_SUCCESS)
	{
		csKeyName.Format( _T( "Error while connecting to the registry !\n\nEnsure that\n\n%s\n%s"),
						  _T( "1) Network Registry access is enabled if this is a remote computer."),
						  _T( "2) You have Administrator privilieges on the computer."));
		AfxMessageBox( csKeyName, MB_ICONSTOP);
		CDialog::OnCancel();
		return TRUE;
	}
	// Open the appropriate SOFTWARE\SaberNet\Syslog key
	csKeyName.Format(_T( "%s\\%s" ), NTSYSLOG_REGISTRY_KEY, csRegPath);
	if (RegOpenKeyEx( hKeyRemote, csKeyName, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		// Forward Information events
		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, INFORMATION_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS) &&
			(dwValue == 1))
		{
			m_uCurrentState += CHECK_INFORMATION;
		}

		// Forward Information priority
		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, INFORMATION_PRIORITY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS))
		{
			m_uInfoPriority = dwValue;
		}

		// Forward Warning events
		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, WARNING_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS) &&
			(dwValue == 1))
		{
			m_uCurrentState += CHECK_WARNING;
		}

		// Forward Warning priority
		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, WARNING_PRIORITY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS))
		{
			m_uWarningPriority = dwValue;
		}

		// Forward Error events
		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, ERROR_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS) &&
			(dwValue == 1))
		{
			m_uCurrentState += CHECK_ERROR;
		}

		// Forward Error priority
		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, ERROR_PRIORITY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS))
		{
			m_uErrorPriority = dwValue;
		}

		// Forward Audit Success events
		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, AUDIT_SUCCESS_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS) &&
			(dwValue == 1))
		{
			m_uCurrentState += CHECK_AUDIT_SUCCESS;
		}

		// Forward Audit Success priority
		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, AUDIT_SUCCESS_PRIORITY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS))
		{
			m_uAuditSuccessPriority = dwValue;
		}

		// Forward Audit Failure events
		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, AUDIT_FAILURE_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS) &&
			(dwValue == 1))
		{
			m_uCurrentState += CHECK_AUDIT_FAILURE;
		}

		// Forward Audit Failure priority
		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, AUDIT_FAILURE_PRIORITY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS))
		{
			m_uAuditFailurePriority = dwValue;
		}

		RegCloseKey(hKey);
	}
	else	// If the key does not exist, then use the default values.
	{
		m_uCurrentState = m_uDefaultChecks;
	}
	RegCloseKey(hKeyRemote);

	// Update the dialog box to match the settings loaded from the registry.
	CheckDlgButton( IDC_INFORMATION_CHECK, (m_uCurrentState & CHECK_INFORMATION));
	CheckDlgButton( IDC_WARNING_CHECK, (m_uCurrentState & CHECK_WARNING));
	CheckDlgButton( IDC_ERROR_CHECK, (m_uCurrentState & CHECK_ERROR));
	CheckDlgButton( IDC_AUDIT_SUCCESS_CHECK, (m_uCurrentState & CHECK_AUDIT_SUCCESS));
	CheckDlgButton( IDC_AUDIT_FAILURE_CHECK, (m_uCurrentState & CHECK_AUDIT_FAILURE));

	SetDialogFromPriority(IDC_INFO_FACILITY, IDC_INFO_SEVERITY, m_uInfoPriority);
	SetDialogFromPriority(IDC_WARNING_FACILITY, IDC_WARNING_SEVERITY, m_uWarningPriority);
	SetDialogFromPriority(IDC_ERROR_FACILITY, IDC_ERROR_SEVERITY, m_uErrorPriority);
	SetDialogFromPriority(IDC_AUDIT_SUCCESS_FACILITY, IDC_AUDIT_SUCCESS_SEVERITY, m_uAuditSuccessPriority);
	SetDialogFromPriority(IDC_AUDIT_FAILURE_FACILITY, IDC_AUDIT_FAILURE_SEVERITY, m_uAuditFailurePriority);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	
}

void CConfigLogging::OnOK() 
{
	// Get and format the confirmation dialog message.
	//--OLD--CString csTitle((LPCTSTR)m_idTitle);

	CString csTitle = m_idTitle; //NEW
	CString csFmt((LPCTSTR)IDS_CONFIRM_SAVE);
	CString csUpdate;
	csUpdate.Format(csFmt, (LPCTSTR)csTitle);
	
	// Check for changes, and confirm them.
	if (IsModified() && (AfxMessageBox( csUpdate, MB_YESNO|MB_ICONQUESTION) == IDNO))
	{
		// Discard changes
		CDialog::OnCancel();
		return;
	}

	CWaitCursor	cWait;	// Hourglass.

	// Save the changes to the appropriate registry.
	HKEY		hKeyRemote,
				hKeySoftware,
				hKeySyslog,
				hKeySection;
	DWORD		dwSize,
				dwValue;

	// Get the path from the string table.
	//--OLD--CString csRegPath((LPCTSTR)m_idRegpath);
	CString csRegPath = m_idRegpath;

	// Connect to the registry on HKLM
	if (RegConnectRegistry( (char*)((LPCTSTR)m_csComputer), HKEY_LOCAL_MACHINE, &hKeyRemote) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error while connecting to the registry !\n\nPlease retry."), MB_ICONSTOP);
		return;
	}
	// Create the SOFTWARE\SaberNet key or open it if it exists
	if (RegCreateKeyEx( hKeyRemote, NTSYSLOG_SOFTWARE_KEY, 0, REG_NONE, REG_OPTION_NON_VOLATILE,
						KEY_WRITE|KEY_READ, NULL, &hKeySoftware, &dwValue) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeyRemote);
		return;
	}
	// Create the Syslog subkey or open it if it exists
	if (RegCreateKeyEx( hKeySoftware, NTSYSLOG_SYSLOG_KEY, 0, REG_NONE, REG_OPTION_NON_VOLATILE,
						KEY_WRITE|KEY_READ, NULL, &hKeySyslog, &dwValue) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}
	// Create the appropriate subkey or open it if it exists
	if (RegCreateKeyEx( hKeySyslog, csRegPath, 0, REG_NONE, REG_OPTION_NON_VOLATILE,
						KEY_WRITE|KEY_READ, NULL, &hKeySection, &dwValue) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}

	// Write if we have have to forward Information events
	dwValue = (IsDlgButtonChecked( IDC_INFORMATION_CHECK) ? 1 : 0);
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySection, INFORMATION_ENTRY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}
	// Write Information event priority
	dwValue = PriorityFromDialog(IDC_INFO_FACILITY, IDC_INFO_SEVERITY);
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySection, INFORMATION_PRIORITY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}

	// Write if we have have to forward Warning events
	dwValue = (IsDlgButtonChecked( IDC_WARNING_CHECK) ? 1 : 0);
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySection, WARNING_ENTRY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}
	// Write Warning event priority
	dwValue = PriorityFromDialog(IDC_WARNING_FACILITY, IDC_WARNING_SEVERITY);
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySection, WARNING_PRIORITY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}
	
	// Write if we have have to forward Error events
	dwValue = (IsDlgButtonChecked( IDC_ERROR_CHECK) ? 1 : 0);
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySection, ERROR_ENTRY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}
	// Write Error event priority
	dwValue = PriorityFromDialog(IDC_ERROR_FACILITY, IDC_ERROR_SEVERITY);
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySection, ERROR_PRIORITY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}

	// Write if we have have to forward Audit Success events
	dwValue = (IsDlgButtonChecked( IDC_AUDIT_SUCCESS_CHECK) ? 1 : 0);
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySection, AUDIT_SUCCESS_ENTRY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}
	// Write Audit Success event priority
	dwValue = PriorityFromDialog(IDC_AUDIT_SUCCESS_FACILITY, IDC_AUDIT_SUCCESS_SEVERITY);
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySection, AUDIT_SUCCESS_PRIORITY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}

	// Write if we have have to forward Audit Failure events
	dwValue = (IsDlgButtonChecked( IDC_AUDIT_FAILURE_CHECK) ? 1 : 0);
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySection, AUDIT_FAILURE_ENTRY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}
	// Write Audit Failure event priority
	dwValue = PriorityFromDialog(IDC_AUDIT_FAILURE_FACILITY, IDC_AUDIT_FAILURE_SEVERITY);
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySection, AUDIT_FAILURE_PRIORITY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}
	
	RegCloseKey( hKeySection);
	RegCloseKey( hKeySyslog);
	RegCloseKey( hKeySoftware);
	RegCloseKey( hKeyRemote);

	CDialog::OnOK();
}

void CConfigLogging::OnCancel() 
{
	// Load the text from the string table, and format it.
	//--OLD--CString csTitle((LPCTSTR)m_idTitle);

	CString csTitle = m_idTitle; //NEW
	CString csFmt((LPCTSTR)IDS_CONFIRM_DISCARD);
	CString csUpdate;
	csUpdate.Format(csFmt, (LPCTSTR)csTitle);

	// If the data changed, confirm the cancel.
	if (IsModified() && (AfxMessageBox( csUpdate, MB_YESNO|MB_ICONQUESTION) == IDNO))
	{
		return;
	}
	
	CDialog::OnCancel();
}


BOOL CConfigLogging::IsModified()
{
	// Start with sensible values.
	UINT uiFacility = this->GetDefaultFacility();
	UINT uNewState = CHECK_NOT_ENABLED;
	UINT infoPriority = 8*uiFacility + 5;
	UINT warningPriority = 8*uiFacility + 4;
	UINT errorPriority = 8*uiFacility + 3;
	UINT auditSuccessPriority = 8*uiFacility + 7;
	UINT auditFailurePriority = 8*uiFacility + 7;

	// Load values for the checkboxes from the dialog.
	if (IsDlgButtonChecked( IDC_INFORMATION_CHECK))
		uNewState += CHECK_INFORMATION;
	if (IsDlgButtonChecked( IDC_WARNING_CHECK))
		uNewState += CHECK_WARNING;
	if (IsDlgButtonChecked( IDC_ERROR_CHECK))
		uNewState += CHECK_ERROR;
	if (IsDlgButtonChecked( IDC_AUDIT_SUCCESS_CHECK))
		uNewState += CHECK_AUDIT_SUCCESS;
	if (IsDlgButtonChecked( IDC_AUDIT_FAILURE_CHECK))
		uNewState += CHECK_AUDIT_FAILURE;

	// Load values for the priorities from the dialog.
	infoPriority = PriorityFromDialog(IDC_INFO_FACILITY, IDC_INFO_SEVERITY);
	warningPriority = PriorityFromDialog(IDC_WARNING_FACILITY, IDC_WARNING_SEVERITY);
	errorPriority = PriorityFromDialog(IDC_ERROR_FACILITY, IDC_ERROR_SEVERITY);
	auditSuccessPriority = PriorityFromDialog(IDC_AUDIT_SUCCESS_FACILITY, IDC_AUDIT_SUCCESS_SEVERITY);
	auditFailurePriority = PriorityFromDialog(IDC_AUDIT_FAILURE_FACILITY, IDC_AUDIT_FAILURE_SEVERITY);

	// Return true or false.
	return !(infoPriority == m_uInfoPriority &&
				warningPriority == m_uWarningPriority &&
				errorPriority == m_uErrorPriority &&
				auditSuccessPriority == m_uAuditSuccessPriority &&
				auditFailurePriority == m_uAuditFailurePriority &&
				uNewState == m_uCurrentState);
}


UINT CConfigLogging::PriorityFromDialog(int facilityId, int severityId)
{
	// Get pointers to the combo boxes.
	CComboBox *pFacilityWnd = (CComboBox *)GetDlgItem(facilityId);
	CComboBox *pSeverityWnd = (CComboBox *)GetDlgItem(severityId);

	// If one didn't exist, stop rather than crashing.  Should never happen.
	if(pFacilityWnd == NULL || pSeverityWnd == NULL)
	{
		AfxMessageBox("Program error: Dialog now missing controls.  This "
				"message should never appear.", MB_ICONSTOP);
		CDialog::OnCancel();
	}

	// Get the selection.  Depends on the dialog having the right items in it.
	int facility = pFacilityWnd->GetCurSel();
	int severity = pSeverityWnd->GetCurSel();

	// Again, make sure we got values back.
	if(facility == CB_ERR || severity == CB_ERR)
	{
		AfxMessageBox("Program error: Dialog now returns errors.  This "
				"message should never appear.", MB_ICONSTOP);
		CDialog::OnCancel();
	}

	// Glue the values together in to a priority.
	return ((facility * 8) + severity);
}

void CConfigLogging::SetDialogFromPriority(int facilityId, int severityId, UINT priority)
{
	// Get pointers to the combo boxes.
	CComboBox *pFacilityWnd = (CComboBox *)GetDlgItem(facilityId);
	CComboBox *pSeverityWnd = (CComboBox *)GetDlgItem(severityId);

	// If one didn't exist, stop rather than crashing.  Should never happen.
	if(pFacilityWnd == NULL || pSeverityWnd == NULL)
	{
		AfxMessageBox("Program error: Dialog missing controls.  This "
				"message should never appear.", MB_ICONSTOP);
		CDialog::OnCancel();
	}

	// Calculate facility and priority.
	int facility = priority / 8;
	int severity = priority % 8;

	// Load values in to the combos.  If one can't, then stop.  This might actually
	// happen if someone has got values too large in the registry.
	if(pFacilityWnd->SetCurSel(facility) == CB_ERR
			|| pSeverityWnd->SetCurSel(severity) == CB_ERR)
	{
		AfxMessageBox("Program error: Cannot set severity or priority.\n\nCheck "
				"registry to make sure it contains valid values..", MB_ICONSTOP);
		CDialog::OnCancel();
	}
}

//void CConfigLogging::SetupDialog(CString title, CString regpath, LPCTSTR lpstrComputer, UINT defaultChecks)
void CConfigLogging::SetupDialog(CString selection, LPCTSTR lpstrComputer, UINT defaultChecks)
{
	// Store computer name, or set empty.
	if ((lpstrComputer == NULL) || (_tcsclen( lpstrComputer) == 0))
	{
		m_csComputer.Empty();
	}
	else
	{
		// Set computer in \\computer_name format
		m_csComputer.Format( _T( "\\\\%s"), lpstrComputer);
	}
	m_uDefaultChecks = defaultChecks;

	// Store title and registry path IDs for later.
	m_idTitle = selection;
	m_idRegpath = selection;
}

UINT CConfigLogging::GetDefaultFacility() const
{
    const char *pszVal = this->m_idRegpath;

    if( (pszVal != NULL) && (*pszVal != '\0') )
    {
	if( !_stricmp(pszVal, "Application") )
		return (1);
	else if( !_stricmp(pszVal, "Security") )
		return (4);
	else if( !_stricmp(pszVal, "System") )
		return (0);

	for( int temp=0 ; pszVal[temp]!='\0' ; temp++ )
	{
	    if( !_strnicmp(&pszVal[temp], "Service", 7) )
//		return (16);
		return (1);
	}
    }
//    return (17);				// Default Facility
    return (1);				// Default Facility‚ÍApplication‚Æ“¯—ñ‚Æ‚·‚é
}

void CConfigLogging::SetDefaultPriorities()
{
    UINT uiFacility = this->GetDefaultFacility();
/*
	m_uInfoPriority = 8*uiFacility + 5;
    m_uWarningPriority = 8*uiFacility + 4;
    m_uErrorPriority = 8*uiFacility + 3;
    m_uAuditSuccessPriority =
	m_uAuditFailurePriority = 8*uiFacility + 7;
*/
	m_uInfoPriority = 9;
    m_uWarningPriority = 9;
    m_uErrorPriority = 9;
    m_uAuditSuccessPriority = 9;
	m_uAuditFailurePriority = 9;
}

void CConfigLogging::OnDefaults() 
{
	// Load the checkboxes from the defaults the caller set up.
	CheckDlgButton( IDC_INFORMATION_CHECK, (m_uDefaultChecks & CHECK_INFORMATION) ? TRUE : FALSE);
	CheckDlgButton( IDC_WARNING_CHECK, (m_uDefaultChecks & CHECK_WARNING) ? TRUE : FALSE);
	CheckDlgButton( IDC_ERROR_CHECK, (m_uDefaultChecks & CHECK_ERROR) ? TRUE : FALSE);
	CheckDlgButton( IDC_AUDIT_SUCCESS_CHECK, (m_uDefaultChecks & CHECK_AUDIT_SUCCESS) ? TRUE : FALSE);
	CheckDlgButton( IDC_AUDIT_FAILURE_CHECK, (m_uDefaultChecks & CHECK_AUDIT_FAILURE) ? TRUE : FALSE);

	// Set all the priorities to the old values used by the first versions.
	// Consider having different values for the different screens, and allowing the
	// caller to configure them.
	UINT uiFacility = this->GetDefaultFacility();
/*
	SetDialogFromPriority(IDC_INFO_FACILITY, IDC_INFO_SEVERITY, 8*uiFacility + 5);
	SetDialogFromPriority(IDC_WARNING_FACILITY, IDC_WARNING_SEVERITY, 8*uiFacility + 4);
	SetDialogFromPriority(IDC_ERROR_FACILITY, IDC_ERROR_SEVERITY, 8*uiFacility + 3);
	SetDialogFromPriority(IDC_AUDIT_SUCCESS_FACILITY, IDC_AUDIT_SUCCESS_SEVERITY, 8*uiFacility + 7);
	SetDialogFromPriority(IDC_AUDIT_FAILURE_FACILITY, IDC_AUDIT_FAILURE_SEVERITY, 8*uiFacility + 7);
*/
	SetDialogFromPriority(IDC_INFO_FACILITY, IDC_INFO_SEVERITY, 9);
	SetDialogFromPriority(IDC_WARNING_FACILITY, IDC_WARNING_SEVERITY, 9);
	SetDialogFromPriority(IDC_ERROR_FACILITY, IDC_ERROR_SEVERITY, 9);
	SetDialogFromPriority(IDC_AUDIT_SUCCESS_FACILITY, IDC_AUDIT_SUCCESS_SEVERITY, 9);
	SetDialogFromPriority(IDC_AUDIT_FAILURE_FACILITY, IDC_AUDIT_FAILURE_SEVERITY, 9);
}



