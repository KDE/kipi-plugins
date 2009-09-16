/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-27-10
 * Description : Twain interface
 *
 * Copyright (C) 2002-2003 Stephan Stapel <stephan dot stapel at web dot de>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "twainiface.h"

// Qt includes

#include <QtGlobal>

// C++ includes

#include <cstring>

/** Constructor parameters : HWND: Window to subclass
 */
TwainIface::TwainIface(HWND hWnd)
{
    m_hMessageWnd     = 0;
    m_hTwainDLL       = NULL;
    m_pDSMProc        = NULL;
    m_bSourceSelected = false;
    m_bDSOpen         = false;
    m_bDSMOpen        = false;
    m_bSourceEnabled  = false;
    m_bModalUI        = true;
    m_nImageCount     = TWCPP_ANYCOUNT;
    if(hWnd)
    {
        InitTwain(hWnd);
    }
}

TwainIface::~TwainIface()
{
    ReleaseTwain();
}

/** Initializes TWAIN interface . Is already called from the constructor.
    It should be called again if ReleaseTwain is called.
    hWnd is the window which has to subclassed in order to receive
    Twain messaged. Normally - this would be your main application window.
 */
bool TwainIface::InitTwain(HWND hWnd)
{
    char libName[512];

    if(IsValidDriver())
    {
        return true;
    }

    memset(&m_AppId,0,sizeof(m_AppId));

    if(!IsWindow(hWnd))
    {
        return false;
    }

    m_hMessageWnd = hWnd;
    strcpy(libName, "TWAIN_32.DLL");

    m_hTwainDLL = LoadLibraryA(libName);
    if(m_hTwainDLL != NULL)
    {
        if(!(m_pDSMProc = (DSMENTRYPROC)GetProcAddress(m_hTwainDLL, (LPCSTR)MAKEINTRESOURCE(1))))
        {
            FreeLibrary(m_hTwainDLL);
            m_hTwainDLL = NULL;
        }
    }

    if(IsValidDriver())
    {
        GetIdentity();
        m_bDSMOpen= CallTwainProc(&m_AppId, NULL, DG_CONTROL,
                                  DAT_PARENT, MSG_OPENDSM, (TW_MEMREF)&m_hMessageWnd);
        return true;
    }
    else
    {
        return false;
    }
}

/** Releases the twain interface . Need not be called unless you
    want to specifically shut it down.
 */
void TwainIface::ReleaseTwain()
{
    if(IsValidDriver())
    {
        CloseDSM();
        FreeLibrary(m_hTwainDLL);
        m_hTwainDLL = NULL;
        m_pDSMProc  = NULL;
    }
}

/** Returns true if a valid driver has been loaded
 */
bool TwainIface::IsValidDriver() const
{
    return (m_hTwainDLL && m_pDSMProc);
}

/** Entry point into Twain. For a complete description of this
    routine  please refer to the Twain specification 1.8
 */
bool TwainIface::CallTwainProc(pTW_IDENTITY pOrigin, pTW_IDENTITY pDest,
                               TW_UINT32 DG, TW_UINT16 DAT, TW_UINT16 MSG, TW_MEMREF pData)
{
    if(IsValidDriver())
    {
        USHORT ret_val;
        ret_val      = (*m_pDSMProc)(pOrigin, pDest, DG, DAT, MSG, pData);
        m_returnCode = ret_val;

        if(ret_val == TWRC_FAILURE)
        {
            (*m_pDSMProc)(pOrigin, pDest, DG_CONTROL, DAT_STATUS, MSG_GET, &m_Status);
        }
        return (ret_val == TWRC_SUCCESS);
    }
    else
    {
        m_returnCode = TWRC_FAILURE;
        return false;
    }
}

/** This function should ideally be overridden in the derived class . If only a
    few fields need to be updated , call TwainIface::GetIdentity first in your
    derived class
 */
