#pragma once


// COption dialog

class COption : public CDialog
{
	DECLARE_DYNAMIC(COption)

public:
	COption(CWnd* pParent = NULL);   // standard constructor
	virtual ~COption();

// Dialog Data
	enum { IDD = IDD_DIALOG_OPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	UINT m_bakClrR;
	UINT m_bakClrG;
	UINT m_bakClrB;
	UINT m_perClrR;
	UINT m_perClrG;
	UINT m_perClrB;
	UINT m_carCircle;
	int m_carSize;
	double m_carSpeed;
	UINT m_perCircle;
	int m_perSize;
	double m_perSpeed;
	float m_rdCenWid;
	float m_sidewalkWid;
	float m_rdWid;
	float m_rdXlen;
	float m_rdYlen;
	afx_msg void OnBnClickedButtonBakColor();
	afx_msg void OnBnClickedButtonPerColor();
	afx_msg void OnBnClickedRadioGenerateEqual();
	afx_msg void OnBnClickedRadioGenerateRandom();
	virtual BOOL OnInitDialog();
	int m_GenetatrNum;
	float m_buf;
	UINT m_carRef;
	UINT m_lightCircle;
	int m_lightSize;
	float m_perLightDis;
	UINT m_perRef;
	UINT m_lightRef;
	float m_carDis;
};
