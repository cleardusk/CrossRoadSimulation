#if !defined _MAP_HELPER_H
#define _MAP_HELPER_H
//////////////////////////////////////////////////////////////////////////
// MapHelper.h - Utility routines for working with the MapObjects control
// 
#include "mapobjects2.h"
///////////////////////////////////////////////////////////////////////////
// Interesting Macros
#include <afxctl.h>
#include <afxpriv.h>
//
// Perform precise integer division.  Cast the result to the type of
// integer that you need.
//
#define DIV(a, b)       (((a)*(b) < 0) ? (((double)(a) / (double)(b)) - 0.5)  \
                                       : (((double)(a) / (double)(b)) + 0.5))
#define MULT(a, b)      (((a)*(b) < 0) ? (((double)(a) * (double)(b)) - 0.5)  \
                                       : (((double)(a) * (double)(b)) + 0.5))
#define MULDIV(a, b, c) (((a)*(b)*(c) < 0) ?                                  \
                           (((double)(a) * (double)(b) / (double)(c)) - 0.5)  \
                         : (((double)(a) * (double)(b) / (double)(c)) + 0.5))

//
// Convert from radians to degrees and back
//
#define RAD2DEG(r) ((r)*57.2957795131)
#define DEG2RAD(d) ((d)/57.2957795131)


//
// Iterate through collections.  
//
//	ObjType	The type of a single object, i.e., "CMoMapLayer"
//	obj		The iterator variable.  It's attached to each object in the
//			collection in turn.
//	objs	A reference to the collection, i.e., "const CMoLayers layers".
//
// The following code demonstrates the use of the macros:
//
//	CMoLayers layers(m_map.GetLayers());
//	FOR_EACH_IN(CMoMapLayer, layer, layers)
//	  TRACE("%s\n", layer.GetName());
//	END_FOR
//
#define FOR_EACH_IN(ObjType, obj, objs)									\
	LPUNKNOWN pUnk = objs.Get_NewEnum();								\
	ASSERT(pUnk);														\
	IEnumVARIANT* pEnumVariant = 0;										\
	pUnk->QueryInterface(IID_IEnumVARIANT, (void**)&pEnumVariant);		\
	pUnk->Release();													\
	ULONG c;															\
	VARIANT v;															\
	ObjType obj;														\
	while (pEnumVariant->Next(1, &v, &c) == 0 && c == 1)				\
	{																	\
		obj.AttachDispatch(v.pdispVal);									

#define END_FOR															\
	}																	\
	pEnumVariant->Release();


///////////////////////////////////////////////////////////////////////////
// Utilities
//
CString GetFileDirectory(const CString& path);
CString GetFileName(const CString& path);
CString GetFileExt(const CString& path);
CString GetFileTitle(const CString& path);


///////////////////////////////////////////////////////////////////////////
// Conversion
//
double DegreesToMiles(double x1r, double y1, double x2, double y2);

///////////////////////////////////////////////////////////////////////////
// Miscellaneous
//
// GetIDispatch - get the IDispatch pointer for the map wrapper.
//
// GetAspectRatio - return the map's aspect ratio, i.e., width/height.
//
// Coordinate Transformation - Use these if MapObjects doesn't return
// accurate results in the container you're using.
//
LPDISPATCH GetIDispatch(CMap1& map);
double GetAspectRatio(CMap1& map);
LPDISPATCH ToMapPoint(CMap1& map, long x, long y);
void FromMapPoint(CMap1& map, CMoPoint& pt, long& x, long& y);
double ToMapDistance(CMap1& map, long x);
long FromMapDistance(CMap1& map, double& mapX);

///////////////////////////////////////////////////////////////////////////
// Layer management
//
void AddLayer(CMap1& map);
void AddLayer(CMap1& map, const CString& path, COLORREF color = -1, LPDISPATCH renderer = 0);
void AddLabelLayer(CMap1& map, const CString& path, LPCTSTR labelField, COLORREF color);
void AddImageLayer(CMap1& map, const CString& path);
short GetLayerType(CMoLayers& layers, short layerNum);

LPDISPATCH CreateStreetRenderer();


