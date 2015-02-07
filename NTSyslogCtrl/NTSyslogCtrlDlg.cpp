// NTSyslogCtrlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NTSyslogCtrl.h"
#include "NTService.h"
#include "NTSyslogCtrlDlg.h"

#include "ConfigLogging.h"
#include "SyslogDaemonDlg.h"
#include "SelectServerDlg.h"

#include <winevt.h>

#pragma comment(lib, "wevtapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//MAYBE HAVE TO COMMENT THIS OUT AND CALL IT
//THE WAY THE OTHER ONE IS CALLED
void CNTSyslogCtrlDlg::OnAppAbout()
{
	CAboutDlg aDlg;
	aDlg.DoModal();
}

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//CDialog(IDD_ABOUTBOX).DoModal();
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString	csAppTitle;

//	csAppTitle.LoadString( AFX_IDS_APP_TITLE);
//	SetDlgItemText( IDC_STATUS, csAppTitle);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CNTSyslogCtrlDlg dialog

CNTSyslogCtrlDlg::CNTSyslogCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNTSyslogCtrlDlg::IDD, pParent)
	, m_NoFullComputerName(FALSE)
	, m_OutputCode(_T(""))
{
	//{{AFX_DATA_INIT(CNTSyslogCtrlDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//  m_StringCode = 0;
	m_CharCode = 0;
}

