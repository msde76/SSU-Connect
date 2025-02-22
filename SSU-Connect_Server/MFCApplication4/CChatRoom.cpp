// CChatRoom.cpp: 구현 파일
//

#include "pch.h"
#include "MFCApplication4.h"
#include "afxdialogex.h"
#include "CChatRoom.h"
#include "MainFrm.h"


// CChatRoom 대화 상자

IMPLEMENT_DYNAMIC(CChatRoom, CDialogEx)

CChatRoom::CChatRoom(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_DIALOG_CHATROOM, pParent)
    , m_strSend(_T(""))
{
    m_rCount = 0;
}

CChatRoom::~CChatRoom()
{
}

void CChatRoom::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_SEND, m_sendbutton);
    DDX_Control(pDX, IDC_EDIT_SEND, m_edit_send);
    DDX_Control(pDX, IDC_LIST_CHATING, m_List_chating);
    DDX_Text(pDX, IDC_EDIT_SEND, m_strSend);
}



BEGIN_MESSAGE_MAP(CChatRoom, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_SEND, &CChatRoom::OnBnClickedButtonSend)
    ON_MESSAGE(UM_ACCEPT, &CChatRoom::OnAccept)
    ON_MESSAGE(UM_RECEIVE, OnReceive)
END_MESSAGE_MAP()


// CChatRoom 메시지 처리기

LPARAM CChatRoom::OnAccept(WPARAM wParam, LPARAM lParam) {
    try {
        // 통신을 보낸 클라이언트의 번호를 가져옴
        int tmp = m_socServer.m_index.front();

        CString number;
        number.Format(_T("%d"), tmp);

        // 통신을 보낸 클라이언트의 소켓 생성
        m_socCom[tmp] = new CSocCom();
        m_socCom[tmp] = m_socServer.GetAcceptSocCom();

        m_socServer.m_index.pop_front();
        m_using.push_back(tmp);

        // 클라이언트 소켓과 서버 소켓 연결
        m_socCom[tmp]->m_index = tmp;
        m_socCom[tmp]->Init(this->m_hWnd);

        UpdateData(FALSE);
        return TRUE;
    }
    catch (CException* ex) {
        ex->ReportError();
        return FALSE;
    }
}



LPARAM CChatRoom::OnReceive(WPARAM wParam, LPARAM lParam) {
    // MainFrame에서 값 받아오기
    CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
    CString chatnameDlg = pMainFrame->chatname;
    // 소켓에서 값을 읽어오기 위한 버퍼
    char pTmp[256];
    // 버퍼 초기화
    memset(pTmp, '\0', sizeof(pTmp));
    CString strTmp;
    // 데이터를 소켓으로부터 수신
    m_socCom[wParam]->Receive(pTmp, sizeof(pTmp));

    // 데이터를 소켓으로부터 수신 후 UTF-8 형식으로 전환 후 CSrring 형식으로 전환(한글 깨짐 방지)
    strTmp = CString(CA2T(pTmp, CP_UTF8));

    //파이프 구분
    int firstPipe = strTmp.Find(_T("|")); // 첫 번째 '|' 위치
    int secondPipe = strTmp.Find(_T("|"), firstPipe + 1); // 두 번째 '|' 위치
    int thirdPipe = strTmp.Find(_T("|"), secondPipe + 1); // 세 번째 '|' 위치
    int fourthPipe = strTmp.Find(_T("|"), thirdPipe + 1); // 네 번째 '|' 위치

    // 파이프를 기준으로 값 구분
    CString firstValue = strTmp.Left(firstPipe).Trim();
    CString secondValue = strTmp.Mid(firstPipe + 1, secondPipe - firstPipe - 1).Trim();
    CString thirdValue = strTmp.Mid(secondPipe + 1, thirdPipe - secondPipe - 1).Trim();
    CString fourthValue = strTmp.Mid(thirdPipe + 1, fourthPipe - thirdPipe - 1).Trim();
    CString fifthValue = strTmp.Mid(fourthPipe + 1).Trim();

    // switch문에 사용하기 위해 int형으로 형 변환
    int switchValue = _ttoi(firstValue); // CString -> 정수 변환
    
    //switchValue값에 따른 case 구분
    switch (switchValue)
    {
    case 0: //채팅 한 줄 수신
        case0(secondValue, thirdValue, fourthValue);
        break;
    case 1: //전체 채팅 내용 수신
        case1(secondValue, wParam);
        break;
    }

    return TRUE;

}