///////////////////////////////////////////////////////////////////////////
// Zoom and Pan
//
void ZoomIn(CMap1& map);
void ZoomOut(CMap1& map);
void ZoomFull(CMap1& map);
void Pan(CMap1& map);
void Resize(CMap1& map, int cx, int cy, int border = 0); // use with OnSize


///////////////////////////////////////////////////////////////////////////
// GeoDatasets
//
void SetValue(CMoFields& fields, LPCTSTR name, const LONG value);
void SetValue(CMoFields& fields, LPCTSTR name, const double value);
void SetValue(CMoFields& fields, LPCTSTR name, const COleDateTime& value);
void SetValue(CMoFields& fields, LPCTSTR name, const LPCTSTR value);
void SetValue(CMoFields& fields, LPCTSTR name, const BOOL value);
void SetValue(CMoFields& fields, LPCTSTR name, const LPDISPATCH value);

///////////////////////////////////////////////////////////////////////////
// Printing and Export
//
// AdjustToRatio - Adjust the rectangle so that it has the same aspect
// ratio as specified.
//
// GetDeviceRect - Get the bounds of the specified device.
//
// OutputMap - Export map to old style Windows metafile.  Not supported 
// directly by MapObjects 1.0.
//
// FrameMap - Draw map to specified frame rectangle on the specified device.
//
CRect	AdjustToRatio(const CRect& r, const double ratio, const UINT align = DT_CENTER|DT_VCENTER);
CRect	GetDeviceRect(CDC* pDC);
void	OutputMap(CMap1& map, CMetaFileDC* pDC);
void	FrameMap(CMap1& map, CDC* pDC, const CRect& dstRect, const UINT align = DT_VCENTER|DT_CENTER);


///////////////////////////////////////////////////////////////////////////
// OLE Fonts
//
//
// FONTDESC wrapper.  Automatic initialization
//
class CFontDesc : public tagFONTDESC
{
public:
	CFontDesc() 
	{
		cbSizeofstruct = sizeof(FONTDESC);
		lpstrName = OLESTR("Arial");
//		cySize = FONTSIZE(12);
		cySize.int64 = 120000;
		sWeight = FW_NORMAL;
		sCharset = ANSI_CHARSET;
		fItalic = FALSE;
		fUnderline = FALSE;
		fStrikethrough = FALSE;
	}
};

//
// Wrapper for IFonts.  Similar to COleDispatchDriver except that
// CreateDispatch isn't needed.
//
class CMoFont : public CFontHolder
{
public:
	CMoFont();
	CMoFont(LPFONTDISP pFontDisp);
	virtual ~CMoFont();

	void AttachDispatch(LPFONTDISP pFontDisp);

	// Attributes
	LPCTSTR GetName();
	void	SetName(LPCTSTR faceName);
	long	GetSize();
	void	SetSize(long size);
	short	GetWeight();
	void	SetWeight(short weight);
	short	GetCharSet();
	void	SetCharSet(short charset);
	BOOL	GetItalic();
	void	SetItalic(BOOL italic);
	BOOL	GetUnderline();
	void	SetUnderline(BOOL underline);
	BOOL	GetStrikeThrough();
	void	SetStrikeThrough(BOOL strikeThrough);

	operator LPDISPATCH();
	operator LPFONTDISP();

private:
	LPFONTDISP	m_pDispatch;
	LPFONTDISP	m_pFontDisp;

	void		ReleaseDispatch();	// Call whenever InitializeFont is called
};


///////////////////////////////////////////////////////////////////////////
// Data Directory
//
// CDataDir is used to point to the sample data.  It defaults to 
// "..\..\data".
// 
//
class CDataDir
{
public:
	CDataDir();
	virtual ~CDataDir() {}
	CString GetPath();
	void	SetPath(LPCTSTR newPath);

protected:
	CString m_path;
};


///////////////////////////////////////////////////////////////////////////
// Collection iterator
//
class CMoIterator
{
public:
	CMoIterator(COleDispatchDriver& collection);
	~CMoIterator();
	BOOL Next(COleDispatchDriver& item);
	void Reset();
	void Skip(unsigned long numToSkip);

private:
	CMoIterator() {}			// must initialize with collection

	IEnumVARIANT* m_pEnumVariant;

};

IUnknown* CreateCOMObject(const GUID& clsid, IUnknown* pOuter = 0);

#endif

