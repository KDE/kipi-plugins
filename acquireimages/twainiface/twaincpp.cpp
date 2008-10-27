#include "twaincpp.h"
#include <string.h>
#include <QtGlobal>


/*
Constructor:
	Parameters : HWND 
				Window to subclass
				 
*/
CTwain::CTwain(HWND hWnd)
{
	m_hMessageWnd = 0;
	m_hTwainDLL = NULL;
	m_pDSMProc = NULL;
	m_bSourceSelected = false;
	m_bDSOpen = m_bDSMOpen = false;
	m_bSourceEnabled = false;
	m_bModalUI = true;
	m_nImageCount = TWCPP_ANYCOUNT;
	if(hWnd)
	{
		InitTwain(hWnd);
	}
}

CTwain::~CTwain()
{
	ReleaseTwain();
}

/*
Initializes TWAIN interface . Is already called from the constructor. 
It should be called again if ReleaseTwain is called.

  hWnd is the window which has to subclassed in order to recieve
  Twain messaged. Normally - this would be your main application window.

*/
bool CTwain::InitTwain(HWND hWnd)
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
	strcpy(libName,"TWAIN_32.DLL");
	
	m_hTwainDLL  = LoadLibraryA(libName);
	if(m_hTwainDLL != NULL)
	{  qWarning("valid driver");
		if(!(m_pDSMProc = (DSMENTRYPROC)GetProcAddress(m_hTwainDLL,(LPCSTR)MAKEINTRESOURCE(1))))
		{
			FreeLibrary(m_hTwainDLL);
			m_hTwainDLL = NULL;
		}
	}
	if(IsValidDriver())
	{
	  
		GetIdentity();
		m_bDSMOpen= CallTwainProc(&m_AppId,NULL,DG_CONTROL,DAT_PARENT,MSG_OPENDSM,(TW_MEMREF)&m_hMessageWnd);
		return true;
	}
	else
	{
		return false;
	}
}

/*
Releases the twain interface . Need not be called unless you
want to specifically shut it down.
*/
void CTwain::ReleaseTwain()
{
	if(IsValidDriver())
	{
		CloseDSM();
		FreeLibrary(m_hTwainDLL);
		m_hTwainDLL = NULL;
		m_pDSMProc = NULL;
	}
}

/*
Returns true if a valid driver has been loaded
*/
bool CTwain::IsValidDriver() const
{
	return (m_hTwainDLL && m_pDSMProc);
}

/*
Entry point into Twain. For a complete description of this
routine  please refer to the Twain specification 1.8
*/
bool CTwain::CallTwainProc(pTW_IDENTITY pOrigin,pTW_IDENTITY pDest,
					   TW_UINT32 DG,TW_UINT16 DAT,TW_UINT16 MSG,
					   TW_MEMREF pData)
{
	if(IsValidDriver())
	{
	USHORT ret_val;
		ret_val = (*m_pDSMProc)(pOrigin,pDest,DG,DAT,MSG,pData);
		m_returnCode = ret_val;
		if(ret_val == TWRC_FAILURE)
		{
			(*m_pDSMProc)(pOrigin,pDest,DG_CONTROL,DAT_STATUS,MSG_GET,&m_Status);
		}
		return (ret_val == TWRC_SUCCESS);
	}
	else
	{
		m_returnCode = TWRC_FAILURE;
		return false;
	}
}