void TwainIface::GetIdentity()
{
    // Expects all the fields in m_AppId to be set except for the id field.
    m_AppId.Id               = 0; // Initialize to 0 (Source Manager will assign real value)
    m_AppId.Version.MajorNum = 2; // Your app's version number
    m_AppId.Version.MinorNum = 0;
    m_AppId.Version.Language = TWLG_USA;
    m_AppId.Version.Country  = TWCY_USA;
    strcpy(m_AppId.Version.Info, "AcquireImage kipi plugin");

    m_AppId.ProtocolMajor    = TWON_PROTOCOLMAJOR;
    m_AppId.ProtocolMinor    = TWON_PROTOCOLMINOR;
    m_AppId.SupportedGroups  = DG_IMAGE | DG_CONTROL;
    strcpy(m_AppId.Manufacturer,  "KDE");
    strcpy(m_AppId.ProductFamily, "Generic");
    strcpy(m_AppId.ProductName,   "AcquireImage");
}

/** Called to display a dialog box to select the Twain source to use.
    This can be overridden if a list of all sources is available
    to the application. These sources can be enumerated by Twain.
    it is not yet supported by TwainIface.
 */
bool TwainIface::SelectSource()
{
    memset(&m_Source, 0, sizeof(m_Source));

    if(!SourceSelected())
    {
        SelectDefaultSource();
    }

    if(CallTwainProc(&m_AppId, NULL, DG_CONTROL,
                     DAT_IDENTITY, MSG_USERSELECT, &m_Source))
    {
        m_bSourceSelected = true;
    }

    return m_bSourceSelected;
}

/** Called to select the default source
 */
bool TwainIface::SelectDefaultSource()
{
    m_bSourceSelected = CallTwainProc(&m_AppId, NULL, DG_CONTROL,
                                      DAT_IDENTITY, MSG_GETDEFAULT, &m_Source);
    return m_bSourceSelected;
}

/** Closes the Data Source
 */
void TwainIface::CloseDS()
{
    if(DSOpen())
    {
        DisableSource();
        CallTwainProc(&m_AppId, NULL, DG_CONTROL,
                      DAT_IDENTITY, MSG_CLOSEDS, (TW_MEMREF)&m_Source);
        m_bDSOpen = false;
    }
}

/** Closes the Data Source Manager
 */
void TwainIface::CloseDSM()
{
    if(DSMOpen())
    {
        CloseDS();
        CallTwainProc(&m_AppId, NULL, DG_CONTROL,
                      DAT_PARENT, MSG_CLOSEDSM, (TW_MEMREF)&m_hMessageWnd);
        m_bDSMOpen = false;
    }
}

/** Returns true if the Data Source Manager is Open
 */
bool TwainIface::DSMOpen() const
{
    return IsValidDriver() && m_bDSMOpen;
}

/** Returns true if the Data Source is Open
 */
bool TwainIface::DSOpen() const
{
    return IsValidDriver() && DSMOpen() && m_bDSOpen;
}

/** Opens a Data Source supplied as the input parameter
 */
bool TwainIface::OpenSource(TW_IDENTITY *pSource)
{
    if(pSource)
    {
        m_Source = *pSource;
    }

    if(DSMOpen())
    {
        if(!SourceSelected())
        {
            SelectDefaultSource();
        }
        m_bDSOpen = CallTwainProc(&m_AppId, NULL, DG_CONTROL,
                                  DAT_IDENTITY, MSG_OPENDS, (TW_MEMREF)&m_Source);
    }

    return DSOpen();
}

/** Should be called from the main message loop of the application. Can always be called,
    it will not process the message unless a scan is in progress.
 */
bool TwainIface::ProcessMessage(MSG msg)
{
    if(SourceEnabled())
    {
        TW_UINT16  twRC = TWRC_NOTDSEVENT;

        TW_EVENT twEvent;
        twEvent.pEvent = (TW_MEMREF)&msg;
        //memset(&twEvent, 0, sizeof(TW_EVENT));

        twEvent.TWMessage = MSG_NULL;

        CallTwainProc(&m_AppId, &m_Source, DG_CONTROL,
                      DAT_EVENT, MSG_PROCESSEVENT, (TW_MEMREF)&twEvent);

        if(GetRC() != TWRC_NOTDSEVENT)
        {
            TranslateMessage(twEvent);
        }

        return (twRC==TWRC_DSEVENT);

    }
    return false;
}

