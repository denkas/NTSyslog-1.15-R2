#if !defined(AFX_APPLICATIONEVENTLOGDLG_H__98503D16_9608_11D5_B2A2_0040055338AF__INCLUDED_)
#define AFX_APPLICATIONEVENTLOGDLG_H__98503D16_9608_11D5_B2A2_0040055338AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ApplicationEventLogDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CApplicationEventLogDlg dialog

class CApplicationEventLogDlg : public CDialog
{
// Construction
public:
	CApplicationEventLogDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CApplicationEventLogDlg)
	enum { IDD = IDD_APPLICATION_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	void SetComputer( LPCTSTR lpstrComputer = NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CApplicationEventLogDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_csComputer;
	UINT m_uCurrentState;
	BOOL IsModified();

	// Generated message map functions
	//{{AFX_MSG(CApplicationEventLogDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDefaults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPLICATIONEVENTLOGDLG_H__98503D16_9608_11D5_B2A2_0040055338AF__INCLUDED_)
