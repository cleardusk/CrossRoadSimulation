#pragma once

#include "resource.h"
#include "MO Commons/map.h"
#include "MY_TRANS_SIMU.h"


class CTRANS_SIMU_VIEW : public CFormView
{
	//  [12/30/2014 guojianzhu]
	/// functions
private:
	BOOL CreateRoad();
	void GetTrackLine();
	// offX offY 为中心点相对原点的偏移，返回已经被创建的对象
	CMoLine CentroSymmetric(CMoLine& l, float offX, float offY);

	void InitTrackingLayerAndData();
	void GenerateLight();
	void SelecetState(int state = 3);
	void GenerateObject();
	void AdjustSymbolSize(double scale = 1.0);
	void Clear();
	/// variables
private:
	BOOL						m_flagOpt;
	BOOL						m_flagSimu;
	unsigned int				m_lyrNum;	
	vector<CMoDataConnection>	m_dataCon;
	vector<CMoTableDesc>		m_desc;
	vector<CMoMapLayer>			m_layer;

	//  [12/16/2014 guojianzhu]
public:
	stack<CMoRectangle> stkPre, stkAft;	// 两个栈，用来控制前进和后退视图

public:
	afx_msg void OnMapTool(UINT nID);
	afx_msg void OnUpdateMapTool(CCmdUI *pCmdUI);
	afx_msg void OnBackMenu();
	afx_msg void OnAfterMenu();

public:
	UINT m_curTool;	// 工具 ID

public:
	CMoPoint pt; 

protected: // create from serialization only
	CTRANS_SIMU_VIEW();
	DECLARE_DYNCREATE(CTRANS_SIMU_VIEW)

public:
	enum{ IDD = IDD_VS2013_MO_DEMO_FORM };

// Attributes
public:
	CTRANS_SIMU_DOC* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CTRANS_SIMU_VIEW();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	CMap1 m_map;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileOpen();
	DECLARE_EVENTSINK_MAP()
	void MouseDownMap1(short Button, short Shift, long X, long Y);
	void AfterTrackingLayerDrawMap1(long hDC);
	afx_msg void OnCrossrdsimuStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCrossrdsimuStop();
	afx_msg void OnDestroy();
	afx_msg void OnCrossrdsimuOption();
};

#ifndef _DEBUG  // debug version in VS2013_MO_DEMOView.cpp
inline CTRANS_SIMU_DOC* CTRANS_SIMU_VIEW::GetDocument() const
   { return reinterpret_cast<CTRANS_SIMU_DOC*>(m_pDocument); }
#endif

