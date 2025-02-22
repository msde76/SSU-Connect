// CChangeDlg.cpp: 구현 파일
//

#include "pch.h"
#include "MFCApplication4.h"
#include "afxdialogex.h"
#include "CChangeDlg.h"
#include "MainFrm.h"

// CChangeDlg 대화 상자

IMPLEMENT_DYNAMIC(CChangeDlg, CDialog)

CChangeDlg::CChangeDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_CHPW, pParent)
{

}

CChangeDlg::~CChangeDlg()
{
}

void CChangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CChangeDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CChangeDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CChangeDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CChangeDlg 메시지 처리기


void CChangeDlg::OnBnClickedOk()
{
    CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
    CString m_strId = pMainFrame->m_strUserId;
    CString m_strPw = pMainFrame->m_strUserPw;
	CString m_strCurrentpw;
	CString m_strNewpw;
	GetDlgItemText(IDC_EDIT_CURRENTPW, m_strCurrentpw);
	GetDlgItemText(IDC_EDIT_NEWPW, m_strNewpw);
	if (m_strPw != m_strCurrentpw) {
		MessageBox(_T("잘못된 비밀번호 입력입니다."));
	}
	else{
        MYSQL Conn;
        MYSQL* ConnPtr = NULL;
        MYSQL_ROW Row;

        mysql_init(&Conn);
        ConnPtr = mysql_real_connect(&Conn, MY_IP, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);

        if (ConnPtr == NULL) {
            MessageBox(_T("데이터베이스 연결 실패"));
            return;
        }

        mysql_query(ConnPtr, "set session character_set_connection=euckr;");
        mysql_query(ConnPtr, "set session character_set_results=euckr;");
        mysql_query(ConnPtr, "set session character_set_client=euckr;");

        // 모든 사용자 정보를 가져오는 쿼리
        CString Query;

        Query.Format(
            _T("UPDATE user SET user_pw = '%s' WHERE user_id = '%s'"), m_strNewpw, m_strId
        );


        CT2A asciiQuery(Query); // CString to ASCII
        const char* queryChar = asciiQuery;

        if (mysql_query(ConnPtr, queryChar)) {
            TRACE("쿼리 실행 실패: %s\n", mysql_error(ConnPtr));
            return;
        }

        mysql_close(ConnPtr); // 연결 해제
        MessageBox(_T("비밀번호 변경 성공"));
        pMainFrame->SetUserInfo(m_strId, m_strNewpw, pMainFrame->m_strUserName);
	}
	CDialog::OnOK();
}


void CChangeDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}