/** Queries the capability of the Twain Data Source
 */
bool TwainIface::GetCapability(TW_CAPABILITY& twCap, TW_UINT16 cap, TW_UINT16 conType)
{
    if(DSOpen())
    {
        twCap.Cap        = cap;
        twCap.ConType    = conType;
        twCap.hContainer = NULL;

        if(CallTwainProc(&m_AppId, &m_Source, DG_CONTROL,
                         DAT_CAPABILITY, MSG_GET, (TW_MEMREF)&twCap))
        {
            return true;
        }
    }
    return false;
}

/** Queries the capability of the Twain Data Source
 */
bool TwainIface::GetCapability(TW_UINT16 cap, TW_UINT32& value)
{
    TW_CAPABILITY twCap;
    if(GetCapability(twCap, cap))
    {
        pTW_ONEVALUE pVal;
        pVal = (pTW_ONEVALUE )GlobalLock(twCap.hContainer);

        if(pVal)
        {
            value = pVal->Item;
            GlobalUnlock(pVal);
            GlobalFree(twCap.hContainer);
            return true;
        }
    }
    return false;
}

/** Sets the capability of the Twain Data Source
 */
bool TwainIface::SetCapability(TW_UINT16 cap, TW_UINT16 value, bool sign)
{
    if(DSOpen())
    {
        TW_CAPABILITY twCap;
        pTW_ONEVALUE pVal;
        bool ret_value   = false;
        twCap.Cap        = cap;
        twCap.ConType    = TWON_ONEVALUE;
        twCap.hContainer = GlobalAlloc(GHND,sizeof(TW_ONEVALUE));

        if(twCap.hContainer)
        {
            pVal           = (pTW_ONEVALUE)GlobalLock(twCap.hContainer);
            pVal->ItemType = sign ? TWTY_INT16 : TWTY_UINT16;
            pVal->Item     = (TW_UINT32)value;
            GlobalUnlock(twCap.hContainer);
            ret_value      = SetCapability(twCap);
            GlobalFree(twCap.hContainer);
        }
        return ret_value;
    }
    return false;
}

/** Sets the capability of the Twain Data Source
 */
bool TwainIface::SetCapability(TW_CAPABILITY& cap)
{
    if(DSOpen())
    {
        return CallTwainProc(&m_AppId, &m_Source, DG_CONTROL,
                             DAT_CAPABILITY, MSG_SET, (TW_MEMREF)&cap);
    }
    return false;
}

/** Sets the number of images which can be accepted by the application at one time
 */
bool TwainIface::SetImageCount(TW_INT16 nCount)
{
    if(SetCapability(CAP_XFERCOUNT,(TW_UINT16)nCount,true))
    {
        m_nImageCount = nCount;
        return true;
    }
    else
    {
        if(GetRC() == TWRC_CHECKSTATUS)
        {
            TW_UINT32 count;

            if(GetCapability(CAP_XFERCOUNT, count))
            {
                nCount = (TW_INT16)count;

                if(SetCapability(CAP_XFERCOUNT, nCount))
                {
                    m_nImageCount = nCount;
                    return true;
                }
            }
        }
    }
    return false;
}

/** Called to enable the Twain Acquire Dialog. This too can be
    overridden but is a helluva job .
 */
bool TwainIface::EnableSource(bool showUI)
{
    if(DSOpen() && !SourceEnabled())
    {
        TW_USERINTERFACE twUI;
        twUI.ShowUI  = showUI;
        twUI.hParent = (TW_HANDLE)m_hMessageWnd;

        if(CallTwainProc(&m_AppId, &m_Source, DG_CONTROL,
                         DAT_USERINTERFACE, MSG_ENABLEDS, (TW_MEMREF)&twUI))
        {
            m_bSourceEnabled = true;
            m_bModalUI       = twUI.ModalUI;
        }
        else
        {
            m_bSourceEnabled = false;
            m_bModalUI       = true;
        }
        return m_bSourceEnabled;
    }
    return false;
}

