// CCheckDlg.cpp: 구현 파일
//

#include "pch.h"
#include "MFCApplication4.h"
#include "afxdialogex.h"
#include "CCheckDlg.h"
#include "MFCApplication4Doc.h"
#include "MainFrm.h"

// CCheckDlg 대화 상자

IMPLEMENT_DYNAMIC(CCheckDlg, CDialogEx)

CCheckDlg::CCheckDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CKPW, pParent)
{

}

CCheckDlg::~CCheckDlg()
{
}

void CCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCheckDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CCheckDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCheckDlg 메시지 처리기


void CCheckDlg::OnBnClickedOk()

{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CString m_strId = pMainFrame->m_strUserId;
	CString m_strPw = pMainFrame->m_strUserPw;

	CString m_strCkpw;
	GetDlgItemText(IDC_EDIT_CKPW, m_strCkpw);
	
	if (m_strCkpw == m_strPw) {
		if (AfxMessageBox(_T("정말 회원탈퇴하시겠습니까?"), MB_YESNO) == IDYES) {
			CString query;
			query.Format(_T("DELETE FROM user WHERE user_id='%s' AND user_pw='%s'"), m_strId, m_strPw);

			CT2A asciiQuery(query); // CString to ASCII
			const char* queryChar = asciiQuery;
			MYSQL Conn;
			mysql_init(&Conn);
			MYSQL* ConnPtr = mysql_real_connect(&Conn, MY_IP, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);
			int Stat = mysql_query(ConnPtr, queryChar);
			if (Stat != 0) {
				MessageBox(NULL, _T("쿼리 오류"), MB_OK);
			}

			Stat = mysql_query(ConnPtr, queryChar);

		}
		EndDialog(IDYES);
	}
	else {
		MessageBox(_T("잘못된 비밀번호입니다. 다시 입력하세요."), _T("Error"), MB_ICONERROR);
		SetDlgItemText(IDC_EDIT_CKPW, _T("")); // 입력한 비밀번호 초기화
		return; // 다이얼로그를 종료하지 않고 그대로 유지
	}
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	CDialogEx::OnOK();
}
