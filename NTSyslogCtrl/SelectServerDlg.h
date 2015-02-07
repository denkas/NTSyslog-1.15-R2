#if !defined(AFX_SELECTSERVERDLG_H__8247ACD3_E1AD_11D5_B309_0040055338AF__INCLUDED_)
#define AFX_SELECTSERVERDLG_H__8247ACD3_E1AD_11D5_B309_0040055338AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectServerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectServerDlg dialog

class CSelectServerDlg : public CDialog
{
// Construction
public:
	LPCTSTR GetNewComputer();
	void SetCurrentComputer( LPCTSTR lpstrComputer);
	CSelectServerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectServerDlg)
	enum { IDD = IDD_SERVER_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectServerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_csComputer;

	// Generated message map functions
	//{{AFX_MSG(CSelectServerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowse();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTSERVERDLG_H__8247ACD3_E1AD_11D5_B309_0040055338AF__INCLUDED_)