//***MUST*** be called ***AFTER*** setting the m_csComputer property for
//CNTSyslogCtrlDlg objects
void CNTSyslogCtrlDlg::ReadMachineEventLogSettings(int good_query)
{
	HKEY			hKeyRemote,
					hKey;

	//Sometimes connecting to a remote computer takes a while
	//Especially if that computer doesn't exist on the network
	//or is off-line
	CWaitCursor		cWait;

	//reset the EventlogSelect property & free memory to avoid memory leaks
	m_csaEventlogSelect.RemoveAll();
	m_csaEventlogSelect.FreeExtra();

	//set the EventlogSelect size to the default
	m_csaEventlogSelectSize = 1;

	//Check if query service status was successful
	//if it wasn't skip attempting to connect to the registry
	//Attempting to connect if the computer is unreachable
	//adds about 15-30 seconds on loading time each time
	//an unreachable computer is selected
	if(good_query == TRUE)
	{
		OSVERSIONINFO osvi;
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx (&osvi);

		if (osvi.dwMajorVersion < 6)
		{
			// Connect to the selected computer's registry on HKLM
			if (RegConnectRegistry( (char*)(LPCTSTR)m_csComputer, HKEY_LOCAL_MACHINE, &hKeyRemote) == ERROR_SUCCESS)
			{
				CStringArray __tmp;
				//Open the key to where Windows stores EventLog info
				if (RegOpenKeyEx( hKeyRemote, EVENTLOG_REG_PATH, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
				{
					//Read the subkeys in HKLM\System\CurrentControlSet\Services\Eventlog
					DWORD no_subkeys, subkey_max_len;
					if(::RegQueryInfoKey(hKey,NULL,NULL,NULL,&no_subkeys,&subkey_max_len,NULL,NULL,NULL,NULL,NULL,NULL) == ERROR_SUCCESS )
					{
						subkey_max_len++;
						m_csaEventlogSelectSize = no_subkeys;

						//loop until done reading all subkeys
						for(DWORD index = 0; index < no_subkeys; index++ )
						{
							CString buffer;
							DWORD buffer_size = subkey_max_len;

							LONG retval = RegEnumKeyEx( hKey, index, buffer.GetBuffer(buffer_size), &buffer_size, NULL, NULL, NULL, NULL);
							if(retval == ERROR_SUCCESS && retval != ERROR_NO_MORE_ITEMS)
							{
								__tmp.Add((LPCSTR)buffer);
							}//end if(retval == ERROR_SUCCESS && retval != ERROR_NO_MORE_ITEMS)
						}//end for(DWORD index = 0; index < no_subkeys; index++ )
					}//end if(ReqQueryInfoKey(hKeyRemote,NULL,NULL,NULL,&no_subkeys,&subkey_max_len,NULL,NULL,NUL.NULL,NULL) == ERROR_SUCCESS)
					//don't need the handles to the Registry anymore
					RegCloseKey(hKey);
					RegCloseKey(hKeyRemote);

					//populate the m_csaEventlogSelect CString array
					//no apparent need to sort this as RegEnumKeyEx appears
					//to be returning the registry key names in alphabetical order
					for(DWORD i = 0; i < no_subkeys; i++)
					{
						m_csaEventlogSelect.Add(__tmp.GetAt(i));
					}//end for(DWORD i = 0; i < no_subkeys; i++)
				}//end if (RegOpenKeyEx( hKeyRemote, EVENTLOG_REG_PATH, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
			}//end if (RegConnectRegistry( (char*)((LPCTSTR)m_csComputer), HKEY_LOCAL_MACHINE, &hKeyRemote) != ERROR_SUCCESS)
		}//end if OS version check
		else // Windows Vista/2008
		{
			EVT_HANDLE hReadEvent = NULL;
			DWORD dwBufferSize = 0;
			DWORD dwBufferUsed = 0;
			DWORD status = ERROR_SUCCESS;
			LPWSTR pBuffer = NULL;
			char channel[256];

			EVT_HANDLE hChannels = EvtOpenChannelEnum(NULL, 0);

			if (NULL == hChannels)
			{
#ifdef _DEBUG
				TRACE ("EvtOpenChannelEnum failed with %lu.\n", GetLastError());
#endif
			}

			while (true)
			{
				if (!EvtNextChannelPath(hChannels, dwBufferSize, pBuffer, &dwBufferUsed))
				{
					status = GetLastError();

					if (ERROR_NO_MORE_ITEMS == status)
					{
						break;
					}
					else if (ERROR_INSUFFICIENT_BUFFER == status)
					{
						LPWSTR pTemp = (LPWSTR)realloc(pBuffer, dwBufferUsed * sizeof(WCHAR));
						dwBufferSize = dwBufferUsed;
						if (pTemp)
						{
							pBuffer = pTemp;
							pTemp = NULL;
							EvtNextChannelPath(hChannels, dwBufferSize, pBuffer, &dwBufferUsed);
						}
						else
						{
#ifdef _DEBUG
							TRACE ("realloc failed\n");
#endif
							status = ERROR_OUTOFMEMORY;
							break;
						}
					}
					else
					{
#ifdef _DEBUG
						TRACE ("EvtNextChannelPath failed with %lu.\n", status);
#endif
					}
				}

				ZeroMemory(channel, sizeof(channel));
				WideCharToMultiByte(CP_THREAD_ACP,0,pBuffer, -1, channel, 256, NULL, NULL);
				m_csaEventlogSelect.Add(channel);
				m_csaEventlogSelectSize ++;
			}
			m_csaEventlogSelectSize --;
			if (hChannels) EvtClose(hChannels);
			if (pBuffer) free(pBuffer);
		}
	}//end if(good_query == TRUE)
	else
	{
		//Generate an error message
		CString __Fmt;
		__Fmt.Format( _T("Error while connecting to the registry on %s!\n\nPlease verify permissions on %s."),m_csComputer,m_csComputer);
		AfxMessageBox(__Fmt, MB_ICONSTOP);
		//Add extra notification to the soon to be disabled combobox
		m_csaEventlogSelect.Add("Error Connecting to Registry");
	}//end else clause of if(good_query == TRUE)

	//Now need to populate the combo box with data
	// Get pointer to the combo boxe.
	CComboBox *peType = (CComboBox *)GetDlgItem(IDC_EVENTLOG_SELECT);
	peType->ResetContent();

	// If it doesn't exist stop rather than crashing.  Should never happen.
	if(peType == NULL)
	{
		AfxMessageBox("Program error: Dialog now missing controls.  This "
				"message should never appear.", MB_ICONSTOP);
		CDialog::OnCancel();
	}

	for(DWORD i = 0; i < m_csaEventlogSelectSize; i++)
	{
		peType->AddString( m_csaEventlogSelect.GetAt(i));
	}

	//add something here to set the combobox to the first value in the list
	peType->SelectString(0,m_csaEventlogSelect.GetAt(0));

}//end NYSyslogCtrlDlg::ReadMachineEventLogSettings()

void CNTSyslogCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNTSyslogCtrlDlg)
	DDX_Control(pDX, IDC_STATUS_LIGHT, m_StatusIcon);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_FULLCN_NON, m_NoFullComputerName);
	//  DDX_CBIndex(pDX, IDC_STRINGCODE, m_StringCode);
	DDX_CBIndex(pDX, IDC_CHARCODE, m_CharCode);
}

BEGIN_MESSAGE_MAP(CNTSyslogCtrlDlg, CDialog)
	//{{AFX_MSG_MAP(CNTSyslogCtrlDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SELECT_COMPUTER, OnSelectComputer)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SYSLOGD, OnSyslogd)
	ON_BN_CLICKED(IDC_EVENTLOG, OnEventLog)
	ON_BN_CLICKED(IDC_START, OnStartService)
	ON_BN_CLICKED(IDC_STOP, OnStopService)
	ON_BN_CLICKED(IDC_ABOUTBOX, OnAppAbout)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, &CNTSyslogCtrlDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNTSyslogCtrlDlg message handlers

