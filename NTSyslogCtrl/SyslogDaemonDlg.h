#if !defined(AFX_SYSLOGDAEMONDLG_H__98503D1F_9608_11D5_B2A2_0040055338AF__INCLUDED_)
#define AFX_SYSLOGDAEMONDLG_H__98503D1F_9608_11D5_B2A2_0040055338AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SyslogDaemonDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSyslogDaemonDlg dialog

class CSyslogDaemonDlg : public CDialog
{
// Construction
public:
	void SetComputer( LPCTSTR lpstrComputer = NULL);
	CSyslogDaemonDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSyslogDaemonDlg)
	enum { IDD = IDD_SYSLOGD_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSyslogDaemonDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL IsModified();
	CString m_csComputer;

	// Generated message map functions
	//{{AFX_MSG(CSyslogDaemonDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSLOGDAEMONDLG_H__98503D1F_9608_11D5_B2A2_0040055338AF__INCLUDED_)
