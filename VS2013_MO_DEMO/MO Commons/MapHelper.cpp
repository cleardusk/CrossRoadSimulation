//////////////////////////////////////////////////////////////////////////
// MapHelper.cpp - Utility routines for working with the MapObjects control
// 

#include "stdafx.h"
#include <math.h>
#include <afxctl.h>
#include "maphelper.h"


///////////////////////////////////////////////////////////////////////////
// Data Directory
//
CDataDir::CDataDir()
:	m_path("")
{

  char  szPath[MAX_PATH];
  ::GetCurrentDirectory(MAX_PATH,szPath);
  CString currentDir(szPath);
  int pos = currentDir.Find("MFC");
  if (pos >= 0)
		m_path =  (currentDir.Left(pos-1)) + TEXT("\\Data");
  else
    m_path = "";
}

CString CDataDir::GetPath()
{
	return m_path;
}

void CDataDir::SetPath(LPCTSTR newPath)
{
	m_path = newPath;
}


///////////////////////////////////////////////////////////////////////////
// File name handling
//
CString GetFileDirectory(const CString& path)
{
	ASSERT(path.GetLength());
	int pos = path.ReverseFind('\\');
	if (pos >= 0)
		return path.Left(pos);
	return "";
}

CString GetFileName(const CString& path)
{
	ASSERT(path.GetLength());
	int pos = path.ReverseFind('\\');
	if (pos >= 0)
		return path.Right(path.GetLength() - pos - 1);
	return "";
}

CString GetFileExt(const CString& path)
{
	ASSERT(path.GetLength());
	int pos = path.ReverseFind('.');
	if (pos >= 0)
		return path.Right(path.GetLength() - pos - 1);
	return "";
}

CString GetFileTitle(const CString& path)
{
	ASSERT(path.GetLength());
	CString strResult = GetFileName(path);
	int pos = strResult.ReverseFind('.');
	if (pos >= 0)
		return strResult.Left(pos);
	return strResult;
}

///////////////////////////////////////////////////////////////////////////
// Layer Management
//
void AddLayer(CMap1& map)
{
	CString filter(TEXT("ESRI Shapefiles (*.shp)|*.shp|"));
	CString allFormats = TEXT("All supported formats|*.shp");
	
#if 0
	// add the image filters to the end of the string
	ImageFactory::FormatDescArray formats;
	ImageFactory::QueryFormats(formats);
	
	for (int i = 0; i < formats.GetSize(); i++)
	{
		if (formats[i].m_extensions.GetLength() == 0) continue;
		
		filter += formats[i].m_name + TEXT("|");
		filter += formats[i].m_extensions + TEXT("|");
		
		allFormats += TEXT(";") + formats[i].m_extensions;
	}
	
	filter = allFormats + TEXT("|") + filter + TEXT("|");
#endif
	
	CFileDialog dlg(TRUE, TEXT(".shp"), 0, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, filter);
	
	if (dlg.DoModal() == IDOK)
	{
		CString ext = GetFileExt(dlg.GetPathName());
		if (ext.CompareNoCase(TEXT("shp")) == 0)
			AddLayer(map, dlg.GetPathName());
		else
			AddImageLayer(map, dlg.GetPathName());
				
	}
}

void AddLayer(CMap1& map, const CString& path, COLORREF color, LPDISPATCH renderer)
{
	// Establish connection to data
	CMoDataConnection conn;
	if (!conn.CreateDispatch(TEXT("MapObjects2.DataConnection")))
		throw "unable to create MapObjects2.DataConnection";
	conn.SetDatabase(GetFileDirectory(path));

	if (!conn.Connect())
		throw "unable to connect to database";

	// Add layer specified by path
	CMoLayers layers(map.GetLayers());
	CMoMapLayer layer;
 	if (!layer.CreateDispatch(TEXT("MapObjects2.MapLayer")))
		throw "unable to create MapObjects2.MapLayer";
	
	CMoGeoDataset geoDataset(conn.FindGeoDataset(GetFileTitle(path)));
	layer.SetGeoDataset(geoDataset);
	if (color != -1)		// Set color if specified
	{
		CMoSymbol layerSymbol(layer.GetSymbol());
		layerSymbol.SetColor(color);
	}

	if (renderer)
		layer.SetRenderer(renderer);

	layers.Add(layer);
}

