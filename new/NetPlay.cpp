#include "stdafx.h"
#include "math.h"
#include <objbase.h>
//#include <initguid.h>
#include <windowsx.h>

#include "global.h"
#include "imemory.h"
#include "NetPlay.h"

char LastErrorMessage[256];
bool ConnectionFound;
LPVOID Connection;
DWORD ConnectionSize;
GUID SPGuid;
char ConnectionName[256];
LPDIRECTPLAY4A dp;
LPDIRECTPLAYLOBBY3A dpLobby;
LPVOID Address;
DWORD AddressSize;
DPSESSIONDESC2 SessionDesc;
char SessionName[256];
DPID PlayerID,ServerID;
DPNAME PlayerName,ServerName;
char ShortPlayerName[128];
bool IsHost;



BOOL FAR PASCAL EnumConnectionsCallback(
	LPCGUID lpguidSP,
	LPVOID lpConnection,
	DWORD dwConnectionSize,
	LPCDPNAME lpName,
	DWORD dwFlags,
	LPVOID lpContext
	)
{
	if(strcmp((const char *)(lpName->lpszShortNameA),"Internet TCP/IP Connection For DirectPlay")==0)
	{
		ConnectionFound=TRUE;
		Connection=malloc(dwConnectionSize);
		ConnectionSize=dwConnectionSize;
		SPGuid=*lpguidSP;
		memcpy(Connection,lpConnection,dwConnectionSize);
		strcpy(ConnectionName,(const char *)(lpName->lpszShortNameA));
		return(FALSE);
	}
	return(TRUE);
}

HRESULT NetPlayCreateAddress(VOID **ppAddress,DWORD *pdwAddressSize)
{
	DWORD   dwElementCount;
	VOID *pAddress;
	DWORD dwAddressSize;
	DPCOMPOUNDADDRESSELEMENT addressElements[3];
	WORD wPort=2300;
	HRESULT hr;

	dwElementCount=0;
	addressElements[dwElementCount].guidDataType = DPAID_ServiceProvider;
	addressElements[dwElementCount].dwDataSize   = sizeof(GUID);
	addressElements[dwElementCount].lpData       = &SPGuid;
	dwElementCount++;

	addressElements[dwElementCount].guidDataType = DPAID_INet;
	addressElements[dwElementCount].dwDataSize   = lstrlen("192.168.000.001") + 1;
	addressElements[dwElementCount].lpData       = "192.168.000.001";
	dwElementCount++;

	addressElements[dwElementCount].guidDataType = DPAID_INetPort;
	addressElements[dwElementCount].dwDataSize   = sizeof(WORD);
	addressElements[dwElementCount].lpData       = (VOID*)&wPort;
	dwElementCount++;

	hr = dpLobby->CreateCompoundAddress( addressElements, dwElementCount,
		                                  NULL, &dwAddressSize );
	if( hr != DPERR_BUFFERTOOSMALL )
		return hr;

	// Allocate space
	pAddress = GlobalAllocPtr( GHND, dwAddressSize );
	if( pAddress == NULL )
		return DPERR_NOMEMORY;

	// Create the address
	hr = dpLobby->CreateCompoundAddress( addressElements, dwElementCount,
						                  pAddress, &dwAddressSize );
	if( FAILED(hr) )
	{
		GlobalFreePtr( pAddress );
		return hr;
	}

	// Return the address info
	*ppAddress      = pAddress;
	*pdwAddressSize = dwAddressSize;
	return(0);
}


bool NetPlayCreate()
{
	HRESULT hr = CoCreateInstance( CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER, 
				                   IID_IDirectPlay4A, (LPVOID*)&dp);

	hr = CoCreateInstance( CLSID_DirectPlayLobby, NULL, CLSCTX_INPROC_SERVER,
						   IID_IDirectPlayLobby3A, (VOID**)&dpLobby );

	dp->EnumConnections(NULL,EnumConnectionsCallback,NULL,DPCONNECTION_DIRECTPLAY);
	if(!ConnectionFound)
		return(false);
	NetPlayCreateAddress((VOID **)&Address,(DWORD *)&AddressSize);

	hr=dp->InitializeConnection(Address,0);

	strcpy(SessionName,"U64FightClub");
	IsHost=true;
	if(IsHost)
	{
		memset(&PlayerName,0,sizeof(DPNAME));
		PlayerName.dwSize=sizeof(DPNAME);
		strcpy(ShortPlayerName,"Master");
		PlayerName.lpszShortNameA=ShortPlayerName;

		memset(&SessionDesc,0,sizeof(DPSESSIONDESC2));
		SessionDesc.dwSize=sizeof(DPSESSIONDESC2);
		SessionDesc.dwFlags=DPSESSION_CLIENTSERVER ;
		SessionDesc.guidInstance=GUID_NULL;		// DirectPlay should fill this in
		SessionDesc.guidApplication=GUID_NULL;
		SessionDesc.dwMaxPlayers=2;
		SessionDesc.dwCurrentPlayers=0;
		SessionDesc.lpszSessionNameA=SessionName;
		hr=dp->Open(&SessionDesc,DPOPEN_CREATE);
		if(hr!=DP_OK)
		{
			::MessageBox(0,ErrorToString(hr),"Open Failed - Create",MB_OK);
			return(hr);
		}
		hr=dp->CreatePlayer(&PlayerID,&PlayerName,NULL,NULL,0,DPPLAYER_SERVERPLAYER);
		if(hr!=DP_OK)
		{
			::MessageBox(0,ErrorToString(hr),"CreatePlayer Failed",MB_OK);
			return(hr);
		}
	}
	else
	{
	}
	return(true);
}

