/*
 *  dibutil.c
 *
 *  Source file for Device-Independent Bitmap (DIB) API.  Provides
 *  the following functions:
 *
 *  FindDIBBits()       - Sets pointer to the DIB bits
 *  DIBWidth()          - Gets the width of the DIB
 *  DIBHeight()         - Gets the height of the DIB
 *  PaletteSize()       - Calculates the buffer size required by a palette
 *  DIBNumColors()      - Calculates number of colors in the DIB's color table
 *  CreateDIBPalette()  - Creates a palette from a DIB
 *  DIBToBitmap()       - Creates a bitmap from a DIB
 *  BitmapToDIB()       - Creates a DIB from a bitmap
 *
 * Development Team: Mark Bader
 *                   Patrick Schreiber
 *                   Garrett Mcauliffe
 *                   Eric Flo
 *                   Tony Claflin
 *
 * Written by Microsoft Product Support Services, Developer Support.
 * Copyright (c) 1991 Microsoft Corporation. All rights reserved.
 */

/* header files */
#include <windows.h>
#include <assert.h>
#include "dibutil.h"


/*************************************************************************
 *
 * FindDIBBits()
 *
 * Parameter:
 *
 * LPSTR lpbi       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * LPSTR            - pointer to the DIB bits
 *
 * Description:
 *
 * This function calculates the address of the DIB's bits and returns a
 * pointer to the DIB bits.
 *
 ************************************************************************/


LPSTR FindDIBBits(LPSTR lpbi)
{
	return (lpbi + (WORD)*(LPDWORD)lpbi + PaletteSize(lpbi));
}


/*************************************************************************
 *
 * DIBWidth()
 *
 * Parameter:
 *
 * LPSTR lpbi       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * DWORD            - width of the DIB
 *
 * Description:
 *
 * This function gets the width of the DIB from the BITMAPINFOHEADER
 * width field if it is a Windows 3.0-style DIB or from the BITMAPCOREHEADER
 * width field if it is an OS/2-style DIB.
 *
 ************************************************************************/


DWORD DIBWidth(LPSTR lpDIB)
{
   LPBITMAPINFOHEADER lpbmi;  // pointer to a Win 3.0-style DIB
   LPBITMAPCOREHEADER lpbmc;  // pointer to an OS/2-style DIB

   /* point to the header (whether Win 3.0 and OS/2) */

   lpbmi = (LPBITMAPINFOHEADER)lpDIB;
   lpbmc = (LPBITMAPCOREHEADER)lpDIB;

   if (lpbmi->biSize == sizeof(BITMAPINFOHEADER))
      return lpbmi->biWidth;
   else  /* it is an OS/2 DIB, so return its width */
      return (DWORD)lpbmc->bcWidth;
}


/*************************************************************************
 *
 * DIBHeight()
 *
 * Parameter:
 *
 * LPSTR lpbi       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * DWORD            - height of the DIB
 *
 * Description:
 *
 * This function gets the height of the DIB from the BITMAPINFOHEADER
 * height field if it is a Windows 3.0-style DIB or from the BITMAPCOREHEADER
 * height field if it is an OS/2-style DIB.
 *
 ************************************************************************/


DWORD DIBHeight(LPSTR lpDIB)
{
   LPBITMAPINFOHEADER lpbmi;  // pointer to a Win 3.0-style DIB
   LPBITMAPCOREHEADER lpbmc;  // pointer to an OS/2-style DIB

   lpbmi = (LPBITMAPINFOHEADER)lpDIB;
   lpbmc = (LPBITMAPCOREHEADER)lpDIB;

   if (lpbmi->biSize == sizeof(BITMAPINFOHEADER))
      return lpbmi->biHeight;
   else
      return (DWORD)lpbmc->bcHeight;
}


/*************************************************************************
 *
 * PaletteSize()
 *
 * Parameter:
 *
 * LPSTR lpbi       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * WORD             - size of the color palette of the DIB
 *
 * Description:
 *
 * This function gets the size required to store the DIB's palette by
 * multiplying the number of colors by the size of an RGBQUAD (for a
 * Windows 3.0-style DIB) or by the size of an RGBTRIPLE (for an OS/2-
 * style DIB).
 *
 ************************************************************************/