void AddLabelLayer(CMap1& map, const CString& path, LPCTSTR labelField, COLORREF color)
{
	// Establish connection to data
	CMoDataConnection conn;
	if (!conn.CreateDispatch(TEXT("MapObjects2.DataConnection")))
		throw "unable to create MapObjects2.DataConnection";
	conn.SetDatabase(GetFileDirectory(path));
	if (!conn.Connect())
		throw "unable to connect to database";

	// Add layer specified by path
	CMoLayers layers(map.GetLayers());
	CMoMapLayer layer;
 	if (!layer.CreateDispatch(TEXT("MapObjects2.MapLayer")))
		throw "unable to create MapObjects2.MapLayer";
	CMoGeoDataset geoDataset(conn.FindGeoDataset(GetFileTitle(path)));
	layer.SetGeoDataset(geoDataset);

	// Setup label renderer
	CMoLabelRenderer lr;
	lr.CreateDispatch("MapObjects2.LabelRenderer");
	lr.SetField(labelField);
	lr.SetDrawBackground(FALSE);

	CMoTextSymbol sym(lr.GetSymbol(0));
	CFontHolder font(0);
	const FONTDESC fd = { sizeof(FONTDESC), OLESTR("Arial"), FONTSIZE(12), FW_NORMAL, ANSI_CHARSET, FALSE, FALSE, FALSE };
	//font.InitializeFont(&fd, sym.GetFont());
	font.InitializeFont(&fd);
  LPFONTDISP pFontDisp = font.GetFontDispatch();
	sym.SetFont(pFontDisp);

  if (pFontDisp)
    pFontDisp->Release();

	if (color != -1)
		sym.SetColor(color);
	sym.SetVerticalAlignment(moAlignBottom);
	sym.SetHorizontalAlignment(moAlignLeft);

	layer.SetRenderer(lr);

	layers.Add(layer);
}


void AddImageLayer(CMap1& map, const CString& path)
{
	// Establish connection to data
	CMoDataConnection conn;
	if (!conn.CreateDispatch(TEXT("MapObjects2.DataConnection")))
		throw "unable to create MapObjects2.DataConnection";

	conn.SetDatabase(GetFileDirectory(path));
	if (!conn.Connect())
		throw "unable to connect to database";

	// Add image layer
	CMoLayers layers(map.GetLayers());
	CMoImageLayer image;
 	if (!image.CreateDispatch(TEXT("MapObjects2.ImageLayer")))
		throw "unable to create MapObjects2.ImageLayer";
	image.SetFile(path);
	layers.Add(image);
}


void AddSDELayer(CMap1& map, LPCTSTR server, LPCTSTR user, 
				 LPCTSTR password, LPCTSTR db, COLORREF color)
{
	// Establish connection to data
	CMoDataConnection conn;
	if (!conn.CreateDispatch(TEXT("MapObjects2.DataConnection")))
		throw "unable to create MapObjects2.DataConnection";
	conn.SetServer(server);
	conn.SetUser(user);
	conn.SetPassword(password);
	conn.SetDatabase(db);
	if (!conn.Connect())
		throw "unable to connect to database";

	// Add layer specified by path
	CMoLayers layers(map.GetLayers());
	CMoMapLayer layer;
 	if (!layer.CreateDispatch(TEXT("MapObjects2.MapLayer")))
		throw "unable to create MapObjects2.MapLayer";
	CMoGeoDataset geoDataset(conn.FindGeoDataset(db));
	layer.SetGeoDataset(geoDataset);
	if (color != -1)		// Set color if specified
	{
		CMoSymbol layerSymbol(layer.GetSymbol());
		layerSymbol.SetColor(color);
	}
	layers.Add(layer);
}

//
// All layer objects support the GetLayerType method.
// Each object's dispatch ID for the property is different 
// though.  Given the object's dispatch pointer, lookup 
// the dispatch ID for the LayerType property then call 
// Invoke to get the layer type.
//
short GetLayerType(CMoLayers& layers, short layerNum)
{
	USES_CONVERSION;
	short		layerType = -1;
	LPCOLESTR	lpOleStr = T2COLE(TEXT("LayerType"));
	DISPID		dispID = -1;

	COleDispatchDriver dd(layers.Item(COleVariant(layerNum)));
	if (SUCCEEDED(LPDISPATCH(dd)->GetIDsOfNames(IID_NULL,
		(LPOLESTR*)&lpOleStr, 1, 0, &dispID)))
	{
		dd.GetProperty(dispID, VT_I2, &layerType);
	}

	return layerType;
}



