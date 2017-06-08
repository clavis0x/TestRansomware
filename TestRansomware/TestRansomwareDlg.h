
// TestRansomwareDlg.h : ��� ����
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "SettingRansomware.h"

#define FILE_BUF_SIZE 4096

typedef struct sFindFileInfo {
	int type;
	CString strPath;
	HANDLE param;
}  FIND_FILE_INFO;

typedef struct sFileLog {
	CString timeStamp;
	CString strPath;
}  FILE_LOG;


// CTestRansomwareDlg ��ȭ ����
class CTestRansomwareDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CTestRansomwareDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTRANSOMWARE_DIALOG };
#endif

	int m_num;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

// TestRansomwareDlg
public:
	CString appPath; // PATH

	bool m_isRunningFindFiles;
	CString m_strFilter;
	int m_numTotal;
	int m_numInfected;
	vector<FILE_LOG> m_listFileLog; // packet list
	bool m_isEncryptReady;

	list<CString> m_listFileExt; // ���� Ȯ����
	queue<CString> m_queueTargetFiles; // ���� ��� ����
	list<CString> m_listInfectedFiles; // ������ ����

	CRITICAL_SECTION m_csFileLog;
	CRITICAL_SECTION m_csFileQueue;

	// Settings
	int m_cryptKey;
	int m_cryptOffset;
	int m_cryptType;
	int m_cryptInterval; // ms
	int m_nDummyByte;
	bool m_bBypassDecoy;
	bool m_bSaltXOR;

	unsigned char m_nSaltXOR;


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CSettingRansomware m_pSettingRansomwareDlg; // �⺻ ���� Dlg
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
	afx_msg void OnDestroy();
};
