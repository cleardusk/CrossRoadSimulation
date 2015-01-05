// Option.cpp : implementation file
//

#include "stdafx.h"
#include "TRANS_SIMU.h"
#include "Option.h"
#include "afxdialogex.h"
#include "MY_TRANS_SIMU.h"


// COption dialog

IMPLEMENT_DYNAMIC(COption, CDialog)

COption::COption(CWnd* pParent /*=NULL*/)
	: CDialog(COption::IDD, pParent)
	, m_carDis(0)
{

}

COption::~COption()
{
}

void COption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_BAK_COLOR_R, m_bakClrR);
	DDX_Text(pDX, IDC_EDIT_BAK_COLOR_G, m_bakClrG);
	DDX_Text(pDX, IDC_EDIT_BAK_COLOR_B, m_bakClrB);
	DDX_Text(pDX, IDC_EDIT_PER_COLOR_R, m_perClrR);
	DDX_Text(pDX, IDC_EDIT_PER_COLOR_G, m_perClrG);
	DDX_Text(pDX, IDC_EDIT_PER_COLOR_B, m_perClrB);
	DDX_Text(pDX, IDC_EDIT_CAR_CIRCLE, m_carCircle);
	DDX_Text(pDX, IDC_EDIT_CAR_SIZE, m_carSize);
	DDX_Text(pDX, IDC_EDIT_CAR_SPEED, m_carSpeed);
	DDX_Text(pDX, IDC_EDIT_PER_CIRCLE, m_perCircle);
	DDX_Text(pDX, IDC_EDIT_PER_SIZE, m_perSize);
	DDX_Text(pDX, IDC_EDIT_PER_SPEED, m_perSpeed);
	DDX_Text(pDX, IDC_EDIT_ROAD_CENTER_WIDTH, m_rdCenWid);
	DDX_Text(pDX, IDC_EDIT_ROAD_SIDEWALK_WIDTH, m_sidewalkWid);
	DDX_Text(pDX, IDC_EDIT_ROAD_WIDTH, m_rdWid);
	DDX_Text(pDX, IDC_EDIT_ROAD_XLEN, m_rdXlen);
	DDX_Text(pDX, IDC_EDIT_ROAD_YLEN, m_rdYlen);
	DDX_Text(pDX, IDC_EDIT_BUF, m_buf);
	DDX_Text(pDX, IDC_EDIT_CAR_REF, m_carRef);
	DDX_Text(pDX, IDC_EDIT_LIGHT_CIRCLE, m_lightCircle);
	DDX_Text(pDX, IDC_EDIT_LIGHT_SIZE, m_lightSize);
	DDX_Text(pDX, IDC_EDIT_PER_LIGHT_DIS, m_perLightDis);
	DDX_Text(pDX, IDC_EDIT_PER_REF, m_perRef);
	DDX_Text(pDX, IDC_LIGHT_REF, m_lightRef);
	DDX_Text(pDX, IDC_EDIT_CAR_DIS, m_carDis);
}


BEGIN_MESSAGE_MAP(COption, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BAK_COLOR, &COption::OnBnClickedButtonBakColor)
	ON_BN_CLICKED(IDC_BUTTON_PER_COLOR, &COption::OnBnClickedButtonPerColor)
	ON_BN_CLICKED(IDC_RADIO_GENERATE_EQUAL, &COption::OnBnClickedRadioGenerateEqual)
	ON_BN_CLICKED(IDC_RADIO_GENERATE_RANDOM, &COption::OnBnClickedRadioGenerateRandom)
END_MESSAGE_MAP()


// COption message handlers

void COption::OnBnClickedButtonBakColor()
{
	CColorDialog clrDlg;
	if (clrDlg.DoModal() == IDOK){
		COLORREF clr = clrDlg.GetColor();
		m_bakClrR = GetRValue(clr);
		m_bakClrG = GetGValue(clr);
		m_bakClrB = GetBValue(clr);
	}
	UpdateData(FALSE);	// 变量到控件显示 TRUE是用户输入的值到变量
}
void COption::OnBnClickedButtonPerColor()
{
	CColorDialog clrDlg;
	if (clrDlg.DoModal() == IDOK){
		COLORREF clr = clrDlg.GetColor();
		m_perClrR = GetRValue(clr);
		m_perClrG = GetGValue(clr);
		m_perClrB = GetBValue(clr);
	}
	UpdateData(FALSE);
}


void COption::OnBnClickedRadioGenerateEqual()
{
	((CButton*)GetDlgItem(IDC_RADIO_GENERATE_EQUAL))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO_GENERATE_RANDOM))->SetCheck(FALSE);
	m_GenetatrNum = 0;
}


void COption::OnBnClickedRadioGenerateRandom()
{
	((CButton*)GetDlgItem(IDC_RADIO_GENERATE_EQUAL))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO_GENERATE_RANDOM))->SetCheck(TRUE);
	m_GenetatrNum = 1;
}


BOOL COption::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_GenetatrNum == 1){
		((CButton*)GetDlgItem(IDC_RADIO_GENERATE_EQUAL))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_GENERATE_RANDOM))->SetCheck(TRUE);
	}
	else{
		((CButton*)GetDlgItem(IDC_RADIO_GENERATE_EQUAL))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_GENERATE_RANDOM))->SetCheck(FALSE);
	}


	return TRUE;  // return TRUE unless you set the focus to a control
}
