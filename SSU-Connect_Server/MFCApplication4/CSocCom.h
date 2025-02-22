#pragma once

// CSocCom 명령 대상
// 통신용 소켓
#define UM_RECEIVE WM_USER+2
#define MAX_CLIENT_COUNT 10

#define SOC_CLIENT_CONNECT _T("접속성공")
#define SOC_CLIENT_DISCONNECT _T("클라이언트 종료")

class CSocCom : public CSocket
{
public:
	CSocCom();
	virtual ~CSocCom();

	HWND m_hWnd;
	void CSocCom::Init(HWND hwnd);
	virtual void OnReceive(int nErrorCode);

	int m_index;
};