//
// Works with Redlands sample data
//
LPDISPATCH CreateStreetRenderer()
{
	CMoSymbol sym;
	CMoValueMapRenderer vr;
	VERIFY(vr.CreateDispatch("MapObjects2.ValueMapRenderer"));

	vr.SetField(TEXT("CFCC"));
	vr.SetSymbolType(moLineSymbol);
	vr.SetValueCount(6);

	vr.SetValue(0, TEXT("A11"));
	sym.AttachDispatch(vr.GetSymbol(0));
	sym.SetColor(moBlue);
	sym.SetSize(9);

	vr.SetValue(1, TEXT("A15"));
	sym.AttachDispatch(vr.GetSymbol(1));
	sym.SetColor(moBlue);
	sym.SetSize(9);

	vr.SetValue(2, TEXT("A20"));
	sym.AttachDispatch(vr.GetSymbol(2));
	sym.SetColor(moNavy);
	sym.SetSize(5);

	vr.SetValue(3, TEXT("A21"));
	sym.AttachDispatch(vr.GetSymbol(3));
	sym.SetColor(moNavy);
	sym.SetSize(5);

	vr.SetValue(4, TEXT("A31"));
	sym.AttachDispatch(vr.GetSymbol(4));
	sym.SetColor(moBrown);
	sym.SetSize(2);

	vr.SetValue(5, TEXT("A63"));
	sym.AttachDispatch(vr.GetSymbol(5));
	sym.SetColor(moRed);
	sym.SetSize(5);

	sym.AttachDispatch(vr.GetDefaultSymbol());
	sym.SetColor(moGray);
	sym.SetSize(2);
	
	LPDISPATCH(vr)->AddRef();
	return LPDISPATCH(vr);
}

///////////////////////////////////////////////////////////////////////////
// Zoom and Pan
//
void ZoomIn(CMap1& map) 
{
	CMoRectangle r(map.TrackRectangle());
	if (LPDISPATCH(r))
		map.SetExtent(r);
}

void ZoomOut(CMap1& map) 
{
	CMoRectangle r(map.GetExtent());
	ASSERT(LPDISPATCH(r));
	r.ScaleRectangle(1.5);
	map.SetExtent(r);
}

void ZoomFull(CMap1& map) 
{
	CMoRectangle r(map.GetFullExtent());
	ASSERT(LPDISPATCH(r));
	map.SetExtent(r);
}

void Pan(CMap1& map) 
{
	map.Pan();
}

void Resize(CMap1& map, int cx, int cy, int border) 
{
	if (map.m_hWnd)
		map.SetWindowPos(0, border, border, cx - border, cy - border, SWP_NOZORDER);
}

///////////////////////////////////////////////////////////////////////////
// Conversion
//
double DegreesToMiles(double x1, double y1, double x2, double y2)
{
	// H. Andoyer: [Annuaire du Bureau des Longitudes, 1950, Paris, p. 145] 
	if (x1 == x2 && y1 == y2)
		return 0;

	double a = 6378137;      // Use GRS 80 spheroid 
	double f = 0.003352813;  // 1 / 298.257 

	double F = (DEG2RAD(y1) + DEG2RAD(y2)) / 2.0;
	double G = (DEG2RAD(y1) - DEG2RAD(y2)) / 2.0;
	double l = (DEG2RAD(x1) - DEG2RAD(x2)) / 2.0;

	double sF2 = pow(sin(F),2);
	double cF2 = pow(cos(F),2);
	double sG2 = pow(sin(G),2);
	double cG2 = pow(cos(G),2);
	double sL2 = pow(sin(l),2);
	double cL2 = pow(cos(l),2);

	double S = sG2*cL2 + cF2*sL2;
	double C = cG2*cL2 + sF2*sL2;

	double omega = atan(sqrt(S/C));
	double rho = sqrt(S*C) / omega;

	double D = 2*a*omega;

	double H1 = (3*rho - 1) / (2*C);
	double H2 = (3*rho + 1) / (2*S);

	double m = D*(1 + f*(H1*sF2*cG2 - H2*cF2*sG2));

	return m * 0.0006214;
}



