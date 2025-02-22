// CLogInDlg.cpp: 구현 파일
//

#include "pch.h"
#include "MFCApplication4.h"
#include "afxdialogex.h"
#include "CLogInDlg.h"
#include "CRegisterDlg.h"
#include "Mainfrm.h"
#include "resource.h"


// CLogInDlg 대화 상자

IMPLEMENT_DYNAMIC(CLogInDlg, CDialogEx)


CLogInDlg::CLogInDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_DIALOG_LOGIN, pParent)
{

}

CLogInDlg::~CLogInDlg()
{
}

void CLogInDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLogInDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CLogInDlg::OnClickedButtonLogin)
    ON_BN_CLICKED(IDC_BUTTON_REGISTER, &CLogInDlg::OnClickedButtonRegister)
    ON_EN_CHANGE(IDC_EDIT_PW, &CLogInDlg::OnChangeEditPw)
    ON_COMMAND(IDOK, &CLogInDlg::OnIdok)
    ON_WM_CLOSE() // WM_CLOSE 메시지 처리기 추가
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CLogInDlg 메시지 처리기

void CLogInDlg::OnClickedButtonLogin()
{
    // DB와 확인 후 맞으면 OK 아니면 아닙니다 모달 출력
    MYSQL Conn;
    MYSQL* ConnPtr = NULL;
    MYSQL_RES* Result;
    MYSQL_ROW Row;

    GetDlgItemText(IDC_EDIT_ID, m_strId);
    GetDlgItemText(IDC_EDIT_PW, m_strPw);

    mysql_init(&Conn);
    ConnPtr = mysql_real_connect(&Conn, MY_IP, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);

    if (ConnPtr == NULL) {
        MessageBox(_T("데이터베이스 연결 실패"));
        return;
    }

    mysql_query(ConnPtr, "set session character_set_connection=utf8;");
    mysql_query(ConnPtr, "set session character_set_results=utf8;");
    mysql_query(ConnPtr, "set session character_set_client=utf8;");

    // 모든 사용자 정보를 가져오는 쿼리
    char* Query = "SELECT user_id, user_pw,user_name FROM user";

    if (mysql_query(ConnPtr, Query)) {
        TRACE("쿼리 실행 실패: %s\n", mysql_error(ConnPtr));
        return;
    }

    Result = mysql_store_result(ConnPtr);
    if (Result == NULL) {
        TRACE(_T("내용 없음\n"));
        return; // 결과가 없으면 종료
    }

    bool loginSuccess = false; // 로그인 성공 여부를 추적하는 변수
    while ((Row = mysql_fetch_row(Result)) != NULL) {
        if (Row[0] == NULL || Row[1] == NULL) {
            continue; // ID 또는 비밀번호가 NULL인 경우 건너뜀
        }

        CString dbId(CA2T(Row[0], CP_UTF8)); // 데이터베이스에서 가져온 ID
        CString dbPw(CA2T(Row[1], CP_UTF8)); // 데이터베이스에서 가져온 비밀번호
        CString dbName(CA2T(Row[2], CP_UTF8));// 데이터베이스에서 이름가져오기


        if (dbId == m_strId) { // ID 비교
            if (dbPw == m_strPw) { // 비밀번호 비교
                m_strName = dbName;
                CString Text;

                MessageBox(_T("로그인 성공"));
                loginSuccess = true; // 로그인 성공 플래그 설정
                break; // 반복 종료
            }
            else {
                MessageBox(_T("로그인 실패: 비밀번호가 잘못되었습니다."));
                mysql_free_result(Result); // 메모리 해제
                mysql_close(ConnPtr); // 연결 해제
                return; // 비밀번호가 틀린 경우 종료
            }
        }
    }

    mysql_free_result(Result); // 결과 해제
    mysql_close(ConnPtr); // 연결 해제

    if (loginSuccess) {
        EndDialog(IDOK); // 로그인 성공 시 대화 상자 종료
    }
    else {
        MessageBox(_T("로그인 실패: ID가 존재하지 않습니다."));
    }
}

void CLogInDlg::OnClickedButtonRegister()
{
    CRegisterDlg Register;
    if (Register.DoModal() == IDOK) {
        //DB에 테이블 추가 및 ---- , 중복처리?
    }
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CLogInDlg::OnChangeEditPw()
{
    // TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
    // CDialogEx::OnInitDialog() 함수를 재지정 
    //하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
    // 이 알림 메시지를 보내지 않습니다.

    // TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CLogInDlg::OnIdok()
{
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CLogInDlg::OnClose()
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

    // 프로그램 종료
    CDialogEx::OnClose(); // 기본 동작 수행
    AfxGetMainWnd()->PostMessage(WM_CLOSE); // 메인 윈도우에도 종료 메시지 전달
}
