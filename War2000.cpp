/*==========================================================================
 *
 *  1997-1998. Andrew (GSC).
 *
 *  The Main cycle of the strategical game
 *  
 *
 ***************************************************************************/

#define NAME "Kernel"
#define TITLE "Game"
int LASTRAND,LASTIND;

//#define WIN32_LEAN_AND_MEAN
#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mgraph.h"
#include "mouse.h"
#include "menu.h"
#include "MapDiscr.h"
#include "multipl.h"
#include "fog.h"
#include "walls.h"
#include "Nature.h"
#include <time.h>
#include "Nucl.h"
#include "TopZones.h"
#include "Megapolis.h"
#include "dialogs.h"
#include <assert.h>
#include "MapEdit.h"
#include "3DSurf.h"
#include "CDirSnd.h"
#include "GSound.h"
#include "Video.h"
#include "dplay.h"
#include "process.h"
#include "direct.h"
extern bool ChangeNation;
extern bool MultiTvar;
extern DPID MyDPID;
extern DPID ServerDPID;
extern int curdx;
extern int curdy;
extern bool CheapMode;
extern int WarSound;
extern int WorkSound;
extern int OrderSound;
extern int MidiSound;
extern int FPSTime;
extern int NMyUnits;
extern int NThemUnits;
void MFix();
void WinnerControl();
bool fixed;
bool MUSTDRAW;
int Shifter;
int Multip;
bool MiniMode;
int AutoTime;
extern int sfVersion;
void OnMouseMoveRedraw();
void GetForces();
bool HealthMode;
#define TIMER_ID        1
#define TIMER_RATE      50
extern int MenuType;
extern bool MakeMenu;
extern byte SpecCmd;
extern int RealLx;
extern int RealLy;
extern bool BuildMode;
extern bool CINFMOD;
extern word rpos;
extern bool realLpressed;
extern bool realRpressed;
extern int MaxAllowedComputerAI;
extern bool GetCoord;
extern int FogMode;
extern CDirSound* CDS;
void ProcessUFO();
void SaveGame(char* fnm,char* gg,int ID);
void LoadGame(char* fnm);
void makeFden();
byte Quality;
HugeExplosion HE;
bool FASTMODE;
void InitDialogs();
int processMainMenu();
//Организация параллельно выполняемых задач
typedef void EventHandPro(void*);
void HandleMultiplayer();
bool EgoFlag;
int tmtmt;
bool MEditMode;
bool EditMapMode;
bool HeightEditMode;
bool HelpMode;
bool ChoosePosition;
CDirSound CDIRSND;
int  ReliefBrush;
int  TerrBrush;
int BlobMode;
int MaxSizeX;
int MaxSizeY;
byte EditMedia;
extern int curptr;
void InitDestn();
Menu MainMenu;
Menu Options;
Menu LoadFile;
Menu About;
word Creator;
Nation WEP;
Weapon FlyFire1;
Weapon Vibux1;
Weapon Fire1;
Weapon Lasso;
Weapon Molnia;
Weapon Sphere;
Weapon MolniaVibux;
Weapon Arrow;
Weapon Flystar;
Weapon Magvib;
byte PlayerMask;
Legion LG;
City CITY[8];
/*City CITY1;
City CITY2;
City CITY3;
City CITY4;
City CITY5;
City CITY6;
City CITY7;*/
static word MsPerFrame=40;
int Flips;
int LastKey;
bool KeyPressed;
int NMONS;
bool DeathMode;
bool AttackMode;
bool InfoMode;
void SetEgoFlag(bool EgoMod);
extern word PlayerMenuMode;
WIN32_FIND_DATA findfi;
struct EventsTag{
	EventHandPro* Pro;
	int	Type;
	int	Handle;
	bool Blocking;
	void* Param;
};
#define maxTask 32
void PlayerMenuWork();
EventsTag Events[maxTask];
int RegisterEventHandler(EventHandPro* pro,int Type,void* param){
	for(int i=0;int(Events[i].Pro)&&i<maxTask;i++);
	if(i>=maxTask)return -1;
	Events[i].Pro=pro;
	Events[i].Type=Type;
	Events[i].Handle=i;
	Events[i].Blocking=false;
	Events[i].Param=param;
	return i;
};
void CloseEventHandler(int i){
	memset(&Events[i],0,sizeof Events[i]);
};

HWND                hwnd;
RLCTable IMM;
RLCTable mIMM;
RLCTable rlc;
RLCFont rlf;
RLCFont rlf1;
RLCTable mon;
RLCTable rlc_s;
RLCFont rlf_s;
RLCFont rlf1_s;
RLCTable mon_s;
RLCTable RCross;
RLCTable mRCross;
//fonts
RLCTable t32x32r;
RLCFont  f32x32r;
RLCTable t32x32g;
RLCFont  f32x32g;
RLCTable t32x32w;
RLCFont  f32x32w;
RLCTable t16x16r;
RLCFont  f16x16r;
RLCTable t16x16y;
RLCFont  f16x16y;
RLCTable t16x16w;
RLCFont  f16x16w;
RLCTable t16x16g;
RLCFont  f16x16g;
int cadr;
int tima;
int tmtim;
/*
 * Timer Callback 
 */