///////////////////////////////////////////////////////////////////////////
// Miscellaneous
//
LPDISPATCH GetIDispatch(CMap1& map)
{
	LPDISPATCH pDispatch = 0;
	LPUNKNOWN pUnknown = map.GetControlUnknown();
	if (pUnknown)
		pUnknown->QueryInterface(IID_IDispatch, (void**)&pDispatch);
	return pDispatch;
}


double GetAspectRatio(CMap1& map)
{
	CMoRectangle r(map.GetExtent());
	return (double)r.GetWidth() / (double)r.GetHeight();
}


//
// Use ratio of map to window to calculate map point
//
LPDISPATCH ToMapPoint(CMap1& map, long x, long y)
{
	ASSERT(map.m_hWnd);
	CRect client;
	map.GetClientRect(&client);
	CMoRectangle bounds(map.GetExtent());

	double mapX = bounds.GetLeft() + (double)x * bounds.GetWidth() / (double)client.Width();
	double mapY = bounds.GetTop() - (double)y * bounds.GetHeight() / (double)client.Height();

	CMoPoint mapPoint;
	mapPoint.CreateDispatch(TEXT("MapObjects2.Point"));
	mapPoint.SetX(mapX);
	mapPoint.SetY(mapY);
	LPDISPATCH(mapPoint)->AddRef(); // Client is responsible for release
	return mapPoint;
}

void FromMapPoint(CMap1& map, CMoPoint& pt, long& x, long& y)
{
	ASSERT(map.m_hWnd);
	CRect client;
	map.GetClientRect(&client);

	CMoRectangle bounds(map.GetExtent());
		
	x = (long)MULDIV((pt.GetX() - bounds.GetLeft()), client.Width(), bounds.GetWidth());
	y = (long)MULDIV((bounds.GetTop() - pt.GetY()),  client.Height(), bounds.GetHeight());
}		

double ToMapDistance(CMap1& map, long x)
{ 
	ASSERT(map.m_hWnd); 
	CRect client; 
	map.GetClientRect(&client);

	CMoRectangle bounds(map.GetExtent());
	return (double)x * bounds.GetWidth() / (double)client.Width(); 
}

long FromMapDistance(CMap1& map, double& mapX)
{
	ASSERT(map.m_hWnd);
	CRect client;
	map.GetClientRect(&client);

	CMoRectangle bounds(map.GetExtent());
		
	return (long)MULDIV(mapX, client.Width(), bounds.GetWidth());
}		

///////////////////////////////////////////////////////////////////////////
// Geodatasets
//
void SetValue(CMoFields& fields, LPCTSTR name, VARIANT& value)
{
	// Find the field
	CMoField field(fields.Item(COleVariant(name)));

	// Set the field value
	field.SetValue(value);
}

void SetValue(CMoFields& fields, LPCTSTR name, const LONG value)
{
	VARIANT v;
	v.vt = VT_I4;
	v.lVal = value;
	SetValue(fields, name, v);
}

void SetValue(CMoFields& fields, LPCTSTR name, const double value)
{
	VARIANT v;
	v.vt = VT_R8;
	v.dblVal = value;
	SetValue(fields, name, v);
}

void SetValue(CMoFields& fields, LPCTSTR name, const COleDateTime& value)
{
	VARIANT v;
	v.vt = VT_DATE;
	v.date = value.m_dt;
	SetValue(fields, name, v);
}

void SetValue(CMoFields& fields, LPCTSTR name, const LPCTSTR value)
{
	SetValue(fields, name, COleVariant(value));
}

void SetValue(CMoFields& fields, LPCTSTR name, const BOOL value)
{
	VARIANT v;
	v.vt = VT_BOOL;
	v.bVal = value;
	SetValue(fields, name, v);
}

void SetValue(CMoFields& fields, LPCTSTR name, const LPDISPATCH value)
{
	VARIANT v;
	v.vt = VT_DISPATCH;
	v.pdispVal = value;
	SetValue(fields, name, v);
}


