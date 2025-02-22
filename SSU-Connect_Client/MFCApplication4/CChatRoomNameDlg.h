#pragma once
#include "afxdialogex.h"



class CChatRoomNameDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CChatRoomNameDlg)

public:
    CChatRoomNameDlg(CWnd* pParent = nullptr); // 표준 생성자
    virtual ~CChatRoomNameDlg();

    // 대화 상자 데이터
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_CHATROOM_NAME };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV 지원

    DECLARE_MESSAGE_MAP()

public:
    CString m_chatRoomName; // 채팅방 이름
};