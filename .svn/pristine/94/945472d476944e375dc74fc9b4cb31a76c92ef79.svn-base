////////////////////////////////////////////////////////////////
// Copyright 1996 Microsoft Systems Journal.
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// See dib.cpp

//#ifndef _INC_VFW
//#include <vfw.h>
//#endif

// global functions for ordinary CBitmap too
//
extern CSize GetBitmapSize(CBitmap* pBitmap);
extern BOOL  DrawBitmap(CDC& dc, CBitmap* pBitmap,
	const CRect* rcDst=NULL, const CRect* rcSrc=NULL);

////////////////
// CDib implements Device Independent Bitmaps as a form of CBitmap.
//
class CDib : public CBitmap {
protected:
	DECLARE_DYNAMIC(CDib)
	BITMAP	m_bm;		// stored for speed
	CPalette m_pal;	// palette
	//HDRAWDIB m_hdd;	// for DrawDib

public:
	CDib();
	~CDib();

	CSize	GetSize() { return CSize(m_bm.bmWidth, m_bm.bmHeight); }
	BOOL Attach(HGDIOBJ hbm);

	BOOL Load(LPCTSTR szPathName);
	BOOL Load(HINSTANCE hInst, LPCTSTR lpResourceName);
	BOOL Load(HINSTANCE hInst, UINT uID)
		{ return Load(hInst, MAKEINTRESOURCE(uID)); }
	BOOL LoadBitmap(LPCTSTR lpszPathName);
	BOOL LoadMappedBitmap(LPCTSTR lpszPathName);

	// Universal Draw function can use DrawDib or not.
   // cannot use DrawDib anymore
	BOOL Draw(CDC& dc, const CRect* rcDst=NULL, const CRect* rcSrc=NULL,
		BOOL bUseDrawDib=TRUE, CPalette* pPal=NULL, BOOL bForeground=FALSE);

	BOOL DeleteObject();
	BOOL CreatePalette(CPalette& pal);
	CPalette* GetPalette()  { return &m_pal; }

	UINT GetColorTable(RGBQUAD* colorTab, UINT nColors);
};