///////////////////////////////////////////////////////////////////////////
// Printing and Export
//
static void S_AdjustWidthToRatio(CRect& r, const double ratio, const UINT align = DT_CENTER)
{
	int newWidth = (int)MULT(r.Height(), ratio);
	int offset;
	if (align & DT_CENTER)
		offset = (int)DIV((double)r.Width() - newWidth, 2);
	else if (align & DT_RIGHT)
		offset = r.Width() - newWidth;
	else
		offset = 0;
	r.left += offset;
	r.right = r.left + newWidth;
}

static void S_AdjustHeightToRatio(CRect& r, const double ratio, const UINT align = DT_VCENTER)
{
	int newHeight = (int)DIV(r.Width(), ratio);
	int offset;
	if (align & DT_VCENTER)
		offset = (int)DIV((double)r.Height() - newHeight, 2);
	else if (align & DT_BOTTOM)
		offset = r.Height() - newHeight;
	else 
		offset = 0;
	r.top += offset;
	r.bottom = r.top + newHeight;
}

//
// Adjust the rectangle to match the aspect ratio specified
// Use the following values or'd together for alignment:
//
//		DT_TOP,  DT_VCENTER, DT_BOTTOM
//		DT_LEFT, DT_CENTER,  DT_RIGHT
//
CRect AdjustToRatio(const CRect& r, const double ratio, const UINT align)
{
	CRect newRect = r;

	// First try simple adjustment
	if (ratio < 1.0)
		S_AdjustWidthToRatio(newRect, ratio, align);
	else
		S_AdjustHeightToRatio(newRect, ratio, align);

	// Didn't fit.  Adjust again.
	if (r.Width() < newRect.Width())
	{
		newRect.left = r.left;
		newRect.right = r.right;
		S_AdjustHeightToRatio(newRect, ratio, align);
	}
	if (r.Height() < newRect.Height())
	{
		newRect.top = r.top;
		newRect.bottom = r.bottom;
		S_AdjustWidthToRatio(newRect, ratio, align);
	}

	return newRect;
}


//
// Calculate the size of the output device in pixels
//
CRect GetDeviceRect(CDC* pDC)
{
	CRect devRect;
//	HDC hDC = pDC->m_hAttribDC;
	HDC hDC = pDC->GetSafeHdc();

	int type = ::GetObjectType(hDC);
	switch (type)
	{
	case OBJ_DC:		// Printer or screen
		//
		// If there's a window associated with 
		// the DC, use the client area.  Otherwise
		// use the device width.
		//
		HWND hWnd;
		if (hWnd = ::WindowFromDC(hDC))
			::GetClientRect(hWnd, &devRect);
		else
			devRect.SetRect(0, 0, pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));
		break;
		
	case OBJ_MEMDC:		// Memory bitmap
		//
		// Use the bitmap size
		//
		HBITMAP hBitmap;
		hBitmap = (HBITMAP)::GetCurrentObject(pDC->GetSafeHdc(), OBJ_BITMAP);
		if (!hBitmap)
			throw "error with memory DC";
		BITMAP bm;
		VERIFY(::GetObject(hBitmap, sizeof(BITMAP), &bm));
		if (bm.bmWidth == 0 || bm.bmHeight == 0)
			throw "error with memory DC";;

		devRect.SetRect(0, 0, bm.bmWidth, bm.bmHeight);
		break;
		
	default: 
		devRect.SetRect(0, 0, pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));
		if (devRect.right == 0)
		{
			CClientDC dc(0);
			devRect.SetRect(0, 0, dc.GetDeviceCaps(HORZRES), dc.GetDeviceCaps(VERTRES));
		}
		break;
	}

	return devRect;
}

//
// Work-around that allows you to draw maps on an old style windows
// metafile.
//
void OutputMap(CMap1& map, CMetaFileDC* pDC)
{
	map.ExportMap(moExportClipboardEMF, TEXT("TRUE"), 1.0);	// system generates old-style metafile

	if (::OpenClipboard(::GetFocus()))
	{
		HANDLE hMFP = ::GetClipboardData(CF_METAFILEPICT);
		METAFILEPICT* pMFP = (METAFILEPICT*)::GlobalLock(hMFP);
		if (pMFP)
		{
			VERIFY(::PlayMetaFile(pDC->GetSafeHdc(), pMFP->hMF));
			::GlobalUnlock(hMFP);
		}

		VERIFY(::CloseClipboard());
	}
}

