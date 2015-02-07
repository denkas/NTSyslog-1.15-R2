// SelectServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NTSyslogCtrl.h"
#include "SelectServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectServerDlg dialog


CSelectServerDlg::CSelectServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectServerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectServerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSelectServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectServerDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectServerDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectServerDlg)
	ON_BN_CLICKED(IDC_SELECT_COMPUTER, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectServerDlg message handlers

BOOL CSelectServerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	TCHAR	szLocalComputer[_MAX_PATH+1];
	DWORD	dwSize = _MAX_PATH+1;

	if (m_csComputer.IsEmpty())
	{
		// Local computer name
		if (!GetComputerName( szLocalComputer, &dwSize))
			AfxMessageBox( _T( "Memory error !"), MB_ICONSTOP);
		szLocalComputer[dwSize]=0;
		SetDlgItemText( IDC_COMPUTER, szLocalComputer);
	}
	else
		SetDlgItemText( IDC_COMPUTER, m_csComputer);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectServerDlg::OnBrowse() 
{
	// TODO: Add your control notification handler code here
	BROWSEINFO		myFolder;
	LPITEMIDLIST	myFolderItemIdList,
					myNetworkItemIdList;
	TCHAR			szNewComputer[_MAX_PATH+1];
	LPMALLOC 		pIMalloc;

	if (SHGetMalloc( &pIMalloc) != NOERROR)
	{
		AfxMessageBox( _T( "Memory error !"), MB_ICONSTOP);
		return;
	}
	if (SHGetSpecialFolderLocation( m_hWnd, CSIDL_NETWORK, &myNetworkItemIdList) != NOERROR)
	{
		AfxMessageBox( _T( "Memory error !"), MB_ICONSTOP);
		return;
	}
	myFolder.hwndOwner = m_hWnd;
	myFolder.pidlRoot = myNetworkItemIdList;
	myFolder.pszDisplayName = szNewComputer;
	myFolder.lpszTitle = _T( "Select a computer which host a NTSyslog Service...");
	myFolder.ulFlags = BIF_BROWSEFORCOMPUTER;
	myFolder.lpfn = NULL;
	myFolder.lParam = 0;
	if ((myFolderItemIdList = SHBrowseForFolder( &myFolder)) == NULL)
	{
		// Cancel
		pIMalloc->Free( myNetworkItemIdList);
		return;
	}
	pIMalloc->Free( myNetworkItemIdList);
	pIMalloc->Free( myFolderItemIdList);
	SetDlgItemText( IDC_COMPUTER, szNewComputer);
}

void CSelectServerDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CSelectServerDlg::OnOK() 
{
	// TODO: Add extra validation here
	TCHAR	szLocalComputer[_MAX_PATH+1];
	DWORD	dwSize = _MAX_PATH+1;
	CString	csNewComputer;

	//Set a wait cursor
	CWaitCursor				 cWait;

	GetDlgItemText( IDC_COMPUTER, csNewComputer);
	if (!GetComputerName( szLocalComputer, &dwSize))
	{
		AfxMessageBox( _T( "Memory error !"), MB_ICONSTOP);
		return;
	}
	szLocalComputer[dwSize]=0;
	if (_tcsicmp( csNewComputer, szLocalComputer) == 0)
	{
		// Local computer name
		csNewComputer.Empty();
	}
	if (m_csComputer.CompareNoCase( csNewComputer) == 0)
	{
		// No change
		CDialog::OnCancel();
	}
	else
	{
		SetCurrentComputer(csNewComputer);
		CDialog::OnOK();
	}
}

void CSelectServerDlg::SetCurrentComputer(LPCTSTR lpstrComputer)
{
	m_csComputer = lpstrComputer;
}

LPCTSTR CSelectServerDlg::GetNewComputer()
{
	return m_csComputer;
}