/*
This function should ideally be overridden in the derived class . If only a 
few fields need to be updated , call CTawin::GetIdentity first in your
derived class
*/
void CTwain::GetIdentity()
{
	// Expects all the fields in m_AppId to be set except for the id field.
	m_AppId.Id = 0; // Initialize to 0 (Source Manager
	// will assign real value)
	m_AppId.Version.MajorNum = 3; //Your app's version number
	m_AppId.Version.MinorNum = 5;
	m_AppId.Version.Language = TWLG_USA;
	m_AppId.Version.Country = TWCY_USA;
	strcpy (m_AppId.Version.Info, "3.5");
	m_AppId.ProtocolMajor = TWON_PROTOCOLMAJOR;
	m_AppId.ProtocolMinor = TWON_PROTOCOLMINOR;
	m_AppId.SupportedGroups = DG_IMAGE | DG_CONTROL;
	strcpy (m_AppId.Manufacturer, "MICSS");
	strcpy (m_AppId.ProductFamily, "Generic");
	strcpy (m_AppId.ProductName, "Twain Test");

}


/*
Called to display a dialog box to select the Twain source to use.
This can be overridden if a list of all sources is available
to the application. These sources can be enumerated by Twain.
it is not yet supportted by CTwain.
*/
bool CTwain::SelectSource()
{

	memset(&m_Source,0,sizeof(m_Source));
	if(!SourceSelected())
	{
		SelectDefaultSource();
	}
	if(CallTwainProc(&m_AppId,NULL,DG_CONTROL,DAT_IDENTITY,MSG_USERSELECT,&m_Source))
	{
		m_bSourceSelected = true;
	}
	return m_bSourceSelected;
}

/*
Called to select the default source
*/
bool CTwain::SelectDefaultSource()
{
	m_bSourceSelected = CallTwainProc(&m_AppId,NULL,DG_CONTROL,DAT_IDENTITY,MSG_GETDEFAULT,&m_Source);
	return m_bSourceSelected;
}

/*
Closes the Data Source
*/
void CTwain::CloseDS()
{
	if(DSOpen())
	{
		DisableSource();
		CallTwainProc(&m_AppId,NULL,DG_CONTROL,DAT_IDENTITY,MSG_CLOSEDS,(TW_MEMREF)&m_Source);
		m_bDSOpen = false;
	}
}

/*
Closes the Data Source Manager
*/
void CTwain::CloseDSM()
{
	if(DSMOpen())
	{
		CloseDS();
		CallTwainProc(&m_AppId,NULL,DG_CONTROL,DAT_PARENT,MSG_CLOSEDSM,(TW_MEMREF)&m_hMessageWnd);
		m_bDSMOpen = false;
	}
}

/*
Returns true if the Data Source Manager is Open
*/
bool CTwain::DSMOpen() const
{
   qWarning("DSMOpen");
	return IsValidDriver() && m_bDSMOpen;
	
}

/*
Returns true if the Data Source is Open
*/
bool CTwain::DSOpen() const
{  qWarning("DSOpen");
	return IsValidDriver() && DSMOpen() && m_bDSOpen;
}

/*
Opens a Data Source supplied as the input parameter
*/
bool CTwain::OpenSource(TW_IDENTITY *pSource)
{
	if(pSource) 
	{
		m_Source = *pSource;
		qWarning("OpenSource - pSource");
	}
	if(DSMOpen())
	{
	  qWarning("OpenSource - DSMOpen");
		if(!SourceSelected())
		{
		qWarning("OpenSource - SourceSelected");
			SelectDefaultSource();
		}
		m_bDSOpen = CallTwainProc(&m_AppId,NULL,DG_CONTROL,DAT_IDENTITY,MSG_OPENDS,(TW_MEMREF)&m_Source);
	}
	return DSOpen();
}

/*
Should be called from the main message loop of the application. Can always be called,
it will not process the message unless a scan is in progress.
*/
bool CTwain::ProcessMessage(MSG msg)
{
 qWarning("ProcessMessage");
	if(SourceEnabled())
	{
	TW_UINT16  twRC = TWRC_NOTDSEVENT;

	TW_EVENT twEvent;
		twEvent.pEvent = (TW_MEMREF)&msg;
		//memset(&twEvent, 0, sizeof(TW_EVENT));

		twEvent.TWMessage = MSG_NULL;
	
		CallTwainProc(&m_AppId,&m_Source,DG_CONTROL,DAT_EVENT,MSG_PROCESSEVENT,(TW_MEMREF)&twEvent);
		if(GetRC() != TWRC_NOTDSEVENT)
		{
			TranslateMessage(twEvent);
		}
		//return false;
		return (twRC==TWRC_DSEVENT);  

	}
	return false;
}