DWORD PaletteSize(LPSTR lpbi)
{
   /* calculate the size required by the palette */
   if (IS_WIN30_DIB (lpbi))
      return (DIBNumColors(lpbi) * sizeof(RGBQUAD));
   else
	  return (DIBNumColors(lpbi) * sizeof(RGBTRIPLE));
}


/*************************************************************************
 *
 * DIBNumColors()
 *
 * Parameter:
 *
 * LPSTR lpbi       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * WORD             - number of colors in the color table
 *
 * Description:
 *
 * This function calculates the number of colors in the DIB's color table
 * by finding the bits per pixel for the DIB (whether Win3.0 or OS/2-style
 * DIB). If bits per pixel is 1: colors=2, if 4: colors=16, if 8: colors=256,
 * if 24, no colors in color table.
 *
 ************************************************************************/


WORD DIBNumColors(LPSTR lpbi)
{
   WORD wBitCount;  // DIB bit count

   if (IS_WIN30_DIB(lpbi))
   {
      DWORD dwClrUsed;

      dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;
      if (dwClrUsed)
         return (WORD)dwClrUsed;
   }

   if (IS_WIN30_DIB(lpbi))
      wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
   else
      wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;

   switch (wBitCount)
      {
   case 1:
      return 2;

   case 4:
      return 16;

   case 8:
      return 256;

   default:
      return 0;
      }
}


/*************************************************************************
 *
 * CreateDIBPalette()
 *
 * Parameter:
 *
 * HDIB hDIB        - specifies the DIB
 *
 * Return Value:
 *
 * HPALETTE         - specifies the palette
 *
 * Description:
 *
 * This function creates a palette from a DIB by allocating memory for the
 * logical palette, reading and storing the colors from the DIB's color table
 * into the logical palette, creating a palette from this logical palette,
 * and then returning the palette's handle. This allows the DIB to be
 * displayed using the best possible colors (important for DIBs with 256 or
 * more colors).
 *
 ************************************************************************/


HPALETTE CreateDIBPalette(HDIB hDIB)
{
   LPLOGPALETTE lpPal;      // pointer to a logical palette
   HANDLE hLogPal;          // handle to a logical palette
   HPALETTE hPal = NULL;    // handle to a palette
   int i, wNumColors;       // loop index, number of colors in color table
   LPSTR lpbi;              // pointer to packed-DIB
   LPBITMAPINFO lpbmi;      // pointer to BITMAPINFO structure (Win3.0)
   LPBITMAPCOREINFO lpbmc;  // pointer to BITMAPCOREINFO structure (OS/2)
   BOOL bWinStyleDIB;       // flag which signifies whether this is a Win3.0 DIB

   if (!hDIB)
      return NULL;

   lpbi = GlobalLock(hDIB);

   lpbmi = (LPBITMAPINFO)lpbi;

   lpbmc = (LPBITMAPCOREINFO)lpbi;

   wNumColors = DIBNumColors(lpbi);

   bWinStyleDIB = IS_WIN30_DIB(lpbi);
   if (wNumColors)
   {
      hLogPal = GlobalAlloc(GHND, sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) *
                            wNumColors);

      if (!hLogPal)
      {
         GlobalUnlock(hDIB);
         return NULL;
      }

      lpPal = (LPLOGPALETTE)GlobalLock(hLogPal);

	  lpPal->palVersion = PALVERSION;
	  lpPal->palNumEntries = (WORD)wNumColors;

	  for (i = 0; i < wNumColors; i++)
	  {
		 if (bWinStyleDIB)
		 {
			lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i].rgbRed;
            lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
            lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;
            lpPal->palPalEntry[i].peFlags = 0;
         }
         else
         {
            lpPal->palPalEntry[i].peRed = lpbmc->bmciColors[i].rgbtRed;
            lpPal->palPalEntry[i].peGreen = lpbmc->bmciColors[i].rgbtGreen;
            lpPal->palPalEntry[i].peBlue = lpbmc->bmciColors[i].rgbtBlue;
            lpPal->palPalEntry[i].peFlags = 0;
         }
      }

      hPal = CreatePalette(lpPal);

      if (!hPal)
      {
         GlobalUnlock(hLogPal);
         GlobalFree(hLogPal);
         return NULL;
      }
   }

   GlobalUnlock(hLogPal);
   GlobalFree(hLogPal);
   GlobalUnlock(hDIB);

   return hPal;
}