BOOL CChatRoom::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
        m_socServer.m_index.push_back(i);
    }
    CString ipAddress = _T("10.14.15.197"); // 원하는 IP 주소로 변경
    UINT port = 5000; // 원하는 포트 번호로 설정

    m_socServer.Create(port);
    m_socServer.Bind(port, ipAddress);
    m_socServer.Listen();
    m_socServer.Init(this->m_hWnd);
    
    // MianFarme으로 부터 값을 가져오기
    CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
    CString chatnameDlg = pMainFrame->chatname;
    
    //DB에서 채팅방table에 대해 전체 내용 SELECT하는 쿼리 작성
    CString query;
    query.Format(_T("SELECT * FROM `%s`"), chatnameDlg);

    CW2A utfQuery(query, CP_UTF8);
    char* queryChar = utfQuery;

    // MySQL 서버 연결
    MYSQL Conn;
    mysql_init(&Conn);

    MYSQL* ConnPtr = mysql_real_connect(&Conn, MY_IP, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);

    if (ConnPtr == NULL) {
        MessageBox(_T("MySQL 연결 실패"));
        return FALSE;  // 연결 실패 시 FALSE 반환
    }

    // SLEECT 쿼리 실행
    int Stat = mysql_query(ConnPtr, queryChar);

    if (Stat != 0) {
        MessageBox(_T("쿼리 실행 오류"));
        mysql_close(ConnPtr); // 연결 닫기
        return FALSE;  // 쿼리 실패 시 FALSE 반환
    }

    // 결과를 저장
    MYSQL_RES* Result = mysql_store_result(ConnPtr);

    if (Result != NULL) {
        // 결과의 필드 수 가져오기
        int num_fields = mysql_num_fields(Result);
        MYSQL_ROW Row;

        // 결과 행을 반복하여 가져오기
        while ((Row = mysql_fetch_row(Result))) {
            CString rowData;  // 한 행을 저장할 변수
            for (int i = 0; i < num_fields; i++) {
                if (Row[i] != NULL) {
                    // UTF-8 문자열로 변환
                    CString colData = CString(CA2T(Row[i], CP_UTF8));
                    rowData += colData;  // 행 데이터를 결합
                    if (i < num_fields - 1) {
                        rowData += _T(" | ");  // 열 사이에 구분자 추가
                    }
                }
                else {
                    rowData += _T("NULL");
                    if (i < num_fields - 1) {
                        rowData += _T(" | ");
                    }
                }
            }
            // 한 행의 데이터를 리스트에 추가
            m_List_chating.AddString(rowData);
        }

        Invalidate();
        // 결과 해제
        mysql_free_result(Result);
    }
    else {
        MessageBox(_T("결과가 없습니다."));
        mysql_close(ConnPtr); // 연결 닫기
        return FALSE;  // 결과가 없을 경우 FALSE 반환
    }

    // 연결 닫기
    mysql_close(ConnPtr);

    return TRUE;  // 성공 시 TRUE 반환

}