void NetPlayDestroy()
{
	if(dp)
	{
		dp->DestroyPlayer(PlayerID);
		dp->Close();
		dpLobby->Release();
		dp->Release();
		dp=NULL;
	}
}

DWORD WINAPI ReceiveThread( VOID* lpThreadParameter )
{
    while (1)
    {
        // receive any messages in the queue
        ReceiveMessage();
    }

    ExitThread(0);

    return (0);
}

HRESULT ReceiveMessage()
{
    DPID    idFrom, idTo;
    VOID*   pMsgBuffer      = NULL;
    DWORD   dwMsgBufferSize = 0L;
    HRESULT hr;

    // Loop to read all messages in queue
    do
    {
        // Loop until a single message is successfully read
        do
        {
            // Read messages from any player, including system player
            idFrom = 0;
            idTo   = 0;

            hr = dp->Receive( &idFrom, &idTo, DPRECEIVE_ALL,
                                           pMsgBuffer, &dwMsgBufferSize );

            // Not enough room, so resize buffer
            if( hr == DPERR_BUFFERTOOSMALL )
            {
                if( pMsgBuffer )
                    GlobalFreePtr( pMsgBuffer );
                pMsgBuffer = GlobalAllocPtr( GHND, dwMsgBufferSize );
                if( pMsgBuffer == NULL )
                    hr = DPERR_OUTOFMEMORY;
            }
        } while( hr == DPERR_BUFFERTOOSMALL );

        if( hr==DP_OK ) && ( dwMsgBufferSize >= sizeof(DPMSG_GENERIC) ) )
        {
            // Check for system message
            if( idFrom == DPID_SYSMSG )
            {
                HandleSystemMessage((DPMSG_GENERIC*)pMsgBuffer,
                                     dwMsgBufferSize, idFrom, idTo );
            }
            else
            {
//                HandleApplicationMessage((DPMSG_GENERIC*)pMsgBuffer,
  //                                        dwMsgBufferSize, idFrom, idTo );
            }
        }
    } while( SUCCEEDED(hr) );

    // Free any memory we created
    if( pMsgBuffer )
        GlobalFreePtr( pMsgBuffer );

    return DP_OK;
}

VOID HandleSystemMessage(DPMSG_GENERIC* pMsg,
                          DWORD dwMsgSize, DPID idFrom, DPID idTo )
{
    // The body of each case is there so you can set a breakpoint and examine
    // the contents of the message received.
    switch( pMsg->dwType )
    {
        case DPSYS_CREATEPLAYERORGROUP:
        {
            DPMSG_CREATEPLAYERORGROUP* p = (DPMSG_CREATEPLAYERORGROUP*)pMsg;
            break;
        }

        case DPSYS_DESTROYPLAYERORGROUP:
        {
            DPMSG_DESTROYPLAYERORGROUP* p = (DPMSG_DESTROYPLAYERORGROUP*)pMsg;
            break;
        }

        case DPSYS_ADDPLAYERTOGROUP:
        {
            DPMSG_ADDPLAYERTOGROUP* p = (DPMSG_ADDPLAYERTOGROUP*)pMsg;
            break;
        }

        case DPSYS_DELETEPLAYERFROMGROUP:
        {
            DPMSG_DELETEPLAYERFROMGROUP* p = (DPMSG_DELETEPLAYERFROMGROUP*)pMsg;
            break;
        }

        case DPSYS_SESSIONLOST:
        {
            DPMSG_SESSIONLOST* p = (DPMSG_SESSIONLOST*)pMsg;
            break;
        }

        case DPSYS_HOST:
        {
            DPMSG_HOST* p = (DPMSG_HOST*)pMsg;
            break;
        }

        case DPSYS_SETPLAYERORGROUPDATA:
        {
            DPMSG_SETPLAYERORGROUPDATA* p = (DPMSG_SETPLAYERORGROUPDATA*)pMsg;
            break;
        }

        case DPSYS_SETPLAYERORGROUPNAME:
        {
            DPMSG_SETPLAYERORGROUPNAME* p = (DPMSG_SETPLAYERORGROUPNAME*)pMsg;
            break;
        }
    }
}