/*************************************************************************
 *
 * DIBToBitmap()
 *
 * Parameters:
 *
 * HDIB hDIB        - specifies the DIB to convert
 *
 * HPALETTE hPal    - specifies the palette to use with the bitmap
 *
 * Return Value:
 *
 * HBITMAP          - identifies the device-dependent bitmap
 *
 * Description:
 *
 * This function creates a bitmap from a DIB using the specified palette.
 * If no palette is specified, default is used.
 *
 ************************************************************************/


HBITMAP DIBToBitmap(HDIB hDIB, HPALETTE hPal)
{
   LPSTR lpDIBHdr, lpDIBBits;  // pointer to DIB header, pointer to DIB bits
   HBITMAP hBitmap;            // handle to device-dependent bitmap
   HDC hDC;                    // handle to DC
   HPALETTE hOldPal = NULL;      // handle to a palette

   if (!hDIB)
      return NULL;

   lpDIBHdr = GlobalLock(hDIB);

   lpDIBBits = FindDIBBits(lpDIBHdr);

   hDC = GetDC(NULL);
   if (!hDC)
   {
      GlobalUnlock(hDIB);
      return NULL;
   }

   if (hPal)
      hOldPal = SelectPalette(hDC, hPal, FALSE);
   RealizePalette(hDC);

   hBitmap = CreateDIBitmap(hDC, (LPBITMAPINFOHEADER)lpDIBHdr, CBM_INIT,
                            lpDIBBits, (LPBITMAPINFO)lpDIBHdr, DIB_RGB_COLORS);

   if (hOldPal)
      SelectPalette(hDC, hOldPal, FALSE);

   ReleaseDC(NULL, hDC);
   GlobalUnlock(hDIB);

   return hBitmap;
}


/*************************************************************************
 *
 * BitmapToDIB()
 *
 * Parameters:
 *
 * HBITMAP hBitmap  - specifies the bitmap to convert
 *
 * HPALETTE hPal    - specifies the palette to use with the bitmap
 *
 * Return Value:
 *
 * HDIB             - identifies the device-dependent bitmap
 *
 * Description:
 *
 * This function creates a DIB from a bitmap using the specified palette.
 *
 ************************************************************************/