void CChatRoom::OnBnClickedButtonSend()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    // MianFarme으로 부터 값을 가져오기
    CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
    CString username = pMainFrame->m_strUserName;
    CString chatnameDlg = pMainFrame->chatname;
    CString text;
    GetDlgItemText(IDC_EDIT_SEND, text);
    char pTmp[256]; //소켓으로 값을 전송하기 위한 버퍼
    UpdateData(TRUE);
    //수신한 내용을 DB에 INSERT
    CString Query;
    Query.Format(
        _T("INSERT INTO `%s` (name, message) VALUES ('%s', '%s')"), //INSERT 쿼리 작성
        chatnameDlg, username, text);
    CW2A utfQuery(Query, CP_UTF8);
    char* queryChar = utfQuery;

    
    // MySQL 서버 연결
    MYSQL Conn;
    mysql_init(&Conn);

    MYSQL* ConnPtr = mysql_real_connect(&Conn, MY_IP, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);

    if (ConnPtr == NULL) {
        MessageBox(_T("MySQL 연결 실패"));
        return;  // 연결 실패 시 FALSE 반환
    }

    // SQL 쿼리 실행
    int Stat = mysql_query(ConnPtr, queryChar);

    if (Stat != 0) {
        MessageBox(_T("쿼리 실행 오류"));
        mysql_close(ConnPtr); // 연결 닫기
        return;  // 쿼리 실패 시 FALSE 반환
    }

    //SELECT 쿼리 
    Query.Format(_T("SELECT * FROM `%s` ORDER BY chat_time DESC LIMIT 1"), chatnameDlg);
    CW2A utfQuery2(Query, CP_UTF8);
    char* queryChar2 = utfQuery2;

    if (ConnPtr == NULL) {
        MessageBox(_T("MySQL 연결 실패"));
        return;  // 연결 실패 시 FALSE 반환
    }

    // SQL 쿼리 실행
    Stat = mysql_query(ConnPtr, queryChar2);

    if (Stat != 0) {
        MessageBox(_T("쿼리 실행 오류"));
        mysql_close(ConnPtr); // 연결 닫기
        return;  // 쿼리 실패 시 FALSE 반환
    }

    // 결과를 저장
    MYSQL_RES* Result = mysql_store_result(ConnPtr);

    if (Result != NULL) {
        // 결과의 필드 수 가져오기
        int num_fields = mysql_num_fields(Result);
        MYSQL_ROW Row;

        // 결과 행을 반복하여 가져오기
        while ((Row = mysql_fetch_row(Result))) {
            CString rowData;  // 한 행을 저장할 변수
            for (int i = 0; i < num_fields; i++) {
                if (Row[i] != NULL) {
                    // UTF-8 문자열로 변환
                    CString colData = CString(CA2T(Row[i], CP_UTF8));
                    rowData += colData;  // 행 데이터를 결합
                    if (i < num_fields - 1) {
                        rowData += _T(" | ");  // 열 사이에 구분자 추가
                    }
                }
                else {
                    rowData += _T("NULL");
                    if (i < num_fields - 1) {
                        rowData += _T(" | ");
                    }
                }
            }
            // 한 행의 데이터를 리스트에 추가
            CW2A utfrowData(rowData, CP_UTF8);
            int i = m_List_chating.GetCount();
            m_List_chating.InsertString(i, rowData);
            strncpy_s(pTmp, sizeof(pTmp), utfrowData, _TRUNCATE);

            // 리스트에 추가한 데이터를 클라이언트에 전송하기 위한 포맷 작성
            CString text;
            text.Format(_T("0|%s|%s"), rowData, chatnameDlg);
            CW2A utftext(text, CP_UTF8);
            strncpy_s(pTmp, sizeof(pTmp), utftext, _TRUNCATE);


            //클라이언트 전체에 전송
            for each (int j in m_using) {
                m_socCom[j]->Send(pTmp, sizeof(pTmp));
            }
        }
        m_strSend.Empty(); //edit에 남아있는 문자열 제거
        Invalidate();
        // 결과 해제
        mysql_free_result(Result);
    }
    else {
        MessageBox(_T("결과가 없습니다."));
        mysql_close(ConnPtr); // 연결 닫기
        return;  // 결과가 없을 경우 FALSE 반환
    }
    UpdateData(FALSE);
    // 연결 닫기
    mysql_close(ConnPtr);
}






void CChatRoom::case0(CString secondValue, CString thirdValue, CString fourthValue)
{
    // TODO: 여기에 구현 코드 추가
     // 변수에 값 저장
    CString chatname = secondValue;  // 첫 번째 값
    CString name = thirdValue;   // 두 번째 값
    CString strTmp = fourthValue; // 세 번째 값

    //소켓으로 값을 전송하기 위한 버퍼
    char pTmp[256];
    // 버퍼 초기화
    memset(pTmp, '\0', sizeof(pTmp));

    // MySQL 서버 연결
    MYSQL Conn;
    mysql_init(&Conn);
    MYSQL* ConnPtr = mysql_real_connect(&Conn, MY_IP, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);

    if (ConnPtr == NULL) {
        MessageBox(_T("MySQL 연결 실패"));
        return;  // 연결 실패 시 FALSE 반환
    }

    //수신한 내용을 DB에 INSERT
    CString Query;
    Query.Format(
        _T("INSERT INTO `%s` (name, message) VALUES ('%s', '%s')"),
        chatname, name, strTmp);
    CW2A utfQuery(Query, CP_UTF8);
    char* queryChar = utfQuery;
    int Stat = mysql_query(ConnPtr, queryChar);

    if (Stat != 0) {
        MessageBox(_T("쿼리 실행 오류"));
        mysql_close(ConnPtr); // 연결 닫기
        return;  // 쿼리 실패 시 FALSE 반환
    }

    //SELECT 쿼리 
    Query.Format(_T("SELECT * FROM `%s` ORDER BY chat_time DESC LIMIT 1"), chatname);
    CW2A utfQuery2(Query, CP_UTF8);
    char* queryChar2 = utfQuery2;
    Stat = mysql_query(ConnPtr, queryChar2);

    if (Stat != 0) {
        MessageBox(_T("쿼리 실행 오류"));
        mysql_close(ConnPtr); // 연결 닫기
        return;  // 쿼리 실패 시 FALSE 반환
    }

    // 결과를 저장
    MYSQL_RES* Result = mysql_store_result(ConnPtr);

    if (Result != NULL) {
        // 결과의 필드 수 가져오기
        int num_fields = mysql_num_fields(Result);
        MYSQL_ROW Row;

        // 결과 행을 반복하여 가져오기
        while ((Row = mysql_fetch_row(Result))) {
            CString rowData;  // 한 행을 저장할 변수
            for (int i = 0; i < num_fields; i++) {
                if (Row[i] != NULL) {
                    // UTF-8 문자열로 변환
                    CString colData = CString(CA2T(Row[i], CP_UTF8));
                    rowData += colData;  // 행 데이터를 결합
                    if (i < num_fields - 1) {
                        rowData += _T(" | ");  // 열 사이에 구분자 추가
                    }
                }
                else {
                    rowData += _T("NULL");
                    if (i < num_fields - 1) {
                        rowData += _T(" | ");
                    }
                }
            }
            // 한 행의 데이터를 리스트에 추가
            CW2A utfrowData(rowData, CP_UTF8);
            int i = m_List_chating.GetCount();
            m_List_chating.InsertString(i, rowData);

            // 리스트에 추가한 데이터를 클라이언트에 전송하기 위한 포맷 작성
            CString text;
            text.Format(_T("0|%s|%s"), rowData, chatname);
            CW2A utftext(text, CP_UTF8);
            strncpy_s(pTmp, sizeof(pTmp), utftext, _TRUNCATE);


            //클라이언트 전체에 전송
            for each (int j in m_using) {
                m_socCom[j]->Send(pTmp, sizeof(pTmp));
            }
        }
        Invalidate();
        // 결과 해제
        mysql_free_result(Result);
    }
    else {
        MessageBox(_T("결과가 없습니다."));
        mysql_close(ConnPtr); // 연결 닫기
        return;  // 결과가 없을 경우 FALSE 반환
    }
    // 연결 닫기
    mysql_close(ConnPtr);
}