//
// Draw the map, scaling it to a frame on the surface of the 
// device referenced by the DC.  If the frame doesn't match the
// aspect ratio of the map, the map is position as specified 
// by hAlign and vAlign within the frame.  Use the following
// values or'd together for alignment:
//
//		DT_TOP,  DT_VCENTER, DT_BOTTOM
//		DT_LEFT, DT_CENTER,  DT_RIGHT
//
void FrameMap(CMap1& map, CDC* pDC, const CRect& dstRect, const UINT align)
{
	//
	// Create a frame of the same aspect ratio as the map and 
	// center or left justify it in dstRect.
	//
	double aspectRatio = GetAspectRatio(map);
	CRect frame = AdjustToRatio(dstRect, aspectRatio, align);

	//
	// Use GDI to position map on page
	//
	CRect devRect = AdjustToRatio(GetDeviceRect(pDC), aspectRatio);
	int saveID = pDC->SaveDC();

	pDC->SetMapMode(MM_ISOTROPIC); 
	pDC->SetWindowOrg(devRect.left, devRect.top); 
	pDC->SetWindowExt(devRect.Width(), devRect.Height()); 
	pDC->SetViewportOrg(frame.left, frame.top);
	pDC->SetViewportExt(frame.Width(), frame.Height());

	map.OutputMap((OLE_HANDLE)pDC->GetSafeHdc());

	pDC->RestoreDC(saveID);

#if 0
	// Rectangles for testing
	HBRUSH hBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(pDC->GetSafeHdc(), hBrush);
	pDC->Rectangle(dstRect);

	HPEN   hPen = (HPEN)::CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	HPEN   hOldPen = (HPEN)::SelectObject(pDC->GetSafeHdc(), hPen);
	pDC->Rectangle(frame);
	::SelectObject(pDC->GetSafeHdc(), hOldPen);

	::SelectObject(pDC->GetSafeHdc(), hOldBrush);
#endif
}


///////////////////////////////////////////////////////////////////////////
// OLE Fonts
//
CMoFont::CMoFont()
:	CFontHolder(0),
	m_pFontDisp(0),
	m_pDispatch(0)
{
	CFontDesc fontDesc;
	InitializeFont(&fontDesc, 0);
}


CMoFont::CMoFont(LPFONTDISP pFontDisp)
:	CFontHolder(0)
{
	InitializeFont(0, pFontDisp);	
		// similar to COleDispatchDriver::CreateDispatch()
}

CMoFont::~CMoFont()
{
	ReleaseDispatch();
}

void CMoFont::AttachDispatch(LPFONTDISP pFontDisp)
{
	ReleaseDispatch();
	InitializeFont(0, pFontDisp);
}

void CMoFont::ReleaseDispatch()
{
	if (m_pFontDisp)
	{
		m_pFontDisp->Release();
		m_pFontDisp = 0;
	}

	if (m_pDispatch)
	{
		m_pDispatch->Release();
		m_pDispatch = 0;
	}
}

CMoFont::operator LPFONTDISP() 
{ 
	if (m_pFontDisp == 0)
		m_pFontDisp = GetFontDispatch();	// must release;

	return m_pFontDisp; 
}

CMoFont::operator LPDISPATCH() 
{ 
	if (m_pDispatch == 0)
	{
		LPFONTDISP pFontDisp = this->operator LPFONTDISP();
		pFontDisp->QueryInterface(IID_IDispatch, (void**)&m_pDispatch);	// must release;
	}

	return m_pDispatch; 
}

LPCTSTR CMoFont::GetName()
{
	USES_CONVERSION;
	ASSERT(m_pFont);
	BSTR pName;	  
	m_pFont->get_Name(&pName);
	return OLE2T(pName);
}

void CMoFont::SetName(LPCTSTR name)
{
	USES_CONVERSION;
	ASSERT(m_pFont);
	m_pFont->put_Name(T2OLE(name));
}

long CMoFont::GetSize()
{
	ASSERT(m_pFont);
	CY size;
	m_pFont->get_Size(&size);
	return (long)DIV(size.int64, 10000);
}

void CMoFont::SetSize(long size)
{
	ASSERT(m_pFont);
	CY _size = { size * 10000, 0 };
	m_pFont->put_Size(_size);
}

short CMoFont::GetWeight()
{
	ASSERT(m_pFont);
	short weight;
	m_pFont->get_Weight(&weight);
	return weight;
}

