﻿// CCheckDlg.cpp: 구현 파일
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
    // DB와 확인 후 비밀번호가 동일하면 회원탈퇴, 아니면 다시 입력하세요 모달 출력
    CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
    //pMainFrame에서 현재 사용자의 ID와 패스워드 불러오기
    CString m_strId = pMainFrame->m_strUserId;
    CString m_strPw = pMainFrame->m_strUserPw;

    //입력된 비밀번호
    CString m_strCkpw;
    GetDlgItemText(IDC_EDIT_CKPW, m_strCkpw);

    //입력한 비밀번호와 실제 사용자의 비밀번호가 일치하는지 확인
    if (m_strCkpw == m_strPw) {
        //회원탈퇴의 여부를 물어보는 메세지 박스 출력.  IDYES를 받아온다면 현재 사용자의 정보를 user테이블에서 제거
        if (AfxMessageBox(_T("정말 회원탈퇴하시겠습니까?"), MB_YESNO) == IDYES) {
            CString query;
            //user테이블에서 현재 사용자의 아이디와 비밀번호가 들어가 있는 행(record)를 찾아 제거함.
            query.Format(_T("DELETE FROM user WHERE user_id='%s' AND user_pw='%s'"), m_strId, m_strPw);

            //Query문을 MYSQL에서 인식을 시키기 위해서 ASCII문자열로 변경
            CT2A asciiQuery(query);
            const char* queryChar = asciiQuery;

            // DB 연결
            MYSQL Conn;
            mysql_init(&Conn);
            MYSQL* ConnPtr = mysql_real_connect(&Conn, MY_IP, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);

            // Query 실패 시
            int Stat = mysql_query(ConnPtr, queryChar);
            if (Stat != 0) {
                MessageBox(NULL, _T("쿼리 오류"), MB_OK);
            }

            Stat = mysql_query(ConnPtr, queryChar);

        }
        EndDialog(IDYES);
    }
    else {
        //입력된 비밀번호가 현재 비밀번호와 다르다면 에러 메세지 박스 출력
        MessageBox(_T("잘못된 비밀번호입니다. 다시 입력하세요."), _T("Error"), MB_ICONERROR);
        SetDlgItemText(IDC_EDIT_CKPW, _T("")); // 입력한 비밀번호 초기화
        return; // 다이얼로그를 종료하지 않고 그대로 유지
    }
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

    CDialogEx::OnOK();
}