/*
Queries the capability of the Twain Data Source
*/
bool CTwain::GetCapability(TW_CAPABILITY& twCap,TW_UINT16 cap,TW_UINT16 conType)
{
	if(DSOpen())
	{
		twCap.Cap = cap;
		twCap.ConType = conType;
		twCap.hContainer = NULL;

		if(CallTwainProc(&m_AppId,&m_Source,DG_CONTROL,DAT_CAPABILITY,MSG_GET,(TW_MEMREF)&twCap))
		{
			return true;
		}
	}
	return false;
}

/*
Queries the capability of the Twain Data Source
*/
bool CTwain::GetCapability(TW_UINT16 cap,TW_UINT32& value)
{
TW_CAPABILITY twCap;
	if(GetCapability(twCap,cap))
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


/*
Sets the capability of the Twain Data Source
*/
bool CTwain::SetCapability(TW_UINT16 cap,TW_UINT16 value,bool sign)
{
	if(DSOpen())
	{
	TW_CAPABILITY twCap;
	pTW_ONEVALUE pVal;
	bool ret_value = false;

		twCap.Cap = cap;
		twCap.ConType = TWON_ONEVALUE;
		
		twCap.hContainer = GlobalAlloc(GHND,sizeof(TW_ONEVALUE));
		if(twCap.hContainer)
		{
			pVal = (pTW_ONEVALUE)GlobalLock(twCap.hContainer);
			pVal->ItemType = sign ? TWTY_INT16 : TWTY_UINT16;
			pVal->Item = (TW_UINT32)value;
			GlobalUnlock(twCap.hContainer);
			ret_value = SetCapability(twCap);
			GlobalFree(twCap.hContainer);
		}
		return ret_value;
	}
	return false;
}

/*
Sets the capability of the Twain Data Source
*/
bool CTwain::SetCapability(TW_CAPABILITY& cap)
{
	if(DSOpen())
	{
		return CallTwainProc(&m_AppId,&m_Source,DG_CONTROL,DAT_CAPABILITY,MSG_SET,(TW_MEMREF)&cap);
	}
	return false;
}

/*
Sets the number of images which can be accpeted by the application at one time
*/
bool CTwain::SetImageCount(TW_INT16 nCount)
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
			if(GetCapability(CAP_XFERCOUNT,count))
			{
				nCount = (TW_INT16)count;
				if(SetCapability(CAP_XFERCOUNT,nCount))
				{
					m_nImageCount = nCount;
					return true;
				}
			}
		}
	}
	return false;
}

/*
Called to enable the Twain Acquire Dialog. This too can be
overridden but is a helluva job . 
*/
bool CTwain::EnableSource(bool showUI)
{
	if(DSOpen() && !SourceEnabled())
	{
		qWarning("EnableSource - DSOpen et SourceEnabled");
		TW_USERINTERFACE twUI;
		twUI.ShowUI = showUI;
		twUI.hParent = (TW_HANDLE)m_hMessageWnd;
		if(CallTwainProc(&m_AppId,&m_Source,DG_CONTROL,DAT_USERINTERFACE,MSG_ENABLEDS,(TW_MEMREF)&twUI))
		{
		 qWarning("EnableSource - CallTwainProc");
			m_bSourceEnabled = true;
			m_bModalUI = twUI.ModalUI;
		}
		else
		{
		
			m_bSourceEnabled = false;
			m_bModalUI = true;
		}
		return m_bSourceEnabled;
	}
	return false;
}

