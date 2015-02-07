// ApplicationEventLogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NTSyslogCtrl.h"
#include "NTService.h"
#include "ApplicationEventLogDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CApplicationEventLogDlg dialog


CApplicationEventLogDlg::CApplicationEventLogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CApplicationEventLogDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CApplicationEventLogDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CApplicationEventLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CApplicationEventLogDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CApplicationEventLogDlg, CDialog)
	//{{AFX_MSG_MAP(CApplicationEventLogDlg)
	ON_BN_CLICKED(IDC_DEFAULTS, OnDefaults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CApplicationEventLogDlg message handlers

BOOL CApplicationEventLogDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CWaitCursor	cWait;
	HKEY		hKeyRemote,
				hKey;
	DWORD		dwSize,
				dwType,
				dwValue;
	CString		csKeyName;

	m_uCurrentState = CHECK_NOT_ENABLED;

	// Open the registry on HKLM
	if (RegConnectRegistry( m_csComputer, HKEY_LOCAL_MACHINE, &hKeyRemote) != ERROR_SUCCESS)
	{
		csKeyName.Format( _T( "Error while connecting to the registry !\n\nEnsure that\n\n%s\n%s"),
						  _T( "1) Network Registry access is enabled if this is a remote computer."),
						  _T( "2) You have Administrator privilieges on the computer."));
		AfxMessageBox( csKeyName, MB_ICONSTOP);
		CDialog::OnCancel();
		return TRUE;
	}
	// Open the SOFTWARE\SaberNet\Syslog\Application key
	csKeyName.Format( _T( "%s\\%s"), NTSYSLOG_REGISTRY_KEY, APPLICATION_SECTION);
	if (RegOpenKeyEx( hKeyRemote, csKeyName, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		// Key exist => read values
		// Forward Information events
		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, INFORMATION_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS) &&
			(dwValue == 1))
		{
			// Information events enabled
			CheckDlgButton( IDC_INFORMATION_CHECK, TRUE);
			m_uCurrentState += CHECK_INFORMATION;
		}
		else
			// Information events not enabled
			CheckDlgButton( IDC_INFORMATION_CHECK, FALSE);
		// Forward Warning events
		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, WARNING_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS) &&
			(dwValue == 1))
		{
			// Warning events enabled
			CheckDlgButton( IDC_WARNING_CHECK, TRUE);
			m_uCurrentState += CHECK_WARNING;
		}
		else
			// Warning events not enabled
			CheckDlgButton( IDC_WARNING_CHECK, FALSE);
		// Forward Error events
		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, ERROR_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS) &&
			(dwValue == 1))
		{
			// Error events enabled
			CheckDlgButton( IDC_ERROR_CHECK, TRUE);
			m_uCurrentState += CHECK_ERROR;
		}
		else
			// Error events not enabled
			CheckDlgButton( IDC_ERROR_CHECK, FALSE);
		// Forward Audit Success events
		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, AUDIT_SUCCESS_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS) &&
			(dwValue == 1))
		{
			// Audit Success events enabled
			CheckDlgButton( IDC_AUDIT_SUCCESS_CHECK, TRUE);
			m_uCurrentState += CHECK_AUDIT_SUCCESS;
		}
		else
			// Audit Success events not enabled
			CheckDlgButton( IDC_AUDIT_SUCCESS_CHECK, FALSE);
		// Forward Audit Failure events
		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, AUDIT_FAILURE_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS) &&
			(dwValue == 1))
		{
			// Audit Failure events enabled
			CheckDlgButton( IDC_AUDIT_FAILURE_CHECK, TRUE);
			m_uCurrentState += CHECK_AUDIT_FAILURE;
		}
		else
			// Audit Failure events not enabled
			CheckDlgButton( IDC_AUDIT_FAILURE_CHECK, FALSE);
		RegCloseKey( hKey);
	}
	else
	{
		// Key does not exist => no events forwarded
		CheckDlgButton( IDC_INFORMATION_CHECK, FALSE);
		CheckDlgButton( IDC_WARNING_CHECK, FALSE);
		CheckDlgButton( IDC_ERROR_CHECK, FALSE);
		CheckDlgButton( IDC_AUDIT_SUCCESS_CHECK, FALSE);
		CheckDlgButton( IDC_AUDIT_FAILURE_CHECK, FALSE);
	}
	RegCloseKey( hKeyRemote);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CApplicationEventLogDlg::OnOK() 
{
	// TODO: Add extra validation here
	if (IsModified() && (AfxMessageBox( _T( "Do you really want to save Application EventLog parameters ?\n\nNB: if service is started, you must restart it for change to take effect."),
				MB_YESNO|MB_ICONQUESTION) == IDNO))
	{
		// Discard changes
		CDialog::OnCancel();
		return;
	}

	CWaitCursor	cWait;
	HKEY		hKeyRemote,
				hKeySoftware,
				hKeySyslog,
				hKeySection;
	DWORD		dwSize,
				dwValue;

	// Connect to the registry on HKLM
	if (RegConnectRegistry( m_csComputer, HKEY_LOCAL_MACHINE, &hKeyRemote) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error while connecting to the registry !\n\nPlease retry."), MB_ICONSTOP);
		return;
	}
	// Create the SOFTWARE\SaberNet key or open it if it exists
	if (RegCreateKeyEx( hKeyRemote, NTSYSLOG_SOFTWARE_KEY, 0, REG_NONE, REG_OPTION_NON_VOLATILE,
						KEY_WRITE|KEY_READ, NULL, &hKeySoftware, &dwValue) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey (hKeyRemote);
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
	// Create the Application subkey or open it if it exists
	if (RegCreateKeyEx( hKeySyslog, APPLICATION_SECTION, 0, REG_NONE, REG_OPTION_NON_VOLATILE,
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
	// Write if we have have to forward Information events
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
	RegCloseKey( hKeySection);
	RegCloseKey( hKeySyslog);
	RegCloseKey( hKeySoftware);
	RegCloseKey( hKeyRemote);
	CDialog::OnOK();
}

void CApplicationEventLogDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if (IsModified() && (AfxMessageBox( _T( "Do you really want to discard Application EventLog parameters changes ?"),
			MB_YESNO|MB_ICONQUESTION) == IDNO))
			return;
	CDialog::OnCancel();
}

BOOL CApplicationEventLogDlg::IsModified()
{
	UINT uNewState = CHECK_NOT_ENABLED;

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
	return (uNewState != m_uCurrentState);
}

void CApplicationEventLogDlg::SetComputer(LPCTSTR lpstrComputer)
{
	if ((lpstrComputer == NULL) || (_tcsclen( lpstrComputer) == 0))
		m_csComputer.Empty();
	else
		// Set computer in \\computer_name format
		m_csComputer.Format( _T( "\\\\%s"), lpstrComputer);
}

void CApplicationEventLogDlg::OnDefaults() 
{
	// TODO: Add your control notification handler code here
	CheckDlgButton( IDC_INFORMATION_CHECK, FALSE);
	CheckDlgButton( IDC_WARNING_CHECK, TRUE);
	CheckDlgButton( IDC_ERROR_CHECK, TRUE);
	CheckDlgButton( IDC_AUDIT_SUCCESS_CHECK, FALSE);
	CheckDlgButton( IDC_AUDIT_FAILURE_CHECK, TRUE);
}
