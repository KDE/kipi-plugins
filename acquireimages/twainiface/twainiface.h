/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-27-10
 * Description : Twain interface
 *
 * Copyright (C) 2002-2003 Stephan Stapel <stephan dot stapel at web dot de>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef TWAINIFACE_H
#define TWAINIFACE_H

// Windows includes.

#include <windows.h>

// Local includes.

#include "twain.h"

#define TWCPP_ANYCOUNT   (-1)
#define TWCPP_CANCELTHIS (1)
#define TWCPP_CANCELALL  (2)
#define TWCPP_DOTRANSFER (0)

class TwainIface
{

public:

    TwainIface(HWND hWnd=NULL);
    virtual ~TwainIface();

    bool InitTwain(HWND hWnd);
    void ReleaseTwain();

    /**
      This routine must be implemented by the derived class 
      After setting the required values in the m_AppId structure,
      the derived class should call the parent class implementation
      Refer Pg: 51 of the Twain Specification version 1.8
    */
    virtual void GetIdentity();
    virtual bool SelectSource();
    virtual bool OpenSource(TW_IDENTITY *pSource=NULL);
    virtual int  ShouldTransfer(TW_IMAGEINFO& info) { return TWCPP_DOTRANSFER;};

    bool ProcessMessage(MSG msg);

    bool SelectDefaultSource();
    bool IsValidDriver() const;
    bool SourceSelected() const {return m_bSourceSelected;} ;
    bool DSMOpen() const;
    bool DSOpen() const;
    bool SourceEnabled() const  { return m_bSourceEnabled;};
    bool ModalUI() const        { return m_bModalUI; };

    TW_INT16 GetRC() const      { return m_returnCode; };
    TW_STATUS GetStatus() const { return m_Status; };

    bool SetImageCount(TW_INT16 nCount = 1);
    bool Acquire(int numImages=1);

protected:

    bool CallTwainProc(pTW_IDENTITY pOrigin, pTW_IDENTITY pDest,
                       TW_UINT32 DG, TW_UINT16 DAT, TW_UINT16 MSG,
                       TW_MEMREF pData);

    void CloseDSM();
    void CloseDS();

    bool GetCapability(TW_CAPABILITY& twCap, TW_UINT16 cap, TW_UINT16 conType=TWON_DONTCARE16);
    bool GetCapability(TW_UINT16 cap, TW_UINT32& value);
    bool SetCapability(TW_UINT16 cap, TW_UINT16 value, bool sign=false);
    bool SetCapability(TW_CAPABILITY& twCap);
    bool EnableSource(bool showUI=true);

    bool GetImageInfo(TW_IMAGEINFO& info);

    virtual bool DisableSource();
    virtual bool CanClose() { return true; };

    void TranslateMessage(TW_EVENT& twEvent);
    void TransferImage();
    bool EndTransfer();
    void CancelTransfer();
    bool ShouldContinue();
    bool GetImage(TW_IMAGEINFO& info);

    virtual void CopyImage(HANDLE hBitmap, TW_IMAGEINFO& info)=0;

protected:

    bool         m_bSourceSelected;
    bool         m_bDSMOpen;
    bool         m_bDSOpen;
    bool         m_bSourceEnabled;
    bool         m_bModalUI;

    int          m_nImageCount;

    HINSTANCE    m_hTwainDLL;
    DSMENTRYPROC m_pDSMProc;

    TW_IDENTITY  m_AppId;
    TW_IDENTITY  m_Source;
    TW_STATUS    m_Status;
    TW_INT16     m_returnCode;
    HWND         m_hMessageWnd;
};

#endif /* TWAINIFACE_H */
