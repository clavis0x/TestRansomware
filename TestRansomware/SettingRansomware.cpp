// SettingRansomware.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "TestRansomware.h"
#include "SettingRansomware.h"
#include "afxdialogex.h"
#include "TestRansomwareDlg.h"

extern CTestRansomwareDlg *g_pParent;


// CSettingRansomware ��ȭ �����Դϴ�.

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


// CSettingRansomware �޽��� ó�����Դϴ�.


BOOL CSettingRansomware::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
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
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CSettingRansomware::OnBnClickedButtonConfirm()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	EndDialog(IDCANCEL);
}


void CSettingRansomware::OnBnClickedButtonAddext()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

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