BOOL CNTSyslogCtrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	// Load last computer from the registry
	m_csComputer = AfxGetApp()->GetProfileString( COMPUTERS_SECTION, LAST_COMPUTER_ENTRY );

	// R2専用コントロールの初期化
	HKEY	hReg;
	long	rc, rv;

	m_CharCode = 0; // 0:UTF8 , 1:EUC-JP , 2:SJIS
	m_NoFullComputerName = false;
	rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, NTSYSLOG_REGISTRY_KEY, 0, KEY_READ, &hReg);
	if( rc == ERROR_SUCCESS )
	{
		DWORD   typ,size = sizeof(rv);

		// フルコンピュータ名出力抑制フラグ
		rc = RegQueryValueEx(hReg, "FullComputerName", NULL, &typ, (BYTE*)&rv, &size) ;
		if (rc == 0 && rv == 1) { m_NoFullComputerName = true; } else { m_NoFullComputerName = false; }

		// 文字コード
		rc = RegQueryValueEx(hReg, "CharCode", NULL, &typ, (BYTE*)&rv, &size) ;
		if (rc == 0) { m_CharCode = rv; }
	}
	UpdateData(false);
	RegCloseKey(hReg);

	SetComputerName();
	int queryserviceresults = ( QueryServiceStatus() ) ? 1 : 0;
	SetMainDialogControls(queryserviceresults);
	ReadMachineEventLogSettings(queryserviceresults);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNTSyslogCtrlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNTSyslogCtrlDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNTSyslogCtrlDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CNTSyslogCtrlDlg::OnSelectComputer() 
{
	// TODO: Add your control notification handler code here
	CSelectServerDlg	cDlg;

	cDlg.SetCurrentComputer( m_csComputer);
	if (cDlg.DoModal() == IDCANCEL)
		return;
	//Set a wait cursor -- PERHAPS THIS IS CONFUSING THE ISSUE
	CWaitCursor				 cWait;
	m_csComputer = cDlg.GetNewComputer();
	// Write computer name to the registry
	AfxGetApp()->WriteProfileString( COMPUTERS_SECTION, LAST_COMPUTER_ENTRY, m_csComputer);
	SetComputerName(); 
	KillTimer( 1);
	//Check for service on remote machine and set dialog buttons
	int queryserviceresults = (QueryServiceStatus()) ? 1 : 0;
	SetMainDialogControls(queryserviceresults);
	//Setup the dialog with the new machine settings, if any
	ReadMachineEventLogSettings(queryserviceresults);
}

//This function either enables or disables the contained buttons
//from the main dialog.  Use 1 to enable, 0 to disable.  Any other
//buttons added later which require enabling or disabling should
//be placed here
void CNTSyslogCtrlDlg::SetMainDialogControls(int _i_)
{
	if(_i_)
		SetTimer( 1, 1000, NULL);

	GetDlgItem( IDC_SYSLOGD)->EnableWindow( _i_);
	GetDlgItem( IDC_EVENTLOG)->EnableWindow( _i_);
	GetDlgItem( IDC_EVENTLOG_SELECT)->EnableWindow( _i_);
}

void CNTSyslogCtrlDlg::OnTimer(UINT_PTR nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnTimer(nIDEvent);
	if (QueryServiceStatus())
		SetTimer( 1, 1000, NULL);
}