void CChatRoom::case1(CString secondValue, WPARAM wParam)
{
    // TODO: 여기에 구현 코드 추가.
    char pTmp[256];//소켓으로 값을 전송하기 위한 버퍼
    
    // 버퍼 초기화
    memset(pTmp, '\0', sizeof(pTmp));
    
    // MySQL 서버 연결
    MYSQL Conn;
    mysql_init(&Conn);
    MYSQL* ConnPtr = mysql_real_connect(&Conn, MY_IP, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);

    if (ConnPtr == NULL) {
        MessageBox(_T("MySQL 연결 실패"));
        return;  // 연결 실패 시 FALSE 반환
    }


    // SELECT 쿼리 작성 및 실행

    CString Query;
    Query.Format(_T("SELECT * FROM `%s`"), secondValue);
    CW2A utfQuery(Query, CP_UTF8);
    char* queryChar = utfQuery;
    int Stat = mysql_query(ConnPtr, queryChar);

    if (Stat != 0) {
        MessageBox(_T("쿼리 실행 오류"));
        mysql_close(ConnPtr); // 연결 닫기
        return;  // 쿼리 실패 시 FALSE 반환
    }

    // 결과를 저장
    MYSQL_RES* Result = mysql_store_result(ConnPtr);

    if (Result != NULL) {
        // 결과의 필드 수 가져오기
        int num_fields = mysql_num_fields(Result);
        MYSQL_ROW Row;

        // 결과 행을 반복하여 가져오기
        while ((Row = mysql_fetch_row(Result))) {
            CString rowData;  // 한 행을 저장할 변수
            rowData += _T("1|");
            for (int i = 0; i < num_fields; i++) {
                if (Row[i] != NULL) {
                    // UTF-8 문자열로 변환
                    CString colData = CString(CA2T(Row[i], CP_UTF8));
                    rowData += colData;  // 행 데이터를 결합
                    if (i < num_fields - 1) {
                        rowData += _T(" | ");  // 열 사이에 구분자 추가
                    }
                }
                else {
                    rowData += _T("NULL");
                    if (i < num_fields - 1) {
                        rowData += _T(" | ");
                    }
                }
            }
            //전송 요청받은 클라이언트 id값 받아오기
            CString Id;
            Id.Format(_T("%d"), wParam);
            
            //전송할 내용 포맷
            CString edit;
            edit.Format(_T("%s|nulldata"), rowData);
            CW2A utfrowData(edit, CP_UTF8);
            strncpy_s(pTmp, sizeof(pTmp), utfrowData, _TRUNCATE);


            //해당 클라이언트에 전송
            for each (int j in m_using) {
                if (j == _ttoi(Id))
                    m_socCom[j]->Send(pTmp, sizeof(pTmp));
            }
        }

        Invalidate();
        // 결과 해제
        mysql_free_result(Result);
    }
    else {
        MessageBox(_T("결과가 없습니다."));
        mysql_close(ConnPtr); // 연결 닫기
        return;  // 결과가 없을 경우 FALSE 반환
    }

    // 연결 닫기
    mysql_close(ConnPtr);

}
