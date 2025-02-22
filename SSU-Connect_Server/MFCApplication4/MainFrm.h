// MainFrm.h: CMainFrame 클래스의 인터페이스

#pragma once


class CMainFrame : public CFrameWnd
{
protected:
   
    DECLARE_DYNCREATE(CMainFrame)

public:
    CMainFrame() noexcept;
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
    // 툴바와 상태바
    CToolBar    m_wndToolBar;
    CStatusBar  m_wndStatusBar;

    // 화면 상태를 나타내는 열거형과 현재 화면 상태 변수
    enum ScreenState { SCREEN_NONE, SCREEN_CHATROOMS, SCREEN_FRIENDS, SCREEN_SETTINGS, SCREEN_EDIT_PROFILE };
    ScreenState m_currentScreen;

    // 설정 화면 및 네비게이션 화면에 필요한 버튼과 컨트롤
    CStatic m_profileName;          // 프로필 이름 표시용
    CButton m_logoutButton;         // 로그아웃 버튼
    CButton m_friendsButton;        // 친구 목록 버튼
    CButton m_chatRoomsButton;      // 채팅방 버튼
    CButton m_settingsButton;       // 설정 버튼
    CButton m_editProfileButton;    // 회원정보 변경 버튼
    CButton m_deleteAccountButton;  // 회원탈퇴 버튼
    CButton m_createChatRoomButton;
    CListBox m_chatRoomList;
    CButton m_joinChatRoomButton; // "채팅방 참여하기" 버튼
    CListBox m_friendsList;

public:
    virtual ~CMainFrame();

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnPaint();
    afx_msg void OnLogoutClicked();
    afx_msg void OnFriendsClicked();
    afx_msg void OnChatRoomsClicked();
    afx_msg void OnSettingsClicked();
    afx_msg void OnEditProfileClicked();
    afx_msg void OnDeleteAccountClicked();
    afx_msg void OnCreateChatRoomClicked();
    afx_msg void OnJoinChatRoomClicked(); // "채팅방 참여하기" 클릭 이벤트
    

    // 설정 화면을 그리는 함수
    void ShowSettingsScreen(CDC* pDC);

    DECLARE_MESSAGE_MAP()
private:
    void RemoveViews();
public:
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    bool m_bLogIn;
    afx_msg void OnSize(UINT nType, int cx, int cy);
    void SetUserInfo(CString Id, CString Pw, CString Name);
    CString m_strUserId;
    CString m_strUserPw;
    CString m_strUserName;
    CString chatname;
};
