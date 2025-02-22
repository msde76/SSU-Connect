#pragma once

// CSocCom 명령 대상
// 통신용 소켓
#define UM_RECEIVE WM_USER+2

class CSocCom : public CSocket
{
public:
	CSocCom();
	virtual ~CSocCom();

	HWND m_hWnd; // 메인 윈도우 핸들
	void CSocCom::Init(HWND hwnd); // 소켓 클래스와 메인 윈도우를 연결
	virtual void OnReceive(int nErrorCode); // 데이터가 도착했다는 것을 알림

	int m_index; // 채팅 클라이언트 번호
};