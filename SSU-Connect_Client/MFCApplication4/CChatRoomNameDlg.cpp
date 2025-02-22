// CChatRoomNameDlg.cpp: 구현 파일
//

#include "pch.h"
#include "MFCApplication4.h"
#include "afxdialogex.h"
#include "CChatRoomNameDlg.h"


// CChatRoomNameDlg 대화 상자

IMPLEMENT_DYNAMIC(CChatRoomNameDlg, CDialogEx)

CChatRoomNameDlg::CChatRoomNameDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_DIALOG_CHATROOM_NAME, pParent)
{
}

CChatRoomNameDlg::~CChatRoomNameDlg()
{
}
void CChatRoomNameDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_CHATROOM_NAME, m_chatRoomName);
}

BEGIN_MESSAGE_MAP(CChatRoomNameDlg, CDialogEx)
END_MESSAGE_MAP()