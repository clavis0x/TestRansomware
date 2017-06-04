#pragma once
#include "afxwin.h"


// CSettingRansomware 대화 상자입니다.

class CSettingRansomware : public CDialog
{
	DECLARE_DYNAMIC(CSettingRansomware)

public:
	CSettingRansomware(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSettingRansomware();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETTINGRANSOMWARE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CButton ctr_checkBypassDecoy;
	CEdit ctr_editCryptKey;
	CComboBox ctr_comboCryptType;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonConfirm();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonAddext();
	CListBox ctr_listExt;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CEdit ctr_editExt;
	CEdit ctr_editEncryptionInterval;
};