char *ErrorToString(HRESULT errornum)
{
	char *string=LastErrorMessage;
	switch(errornum)
	{
	case DP_OK:
		{
			strcpy(string,"The request completed successfully.");
			break;
		}
	case DPERR_ACCESSDENIED:
		{
			strcpy(string,"The session is full or an incorrect password was supplied.");
			break;
		}
	case DPERR_ACTIVEPLAYERS:
		{
			strcpy(string,"The requested operation cannot be performed because there are existing active players.");
			break;
		}
	case DPERR_ALREADYINITIALIZED:
		{
			strcpy(string,"This object is already initialized.");
			break;
		}
	case DPERR_APPNOTSTARTED:
		{
			strcpy(string,"The application has not been started yet.");
			break;
		}
	case DPERR_AUTHENTICATIONFAILED:
		{
			strcpy(string,"The password or credentials supplied could not be authenticated. ");
			break;
		}
	case DPERR_BUFFERTOOLARGE:
		{
			strcpy(string,"The data buffer is too large to store. ");
			break;
		}
	case DPERR_BUSY:
		{
			strcpy(string,"A message cannot be sent because the transmission medium is busy. ");
			break;
		}
	case DPERR_BUFFERTOOSMALL:
		{
			strcpy(string,"The supplied buffer is not large enough to contain the requested data. ");
			break;
		}
	case DPERR_CANTADDPLAYER:
		{
			strcpy(string,"The player cannot be added to the session. ");
			break;
		}
	case DPERR_CANTCREATEGROUP:
		{
			strcpy(string,"A new group cannot be created.");
			break;
		}
	case DPERR_CANTCREATEPLAYER:
		{
			strcpy(string,"A new player cannot be created.");
			break;
		}
	case DPERR_CANTCREATEPROCESS:
		{
			strcpy(string,"Cannot start the application. ");
			break;
		}
	case DPERR_CANTCREATESESSION:
		{
			strcpy(string,"A new session cannot be created.");
			break;
		}
	case DPERR_CANTLOADCAPI:
		{
			strcpy(string,"No credentials were supplied and the CryptoAPI package (CAPI) to use for cryptography services cannot be loaded. ");
			break;
		}
	case DPERR_CANTLOADSECURITYPACKAGE:
		{
			strcpy(string,"The software security package cannot be loaded. ");
			break;
		}
	case DPERR_CANTLOADSSPI:
		{
			strcpy(string,"No credentials were supplied and the software security package (SSPI) that will prompt for credentials cannot be loaded. ");
			break;
		}
	case DPERR_CAPSNOTAVAILABLEYET:
		{
			strcpy(string,"The capabilities of the DirectPlay object have not been determined yet. This error will occur if the DirectPlay object is implemented on a connectivity solution that requires polling to determine available bandwidth and latency. ");
			break;
		}
	case DPERR_CONNECTING:
		{
			strcpy(string,"The method is in the process of connecting to the network. The application should keep calling the method until it returns DP_OK, indicating successful completion, or it returns a different error. ");
			break;
		}
	case DPERR_ENCRYPTIONFAILED:
		{
			strcpy(string,"The requested information could not be digitally encrypted. Encryption is used for message privacy. This error is only relevant in a secure session. ");
			break;
		}
	case DPERR_EXCEPTION:
		{
			strcpy(string,"An exception occurred when processing the request. ");
			break;
		}
	case DPERR_GENERIC:
		{
			strcpy(string,"An undefined error condition occurred. ");
			break;
		}
/*  this is in DirectX5 help... but not in dplay.h???
	case DPERR_INVALIDCREDENTIALS:
		{
			strcpy(string,"The credentials supplied (as to IDirectPlay3::SecureOpen) were not valid. ");
			break;
		}
*/
	case DPERR_INVALIDFLAGS:
		{
			strcpy(string,"The flags passed to this method are invalid. ");
			break;
		}
	case DPERR_INVALIDGROUP:
		{
			strcpy(string,"The group ID is not recognized as a valid group ID for this game session. ");
			break;
		}
	case DPERR_INVALIDINTERFACE:
		{
			strcpy(string,"The interface parameter is invalid. ");
			break;
		}
	case DPERR_INVALIDOBJECT:
		{
			strcpy(string,"The DirectPlay object pointer is invalid. ");
			break;
		}
	case DPERR_INVALIDPARAMS:
		{
			strcpy(string,"One or more of the parameters passed to the method are invalid. ");
			break;
		}
	case DPERR_INVALIDPASSWORD:
		{
			strcpy(string,"An invalid password was supplied when attempting to join a session that requires a password. ");
			break;
		}
	case DPERR_INVALIDPLAYER:
		{
			strcpy(string,"The player ID is not recognized as a valid player ID for this game session. ");
			break;
		}
	case DPERR_LOGONDENIED:
		{
			strcpy(string,"The session could not be opened because credentials are required and either no credentials were supplied or the credentials were invalid. ");
			break;
		}
	case DPERR_NOCAPS:
		{
			strcpy(string,"The communication link that DirectPlay is attempting to use is not capable of this function. ");
			break;
		}
	case DPERR_NOCONNECTION:
		{
			strcpy(string,"No communication link was established. ");
			break;
		}
	case DPERR_NOINTERFACE:
		{
			strcpy(string,"The interface is not supported. ");
			break;
		}
	case DPERR_NOMESSAGES:
		{
			strcpy(string,"There are no messages in the receive queue. ");
			break;
		}
	case DPERR_NONAMESERVERFOUND:
		{
			strcpy(string,"No name server (host) could be found or created. A host must exist to create a player. ");
			break;
		}
	case DPERR_NONEWPLAYERS:
		{
			strcpy(string,"The session is not accepting any new players. ");
			break;
		}
	case DPERR_NOPLAYERS:
		{
			strcpy(string,"There are no active players in the session. ");
			break;
		}
	case DPERR_NOSESSIONS:
		{
			strcpy(string,"There are no existing sessions for this game. ");
			break;
		}
	case DPERR_NOTLOBBIED:
		{
			strcpy(string,"Returned by the IDirectPlayLobby2::Connect method if the application was not started by using the IDirectPlayLobby2::RunApplication method or if there is no DPLCONNECTION structure currently initialized for this DirectPlayLobby object. ");
			break;
		}
	case DPERR_NOTLOGGEDIN:
		{
			strcpy(string,"An action cannot be performed because a player or client application is not logged in. Returned by the IDirectPlay3::Send method when the client application tries to send a secure message without being logged in. ");
			break;
		}
	case DPERR_OUTOFMEMORY:
		{
			strcpy(string,"There is insufficient memory to perform the requested operation. ");
			break;
		}
	case DPERR_PLAYERLOST:
		{
			strcpy(string,"A player has lost the connection to the session. ");
			break;
		}
	case DPERR_SENDTOOBIG:
		{
			strcpy(string,"The message being sent by the IDirectPlay3::Send method is too large. ");
			break;
		}
	case DPERR_SESSIONLOST:
		{
			strcpy(string,"The connection to the session has been lost. ");
			break;
		}
	case DPERR_SIGNFAILED:
		{
			strcpy(string,"The requested information could not be digitally signed. Digital signatures are used to establish the authenticity of messages. ");
			break;
		}
	case DPERR_TIMEOUT:
		{
			strcpy(string,"The operation could not be completed in the specified time. ");
			break;
		}
	case DPERR_UNAVAILABLE:
		{
			strcpy(string,"The requested function is not available at this time. ");
			break;
		}
	case DPERR_UNINITIALIZED:
		{
			strcpy(string,"The requested object has not been initialized. ");
			break;
		}
	case DPERR_UNKNOWNAPPLICATION:
		{
			strcpy(string,"An unknown application was specified. ");
			break;
		}
	case DPERR_UNSUPPORTED:
		{
			strcpy(string,"The function is not available in this implementation. Returned from IDirectPlay3::GetGroupConnectionSettings and IDirectPlay3::SetGroupConnectionSettings if they are called from a session that is not a lobby session. ");
			break;
		}
	case DPERR_USERCANCEL:
		{
			strcpy(string,"1) The user canceled the connection process during a call to the IDirectPlay3::Open method. 2) The user clicked Cancel in one of the DirectPlay service provider dialog boxes during a call to IDirectPlay3::EnumSessions. }");
			break;
		}
	case DPERR_CANNOTCREATESERVER:
		{
			strcpy(string,"Cannot create server");
			break;
		}
	default:
		{
			sprintf(string,"Unknown %lX %lX",errornum,DPERR_NOPLAYERS);
			break;
		}
	}
	return(string);
}
