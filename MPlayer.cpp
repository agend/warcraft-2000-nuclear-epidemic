//MULTIPLAYER
#define INITGUID
#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mgraph.h"
#include "mouse.h"
#include "menu.h"
#include "MapDiscr.h"
#include "dpchat.h"
#include "dplobby.h"
#include "fonts.h"
#include "dialogs.h"
extern RLCFont FPassive;
extern RLCFont FActive;
extern RLCFont FDisable;
extern DWORD Signatur[2049];
Menu SingleMulti;
Menu ChooseConn;
Menu ChooseSession;
Menu StartMultiplayer;
word PlayerMenuMode;
word PrevRpos;
extern int COUNTER;
bool ProcessMessages();
int WaitCycle;
bool NetworkGame;
bool GameInProgress;
LPDIRECTPLAY3A		lpDirectPlay3A;
static LPDPLAYINFO		lpDPInfo;
const DWORD APPMSG_CHATSTRING	= 0;			// message type for chat string
const DWORD MAXNAMELEN		= 200;		// max size of a session or player name
char					szSessionName[MAXNAMELEN];
char					szPlayerName[MAXNAMELEN];
const DWORD ExDataID=0xF376425E;
const DWORD PlExitID=0xEA3521BC;
const DWORD NON=0xFFFFFFFF;
//data transferring format
// int ExDataID
// int Time 
// int Rand_Index
// data
extern int tmtmt;
word NProviders;
word NPlayers;
word NSessions;
DWORD RealTime;
LPVOID lplpConnectionBuffer[16];
GUID SessionsGUID[32];
DPID PlayersID[32];
byte* PData[32];
DWORD PDSize[32];
bool PUsed[32];
bool Ready;
extern byte ExBuf[8192];
extern byte MyRace;
extern char CurrentMap[64];
extern int EBPos;
DPID MyDPID;
DPID ServerDPID;
DWORD MyDSize;
byte* MyData;
char ProvidersList [512];
char PlayersList[512];
char SessionsList[512];
bool CreateMultiplaterInterface();
DWORD ExBuf1[2193];
DWORD EBPos1;
int PLNAT[8];
void PrepareGameMedia(byte myid);
// message structure used to send a chat string to another player
typedef struct {
	DWORD	dwType;								// message type (APPMSG_CHATSTRING)
	char	szMsg[1];							// message string (variable length)
} MSG_CHATSTRING, *LPMSG_CHATSTRING;
void IAmLeft();
HRESULT ShutdownConnection(LPDPLAYINFO lpDPInfo);
// globals
HANDLE			ghReceiveThread = NULL;			// handle of receive thread
DWORD			gidReceiveThread = 0;			// id of receive thread
HANDLE			ghKillReceiveEvent = NULL;		// event used to kill receive thread
HWND			ghChatWnd = NULL;				// main chat window
DPLAYINFO	DPInfo;
//execute bufferisation
#define MaxPL 8
struct EXBUFFER{
	DWORD Size;
	bool Enabled;
	DWORD Sign;//0xF376425E
	DWORD RealTime;//if(??==0xFFFFFFFF)-empty buffer
	DWORD RandIndex;
	byte  Data[2048];
};
EXBUFFER EBufs[MaxPL];
EXBUFFER EBufs1[MaxPL];
//network errors:
int SeqErrorsCount;
int LastRandDif;
void SetupEBufs(){
	SeqErrorsCount=0;
	LastRandDif=0;
	PrevRpos=0;
	for(int i=0;i<MaxPL;i++){
		EBufs[i].RealTime=NON;
		EBufs1[i].RealTime=NON;
		EBufs[i].Enabled=true;
		EBufs1[i].Enabled=true;
		EBufs[i].Size=0;
		EBufs1[i].Size=0;
	};
};
extern word rpos;
void UpdateEBufs(){
	//copy containig to ExBuf
	byte* EPOS=ExBuf;
	EBPos=0;
	word FirstRand=0xFFFF;
	word OtherRand=0xFFFF;
	for(int i=0;i<MaxPL;i++){
		//checking random sequence
		if(EBufs[i].RealTime!=NON){
			if(FirstRand==0xFFFF){
				FirstRand=word(EBufs[i].RandIndex);
			}else{
				if(EBufs[i].RandIndex!=FirstRand){
					SeqErrorsCount++;
					LastRandDif=int(EBufs[i].RandIndex)-int(FirstRand);
					rpos=FirstRand;
				};
			};
			//copy data to execute buffer
			memcpy(EPOS,&EBufs[i].Data,EBufs[i].Size);
			EPOS+=EBufs[i].Size;
			EBPos+=EBufs[i].Size;
		};
		EBufs[i].RealTime=EBufs1[i].RealTime;
		if(EBufs1[i].RealTime!=NON){
			memcpy(&EBufs[i].Data,&EBufs1[i].Data,
				EBufs1[i].Size);
			EBufs[i].Size=EBufs1[i].Size;
			EBufs[i].RandIndex=EBufs1[i].RandIndex;
			EBufs1[i].Size=0;
			EBufs1[i].RealTime=NON;
		};
	};
};
bool PresentEmptyBuf(){
	bool zzz=false;
	for(int i=0;i<NPlayers;i++)
		if(EBufs[i].RealTime==NON&&EBufs[i].Enabled)zzz=true;
	return zzz;
};
BOOL WINAPI EnumPlayersCallback2(DPID dpId,
    DWORD dwPlayerType, LPCDPNAME lpName,
    DWORD dwFlags, LPVOID lpContext){
	PlayersID[NPlayers]=dpId;
	strcat(PlayersList,lpName->lpszShortNameA);
	strcat(PlayersList,"|");
	NPlayers++;
	return true;
};
void EnumeratePlayers(){
	NPlayers=0;
	PlayersList[0]=0;
	strcat(PlayersList,"Õ¿◊¿“‹ »√–”|¬€’Œƒ| |");
	if(int(lpDirectPlay3A)){
		lpDirectPlay3A->EnumPlayers((GUID*)&DPCHAT_GUID,
			EnumPlayersCallback2,NULL,0);
	};
	StartMultiplayer.CreateMenu(PlayersList,
		&f32x32w,&f32x32g,NULL,MCP_MAKESHIFT|MCP_CHANGEFONT);
};
InputBox** IBBX;
BOOL WINAPI IBEnumPlayersCallback2(DPID dpId,
    DWORD dwPlayerType, LPCDPNAME lpName,
    DWORD dwFlags, LPVOID lpContext){
	PlayersID[NPlayers]=dpId;
	strcpy(IBBX[NPlayers]->Str,lpName->lpszShortNameA);
	NPlayers++;
	return true;
};
void IBEnumeratePlayers(InputBox** IB){
	for(int i=0;i<8;i++)IB[i]->Str[0]=0;
	IBBX=IB;
	NPlayers=0;
	PlayersList[0]=0;
	strcat(PlayersList,"Õ¿◊¿“‹ »√–”|¬€’Œƒ| |");
	if(int(lpDirectPlay3A)){
		lpDirectPlay3A->EnumPlayers((GUID*)&DPCHAT_GUID,
			IBEnumPlayersCallback2,NULL,0);
	};
	StartMultiplayer.CreateMenu(PlayersList,
		&f32x32w,&f32x32g,NULL,MCP_MAKESHIFT|MCP_CHANGEFONT);
};
int GetPIndex(DPID PD){
	int i=0;
	while(i<NPlayers&&PD!=PlayersID[i])i++;
	if(i<NPlayers)return i;
	else return -1;
};
void HandleApplicationMessage(LPDPLAYINFO lpDPInfo, LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize,
							  DPID idFrom, DPID idTo)
{
	DWORD SDP=idTo==DPID_ALLPLAYERS?MyDPID:idTo;
		if(SDP==MyDPID){
			int ii=GetPIndex(idFrom);
			if(ii<0)return;
			DWORD*   lp =(DWORD*) (LPMSG_CHATSTRING) lpMsg;
			if(lp[0]==ExDataID&&dwMsgSize<2048){
				if(lp[1]==RealTime){
					EBufs[ii].Size=dwMsgSize-12;
					EBufs[ii].RandIndex=lp[2];
					memcpy(&EBufs[ii].Data,&lp[3],dwMsgSize-12);
					EBufs[ii].RealTime=RealTime;
				};
				if(lp[1]==RealTime+1){
					EBufs1[ii].Size=dwMsgSize-12;
					EBufs1[ii].RandIndex=lp[2];
					memcpy(&EBufs1[ii].Data,&lp[3],dwMsgSize-12);
					EBufs1[ii].RealTime=RealTime+1;
				};
			}else{
				if(lp[0]==PlExitID){
					EBufs[ii].Enabled=false;
					sprintf(Prompt,"»„ÓÍ %s ‚˚¯ÂÎ ËÁ Ë„˚.",StartMultiplayer.GetNstr(3+ii));
					PromptTime=400;
				};
				if(MyData)free(MyData);
				MyData=new byte[dwMsgSize];
				memcpy(MyData,lp,dwMsgSize);
				
			};
        };
};

