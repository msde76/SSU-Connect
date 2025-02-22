// MainFrm.cpp: CMainFrame 클래스의 구현

#include "pch.h"
#include "framework.h"
#include "MFCApplication4.h"
#include "Resource.h"
#include "MainFrm.h"
#include "CLogInDlg.h"
#include "CCheckDlg.h"
#include "CChangeDlg.h"
#include "CChatRoomNameDlg.h"
#include "CChatRoom.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()
    ON_BN_CLICKED(IDC_BUTTON_LOGOUT, &CMainFrame::OnLogoutClicked)
    ON_BN_CLICKED(IDC_BUTTON_FRIENDS, &CMainFrame::OnFriendsClicked)
    ON_BN_CLICKED(IDC_BUTTON_CHATROOMS, &CMainFrame::OnChatRoomsClicked)
    ON_BN_CLICKED(IDC_BUTTON_SETTINGS, &CMainFrame::OnSettingsClicked)
    ON_BN_CLICKED(IDC_BUTTON_EDIT, &CMainFrame::OnEditProfileClicked)
    ON_BN_CLICKED(IDC_BUTTON_WITHDRAW, &CMainFrame::OnDeleteAccountClicked)
    ON_BN_CLICKED(IDC_BUTTON_CREATE_CHATROOM, &CMainFrame::OnCreateChatRoomClicked) // "채팅방 만들기" 버튼
    ON_BN_CLICKED(IDC_BUTTON_JOIN_CHATROOM, &CMainFrame::OnJoinChatRoomClicked) // 매핑 확인
    ON_WM_PAINT()
    ON_WM_SHOWWINDOW()
    ON_WM_SIZE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
    ID_SEPARATOR,           // 상태 줄 표시기
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() noexcept
    : m_currentScreen(SCREEN_NONE)  // 초기 화면 상태 설정
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    m_currentScreen = SCREEN_FRIENDS;
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    SetMenu(NULL);

    // 상단 프로필 이름 생성
    if (!m_profileName.Create(_T("이름"), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(10, 10, 100, 30), this, IDC_STATIC_PROFILE))
    {
        TRACE0("프로필 이름 컨트롤을 생성하지 못했습니다.\n");
        return -1;
    }


    // 로그아웃 버튼 생성
    if (!m_logoutButton.Create(_T("로그아웃"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(280, 10, 370, 30), this, IDC_BUTTON_LOGOUT))
    {
        TRACE0("로그아웃 버튼을 생성하지 못했습니다.\n");
        m_logoutButton.MoveWindow(280, 5, 100, 20);
        return -1;
    }

    // 하단 네비게이션 버튼들 생성
    if (!m_friendsButton.Create(_T("친구"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(0, 500, 125, 550), this, IDC_BUTTON_FRIENDS))
    {
        TRACE0("친구 버튼을 생성하지 못했습니다.\n");
        return -1;
    }
    if (!m_chatRoomsButton.Create(_T("채팅방"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(125, 500, 255, 550), this, IDC_BUTTON_CHATROOMS))
    {
        TRACE0("채팅방 버튼을 생성하지 못했습니다.\n");
        return -1;
    }
    if (!m_settingsButton.Create(_T("설정"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(255, 500, 380, 550), this, IDC_BUTTON_SETTINGS))
    {
        TRACE0("설정 버튼을 생성하지 못했습니다.\n");
        return -1;
    }
    // 채팅방 리스트박스 생성

    if (!m_friendsList.GetSafeHwnd())
    {
        // 친구 리스트 박스 생성
        m_friendsList.Create(
            WS_CHILD | WS_VISIBLE | LBS_STANDARD,
            CRect(10, 80, 380, 400), this, IDC_LIST_FRIENDS);

        // 데이터베이스와 연결 준비
        MYSQL Conn;
        MYSQL* ConnPtr = NULL;
        MYSQL_RES* Result;
        MYSQL_ROW Row;

        mysql_init(&Conn);
        ConnPtr = mysql_real_connect(&Conn, MY_IP, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);

        if (ConnPtr == NULL) {
            MessageBox(_T("데이터베이스 연결 실패"));
        }

        // 데이터베이스의 set을 UTF-8로 변환
        mysql_query(ConnPtr, "set session character_set_connection=utf8;");
        mysql_query(ConnPtr, "set session character_set_results=utf8;");
        mysql_query(ConnPtr, "set session character_set_client=utf8;");

        // user table로부터 user_name 값 들고 오는 Query문
        char* Query = "SELECT user_name FROM user";

        // 쿼리 문이 실패 시
        if (mysql_query(ConnPtr, Query)) {
            TRACE("쿼리 실행 실패: %s\n", mysql_error(ConnPtr));
            mysql_close(ConnPtr); // 연결 해제
        }

        // 결과를 저장
        Result = mysql_store_result(ConnPtr);
        if (Result == NULL) {
            TRACE(_T("쿼리 결과가 없습니다.\n"));
            mysql_close(ConnPtr); // 연결 해제
        }

        // 가져온 데이터를 Row에 데이터를 저장
        while ((Row = mysql_fetch_row(Result)) != NULL) {
            if (Row[0] != NULL) { // Row[0]이 NULL인지 확인
                CString dbName(CA2T(Row[0], CP_UTF8)); // 데이터베이스에서 이름 가져오기
                m_friendsList.AddString(dbName); // 리스트박스에 이름 추가
            }
        }

        mysql_free_result(Result); // 결과 해제
        mysql_close(ConnPtr); // 연결 해제
    }

    SetWindowPos(NULL, 100, 100, 400, 600, SWP_NOZORDER | SWP_NOACTIVATE);

    return 0;
}

// PreCreateWindow 함수 정의
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CFrameWnd::PreCreateWindow(cs))
        return FALSE;

    return TRUE;
}

// 각 버튼 클릭 이벤트 핸들러
void CMainFrame::OnLogoutClicked()
{
    AfxMessageBox(_T("로그아웃 되었습니다."));
    DestroyWindow();
}

void CMainFrame::OnFriendsClicked()
{
    m_currentScreen = SCREEN_FRIENDS;
    RemoveViews();

    // 친구 리스트가 없으면 생성
    if (!m_friendsList.GetSafeHwnd())
    {
        // 친구 리스트 박스 생성
        m_friendsList.Create(
            WS_CHILD | WS_VISIBLE | LBS_STANDARD,
            CRect(10, 80, 380, 400), this, IDC_LIST_FRIENDS);

        // DB 연결 준비
        MYSQL Conn;
        MYSQL* ConnPtr = NULL;
        MYSQL_RES* Result;
        MYSQL_ROW Row;

        // DB 연결
        mysql_init(&Conn);
        ConnPtr = mysql_real_connect(&Conn, MY_IP, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);

        if (ConnPtr == NULL) {
            MessageBox(_T("데이터베이스 연결 실패"));
            return;
        }

        // DB set을 UTF-8로 초기화
        mysql_query(ConnPtr, "set session character_set_connection=utf8;");
        mysql_query(ConnPtr, "set session character_set_results=utf8;");
        mysql_query(ConnPtr, "set session character_set_client=utf8;");

        // user tabel에서 user_name을 가져오는 Query문 작성
        char* Query = "SELECT user_name FROM user";

        // Query 실패 시
        if (mysql_query(ConnPtr, Query)) {
            TRACE("쿼리 실행 실패: %s\n", mysql_error(ConnPtr));
            mysql_close(ConnPtr); // 연결 해제
            return;
        }

        // 결과를 저장
        Result = mysql_store_result(ConnPtr);
        if (Result == NULL) {
            TRACE(_T("쿼리 결과가 없습니다.\n"));
            mysql_close(ConnPtr); // 연결 해제
            return;
        }

        // 가져온 데이터를 행별로 ListBox에 저장
        while ((Row = mysql_fetch_row(Result)) != NULL) {
            if (Row[0] != NULL) { // Row[0]이 NULL인지 확인
                CString dbName(CA2T(Row[0], CP_UTF8)); // 데이터베이스에서 이름 가져오기
                m_friendsList.AddString(dbName); // 리스트박스에 이름 추가
            }
        }

        mysql_free_result(Result); // 결과 해제
        mysql_close(ConnPtr); // 연결 해제
    }

    Invalidate();  // 화면 갱신
}

void CMainFrame::OnChatRoomsClicked()
{
    m_currentScreen = SCREEN_CHATROOMS;  // 채팅방 화면으로 설정
    RemoveViews();

    // 채팅방 리스트 박스가 이미 생성되지 않았다면 생성
    if (!m_chatRoomList.GetSafeHwnd())
    {
        m_chatRoomList.Create(
            WS_CHILD | WS_VISIBLE | LBS_STANDARD,
            CRect(10, 80, 380, 400), // 채팅방 화면 내 위치 및 크기
            this,
            IDC_LIST_CHATROOM);
    }
    else
    {
        m_chatRoomList.ResetContent(); // 기존 내용을 초기화
    }

    // MySQL 연결 준비
    MYSQL Conn;
    MYSQL* ConnPtr = mysql_init(&Conn);
    if (!ConnPtr)
    {
        MessageBox(_T("MySQL 초기화 실패."), _T("Error"), MB_ICONERROR);
        return;
    }

    // DB 연결
    ConnPtr = mysql_real_connect(&Conn, MY_IP, DB_USER, DB_PASS, DB_NAME, 3306, NULL, 0);
    if (!ConnPtr)
    {
        CString errorMsg = CString(mysql_error(&Conn));
        MessageBox(_T("MySQL 연결 실패: ") + errorMsg, _T("Error"), MB_ICONERROR);
        return;
    }

    // 테이블 목록 가져오기
    // MySQL 쿼리 실행
    if (mysql_query(ConnPtr, "SHOW TABLES") == 0) {
        MYSQL_RES* res = mysql_store_result(ConnPtr);
        if (res) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != NULL) {
                // UTF-8 데이터를 CString으로 변환
                CString tableName(CA2T(row[0], CP_UTF8)); // UTF-8 → CString 변환

                // 'user' 테이블 제외 조건 추가
                if (tableName.CompareNoCase(_T("user")) != 0 && tableName.CompareNoCase(_T("admin")) != 0) {
                    m_chatRoomList.AddString(tableName); // 리스트 박스에 추가
                }
            }
            mysql_free_result(res);
        }
    }
    else {
        // UTF-8 에러 메시지 처리
        CString errorMsg(CA2T(mysql_error(ConnPtr), CP_UTF8));
        MessageBox(_T("쿼리 실행 실패: ") + errorMsg, _T("Error"), MB_ICONERROR);
    }


    // MySQL 연결 닫기
    mysql_close(ConnPtr);

    // 리스트 박스 보이기
    m_chatRoomList.ShowWindow(SW_SHOW);

    Invalidate();  // 화면 갱신
}