//Check the status of the NTSyslog service on the selected computer
BOOL CNTSyslogCtrlDlg::QueryServiceStatus()
{
	CNTScmService			 myService;
	CNTServiceControlManager mySCM;
	SERVICE_STATUS			 myServiceStatus;
	CString					 csComputer;

	if (!m_csComputer.IsEmpty())
		// Set computer in \\computer_name format
		csComputer.Format( _T( "\\\\%s"), m_csComputer);
	else
		csComputer.Empty();
	if (!mySCM.Open( csComputer, SC_MANAGER_ALL_ACCESS|GENERIC_READ))
	{
		DisplayStatus( IDI_ERROR_ICON);
		return FALSE;
	}
	if (!mySCM.OpenService( NTSYSLOG_SERVICE_NAME, SERVICE_QUERY_STATUS, myService))
	{
		DisplayStatus( IDI_ERROR_ICON);
		mySCM.Close();
		return FALSE;
	}
	if (!myService.QueryStatus( &myServiceStatus))
	{
		DisplayStatus( IDI_ERROR_ICON);
		myService.Close();
		mySCM.Close();
		return FALSE;
	}
	myService.Close();
	mySCM.Close();
	switch (myServiceStatus.dwCurrentState)
	{
	case SERVICE_START_PENDING:
		DisplayStatus( IDI_YELLOW_ICON, myServiceStatus.dwCurrentState);
		break;
	case SERVICE_RUNNING:
		DisplayStatus( IDI_GREEN_ICON);
		break;
	case SERVICE_STOP_PENDING:
		DisplayStatus( IDI_YELLOW_ICON, myServiceStatus.dwCurrentState);
		break;
	case SERVICE_STOPPED:
		DisplayStatus( IDI_RED_ICON);
		break;
	case SERVICE_CONTINUE_PENDING:
		DisplayStatus( IDI_YELLOW_ICON, myServiceStatus.dwCurrentState);
		break;
	case SERVICE_PAUSE_PENDING:
		DisplayStatus( IDI_YELLOW_ICON, myServiceStatus.dwCurrentState);
		break;
	case SERVICE_PAUSED:
		DisplayStatus( IDI_YELLOW_ICON, myServiceStatus.dwCurrentState);
		break;
	default:
		DisplayStatus( IDI_ERROR_ICON);
		break;
	}
	return TRUE;
}



BOOL CNTSyslogCtrlDlg::DisplayStatus(UINT nIconID, DWORD dwServiceState)
{
	HICON	hIcon;

	if ((hIcon = AfxGetApp()->LoadIcon( nIconID)) == NULL)
		return FALSE;
	m_StatusIcon.SetIcon( hIcon);
	switch (nIconID)
	{
	case IDI_GREEN_ICON: // Service started
		SetDlgItemText( IDC_STATUS, _T( "サービスは動作中です"));
		GetDlgItem( IDC_START)->EnableWindow( FALSE);
		GetDlgItem( IDC_STOP)->EnableWindow( TRUE);
		break;
	case IDI_YELLOW_ICON: // Service pending
		switch (dwServiceState)
		{
		case SERVICE_START_PENDING:
			SetDlgItemText( IDC_STATUS, _T( "サービスは開始しました"));
			break;
		case SERVICE_STOP_PENDING:
			SetDlgItemText( IDC_STATUS, _T( "サービスは停止しました"));
			break;
		case SERVICE_CONTINUE_PENDING:
			SetDlgItemText( IDC_STATUS, _T( "サービスを再開しています"));
			break;
		case SERVICE_PAUSE_PENDING:
			SetDlgItemText( IDC_STATUS, _T( "サービスを一時停止中です"));
			break;
		case SERVICE_PAUSED:
			SetDlgItemText( IDC_STATUS, _T( "サービスは一時停止しました"));
			break;
		default:
			SetDlgItemText( IDC_STATUS, _T( "サービスは不明な状態です"));
			break;
		}
		GetDlgItem( IDC_START)->EnableWindow( FALSE);
		GetDlgItem( IDC_STOP)->EnableWindow( FALSE);
		break;
	case IDI_RED_ICON: // Service stoppped
		SetDlgItemText( IDC_STATUS, _T( "サービスは停止しています"));
		GetDlgItem( IDC_START)->EnableWindow( TRUE);
		GetDlgItem( IDC_STOP)->EnableWindow( FALSE);
		break;
	case IDI_ERROR_ICON: // Error
	default:
		SetDlgItemText( IDC_STATUS, _T( "サービスはインストールされていないかエラーが発生しました"));
		GetDlgItem( IDC_START)->EnableWindow( FALSE);
		GetDlgItem( IDC_STOP)->EnableWindow( FALSE);
		break;
	}
	return TRUE;
}

void CNTSyslogCtrlDlg::OnSyslogd() 
{
	// TODO: Add your control notification handler code here
	CSyslogDaemonDlg	cDlg;

	cDlg.SetComputer( m_csComputer);
	cDlg.DoModal();
}

void CNTSyslogCtrlDlg::OnEventLog()
{

	CConfigLogging cDlg;

	// Get pointers to the combo boxes.
	CComboBox *peType = (CComboBox *)GetDlgItem(IDC_EVENTLOG_SELECT);

	// If doesn't exist, stop rather than crashing.  Should never happen.
	if(peType == NULL)
	{
		AfxMessageBox("Program error: Dialog now missing controls.  This "
				"message should never appear.", MB_ICONSTOP);
		CDialog::OnCancel();
	}

	// Get the selection.  Depends on the dialog having the right items in it.
	int cur_index = peType->GetCurSel();
	
	// Again, make sure we got values back.
	if(cur_index == CB_ERR)
	{
		AfxMessageBox("Program error: Dialog now returns errors.  This "
				"message should never appear.", MB_ICONSTOP);
		CDialog::OnCancel();
	}

	cDlg.SetupDialog(m_csaEventlogSelect.GetAt(cur_index), m_csComputer);
	cDlg.DoModal();
}