HDIB BitmapToDIB(HBITMAP hBitmap, HPALETTE hPal)
{
   BITMAP bm;
   BITMAPINFOHEADER bi;
   BITMAPINFOHEADER FAR *lpbi;
   DWORD dwLen;
   HANDLE hDIB, h;
   HDC hDC;
   DWORD biBits;


   if (!hBitmap)
      return NULL;

   if (hPal == NULL)
      hPal = GetStockObject(DEFAULT_PALETTE);

   GetObject(hBitmap, sizeof(bm), (LPSTR)&bm);

   biBits = bm.bmPlanes * bm.bmBitsPixel;

   bi.biSize = sizeof(BITMAPINFOHEADER);
   bi.biWidth = bm.bmWidth;
   bi.biHeight = bm.bmHeight;
   bi.biPlanes = 1;
   bi.biBitCount = (WORD)biBits;
   bi.biCompression = DIB_RGB_COLORS;
   bi.biSizeImage = 0;
   bi.biXPelsPerMeter = 0;
   bi.biYPelsPerMeter = 0;
   bi.biClrUsed = 0;
   bi.biClrImportant = 0;

   dwLen = bi.biSize + PaletteSize((LPSTR)&bi);

   hDC = GetDC(NULL);

   hPal = SelectPalette(hDC, hPal, FALSE);
   RealizePalette(hDC);

   hDIB = GlobalAlloc(GHND, dwLen);

   if (!hDIB)
   {
      SelectPalette(hDC, hPal, TRUE);
      RealizePalette(hDC);
      ReleaseDC(NULL, hDC);
      return NULL;
   }

   lpbi = (VOID FAR *)GlobalLock(hDIB);

   *lpbi = bi;

   GetDIBits(hDC, hBitmap, 0, (WORD)bi.biHeight, NULL, (LPBITMAPINFO)lpbi,
             DIB_RGB_COLORS);

   bi = *lpbi;
   GlobalUnlock(hDIB);

   if (bi.biSizeImage == 0)
      bi.biSizeImage = WIDTHBYTES((DWORD)bm.bmWidth * biBits) * bm.bmHeight;

   dwLen = bi.biSize + PaletteSize((LPSTR)&bi) + bi.biSizeImage;
   if ((h = GlobalReAlloc(hDIB, dwLen, 0)) != 0)
      hDIB = h;
   else
   {
	  GlobalFree(hDIB);
	  SelectPalette(hDC, hPal, TRUE);
	  RealizePalette(hDC);
	  ReleaseDC(NULL, hDC);
	  return hDIB = NULL;
   }

   lpbi = (VOID FAR *)GlobalLock(hDIB);

   if (GetDIBits(hDC, hBitmap, 0, (WORD)bi.biHeight, (LPSTR)lpbi + (WORD)lpbi
                 ->biSize + PaletteSize((LPSTR)lpbi), (LPBITMAPINFO)lpbi,
                 DIB_RGB_COLORS) == 0)
   {
	  GlobalUnlock(hDIB);
	  SelectPalette(hDC, hPal, TRUE);
	  RealizePalette(hDC);
	  ReleaseDC(NULL, hDC);
	  return hDIB = NULL;
   }
   bi = *lpbi;

   GlobalUnlock(hDIB);
   SelectPalette(hDC, hPal, TRUE);
   RealizePalette(hDC);
   ReleaseDC(NULL, hDC);

   return hDIB;
}


/*************************************************************************
 *
 * PalEntriesOnDevice()
 *
 * Parameter:
 *
 * HDC hDC          - device context
 *
 * Return Value:
 *
 * int              - number of palette entries on device
 *
 * Description:
 *
 * This function gets the number of palette entries on the specified device
 *
 ************************************************************************/


int PalEntriesOnDevice(HDC hDC)
{
   int nColors;  // number of colors

   nColors = GetDeviceCaps(hDC, SIZEPALETTE);

   if (!nColors)
      nColors = GetDeviceCaps(hDC, NUMCOLORS);
   assert(nColors);
   return nColors;
}


/*************************************************************************
 *
 * DIBHeight()
 *
 * Parameter:
 *
 * LPSTR lpbi       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * DWORD            - height of the DIB
 *
 * Description:
 *
 * This function returns a handle to a palette which represents the system
 * palette (each entry is an offset into the system palette instead of an
 * RGB with a flag of PC_EXPLICIT).
 *
 ************************************************************************/


HPALETTE GetSystemPalette(void)
{
   HDC hDC;
   HPALETTE hPal;
   HANDLE hLogPal;
   LPLOGPALETTE lpLogPal;
   int i, nColors;         


   hDC = GetDC(NULL);
   if (!hDC)
      return NULL;
   nColors = PalEntriesOnDevice(hDC);
   ReleaseDC(NULL, hDC);

   hLogPal = GlobalAlloc(GHND, sizeof(LOGPALETTE) + nColors * sizeof(
                         PALETTEENTRY));

   if (!hLogPal)
      return NULL;

   lpLogPal = (LPLOGPALETTE)GlobalLock(hLogPal);

   lpLogPal->palVersion = PALVERSION;
   lpLogPal->palNumEntries = (WORD)nColors;
   for (i = 0; i < nColors; i++)
   {
	  lpLogPal->palPalEntry[i].peBlue = 0;
	  *((LPWORD)(&lpLogPal->palPalEntry[i].peRed)) = (WORD)i;
	  lpLogPal->palPalEntry[i].peFlags = PC_EXPLICIT;
   }

   hPal = CreatePalette(lpLogPal);

   GlobalUnlock(hLogPal);
   GlobalFree(hLogPal);
   return hPal;
}
