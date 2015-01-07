	m_Sta.totalPer = 0;

// VS2013_MO_DEMOView.cpp : implementation of the CVS2013_MO_DEMOView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "TRANS_SIMU.h"
#endif

#include "TRANS_SIMU_DOC.h"
#include "TRANS_SIMU_VIEW.h"
#include "Option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ADJUST_SYMBOL_TEST
#define RANDOM_GENERATE
#define ANG_TEST
#define BUMP_TEST


IMPLEMENT_DYNCREATE(CTRANS_SIMU_VIEW, CFormView)

BEGIN_MESSAGE_MAP(CTRANS_SIMU_VIEW, CFormView)
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_OPEN, &CTRANS_SIMU_VIEW::OnFileOpen)
	ON_COMMAND_RANGE(ID_MAP_FULLEXTENT, ID_MAP_ZOOMOUT, 
	&CTRANS_SIMU_VIEW::OnMapTool)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MAP_FULLEXTENT, ID_MAP_ZOOMOUT, 
	&CTRANS_SIMU_VIEW::OnUpdateMapTool)

	ON_COMMAND(ID_MENU_BACK, &CTRANS_SIMU_VIEW::OnBackMenu)
	ON_COMMAND(ID_MENU_AFTER, &CTRANS_SIMU_VIEW::OnAfterMenu)
	ON_COMMAND(ID_CROSSRDSIMU_START, &CTRANS_SIMU_VIEW::OnCrossrdsimuStart)
	ON_WM_TIMER()
	ON_COMMAND(ID_CROSSRDSIMU_STOP, &CTRANS_SIMU_VIEW::OnCrossrdsimuStop)
	ON_WM_DESTROY()
	ON_COMMAND(ID_CROSSRDSIMU_OPTION, &CTRANS_SIMU_VIEW::OnCrossrdsimuOption)
END_MESSAGE_MAP()

// CVS2013_MO_DEMOView construction/destruction
void CTRANS_SIMU_VIEW::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAP1, m_map);
}

BOOL CTRANS_SIMU_VIEW::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return CFormView::PreCreateWindow(cs);
}

void CTRANS_SIMU_VIEW::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	//  [12/12/2014 guojianzhu]
	// Map�ؼ����Բ���
	//m_map.SetAppearance(0); // 1-mo3D 0-moFlat
	m_map.SetBackColor(RGB(240,255,233));	// ����ʵ������RGB
	srand(unsigned(time(0)));
}


// CVS2013_MO_DEMOView diagnostics

#ifdef _DEBUG
void CTRANS_SIMU_VIEW::AssertValid() const
{
	CFormView::AssertValid();
}

void CTRANS_SIMU_VIEW::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CTRANS_SIMU_DOC* CTRANS_SIMU_VIEW::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTRANS_SIMU_DOC)));
	return (CTRANS_SIMU_DOC*)m_pDocument;
}
#endif //_DEBUG


// CVS2013_MO_DEMOView message handlers


void CTRANS_SIMU_VIEW::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (m_map.m_hWnd)
		m_map.SetWindowPos(0, 0, 0, cx, cy, SWP_NOZORDER);
}

