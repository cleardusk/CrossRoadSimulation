
// VS2013_MO_DEMODoc.cpp : implementation of the CVS2013_MO_DEMODoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "TRANS_SIMU.h"
#endif

#include "TRANS_SIMU_DOC.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVS2013_MO_DEMODoc

IMPLEMENT_DYNCREATE(CTRANS_SIMU_DOC, CDocument)

BEGIN_MESSAGE_MAP(CTRANS_SIMU_DOC, CDocument)
END_MESSAGE_MAP()


// CVS2013_MO_DEMODoc construction/destruction

CTRANS_SIMU_DOC::CTRANS_SIMU_DOC()
{
	// TODO: add one-time construction code here

}

CTRANS_SIMU_DOC::~CTRANS_SIMU_DOC()
{
}

BOOL CTRANS_SIMU_DOC::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CVS2013_MO_DEMODoc serialization

void CTRANS_SIMU_DOC::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CTRANS_SIMU_DOC::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CTRANS_SIMU_DOC::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CTRANS_SIMU_DOC::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CVS2013_MO_DEMODoc diagnostics

#ifdef _DEBUG
void CTRANS_SIMU_DOC::AssertValid() const
{
	CDocument::AssertValid();
}

void CTRANS_SIMU_DOC::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CVS2013_MO_DEMODoc commands