void CMoFont::SetWeight(short weight)
{
	ASSERT(m_pFont);
	m_pFont->put_Weight(weight);
}

short CMoFont::GetCharSet()
{
	ASSERT(m_pFont);
	short charSet;
	m_pFont->get_Charset(&charSet);
	return charSet;
}

void CMoFont::SetCharSet(short charset)
{
	ASSERT(m_pFont);
	m_pFont->put_Charset(charset);
}

BOOL CMoFont::GetItalic()
{
	ASSERT(m_pFont);
	BOOL flag;
	m_pFont->get_Italic(&flag);
	return flag;
}

void CMoFont::SetItalic(BOOL italic)
{
	ASSERT(m_pFont);
	m_pFont->put_Italic(italic);
}

BOOL CMoFont::GetUnderline()
{
	ASSERT(m_pFont);
	BOOL flag;
	m_pFont->get_Underline(&flag);
	return flag;
}

void CMoFont::SetUnderline(BOOL underline)
{
	ASSERT(m_pFont);
	m_pFont->put_Underline(underline);
}

BOOL CMoFont::GetStrikeThrough()
{
	ASSERT(m_pFont);
	BOOL flag;
	m_pFont->get_Strikethrough(&flag);
	return flag;
}

void CMoFont::SetStrikeThrough(BOOL strikeThrough)
{
	ASSERT(m_pFont);
	m_pFont->put_Strikethrough(strikeThrough);
}



///////////////////////////////////////////////////////////////////////////
// Collection Iterator
//
CMoIterator::CMoIterator(COleDispatchDriver& collection)
: m_pEnumVariant(0)
{
	//
	// Find out if collection is really a collection
	// by querying for the _NewEnum property and 
	// assigning the m_pEnumVariant pointer to it.
	//
	USES_CONVERSION;
	LPCOLESTR	pName = T2COLE(TEXT("_NewEnum"));
	DISPID		dispID = -1;
	if (!SUCCEEDED(LPDISPATCH(collection)->GetIDsOfNames(IID_NULL, (OLECHAR**)&pName, 1, 0, &dispID)))
		throw "invalid collection object";
	
	LPUNKNOWN pUnknown = 0;
	collection.GetProperty(dispID, VT_UNKNOWN, &pUnknown);
	if (pUnknown == 0)
		throw "unable to get IUnknown for collection object";
	
	pUnknown->QueryInterface(IID_IEnumVARIANT, (void**)&m_pEnumVariant);
	pUnknown->Release();
	if (m_pEnumVariant == 0) 
		throw "unable to get IEnumVARIANT for collection object";

	Reset();	// might not need this.
	// success, collection has _NewEnum property!
	
}

//
// Sets item to next element of collection
// 
BOOL CMoIterator::Next(COleDispatchDriver& item)
{
	HRESULT result = S_FALSE;
	ASSERT(m_pEnumVariant);
	if (m_pEnumVariant)
	{
		ULONG c;
		VARIANT v;
		result = m_pEnumVariant->Next(1, &v, &c);
		if (result == S_OK && c == 1)
		{
			// v.pdispVal->AddRef(); // You shouldn't need to AddRef anything
			item.AttachDispatch(v.pdispVal);
		}
	}
	
	return (result == S_OK);
}

void CMoIterator::Reset()
{
	ASSERT(m_pEnumVariant);
	if (m_pEnumVariant)
		m_pEnumVariant->Reset();
}

void CMoIterator::Skip(unsigned long index)
{
	ASSERT(m_pEnumVariant);
	if (m_pEnumVariant)
		m_pEnumVariant->Skip(index);
}

CMoIterator::~CMoIterator() 
{
	if (m_pEnumVariant)
		m_pEnumVariant->Release();
}



IUnknown* CreateCOMObject(const GUID& clsid, IUnknown* pOuter)
{
	USES_CONVERSION;
	IUnknown* pUnknown = 0;

	IClassFactory* pFactory = 0;
	HRESULT hr = CoGetClassObject(clsid, CLSCTX_ALL, 0, IID_IClassFactory, (void**)&pFactory);
	if (SUCCEEDED(hr))
	{
		hr = pFactory->CreateInstance(pOuter, IID_IUnknown, (void**)&pUnknown);
		pFactory->Release();
	}
			
	return pUnknown;
}

