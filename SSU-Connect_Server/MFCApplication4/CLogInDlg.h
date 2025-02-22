#pragma once
#include "afxdialogex.h"


// CLogInDlg 대화 상자


class CLogInDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLogInDlg)

public:
	CLogInDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CLogInDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LOGIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedButtonLogin();
	afx_msg void OnClickedButtonRegister();
	afx_msg void OnChangeEditPw();
	afx_msg void OnIdok();
public:
	CString m_strId;
	CString m_strPw;
	CString m_strName;
	afx_msg void OnClose();
};