/*
Called to acquire images from the source. parameter numImages i the
numberof images that you an handle concurrently
*/
bool CTwain::Acquire(int numImages)
{
	if(DSOpen() || OpenSource())
	{
	qWarning("1!");
		if(SetImageCount(numImages))
		{
		qWarning("2!");
			if(EnableSource())
			{
			qWarning("3!");
			//ReleaseTwain();
			//TW_IMAGEINFO info;
			//GetImageInfo(info);
			// GetImage(info);
				return true;
			}
		}
	}
	return false;
}

/*
 Called to disable the source.
*/
bool CTwain::DisableSource()
{
	if(SourceEnabled())
	{
	TW_USERINTERFACE twUI;
		if(CallTwainProc(&m_AppId,&m_Source,DG_CONTROL,DAT_USERINTERFACE,MSG_DISABLEDS,&twUI))
		{
			m_bSourceEnabled = false;
			return true;
		}
	}
	return false;
}

/*
Called by ProcessMessage to Translate a TWAIN message
*/
void CTwain::TranslateMessage(TW_EVENT& twEvent)
{
	switch(twEvent.TWMessage)
	{
	case MSG_XFERREADY:
			TransferImage();
			qWarning("translate1");
			break;
	case MSG_CLOSEDSREQ:
			if(CanClose())
			{
				CloseDS();
			}
			qWarning("translate2");
			break;
	}
}

/*
Gets Imageinfo for an image which is about to be transferred.
*/
bool CTwain::GetImageInfo(TW_IMAGEINFO& info)
{
	if(SourceEnabled())
	{
		return CallTwainProc(&m_AppId,&m_Source,DG_IMAGE,DAT_IMAGEINFO,MSG_GET,(TW_MEMREF)&info);
	}
	return false;
}

/*
Trasnfers the image or cancels the transfer depending on the state of the
TWAIN system
*/
void CTwain::TransferImage()
{
TW_IMAGEINFO info;
bool bContinue=true;
	while(bContinue)
	{
		if(GetImageInfo(info))
		{
			int permission;
			permission = ShouldTransfer(info);
			switch(permission)
			{
			case TWCPP_CANCELTHIS:
					bContinue=EndTransfer();
					qWarning("endhhhhhhhhhhh");
					break;
					
			case TWCPP_CANCELALL:
					CancelTransfer();
					bContinue=false;
					qWarning("endalllllllllhhhhhhhhhhh");
					break;
					
			case TWCPP_DOTRANSFER:
			qWarning("dohhhhhhhhhhhhhhhhhhhh");
					bContinue=GetImage(info);
					break;
			}
		}
	}
}

/*
Ends the current transfer.
Returns true if the more images are pending
*/
bool CTwain::EndTransfer()
{
TW_PENDINGXFERS twPend;
	if(CallTwainProc(&m_AppId,&m_Source,DG_CONTROL,DAT_PENDINGXFERS,MSG_ENDXFER,(TW_MEMREF)&twPend))
	{
		return twPend.Count != 0;
	}
	return false;
}

/*
Aborts all transfers
*/
void CTwain::CancelTransfer()
{
TW_PENDINGXFERS twPend;
	CallTwainProc(&m_AppId,&m_Source,DG_CONTROL,DAT_PENDINGXFERS,MSG_RESET,(TW_MEMREF)&twPend);
}

/*
Calls TWAIN to actually get the image
*/
bool CTwain::GetImage(TW_IMAGEINFO& info)
{
HANDLE hBitmap;
	CallTwainProc(&m_AppId,&m_Source,DG_IMAGE,DAT_IMAGENATIVEXFER,MSG_GET,&hBitmap);
	qWarning("calltwain");
	switch(m_returnCode)
	{
	case TWRC_XFERDONE:
			CopyImage(hBitmap,info);
			break;
	case TWRC_CANCEL:
			break;
	case TWRC_FAILURE:
			CancelTransfer();
			return false;

	}
	GlobalFree(hBitmap);
	return EndTransfer();
}