// �˺����ѵ��Թ�
void CTRANS_SIMU_VIEW::OnFileOpen()
{
	// TODO: Add your command handler code here
	CString filter(TEXT("ʸ����ͼ�ļ� (*.shp)|*.shp|դ���ͼ�ļ�(*.jpg)|*.jpg|"));
	CFileDialog dlg(TRUE, TEXT(".shp|*.jpg"), 0, OFN_ALLOWMULTISELECT |
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, filter);
	if (dlg.DoModal() == IDOK)
	{
		POSITION pos1 = dlg.GetStartPosition();//��ʼ�������ж�ѡ���ļ�·��
		while (pos1)//��Ӷ��ͼ��
		{
			CString pathName = dlg.GetNextPathName(pos1);
			if ((!pathName.Right(3).Compare("shp")) || (!pathName.Right(3).Compare("SHP")))
				AddLayer(m_map, pathName);
			else  AddImageLayer(m_map, pathName);
		}
		m_map.Refresh();

		// ��Ų�����Ϣ��ջ
		stkPre.push(m_map.GetExtent());
	}
}
BEGIN_EVENTSINK_MAP(CTRANS_SIMU_VIEW, CFormView)
	ON_EVENT(CTRANS_SIMU_VIEW, IDC_MAP1, DISPID_MOUSEDOWN, 
	CTRANS_SIMU_VIEW::MouseDownMap1, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
END_EVENTSINK_MAP()

// ���� PAN ZOOMIN ZOOMOUT ����Ϣ
void CTRANS_SIMU_VIEW::MouseDownMap1(short Button, short Shift, long X, long Y)
{
	switch (m_curTool)
	{
	case ID_MAP_ZOOMIN:
	{
						  CMoRectangle rect(m_map.TrackRectangle());
						  if (LPDISPATCH(rect) && rect.GetHeight() != 0 && 
							  rect.GetWidth() != 0)
						  {
							  m_map.SetExtent(rect);

#ifdef ADJUST_SYMBOL_TEST
							  double tmp = m_map.GetFullExtent().GetHeight() /
								  m_map.GetExtent().GetHeight();
							  AdjustSymbolSize(tmp);
#endif
						  }
						  else
						  {
							  CMoPoint m_CenterPoint; // ����Ŵ�
							  m_CenterPoint = m_map.ToMapPoint(float(X), float(Y));
							  CMoRectangle rect_Extent(m_map.GetExtent());
							  rect_Extent.ScaleRectangle(0.5);
							  m_map.SetExtent(rect_Extent);
							  m_map.CenterAt(m_CenterPoint.GetX(), m_CenterPoint.GetY());

#ifdef ADJUST_SYMBOL_TEST
							  double tmp = m_map.GetFullExtent().GetHeight() /
								  m_map.GetExtent().GetHeight();
							  AdjustSymbolSize(tmp);
#endif
						  }

						  // ��Ų�����Ϣ��ջ
						  stkPre.push(m_map.GetExtent());

						  // �������ͼ������
						  while (!stkAft.empty())	
							  stkAft.pop();
						  break;
	}
	case ID_MAP_ZOOMOUT:
	{
						   CMoRectangle rect(m_map.GetExtent());
						   rect.ScaleRectangle(1.5);
						   m_map.SetExtent(rect);
#ifdef ADJUST_SYMBOL_TEST
						   double tmp = m_map.GetFullExtent().GetHeight() /
							   m_map.GetExtent().GetHeight();
						   AdjustSymbolSize(tmp);
#endif
						   // ��Ų�����Ϣ��ջ
						   stkPre.push(m_map.GetExtent());

						   // �������ͼ������
						   while (!stkAft.empty())
							   stkAft.pop();
						   break;
	}
	case ID_MAP_PAN:
	{

					   m_map.Pan();

					   // ��Ų�����Ϣ��ջ
					   stkPre.push(m_map.GetExtent());

					   // �������ͼ������
					   while (!stkAft.empty())
						   stkAft.pop();
					   break;
	}
	default:
		break;
	}
}

// [12/16/2014 guojianzhu]
void CTRANS_SIMU_VIEW::OnMapTool(UINT nID)
{
	// TODO: �ڴ���������������
	m_curTool = nID;
	if (m_curTool == ID_MAP_FULLEXTENT)//ȫ����ʾΪ����ʽ���ߣ�����ֱ��ʵ��
	{
		stkPre.push(m_map.GetExtent());
		m_map.SetExtent(m_map.GetFullExtent());

		while (!stkAft.empty())	// �������ͼ������
			stkAft.pop();
		
#ifdef ADJUST_SYMBOL_TEST
		AdjustSymbolSize(1.0);
#endif
	}
}

void CTRANS_SIMU_VIEW::OnUpdateMapTool(CCmdUI *pCmdUI)
{
	if ((m_curTool != ID_MAP_FULLEXTENT))//�����͹��߲��ı��临ѡ״̬
		pCmdUI->SetCheck(pCmdUI->m_nID == m_curTool);
}

void CTRANS_SIMU_VIEW::OnBackMenu()
{
	if (stkPre.size() > 1){
		stkAft.push(stkPre.top());
		stkPre.pop();
		m_map.SetExtent(stkPre.top());

		double tmp = m_map.GetFullExtent().GetHeight() /
			m_map.GetExtent().GetHeight();
		AdjustSymbolSize(tmp);
		m_map.Refresh();
	}
}

void CTRANS_SIMU_VIEW::OnAfterMenu()
{
	if (stkAft.size() > 0){
		stkPre.push(stkAft.top());
		m_map.SetExtent(stkAft.top());
		stkAft.pop();

		double tmp = m_map.GetFullExtent().GetHeight() /
			m_map.GetExtent().GetHeight();
		AdjustSymbolSize(tmp);
		m_map.Refresh();
	}
}

//////////////////////////////////////////////////////////////////////////
CTRANS_SIMU_VIEW::CTRANS_SIMU_VIEW()
: CFormView(CTRANS_SIMU_VIEW::IDD)
{
	m_flagOpt = FALSE;
	m_flagSimu = FALSE;
	m_curTool = ID_MAP_ZOOMIN;

	// ͼ����Ŀ
	m_lyrNum = LAYER_NUM;

	// ��ʼ��
	// ��·
	m_OptSet.rd_wid = 3;
	m_OptSet.rd_xlen = 200;
	m_OptSet.rd_ylen = 100;
	m_OptSet.rd_sidwk_wid = 5;
	m_OptSet.rd_cen_wid = 0.7f;

	m_OptSet.rd_lnm = m_OptSet.rd_rnm = 3;
	m_OptSet.rd_arc = m_OptSet.rd_sidwk_wid;

	// ��
	m_OptSet.car_speed = 60;
	m_OptSet.car_size = 25;
	
	// ����
	m_OptSet.per_speed = 5;
	m_OptSet.per_size = 3;

	// Ͷ������
	m_OptSet.car_elapse_circle = 100;
	m_OptSet.per_elapse_circle = 200;

	// Ͷ��ģʽ
	m_OptSet.generate_num = 1; // 0 ����Ͷ�� 1 ���Ͷ��

	// ��ɫ����
	m_OptSet.bak_clr = RGB(50, 50, 50);
	m_OptSet.per_clr = RGB(255, 255, 0);

	// ��������
	m_OptSet.light_circle = 10000;
	m_OptSet.car_ref = 30;
	m_OptSet.per_ref = 60;
	m_OptSet.light_ref = 200;
	m_OptSet.buf = 5;
	m_OptSet.car_dis = 5;
	m_OptSet.per_light_dis = 3;
	m_OptSet.light_size = 3; // ��ʱ�������ô���
	//////////////////////////////////////////////////////////////////////////
	m_Sta.totalPer = 0;
	m_Sta.totalCar = 0;
	m_Sta.totalEvtNum = 0;
	m_Sta.time_left = m_OptSet.light_circle;
	m_Sta.time_run = 0;
};

CTRANS_SIMU_VIEW::~CTRANS_SIMU_VIEW()
{
	for (unsigned i = 0; i < m_dataCon.size(); ++i){
		m_dataCon.at(i).Disconnect();
		m_dataCon.at(i).ReleaseDispatch();
	}
	for (unsigned i = 0; i < m_desc.size(); ++i){
		m_desc.at(i).ReleaseDispatch();
	}
	for (unsigned i = 0; i < m_layer.size(); ++i){
		m_layer.at(i).ReleaseDispatch();
	}
}

BOOL CTRANS_SIMU_VIEW::CreateRoad()
{
	// �ӵײ㴴����� MO ����
	for (unsigned int i = 0; i < m_lyrNum; ++i){
		CMoDataConnection tmpCon;
		tmpCon.CreateDispatch(TEXT("MapObjects2.DataConnection"));
		m_dataCon.push_back(tmpCon);

		CMoTableDesc tmpTl;
		tmpTl.CreateDispatch("MapObjects2.TableDesc");
		m_desc.push_back(tmpTl);
		
		CMoMapLayer tmpLyr;
		tmpLyr.CreateDispatch("MapObjects2.MapLayer");
		m_layer.push_back(tmpLyr);
	}

	// ��������
	VARIANT vt;
	vt.vt = VT_BOOL;
	vt.boolVal = VARIANT_FALSE;
	CreateDirectory("Data", NULL);
	for (unsigned int i = 0; i < m_lyrNum; ++i){
		m_dataCon[i].SetDatabase((LPCTSTR)"Data"); //����ͼ�㱣��Ŀ¼
		if (!m_dataCon[i].Connect())
			return FALSE;
	}

	// ����ÿһ��������ļ���
	CMoGeoDataset geoDataset[LAYER_NUM];
	geoDataset[0] = m_dataCon[0].AddGeoDataset("������_˫����", moShapeTypeLine,
		(LPDISPATCH)m_desc[0], vt, vt);
	geoDataset[1] = m_dataCon[1].AddGeoDataset("��·", moShapeTypeLine,
		(LPDISPATCH)m_desc[1], vt, vt);
	geoDataset[2] = m_dataCon[2].AddGeoDataset("���е�", moPolygon,
		(LPDISPATCH)m_desc[2], vt, vt);
	geoDataset[3] = m_dataCon[3].AddGeoDataset("��Χ", moPolygon,
		(LPDISPATCH)m_desc[3], vt, vt);

	// ����ͼ������Լ�����
	/// ��һ��
	CMoLine   lnCntre[4];			// 4 ����
	CMoPoints pts[4], pts2[2];	// 2 �����(��һ��ڶ���ĵ�)
	CMoPoint  pt[8];			// 8 ������
	for (int i = 0; i < 4; ++i)
		lnCntre[i].CreateDispatch("MapObjects2.Line");
	for (int i = 0; i < 4; ++i)
		pts[i].CreateDispatch("MapObjects2.Points");
	for (int i = 0; i < 2; ++i)
		pts2[i].CreateDispatch("MapObjects2.Points");
	for (int i = 0; i < 8; ++i)
		pt[i].CreateDispatch("MapObjects2.Point");

	// �ߵ������趨��ͨ�� Parts Points �������ã�
	float wid = m_OptSet.rd_cen_wid;	// ˫���߿��
	float tmp = m_OptSet.rd_lnm *m_OptSet.rd_wid + m_OptSet.rd_sidwk_wid;
	pt[0].SetX(0);
	pt[0].SetY(m_OptSet.rd_ylen / 2);
	pt[1].SetX(m_OptSet.rd_xlen / 2 - tmp);
	pt[1].SetY(m_OptSet.rd_ylen / 2);
	pt[2].SetX(m_OptSet.rd_xlen / 2 + tmp);
	pt[2].SetY(m_OptSet.rd_ylen / 2);
	pt[3].SetX(m_OptSet.rd_xlen);
	pt[3].SetY(m_OptSet.rd_ylen / 2);

	pt[4].SetX(m_OptSet.rd_xlen / 2);
	pt[4].SetY(0);
	pt[5].SetX(m_OptSet.rd_xlen / 2);
	pt[5].SetY(m_OptSet.rd_ylen / 2 - tmp);
	pt[6].SetX(m_OptSet.rd_xlen / 2);
	pt[6].SetY(m_OptSet.rd_ylen / 2 + tmp);
	pt[7].SetX(m_OptSet.rd_xlen / 2);
	pt[7].SetY(m_OptSet.rd_ylen);

	pts[0].Add(pt[0]);
	pts[0].Add(pt[1]);	// �㴮
	pts[1].Add(pt[2]);
	pts[1].Add(pt[3]);	// �㴮
	pts[2].Add(pt[4]);
	pts[2].Add(pt[5]);	// �㴮
	pts[3].Add(pt[6]);
	pts[3].Add(pt[7]);	// �㴮

	lnCntre[0].GetParts().Add(pts[0]);	
	lnCntre[0].GetParts().Add(pts[1]);	// ��һ���߶�
	lnCntre[1].GetParts().Add(pts[0]);
	lnCntre[1].GetParts().Add(pts[1]);	// �ڶ����߶�
	lnCntre[2].GetParts().Add(pts[2]);
	lnCntre[2].GetParts().Add(pts[3]);	// �������߶�
	lnCntre[3].GetParts().Add(pts[2]);
	lnCntre[3].GetParts().Add(pts[3]);	// �������߶�
	lnCntre[0].Offset(0, wid / 2);
	lnCntre[1].Offset(0, -wid / 2);
	lnCntre[2].Offset(wid / 2, 0);
	lnCntre[3].Offset(-wid / 2, 0);

	/// �ڶ���
	int dashRdNum = 4 * (m_OptSet.rd_lnm + m_OptSet.rd_rnm - 2);
	CMoLine   *lnDash = new CMoLine[dashRdNum];
	for (int i = 0; i < dashRdNum; ++i)
		lnDash[i].CreateDispatch("MapObjects2.Line");

	pt[0].SetX(0);
	pt[0].SetY(m_OptSet.rd_ylen / 2);
	pt[1].SetX(m_OptSet.rd_xlen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid - m_OptSet.rd_sidwk_wid);
	pt[1].SetY(m_OptSet.rd_ylen / 2);
	pt[2].SetX(m_OptSet.rd_xlen / 2);
	pt[2].SetY(0);
	pt[3].SetX(m_OptSet.rd_xlen / 2);
	pt[3].SetY(m_OptSet.rd_ylen / 2 - m_OptSet.rd_rnm*m_OptSet.rd_wid - m_OptSet.rd_sidwk_wid);


	pts2[0].Add(pt[0]);
	pts2[0].Add(pt[1]);	// �㴮
	pts2[1].Add(pt[2]);
	pts2[1].Add(pt[3]);	// �㴮

	for (int i = 0; i < dashRdNum; ++i){
		if ((dashRdNum / 4) % 2 == 1) continue;
		if (i < dashRdNum / 2)
			lnDash[i].GetParts().Add(pts2[0]);
		else
			lnDash[i].GetParts().Add(pts2[1]);
	}
	for (int i = 0; i < dashRdNum / 2; ++i){
		if (i < dashRdNum / 8)
			lnDash[i].Offset(0, (i + 1)*m_OptSet.rd_wid);
		else if (i < dashRdNum / 4)
			lnDash[i].Offset(0, -(i + 1 - dashRdNum / 8)*m_OptSet.rd_wid);
		else if (i < 3 * dashRdNum / 8)
			lnDash[i].Offset(m_OptSet.rd_xlen / 2 + m_OptSet.rd_wid*m_OptSet.rd_rnm + m_OptSet.rd_sidwk_wid,
			(i + 1 - dashRdNum / 4)*m_OptSet.rd_wid);
		else
			lnDash[i].Offset(m_OptSet.rd_xlen / 2 + m_OptSet.rd_wid*m_OptSet.rd_rnm + m_OptSet.rd_sidwk_wid,
			-(i + 1 - 3 * dashRdNum / 8)*m_OptSet.rd_wid);
	}

	for (int i = dashRdNum / 2; i < dashRdNum; ++i){
		if (i < 5 * dashRdNum / 8)
			lnDash[i].Offset((i + 1 - dashRdNum / 2)*m_OptSet.rd_wid, 0);
		else if (i < 6 * dashRdNum / 8)
			lnDash[i].Offset(-(i + 1 - 5 * dashRdNum / 8)*m_OptSet.rd_wid, 0);
		else if (i < 7 * dashRdNum / 8)
			lnDash[i].Offset((i + 1 - 6 * dashRdNum / 8)*m_OptSet.rd_wid,
			m_OptSet.rd_ylen / 2 + m_OptSet.rd_wid*m_OptSet.rd_rnm + m_OptSet.rd_sidwk_wid);
		else
			lnDash[i].Offset(-(i + 1 - 7 * dashRdNum / 8)*m_OptSet.rd_wid,
			m_OptSet.rd_ylen / 2 + m_OptSet.rd_wid*m_OptSet.rd_rnm + m_OptSet.rd_sidwk_wid);
	}

	/// �����㣨���е���
	int walkRdNum = 4;
	CMoPoint ptWalk[4];
	CMoPoints ptsWalk[2];
	CMoPolygon poWalk[4];
	for (int i = 0; i < 4; ++i)
		ptWalk[i].CreateDispatch("MapObjects2.Point");
	for (int i = 0; i < 2; ++i)
		ptsWalk[i].CreateDispatch("MapObjects2.Points");
	for (int i = 0; i < 4; ++i)
		poWalk[i].CreateDispatch("MapObjects2.Polygon");
	ptWalk[0].SetX(m_OptSet.rd_xlen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid - m_OptSet.rd_sidwk_wid);
	ptWalk[0].SetY(m_OptSet.rd_ylen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid);
	ptWalk[1].SetX(m_OptSet.rd_xlen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid);
	ptWalk[1].SetY(m_OptSet.rd_ylen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid);
	ptWalk[2].SetX(m_OptSet.rd_xlen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid);
	ptWalk[2].SetY(m_OptSet.rd_ylen / 2 + m_OptSet.rd_lnm*m_OptSet.rd_wid);
	ptWalk[3].SetX(m_OptSet.rd_xlen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid - m_OptSet.rd_sidwk_wid);
	ptWalk[3].SetY(m_OptSet.rd_ylen / 2 + m_OptSet.rd_lnm*m_OptSet.rd_wid);

	for (int i = 0; i < 4; ++i)
		ptsWalk[0].Add(ptWalk[i]);

	poWalk[0].GetParts().Add(ptsWalk[0]);
	poWalk[1].GetParts().Add(ptsWalk[0]);
	poWalk[1].Offset(m_OptSet.rd_sidwk_wid + (m_OptSet.rd_lnm + m_OptSet.rd_rnm)*m_OptSet.rd_wid, 0);

	ptWalk[0].SetX(m_OptSet.rd_xlen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid);
	ptWalk[0].SetY(m_OptSet.rd_ylen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid - m_OptSet.rd_sidwk_wid);
	ptWalk[1].SetX(m_OptSet.rd_xlen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid + (m_OptSet.rd_lnm + m_OptSet.rd_rnm)*m_OptSet.rd_wid);
	ptWalk[1].SetY(m_OptSet.rd_ylen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid - m_OptSet.rd_sidwk_wid);
	ptWalk[2].SetX(m_OptSet.rd_xlen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid + (m_OptSet.rd_lnm + m_OptSet.rd_rnm)*m_OptSet.rd_wid);
	ptWalk[2].SetY(m_OptSet.rd_ylen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid - m_OptSet.rd_sidwk_wid + m_OptSet.rd_sidwk_wid);
	ptWalk[3].SetX(m_OptSet.rd_xlen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid);
	ptWalk[3].SetY(m_OptSet.rd_ylen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid - m_OptSet.rd_sidwk_wid + m_OptSet.rd_sidwk_wid);
	for (int i = 0; i < 4; ++i)
		ptsWalk[1].Add(ptWalk[i]);
	poWalk[2].GetParts().Add(ptsWalk[1]);
	poWalk[3].GetParts().Add(ptsWalk[1]);
	poWalk[3].Offset(0, m_OptSet.rd_sidwk_wid + (m_OptSet.rd_lnm + m_OptSet.rd_rnm)*m_OptSet.rd_wid);


	/// ���Ĳ�
	CMoPoint ptBord[204];
	CMoPoints ptsBord[4];
	CMoPolygon poBord[4];
	for (int i = 0; i < 204; ++i)
		ptBord[i].CreateDispatch("MapObjects2.Point");
	for (int i = 0; i < 4; ++i)
		ptsBord[i].CreateDispatch("MapObjects2.Points");
	for (int i = 0; i < 4; ++i)
		poBord[i].CreateDispatch("MapObjects2.Polygon");

	// x y ΪԲ��
	float x = m_OptSet.rd_xlen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid - m_OptSet.rd_sidwk_wid * 2;
	float y = m_OptSet.rd_ylen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid - m_OptSet.rd_sidwk_wid * 2;
	ptBord[0].SetX(0);
	ptBord[0].SetY(y + 2 * m_OptSet.rd_sidwk_wid);
	ptBord[1].SetX(0);
	ptBord[1].SetY(y + m_OptSet.rd_sidwk_wid);
	for (int i = 0; i < 100; ++i){
		ptBord[i + 2].SetX(x + m_OptSet.rd_sidwk_wid*sin(i / 100.0*PI / 2.0));
		ptBord[i + 2].SetY(y + m_OptSet.rd_sidwk_wid*cos(i / 100.0*PI / 2.0));
	}
	ptBord[102].SetX(x + m_OptSet.rd_sidwk_wid);
	ptBord[102].SetY(0);
	ptBord[103].SetX(x + 2 * m_OptSet.rd_sidwk_wid);
	ptBord[103].SetY(0);

	x += m_OptSet.rd_sidwk_wid;
	y += m_OptSet.rd_sidwk_wid;
	for (int i = 0; i < 100; ++i){
		ptBord[i + 104].SetX(x + m_OptSet.rd_sidwk_wid*cos(i / 100.0*PI / 2.0));
		ptBord[i + 104].SetY(y + m_OptSet.rd_sidwk_wid*sin(i / 100.0*PI / 2.0));
	}

	for (int i = 0; i < 204; ++i)
		ptsBord[0].Add(ptBord[i]);
	poBord[0].GetParts().Add(ptsBord[0]);

	/// ����Գ�
	for (int i = 0; i < 204; ++i)
		ptBord[i].SetX(m_OptSet.rd_xlen - ptBord[i].GetX());
	for (int i = 0; i < 204; ++i)
		ptsBord[1].Add(ptBord[i]);
	poBord[1].GetParts().Add(ptsBord[1]);

	for (int i = 0; i < 204; ++i)
		ptBord[i].SetY(m_OptSet.rd_ylen - ptBord[i].GetY());
	for (int i = 0; i < 204; ++i)
		ptsBord[2].Add(ptBord[i]);
	poBord[2].GetParts().Add(ptsBord[2]);

	for (int i = 0; i < 204; ++i)
		ptBord[i].SetX(m_OptSet.rd_xlen - ptBord[i].GetX());
	for (int i = 0; i < 204; ++i)
		ptsBord[3].Add(ptBord[i]);
	poBord[3].GetParts().Add(ptsBord[3]);


	// ����¼��д������
	for (unsigned int i = 0; i < m_lyrNum; ++i){
		m_layer[i].SetGeoDataset(geoDataset[i]);
	}
	CMoRecordset recs(m_layer[0].GetRecords());	// ��һͼ��ļ�¼��
	for (int i = 0; i < 4; ++i){
		recs.AddNew();
		SetValue(recs.GetFields(), "Shape", lnCntre[i]);
		recs.Update();
	}
	recs = m_layer[1].GetRecords();	// �ڶ�ͼ��ļ�¼��
	for (int i = 0; i < dashRdNum; ++i){
		recs.AddNew();
		SetValue(recs.GetFields(), "Shape", lnDash[i]);
		recs.Update();
	}
	recs = m_layer[2].GetRecords();	// ����ͼ��ļ�¼��
	for (int i = 0; i < walkRdNum; ++i){
		recs.AddNew();
		SetValue(recs.GetFields(), "Shape", poWalk[i]);
		recs.Update();
	}
	recs = m_layer[3].GetRecords();	// ����ͼ��ļ�¼��
	for (int i = 0; i < 4; ++i){
		recs.AddNew();
		SetValue(recs.GetFields(), "Shape", poBord[i]);
		recs.Update();
	}
	// ���÷��ż�ˢ��
	m_layer[0].GetSymbol().SetSize(1);
	m_layer[0].GetSymbol().SetColor(RGB(255, 251, 134));
	m_layer[1].GetSymbol().SetSize(1);
	m_layer[1].GetSymbol().SetColor(RGB(255, 255, 255));
	m_layer[1].GetSymbol().SetStyle(moDashLine);
	m_layer[2].GetSymbol().SetColor(RGB(255, 255, 255));
	m_layer[2].GetSymbol().SetStyle(moDownwardDiagonalFill);
	m_layer[3].GetSymbol().SetStyle(moGrayFill);

	CMoLayers layers(m_map.GetLayers());
	for (unsigned int i = 0; i < m_lyrNum; ++i)
		layers.Add(m_layer[i]);
	m_map.SetBackColor(m_OptSet.bak_clr);
	m_map.Refresh();
	stkPre.push(m_map.GetExtent());

	return TRUE;
}

void CTRANS_SIMU_VIEW::GetTrackLine()
{
	/*
	//  [12/30/2014 guojianzhu]
	*	Բ��Ĭ�ϱ���Ϊ 100 �ȷֵ㣬�������ã���΢�������´���Ƭ�μ���
	*	--to be updated
	*/
	CMoLine		mo_line[10];
	CMoPoints	mo_pts[10];
	CMoPoint	mo_pt[102];
	for (int i = 0; i < 10; ++i)
		mo_line[i].CreateDispatch("MapObjects2.Line");
	for (int i = 0; i < 10; ++i)
		mo_pts[i].CreateDispatch("MapObjects2.Points");
	for (int i = 0; i < 102; ++i)
		mo_pt[i].CreateDispatch("MapObjects2.Point");

	// x y ΪԲ�� r Ϊ�뾶
	float x = m_OptSet.rd_xlen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid - 
		m_OptSet.rd_sidwk_wid;
	float y = m_OptSet.rd_ylen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid - 
		m_OptSet.rd_sidwk_wid;
	float r = m_OptSet.rd_sidwk_wid + m_OptSet.rd_wid / 2;

	mo_pt[0].SetX(0);
	mo_pt[0].SetY(y + r);
	for (int i = 0; i < 100; ++i){
		mo_pt[i + 1].SetX(x + r*sin(i / 100.0*PI / 2.0));
		mo_pt[i + 1].SetY(y + r*cos(i / 100.0*PI / 2.0));
	}
	mo_pt[101].SetX(x + r);
	mo_pt[101].SetY(0);

	for (int i = 0; i < 102; ++i)
		mo_pts[0].Add(mo_pt[i]);
	mo_line[0].GetParts().Add(mo_pts[0]);		// ��һ���켣

	mo_pt[1].SetX(m_OptSet.rd_xlen);
	mo_pt[1].SetY(y + r);
	for (int i = 0; i < 2; ++i)
		mo_pts[1].Add(mo_pt[i]);
	mo_line[1].GetParts().Add(mo_pts[1]);		// �ڶ����켣

	mo_line[2].GetParts().Add(mo_pts[1]);
	mo_line[2].Offset(0, m_OptSet.rd_wid);		// �������켣

	mo_line[3].GetParts().Add(mo_pts[1]);
	mo_line[3].Offset(0, m_OptSet.rd_wid * 2);	// �������켣

	// x ����
	y = m_OptSet.rd_ylen - y; // x��Գ�
	r += m_OptSet.rd_lnm*m_OptSet.rd_wid;

	mo_pt[0].SetX(0);
	mo_pt[0].SetY(y - r);
	for (int i = 0; i < 100; ++i){
		mo_pt[i + 1].SetX(x + r*sin(i / 100.0*PI / 2.0));
		mo_pt[i + 1].SetY(y - r*cos(i / 100.0*PI / 2.0));
	}
	mo_pt[101].SetX(x + r);
	mo_pt[101].SetY(m_OptSet.rd_ylen);
	for (int i = 0; i < 102; ++i)
		mo_pts[4].Add(mo_pt[i]);
	mo_line[4].GetParts().Add(mo_pts[4]);	// �������켣

	// �洢��������
	for (int i = 0; i < 5; ++i)
		MoTrackLine.push_back(mo_line[i]);

	// ���ĶԳƱ任
	for (int i = 0; i < 5; ++i)
		MoTrackLine.push_back(CentroSymmetric(mo_line[i], m_OptSet.rd_xlen / 2, 
		m_OptSet.rd_ylen / 2));

	//////////////////////////////////////////////////////////////////////////
	// x y ΪԲ�� r Ϊ�뾶
	x = m_OptSet.rd_xlen - x;
	y = m_OptSet.rd_ylen - y;
	r -= m_OptSet.rd_lnm*m_OptSet.rd_wid;

	mo_pt[0].SetX(x - m_OptSet.rd_sidwk_wid - m_OptSet.rd_wid / 2);
	mo_pt[0].SetY(0);
	for (int i = 0; i < 100; ++i){
		mo_pt[i + 1].SetX(x - r*cos(i / 100.0*PI / 2.0));
		mo_pt[i + 1].SetY(y + r*sin(i / 100.0*PI / 2.0));
	}
	mo_pt[101].SetX(m_OptSet.rd_xlen);
	mo_pt[101].SetY(y + r);


	for (int i = 0; i < 102; ++i)
		mo_pts[5].Add(mo_pt[i]);
	mo_line[5].GetParts().Add(mo_pts[5]);		// ��6���켣

	mo_pt[1].SetX(x - m_OptSet.rd_sidwk_wid - m_OptSet.rd_wid / 2);
	mo_pt[1].SetY(m_OptSet.rd_ylen);
	for (int i = 0; i < 2; ++i)
		mo_pts[6].Add(mo_pt[i]);
	mo_line[6].GetParts().Add(mo_pts[6]);		// ��7���켣

	mo_line[7].GetParts().Add(mo_pts[6]);
	mo_line[7].Offset(-m_OptSet.rd_wid, 0);		// ��8���켣

	mo_line[8].GetParts().Add(mo_pts[6]);
	mo_line[8].Offset(-m_OptSet.rd_wid * 2, 0);	// ��9���켣

	// y ����
	x = m_OptSet.rd_xlen - x; // x��Գ�
	r += m_OptSet.rd_lnm*m_OptSet.rd_wid;

	mo_pt[0].SetX(x + r);
	mo_pt[0].SetY(0);
	for (int i = 0; i < 100; ++i){
		mo_pt[i + 1].SetX(x + r*cos(i / 100.0*PI / 2.0));
		mo_pt[i + 1].SetY(y + r*sin(i / 100.0*PI / 2.0));
	}
	mo_pt[101].SetX(0);
	mo_pt[101].SetY(y + r);
	for (int i = 0; i < 102; ++i)
		mo_pts[9].Add(mo_pt[i]);
	mo_line[9].GetParts().Add(mo_pts[9]);	// ��10���켣

	// �洢��������
	for (int i = 5; i < 10; ++i)
		MoTrackLine.push_back(mo_line[i]);

	// ���ĶԳƱ任
	for (int i = 5; i < 10; ++i)
		MoTrackLine.push_back(CentroSymmetric(mo_line[i], m_OptSet.rd_xlen / 2,
		m_OptSet.rd_ylen / 2));
}

CMoLine CTRANS_SIMU_VIEW::CentroSymmetric(CMoLine& l, float offX, float offY)
{
	CMoLine mo_line;
	mo_line.CreateDispatch("MapObjects2.Line");

	CMoParts pats = l.GetParts();
	for (int j = 0; j < pats.GetCount(); ++j){
		CMoPoints pts(pats.Item(COleVariant(long(j))));
		CMoPoints ptsTmp;
		ptsTmp.CreateDispatch("MapObjects2.Points");
		for (int i = 0; i < pts.GetCount(); ++i){
			CMoPoint pt(pts.Item(COleVariant(long(i))));	// pt ���´����Ķ���
			pt.SetX(-pt.GetX() + 2 * offX);
			pt.SetY(-pt.GetY() + 2 * offY);
			ptsTmp.Add(pt);
		}
		mo_line.GetParts().Add(ptsTmp);
	}

	return mo_line;
}

// ��ʼ�����ſ�+���е����ݱ�ʶ
void CTRANS_SIMU_VIEW::InitTrackingLayerAndData()
{
	// ��ȡ MoTrackingLayer��ʹ��ȫ�ֱ����洢
	MoTrackingLayer = m_map.GetTrackingLayer();
	int tmp = CAR_COLOR_NUM + LIGHT_COLOR_NUM + ANGLE_NUM*CAR_COLOR_NUM + 1;
	MoTrackingLayer.SetSymbolCount(tmp);

	for (int i = 0; i < tmp; ++i){
		CMoSymbol sym(MoTrackingLayer.GetSymbol(long(i)));

		// ��
		if (i == tmp - 1){
			sym.SetSymbolType(0);
			sym.SetSize(m_OptSet.per_size);
			sym.SetColor(m_OptSet.per_clr);
			continue;
		}
		// ���̵�����
		if (i >= CAR_COLOR_NUM && i < CAR_COLOR_NUM + LIGHT_COLOR_NUM){
			sym.SetSymbolType(1);  // ����Ĭ��Ϊ 0 ����״��
			sym.SetSize(m_OptSet.light_size);
			sym.SetColor(LightColor[i - CAR_COLOR_NUM]);
		}

		// ����������
		else{
			CMoFont fnt;
			fnt.InitializeFont();			// ���� CreateDispatch
			fnt.SetName(TEXT("MoCars2"));	// ���������

			sym.SetStyle(moTrueTypeMarker);		// TrueType����
			sym.SetFont(fnt.GetFontDispatch());
			sym.SetSize(m_OptSet.car_size);
			sym.SetCharacterIndex(1);			// ������������ֵı��ֵ��ʮ���ƣ�
			fnt.ReleaseFont();
			if (i < CAR_COLOR_NUM){
				sym.SetColor(CarColor[i]);
			}
			else{
				int tmp = i - CAR_COLOR_NUM - LIGHT_COLOR_NUM;
				sym.SetColor(CarColor[tmp / ANGLE_NUM]);
				sym.SetRotation(double(tmp % ANGLE_NUM));
			}
		}
	}

	// �������е���ʶ����
	SIDEWALK[0] = m_OptSet.rd_ylen / 2 - m_OptSet.rd_wid*m_OptSet.rd_lnm;
	SIDEWALK[1] = m_OptSet.rd_ylen / 2 + m_OptSet.rd_wid*m_OptSet.rd_lnm;
	SIDEWALK[2] = m_OptSet.rd_xlen / 2 - m_OptSet.rd_wid*m_OptSet.rd_lnm;
	SIDEWALK[3] = m_OptSet.rd_xlen / 2 + m_OptSet.rd_wid*m_OptSet.rd_lnm;
}

//////////////////////////////////////////////////////////////////////////
TrafficLight::TrafficLight(){}
TrafficLight::~TrafficLight(){}
void TrafficLight::CreateLight2(Pos pt1, Pos pt2, Pos pt3, Pos pt4)
{
	// ���ö�λ��
	CMoLine lne;
	lne.CreateDispatch("MapObjects2.Line");
	CMoPoint pt;
	pt.CreateDispatch("MapObjects2.Point");
	CMoPoints pts[2];
	for (int i = 0; i < 2; ++i)
		pts[i].CreateDispatch("MapObjects2.Points");

	pt.SetX(pt1.x);
	pt.SetY(pt1.y);
	pts[0].Add(pt);
	pt.SetX(pt2.x);
	pt.SetY(pt2.y);
	pts[0].Add(pt);
	pt.SetX(pt3.x);
	pt.SetY(pt3.y);
	pts[1].Add(pt);
	pt.SetX(pt4.x);
	pt.SetY(pt4.y);
	pts[1].Add(pt);

	lne.GetParts().Add(pts[0]);
	lne.GetParts().Add(pts[1]);

	// �¼�
	evnt = MoTrackingLayer.AddEvent(lne, rand() % LIGHT_COLOR_NUM + CAR_COLOR_NUM);
	evntNum = m_Sta.totalEvtNum++;
}
void TrafficLight::CreateLight(Pos pt1, Pos pt2)
{
	// ���ö�λ��
	CMoLine lne;
	lne.CreateDispatch("MapObjects2.Line");
	CMoPoint pt;
	pt.CreateDispatch("MapObjects2.Point");
	CMoPoints pts;
	pts.CreateDispatch("MapObjects2.Points");

	pt.SetX(pt1.x);
	pt.SetY(pt1.y);
	pts.Add(pt);
	pt.SetX(pt2.x);
	pt.SetY(pt2.y);
	pts.Add(pt);

	lne.GetParts().Add(pts);

	// �¼�
	evnt = MoTrackingLayer.AddEvent(lne, rand() % LIGHT_COLOR_NUM + CAR_COLOR_NUM);
	evntNum = m_Sta.totalEvtNum++;
}
void TrafficLight::ChangeLightColor(short id /*= 0*/)
{
	evnt.SetSymbolIndex(id + CAR_COLOR_NUM);
}

//////////////////////////////////////////////////////////////////////////
Car::Car()
{
}
Car::~Car()
{
	pts.ReleaseDispatch();
}
void Car::CreateCar(int lNum /*= 1*/, double dis /*= 1*/)
{
	flagBump = FALSE;
	lneNum = lNum;
	CMoLine lne = MoTrackLine.at(lneNum - 1);

	// �����߻�ȡ�㼯
	pts.CreateDispatch(TEXT("MapObjects2.Points"));
	stepDis = dis;
	divNum = int(lne.GetLength() / stepDis);

	lne.SetMeasuresAsLength();
	for (int i = 0; i < divNum; ++i)
		pts.Add(lne.ReturnPointEvents(stepDis*i).Item(COleVariant(long(0))));

	// ��ʼ�����λ��
	ptsInd = 0;
	CMoPoint startPos;
	startPos.CreateDispatch("MapObjects2.Point");
	startPos.SetX(pts.Item(COleVariant(long(ptsInd))).GetX());
	startPos.SetY(pts.Item(COleVariant(long(ptsInd))).GetY());

	symColorInd = rand() % CAR_COLOR_NUM;
	evnt = MoTrackingLayer.AddEvent(startPos, symColorInd);
	evntNum = m_Sta.totalEvtNum++;

	// ���ó�ʼ�Ƕ�
	if (lneNum <= 5) initAng = 270;
	else if (lneNum <= 10) initAng = 90;
	else if (lneNum <= 15) initAng = 0;
	else initAng = 180;
	if (initAng < 0)
		initAng = 360.0 + initAng;

	evnt.SetSymbolIndex(CAR_COLOR_NUM + LIGHT_COLOR_NUM + symColorInd*ANGLE_NUM + int(initAng));
}
void Car::Move()
{
	if (ptsInd < divNum-1){
		Pos curPos, nexPos; // ��ǰλ��
		curPos.x = pts.Item(COleVariant(long(ptsInd))).GetX();
		curPos.y = pts.Item(COleVariant(long(ptsInd))).GetY();
		nexPos.x = pts.Item(COleVariant(long(ptsInd + 1))).GetX();
		nexPos.y = pts.Item(COleVariant(long(ptsInd + 1))).GetY();


		#ifdef ANG_TEST
/// ֻ�����µĵ�·�Ƕ��Ǳ仯��
		double ang = 0;
		if (IsInArcRoad() && IsInCenterRegion(curPos)){
			if (nexPos.y != curPos.y)
				ang = -180.0 / PI*(atan((nexPos.x - curPos.x) /
				(nexPos.y - curPos.y)));
			else
				ang = initAng;
			if (ang < 0)
				ang = 360.0 + ang;

			//// ����Ƕ�������
			//if (curPos.x == nexPos.x && curPos.y > nexPos.y)
			//	ang = 180;
			//else if (curPos.x == nexPos.x && curPos.y < nexPos.y)
			//	ang = 0;
			//else if (curPos.x < nexPos.x && curPos.y == nexPos.y)
			//	ang = 270;
			//else if (curPos.x > nexPos.x && curPos.y == nexPos.y)
			//	ang = 90;

			evnt.SetSymbolIndex(CAR_COLOR_NUM + LIGHT_COLOR_NUM +
				symColorInd*ANGLE_NUM + int(ang));
		}
		
		evnt.MoveTo(nexPos.x, nexPos.y);

		++ptsInd;
#endif
	}
	else
		Disappear();
}
void Car::Bump()
{
	flagBump = FALSE;
	BOOL flagNex = TRUE;

	// ������̵��ж�
	switch ((lneNum - 1) / 5)	// ��λ�õ��ж�
	{
	case 0:
		if (evnt.GetX() < LIGHT[0] && evnt.GetX() > LIGHT[0] - m_OptSet.buf){
			flagBump = TRUE;
			flagNex = FALSE;
		}
		else
			flagBump = FALSE;
		break;
	case 1:
		if (evnt.GetX() > LIGHT[1] && evnt.GetX() < LIGHT[1] + m_OptSet.buf){
			flagBump = TRUE;
			flagNex = FALSE;
		}
		else
			flagBump = FALSE;
		break;
	case 2:
		if (evnt.GetY() < LIGHT[2] && evnt.GetY() > LIGHT[2] - m_OptSet.buf){
			flagBump = TRUE;
			flagNex = FALSE;
		}
		else
			flagBump = FALSE;
		break;
	case 3:
		if (evnt.GetY() > LIGHT[3] && evnt.GetY() < LIGHT[3] + m_OptSet.buf){
			flagBump = TRUE;
			flagNex = FALSE;
		}
		else
			flagBump = FALSE;
		break;
	default:
		break;
	}

	// ���복�ж�
	if (flagNex){ // �����ж�
		double judNum = m_OptSet.car_dis*m_OptSet.car_dis;
		for (unsigned i = 0; i < m_Car.size(); ++i){
			if (evntNum > m_Car.at(i).evntNum &&  // ֮ǰ�ĳ�&&��ͬһ����·��
				InOneWay(m_Car.at(i).lneNum, lneNum)){
				double dx = evnt.GetX() - m_Car.at(i).evnt.GetX();
				double dy = evnt.GetY() - m_Car.at(i).evnt.GetY();
				double res = dx*dx + dy*dy;
				if (res <= judNum){
					flagBump = TRUE;
					break;
				}
			}
		}
	}
}
// �¶δ�������˰���
void Car::Disappear()
{
	MoTrackingLayer.RemoveEvent(evntNum);
	vector<Car>::iterator ite;
	for (ite = m_Car.begin(); ite != m_Car.end(); ++ite){
		if (ite->evntNum == evntNum)
			break; // �˴������׳���
	}
	for (unsigned i = 0; i < m_Car.size(); ++i){
		if (m_Car.at(i).evntNum > evntNum)
			m_Car.at(i).evntNum--;
	}
	for (unsigned i = 0; i < m_Per.size(); ++i){
		if (m_Per.at(i).evntNum > evntNum)
			m_Per.at(i).evntNum--;
	}
	m_Sta.totalEvtNum--;
	m_Car.erase(ite);
}
BOOL Car::InOneWay(int l1, int l2)
{
	if (l1 == l2)
		return TRUE;
	if (abs(l1 - l2) > 1)
		return FALSE;
	if (l1 % 5 == 3 || l2 % 5 == 3)
		return FALSE;
	return TRUE;
}
BOOL Car::IsInCenterRegion(Pos pt)
{
	if (pt.x >= LIGHT[0] - m_OptSet.buf && pt.x <= LIGHT[1] + m_OptSet.buf
		&& pt.y >= LIGHT[2] - m_OptSet.buf && pt.y <= LIGHT[3] + m_OptSet.buf)
		return TRUE;
	return FALSE;
}
BOOL Car::IsInArcRoad()
{
	int tmp = lneNum % 10;
	if (tmp == 0 || tmp == 1 || tmp == 5 || tmp == 6)
		return TRUE;
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
Person::Person()
{
}
Person::~Person()
{
	pts.ReleaseDispatch();
}
void Person::CreatePerson(int lNum, double dis, double len)

{
	lneNum = lNum;
	CMoPoint pt[2];
	for (int i = 0; i < 2; ++i)
		pt[i].CreateDispatch("MapObjects2.Point");
	ptsInd = 0;
	stepDis = dis;
	Pos startPos;

	double tmp = 0;
	switch (lNum)
	{
	case 1:case 2:
		tmp = m_OptSet.rd_xlen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid;
		tmp = tmp - RandGenerate(0, int(m_OptSet.rd_sidwk_wid));
		pt[0].SetX(tmp);
		pt[1].SetX(tmp);
		pt[0].SetY(m_OptSet.rd_ylen*(1 - len) / 2);
		pt[1].SetY(m_OptSet.rd_ylen*(1 + len) / 2);
		break;

	case 3:case 4:
		tmp = m_OptSet.rd_xlen / 2 + m_OptSet.rd_lnm*m_OptSet.rd_wid;
		tmp = tmp + RandGenerate(0, int(m_OptSet.rd_sidwk_wid));
		pt[0].SetX(tmp);
		pt[1].SetX(tmp);
		pt[0].SetY(m_OptSet.rd_ylen*(1 - len) / 2);
		pt[1].SetY(m_OptSet.rd_ylen*(1 + len) / 2);
		break;

	case 5:case 6:
		tmp = m_OptSet.rd_ylen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid;
		tmp = tmp - RandGenerate(0, int(m_OptSet.rd_sidwk_wid));
		pt[0].SetY(tmp);
		pt[1].SetY(tmp);
		pt[0].SetX(m_OptSet.rd_xlen*(1 - len) / 2);
		pt[1].SetX(m_OptSet.rd_xlen*(1 + len) / 2);
		break;

	case 7:case 8:
		tmp = m_OptSet.rd_ylen / 2 + m_OptSet.rd_lnm*m_OptSet.rd_wid;
		tmp = tmp + RandGenerate(0, int(m_OptSet.rd_sidwk_wid));
		pt[0].SetY(tmp);
		pt[1].SetY(tmp);
		pt[0].SetX(m_OptSet.rd_xlen*(1 - len) / 2);
		pt[1].SetX(m_OptSet.rd_xlen*(1 + len) / 2);
		break;

	default:
		break;
	}
	if ((lNum % 2) == 1)
		GeneratePts(pt[0], pt[1]);
	else 
		GeneratePts(pt[1], pt[0]);
	
	startPos.x = pts.Item(COleVariant(long(ptsInd))).GetX();
	startPos.y = pts.Item(COleVariant(long(ptsInd))).GetY();

	evnt = MoTrackingLayer.AddEvent(pt[0], CAR_COLOR_NUM + LIGHT_COLOR_NUM + ANGLE_NUM*CAR_COLOR_NUM);
	evnt.MoveTo(startPos.x, startPos.y);
	evntNum = m_Sta.totalEvtNum++;
}
double Person::RandGenerate(int a /*= 0*/, int b /*= 1*/)
{
	double tmp = (rand() % RAND_MAX) / double(RAND_MAX);
	return (b - a)*tmp + a;
}
void Person::GeneratePts(CMoPoint& pt1, CMoPoint& pt2)
{
	// ������	
	CMoPoints ps;
	ps.CreateDispatch("MapObjects2.Points");
	CMoLine lne;
	lne.CreateDispatch("MapObjects2.Line");
	ps.Add(pt1);
	ps.Add(pt2);
	lne.GetParts().Add(ps);

	// ��ȡ�㼯
	pts.CreateDispatch("MapObjects2.Points");
	divNum = int(lne.GetLength() / stepDis);
	lne.SetMeasuresAsLength();
	for (int i = 0; i < divNum; ++i)
		pts.Add(lne.ReturnPointEvents(stepDis*i).Item(COleVariant(long(0))));
}
void Person::Move()
{
	if (ptsInd < divNum - 1){
		Pos curPos, nexPos; // ��ǰλ��
		curPos.x = pts.Item(COleVariant(long(ptsInd))).GetX();
		curPos.y = pts.Item(COleVariant(long(ptsInd))).GetY();
		nexPos.x = pts.Item(COleVariant(long(ptsInd + 1))).GetX();
		nexPos.y = pts.Item(COleVariant(long(ptsInd + 1))).GetY();

		evnt.MoveTo(nexPos.x, nexPos.y);
		++ptsInd;
	}
	else
		Disappear();
}
void Person::Disappear()
{
	MoTrackingLayer.RemoveEvent(evntNum);
	vector<Person>::iterator ite;
	for (ite = m_Per.begin(); ite != m_Per.end(); ++ite){
		if (ite->evntNum == evntNum)
			break; // �˴������׳���
	}
	for (unsigned i = 0; i < m_Car.size(); ++i){
		if (m_Car.at(i).evntNum > evntNum)
			m_Car.at(i).evntNum--;
	}
	for (unsigned i = 0; i < m_Per.size(); ++i){
		if (m_Per.at(i).evntNum > evntNum)
			m_Per.at(i).evntNum--;
	}
	m_Sta.totalEvtNum--;
	m_Per.erase(ite);
}
void Person::Bump()
{
	flagBump = FALSE;

	// ������̵��ж�
	Pos curPos;
	curPos.x = evnt.GetX();
	curPos.y = evnt.GetY();
	switch (lneNum)
	{
	case 1:case 3:
		if ((curPos.y < SIDEWALK[0] && curPos.y > SIDEWALK[0] - m_OptSet.per_light_dis))
			flagBump = TRUE;
		else
			flagBump = FALSE;
		break;
	case 2:case 4:
		if ((curPos.y > SIDEWALK[1] && curPos.y < SIDEWALK[1] + m_OptSet.per_light_dis))
			flagBump = TRUE;
		else
			flagBump = FALSE;
		break;
	case 5:case 7:
		if ((curPos.x < SIDEWALK[2] && curPos.x > SIDEWALK[2] - m_OptSet.per_light_dis))
			flagBump = TRUE;
		else
			flagBump = FALSE;
		break;
	case 6:case 8:
		if ((curPos.x > SIDEWALK[3] && curPos.x < SIDEWALK[3] + m_OptSet.per_light_dis))
			flagBump = TRUE;
		else
			flagBump = FALSE;
		break;
	default:
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
void CTRANS_SIMU_VIEW::OnCrossrdsimuStart()
{
	m_flagOpt = TRUE;
	if (!m_flagSimu){
		CreateRoad();			// �����ײ��·
		GetTrackLine();			// ��ȡ������
		InitTrackingLayerAndData();	// ��ʼ�����ſ�
		GenerateLight();		// ������
#ifndef RANDOM_GENERATE
		GenerateObject();
#endif // RANDOM_GENERATE
		SelecetState(3);		// ѡ����λ
	}
	SetTimer(1, m_OptSet.light_ref, NULL);// ��λʱ��ˢ������
	SetTimer(2, m_OptSet.car_ref, NULL);		// ��ʱ��
	SetTimer(4, m_OptSet.per_ref, NULL);		// ��ʱ��
#ifdef RANDOM_GENERATE
	SetTimer(3, m_OptSet.car_elapse_circle, NULL);	
	SetTimer(5, m_OptSet.per_elapse_circle, NULL);
#endif // RANDOM_GENERATE

	if (!m_flagSimu)
		m_flagSimu = TRUE;
}
void CTRANS_SIMU_VIEW::OnCrossrdsimuStop()
{
	KillTimer(1);
	KillTimer(2);
	KillTimer(3);
	KillTimer(4);
	KillTimer(5);
}
void CTRANS_SIMU_VIEW::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1: // ��
	{
				m_Sta.time_run += m_OptSet.light_ref;
				// ��ǰ��λ
				int op = int(m_Sta.time_run) / m_OptSet.light_circle;
				//// ֮ǰ��λ
				//int opPre = int(TIME_LIGHT - CIRCLE_LIGHT_REF) / CIRCLE_LIGHT;
				//if ((op - opPre) == 1)
				//	TIME_LIGHT_LEFT = CIRCLE_LIGHT / 1000;

				m_Sta.time_left = UINT((m_OptSet.light_circle - (m_Sta.time_run - int(m_Sta.time_run / m_OptSet.light_circle)*m_OptSet.light_circle)) / 1000.0);
				
				switch (op % 4)
				{
				case 0:
					SelecetState(3);
					break;
				case 1:
					SelecetState(2);
					break;
				case 2:
					SelecetState(1);
					break;
				case 3:
					SelecetState(4);
					break;
				default:
					break;
				}
				break;
	}
	case 2:
	{
#ifdef BUMP_TEST
			  for (unsigned i = 0; i < m_Car.size(); ++i){
				  if (m_Car.at(i).flagState == TRUE)
					  m_Car.at(i).Move();
				  else{
					  m_Car.at(i).Bump();
					  if (!m_Car.at(i).flagBump)
						  m_Car.at(i).Move();
				  }
			  }
#else
			  for (unsigned i = 0; i < m_Car.size(); ++i){
				  m_Car.at(i).Move();
			  }
#endif
				  
			  break;
	}
#ifdef RANDOM_GENERATE
	case 3:
	{
			  // ����Ͷ��
			  int rdNum = 0;
			  int tmp = 0;

			  /// ���Ͷ��
			  if (m_OptSet.generate_num == 1){
				  rdNum = rand() % 20 + 1;
				  tmp = rdNum % 10;
			  }
			  else{ /// ����Ͷ��
				  rdNum = m_Sta.totalCar % 20 + 1;
				  tmp = rdNum % 10;
				  m_Sta.totalCar++;
			  }
			  
			  if (tmp == 2 || tmp == 4 || tmp == 7 || tmp == 9){
				  // ��Ͷ��
			  }
			  else{
				  // �ж������һ�����Ƿ��ڳ�ʼ�㣬���ǣ�������
				  int carInd = -1;
				  double dx = 0, dy = 0;
				  for (unsigned i = 0; i < m_Car.size(); ++i){
					  if (m_Car.at(i).lneNum == rdNum){
						  carInd = i;
					  }
				  }
				  if (carInd == -1){
					  Car car;
					  car.CreateCar(rdNum,
						  m_OptSet.car_ref / 1000.0*m_OptSet.car_speed);
					  m_Car.push_back(car);
				  }
				  else if(carInd >=0){
					  dx = m_Car.at(carInd).evnt.GetX() -
						  m_Car.at(carInd).pts.Item(COleVariant(long(0))).GetX();
					  dy = m_Car.at(carInd).evnt.GetY() -
						  m_Car.at(carInd).pts.Item(COleVariant(long(0))).GetY();

					  if (abs(dx) + abs(dy) > m_OptSet.car_dis){
						  Car car;
						  car.CreateCar(rdNum,
							  m_OptSet.car_ref / 1000.0*m_OptSet.car_speed);
						  m_Car.push_back(car);
					  }
				  }
			  }
			  break;
	}
#endif // RANDOM_GENERATE
	case 4:
	{
#ifdef BUMP_TEST
			  // �������ж��Ƿ��ܹ���ȥ��·��һ�ж�ģ��
			  for (unsigned i = 0; i < m_Per.size(); ++i){
				  m_Per.at(i).Bump();

				  if (!m_Per.at(i).flagBump)
					  m_Per.at(i).Move();
				  else if (m_Per.at(i).flagState == TRUE){
					  double disPossible = 1000 *
						  m_Sta.time_left*m_Per.at(i).stepDis / m_OptSet.per_ref;
					  if (disPossible >=
						  m_OptSet.rd_wid*(m_OptSet.rd_lnm + m_OptSet.rd_rnm)){
						  m_Per.at(i).Move();
					  }
				  }
			  }
#else
			  for (unsigned i = 0; i < m_Per.size(); ++i){
				  m_Per.at(i).Move();
			  }
#endif
			  break;
	}
#ifdef RANDOM_GENERATE
	case 5:
	{
			  Person per;
			  int rdNum = 0;
			  if (m_OptSet.generate_num == 1) /// ���Ͷ��
				  rdNum = rand() % 8 + 1;
			  else{
				  rdNum = m_Sta.totalPer % 8 + 1;	  /// ����Ͷ��
				  m_Sta.totalPer++;
			  }
			  per.CreatePerson(rdNum, m_OptSet.per_ref / 1000.0*m_OptSet.per_speed);
			  m_Per.push_back(per);

			  break;
	}
#endif // RANDOM_GENERATE
	case 6:
	{
			  HANDLE hWnd = ::FindWindowEx(NULL, NULL, NULL, "Warning");
			  ::SendMessage((HWND)hWnd, WM_CLOSE, NULL, NULL);
			  break;
	}
	case 7:
	{
			  HANDLE hWnd = ::FindWindowEx(NULL, NULL, NULL, ":-)");
			  ::SendMessage((HWND)hWnd, WM_CLOSE, NULL, NULL);
			  break;
	}
	}
	CFormView::OnTimer(nIDEvent);
}

void CTRANS_SIMU_VIEW::GenerateObject()
{
	// ��������
	for (int i = 1; i <= 5; ++i){
		Car car;
		car.CreateCar(i, 1.0);
		m_Car.push_back(car);
	}
	// ������
	for (int i = 1; i <= 8; ++i){
		Person per;
		per.CreatePerson(i, 0.3);
		m_Per.push_back(per);
	}
}
void CTRANS_SIMU_VIEW::GenerateLight()
{
	TrafficLight light[8];
	Pos pt1, pt2, pt3, pt4;
	float x = m_OptSet.rd_xlen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid - m_OptSet.rd_sidwk_wid;
	float y = m_OptSet.rd_ylen / 2;
	pt1.x = x;
	pt1.y = y;
	pt2.x = x;
	pt2.y = y - m_OptSet.rd_wid;
	pt3.x = x;
	pt3.y = y - (m_OptSet.rd_rnm - 1)*m_OptSet.rd_wid;
	pt4.x = x;
	pt4.y = y - m_OptSet.rd_rnm*m_OptSet.rd_wid;

	light[0].CreateLight2(pt1, pt2, pt3, pt4);	// һ
	light[1].CreateLight(pt2, pt3);	// ��

	LIGHT[0] = pt1.x;

	pt1.x = m_OptSet.rd_xlen - pt1.x;
	pt1.y = m_OptSet.rd_ylen - pt1.y;
	pt2.x = m_OptSet.rd_xlen - pt2.x;
	pt2.y = m_OptSet.rd_ylen - pt2.y;
	pt3.x = m_OptSet.rd_xlen - pt3.x;
	pt3.y = m_OptSet.rd_ylen - pt3.y;
	pt4.x = m_OptSet.rd_xlen - pt4.x;
	pt4.y = m_OptSet.rd_ylen - pt4.y;
	light[3].CreateLight2(pt1, pt2, pt3, pt4);	// ��
	light[2].CreateLight(pt2, pt3);	// ��

	LIGHT[1] = pt1.x;

	x = m_OptSet.rd_xlen / 2;
	y = m_OptSet.rd_ylen / 2 - m_OptSet.rd_lnm*m_OptSet.rd_wid - m_OptSet.rd_sidwk_wid;
	pt1.x = x;
	pt1.y = y;
	pt2.x = x + m_OptSet.rd_wid;
	pt2.y = y;
	pt3.x = x + m_OptSet.rd_wid*(m_OptSet.rd_lnm-1);
	pt3.y = y;
	pt4.x = x + m_OptSet.rd_wid*m_OptSet.rd_lnm;
	pt4.y = y;
	light[4].CreateLight2(pt1, pt2,pt3,pt4);	// ��
	light[5].CreateLight(pt2, pt3);	// ��

	LIGHT[2] = pt1.y;

	pt1.x = m_OptSet.rd_xlen - pt1.x;
	pt1.y = m_OptSet.rd_ylen - pt1.y;
	pt2.x = m_OptSet.rd_xlen - pt2.x;
	pt2.y = m_OptSet.rd_ylen - pt2.y;
	pt3.x = m_OptSet.rd_xlen - pt3.x;
	pt3.y = m_OptSet.rd_ylen - pt3.y;
	pt4.x = m_OptSet.rd_xlen - pt4.x;
	pt4.y = m_OptSet.rd_ylen - pt4.y;
	light[7].CreateLight2(pt1, pt2, pt3, pt4);	// ��
	light[6].CreateLight(pt2, pt3);	// ��

	LIGHT[3] = pt1.y;

	for (int i = 0; i < 8; ++i)
		m_Light.push_back(light[i]);
}

void CTRANS_SIMU_VIEW::SelecetState(int state /*= 3*/)
{
	switch (state)	// state Ϊ��λ����
	{
	case 1:
		// �ƣ�6 7�� ����죩
		for (unsigned i = 0; i < m_Light.size(); ++i){
			if (i == 5 || i == 6) m_Light.at(i).ChangeLightColor(short(1));
			else m_Light.at(i).ChangeLightColor(short(0));
		}
		// ��
		for (unsigned i = 0; i < m_Car.size(); ++i){
			int tmp = m_Car.at(i).lneNum;
			if (tmp == 12 || tmp == 13 || tmp == 14 || 
				tmp == 17 || tmp == 18 || tmp == 19){
				m_Car.at(i).flagState = TRUE; // ��
			}
			else
				m_Car.at(i).flagState = FALSE;
		}
		// ��
		for (unsigned i = 0; i < m_Per.size(); ++i){
			if (m_Per.at(i).lneNum <= 4)
				m_Per.at(i).flagState = TRUE; // ��
			else
				m_Per.at(i).flagState = FALSE; // ����
		}

		break;
	case 2:
		// �ƣ�1 4�� ����죩
		for (unsigned i = 0; i < m_Light.size(); ++i){
			if (i == 0 || i == 3) m_Light.at(i).ChangeLightColor(short(1));
			else m_Light.at(i).ChangeLightColor(short(0));
		}
		// ��
		for (unsigned i = 0; i < m_Car.size(); ++i){
			int tmp = m_Car.at(i).lneNum;
			if (tmp == 1 || tmp == 5 || 
				tmp == 6 || tmp == 10){
				m_Car.at(i).flagState = TRUE; // ��
			}
			else
				m_Car.at(i).flagState = FALSE;
		}
		// ��
		for (unsigned i = 0; i < m_Per.size(); ++i){
			m_Per.at(i).flagState = FALSE;
		}
		break;
	case 3:
		// ��(2 3�� �����)
		for (unsigned i = 0; i < m_Light.size(); ++i){
			if (i == 1 || i == 2) m_Light.at(i).ChangeLightColor(short(1));
			else m_Light.at(i).ChangeLightColor(short(0));
		}
		// ��
		for (unsigned i = 0; i < m_Car.size(); ++i){
			int tmp = m_Car.at(i).lneNum;
			if (tmp == 2 || tmp == 3 || tmp == 4 ||
				tmp == 7 || tmp == 8 || tmp == 9){
				m_Car.at(i).flagState = TRUE; // ��
			}
			else
				m_Car.at(i).flagState = FALSE;
		}
		// ��
		for (unsigned i = 0; i < m_Per.size(); ++i){
			if (m_Per.at(i).lneNum >= 5 && m_Per.at(i).lneNum <=8)
				m_Per.at(i).flagState = TRUE; // ��
			else
				m_Per.at(i).flagState = FALSE; // ����
		}
		break;
	case 4:
		// �ƣ�5 8�� ����죩
		for (unsigned i = 0; i < m_Light.size(); ++i){
			if (i == 4 || i == 7) m_Light.at(i).ChangeLightColor(short(1));
			else m_Light.at(i).ChangeLightColor(short(0));
		}
		// ��
		for (unsigned i = 0; i < m_Car.size(); ++i){
			int tmp = m_Car.at(i).lneNum;
			if (tmp == 11 || tmp == 15 ||
				tmp == 16 || tmp == 20){
				m_Car.at(i).flagState = TRUE; // ��
			}
			else
				m_Car.at(i).flagState = FALSE;
		}
		// ��
		for (unsigned i = 0; i < m_Per.size(); ++i){
			m_Per.at(i).flagState = FALSE;
		}
		break;
// 	case 5:
// 		// ���˳����ж�
// 		// ...
// 		// ȫ���Ƶ�
// 		for (unsigned i = 0; i < m_Light.size(); ++i){
// 			m_Light.at(i).ChangeLightColor(short(2));
// 		}
// 		break;
	default:
		break;
	}
}

void CTRANS_SIMU_VIEW::AdjustSymbolSize(double scale /*= 1.0*/)
{
	// ���ſ��С����
	if (MoTrackingLayer){
		int tmp = CAR_COLOR_NUM + LIGHT_COLOR_NUM + ANGLE_NUM*CAR_COLOR_NUM + 1;

		for (int i = 0; i < MoTrackingLayer.GetSymbolCount(); ++i){
			CMoSymbol sym(MoTrackingLayer.GetSymbol(long(i)));
			if (i == tmp - 1)
				sym.SetSize(int(m_OptSet.per_size*scale));
			else if (i >= CAR_COLOR_NUM && i < CAR_COLOR_NUM + LIGHT_COLOR_NUM){
				sym.SetSize(int(m_OptSet.light_size*scale));
			}
			else
				sym.SetSize(int(m_OptSet.car_size*scale));
		}
	}
	m_map.Refresh();
}

void CTRANS_SIMU_VIEW::OnCrossrdsimuOption()
{
	if (m_flagSimu)
		OnCrossrdsimuStop();

	COption optDlg;
	optDlg.m_carCircle	= m_OptSet.car_elapse_circle;
	optDlg.m_carSize	= m_OptSet.car_size;
	optDlg.m_carSpeed	= m_OptSet.car_speed;
	
	optDlg.m_perCircle	= m_OptSet.per_elapse_circle;
	optDlg.m_perSize	= m_OptSet.per_size;
	optDlg.m_perSpeed	= m_OptSet.per_speed;

	optDlg.m_rdWid		= m_OptSet.rd_wid;
	optDlg.m_rdCenWid	= m_OptSet.rd_cen_wid;
	optDlg.m_rdXlen		= m_OptSet.rd_xlen;
	optDlg.m_sidewalkWid = m_OptSet.rd_sidwk_wid;
	optDlg.m_rdYlen		= m_OptSet.rd_ylen;

	optDlg.m_bakClrR = GetRValue(m_OptSet.bak_clr);
	optDlg.m_bakClrG = GetGValue(m_OptSet.bak_clr);
	optDlg.m_bakClrB = GetBValue(m_OptSet.bak_clr);
	optDlg.m_perClrR = GetRValue(m_OptSet.per_clr);
	optDlg.m_perClrG = GetGValue(m_OptSet.per_clr);
	optDlg.m_perClrB = GetBValue(m_OptSet.per_clr);
	optDlg.m_GenetatrNum = m_OptSet.generate_num;

	optDlg.m_lightCircle = m_OptSet.light_circle / 1000; // ms->s
	optDlg.m_carRef = m_OptSet.car_ref;
	optDlg.m_perRef = m_OptSet.per_ref;
	optDlg.m_lightRef = m_OptSet.light_ref;
	optDlg.m_buf = m_OptSet.buf;
	optDlg.m_carDis = m_OptSet.car_dis;
	optDlg.m_perLightDis = m_OptSet.per_light_dis;
	optDlg.m_lightSize = m_OptSet.light_size;

	if (optDlg.DoModal() == IDOK){
		m_OptSet.car_elapse_circle = optDlg.m_carCircle;
		m_OptSet.car_size = optDlg.m_carSize;
		m_OptSet.car_speed = optDlg.m_carSpeed;

		m_OptSet.per_elapse_circle = optDlg.m_perCircle;
		m_OptSet.per_size = optDlg.m_perSize;
		m_OptSet.per_speed = optDlg.m_perSpeed;

		m_OptSet.rd_wid = optDlg.m_rdWid;
		m_OptSet.rd_cen_wid = optDlg.m_rdCenWid;
		m_OptSet.rd_xlen = optDlg.m_rdXlen;
		m_OptSet.rd_sidwk_wid = optDlg.m_sidewalkWid;
		m_OptSet.rd_ylen = optDlg.m_rdYlen;

		m_OptSet.bak_clr = RGB(optDlg.m_bakClrR, optDlg.m_bakClrG, optDlg.m_bakClrB);
		m_OptSet.per_clr = RGB(optDlg.m_perClrR, optDlg.m_perClrG, optDlg.m_perClrB);
		//m_OptSet.per_clr = optDlg.m_perClrR + optDlg.m_perClrG * 256, optDlg.m_perClrB * 256 * 256; // for static lib 
		m_OptSet.generate_num = optDlg.m_GenetatrNum;

		m_OptSet.light_circle = optDlg.m_lightCircle*1000; // s->ms
		m_OptSet.car_ref = optDlg.m_carRef;
		m_OptSet.per_ref = optDlg.m_perRef;
		m_OptSet.light_ref = optDlg.m_lightRef;
		m_OptSet.buf = optDlg.m_buf;
		m_OptSet.car_dis = optDlg.m_carDis;
		m_OptSet.per_light_dis = optDlg.m_perLightDis;
		m_OptSet.light_size = optDlg.m_lightSize;

		/// ����ģ���У������������
		if (m_flagSimu){
			SetTimer(6, 1000, NULL);
			MessageBox("ģ�����ڽ��У����ڹر�", "Warning", MB_ICONEXCLAMATION | MB_ICONWARNING);
			Clear();
			KillTimer(6);

			GenerateLight();	// �Ʋ��ܱ�ɾ��
			OnCrossrdsimuStart();
		}
		else{
			SetTimer(7, 800, NULL);
			MessageBox("ģ�⼴����ʼ", ":-)", NULL);
			OnCrossrdsimuStart();
			KillTimer(7);
		}
		m_map.SetBackColor(m_OptSet.bak_clr);
		InitTrackingLayerAndData();
	}
	else if (m_flagOpt == TRUE){
		OnCrossrdsimuStart();
	}
}

void CTRANS_SIMU_VIEW::OnDestroy()
{
	if (m_flagSimu){
		Clear();

		/// ����ɾ��
		// ɾ����
		vector<CMoLine>::iterator iteLine = MoTrackLine.begin();
		for (; iteLine != MoTrackLine.end();){
			iteLine = MoTrackLine.erase(iteLine);
		}
	}
	CFormView::OnDestroy();
}

/// ����ͼ�����ݡ��켣�߽���ɾ��
void CTRANS_SIMU_VIEW::Clear()
{
	if (m_flagSimu){
		vector<TrafficLight>::iterator iteLight = m_Light.begin();
		for (; iteLight != m_Light.end();)
			iteLight = m_Light.erase(iteLight);

		vector<Car>::iterator iteCar = m_Car.begin();
		for (; iteCar != m_Car.end();){
			iteCar = m_Car.erase(iteCar);
		}

		vector<Person>::iterator itePer = m_Per.begin();
		for (; itePer != m_Per.end();)
			itePer = m_Per.erase(itePer);

		MoTrackingLayer.ClearEvents();
		m_Sta.totalCar = 0;
		m_Sta.totalEvtNum = 0;
		m_Sta.totalEvtNum = 0;
	}
}