// 화면 전환(설정 화면)
void CMainFrame::OnSettingsClicked()
{
    m_currentScreen = SCREEN_SETTINGS;
    RemoveViews();
    Invalidate();

}
// 회원정보 변경 버튼 핸들러 함수
void CMainFrame::OnEditProfileClicked() {
    CChangeDlg ChangePW;
    // 비밀번호 확인 모달 창
    if (ChangePW.DoModal() == IDOK) {

    }

}
// 회원탈퇴 버튼 핸들러 함수
void CMainFrame::OnDeleteAccountClicked() {
    CCheckDlg CheckPW;
    // 비밀번호 확인 모달 창
    if (CheckPW.DoModal() == IDOK) {
        DestroyWindow(); // 회원탈퇴 시 프로그램 종료
    }

}

// OnPaint 함수에서 화면 상태에 맞는 내용을 그립니다.
void CMainFrame::OnPaint()
{
    CPaintDC dc(this); // 그리기 장치를 위한 DC

    dc.MoveTo(0, 30);
    dc.LineTo(400, 30);

    dc.MoveTo(0, 500);
    dc.LineTo(400, 500);
    dc.TextOutW(110, 10, m_strUserName);
    switch (m_currentScreen)
    {
    case SCREEN_FRIENDS:
        //m_chatRoomList.Create(WS_CHILD | WS_VISIBLE | LBS_STANDARD, CRect(10, 80, 380, 400), this, IDC_STATIC);
        break;

    case SCREEN_CHATROOMS:
        //m_chatRoomList.Create(WS_CHILD | WS_VISIBLE | LBS_STANDARD, CRect(10, 80, 380, 400), this, IDC_STATIC);

        // "채팅방 만들기" 버튼 생성
        if (!m_createChatRoomButton.GetSafeHwnd())
        {
            m_createChatRoomButton.Create(
                _T("채팅방 만들기"),
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                CRect(10, 40, 190, 70), // 반으로 크기를 줄임
                this,
                IDC_BUTTON_CREATE_CHATROOM);
        }

        // "채팅방 참여하기" 버튼 생성
        if (!m_joinChatRoomButton.GetSafeHwnd())
        {
            m_joinChatRoomButton.Create(
                _T("채팅방 참여하기"),
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                CRect(200, 40, 380, 70), // 나머지 반 크기
                this,
                IDC_BUTTON_JOIN_CHATROOM); // 새로운 버튼 ID
        }
        break;

    case SCREEN_SETTINGS:

        // 회원정보 변경 버튼 생성
        if (!m_editProfileButton.GetSafeHwnd())
        {
            m_editProfileButton.Create(_T("회원정보 변경"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(100, 200, 260, 250), this, IDC_BUTTON_EDIT);
        }

        if (m_logoutButton.GetSafeHwnd())
        {
            m_logoutButton.MoveWindow(100, 360, 160, 50);
        }
        // 회원탈퇴 버튼 생성
        if (!m_deleteAccountButton.GetSafeHwnd())
        {
            m_deleteAccountButton.Create(_T("회원탈퇴"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(100, 280, 260, 330), this, IDC_BUTTON_WITHDRAW);
        }
        break;

    default:
        break;
    }
}



// 화면 초기화
void CMainFrame::RemoveViews()
{
    if (m_currentScreen != SCREEN_SETTINGS) {
        m_logoutButton.MoveWindow(280, 5, 100, 20);
    }
    if (m_currentScreen != SCREEN_FRIENDS) {
        m_friendsList.DestroyWindow(); // 친구리스트 제거
    }
    if (m_currentScreen != SCREEN_CHATROOMS) {
        m_chatRoomList.DestroyWindow(); // 친구리스트 제거
    }
    // TODO: 여기에 구현 코드 추가.
    if (m_editProfileButton.GetSafeHwnd())
    {
        m_editProfileButton.DestroyWindow(); // 첫 번째 버튼 제거
    }
    if (m_deleteAccountButton.GetSafeHwnd())
    {
        m_deleteAccountButton.DestroyWindow(); // 두 번째 버튼 제거
    }
    if (m_joinChatRoomButton.GetSafeHwnd())
    {
        m_joinChatRoomButton.DestroyWindow(); // 화면 초기화
    }
    if (m_createChatRoomButton.GetSafeHwnd())
    {
        m_createChatRoomButton.DestroyWindow(); // 화면 초기화
    }
}


// 초기화면 실행
void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CFrameWnd::OnShowWindow(bShow, nStatus);

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.

    MYSQL Conn;
    MYSQL* ConnPtr = NULL;

    int Stat;



    if (bShow)  // 창이 처음 보일 때만 실행
    {
        static bool isDialogShown = false;
        if (!isDialogShown)
        {
            isDialogShown = true;
            CLogInDlg login;
            if (login.DoModal() == IDOK && m_bLogIn)
            {
                SetUserInfo(login.m_strId, login.m_strPw, login.m_strName);

            }
        }
    }
}


// 프로그램 크기 고정
void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
    CFrameWnd::OnSize(nType, cx, cy);
    if (cx != 400 || cy != 600)  // 설정한 고정 크기
    {
        SetWindowPos(NULL, 0, 0, 400, 600, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


// 회원정보 저장
void CMainFrame::SetUserInfo(CString Id, CString Pw, CString Name)
{
    m_strUserId = Id;
    m_strUserPw = Pw;
    m_strUserName = Name;
    // TODO: 여기에 구현 코드 추가.
}
// 채팅방 생성
void CMainFrame::OnCreateChatRoomClicked()
{
    CChatRoomNameDlg dlg;
    if (dlg.DoModal() == IDOK)
    {
        if (!dlg.m_chatRoomName.IsEmpty())
        {
            // 'user' 이름 제한 추가
            if (dlg.m_chatRoomName.CompareNoCase(_T("user")) == 0 || dlg.m_chatRoomName.CompareNoCase(_T("admin")) == 0)
            {
                AfxMessageBox(_T("해당 이름으로는 채팅방을 생성할 수 없습니다."));
                return;
            }

            // MySQL 연결
            MYSQL Conn;
            mysql_init(&Conn);
            MYSQL* ConnPtr = mysql_real_connect(&Conn, MY_IP, DB_USER, DB_PASS, DB_NAME, 3306, NULL, 0);

            if (!ConnPtr)
            {
                AfxMessageBox(_T("MySQL 연결 실패"));
                return;
            }

            // 동일한 채팅방 이름이 이미 있는지 확인하는 쿼리
            CString checkQuery;
            checkQuery.Format(_T("SHOW TABLES LIKE '%s'"), dlg.m_chatRoomName);

            CW2A utfQuery(checkQuery, CP_UTF8);
            char* queryChar = utfQuery;

            // 쿼리 실패 시
            if (mysql_query(ConnPtr, queryChar))
            {
                CString errorMsg = CString(mysql_error(ConnPtr));
                MessageBox(_T("쿼리 실행 실패: ") + errorMsg, _T("Error"), MB_ICONERROR);
                mysql_close(ConnPtr);
                return;
            }

            MYSQL_RES* res = mysql_store_result(ConnPtr);
            if (res && mysql_num_rows(res) > 0) // 동일 이름의 테이블이 존재
            {
                AfxMessageBox(_T("이미 생성되어 있는 채팅방 이름입니다."));
                mysql_free_result(res);
                mysql_close(ConnPtr);
                return;
            }
            mysql_free_result(res);

            // 채팅방 이름을 리스트 박스에 추가
            if (!m_chatRoomList.GetSafeHwnd())
            {
                // 리스트 박스 생성 (한 번만 생성)
                m_chatRoomList.Create(
                    WS_CHILD | WS_VISIBLE | LBS_STANDARD,
                    CRect(10, 80, 380, 400), // 채팅방 화면 내 위치 및 크기
                    this,
                    IDC_LIST_CHATROOM);
            }
            m_chatRoomList.AddString(dlg.m_chatRoomName);  // 채팅방 이름 추가

            // 채팅방 테이블 생성 쿼리
            CString query;
            query.Format(
                _T("CREATE TABLE `%s` (name VARCHAR(100) NOT NULL, chat_time DATETIME DEFAULT CURRENT_TIMESTAMP, message TEXT NOT NULL)"),
                dlg.m_chatRoomName);

            CW2A utfQuery2(query, CP_UTF8);
            char* queryChar2 = utfQuery2;
            // 쿼리 실패 시
            if (mysql_query(ConnPtr, queryChar2))
            {
                MessageBox(_T("쿼리 실행 중 오류가 발생했습니다."), _T("Error"), MB_ICONERROR);

            }
            else
            {
                AfxMessageBox(_T("채팅방이 성공적으로 생성되었습니다."));
            }

            // MySQL 연결 닫기
            mysql_close(ConnPtr);
        }
        else
        {
            AfxMessageBox(_T("채팅방 이름을 입력해주세요."));
        }
    }
}


// 채팅방 참여 버튼 핸들러 함수
void CMainFrame::OnJoinChatRoomClicked()
{
    CChatRoomNameDlg d_join;
    CChatRoom dlg;

    if (d_join.DoModal() == IDOK)
    {
        if (!d_join.m_chatRoomName.IsEmpty())
        {
            // 'user' 이름 제한 추가
            if (d_join.m_chatRoomName.CompareNoCase(_T("user")) == 0 || d_join.m_chatRoomName.CompareNoCase(_T("admin")) == 0)
            {
                AfxMessageBox(_T("해당 채팅방은 참여할 수 없습니다."));
                return;
            }

            MYSQL Conn;
            MYSQL* ConnPtr = mysql_init(&Conn);

            if (!ConnPtr)
            {
                MessageBox(_T("MySQL 초기화 실패"), _T("Error"), MB_ICONERROR);
                return;
            }

            ConnPtr = mysql_real_connect(&Conn, MY_IP, DB_USER, DB_PASS, DB_NAME, 3306, NULL, 0);
            if (!ConnPtr)
            {
                CString errorMsg = CString(mysql_error(&Conn));
                MessageBox(_T("MySQL 연결 실패: ") + errorMsg, _T("Error"), MB_ICONERROR);
                return;
            }

            // 채팅방이 존재하는지 확인
            CString query;
            query.Format(_T("SHOW TABLES LIKE '%s'"), d_join.m_chatRoomName);

            CW2A utfQuery(query, CP_UTF8);
            char* queryChar = utfQuery;

            if (mysql_query(ConnPtr, queryChar) == 0)
            {
                MYSQL_RES* res = mysql_store_result(ConnPtr);

                if (res)
                {
                    if (mysql_num_rows(res) > 0)
                    {
                        // 채팅방이 존재하는 경우
                        CString message;
                        message.Format(_T("채팅방 '%s'에 참여합니다."), d_join.m_chatRoomName);
                        AfxMessageBox(message);

                        // 채팅방 이름 저장 및 다이얼로그 실행
                        chatname = d_join.m_chatRoomName;
                        dlg.DoModal();
                    }
                    else
                    {
                        // 채팅방이 존재하지 않는 경우
                        AfxMessageBox(_T("없는 채팅방입니다. 다시 선택해주세요."));
                    }

                    mysql_free_result(res);
                }
            }
            else
            {
                CString errorMsg = CString(mysql_error(ConnPtr));
                MessageBox(_T("쿼리 실행 실패: ") + errorMsg, _T("Error"), MB_ICONERROR);
            }

            mysql_close(ConnPtr);
        }
        else
        {
            AfxMessageBox(_T("채팅방 이름을 입력해주세요."));
        }
    }
}