int xxx;
void ShowFon1();
void WaterCorrection();
void TimerProc(void)
{
	
	//LockSurface();
	//FlipPages();
	
	if(PlayerMenuMode==1){
		ShowFon1();
		for(int j=0;j<maxTask;j++)
		if(Events[j].Pro)(*(Events[j].Pro))(Events[j].Param);
		SetRLCWindow(0,1,MaxSizeX,RSCRSizeY-1,ScrWidth);
		//-ShowMPtr();
	}else{
		//ShowMPtr();
		ShowMap();
		SetRLCWindow(0,1,MaxSizeY,RSCRSizeY-1,ScrWidth);
		//-ShowMPtr();
	};
	HandleMouse(mouseX,mouseY);
	MFix();
	FlipPages();
	//UnlockSurface();
	
	
}
//Loading...
void LoadWeapon();
void LoadNation(char* fn,byte msk,byte NIndex);
void Loading()
{
	LoadFon();
	LoadRDS();
	LoadFog();
	LoadTiles();
	LoadWalls();
	LoadLock();
	LoadMap("map13h.mpf");
	LoadPointer("cursor_y.rlc");
	LoadMonsters();
	LoadWeapon();
	LoadNation("PEOPLES.nds",1,0);
	LoadNation("Orcs.nds",2,1);
	LoadNation("PEOPLES.nds",4,2);
	LoadNation("Orcs.nds",8,3);
	LoadNation("orcs.nds",16,4);
	LoadNation("PEOPLES.nds",32,5);
	LoadNation("orcs.nds",64,6);
	LoadNation("PEOPLES.nds",128,7);
	CITY[0].CreateCity(200,30,0);
	CITY[1].CreateCity(190,130,1);
	CITY[2].CreateCity(190,90,2);
	CITY[3].CreateCity(100,80,3);
	CITY[4].CreateCity(50,70,4);
	CITY[5].CreateCity(180,50,5);
	CITY[6].CreateCity(50,180,6);
	CITY[7].CreateCity(180,170,7);
	CITY[4].IntellectEnabled=true;
	CITY[5].IntellectEnabled=true;
	CITY[6].IntellectEnabled=true;
	CITY[7].IntellectEnabled=true;
	LoadPalettes();
	InitPrpBar();
	MyNation=0;
	InitExplosions();
	//HE.CreateExpl(20,20,64,256*8,300);
	//CreateOilSpotAnyway();
	//CreateOilSpotAnyway();
	//CreateOilSpotAnyway();
	//CreateOilSpotAnyway();
	//CreateOilSpotAnyway();
};
void SaveScreen(){
	ResFile ff1=RRewrite("screen.raw");
	for(int i=0;i<RSCRSizeY;i++){
		char* pos=(char*)(int(ScreenPtr)+i*SCRSizeX);
		RBlockWrite(ff1,pos,RSCRSizeX);
	};
	RClose(ff1);
};
/*
 * finiObjects
 *
 * finished with all objects we use; release them
 */
static void finiObjects( void )
{
   FreeDDObjects();
} /* finiObjects */


void MyMCINotify(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  if((wParam==MCI_NOTIFY_SUCCESSFUL)&&(lParam==LPARAM(wAVIDeviceID)))
  {
	  CloseAVI(hwnd);
	  Lpressed=true;
  }
  if((wParam==MCI_NOTIFY_SUCCESSFUL)&&(lParam==LPARAM(wMIDDeviceID)))
  {
	  CloseMIDI(hwnd);
  }
};

long FAR PASCAL WindowProc( HWND hWnd, UINT message, 
                            WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    RECT        rc;
    SIZE        size;
    static BYTE phase = 0;
//    HDC         hdc;
	

    switch( message )
    {
	case MM_MCINOTIFY:
		MyMCINotify(hWnd,wParam,lParam);
		break;
	case WM_LBUTTONDOWN:
		wParam=wParam|MK_LBUTTON;
		Lpressed=true;
		realLpressed=true;
		fixed=false;
		//xLpressed=true;
		SetMPtr(LOWORD(lParam),HIWORD(lParam),wParam);
		break;
	case WM_LBUTTONUP:
		wParam=wParam&!MK_LBUTTON;
		if(fixed)Lpressed=false;
		realLpressed=false;
		//xLpressed=false;
		SetMPtr(LOWORD(lParam),HIWORD(lParam),wParam);
		break;
	case WM_RBUTTONDOWN:
		wParam=wParam|MK_RBUTTON;
		Rpressed=true;
		realRpressed=true;
		fixed=false;
		//xRpressed=true;
		SetMPtr(LOWORD(lParam),HIWORD(lParam),wParam);
		break;
	case WM_RBUTTONUP:
		wParam=wParam&!MK_RBUTTON;
		Rpressed=false;
		if(fixed)Rpressed=false;
		//xRpressed=false;
		SetMPtr(LOWORD(lParam),HIWORD(lParam),wParam);
		break;
	case WM_MOUSEMOVE:
		if(LOWORD(lParam)!=mouseX||HIWORD(lParam)!=mouseY){
			SetMPtr(LOWORD(lParam),HIWORD(lParam),wParam);
			OnMouseMoveRedraw();
		};
		//ShowMPtra();
		break;
    case WM_ACTIVATEAPP:
        bActive = wParam;
		//if(bActive)MUSTDRAW=true;
        break;

    case WM_CREATE:
        break;

    case WM_SETCURSOR:
        SetCursor(NULL);
        return TRUE;

    case WM_TIMER:
        // Flip surfaces
        if( bActive )
        {
			//ShowMPtra();
			//TimerProc();
		}
        break;
	
	
    case WM_KEYDOWN:
		LastKey=wParam;
		KeyPressed=true;
		break;
    case WM_PAINT:
        BeginPaint( hWnd, &ps );
        GetClientRect(hWnd, &rc);
        SetBkColor( ps.hdc, RGB( 0, 0, 255 ) );
        SetTextColor( ps.hdc, RGB( 255, 255, 0 ) );
        EndPaint( hWnd, &ps );
        break;

    case WM_DESTROY:
        finiObjects();
        PostQuitMessage( 0 );
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);

} /* WindowProc */

