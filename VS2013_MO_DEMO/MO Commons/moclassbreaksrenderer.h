#if !defined(AFX_MOCLASSBREAKSRENDERER_H__BA727F04_DD0F_45F2_BB8B_4BEEBB65980B__INCLUDED_)
#define AFX_MOCLASSBREAKSRENDERER_H__BA727F04_DD0F_45F2_BB8B_4BEEBB65980B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


// Dispatch interfaces referenced by this interface
class CMoSymbol;

/////////////////////////////////////////////////////////////////////////////
// CMoClassBreaksRenderer wrapper class

class CMoClassBreaksRenderer : public COleDispatchDriver
{
public:
	CMoClassBreaksRenderer() {}		// Calls COleDispatchDriver default constructor
	CMoClassBreaksRenderer(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CMoClassBreaksRenderer(const CMoClassBreaksRenderer& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	CString GetField();
	void SetField(LPCTSTR);
	short GetBreakCount();
	void SetBreakCount(short);
	CString GetTag();
	void SetTag(LPCTSTR);
	long GetSymbolType();
	void SetSymbolType(long);
	BOOL GetDrawBackground();
	void SetDrawBackground(BOOL);

// Operations
public:
	double GetBreak(short index);
	void SetBreak(short index, double newValue);
	CMoSymbol GetSymbol(short index);
	void SetRefSymbol(short index, LPDISPATCH newValue);
	void RampColors(unsigned long startColor, unsigned long endColor);
	void SizeSymbols(short startSize, short endSize);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOCLASSBREAKSRENDERER_H__BA727F04_DD0F_45F2_BB8B_4BEEBB65980B__INCLUDED_)