/** Called to acquire images from the source. parameter numImages is the
    number of images that you an handle concurrently
 */
bool TwainIface::Acquire(int numImages)
{
    if(DSOpen() || OpenSource())
    {
        if(SetImageCount(numImages))
        {
            if(EnableSource())
            {
                //ReleaseTwain();
                //TW_IMAGEINFO info;
                //GetImageInfo(info);
                //GetImage(info);
                return true;
            }
        }
    }
    return false;
}

/** Called to disable the source.
 */
bool TwainIface::DisableSource()
{
    if(SourceEnabled())
    {
        TW_USERINTERFACE twUI;
        if(CallTwainProc(&m_AppId, &m_Source, DG_CONTROL,
                         DAT_USERINTERFACE, MSG_DISABLEDS, &twUI))
        {
            m_bSourceEnabled = false;
            return true;
        }
    }
    return false;
}

/** Called by ProcessMessage to Translate a TWAIN message
 */
void TwainIface::TranslateMessage(TW_EVENT& twEvent)
{
    switch(twEvent.TWMessage)
    {
        case MSG_XFERREADY:
            TransferImage();
            break;

        case MSG_CLOSEDSREQ:
            if(CanClose())
            {
                CloseDS();
            }
            break;
    }
}

/** Gets Imageinfo for an image which is about to be transferred.
 */
bool TwainIface::GetImageInfo(TW_IMAGEINFO& info)
{
    if(SourceEnabled())
    {
        return CallTwainProc(&m_AppId, &m_Source, DG_IMAGE,
                             DAT_IMAGEINFO, MSG_GET, (TW_MEMREF)&info);
    }
    return false;
}

/** Transfers the image or cancels the transfer depending on the state of the TWAIN system
 */
void TwainIface::TransferImage()
{
    TW_IMAGEINFO info;
    bool bContinue=true;

    while(bContinue)
    {
        if(GetImageInfo(info))
        {
            int permission = ShouldTransfer(info);

            switch(permission)
            {
                case TWCPP_CANCELTHIS:
                    bContinue=EndTransfer();
                    break;

                case TWCPP_CANCELALL:
                    CancelTransfer();
                    bContinue=false;
                    break;

                case TWCPP_DOTRANSFER:
                    bContinue=GetImage(info);
                    break;
            }
        }
    }
}

/** Ends the current transfer.
    Returns true if the more images are pending
 */
bool TwainIface::EndTransfer()
{
    TW_PENDINGXFERS twPend;
    if(CallTwainProc(&m_AppId, &m_Source, DG_CONTROL,
                     DAT_PENDINGXFERS, MSG_ENDXFER, (TW_MEMREF)&twPend))
    {
        return twPend.Count != 0;
    }
    return false;
}

/** Aborts all transfers
 */
void TwainIface::CancelTransfer()
{
    TW_PENDINGXFERS twPend;
    CallTwainProc(&m_AppId, &m_Source, DG_CONTROL,
                  DAT_PENDINGXFERS, MSG_RESET, (TW_MEMREF)&twPend);
}

/** Calls TWAIN to actually get the image
 */
bool TwainIface::GetImage(TW_IMAGEINFO& info)
{
    TW_MEMREF pdata;
    CallTwainProc(&m_AppId, &m_Source, DG_IMAGE, DAT_IMAGENATIVEXFER, MSG_GET, &pdata);

    switch(m_returnCode)
    {
        case TWRC_XFERDONE:
            CopyImage(pdata, info);
            break;

        case TWRC_CANCEL:
            break;

        case TWRC_FAILURE:
            CancelTransfer();
            return false;
            break;
    }

    GlobalFree(pdata);
    return EndTransfer();
}
