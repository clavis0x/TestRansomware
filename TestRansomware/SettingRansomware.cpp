// SettingRansomware.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "TestRansomware.h"
#include "SettingRansomware.h"
#include "afxdialogex.h"
#include "TestRansomwareDlg.h"

extern CTestRansomwareDlg *g_pParent;


// CSettingRansomware 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSettingRansomware, CDialog)

CSettingRansomware::CSettingRansomware(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_SETTINGRANSOMWARE, pParent)
{

}

CSettingRansomware::~CSettingRansomware()
{
}

void CSettingRansomware::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_BYPASSDECOY, ctr_checkBypassDecoy);
	DDX_Control(pDX, IDC_EDIT_CRYPTKEY, ctr_editCryptKey);
	DDX_Control(pDX, IDC_COMBO_CRYPTTYPE, ctr_comboCryptType);
	DDX_Control(pDX, IDC_LIST1, ctr_listExt);
	DDX_Control(pDX, IDC_EDIT_EXT, ctr_editExt);
}


BEGIN_MESSAGE_MAP(CSettingRansomware, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_Confirm, &CSettingRansomware::OnBnClickedButtonConfirm)
	ON_BN_CLICKED(IDC_BUTTON_Cancel, &CSettingRansomware::OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_AddExt, &CSettingRansomware::OnBnClickedButtonAddext)
END_MESSAGE_MAP()


// CSettingRansomware 메시지 처리기입니다.


BOOL CSettingRansomware::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CString strTemp;

	ctr_comboCryptType.AddString("Replace only");
	ctr_comboCryptType.AddString("Create & Delete");
	ctr_comboCryptType.AddString("Create & Replace & Delete");
	ctr_comboCryptType.SetCurSel(0);

	list<CString>::iterator itor = g_pParent->m_listFileExt.begin();
	while (itor != g_pParent->m_listFileExt.end())
	{
		ctr_listExt.AddString(*itor);
		itor++;
	}

	// Key
	strTemp.Format("%X", g_pParent->m_cryptKey);
	ctr_editCryptKey.SetWindowTextA(strTemp);

	// Type
	ctr_comboCryptType.SetCurSel(g_pParent->m_cryptType);

	// Bypass-Decoy
	ctr_checkBypassDecoy.SetCheck((int)g_pParent->m_bBypassDecoy);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CSettingRansomware::OnBnClickedButtonConfirm()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strTemp;

	// Ext
	g_pParent->m_listFileExt.clear();
	for (int i = 0; i < ctr_listExt.GetCount(); i++) {
		ctr_listExt.GetText(i, strTemp);
		g_pParent->AddCheckFileExtension(strTemp);
	}

	// Key
	ctr_editCryptKey.GetWindowTextA(strTemp);
	g_pParent->m_cryptKey = (int)strtol(strTemp.GetBufferSetLength(2), NULL, 16);

	// Type
	g_pParent->m_cryptType = ctr_comboCryptType.GetCurSel();

	// Bypass-Decoy
	g_pParent->m_bBypassDecoy = (ctr_checkBypassDecoy.GetCheck() != 0);

	EndDialog(IDOK);
}


void CSettingRansomware::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EndDialog(IDCANCEL);
}


void CSettingRansomware::OnBnClickedButtonAddext()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strTemp;
	CString strTemp2;
	ctr_editExt.GetWindowTextA(strTemp);

	for (int i = 0; i < ctr_listExt.GetCount(); i++) {
		ctr_listExt.GetText(i, strTemp2);
		if (strTemp.Compare(strTemp2) == 0)
			return;
	}

	ctr_listExt.AddString(strTemp);
}


BOOL CSettingRansomware::PreTranslateMessage(MSG* pMsg)
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

		// Delete
		if (pMsg->wParam == VK_DELETE) {
			if(pMsg->hwnd == ctr_listExt.m_hWnd){
				int nIndex = ctr_listExt.GetCurSel();
				if(nIndex != LB_ERR){
					ctr_listExt.DeleteString(nIndex);
				}
			}
			return true;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
