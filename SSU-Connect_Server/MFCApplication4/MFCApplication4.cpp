// MFCApplication4.cpp: 애플리케이션에 대한 클래스 동작을 정의합니다.

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "MFCApplication4.h"
#include "MainFrm.h"
#include "MFCApplication4Doc.h"
#include "MFCApplication4View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCApplication4App

BEGIN_MESSAGE_MAP(CMFCApplication4App, CWinApp)
    ON_COMMAND(ID_APP_ABOUT, &CMFCApplication4App::OnAppAbout)
    ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
    ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

// CMFCApplication4App 생성

CMFCApplication4App::CMFCApplication4App() noexcept
{
    // 다시 시작 관리자 지원
    m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;

#ifdef _MANAGED
    System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

    SetAppID(_T("MFCApplication4.AppID.NoVersion"));
}

// 유일한 CMFCApplication4App 개체입니다.

CMFCApplication4App theApp;

// CMFCApplication4App 초기화

BOOL CMFCApplication4App::InitInstance()
{
    // Windows XP에서 비주얼 스타일을 사용하려면 InitCommonControlsEx가 필요합니다.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    // OLE 라이브러리를 초기화합니다.
    if (!AfxOleInit())
    {
        AfxMessageBox(IDP_OLE_INIT_FAILED);
        return FALSE;
    }

    AfxEnableControlContainer();
    EnableTaskbarInteraction(FALSE);

    // 표준 초기화
    SetRegistryKey(_T("로컬 애플리케이션 마법사에서 생성된 애플리케이션"));
    LoadStdProfileSettings(4);

    // 주 프레임 창을 만듭니다.
    CMainFrame* pMainFrame = new CMainFrame();
    if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
    {
        delete pMainFrame;
        return FALSE;
    }
    m_pMainWnd = pMainFrame;

    // 주 창을 초기화하고 표시합니다.
    pMainFrame->ShowWindow(m_nCmdShow);
    pMainFrame->UpdateWindow();

    return TRUE;
}

int CMFCApplication4App::ExitInstance()
{
    AfxOleTerm(FALSE);
    return CWinApp::ExitInstance();
}

// CMFCApplication4App 메시지 처리기

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg() noexcept;

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    DECLARE_MESSAGE_MAP()
public:
//    virtual BOOL OnInitDialog();
    virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CMFCApplication4App::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}


BOOL CAboutDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
