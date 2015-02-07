#if !defined(AFX_SECURITYEVENTLOGDLG_H__98503D17_9608_11D5_B2A2_0040055338AF__INCLUDED_)
#define AFX_SECURITYEVENTLOGDLG_H__98503D17_9608_11D5_B2A2_0040055338AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SecurityEventLogDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSecurityEventLogDlg dialog

class CSecurityEventLogDlg : public CDialog
{
// Construction
public:
	CSecurityEventLogDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSecurityEventLogDlg)
	enum { IDD = IDD_SECURITY_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	void SetComputer( LPCTSTR lpstrComputer = NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSecurityEventLogDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_csComputer;
	UINT m_uCurrentState;
	BOOL IsModified();

	// Generated message map functions
	//{{AFX_MSG(CSecurityEventLogDlg)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDefaults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SECURITYEVENTLOGDLG_H__98503D17_9608_11D5_B2A2_0040055338AF__INCLUDED_)