/*
 * doInit - do work required for every instance of the application:
 *                create the window, initialize data
 */
void EditorKeyCheck();
void GameKeyCheck(){
	if(EditMapMode){
		EditorKeyCheck();
		return;
	};
	if(KeyPressed){
		KeyPressed=false;
		int wParam=LastKey;
			switch( wParam ){
		        case VK_ESCAPE:
					BuildMode=false;
					BuildWall=false;
					GetCoord=false;
					curptr=0;
					curdx=0;
					curdy=0;
					//if(PlayerMenuMode==2)
					//MainMenu.ShowModal();
					//Options.ShowModal();
					break;
				//case 'G':
				//	smapx+=16;
				//	smaplx--;
				//	smapy+=16;
				//	smaply--;
				//	ShowFon();
				//	break;
				//case 'H':
				//	smapx-=16;
				//	smaplx++;
				//	smapy-=16;
				//	smaply++;
				//	ShowFon();
				//	break;
				case 'V':
					//SaveGame("save1.wsf","my game");
					break;
				case 'M':
					//LoadGame("save1.wsf");
					MEditMode=!MEditMode;
					if(!MEditMode)BlobMode=0;
					break;
		       case VK_F12:
					//UnlockSurface();
					//CloseExplosions();
					//ShutdownMultiplayer();
		            //PostMessage(hwnd, WM_CLOSE, 0, 0);
				    MenuType=1;
					MakeMenu=true;
				    break;
				case VK_F1:
					MenuType=4;
					MakeMenu=true;
					//Creator=0;
					break;
				case 192:
					HealthMode=!HealthMode;
					break;
				case VK_F2:
					//Creator=1;
					break;
				case VK_F3:
					//Creator=2;
					break;
				case VK_F4:
					//Creator=3;
					break;
				case VK_F5:
					//Creator=5;
					break;
				case VK_F6:
					//Creator=11;
					break;
				case VK_F7:
					//Creator=13;
					break;
				case VK_F8:
					//Creator=15;
					break;
				case 'D':
					if(GetKeyState(VK_CONTROL)&0x8000)
						SpecCmd=200;//Die
					else SpecCmd=201;//Sit down
					break;
				case 'A':
					if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=1;
					else SpecCmd=2;
					break;
				case 'S':
					SpecCmd=2;
					break;
				case 'W':
					SetBuildWallMode();
					break;
				case 'J':
					//RSCRSizeX++;
					break;
				case 'K':
					//Quality++;
					//if(Quality>=4)Quality=0;
					//RSCRSizeX--;
					break;
				case 'N':
					//switch(RSCRSizeX){
					//	case 800:RSCRSizeX=1024;
					//			break;
					//	case 1024:RSCRSizeX=800+32;
					//			break;
					//	case 1280:RSCRSizeX=1600;
					//		break;
					//	case 1600:RSCRSizeX=800;
					//		break;
					//	default:
					//		RSCRSizeX=800;
					//};
					/*if(RSCRSizeX!=1024)RSCRSizeX=1024;
					else RSCRSizeX=800;*/
					break;
				case 'C':
					CINFMOD=!CINFMOD;
					break;
				//case 'F':
				//	//FASTMODE=true;
				//	break;
				case 'G':
					SaveScreen();
					break;
				//	//FASTMODE=false;
				//	break;
				case 'X':
					if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=5;
					else SpecCmd=6;
					break;
				case 'Q':
					if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=3;
					else SpecCmd=4;
					break;
				case 'B':
					if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=9;
					else SpecCmd=10;
					break;
				case 'Z':
					if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=11;
					else SpecCmd=12;
					break;
				case 'F':
					if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=13;
					else SpecCmd=14;
					break;
				case VK_F9:
					Creator=255;
					break;
				case VK_NUMPAD1:
					if(MEditMode)EditMedia=0;
					else{
						if(ChangeNation)MyNation=0;
						PlayerMask=1;
					};
					break;
				case VK_NUMPAD2:
					if(MEditMode)EditMedia=1;
					else{
						if(ChangeNation)MyNation=1;
						PlayerMask=2;
					};
					break;
				case VK_NUMPAD3:
					if(MEditMode)EditMedia=2;
					else{
						if(ChangeNation)MyNation=2;
						PlayerMask=4;
					};
					break;
				case VK_NUMPAD4:
					if(MEditMode)EditMedia=3;
					else{
						if(ChangeNation)MyNation=3;
						PlayerMask=8;
					};
					break;
				case VK_NUMPAD5:
					if(MEditMode)EditMedia=4;
					else{
						if(ChangeNation)MyNation=4;
						PlayerMask=16;
					};
					break;
				case VK_NUMPAD6:
					if(MEditMode)BlobMode=1;
					else{
						if(ChangeNation)MyNation=5;
						PlayerMask=32;
					};
					break;
				case VK_NUMPAD7:
					if(MEditMode)BlobMode=-1;
					else{
						if(ChangeNation)MyNation=6;
						PlayerMask=64;
					};
					break;
				case VK_NUMPAD8:
					if(ChangeNation)MyNation=7;
					PlayerMask=128;
					break;
				case 'I':
					InfoMode=!InfoMode;
				break;
				case VK_CAPITAL:
					EgoFlag=!EgoFlag;
					SetEgoFlag(EgoFlag);
					break;
				case 'O':
					if(MsPerFrame)MsPerFrame--;
					break;
				case 'P':
					//MsPerFrame++;
					//if(GetKeyState(VK_CONTROL)&0x8000)RotatePhiI();
					//RotatePhi();
					if(MultiTvar){
						MakeMenu=true;
						MenuType=2;
					};
					break;
				case 13:
					MakeMenu=true;
					MenuType=3;
					break;
				case 'L':
					//LG.CreateLegionFromSelection(PlayerMask);
					if(!MiniMode)SetMiniMode();
					else ClearMiniMode();
					break;
				default:
					if(wParam>='0'&&wParam<='9'){
						if(GetKeyState(VK_CONTROL)&0x8000)
							CmdMemSelection(MyNation,wParam-'0');
						else CmdRememSelection(MyNation,wParam-'0');
					};
			 };
	};
};
void EditorKeyCheck(){
	if(KeyPressed){
		KeyPressed=false;
		int wParam=LastKey;
			switch( wParam ){
				case '1':
					if(HeightEditMode)ReliefBrush=1;
					else TerrBrush=1;
					break;
				case '2':
					if(HeightEditMode)ReliefBrush=2;
					else TerrBrush=2;
					break;
				case '3':
					if(HeightEditMode)ReliefBrush=1;
					else TerrBrush=3;
					break;
				case '4':
					if(HeightEditMode)ReliefBrush=4;
					else TerrBrush=4;
					break;
				case '5':
					if(HeightEditMode)ReliefBrush=5;
					else TerrBrush=5;
					break;
				case '6':
					if(HeightEditMode)ReliefBrush=6;
					else TerrBrush=6;
					break;
				case '7':
					if(HeightEditMode)ReliefBrush=7;
					else TerrBrush=7;
					break;
				case '8':
					if(HeightEditMode)ReliefBrush=8;
					else TerrBrush=8;
					break;
				case '9':
					if(HeightEditMode)ReliefBrush=9;
					else TerrBrush=9;
					break;
		        case VK_ESCAPE:
					BuildMode=false;
					BuildWall=false;
					GetCoord=false;
					curptr=0;
					curdx=0;
					curdy=0;
					//if(PlayerMenuMode==2)
					//MainMenu.ShowModal();
					//Options.ShowModal();
					break;
		       case VK_F12:
					//UnlockSurface();
					//CloseExplosions();
					//ShutdownMultiplayer();
		            //PostMessage(hwnd, WM_CLOSE, 0, 0);
					MenuType=1;
					MakeMenu=true;
				    break;
				case 192:
					HealthMode=!HealthMode;
					break;
			   case VK_F1:
					HelpMode=!HelpMode;
				    break;
			   case VK_F2:
					HeightEditMode=false;
					MEditMode=true;
					EditMedia=0;
					break;
				case VK_F3:
					HeightEditMode=false;
					MEditMode=true;
					EditMedia=1;
					break;
				case VK_F4:
					HeightEditMode=false;
					MEditMode=true;
					EditMedia=2;
					break;
				case VK_F5:
					HeightEditMode=false;
					MEditMode=true;
					EditMedia=3;
					break;
				case 'F':
					HeightEditMode=false;
					MEditMode=true;
					EditMedia=5;
					break;
				case VK_F6:
					HeightEditMode=false;
					MEditMode=true;
					EditMedia=4;
					break;
				case VK_F7:
					HeightEditMode=true;
					MEditMode=false;
					BlobMode=1;
					break;
				case VK_F8:
					HeightEditMode=true;
					MEditMode=false;
					BlobMode=-1;
					break;
				case 'N':
					HeightEditMode=false;
					MEditMode=true;
					EditMedia=5;
					break;
				case 'D':
					if(GetKeyState(VK_CONTROL)&0x8000)
						SpecCmd=200;//Die
					else SpecCmd=201;//Sit down
					break;
				case 'A':
					if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=1;
					else SpecCmd=2;
					break;
				case 'S':
					MEditMode=false;
					SpecCmd=2;
					break;
				case 'W':
					MEditMode=false;
					HeightEditMode=false;
					SetBuildWallMode();
					break;
					/*
				case 'N':
					switch(RSCRSizeX){
						case 800:RSCRSizeX=1024;
								break;
						case 1024:RSCRSizeX=800+32;
								break;
						case 1280:RSCRSizeX=1600;
							break;
						case 1600:RSCRSizeX=800;
							break;
						default:
							RSCRSizeX=800;
					};*/
					/*if(RSCRSizeX!=1024)RSCRSizeX=1024;
					else RSCRSizeX=800;*/
					//break;
				//case 'C':
				//	CINFMOD=!CINFMOD;
				//	break;
				case 'X':
					if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=5;
					else SpecCmd=6;
					break;
				case 'Q':
					if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=3;
					else SpecCmd=4;
					break;
				case 'B':
					if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=9;
					else SpecCmd=10;
					break;
				case 'Z':
					if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=11;
					else SpecCmd=12;
					break;
				//case VK_F11:
				//	WaterCorrection();
				//	break;
				case VK_F9:
					MEditMode=false;
					HeightEditMode=false;
					Creator=255;
					break;
				case VK_NUMPAD1:
					MyNation=0;
					MEditMode=false;
					HeightEditMode=false;
					PlayerMask=1;
					break;
				case VK_NUMPAD2:
					MyNation=1;
					MEditMode=false;
					HeightEditMode=false;
					PlayerMask=2;
					break;
				case VK_NUMPAD3:
					MyNation=2;
					MEditMode=false;
					HeightEditMode=false;
					PlayerMask=4;
					break;
				case VK_NUMPAD4:
					MyNation=3;
					MEditMode=false;
					HeightEditMode=false;
					PlayerMask=8;
					break;
				case VK_NUMPAD5:
					MyNation=4;
					MEditMode=false;
					HeightEditMode=false;
					PlayerMask=16;
					break;
				case VK_NUMPAD6:
					MyNation=5;
					MEditMode=false;
					HeightEditMode=false;
					PlayerMask=32;
					break;
				case VK_NUMPAD7:
					MyNation=6;
					MEditMode=false;
					HeightEditMode=false;
					PlayerMask=64;
					break;
				case VK_NUMPAD8:
					MyNation=7;
					MEditMode=false;
					HeightEditMode=false;PlayerMask=128;
					break;
				case 'I':
					InfoMode=!InfoMode;
					break;
				case 'O':
					WaterCorrection();
					//if(MsPerFrame)MsPerFrame--;
					break;
				case 'P':
					//MsPerFrame++;
					//if(GetKeyState(VK_CONTROL)&0x8000)RotatePhiI();
					//RotatePhi();
					MEditMode=false;
					HeightEditMode=false;
					MakeMenu=true;
					MenuType=2;
					break;
				case 'R':
					MEditMode=false;
					HeightEditMode=false;
					MakeMenu=true;
					MenuType=3;
					break;
				case 'L':
					if(!MiniMode)SetMiniMode();
					else ClearMiniMode();
					MEditMode=false;
					break;
				case 'T':
					HeightEditMode=false;
					ChoosePosition=true;
					break;
				case 'G':
					SaveScreen();
					break;
				default:
					if(wParam>='0'&&wParam<='9'){
						if(GetKeyState(VK_CONTROL)&0x8000)
							CmdMemSelection(MyNation,wParam-'0');
						else CmdRememSelection(MyNation,wParam-'0');
					};
			 };
	};
};
void SERROR();
void SERROR1();
void SERROR2();
bool PalDone;
bool InitScreen(){
	PalDone=false;
	CreateDDObjects(hwnd); 
	PalDone=false;
	LoadPalette("darkw.pal");
    if(!DDError)
    {
		LockSurface();
		UnlockSurface();
		//LockSurface();
		//UnlockSurface();
		ShowCursor(true);
		RECT OOO;
		OOO.left=-16;
		OOO.top=-16;
		OOO.right=1500;
		OOO.bottom=1200;
		ClipCursor(&OOO);
		SetCursorPos(4000,2000);//!!!!!!!!!!!!!!!!
		ShowCursor(false);
		OOO.left=0;
		OOO.top=0;
		OOO.right=RealLx-1;
		OOO.bottom=RealLy-1;
		ClipCursor(&OOO);
		return true;
    }else{
		PlayEffect(0,0,0);
	};
	return false;
};
BOOL CreateRGBDDObjects(HWND hwnd);
bool InitRGBScreen(){
	PalDone=false;
	CreateRGBDDObjects(hwnd); 
	PalDone=false;
    if(!DDError)
    {
		LockSurface();
		UnlockSurface();
		//LockSurface();
		//UnlockSurface();
		ShowCursor(true);
		RECT OOO;
		OOO.left=-16;
		OOO.top=-16;
		OOO.right=1500;
		OOO.bottom=1200;
		ClipCursor(&OOO);
		SetCursorPos(4000,2000);//!!!!!!!!!!!!!!!!
		ShowCursor(false);
		OOO.left=0;
		OOO.top=0;
		OOO.right=RealLx-1;
		OOO.bottom=RealLy-1;
		ClipCursor(&OOO);
		return true;
	};
	return false;
};
BOOL CreateRGB640DDObjects(HWND hwnd);
bool InitRGB640Screen(){
	CreateRGB640DDObjects(hwnd); 
    if(!DDError)
    {
		LockSurface();
		UnlockSurface();
		//LockSurface();
		//UnlockSurface();
		ShowCursor(true);
		RECT OOO;
		OOO.left=-16;
		OOO.top=-16;
		OOO.right=1500;
		OOO.bottom=1200;
		ClipCursor(&OOO);
		SetCursorPos(4000,2000);//!!!!!!!!!!!!!!!!
		ShowCursor(false);
		OOO.left=0;
		OOO.top=0;
		OOO.right=RealLx-1;
		OOO.bottom=RealLy-1;
		ClipCursor(&OOO);
		return true;
	};
	return false;
};
bool ProcessMessages();
bool xLockMouse;
void PlayVideo(){
	FreeDDObjects();
	//InitRGB640Screen();
	MiniMode=false;
	CDIRSND.~CDirSound();
	LPCSTR Vid=GetCDROMWithFile("Video\\warVideo.exe");
	if(strcmp(Vid,"none")){
		char fnn[128];
		char cwd[128];
		_getcwd(cwd,128);
		sprintf(fnn,"%sVideo",Vid);
		_chdir(fnn);
		sprintf(fnn,"%sVideo\\warVideo.exe",Vid);
		_spawnl(_P_WAIT,fnn,"warVideo.exe",NULL);
		_chdir(cwd);
	};
//	PlayAVI(hwnd,"e:\T_Z211.avi");
//	xLockMouse=true;
//	do{
//		ProcessMessages();
//	}while(!(KeyPressed));
//	CloseAVI(hwnd);
	Lpressed=false;
	KeyPressed=false;
	//FreeDDObjects();
	InitScreen();
	CDIRSND.CreateDirSound(hwnd);
	CDS=&CDIRSND;
	LoadSounds("SoundList.txt");
	xLockMouse=false;
};
extern bool Lpressed;
static BOOL doInit( HINSTANCE hInstance, int nCmdShow )
{

    WNDCLASS            wc;
    HRESULT             ddrval;
    HDC                 hdc;
    char                buf[256];

    /*
     * set up and register window class
     */
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( hInstance, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NAME;
    wc.lpszClassName = NAME;
    RegisterClass( &wc );
    
    /*
     * create a window
     */
    hwnd = CreateWindowEx(
        WS_EX_APPWINDOW/*TOPMOST*/,
        NAME,
        TITLE,
        WS_POPUP,
        0, 0,
        GetSystemMetrics( SM_CXSCREEN ),
        GetSystemMetrics( SM_CYSCREEN ),
        NULL,
        NULL,
        hInstance,
        NULL );

    if( !hwnd )
    {
        return FALSE;
    }
	//UpdateWindow( hwnd );
    ShowWindow( hwnd, SW_SHOWNORMAL);//nCmdShow );
    UpdateWindow( hwnd );
	CDIRSND.CreateDirSound(hwnd);
	CDS=&CDIRSND;
	LoadSounds("SoundList.txt");
	//SetCapture(hwnd);
	Loading();
	//HCURSOR mcr=LoadCursorFromFile("curs1.cur");
	//SetCursor(mcr);
	//ShowCursor(true);
	SetCursorPos(4000,2000);//!!!!!!!!!!!!!!!!
	ShowCursor(false);

	//HANDLE hh1=LoadImage(NULL,"mcur.bmp",IMAGE_BITMAP,32,32,LR_LOADFROMFILE);
	CurrentSurface=FALSE;
	
    /*
     * create the main DirectDraw object
     */
	PalDone=false;
/*	InitScreen();
	InitRGBScreen();
	xLockMouse=true;
	ResFile ff1=RReset("war2000.raw");
	for(int i=0;i<600;i++){
		RBlockRead(ff1,(char*)(int(RealScreenPtr)+4*800*i),4*800);
	};
	RClose(ff1);
	do{
		ProcessMessages();
	}while(!(Lpressed||KeyPressed));
	xLockMouse=false;
	FreeDDObjects();*/
	KeyPressed=false;
	CreateDDObjects(hwnd); 
	LoadPalette("darkw.pal");
    if(!DDError)
    {
		//hdc=GetSDC();
        //lpDDSPrimary->ReleaseDC(hdc);
		//FlipPages();
		//hdc=GetSDC();
        //lpDDSBack->ReleaseDC(hdc);
		LockSurface();
		//ShowFon();
		//ShowMap();
		//ShowMPtr();
		UnlockSurface();
		//FlipPages();
		LockSurface();
		//ShowFon();
		//ShowMap();
		//ShowMPtr();
		UnlockSurface();
		//HCURSOR mcrs=LoadCursorFromFile("cursor1.cur");
		//SetCursor(mcrs);
		//SetCursor(mcr);
		// Create a timer to flip the pages
		ShowCursor(true);
		RECT OOO;
		OOO.left=-16;
		OOO.top=-16;
		OOO.right=1500;
		OOO.bottom=1200;
		ClipCursor(&OOO);
		SetCursorPos(4000,2000);//!!!!!!!!!!!!!!!!
		ShowCursor(false);
		OOO.left=0;
		OOO.top=0;
		OOO.right=RealLx-1;
		OOO.bottom=RealLy-1;
		ClipCursor(&OOO);
        if( SetTimer(hwnd, TIMER_ID, 20, NULL ) )
        {
            return TRUE;
        }
		//FlipPages();
    }
    wsprintf(buf, "Direct Draw Init Failed (%08lx)\n", ddrval );
    MessageBox( hwnd, buf, "ERROR", MB_OK );
    finiObjects();
    DestroyWindow( hwnd );
    return FALSE;
}; /* doInit */
void HandleAntiNuc();
void AddDestn(byte x,byte y);
int PrevTime;
void PreDrawGameProcess(){
	WinnerControl();
	PrevTime=GetTickCount();
	InitDestn();
	if(tima!=time(NULL)){
		Flips=tmtim;	
		tmtim=0;
		tima=time(NULL);
	};
	if(!(tmtmt&63)){
		LASTRAND=rando();
		LASTIND=rpos;
	};
	tmtim++;
	tmtmt++;
	GameKeyCheck();
//	GETALL();
	NMONS=0;
	int tmm=tmtmt&7;
	if(!tmm)CITY[0].ProcessCreation();
	if(tmm==1)CITY[1].ProcessCreation();
	if(tmm==2)CITY[2].ProcessCreation();
	if(tmm==3)CITY[3].ProcessCreation();
	if(tmm==4)CITY[4].ProcessCreation();
	if(tmm==5)CITY[5].ProcessCreation();
	if(tmm==6)CITY[6].ProcessCreation();
	if(tmm==7)CITY[7].ProcessCreation();
	//Computer AI handling
	if(tmm==0){
		GetForces();
		//Calculate maximal allowed computer AI
		int MaxHumAI=0;
		for(int i=0;i<8;i++){
			Nation* NT=&NATIONS[i];
			if(!NT->CITY&&NForces[i].StrongFactor>MaxAllowedComputerAI)
				MaxAllowedComputerAI=NForces[i].StrongFactor;
		};
	};
	PrepareProcessing();
	for(int i=0;i<MAXOBJECT;i++)if(int(Group[i])){
		Group[i]->MakePreProcess();
	};
	MakePostProcess();
	PrepareProcessing();
	for(i=0;i<MAXOBJECT;i++){
		OneObject* OB=Group[i];
		if(OB){
			if(OB->DstX&&OB->Selected&&OB->NNUM==MyNation)AddDestn(OB->DstX,OB->DstY);
			OB->MakeWPreProcess();
		};
	};
	MakeWPostProcess();
	ClearFlyMarkers();
	for(i=0;i<MAXOBJECT;i++)if(int(Group[i])){
		Group[i]->ProcessFly();
	};
	SetFlyMarkers();
	CarryOutOrder();
	LG.Process();
	GWALLS.ProcessClusters();
	HandleAntiNuc();
	if(!tmm)ProcessUFO();
	//TimerProc();
	//HandleMultiplayer();
	//ExecuteBuffer();
	//HandleHExpls();
	//HandleSW();
	//return true;
	//CHKALL();
};
bool ProcessMessages();
extern word NPlayers;;
void CmdSaveNetworkGame(byte NI,int ID,char* Name);
int SaveTime;
extern char SaveFileName[128];
void PostDrawGameProcess(){
	HandleMultiplayer();
	ExecuteBuffer();
	//HandleHExpls();
	HandleSW();
	int difTime=GetTickCount()-AutoTime;
	if(difTime>60000){
		if(NPlayers>1){
			if(MyDPID==ServerDPID)
				CmdSaveNetworkGame(MyNation,GetTickCount(),SaveFileName);
			 //SaveGame("saveNETW.sav","NETWORK SAVE",0);
		}else SaveGame("saveAUTO.sav","АВТОСОХРАНЕНИЕ",0);
	    AutoTime=GetTickCount();
	};
	/*
	if(NPlayers>1&&MyDPID==ServerDPID&&SaveTime-GetTickCount()>60000*5){
		CmdSaveNetworkGame(MyNation,GetTickCount(),"NETWORK SAVE");
		SaveTime=GetTickCount();
	};
	*/
	do{
		ProcessMessages(); 
	}while(GetTickCount()-PrevTime<FPSTime);
};

/*
 * WinMain - initialization, message loop
 */
void AllGame();
void PrepareToEdit(){
	ReliefBrush=3;
	TerrBrush=2;
	EditMedia=0;
	HeightEditMode=false;
	MEditMode=false;
	EditMapMode=true;
	FogMode=0;
	HelpMode=true;
	ChoosePosition=false;
	CheapMode=false;
	NMyUnits=1;
	NThemUnits=1;
	AutoTime=GetTickCount()+180000;
};
void PrepareToGame(){
	ReliefBrush=3;
	TerrBrush=2;
	EditMedia=0;
	HeightEditMode=false;
	MEditMode=false;
	EditMapMode=false;
	FogMode=1;
	HelpMode=false;
	ChoosePosition=false;
	CheapMode=false;
	NMyUnits=1;
	NThemUnits=1;
	SaveTime=GetTickCount();
	AutoTime=GetTickCount()+180000;
};
void ShowIntro(){

};
void SetupRenderStack();
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                        LPSTR lpCmdLine, int nCmdShow)
{
	MaxSizeX=1600;
	MaxSizeY=1200;
	Shifter=5;
	Multip=0;
	SetupRenderStack();
	ChangeNation=false;
	MultiTvar=false;
	AutoTime=0;
	BlobMode=0;
	MEditMode=false;
	EditMedia=0;
	LoadMediaFile();
	SpecCmd=0;
	sfVersion=15;
	Quality=2;
	RealLx=1024;
	RealLy=768;
	WarSound=0;
	WorkSound=0;
	OrderSound=0;
	MidiSound=0;
	FPSTime=50;
	FILE* fff=fopen("mode.dat","rt");
	if(fff){
		fscanf(fff,"%d%d%d%d%d%d%d",&RealLx,&RealLy,&WarSound,&OrderSound,&OrderSound,&MidiSound,&FPSTime);
		fclose(fff);
	};
	HealthMode=false;				
	tima=0;
	PlayerMask=1;
	Flips=0;
	tmtim=0;
	InfoMode=true;
    MSG         msg;
	DeathMode=false;
	AttackMode=false;
	NoDebugMode();
	makeFden();
	PlayerMenuMode=1;
	InitStack();
	Creator=255;
	xxx=0;
	cadr=0;
	InitZones();
	InitEBuf();
	InitDialogs();
	MUSTDRAW=false;
	LoadRLC("xrcross.rlc",&RCross);
	LoadRLC("xrcross.rlx",&mRCross);
	LoadRLC("64x64dc9.rlc",&rlc);
	LoadRLC("64x64dc4.rlc",&mon);
	LoadRLC("16x16dr1.rlc",&rlc_s);
	LoadRLC("16x16dr1.rlc",&mon_s);

	LoadRLC("32x32r.rlc",&t32x32r);
	LoadRLC("32x32g.rlc",&t32x32g);
	LoadRLC("32x32w.rlc",&t32x32w);
	LoadRLC("16x16r.rlc",&t16x16r);
	LoadRLC("16x16y.rlc",&t16x16y);
	LoadRLC("16x16w.rlc",&t16x16w);
	LoadRLC("16x16g.rlc",&t16x16g);
	LoadRLC("img.rlc",&IMM);
	LoadRLC("img.rlx",&mIMM);
	RegisterRLCFont(&rlf,rlc,0);
	RegisterRLCFont(&rlf1,mon,0);
	RegisterRLCFont(&rlf_s,rlc_s,0);
	RegisterRLCFont(&rlf1_s,mon_s,0);

	RegisterRLCFont(&f32x32r,t32x32r,0);
	RegisterRLCFont(&f32x32g,t32x32g,0);
	RegisterRLCFont(&f32x32w,t32x32w,0);
	RegisterRLCFont(&f16x16r,t16x16r,0);
	RegisterRLCFont(&f16x16y,t16x16y,0);
	RegisterRLCFont(&f16x16w,t16x16w,0);
	RegisterRLCFont(&f16x16g,t16x16g,0);
	memset(Events,0,sizeof Events);
	MainMenu.CreateMenu("LOAD GAME|SAVE GAME|OPTIONS|ABOUT|EXIT",&rlf1,&rlf,NULL,
		MCP_MAKESHIFT|MCP_CHANGEFONT);
	Options.CreateMenu("SPEED|SOUND|MAIN MENU",&rlf1,&rlf,NULL,
		MCP_MAKESHIFT|MCP_CHANGEFONT);
	LoadFile.CreateMenu("MAIN MENU",&rlf1_s,&rlf_s,NULL,
						MCP_MAKESHIFT|MCP_CHANGEFONT);
	About.CreateMenu("THIS GAME|WAS CREATED|BY ANDREW SHPAGIN|IN GAME WORLD| |MAIN MENU",&rlf1_s,&rlf_s,NULL,
						MCP_MAKESHIFT|MCP_CHANGEFONT);
    if( !doInit( hInstance, nCmdShow ) )
    {
        return FALSE;
    }
	SetupMultiplayer(hInstance);
	InitMultiDialogs();
	SetupHint();
	tmtmt=0;
	//processMainMenu();
	KeyPressed=false;
	while (TRUE)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				PostQuitMessage(msg.wParam);
				return 1;
			}

        
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(bActive){
			//int Ticki=GetTickCount();	
			//while(GetTickCount()-Ticki<MsPerFrame);
			AllGame();
			CloseExplosions();
			ShutdownMultiplayer();
			FILE* fff=fopen("mode.dat","wt");
			if(fff){
				fprintf(fff,"%d %d %d %d %d %d %d",RealLx,RealLy,WarSound,OrderSound,OrderSound,MidiSound,FPSTime);
				fclose(fff);
			};
			//CDS->~CDirSound();
		    PostMessage(hwnd, WM_CLOSE, 0, 0);
		};
	};
    return msg.wParam;

}; /* WinMain */