void CNTSyslogCtrlDlg::SetComputerName()
{
	CString	csMessage;

	csMessage.Format( _T( "Service status on computer <%s>..."),
					 (m_csComputer.IsEmpty() ? _T( "Local Machine") : m_csComputer));
	SetDlgItemText( IDC_COMPUTER, csMessage);
}

void CNTSyslogCtrlDlg::OnStartService() 
{
	// TODO: Add your control notification handler code here
	CNTScmService			 myService;
	CNTServiceControlManager mySCM;
	CString					 csComputer;
	CWaitCursor				 cWait;

	HKEY	hReg;
	long	rc;

	UpdateData(true);
	rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, NTSYSLOG_REGISTRY_KEY, 0, KEY_WRITE, &hReg);
	if( rc == ERROR_SUCCESS )
	{
		DWORD   val,size = sizeof(val);

		// フルコンピュータ名出力抑制フラグ
		val = m_NoFullComputerName;
	    rc = RegSetValueEx(hReg, "FullComputerName", 0, REG_DWORD, (BYTE*)&val, size);

		// 文字コード
		val = m_CharCode;
	    rc = RegSetValueEx(hReg, "CharCode", 0, REG_DWORD, (BYTE*)&val, size);
	}
	RegCloseKey(hReg);

	if (!m_csComputer.IsEmpty())
		// Set computer in \\computer_name format
		csComputer.Format( _T( "\\\\%s"), m_csComputer);
	else
		csComputer.Empty();
	if (!mySCM.Open( csComputer, SC_MANAGER_ALL_ACCESS|GENERIC_READ))
	{
		AfxMessageBox( _T( "Unable to contact Service Control Manager !"), MB_ICONSTOP);
		return;
	}
	if (!mySCM.OpenService( NTSYSLOG_SERVICE_NAME, SERVICE_START, myService))
	{
		mySCM.Close();
		AfxMessageBox( _T( "Unable to send command to Service Control Manager !"), MB_ICONSTOP);
		return;
	}
	if (!myService.Start( 0, NULL))
	{
		myService.Close();
		mySCM.Close();
		AfxMessageBox( _T( "Error while sending command to Service Control Manager !"), MB_ICONSTOP);
		return;
	}
	myService.Close();
	mySCM.Close();
	QueryServiceStatus();
}

void CNTSyslogCtrlDlg::OnStopService() 
{
	// TODO: Add your control notification handler code here
	CNTScmService			 myService;
	CNTServiceControlManager mySCM;
	CString					 csComputer;
	CWaitCursor				 cWait;

	if (!m_csComputer.IsEmpty())
		// Set computer in \\computer_name format
		csComputer.Format( _T( "\\\\%s"), m_csComputer);
	else
		csComputer.Empty();
	if (!mySCM.Open( csComputer, SC_MANAGER_ALL_ACCESS|GENERIC_READ))
	{
		AfxMessageBox( _T( "Unable to contact Service Control Manager !"), MB_ICONSTOP);
		return;
	}
	if (!mySCM.OpenService( NTSYSLOG_SERVICE_NAME, SERVICE_STOP, myService))
	{
		mySCM.Close();
		AfxMessageBox( _T( "Unable to send command to Service Control Manager !"), MB_ICONSTOP);
		return;
	}
	if (!myService.Stop())
	{
		myService.Close();
		mySCM.Close();
		AfxMessageBox( _T( "Error while sending command to Service Control Manager !"), MB_ICONSTOP);
		return;
	}
	myService.Close();
	mySCM.Close();
	QueryServiceStatus();
}

void CNTSyslogCtrlDlg::OnBnClickedCancel()
{
	HKEY	hReg;
	long	rc;

	UpdateData(true);
	rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, NTSYSLOG_REGISTRY_KEY, 0, KEY_WRITE, &hReg);
	if( rc == ERROR_SUCCESS )
	{
		DWORD   val,size = sizeof(val);

		// フルコンピュータ名出力抑制フラグ
		val = m_NoFullComputerName;
	    rc = RegSetValueEx(hReg, "FullComputerName", 0, REG_DWORD, (BYTE*)&val, size);

		// 文字コード
		val = m_CharCode;
	    rc = RegSetValueEx(hReg, "CharCode", 0, REG_DWORD, (BYTE*)&val, size);
	}
	RegCloseKey(hReg);

	CDialog::OnCancel();
}
