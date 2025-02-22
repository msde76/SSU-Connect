// CRegisterDlg.cpp: 구현 파일
//

#include "pch.h"
#include "MFCApplication4.h"
#include "afxdialogex.h"
#include "CRegisterDlg.h"
#include "MFCApplication4Doc.h"


// CRegisterDlg 대화 상자

IMPLEMENT_DYNAMIC(CRegisterDlg, CDialogEx)

CRegisterDlg::CRegisterDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_DIALOG_REGISTER, pParent)
{

}

CRegisterDlg::~CRegisterDlg()
{
}

void CRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRegisterDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &CRegisterDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CRegisterDlg 메시지 처리기


void CRegisterDlg::OnBnClickedOk()
{
    // DB 연결 준비
    CString m_StrRegi_name;
    CString m_StrRegi_id;
    CString m_StrRegi_pw;

    // 각각의 Edit에서 이름, id, pw 가져오기
    GetDlgItemText(IDC_EDIT_REGI_NAME, m_StrRegi_name);
    GetDlgItemText(IDC_EDIT_REGI_ID, m_StrRegi_id);
    GetDlgItemText(IDC_EDIT_REGI_PW, m_StrRegi_pw);

    // DB 연결
    MYSQL Conn;
    mysql_init(&Conn);
    MYSQL* ConnPtr = mysql_real_connect(&Conn, MY_IP, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);

    // ID 중복 확인 쿼리
    CString CheckQuery;
    CheckQuery.Format(_T("SELECT COUNT(*) FROM user WHERE user_id='%s'"), m_StrRegi_id);
    CT2A asciiCheckQuery(CheckQuery);
    const char* checkQueryChar = asciiCheckQuery;

    if (mysql_query(ConnPtr, checkQueryChar) == 0) {
        MYSQL_RES* res = mysql_store_result(ConnPtr);
        MYSQL_ROW row = mysql_fetch_row(res);

        // id가 같을 경우
        if (row && atoi(row[0]) > 0) {
            MessageBox(_T("이미 사용중인 아이디입니다."), _T("오류"));
            mysql_free_result(res);
            mysql_close(ConnPtr);
            return; // 중복 ID일 경우 함수 종료
        }
        mysql_free_result(res);
    }

    // ID가 중복되지 않는 경우
    CString Query;
    Query.Format(
        _T("INSERT INTO user (user_id, user_pw, user_name, user_friend) VALUES('%s', '%s', '%s', '')"),
        m_StrRegi_id, m_StrRegi_pw, m_StrRegi_name
    );

    //CT2A asciiQuery(Query); // CString to ASCII
    CW2A utfQuery(Query, CP_UTF8);
    char* queryChar = utfQuery;

    // 쿼리 오류 시
    int Stat = mysql_query(ConnPtr, queryChar);
    if (Stat != 0) {
        MessageBox(NULL, _T("쿼리 오류"), MB_OK);
    }

    mysql_close(ConnPtr);
    CDialogEx::OnOK();
}