void HandleSystemMessage(LPDPLAYINFO lpDPInfo, LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize,
						 DPID idFrom, DPID idTo)
{
	LPSTR		lpszStr = NULL;

    // The body of each case is there so you can set a breakpoint and examine
    // the contents of the message received.
	switch (lpMsg->dwType)
	{
	case DPSYS_CREATEPLAYERORGROUP:
        {
            LPDPMSG_CREATEPLAYERORGROUP		lp = (LPDPMSG_CREATEPLAYERORGROUP) lpMsg;
			LPSTR							lpszPlayerName;
			LPSTR							szDisplayFormat = "\"%s\" has joined\r\n";
            
			// get pointer to player name
			if (lp->dpnName.lpszShortNameA)
				lpszPlayerName = lp->dpnName.lpszShortNameA;
			else
				lpszPlayerName = "unknown";

			// allocate space for string
			lpszStr = new char[lstrlen(szDisplayFormat) +
												   lstrlen(lpszPlayerName) + 1];
			if (lpszStr == NULL)
				break;

			// build string
			wsprintf(lpszStr, szDisplayFormat, lpszPlayerName);
        }
		break;

	case DPSYS_DESTROYPLAYERORGROUP:
        {
            LPDPMSG_DESTROYPLAYERORGROUP	lp = (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
			LPSTR							lpszPlayerName;
			LPSTR							szDisplayFormat = "\"%s\" has left\r\n";
            
			// get pointer to player name
			if (lp->dpnName.lpszShortNameA)
				lpszPlayerName = lp->dpnName.lpszShortNameA;
			else
				lpszPlayerName = "unknown";

			// allocate space for string
			lpszStr = new char[lstrlen(szDisplayFormat) +
												   lstrlen(lpszPlayerName) + 1];
			if (lpszStr == NULL)
				break;

			// build string
			wsprintf(lpszStr, szDisplayFormat, lpszPlayerName);
        }
		break;

	case DPSYS_ADDPLAYERTOGROUP:
        {
            LPDPMSG_ADDPLAYERTOGROUP lp = (LPDPMSG_ADDPLAYERTOGROUP)lpMsg;
        }
		break;

	case DPSYS_DELETEPLAYERFROMGROUP:
        {
            LPDPMSG_DELETEPLAYERFROMGROUP lp = (LPDPMSG_DELETEPLAYERFROMGROUP)lpMsg;
        }
		break;

	case DPSYS_SESSIONLOST:
        {
            LPDPMSG_SESSIONLOST lp = (LPDPMSG_SESSIONLOST)lpMsg;
        }
		break;

	case DPSYS_HOST:
        {
            LPDPMSG_HOST	lp = (LPDPMSG_HOST)lpMsg;
			LPSTR			szDisplayFormat = "You have become the host\r\n";

			// allocate space for string
			lpszStr = new char[strlen(szDisplayFormat) + 1];
			if (lpszStr == NULL)
				break;

			// build string
			lstrcpy(lpszStr, szDisplayFormat);

			// we are now the host
			lpDPInfo->bIsHost = TRUE;
        }
		break;

	case DPSYS_SETPLAYERORGROUPDATA:
        {
            LPDPMSG_SETPLAYERORGROUPDATA lp = (LPDPMSG_SETPLAYERORGROUPDATA)lpMsg;
        }
		break;

	case DPSYS_SETPLAYERORGROUPNAME:
        {
            LPDPMSG_SETPLAYERORGROUPNAME lp = (LPDPMSG_SETPLAYERORGROUPNAME)lpMsg;
        }
		break;
	}

	// post string to chat window
	if (lpszStr)
	{
		// make sure window is still valid
		//if (ghChatWnd)
		//	PostMessage(ghChatWnd, WM_USER_ADDSTRING, (WPARAM) 0, (LPARAM) lpszStr);
		//else
		free(lpszStr);
	}
}
HRESULT ReceiveMessage(LPDPLAYINFO lpDPInfo){
	DPID				idFrom, idTo;
	LPVOID				lpvMsgBuffer;
	DWORD				dwMsgBufferSize;
	HRESULT				hr;

	lpvMsgBuffer = NULL;
	dwMsgBufferSize = 0;

	// loop to read all messages in queue
	//do
	//{
		// loop until a single message is successfully read
		do
		{
			// read messages from any player, including system player
			idFrom = 0;
			idTo = 0;

			hr = lpDPInfo->lpDirectPlay3A->Receive(&idFrom, &idTo, DPRECEIVE_ALL,
												   lpvMsgBuffer, &dwMsgBufferSize);

			// not enough room, so resize buffer
			if (hr == DPERR_BUFFERTOOSMALL)
			{
				if (lpvMsgBuffer)
					free(lpvMsgBuffer);
				lpvMsgBuffer = new byte[dwMsgBufferSize];
				if (lpvMsgBuffer == NULL)
					hr = DPERR_OUTOFMEMORY;
			}
		} while (hr == DPERR_BUFFERTOOSMALL);

		if ((SUCCEEDED(hr)) &&							// successfully read a message
			(dwMsgBufferSize >= sizeof(DPMSG_GENERIC)))	// and it is big enough
		{
			// check for system message
			if (idFrom == DPID_SYSMSG)
			{
//				HandleSystemMessage(lpDPInfo, (LPDPMSG_GENERIC) lpvMsgBuffer,
//									dwMsgBufferSize, idFrom, idTo);
			}
			else
			{
				HandleApplicationMessage(lpDPInfo, (LPDPMSG_GENERIC) lpvMsgBuffer,
										 dwMsgBufferSize, idFrom, idTo);
			}
		}
	//} while (SUCCEEDED(hr));

	// free any memory we created
	if (lpvMsgBuffer)
		free(lpvMsgBuffer);

	return (DP_OK);

	return 0;
};

HRESULT SetupConnection(HINSTANCE hInstance, LPDPLAYINFO lpDPInfo)
{
	HRESULT		hr;

	ZeroMemory(lpDPInfo, sizeof(DPLAYINFO));
	ZeroMemory(PData,sizeof(PData));
	ZeroMemory(PlayersID,sizeof(PlayersID));
	ZeroMemory(&MyData,sizeof(4*32));
	NPlayers=0;
	ServerDPID=0xFFFFFFFF;
	MyDPID=0;
	GameInProgress=false;
	// create event used by DirectPlay to signal a message has arrived
		return (DP_OK);	

	
	return (hr);
}
void SetupMultiplayer(HINSTANCE hInstance){
	SetupConnection(hInstance,&DPInfo);
	lpDPInfo=&DPInfo;
};
void ShutdownMultiplayer(){
	IAmLeft();
	ShutdownConnection(lpDPInfo);
};
HRESULT ShutdownConnection(LPDPLAYINFO lpDPInfo)
{
	

	if (lpDPInfo->lpDirectPlay3A)
	{
		if (lpDPInfo->dpidPlayer)
		{
			lpDPInfo->lpDirectPlay3A->DestroyPlayer(lpDPInfo->dpidPlayer);
			lpDPInfo->dpidPlayer = 0;
		}
		lpDPInfo->lpDirectPlay3A->Close();
		lpDPInfo->lpDirectPlay3A->Release();
		lpDPInfo->lpDirectPlay3A = NULL;
	}

	
	return (DP_OK);
}

void InitMultiDialogs(){
	//SingleMulti.CreateMenu("Œƒ»ÕŒ◊Õ¿ﬂ »√–¿|»√–¿ œŒ —≈“»|¬€’Œƒ",
	//	&f32x32g,&f32x32r,NULL,MCP_MAKESHIFT|MCP_CHANGEFONT);
	//StartMultiplayer.CreateMenu("Õ¿◊¿“‹ »√–”|¬€’Œƒ",
	//	&f32x32g,&f32x32r,NULL,MCP_MAKESHIFT|MCP_CHANGEFONT);
	NetworkGame=false;
	lpDirectPlay3A = NULL;
	NProviders=0;
	CreateMultiplaterInterface();
	//strcat(ProvidersList,"œÂ‰˚‰Û˘ÂÂ ÏÂÌ˛");
	//ChooseConn.CreateMenu(ProvidersList,
	//	&f16x16w,&f16x16r,NULL,MCP_MAKESHIFT|MCP_CHANGEFONT);
	PlayerMenuMode=1;
	NPlayers=0;
	GameInProgress=false;
};

HRESULT CreateDirectPlayInterface( LPDIRECTPLAY3A *lplpDirectPlay3A )
{
	HRESULT				hr;
	LPDIRECTPLAY3A		lpDirectPlay3A = NULL;
	hr = CoInitialize(NULL);
	// Create an IDirectPlay3 interface
	hr = CoCreateInstance(	CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER, 
							IID_IDirectPlay3A, (LPVOID*)&lpDirectPlay3A);

	// return interface created
	*lplpDirectPlay3A = lpDirectPlay3A;

	return (hr);
};
/*HRESULT DestroyDirectPlayInterface(HWND hWnd, LPDIRECTPLAY3A lpDirectPlay3A)
{
	HRESULT		hr = DP_OK;

	if (lpDirectPlay3A)
	{
		DeleteSessionInstanceList(hWnd);
		EnableDlgButton(hWnd, IDC_JOINBUTTON, FALSE);

		hr = lpDirectPlay3A->Release();
	}

	return (hr);
};*/
BOOL FAR PASCAL DirectPlayEnumConnectionsCallback(
						LPCGUID     lpguidSP,
						LPVOID		lpConnection,
						DWORD		dwConnectionSize,
						LPCDPNAME   lpName,
						DWORD 		dwFlags,
						LPVOID 		lpContext)
{

    HWND			hWnd = (HWND) lpContext;

	// store service provider name in list
	strcat(ProvidersList,lpName->lpszShortNameA);
	strcat(ProvidersList,"|");
	// make space for connection shortcut
	lplpConnectionBuffer[NProviders] = new byte[dwConnectionSize];
	if (lplpConnectionBuffer[NProviders] == NULL)
		goto FAILURE;

	// store pointer to connection shortcut in list
	memcpy(lplpConnectionBuffer[NProviders], lpConnection, dwConnectionSize);
	NProviders++;

FAILURE:
    return (TRUE);
};
bool CreateMultiplaterInterface(){
	if FAILED(CreateDirectPlayInterface(&lpDirectPlay3A)) return false;
	if(NProviders>0){
		for(int i=0;i<NProviders;i++)free(lplpConnectionBuffer[i]);
	};
	ProvidersList[0]=0;
	NProviders=0;
	lpDirectPlay3A->EnumConnections(&DPCHAT_GUID,
		DirectPlayEnumConnectionsCallback,hwnd,0);
	return true;
};
BOOL FAR PASCAL EnumSessionsCallback(
						LPCDPSESSIONDESC2	lpSessionDesc,
						LPDWORD				lpdwTimeOut,
						DWORD				dwFlags,
						LPVOID				lpContext)
{
	HWND			hWnd = (HWND) lpContext;

	// see if last session has been enumerated
    if (dwFlags & DPESC_TIMEDOUT)
		return (FALSE);						

	// store session name in list
	strcat(SessionsList,lpSessionDesc->lpszSessionNameA);
	strcat(SessionsList,"|");
	SessionsGUID[NSessions]=lpSessionDesc->guidInstance;
	NSessions++;
    return (TRUE);
}
void EnumerateSessions(){
	DPSESSIONDESC2		sessionDesc;
	PlayersList[0]=0;
	NPlayers=0;
	SessionsList[0]=0;
	NSessions=0;
	// add sessions to session list
	ZeroMemory(&sessionDesc, sizeof(DPSESSIONDESC2));
	sessionDesc.dwSize = sizeof(DPSESSIONDESC2);
    sessionDesc.guidApplication = DPCHAT_GUID;
	// start enumerating the sessions
	lpDirectPlay3A->EnumSessions(&sessionDesc,0,
		EnumSessionsCallback,hwnd,
		DPENUMSESSIONS_AVAILABLE);
	strcat(SessionsList,"| |—Œ«ƒ¿“‹ —¬Œﬁ »√–”|œ–≈ƒ€ƒ”Ÿ≈≈ Ã≈Õﬁ");
	ChooseSession.CreateMenu(SessionsList,
		&f32x32w,&f32x32g,NULL,MCP_MAKESHIFT|MCP_CHANGEFONT);
};
ListBox* LBBX;
BOOL FAR PASCAL LBEnumSessionsCallback(
						LPCDPSESSIONDESC2	lpSessionDesc,
						LPDWORD				lpdwTimeOut,
						DWORD				dwFlags,
						LPVOID				lpContext)
{
	HWND			hWnd = (HWND) lpContext;

	// see if last session has been enumerated
    if (dwFlags & DPESC_TIMEDOUT)
		return (FALSE);						

	// store session name in list
	LBBX->AddItem(lpSessionDesc->lpszSessionNameA,0);
	SessionsGUID[NSessions]=lpSessionDesc->guidInstance;
	NSessions++;
    return (TRUE);
}
void LBEnumerateSessions(ListBox* LB){
	LBBX=LB;
	LB->ClearItems();
	DPSESSIONDESC2		sessionDesc;
	PlayersList[0]=0;
	NPlayers=0;
	SessionsList[0]=0;
	NSessions=0;
	// add sessions to session list
	ZeroMemory(&sessionDesc, sizeof(DPSESSIONDESC2));
	sessionDesc.dwSize = sizeof(DPSESSIONDESC2);
    sessionDesc.guidApplication = DPCHAT_GUID;
	// start enumerating the sessions
	lpDirectPlay3A->EnumSessions(&sessionDesc,0,
		LBEnumSessionsCallback,hwnd,
		DPENUMSESSIONS_AVAILABLE);
};
HRESULT HostSession(LPDIRECTPLAY3A lpDirectPlay3A,
					LPSTR lpszSessionName, LPSTR lpszPlayerName,
					LPDPLAYINFO lpDPInfo)
{
	DPID				dpidPlayer;
	DPNAME				dpName;
	DPSESSIONDESC2		sessionDesc;
	HRESULT				hr;

	// check for valid interface
	if (lpDirectPlay3A == NULL)
		return (DPERR_INVALIDOBJECT);

	// host a new session
	ZeroMemory(&sessionDesc, sizeof(DPSESSIONDESC2));
	sessionDesc.dwSize = sizeof(DPSESSIONDESC2);
    sessionDesc.dwFlags = DPSESSION_MIGRATEHOST | DPSESSION_KEEPALIVE;
    sessionDesc.guidApplication = DPCHAT_GUID;
    sessionDesc.dwMaxPlayers = MAXPLAYERS;
	sessionDesc.lpszSessionNameA = lpszSessionName;

	hr = lpDirectPlay3A->Open(&sessionDesc, DPOPEN_CREATE);
	if FAILED(hr)
		goto OPEN_FAILURE;

	// fill out name structure
	ZeroMemory(&dpName, sizeof(DPNAME));
	dpName.dwSize = sizeof(DPNAME);
	dpName.lpszShortNameA = lpszPlayerName;
	dpName.lpszLongNameA = NULL;

	// create a player with this name
	hr = lpDirectPlay3A->CreatePlayer(&dpidPlayer, &dpName, 
							lpDPInfo->hPlayerEvent, NULL, 0, 0);
	if FAILED(hr)
		goto CREATEPLAYER_FAILURE;
	MyDPID=dpidPlayer;
	// return connection info
	lpDPInfo->lpDirectPlay3A = lpDirectPlay3A;
	lpDPInfo->dpidPlayer = dpidPlayer;
	lpDPInfo->bIsHost = TRUE;

	return (DP_OK);

CREATEPLAYER_FAILURE:
OPEN_FAILURE:
	lpDirectPlay3A->Close();
	return (hr);
}
HRESULT JoinSession(LPDIRECTPLAY3A lpDirectPlay3A,
					LPGUID lpguidSessionInstance, LPSTR lpszPlayerName,
					LPDPLAYINFO lpDPInfo)
{
	DPID				dpidPlayer;
	DPNAME				dpName;
	DPSESSIONDESC2		sessionDesc;
	HRESULT				hr;

	// check for valid interface
	if (lpDirectPlay3A == NULL)
		return (DPERR_INVALIDOBJECT);

	// join existing session
	ZeroMemory(&sessionDesc, sizeof(DPSESSIONDESC2));
	sessionDesc.dwSize = sizeof(DPSESSIONDESC2);
    sessionDesc.guidInstance = *lpguidSessionInstance;

	hr = lpDirectPlay3A->Open(&sessionDesc, DPOPEN_JOIN);
	if FAILED(hr)
		goto OPEN_FAILURE;

	// fill out name structure
	ZeroMemory(&dpName, sizeof(DPNAME));
	dpName.dwSize = sizeof(DPNAME);
	dpName.lpszShortNameA = lpszPlayerName;
	dpName.lpszLongNameA = NULL;

	// create a player with this name
	hr = lpDirectPlay3A->CreatePlayer(&dpidPlayer, &dpName, 
							lpDPInfo->hPlayerEvent, NULL, 0, 0);
	if FAILED(hr)
		goto CREATEPLAYER_FAILURE;

	// return connection info
	MyDPID=dpidPlayer;
	lpDPInfo->lpDirectPlay3A = lpDirectPlay3A;
	lpDPInfo->dpidPlayer = dpidPlayer;
	lpDPInfo->bIsHost = FALSE;

	return (DP_OK);

CREATEPLAYER_FAILURE:
OPEN_FAILURE:
	lpDirectPlay3A->Close();
	return (hr);
};
bool CreateNamedSession(char* Name){
	// use computer name for session name
	DWORD dwNameSize = MAXNAMELEN;
	if (!GetComputerName(szSessionName, &dwNameSize))
	lstrcpy(szSessionName, "Session");
	// host a new session on this service provider
	HRESULT hr = HostSession(lpDirectPlay3A, szSessionName, Name, lpDPInfo);
	if FAILED(hr)return false;
		else return true;
};
bool CreateSession(){
	// use computer name for session name
	DWORD dwNameSize = MAXNAMELEN;
	if (!GetComputerName(szSessionName, &dwNameSize))
	lstrcpy(szSessionName, "Session");
	// use user name for player name
	dwNameSize = MAXNAMELEN;
	if (!GetUserName(szPlayerName, &dwNameSize))
	lstrcpy(szPlayerName, "unknown");
	// host a new session on this service provider
	HRESULT hr = HostSession(lpDirectPlay3A, szSessionName, szPlayerName, lpDPInfo);
	if FAILED(hr)return false;
		else return true;
};
bool JoinNameToSession(int ns,char* Name){
	DWORD dwNameSize;
	EnumerateSessions();
	if(ns>=NSessions)return false;
	if FAILED(JoinSession(lpDirectPlay3A,
				&SessionsGUID[ns],Name,
				lpDPInfo))return false;
	else return true;
};
bool JoinToSession(int ns){
	DWORD dwNameSize;
	EnumerateSessions();
	if(ns>=NSessions)return false;
	// use user name for player name
	dwNameSize = MAXNAMELEN;
	if (!GetUserName(szPlayerName, &dwNameSize))
	lstrcpy(szPlayerName, "unknown");
	if FAILED(JoinSession(lpDirectPlay3A,
				&SessionsGUID[ns],szPlayerName,
				lpDPInfo))return false;
	else return true;
};
bool SendToAllPlayers(DWORD Size,LPVOID lpData){
	if(!int(lpDirectPlay3A)||NPlayers<2)return false;
	int count=0;
	HRESULT hr;
	int ttt=GetTickCount();
	do{
		hr=lpDirectPlay3A->Send(MyDPID,DPID_ALLPLAYERS,
		DPSEND_GUARANTEED,lpData,Size);
		count++;
	}while(hr!=DP_OK/*&&(GetTickCount()-ttt)<60000*/);
	if(hr!=DP_OK)return false;
	return true;
};
bool SendToServer(DWORD Size,LPVOID lpData){
	if(!int(lpDirectPlay3A)||NPlayers<2)return false;
	int count=0;
	HRESULT hr;
	do{
		hr=lpDirectPlay3A->Send(MyDPID,ServerDPID,0,
		/*DPSEND_GUARANTEED,*/lpData,Size);
		count++;
	}while(hr!=DP_OK&&count<300);
	if(hr!=DP_OK)return false;
	return true;
};
void FreePDatas(){
	for(int i=0;i<NPlayers;i++){
		if(int(PData[i]))free(PData[i]);
		PData[i]=NULL;
	};
};
extern int RealLx;
extern int RealLy;
extern int LastKey;
extern bool KeyPressed;
void CBar(int x,int y,int Lx,int Ly,byte c);
void ComeInGame(){
	//NationalExcange;
	static int nttm[3];
	nttm[0]=0x12345678;
	nttm[1]=MyDPID;
	nttm[2]=MyRace;
	SendToAllPlayers(12,nttm);
	bool allOk=false;
	int myid=-1;
	for(int i=0;i<NPlayers;i++)if(MyDPID==PlayersID[i])myid=i;
	for(i=0;i<8;i++){
		PLNAT[i]=-1;
	};
	PLNAT[myid]=MyRace;
	do{
		ReceiveMessage(lpDPInfo);
		DWORD* lp=(DWORD*)MyData;
		if(lp){
			if(lp[0]==0x12345678){
				for(i=0;i<NPlayers;i++){
					if(lp[1]==PlayersID[i]){
						PLNAT[i]=lp[2];
					};
				};
				allOk=true;
				for(i=0;i<NPlayers;i++){
					if(PLNAT[i]==-1)allOk=false;
				};
			};
		};
		/*
		ProcessMessages();
		int x=(RealLx>>1)-100;
		int y=RealLy>>1;
		char ddd[128];
		CBar(x-20,y-20,200,200,0);
		ShowString(x,y,"Waiting for opponents...",&f16x16w);
		y+=20;
		for(i=0;i<8;i++){
			sprintf(ddd,"PL%d :ID=%d , NAT=%d",i,PlayersID[i],PLNAT[i]);
			ShowString(x,y,ddd,&f16x16w);
			y+=16;
		};
		FlipPages();*/
		if(KeyPressed&&LastKey==VK_F3){
			IAmLeft();
			return;
		};
	}while(!allOk);
	PrepareGameMedia(myid);
	//-----------
	EBPos=0;
	EBPos1=0;
	ExBuf1[0]=ExDataID;
	SetupEBufs();
	RealTime=0;
	ExBuf1[1]=0;
	ExBuf[2]=0;
	rpos=0;
	SendToAllPlayers(12,ExBuf1);
	PlayerMenuMode=0;
};
bool StartGame(){
	DWORD BUF[35];
	if(NPlayers<2)return false;
	ServerDPID=MyDPID;
	BUF[0]=0x037242F3;//start game code
	BUF[1]=MyDPID;
	BUF[2]=NPlayers;
	memcpy(&BUF[3],PlayersID,32*4);
	FreePDatas();
	if(SendToAllPlayers(35*4,BUF)){
		int ttm=GetTickCount();
		bool xx;
		do{
			xx=false;
			for(int j=0;j<NPlayers;j++){
				if(!int(PData[j]))xx=true;
				if(int(PData[j])&&(*(DWORD*)PData)!=0x773F2945) xx=true;
			};
		}while(xx&&GetTickCount()-ttm<200);
		if(!xx){
			ServerDPID=0;
			return false;
		};
		GameInProgress=true;
		StartMultiplayer.Close();
		PlayerMenuMode=0;
		ComeInGame();
		tmtmt=0;
		return true;
	}else return false;
};
bool StartIGame(){
	PrepareToGame();
	DWORD BUF[48];
	if(NPlayers<2)return false;
	//BUF[0]=.....
	//BUF[1]=Nation
	//BUF[2],[3],[4],[5],[6],[7]-map name
	ServerDPID=MyDPID;
	BUF[0]=0x037242F3;//start game code
	BUF[1]=MyRace;
	for(int i=2;i<8;i++)BUF[i]=0;
	strcpy((char*)&BUF[2],CurrentMap);
	BUF[8]=MyDPID;
	BUF[9]=NPlayers;
	memcpy(&BUF[10],PlayersID,32*4);
	FreePDatas();
	if(SendToAllPlayers(42*4,BUF)){
		//int ttm=GetTickCount();
		//bool xx;
		//do{
		//	xx=false;
		//	for(int j=0;j<NPlayers;j++){
		//		if(!int(PData[j]))xx=true;
		//		if(int(PData[j])&&(*(DWORD*)PData)!=0x773F2945) xx=true;
		//	};
		//}while(xx&&GetTickCount()-ttm<200);
		//if(!xx){
		//	ServerDPID=0;
		//	return false;
		//};
		int ttm=GetTickCount();
		bool xx;
		do{
		}while(xx&&GetTickCount()-ttm<400);
		GameInProgress=true;
		StartMultiplayer.Close();
		PlayerMenuMode=0;
		ComeInGame();
		tmtmt=0;
		return true;
	}else return false;
};

void AnalyseMessages(){
	ReceiveMessage(lpDPInfo);
	if(MyDPID!=ServerDPID){
		DWORD* lp=(DWORD*)MyData;
		if(lp){
			if(lp[0]==0x037242F3&&!GameInProgress){
				//start game
				ServerDPID=lp[8];
				NPlayers=lp[9];
				memcpy(PlayersID,&lp[10],32*4);
				strcpy(CurrentMap,(char*)&lp[2]);
				PLNAT[0]=lp[1];
				//DWORD Ready=0x773F2945;
				//SendToServer(4,&Ready);
				GameInProgress=true;
				StartMultiplayer.Close();
				PlayerMenuMode=0;
				ComeInGame();
			};
		};
	};
};
void xAnalyseMessages(){
	ReceiveMessage(lpDPInfo);
	if(MyDPID!=ServerDPID){
		DWORD* lp=(DWORD*)MyData;
		if(lp){
			if(lp[0]==0x037242F3&&!GameInProgress){
				//start game
				ServerDPID=lp[1];
				NPlayers=lp[2];
				memcpy(PlayersID,&lp[3],32*4);
				//DWORD Ready=0x773F2945;
				//SendToServer(4,&Ready);
				GameInProgress=true;
				StartMultiplayer.Close();
				PlayerMenuMode=0;
				ComeInGame();
			};
		};
	};
};
void IAmLeft(){
	DWORD Left=PlExitID;
	SendToAllPlayers(4,(void*)&PlExitID);
};

int GetRLen(char* s,RLCFont* font);
extern int LastKey;
extern bool KeyPressed;
void HandleMultiplayer(){
	if(NPlayers<2)return;
	DWORD ii=GetPIndex(MyDPID);
	if(ii<0||ii>=NPlayers)return;
	EBufs[ii].RealTime=RealTime;
	EBufs[ii].RandIndex=PrevRpos;
	memcpy(&EBufs[ii].Data,&ExBuf1[3],EBPos1);
	EBufs[ii].Size=EBPos1;
	int fff=GetTickCount();
	int yyy;
	//COUNTER=GetTickCount();
	do{
		ReceiveMessage(lpDPInfo);
		yyy=(GetTickCount()-fff)>>6;
		ProcessMessages();
		if(yyy>50){
			char* sss="œ¿”«¿. (F3-ŒÚÒÓÂ‰ËÌÂÌËÂ)";
			int len=GetRLen(sss,&FPassive);
			yyy=100;
			ShowString(smapx+(smaplx<<(Shifter-1))-(len>>1),smapy+(smaply<<(Shifter-1)),sss,&FPassive);
			FlipPages();
			if(KeyPressed){
				if(LastKey==VK_F3){
					yyy=10000;
				};
			};
		};
	}while(PresentEmptyBuf()&&yyy<1000);
	//COUNTER=GetTickCount()-COUNTER;
	if(yyy>=1000){
		sprintf(Prompt,"—‚ˇÁ¸ ‡ÁÓ‚‡Ì‡.");
		PromptTime=300;
		NPlayers=1;
		IAmLeft();
		return;
	};
	EBPos1=EBPos;
	memcpy(&ExBuf1[3],ExBuf,EBPos1);
	ExBuf1[2]=rpos;
	PrevRpos=rpos;
	ExBuf1[1]=RealTime+1;
	ExBuf1[0]=ExDataID;
	SendToAllPlayers(EBPos1+12,ExBuf1);
	EBPos=0;
	UpdateEBufs();
	RealTime++;
};
void CloseMPL(){
	if(int(lpDirectPlay3A))
		lpDirectPlay3A->Close();
	lpDirectPlay3A=NULL;
};
void PlayerMenuWork(){
	if(PlayerMenuMode==1){
		if((!SingleMulti.Active)&&
		   (!ChooseConn.Active)&&
		   (!ChooseSession.Active)&&
		   (!StartMultiplayer.Active))
				SingleMulti.ShowModal();
		if(SingleMulti.Active){
			switch(SingleMulti.choose){
			case 0:
				PlayerMenuMode=2;
				SingleMulti.Close();
				Lpressed=false;
				return;
			case 1:
				SingleMulti.Close();
				ChooseConn.ShowModal();
				Lpressed=false;
				return;
			case 2:
				SingleMulti.Close();
				CloseExplosions();
				void ShutdownMultiplayer();
				PostMessage(hwnd, WM_CLOSE, 0, 0);
				return;
			};
		};
		if(ChooseConn.Active){
			if(ChooseConn.choose==ChooseConn.NItems-1){
				ChooseConn.Close();
				Lpressed=false;
				SingleMulti.ShowModal();
				return;
			};
			if(ChooseConn.choose>=0){
				CreateMultiplaterInterface();
				if FAILED(lpDirectPlay3A->InitializeConnection(
					lplpConnectionBuffer[ChooseConn.choose],0)) return;
				EnumerateSessions();
				ChooseConn.Close();
				Lpressed=false;
				ChooseSession.ShowModal();
				return;
			};
		};
		if(ChooseSession.Active){
			//EnumerateSessions();
			//ChooseSession.Active=true;
			if(ChooseSession.choose==NSessions+3){
				ChooseSession.Close();
				Lpressed=false;
				ChooseConn.ShowModal();
				return;
			};
			if(ChooseSession.choose>=0&&
				ChooseSession.choose<NSessions){
				if(JoinToSession(ChooseSession.choose)){
					ChooseSession.Close();
					Lpressed=false;
					StartMultiplayer.ShowModal();
					return;
				};

			};
			if(ChooseSession.choose==NSessions+2){
				if(CreateSession()){
					ChooseSession.Close();
					Lpressed=false;
					StartMultiplayer.ShowModal();
					return;
				};
			};
		};
		if(StartMultiplayer.Active){
			AnalyseMessages();
			EnumeratePlayers();
			StartMultiplayer.Active=true;
			if(StartMultiplayer.choose==1){
				StartMultiplayer.Close();
				if(int(lpDirectPlay3A))
					lpDirectPlay3A->Close();
				Lpressed=false;
				ChooseConn.ShowModal();
				return;
			};
			if(StartMultiplayer.choose==0){
				StartGame();
				return;
			};
		};
	};
};