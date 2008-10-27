#ifndef _DIB_H_
#define _DIB_H_


#include "twain.h"


class CDIB 
{
public:
	CDIB(HANDLE hDib=NULL,int nBits=8);
	virtual ~CDIB();
	CDIB& operator=(CDIB& dib);
	bool IsValid() { return (m_pVoid && Width() && Height());};
	void UseGamma(float fg,bool bUse=true);
	bool CreateFromHandle(HANDLE hDib,int nBits);
	bool Create(int width,int height,int bits=24);
	bool Create(BITMAPINFOHEADER& bmInfo);
	bool CopyDIB(CDIB& dib);	
	void ReplaceColor(unsigned char oldColor,unsigned char newColor);
	HANDLE GetDIBits(int nStartX=-1,int nStartY=-1,int nCx=-1,int nCy=-1);
	DWORD GetDIBSize();
	int GetPaletteSize(BITMAPINFOHEADER& bmInfo);
	int GetPaletteSize();
	int CountColors();
	int EnumColors(BYTE *colors);
	void InitDIB(COLORREF color);
	void CopyLine(int source,int dest);
	void DestroyDIB();
	void SetPalette(unsigned char *palette);
	void SetPalette(RGBQUAD *pRGB);
	COLORREF PaletteColor(int index);
	void SetPixel(int x,int y,COLORREF color);
	void SetPixel8(int x,int y,unsigned char color);
	COLORREF GetPixel(int x,int y);
	void GetPixel(UINT x,UINT y,int& pixel);
	void BitBlt(HDC hDest,int nXDest,int nYDest,int nWidth,int nHeight,int xSrc,int ySrc);
	void BitBlt(int nXDest,int nYDest,int nWidth,int nHeight,CDIB& dibSrc,int nSrcX,int nSrcY,BYTE *colors=NULL);
	void StretchBlt(HDC hDest,int nXDest,int nYDest,int nDWidth,int nDHeight,int xSrc,int ySrc,int  nSWidth,int nSHeight);
	void StretchBlt(int nXDest,int nYDest,int nDWidth,int nDHeight,CDIB& dibSrc,int xSrc,int ySrc,int  nSWidth,int nSHeight);
	void ExpandBlt(int nXDest,int nYDest,int xRatio,int yRatio,CDIB& dibSrc,int xSrc,int ySrc,int  nSWidth,int nSHeight);
	void SetFlags(int flag) { m_nFlags = flag; };
	int Height() { return height ; };
	int Width() { return width ; };
	unsigned char *GetLinePtr(int line);	
	inline int GetBitCount() {return m_pInfo->bmiHeader.biBitCount;};
	bool Make8Bit(CDIB& dib);
	bool SwitchFromOne(CDIB& dib);
	bool SwitchFromFour(CDIB& dib);
	bool SwitchFrom24(CDIB& dib);
	bool SwitcHPALETTE(CDIB& dib);
	int  ClosestColor(RGBQUAD *pRgb);
	LPBITMAPINFO GetBitmapInfo() { return m_pInfo;};
	static unsigned int Distance(RGBQUAD& rgb1,RGBQUAD& rgb2);
	HANDLE DIBHandle();	
	
protected :
	
	void CreateGammaCurve();
	void Expand(int nXDest,int nYDest,int xRatio,int yRatio,CDIB& dibSrc,int xSrc,int ySrc,int  nSWidth,int nSHeight);
	unsigned char *m_pBits;
	PBITMAPINFO m_pInfo;
	RGBQUAD *m_pRGB;
	void *m_pVoid; 
	BYTE **m_pLinePtr;
	int height,bytes,width,m_nBits;
	int m_nFlags;
	bool m_bUseGamma;
	float m_fGamma;
	float m_fOldGamma;
	unsigned char Gamma[256];
	RGBQUAD CacheQuad[256];
	char CachePtr[256];
};

#endif // !_DIB_H_
