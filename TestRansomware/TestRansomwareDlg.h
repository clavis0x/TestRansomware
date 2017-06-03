
// TestRansomwareDlg.h : 헤더 파일
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "SettingRansomware.h"

typedef struct sFindFileInfo {
	int type;
	CString strPath;
	HANDLE param;
}  FIND_FILE_INFO;

typedef struct sFileLog {
	CString timeStamp;
	CString strPath;
}  FILE_LOG;


// CTestRansomwareDlg 대화 상자
class CTestRansomwareDlg : public CDialogEx
{
// 생성입니다.
public:
	CTestRansomwareDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTRANSOMWARE_DIALOG };
#endif

	int m_num;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// TestRansomwareDlg
public:
	CString appPath; // PATH

	bool m_isRunningFindFiles;
	CString m_strFilter;
	int m_numTotal;
	int m_numInfected;
	vector<FILE_LOG> m_listFileLog; // packet list

	list<CString> m_listFileExt; // 파일 확장자
	list<CString> m_listInfectedFiles; // 감염된 파일

	CRITICAL_SECTION m_cs;

	// Settings
	int m_cryptKey;
	int m_cryptType;
	bool m_bBypassDecoy;


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CSettingRansomware m_pSettingRansomwareDlg; // 기본 설정 Dlg
	CListCtrl ctr_listLog;
	CEdit ctr_editFilter;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	void AddLogList(CString msg, bool wTime = false);
	bool FindFiles(LPCTSTR szFilePath);
	void UpdateLogList();
	afx_msg void OnLvnGetdispinfoList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CStatic ctr_staticStatus;
	CStatic ctr_staticStatus2;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CEdit ctr_editPath;
	bool AddCheckFileExtension(CString StrExt);
	bool CheckFileExtension(CString strPath);
	bool AddInfectedFile(CString StrPath);
	bool CreateListFile();
	bool EncryptFileRs(CString strPath);
	bool DecryptFileRs();
};
