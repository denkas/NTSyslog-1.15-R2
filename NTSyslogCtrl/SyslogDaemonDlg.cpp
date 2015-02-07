// SyslogDaemonDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NTSyslogCtrl.h"
#include "NTService.h"
#include "SyslogDaemonDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSyslogDaemonDlg dialog


CSyslogDaemonDlg::CSyslogDaemonDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSyslogDaemonDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSyslogDaemonDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSyslogDaemonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSyslogDaemonDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSyslogDaemonDlg, CDialog)
	//{{AFX_MSG_MAP(CSyslogDaemonDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSyslogDaemonDlg message handlers

BOOL CSyslogDaemonDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
	
    // TODO: Add extra initialization here
    register LONG rc;
    CWaitCursor	cWait;
    HKEY hKeyRemote, hKey;
    CString csKeyName;
    DWORD dwType, dwSize;
    TCHAR szBuffer[255+1];

	// Open the registry on HKLM
    if (RegConnectRegistry(m_csComputer, HKEY_LOCAL_MACHINE, &hKeyRemote) != ERROR_SUCCESS)
    {
	csKeyName.Format(
		_T("Error while connecting to the registry !\n\nEnsure that\n\n%s\n%s"),
		_T("1) Network Registry access is enabled if this is a remote computer."),
		_T("2) You have Administrator privilieges on the computer."));
	AfxMessageBox(csKeyName, MB_ICONSTOP);
	CDialog::OnCancel();
	return TRUE;
    }

    // Open the SOFTWARE\SaberNet key
    if (RegOpenKeyEx(hKeyRemote, NTSYSLOG_SOFTWARE_KEY, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
	// Key exist => read values
	// Primary Syslogd
	dwSize = sizeof(szBuffer) - sizeof(TCHAR);
	rc = RegQueryValueEx(hKey, PRIMARY_SYSLOGD_ENTRY, 0, &dwType, (LPBYTE)szBuffer, &dwSize);
	szBuffer[255] = '\0';
	if( (rc == ERROR_SUCCESS) && (dwSize > 1) && (dwType == REG_SZ) )
	    SetDlgItemText(IDC_PRIMARY_SYSLOGD, szBuffer);
	// Backup Syslogd
	dwSize = sizeof(szBuffer) - sizeof(TCHAR);
	LONG rc = RegQueryValueEx(hKey, BACKUP_SYSLOGD_ENTRY, 0, &dwType, (LPBYTE)szBuffer, &dwSize);
	szBuffer[255] = '\0';
	if( (rc == ERROR_SUCCESS) && (dwSize > 1) && (dwType == REG_SZ) )
	    SetDlgItemText(IDC_BACKUP_SYSLOGD, szBuffer);
	RegCloseKey(hKey);
    }

    RegCloseKey(hKeyRemote);
    return TRUE;	// return TRUE unless you set the focus to a control
			// EXCEPTION: OCX Property Pages should return FALSE
}

void CSyslogDaemonDlg::OnOK() 
{
	// TODO: Add extra validation here
	CWaitCursor	cWait;
	HKEY		hKeyRemote,
				hKeySoftware;
	DWORD		dwSize,
				dwValue;
	CString		csPrimary,
				csBackup;

	if (IsModified() && (AfxMessageBox( _T( "Do you really want to save Syslog Daemons parameters ?\n\nNB: if service is started, you must restart it for change to take effect."),
				MB_YESNO|MB_ICONQUESTION) == IDNO))
	{
		// Discard changes
		CDialog::OnCancel();
		return;
	}

	if (GetDlgItemText( IDC_PRIMARY_SYSLOGD, csPrimary) == 0)
	{
		AfxMessageBox( _T( "You MUST specify at least a Primary Syslog Daemon server !"), MB_ICONSTOP);
		return;
	}
	GetDlgItemText( IDC_BACKUP_SYSLOGD, csBackup);

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
	// Write the primary syslogd server
	dwSize = csPrimary.GetLength();
	if (RegSetValueEx( hKeySoftware, PRIMARY_SYSLOGD_ENTRY, 0, REG_SZ, (LPBYTE ) LPCTSTR( csPrimary), dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey (hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}
	// Write the backup syslogd server
	if (!csBackup.IsEmpty())
	{
		dwSize = csBackup.GetLength();
		if (RegSetValueEx( hKeySoftware, BACKUP_SYSLOGD_ENTRY, 0, REG_SZ, (LPBYTE ) LPCTSTR( csBackup), dwSize) != ERROR_SUCCESS)
		{
			AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
			RegCloseKey (hKeySoftware);
			RegCloseKey( hKeyRemote);
			return;
		}
	}
	else
		RegDeleteValue( hKeySoftware, BACKUP_SYSLOGD_ENTRY);
	RegCloseKey (hKeySoftware);
	RegCloseKey( hKeyRemote);
	CDialog::OnOK();
}

void CSyslogDaemonDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if (IsModified() && (AfxMessageBox( _T( "Do you really want to discard Syslog Daemons parameters changes ?"),
			MB_YESNO|MB_ICONQUESTION) == IDNO))
			return;
	CDialog::OnCancel();
}

void CSyslogDaemonDlg::SetComputer(LPCTSTR lpstrComputer)
{
	if ((lpstrComputer == NULL) || (_tcsclen( lpstrComputer) == 0))
		m_csComputer.Empty();
	else
		// Set computer in \\computer_name format
		m_csComputer.Format( _T( "\\\\%s"), lpstrComputer);
}

BOOL CSyslogDaemonDlg::IsModified()
{
	if (SendDlgItemMessage( IDC_PRIMARY_SYSLOGD, EM_GETMODIFY, 0, 0L))
		return TRUE;
	if (SendDlgItemMessage( IDC_BACKUP_SYSLOGD, EM_GETMODIFY, 0, 0L))
		return TRUE;
	return FALSE;
}
