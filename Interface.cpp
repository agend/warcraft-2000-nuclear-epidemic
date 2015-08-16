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
#include "Dialogs.h"
#include "fonts.h"
#include "dpchat.h"
#include "dplobby.h"
#include "GSound.h"
#include <assert.h>
int time1,time2,time3,time4,time5;
extern int time8;
void processMLoadGame();
extern char GameName[128];
bool ContinueGame;
extern word rpos;
char CurrentMap[64];
void CBar(int x,int y,int Lx,int Ly,byte c);
void PrepareEditMedia(byte myid);
extern bool EditMapMode;
extern int RES[8][8];
extern byte mapPos[16];
void PreLoadExtendedMap(char* s);
void PostLoadExtendedMap(char* s);
void InitGame();
int nsmaplx;
int nsmaply;
int GameMode;
extern int LastKey;
extern bool KeyPressed;
void MFix();
SQPicture MainMenuGround;
SQPicture Panel1;
SQPicture r128x20e;
SQPicture r128x20p;
SQPicture r80x20e;
SQPicture r80x20p;
SQPicture w106x28d;
SQPicture r106x28e;
SQPicture r106x28p;
SQPicture w164x28d;
SQPicture r164x28e;
SQPicture r164x28p;
SQPicture w224x28d;
SQPicture r224x28e;

SQPicture sb_up_e;
SQPicture sb_up_p;
SQPicture sb_up_d;

SQPicture sb_dn_e;
SQPicture sb_dn_p;
SQPicture sb_dn_d;

SQPicture sb_lf_e;
SQPicture sb_lf_p;
SQPicture sb_lf_d;

SQPicture sb_ri_e;
SQPicture sb_ri_p;
SQPicture sb_ri_d;

SQPicture sb_vbar;
SQPicture sb_vbare;
SQPicture sb_hbar;
SQPicture sb_hbare;

SQPicture sb_btn;

SQPicture chk1_on;
SQPicture chk1_off;
SQPicture chk2_on;
SQPicture chk2_off;

SQPicture w160x160;
SQPicture w160x256;
SQPicture y288x256;
SQPicture y352x352;
SQPicture y288x128;
SQPicture w288x256;
SQPicture w352x352;
SQPicture w288x128;

RLCTable TPassive;
RLCTable TActive;
RLCTable TDisable;
RLCFont FPassive;
RLCFont FActive;
RLCFont FDisable;
extern bool CheapMode;
int PanelLx;
int PanelLy;
bool MultiplayerStart;
bool GameNeedToDraw;
extern bool ChoosePosition;
extern word NPlayers;
void CloseMPL();
void IBEnumeratePlayers(InputBox** IB);
void LBEnumerateSessions(ListBox* LB);
void AnalyseMessages();
bool CreateNamedSession(char* Name);
bool InGame;
bool JoinNameToSession(int ns,char* Name);
void FreeDDObjects( void );
bool StartIGame();
byte MyRace;
extern LPDIRECTPLAY3A		lpDirectPlay3A;
extern word PlayerMenuMode;
extern LPVOID lplpConnectionBuffer[16];
void PlayVideo();
bool ProcessMessages(){
	MSG         msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
		if (msg.message == WM_QUIT)
		{
			//FreeDDObjects();
			//PostQuitMessage(msg.wParam);
			return true;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return false;
};

char PlName[64];
void ProcessHelp();
void InitDialogs(){
	unsigned long dws=20;
	GetComputerName(PlName,&dws);
	LoadRLC("fpassive.rlc",&TPassive);
	LoadRLC("factive.rlc",&TActive);
	LoadRLC("fdisable.rlc",&TDisable);
	RegisterRLCFont(&FPassive,TPassive,0);
	RegisterRLCFont(&FActive,TActive,0);
	RegisterRLCFont(&FDisable,TDisable,0);
	MainMenuGround.LoadPicture("zast.bpx");
	Panel1.LoadPicture("panel1.bpx");
	PanelLx=Panel1.PicPtr[0];
	PanelLy=Panel1.PicPtr[1];
	//SQPicture r128x20e;
	r128x20p.LoadPicture("r128x20p.bpx");
	r128x20e.LoadPicture("r128x20e.bpx");
	r80x20e.LoadPicture("r80x20e.bpx");
	r80x20p.LoadPicture("r80x20p.bpx");
	w106x28d.LoadPicture("w106x28d.bpx");
	r106x28e.LoadPicture("r106x28e.bpx");
	r106x28p.LoadPicture("r106x28p.bpx");
	w164x28d.LoadPicture("w164x28d.bpx");
	r164x28e.LoadPicture("r164x28e.bpx");
	r164x28p.LoadPicture("r164x28p.bpx");
	w224x28d.LoadPicture("w224x28d.bpx");
	r224x28e.LoadPicture("r224x28e.bpx");

	sb_up_e.LoadPicture("sb_up_e.bpx");
	sb_up_p.LoadPicture("sb_up_p.bpx");
	sb_up_d.LoadPicture("sb_up_d.bpx");

	sb_dn_e.LoadPicture("sb_dn_e.bpx");
	sb_dn_p.LoadPicture("sb_dn_p.bpx");
	sb_dn_d.LoadPicture("sb_dn_d.bpx");

	sb_lf_e.LoadPicture("sb_lf_e.bpx");
	sb_lf_p.LoadPicture("sb_lf_p.bpx");
	sb_lf_d.LoadPicture("sb_lf_d.bpx");

	sb_ri_e.LoadPicture("sb_ri_e.bpx");
	sb_ri_p.LoadPicture("sb_ri_p.bpx");
	sb_ri_d.LoadPicture("sb_ri_d.bpx");

	sb_vbar.LoadPicture("sb_vbar.bpx");
	sb_vbare.LoadPicture("sb_vbare.bpx");
	sb_hbar.LoadPicture("sb_hbar.bpx");
	sb_hbare.LoadPicture("sb_hbare.bpx");

	sb_btn.LoadPicture("sb_btn.bpx");

	chk1_on.LoadPicture("chk1_on.bpx");
	chk1_off.LoadPicture("chk1_off.bpx");
	chk2_on.LoadPicture("chk2_on.bpx");
	chk2_off.LoadPicture("chk2_off.bpx");

	w160x160.LoadPicture("w160x160.bpx");
	w160x256.LoadPicture("w160x256.bpx");

	y288x256.LoadPicture("y288x256.bpx");
	y352x352.LoadPicture("y352x352.bpx");
	y288x128.LoadPicture("y288x128.bpx");
	w288x256.LoadPicture("w288x256.bpx");
	w352x352.LoadPicture("w352x352.bpx");
	w288x128.LoadPicture("w288x128.bpx");
};
static int ItemChoose;

bool MMItemChoose(SimpleDialog* SD){
	ItemChoose=SD->UserParam;
	Lpressed=false;
	return true;
};
#define mcmExit		0xFF
#define mcmSingle	0xF1
#define mcmMulti	0xF2
#define mcmVideo	0xF3
#define mcmResume	0xF4
#define mcmOk		0xF5
#define mcmCancel	0xF6
#define mcmLoad		0xF7
#define mcmSave		0xF8
#define mcmHost		0xF9
#define mcmJoin		0xFA
#define mcmRefresh	0xFB
#define mcmEdit		0xFC
#define mcmAll		0xFD
#define mcmGraph	0xE0
#define mcmSound	0xE1
#define mcmSpeed	0xE2
#define mcmSnWeapon 0xE3
#define mcmSnBirth	0xE4
#define mcmSnOrder	0xE5
#define mcmSnAttack 0xE6
#define mcmOptions	0xE7
#define mcmHelp		0xE8
void InstallMap(ListBox* LB,WIN32_FIND_DATA* FD){
	LB->AddItem(FD->cFileName,0);
};
void CreateMapsList(ListBox* LB){
	WIN32_FIND_DATA FD;
	HANDLE HF=FindFirstFile("*.wm",&FD);
	if(HF!=INVALID_HANDLE_VALUE){
		InstallMap(LB,&FD);
		while(FindNextFile(HF,&FD))InstallMap(LB,&FD);
	};
};
void InstallNewMap(ListBox* LB,WIN32_FIND_DATA* FD){
	LB->AddItem(FD->cFileName,0);
};
void CreateNewMapsList(ListBox* LB){
	WIN32_FIND_DATA FD;
	HANDLE HF=FindFirstFile("*.wm",&FD);
	if(HF!=INVALID_HANDLE_VALUE){
		InstallMap(LB,&FD);
		while(FindNextFile(HF,&FD))InstallMap(LB,&FD);
	};
};
//----------------Graphics menu----------------//
bool InitScreen();
int OldSizeX;
int OldSizeY;
extern LPDIRECTDRAW            lpDD;
bool CheckMode(){
	ContinueGame=false;
	DialogsSystem GMM(smapx+(nsmaplx<<4)-(288>>1),smapy+(nsmaply<<4)-(288>>1));
	Picture* PIC1=GMM.addPicture(NULL,0,0,&y288x256,&y288x256,&y288x256);
	TextButton* Mess=GMM.addTextButton(PIC1,288>>1,30,"Íîðìàëüíî ?",&FActive,&FPassive,&FDisable,1);
	TextButton* OkBtn=GMM.addTextButton(PIC1,(288>>1)-60,180,"Äà",&FActive,&FPassive,&FDisable,1);
	TextButton* CancelBtn=GMM.addTextButton(PIC1,(288>>1)+60,180,"Íåò",&FActive,&FPassive,&FDisable,1);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	int time=GetTickCount();
	ItemChoose=-1;
	do{
		ProcessMessages();
		GMM.ProcessDialogs();
		GMM.RefreshView();
	}while(ItemChoose==-1&&GetTickCount()-time<8000);
	if(ItemChoose==mcmOk)return true;
	else return false;

};
void GSSetup800();
void DrawAllScreen();
extern int COPYSizeX;
extern int RealLx;
extern int RealLy;
void SERROR(){
	PlayEffect(0,0,0);
	int time=GetTickCount();
	do{}while(GetTickCount()-time<1000);
};
void SERROR1(){
	PlayEffect(1,0,0);
	int time=GetTickCount();
	do{}while(GetTickCount()-time<1000);
};
void SERROR2(){
	PlayEffect(2,0,0);
	int time=GetTickCount();
	do{}while(GetTickCount()-time<1000);
};
bool SetGameDisplayMode(int SizeX,int SizeY){
	RealLx=SizeX;
	RealLy=SizeY;
	FreeDDObjects();
	InitScreen();
	GSSetup800();
	DrawAllScreen();
	if(CheckMode())return true;
	else{
		RealLx=OldSizeX;
		RealLy=OldSizeY;
		FreeDDObjects();
		InitScreen();
		GSSetup800();
		DrawAllScreen();
		return false;
	};
	return false;
};
bool ProcessGraphics(int x,int y){
	ItemChoose=-1;
	DialogsSystem MMenu(x,y);
	Picture* PIC=MMenu.addPicture(NULL,0,0,&Panel1,&Panel1,&Panel1);
	BpxTextButton* OkBtn=MMenu.addBpxTextButton(PIC,60,190,"ÓÑÒÀÍÎÂÈÒÜ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	TextButton* BTN1=MMenu.addTextButton(PIC,PanelLx>>1,30,"Ðåæèì",&FActive,&FPassive,&FDisable,1);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	BpxTextButton* ExitBtn=MMenu.addBpxTextButton(PIC,60,220,"ÂÛÕÎÄ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	ExitBtn->UserParam=mcmExit;
	ExitBtn->OnUserClick=&MMItemChoose;
	CheckBox* mode800=MMenu.addCheckBox(PIC,70,70,"800x600",1,0,&chk2_on,&chk2_off,&f16x16w,&f16x16g);	
	CheckBox* mode1024=MMenu.addCheckBox(PIC,70,100,"1024x768",1,0,&chk2_on,&chk2_off,&f16x16w,&f16x16g);	
	CheckBox* mode1152=MMenu.addCheckBox(PIC,70,130,"1152x864",1,0,&chk2_on,&chk2_off,&f16x16w,&f16x16g);	
	CheckBox* mode1280=MMenu.addCheckBox(PIC,70,160,"1280x1024",1,0,&chk2_on,&chk2_off,&f16x16w,&f16x16g);	
	if(RealLx==800)mode800->State=true;
	if(RealLx==1024)mode1024->State=true;
	if(RealLx==1152)mode1152->State=true;
	if(RealLx==1280)mode1280->State=true;
	do{
		ProcessMessages();
		MMenu.MarkToDraw();
		MMenu.ProcessDialogs();
		MMenu.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose==mcmOk){
		if((RealLx==800&&mode800->State)||
		   (RealLx==1024&&mode1024->State)||
		   (RealLx==1152&&mode1152->State)||
		   (RealLx==1280&&mode1280->State))return false;
		//Set mode
		OldSizeX=RealLx;
		OldSizeY=RealLy;
		int msizex=800,msizey=600;
		if(mode1024->State){
			msizex=1024;
			msizey=768;
		};
		if(mode1152->State){
			msizex=1152;
			msizey=864;
		};
		if(mode1280->State){
			msizex=1280;
			msizey=1024;
		};
		if(SetGameDisplayMode(msizex,msizey)){
			ContinueGame=true;
			return true;
		};
	};
	return false;
};
//-----------------Speed menu------------------//
int FPSTime;
void ProcessSpeed(int x,int y){
	ItemChoose=-1;
	DialogsSystem MMenu(x,y);
	Picture* PIC=MMenu.addPicture(NULL,0,0,&Panel1,&Panel1,&Panel1);
	BpxTextButton* ExitBtn=MMenu.addBpxTextButton(PIC,60,220,"ÂÛÕÎÄ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	ExitBtn->UserParam=mcmExit;
	ExitBtn->OnUserClick=&MMItemChoose;
	if(FPSTime>100)FPSTime=100;
	if(FPSTime<0)FPSTime=0;
	VScrollBar* HB=MMenu.addHScrollBar(PIC,40,100,100,100-FPSTime,&sb_lf_d,&sb_lf_e,&sb_lf_p,&sb_ri_d,&sb_ri_e,&sb_ri_p,&sb_hbare,&sb_hbar,&sb_btn);
	MMenu.addTextButton(PIC,PanelLx>>1,20,"Ñêîðîñòü èãðû",&FActive,&FPassive,&FDisable,1);
	MMenu.addTextButton(PIC,50,95+30,"Ìèí.",&FActive,&FPassive,&FDisable,1);
	MMenu.addTextButton(PIC,235,95+30,"Ìàêñ.",&FActive,&FPassive,&FDisable,1);
	do{
		ProcessMessages();
		MMenu.MarkToDraw();
		MMenu.ProcessDialogs();
		MMenu.RefreshView();
	}while(ItemChoose==-1);
	FPSTime=100-HB->SPos;
};
//----------------SOUND----------------
int WarSound;
int WorkSound;
int OrderSound;
int MidiSound;
void ProcessSound(int x,int y){
	ItemChoose=-1;
	DialogsSystem MMenu(x,y);
	Picture* PIC=MMenu.addPicture(NULL,0,0,&Panel1,&Panel1,&Panel1);
	BpxTextButton* ExitBtn=MMenu.addBpxTextButton(PIC,60,220,"ÂÛÕÎÄ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	ExitBtn->UserParam=mcmExit;
	ExitBtn->OnUserClick=&MMItemChoose;
	if(FPSTime>100)FPSTime=100;
	if(FPSTime<0)FPSTime=0;
	if(WarSound>0)WarSound=0;
	if(OrderSound>0)OrderSound=0;
	if(WorkSound>0)WorkSound=0;
	if(WarSound<-1000)WarSound=-1000;
	if(WorkSound<-1000)WorkSound=-1000;
	if(OrderSound<-1000)OrderSound=-1000;
	if(MidiSound<-1000)MidiSound=-1000;
	MMenu.addTextButton(PIC,PanelLx>>1,20,"Ãðîìêîñòü",&FActive,&FPassive,&FDisable,1);
	MMenu.addTextButton(PIC,PanelLx>>1,60,"Çâóêè âîéíû",&f16x16w,&f16x16w,&f16x16w,1);
	VScrollBar* HB1=MMenu.addHScrollBar(PIC,40,80,1000,1000+WarSound,&sb_lf_d,&sb_lf_e,&sb_lf_p,&sb_ri_d,&sb_ri_e,&sb_ri_p,&sb_hbare,&sb_hbar,&sb_btn);
	MMenu.addTextButton(PIC,PanelLx>>1,110,"Ðàáîòà",&f16x16w,&f16x16w,&f16x16w,1);
	VScrollBar* HB2=MMenu.addHScrollBar(PIC,40,130,1000,1000+WorkSound,&sb_lf_d,&sb_lf_e,&sb_lf_p,&sb_ri_d,&sb_ri_e,&sb_ri_p,&sb_hbare,&sb_hbar,&sb_btn);
	MMenu.addTextButton(PIC,PanelLx>>1,160,"Îòâåò íà ïðèêàç",&f16x16w,&f16x16w,&f16x16w,1);
	VScrollBar* HB3=MMenu.addHScrollBar(PIC,40,180,1000,1000+OrderSound,&sb_lf_d,&sb_lf_e,&sb_lf_p,&sb_ri_d,&sb_ri_e,&sb_ri_p,&sb_hbare,&sb_hbar,&sb_btn);
	do{
		ProcessMessages();
		MMenu.MarkToDraw();
		MMenu.ProcessDialogs();
		MMenu.RefreshView();
		if(HB1->SPos-1000!=WarSound)PlayEffect(2,(rand()&1024)-512,HB1->SPos-1000);
		if(HB2->SPos-1000!=WorkSound)PlayEffect(0,(rand()&1024)-512,HB2->SPos-1000);
		if(HB3->SPos-1000!=OrderSound)PlayEffect(9,(rand()&1024)-512,HB3->SPos-1000);
		WarSound=HB1->SPos-1000;
		WorkSound=HB2->SPos-1000;
		OrderSound=HB3->SPos-1000;
	}while(ItemChoose==-1);
};
//-----------------Options menu----------------//
void ProcessOptions(int x,int y){
	ItemChoose=-1;
	DialogsSystem MMenu(x,y);
	Picture* PIC=MMenu.addPicture(NULL,0,0,&Panel1,&Panel1,&Panel1);
	BpxTextButton* ExitBtn=MMenu.addBpxTextButton(PIC,60,220,"ÂÛÕÎÄ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	ExitBtn->UserParam=mcmExit;
	ExitBtn->OnUserClick=&MMItemChoose;
	TextButton* BTN1=MMenu.addTextButton(PIC,PanelLx>>1,30,"Ãðàôèêà",&FActive,&FPassive,&FDisable,1);
	BTN1->UserParam=mcmGraph;
	BTN1->OnUserClick=&MMItemChoose;
	TextButton* BTN2=MMenu.addTextButton(PIC,PanelLx>>1,70,"Çâóê",&FActive,&FPassive,&FDisable,1);
	BTN2->UserParam=mcmSound;
	BTN2->OnUserClick=&MMItemChoose;
	TextButton* BTN4=MMenu.addTextButton(PIC,PanelLx>>1,110,"Ñêîðîñòü",&FActive,&FPassive,&FDisable,1);
	BTN4->UserParam=mcmEdit;
	BTN4->OnUserClick=&MMItemChoose;
	ItemChoose=-1;
	do{
		ProcessMessages();
		MMenu.ProcessDialogs();
		MMenu.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose==mcmGraph){
		ProcessGraphics(x,y);
	};
	if(ItemChoose==mcmEdit){
		ProcessSpeed(x,y);
	};
	if(ItemChoose==mcmSound){
		ProcessSound(x,y);
	};
};
//-----------------SINGLE PLAYER---------------//
void UnLoading();
static byte Ntn[8];
bool NatChoose(SimpleDialog* SD){
	BpxTextButton* TB=(BpxTextButton*)SD;
	char* qq=TB->Message;
	if(qq[0]=='Î')strcpy(TB->Message,"ËÞÄÈ");
	else strcpy(TB->Message,"ÎÐÊÈ");
	SD->NeedToDraw=true;
	Lpressed=true;
	return true;
};	
int coorX[8];
int coorY[8];
void setCoor(){
	for(int i=0;i<8;i++){
		coorX[i]=-1;
		coorY[i]=-1;
	};
};
void chooseCoor(int* xx,int* yy){
	int x,y;
	do{
		x=div(int(rando())*(msx-16),32768).quot;
		y=div(int(rando())*(msy-16),32768).quot;
		//checking for distance from enemy
		bool ok=true;
		for(int j=0;j<8;j++){
			if(coorX[j]!=-1&&abs(coorX[j]-x)+abs(coorY[j]-y)<25)ok=false;
		};
		if(ok){
			//checking for empty space
			for(int dx=0;dx<16;dx++)
				for(int dy=0;dy<16;dy++)
					if(LLock[y+dy][x+dx]){
						ok=false;
						goto hhh;
					};
hhh:		if(ok){
				*xx=x;
				*yy=y;
				return;
			};
		};
	}while(true);
};
void LoadNation(char* fn,byte msk,byte NIndex);
extern City CITY[8];
void processSinglePlayer(){
	int mmlx=MainMenuGround.PicPtr[0];
	int mmly=MainMenuGround.PicPtr[1];
	//MainMenuGround.Draw((RSCRSizeX-mmlx)>>1,(RSCRSizeY-mmly)>>1);
	DialogsSystem DSY(((RealLx-352)>>1)-130,((RSCRSizeY-352)>>1)+64);
	//choose map
	Picture* Pic=DSY.addPicture(NULL,352+20,0,&y288x256,&y288x256,&y288x256);
	DSY.addTextButton(Pic,352+(352>>1)-10,20,"Âûáåðèòå êàðòó",&FActive,&FPassive,&FDisable,1);
	ListBox* LB=DSY.addListBox(Pic,352+60-15,60,6,&r224x28e,&f16x16g,&f16x16w,NULL);
	VScrollBar* VS=DSY.addVScrollBar(Pic,LB->x-DSY.BaseX+LB->oneLx-1,1+LB->y-DSY.BaseY,1000,4,&sb_up_d,&sb_up_e,&sb_up_p,&sb_dn_d,&sb_dn_e,&sb_dn_p,&sb_vbare,&sb_vbar,&sb_btn);
	VS->SPos=0;
	LB->VS=VS;
	//choose opponents
	Picture* PIC=DSY.addPicture(NULL,0,0,&y352x352,&y352x352,&y352x352);
	//DSY.addTextButton(PIC,352>>1,20,"Âûáåðèòå ïðîòèâíèêîâ",&FActive,&FPassive,&FDisable,1);
	//CheckBox* CHME[8];		//choose my nation
	CheckBox* CHTHEM[8];	//choose enabled nation
	BpxTextButton* NCH[8];	//choose nation
	int y=30;
	DSY.addTextButton(Pic,30+40,y-10,"Âû:",&FActive,&FActive,&FActive,1);
	DSY.addTextButton(Pic,95+70,y+28,"ÏÐÎÒÈÂÍÈÊÈ:",&FPassive,&FPassive,&FPassive,1);
	NCH[0]=DSY.addBpxTextButton(PIC,110,y-8,"ÎÐÊÈ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	NCH[0]->OnUserClick=&NatChoose;
	y+=60;
	for(int i=1;i<8;i++){
		CHTHEM[i]=DSY.addCheckBox(PIC,70,y,"",0,1,&chk1_on,&chk1_off,&f16x16w,&f16x16g);	
		//if(!i)CHME[i]=DSY.addCheckBox(PIC,90,y,"",1,1,&chk2_on,&chk2_off,&f16x16w,&f16x16g);	
		//else CHME[i]=DSY.addCheckBox(PIC,90,y,"",1,0,&chk2_on,&chk2_off,&f16x16w,&f16x16g);	
		NCH[i]=DSY.addBpxTextButton(PIC,110,y,"ÎÐÊÈ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
		y+=30;
		NCH[i]->OnUserClick=&NatChoose;
	};
	BpxTextButton* OkBtn=DSY.addBpxTextButton(PIC,10,y+10,"OK",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	BpxTextButton* CancelBtn=DSY.addBpxTextButton(PIC,180,y+10,"ÎÒÌÅÍÀ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	//Create list of maps
	CreateMapsList(LB);
	ItemChoose=-1;
	do{
		/*
		//rando();
		for(i=0;i<8;i++){
			if(!CHTHEM[i]->State){
				//CHME[i]->Enabled=false;
				NCH[i]->Enabled=false;
				//CHME[i]->NeedToDraw=true;
				NCH[i]->NeedToDraw=true;
			}else{
				//CHME[i]->Enabled=true;
				NCH[i]->Enabled=true;
				CHME[i]->NeedToDraw=true;
				NCH[i]->NeedToDraw=true;
			};
		};
		for(i=0;i<8;i++){
			if(CHME[i]->State&&!CHME[i]->Enabled){
				for(int j=0;j<8;j++)CHME[j]->State=false;
				for(j=0;j<8;j++){
					if(CHME[(i+j)&7]->Enabled){
						CHME[(i+j)&7]->State=true;
						goto uuu;
					};
				};
uuu:;
			};
		};
		bool cc=false;
		for(i=0;i<8;i++)if(CHME[i]->State)cc=true;
		int nc=0;
		int cp=-1;
		for(i=0;i<8;i++){
			if(CHTHEM[i]->State){
				nc++;
				cp=i;
			};
		};
		if(nc==1)CHME[cp]->State=true;
		OkBtn->Enabled=cc;*/
		if(LB->CurItem==-1)OkBtn=false;
		OkBtn->NeedToDraw=true;
		DSY.MarkToDraw();
		ProcessMessages();
		DSY.ProcessDialogs();
		DSY.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose==mcmOk){
		//setCoor();
		UnLoading();
		PreLoadExtendedMap((LB->GetItem(LB->CurItem))->Message);
		MyNation=0;
		memcpy(&RESRC[0][0],&RES[0][0],sizeof RES);
		for(i=0;i<8;i++){
			byte mss=254;
			if(!i)mss=1;
			if(NCH[i]->Message[0]=='Î')
				LoadNation("orcs.nds",mss,i);
			else LoadNation("PEOPLES.nds",mss,i);
			//if(CHME[i]->State)MyNation=i;
			int xx,yy;
			//chooseCoor(&xx,&yy);
			xx=mapPos[i*2];
			yy=mapPos[i*2+1];
			CITY[i].CreateCity(xx+4,yy+4,i);
			if((!i)||CHTHEM[i]->State){
				//coorX[i]=xx;
				//coorY[i]=yy;
				NATIONS[i].CreateTerrainAt(xx,yy,1);
				if(i==MyNation){
					mapx=xx-(smaplx>>1);
					mapy=yy-(smaply>>1);
					if(mapx<1)mapx=1;
					if(mapy<1)mapy=1;
					if(mapx+smaplx>=msx)mapx=msx-smaplx-1;
					if(mapy+smaply>=msy)mapy=msy-smaply-1;
				};
				if(i)CITY[i].IntellectEnabled=true;
			};
		};
		PostLoadExtendedMap((LB->GetItem(LB->CurItem))->Message);
		CheapMode=false;
		InitGame();
		ItemChoose=mcmSingle;
		InitExplosions();
		PrepareToGame();
	};
};
//-----------------WAITING FOR OPPONENTS(HOST)-------------------//
bool processHostConnection(){
	DialogsSystem DSY((RealLx-352)>>1,RSCRSizeY-352-120);
	//choose map
	Picture* Pic=DSY.addPicture(NULL,0,0,&y352x352,&y352x352,&y352x352);
	DSY.addTextButton(Pic,352>>1,20,"Æäåì ñîïåðíèêîâ...",&FActive,&FPassive,&FDisable,1);
	char GamersNames[8][32];
	int yy=60;
	InputBox* IBN[8];
	for(int i=0;i<8;i++){
		GamersNames[i][0]=0;
		IBN[i]=DSY.addInputBox(Pic,65,yy,GamersNames[i],20,&r224x28e,&f16x16w,&f16x16g);
		yy+=28;
	};
	BpxTextButton* OkBtn=DSY.addBpxTextButton(Pic,10,314,"Íà÷àòü èãðó",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	OkBtn->UserParam=mcmJoin;
	OkBtn->OnUserClick=&MMItemChoose;
	BpxTextButton* CancelBtn=DSY.addBpxTextButton(Pic,180,314,"ÎÒÌÅÍÀ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	ItemChoose=-1;
	PlayerMenuMode=true;
	do{
		ProcessMessages();
		AnalyseMessages();
		OkBtn->NeedToDraw=true;
		if(NPlayers>1)OkBtn->Enabled=true;
		else OkBtn->Enabled=false;
		IBEnumeratePlayers(IBN);
		DSY.MarkToDraw();
		DSY.ProcessDialogs();
		DSY.RefreshView();
	}while(ItemChoose==-1&&PlayerMenuMode);
	if(ItemChoose==mcmCancel){
		CloseMPL();
	}else{
		if(ItemChoose==mcmJoin){
			StartIGame();
			return true;
		};
	};
	return false;
};
//-----------------WAITING FOR OPPONENTS(JOIN)-------------------//
bool processJoinConnection(){
	DialogsSystem DSY((RealLx-352)>>1,RSCRSizeY-352-120);
	//choose map
	Picture* Pic=DSY.addPicture(NULL,0,0,&y352x352,&y352x352,&y352x352);
	DSY.addTextButton(Pic,352>>1,20,"Æäåì ñîïåðíèêîâ...",&FActive,&FPassive,&FDisable,1);
	char GamersNames[8][32];
	int yy=60;
	InputBox* IBN[8];
	for(int i=0;i<8;i++){
		GamersNames[i][0]=0;
		IBN[i]=DSY.addInputBox(Pic,65,yy,GamersNames[i],20,&r224x28e,&f16x16w,&f16x16g);
		yy+=28;
	};
	//BpxTextButton* OkBtn=DSY.addBpxTextButton(Pic,10,314,"Íà÷àòü èãðó",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	//OkBtn->UserParam=mcmJoin;
	//OkBtn->OnUserClick=&MMItemChoose;
	BpxTextButton* CancelBtn=DSY.addBpxTextButton(Pic,180,314,"ÎÒÌÅÍÀ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	ItemChoose=-1;
	PlayerMenuMode=1;
	do{
		ProcessMessages();
		AnalyseMessages();
		IBEnumeratePlayers(IBN);
		DSY.MarkToDraw();
		DSY.ProcessDialogs();
		DSY.RefreshView();
	}while(ItemChoose==-1&&PlayerMenuMode);
	if(ItemChoose==mcmCancel){
		CloseMPL();
	}else{
		if(!PlayerMenuMode)return true;
	};
	return false;
};
//--------------------JOIN  TO SESSION---------------------//
bool processJoinGame(){
	DialogsSystem DSY((RealLx-352)>>1,RSCRSizeY-352-120);
	//choose map
	Picture* Pic=DSY.addPicture(NULL,0,0,&y352x352,&y352x352,&y352x352);
	DSY.addTextButton(Pic,352>>1,20,"Âûáåðèòå èãðó",&FActive,&FPassive,&FDisable,1);
	ListBox* LB=DSY.addListBox(Pic,60,90,6,&r224x28e,&f16x16g,&f16x16w,NULL);
	VScrollBar* VS=DSY.addVScrollBar(Pic,LB->x-DSY.BaseX+LB->oneLx-1,1+LB->y-DSY.BaseY,1000,4,&sb_up_d,&sb_up_e,&sb_up_p,&sb_dn_d,&sb_dn_e,&sb_dn_p,&sb_vbare,&sb_vbar,&sb_btn);
	LB->VS=VS;
	BpxTextButton* OkBtn=DSY.addBpxTextButton(Pic,10,314,"Ïðèñîåäèíèòüñÿ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	OkBtn->UserParam=mcmJoin;
	OkBtn->OnUserClick=&MMItemChoose;
	BpxTextButton* CancelBtn=DSY.addBpxTextButton(Pic,180,314,"ÎÒÌÅÍÀ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	BpxTextButton* RefreshBtn=DSY.addBpxTextButton(Pic,10+90,314-40,"Îáíîâèòü ñïèñîê",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	RefreshBtn->UserParam=mcmRefresh;
	RefreshBtn->OnUserClick=&MMItemChoose;
	LBEnumerateSessions(LB);
	do{
		ItemChoose=-1;
		do{
			ProcessMessages();
			OkBtn->NeedToDraw=true;
			if(LB->CurItem==-1)OkBtn->Enabled=false;
			else OkBtn->Enabled=true;
			DSY.MarkToDraw();
			DSY.ProcessDialogs();
			DSY.RefreshView();
			if(ItemChoose==mcmRefresh){
				LBEnumerateSessions(LB);
				ItemChoose=-1;
			};
		}while(ItemChoose==-1);
		if(ItemChoose==mcmJoin&&LB->CurItem!=-1){
			JoinNameToSession(LB->CurItem,PlName);
			if(processJoinConnection())return true;
		};
	}while(ItemChoose!=mcmCancel);
	return false;
};
//--------------------HOST THE SESSION---------------------//
bool processHostGame(){
	DialogsSystem DSY((RealLx-352)>>1,RSCRSizeY-352-120);
	//choose map
	Picture* Pic=DSY.addPicture(NULL,0,0,&y352x352,&y352x352,&y352x352);
	DSY.addTextButton(Pic,352>>1,20,"Âûáåðèòå êàðòó",&FActive,&FPassive,&FDisable,1);
	ListBox* LB=DSY.addListBox(Pic,60,90,6,&r224x28e,&f16x16g,&f16x16w,NULL);
	VScrollBar* VS=DSY.addVScrollBar(Pic,LB->x-DSY.BaseX+LB->oneLx-1,1+LB->y-DSY.BaseY,1000,4,&sb_up_d,&sb_up_e,&sb_up_p,&sb_dn_d,&sb_dn_e,&sb_dn_p,&sb_vbare,&sb_vbar,&sb_btn);
	LB->VS=VS;
	CreateMapsList(LB);
	BpxTextButton* OkBtn=DSY.addBpxTextButton(Pic,10,314,"ÑÎÇÄÀÒÜ ÈÃÐÓ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	OkBtn->UserParam=mcmHost;
	OkBtn->OnUserClick=&MMItemChoose;
	BpxTextButton* CancelBtn=DSY.addBpxTextButton(Pic,180,314,"ÎÒÌÅÍÀ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	ItemChoose=-1;
	do{
		do{
			ProcessMessages();
			DSY.MarkToDraw();
			DSY.ProcessDialogs();
			DSY.RefreshView();
		}while(ItemChoose==-1);
		if(ItemChoose==mcmHost&&LB->CurItem!=-1){
			strcpy(CurrentMap,(LB->GetItem(LB->CurItem))->Message);
			if(CreateNamedSession(PlName))
			if(processHostConnection())return true;
		};
	}while(ItemChoose!=mcmCancel);
	return false;
};
//--------------------MULTIPLAYER GAME---------------------//
char SaveFileName[128];
void processMultiplayer(){
	sprintf(SaveFileName,"Save %d",GetTickCount());
	CreateMultiplaterInterface();
	if(!lpDirectPlay3A)return;
	if FAILED(lpDirectPlay3A->InitializeConnection(
		lplpConnectionBuffer[0],0)) return;
	//Multiplayer interface is now created
	DialogsSystem DSY((RealLx-352)>>1,RSCRSizeY-352-120);
	Picture* PIC=DSY.addPicture(NULL,0,0,&y352x352,&y352x352,&y352x352);
	TextButton* BTN1=DSY.addTextButton(PIC,352>>1,20,"Èãðà ïî ñåòè (IPX)",&FActive,&FPassive,&FDisable,1);
	BpxTextButton* HostBtn=DSY.addBpxTextButton(PIC,10,280,"Ñîçäàòü èãðó",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	HostBtn->UserParam=mcmHost;
	HostBtn->OnUserClick=&MMItemChoose;
	BpxTextButton* JoinBtn=DSY.addBpxTextButton(PIC,180,280,"Ïðèñîåäèíèòüñÿ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	JoinBtn->UserParam=mcmJoin;
	JoinBtn->OnUserClick=&MMItemChoose;
	BpxTextButton* CancelBtn=DSY.addBpxTextButton(PIC,180-90,315,"ÎÒÌÅÍÀ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	TextButton* BTN2=DSY.addTextButton(PIC,(352>>1)-130+20,90-8-20,"èìÿ:",&FActive,&FPassive,&FDisable,1);
	InputBox* IB=DSY.addInputBox(PIC,80+20,92-8-20,PlName,20,&r224x28e,&f16x16w,&f16x16g);
	TextButton* BTN8=DSY.addTextButton(PIC,(352>>1),200,"Àâòîñîõðàíåíèå",&FActive,&FPassive,&FDisable,1);
	InputBox* SavN=DSY.addInputBox(PIC,70,240,SaveFileName,20,&r224x28e,&f16x16w,&f16x16g);
	CheckBox* ORCS=DSY.addCheckBox(PIC,100,120," Îðêè",1,1,&chk2_on,&chk2_off,&FActive,&FPassive);
	CheckBox* PEOP=DSY.addCheckBox(PIC,100,160," Ëþäè",1,0,&chk2_on,&chk2_off,&FActive,&FPassive);
	do{
		ItemChoose=-1;
		do{
			ProcessMessages();
			DSY.MarkToDraw();
			DSY.ProcessDialogs();
			DSY.RefreshView();
		}while(ItemChoose==-1);
		if(ORCS->State)MyRace=0;
		if(PEOP->State)MyRace=1;
		if(ItemChoose==mcmHost){
			if(processHostGame())return;
		}else if(ItemChoose==mcmJoin)if(processJoinGame())return;
	}while(ItemChoose!=mcmCancel);
};
//--------------------RESOURCE EDITOR----------------------//
int ProcessResEdit(){
	//ContinueGame=false;
	DialogsSystem GMM(smapx+(nsmaplx<<4)-(352>>1),smapy+(nsmaply<<4)-(352>>1));
	Picture* PIC1=GMM.addPicture(NULL,0,0,&y352x352,&y352x352,&y352x352);
	TextButton* ExitBtn=GMM.addTextButton(PIC1,352>>1,30,"ÐÅÑÓÐÑÛ",&FActive,&FPassive,&FDisable,1);
	char XX[12];
	char GoldST[22];
	char WoodST[22];
	char OilST[22];
	itoa(RES[MyNation][1],GoldST,10);
	itoa(RES[MyNation][2],WoodST,10);
	itoa(RES[MyNation][3],OilST,10);
	strcpy(XX,"Èãðîê ");
	itoa(MyNation+1,&XX[6],10);
	TextButton* PLID=GMM.addTextButton(PIC1,352>>1,30+40,XX,&FActive,&FPassive,&FDisable,1);
	TextButton* GoldBtn=GMM.addTextButton(PIC1,40,30+80,"Gold:",&f16x16w,&f16x16w,&f16x16w,1);
	InputBox* GoldIB=GMM.addInputBox(PIC1,90,30+80,GoldST,20,&r224x28e,&f16x16w,&f16x16g);
	TextButton* WoodBtn=GMM.addTextButton(PIC1,40,30+120,"Wood:",&f16x16w,&f16x16w,&f16x16w,1);
	InputBox* WoodIB=GMM.addInputBox(PIC1,90,30+120,WoodST,20,&r224x28e,&f16x16w,&f16x16g);
	TextButton* OilBtn=GMM.addTextButton(PIC1,40,30+160,"Oil:",&f16x16w,&f16x16w,&f16x16w,1);
	InputBox* OilIB=GMM.addInputBox(PIC1,90,30+160,OilST,20,&r224x28e,&f16x16w,&f16x16g);
	BpxTextButton* OkBtn=
	GMM.addBpxTextButton(PIC1,((352-164)>>1)-85,280,"OK",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	BpxTextButton* CancelBtn=
	GMM.addBpxTextButton(PIC1,((352-164)>>1)+85,280,"ÎÒÌÅÍÀ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	BpxTextButton* AllBtn=
	GMM.addBpxTextButton(PIC1,((352-164)>>1)+85,315,"ÄËß ÂÑÅÕ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	AllBtn->UserParam=mcmAll;
	AllBtn->OnUserClick=&MMItemChoose;
	
stg:
	ItemChoose=-1;
	do{
		GMM.MarkToDraw();
		RES[MyNation][1]=atoi(GoldST);
		RES[MyNation][2]=atoi(WoodST);
		RES[MyNation][3]=atoi(OilST);
		ProcessMessages();
		GMM.ProcessDialogs();
		GMM.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose==mcmAll){
		int G=RES[MyNation][1];
		int W=RES[MyNation][2];
		int O=RES[MyNation][3];
		for(int i=0;i<8;i++){
			RES[i][1]=G;
			RES[i][2]=W;
			RES[i][3]=O;
		};
	};
	return ItemChoose;	
};
//---------------------------------------------------------//
int processMainMenu(){
MMStart:
	InGame=false;
	GameMode=1;
	SetRLCWindow(0,0,RealLx,RSCRSizeY,SCRSizeX);
	NPlayers=1;
	MultiplayerStart=false;
	ItemChoose=-1;
	CBar(0,0,RealLx,RSCRSizeY,0);
	LoadPalette("015w.pal");
	int mmlx=MainMenuGround.PicPtr[0];
	int mmly=MainMenuGround.PicPtr[1];
	MainMenuGround.Draw((RealLx-mmlx)>>1,(RSCRSizeY-mmly)>>1);
	PlayerMenuMode=1;
	FlipPages();
	DialogsSystem MMenu((RealLx-PanelLx)>>1,RSCRSizeY-PanelLy-80);
	Picture* PIC=MMenu.addPicture(NULL,0,0,&Panel1,&Panel1,&Panel1);
	BpxTextButton* ExitBtn=MMenu.addBpxTextButton(PIC,60,220,"ÂÛÕÎÄ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	ExitBtn->UserParam=mcmExit;
	ExitBtn->OnUserClick=&MMItemChoose;
	TextButton* BTN1=MMenu.addTextButton(PIC,PanelLx>>1,30,"Îäèíî÷íàÿ èãðà",&FActive,&FPassive,&FDisable,1);
	BTN1->UserParam=mcmSingle;
	BTN1->OnUserClick=&MMItemChoose;
	TextButton* BTN2=MMenu.addTextButton(PIC,PanelLx>>1,65,"Èãðà ïî ñåòè",&FActive,&FPassive,&FDisable,1);
	BTN2->UserParam=mcmMulti;
	BTN2->OnUserClick=&MMItemChoose;
	TextButton* BTNL=MMenu.addTextButton(PIC,PanelLx>>1,100,"Çàãðóçèòü èãðó",&FActive,&FPassive,&FDisable,1);
	BTNL->UserParam=mcmLoad;
	BTNL->OnUserClick=&MMItemChoose;
	TextButton* BTN4=MMenu.addTextButton(PIC,PanelLx>>1,135,"Ðåäàêòîð êàðòû",&FActive,&FPassive,&FDisable,1);
	BTN4->UserParam=mcmEdit;
	BTN4->OnUserClick=&MMItemChoose;
	TextButton* BTN3=MMenu.addTextButton(PIC,PanelLx>>1,170,"Âèäåî",&FActive,&FPassive,&FDisable,1);
	BTN3->UserParam=mcmVideo;
	BTN3->OnUserClick=&MMItemChoose;
	do{
		ProcessMessages();
		MMenu.ProcessDialogs();
		MMenu.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose==mcmSingle)processSinglePlayer();
	if(ItemChoose==mcmEdit){
		PrepareEditMedia(0);
		ItemChoose=mcmSingle;
	};
	if(ItemChoose==mcmLoad){
		processMLoadGame();
	};
	if(ItemChoose==mcmVideo){
		PlayVideo();
		goto MMStart;
	};
	if(ItemChoose==mcmMulti)processMultiplayer();
	GameMode=0;
	return ItemChoose;
};
//--------------=< G A M E   S C R E E N >=------------//
extern int PrpX;
extern int PrpY;
extern int PrpNx;
extern int PrpNy;
extern int AblX;
extern int AblY;
extern int AblNx;
extern int AblNy;
extern int IconLx;
//extern int IconLx;
extern int IconLy;
DialogsSystem GSYS(0,0);
SimpleDialog* GVPort;
SimpleDialog* MiniVPort;
SimpleDialog* AblVPort;
SimpleDialog* PrpVPort;
Picture* Pan1;
Picture* Pan2;
Picture* Pan3;
ColoredBar* Pan4;
ColoredBar* Pan5;
bool GameExit;
int MenuType;
bool MakeMenu;

int NNames;
char* names[128];
char* fnames[128];
int   serials[128];
void ClearNames(){
	for(int i=0;i<128;i++)names[i]=NULL;
	NNames=0;
};
void FreeNames(){
	for(int i=0;i<128;i++)if(names[i]){
		free(names[i]);
		free(fnames[i]);
		names[i]=NULL;
		fnames[i]=NULL;
	};
	NNames=0;
};
void ErrorBox(char* str){
	DialogsSystem DSY(smapx+(nsmaplx<<4)-(288>>1),smapy+(nsmaply<<4)-(128>>1));
	Picture* PIC1=DSY.addPicture(NULL,0,0,&w288x128,&w288x128,&w288x128);
	DSY.addTextButton(PIC1,288>>1,30,str,&f16x16w,&f16x16w,&f16x16w,1);
	BpxTextButton* OkBtn=DSY.addBpxTextButton(PIC1,(288-164)>>1,80,"OK",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	do{
		ProcessMessages();
		DSY.ProcessDialogs();
		DSY.RefreshView();
	}while(ItemChoose==-1);
};
extern int sfVersion;
void InstallName(ListBox* LB,WIN32_FIND_DATA* FD){
	ResFile ff1=RReset(FD->cFileName);
	char nam[128];
	if(!IOresult()){
		int sig,lap;
		RBlockRead(ff1,&sig,4);
		if(sig=='WSF2'){
			RBlockRead(ff1,&sig,4);
			RBlockRead(ff1,&lap,4);
			if(sig==sfVersion&&NNames<128){
				int nlen=0;
				RBlockRead(ff1,&nlen,2);
				nlen=0;
				RBlockRead(ff1,&nlen,1);
				RBlockRead(ff1,nam,nlen);
				RClose(ff1);
				LB->AddItem(nam,NNames);
				names[NNames]=new char[strlen(nam)+1];
				fnames[NNames]=new char[strlen(FD->cFileName)+1];
				serials[NNames]=lap;
				strcpy(names[NNames],nam);
				strcpy(fnames[NNames],FD->cFileName);
				NNames++;
			}else RClose(ff1);
		}else RClose(ff1);
	};
};
void CreateGamesList(ListBox* LB){
	WIN32_FIND_DATA FD;
	ClearNames();
	HANDLE HF=FindFirstFile("*.sav",&FD);
	if(HF!=INVALID_HANDLE_VALUE){
		InstallName(LB,&FD);
		while(FindNextFile(HF,&FD))InstallName(LB,&FD);
	};
};
//----------------LOAD GAME--------------//
void CmdLoadNetworkGame(byte NI,int ID,char* Name);
void LoadGame(char* fnm);
void ProcessGLoadGame(){
	DialogsSystem DSY(smapx+(nsmaplx<<4)-(352>>1),smapy+(nsmaply<<4)-(352>>1));
	Picture* PIC1=DSY.addPicture(NULL,0,0,&y352x352,&y352x352,&y352x352);
	TextButton* LoadBtn=DSY.addTextButton(PIC1,352>>1,30,"ÇÀÃÐÓÇÈÒÜ ÈÃÐÓ",&FActive,&FPassive,&FDisable,1);
	ListBox* LB=DSY.addListBox(PIC1,60,100,6,&r224x28e,&f16x16g,&f16x16w,NULL);
	VScrollBar* VS=DSY.addVScrollBar(PIC1,LB->x-DSY.BaseX+LB->oneLx-1,1+LB->y-DSY.BaseY,1000,4,&sb_up_d,&sb_up_e,&sb_up_p,&sb_dn_d,&sb_dn_e,&sb_dn_p,&sb_vbare,&sb_vbar,&sb_btn);
	LB->VS=VS;
	BpxTextButton* OkBtn=DSY.addBpxTextButton(PIC1,10,315,"OK",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	BpxTextButton* CancelBtn=DSY.addBpxTextButton(PIC1,180,315,"ÎÒÌÅÍÀ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	CreateGamesList(LB);
	ItemChoose=-1;
	do{
		DSY.MarkToDraw();
		ProcessMessages();
		if(LB->CurItem==-1)OkBtn->Enabled=false;
		else OkBtn->Enabled=true;
		DSY.ProcessDialogs();
		DSY.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose==mcmOk&&LB->CurItem!=-1){
		CmdLoadNetworkGame(MyNation,serials[LB->CurItem],names[LB->CurItem]);
		//LoadGame(fnames[LB->CurItem]);
		ContinueGame=true;
	};
	FreeNames();
};
void processMLoadGame(){
	DialogsSystem DSY((RealLx-352)>>1,RealLy-352-80);
	Picture* PIC1=DSY.addPicture(NULL,0,0,&y352x352,&y352x352,&y352x352);
	TextButton* LoadBtn=DSY.addTextButton(PIC1,352>>1,30,"ÇÀÃÐÓÇÈÒÜ ÈÃÐÓ",&FActive,&FPassive,&FDisable,1);
	ListBox* LB=DSY.addListBox(PIC1,60,100,6,&r224x28e,&f16x16g,&f16x16w,NULL);
	VScrollBar* VS=DSY.addVScrollBar(PIC1,LB->x-DSY.BaseX+LB->oneLx-1,1+LB->y-DSY.BaseY,1000,4,&sb_up_d,&sb_up_e,&sb_up_p,&sb_dn_d,&sb_dn_e,&sb_dn_p,&sb_vbare,&sb_vbar,&sb_btn);
	LB->VS=VS;
	BpxTextButton* OkBtn=DSY.addBpxTextButton(PIC1,10,315,"OK",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	BpxTextButton* CancelBtn=DSY.addBpxTextButton(PIC1,180,315,"ÎÒÌÅÍÀ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	CreateGamesList(LB);
	ItemChoose=-1;
	do{
		DSY.MarkToDraw();
		ProcessMessages();
		if(LB->CurItem==-1)OkBtn->Enabled=false;
		else OkBtn->Enabled=true;
		DSY.ProcessDialogs();
		DSY.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose==mcmOk&&LB->CurItem!=-1){
		LoadGame(fnames[LB->CurItem]);
		ContinueGame=true;
		ItemChoose=mcmSingle;
		PrepareToGame();
	};
	FreeNames();
};
//----------------SAVE GAME--------------//
void CmdSaveNetworkGame(byte NI,int ID,char* Name);
bool CheckName(char* str){
	for(int i=0;i<NNames;i++)if(strstr(names[i],str))return false;
	return true;
};
bool ChangeNation;
bool MultiTvar;
extern int FogMode;
void EnterChat(){
	char passw[128];
	passw[0]=0;
	ItemChoose=-1;
	DialogsSystem DSY(smapx+(nsmaplx<<4)-(288>>1),smapy+(nsmaply<<4)-(128>>1));
	Picture* PIC1=DSY.addPicture(NULL,0,0,&y288x128,&y288x128,&y288x128);
	TextButton* LoadBtn=DSY.addTextButton(PIC1,288>>1,5,"ÏÀÐÎËÜ:",&FActive,&FPassive,&FDisable,1);
	InputBox* IB=DSY.addInputBox(PIC1,30,45,passw,20,&r224x28e,&f16x16w,&f16x16g);
	IB->Active=true;
	//BpxTextButton* CancelBtn=DSY.addBpxTextButton(PIC1,120,90,"ÎÒÌÅÍÀ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	//CancelBtn->UserParam=mcmCancel;
	//CancelBtn->OnUserClick=&MMItemChoose;
	BpxTextButton* OkBtn=DSY.addBpxTextButton(PIC1,55,90,"OK",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	KeyPressed=false;
	do{
		DSY.MarkToDraw();
		ProcessMessages();
		if(KeyPressed&&LastKey==13){
			ItemChoose=mcmOk;
			KeyPressed=false;
		};
		DSY.ProcessDialogs();
		DSY.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose==mcmOk){
		if(!strcmp(passw,"IZMENA"))ChangeNation=!ChangeNation;
		if(!strcmp(passw,"MULTITVAR"))MultiTvar=!MultiTvar;
		if(!strcmp(passw,"SUPERVIZOR"))FogMode=!FogMode;
	};
};
void SaveGame(char* fnm,char* Messtr,int ID);
void ProcessGSaveGame(){
	DialogsSystem DSY(smapx+(nsmaplx<<4)-(352>>1),smapy+(nsmaply<<4)-(352>>1));
	Picture* PIC1=DSY.addPicture(NULL,0,0,&y352x352,&y352x352,&y352x352);
	TextButton* LoadBtn=DSY.addTextButton(PIC1,352>>1,30,"ÑÎÕÐÀÍÈÒÜ ÈÃÐÓ",&FActive,&FPassive,&FDisable,1);
	InputBox* IB=DSY.addInputBox(PIC1,60,75,GameName,20,&r224x28e,&f16x16w,&f16x16g);
	ListBox* LB=DSY.addListBox(PIC1,60,120,6,&r224x28e,&f16x16g,&f16x16w,NULL);
	VScrollBar* VS=DSY.addVScrollBar(PIC1,LB->x-DSY.BaseX+LB->oneLx-1,1+LB->y-DSY.BaseY,1000,4,&sb_up_d,&sb_up_e,&sb_up_p,&sb_dn_d,&sb_dn_e,&sb_dn_p,&sb_vbare,&sb_vbar,&sb_btn);
	LB->VS=VS;
	BpxTextButton* OkBtn=DSY.addBpxTextButton(PIC1,10,315,"OK",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	BpxTextButton* CancelBtn=DSY.addBpxTextButton(PIC1,180,315,"ÎÒÌÅÍÀ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	CreateGamesList(LB);
	ItemChoose=-1;
	do{
		DSY.MarkToDraw();
		ProcessMessages();
		if(strlen(IB->Str))OkBtn->Enabled=true;
		else OkBtn->Enabled=false;
		DSY.ProcessDialogs();
		DSY.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose=mcmOk){
		CmdSaveNetworkGame(MyNation,GetTickCount(),IB->Str);
		ContinueGame=true;
/*		else{
			for(int j=0;j<NNames;j++){
				if(!strcmp(names[j],IB->Str))goto jhg;
			};
jhg:		if(j<NNames){
				//record already exists
				SaveGame(names[j],IB->Str,0);
				strcpy(GameName,names[j]);
			}else{
				//new record
				//discovery for new name
				char XX[128];
				strcpy(GameName,IB->Str);
				for(j=0;j<9999;j++){
					strcpy(XX,"save");
					itoa(j,&XX[4],10);
					strcat(XX,".sav");
					if(CheckName(XX)){
						SaveGame(XX,GameName,0);
						goto qqwer;
					};
				};
			};
		};*/
	};
qqwer:
	FreeNames();
};
//----------------SAVE MAP--------------//
void NormalName(char* s){
	if(!strstr(s,".wm")){
		strcat(s,".wm");
	};
};
bool LBCLICK(SimpleDialog* SD){
	ListBox* LB=(ListBox*)SD;
	if(LB->CurItem!=-1){
		strcpy(GameName,(LB->GetItem(LB->CurItem))->Message);
	};
	return false;
};
void SaveExtendedMap(char* fnm);
void ProcessGSaveMap(){
	DialogsSystem DSY(smapx+(nsmaplx<<4)-(352>>1),smapy+(nsmaply<<4)-(352>>1));
	Picture* PIC1=DSY.addPicture(NULL,0,0,&y352x352,&y352x352,&y352x352);
	TextButton* LoadBtn=DSY.addTextButton(PIC1,352>>1,30,"ÑÎÕÐÀÍÈÒÜ ÊÀÐÒÓ",&FActive,&FPassive,&FDisable,1);
	InputBox* IB=DSY.addInputBox(PIC1,60,75,GameName,20,&r224x28e,&f16x16w,&f16x16g);
	ListBox* LB=DSY.addListBox(PIC1,60,120,6,&r224x28e,&f16x16g,&f16x16w,NULL);
	LB->OnUserClick=&LBCLICK;
	VScrollBar* VS=DSY.addVScrollBar(PIC1,LB->x-DSY.BaseX+LB->oneLx-1,1+LB->y-DSY.BaseY,1000,4,&sb_up_d,&sb_up_e,&sb_up_p,&sb_dn_d,&sb_dn_e,&sb_dn_p,&sb_vbare,&sb_vbar,&sb_btn);
	LB->VS=VS;
	BpxTextButton* OkBtn=DSY.addBpxTextButton(PIC1,10,315,"OK",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	BpxTextButton* CancelBtn=DSY.addBpxTextButton(PIC1,180,315,"ÎÒÌÅÍÀ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	CreateNewMapsList(LB);
	ItemChoose=-1;
	do{
		ProcessMessages();
		if(strlen(IB->Str))OkBtn->Enabled=true;
		else OkBtn->Enabled=false;
		DSY.ProcessDialogs();
		DSY.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose=mcmOk){
		NormalName(GameName);
		SaveExtendedMap(GameName);
	};
//qqwer:
	//FreeNames();
};
//----------------LOAD Map--------------//
void PreLoadExtendedMap(char* fnm);
void PostLoadExtendedMap(char* fnm);
void ProcessGLoadMap(){
	DialogsSystem DSY(smapx+(nsmaplx<<4)-(352>>1),smapy+(nsmaply<<4)-(352>>1));
	Picture* PIC1=DSY.addPicture(NULL,0,0,&y352x352,&y352x352,&y352x352);
	TextButton* LoadBtn=DSY.addTextButton(PIC1,352>>1,30,"ÇÀÃÐÓÇÈÒÜ ÊÀÐÒÓ",&FActive,&FPassive,&FDisable,1);
	ListBox* LB=DSY.addListBox(PIC1,60,100,6,&r224x28e,&f16x16g,&f16x16w,NULL);
	VScrollBar* VS=DSY.addVScrollBar(PIC1,LB->x-DSY.BaseX+LB->oneLx-1,1+LB->y-DSY.BaseY,1000,4,&sb_up_d,&sb_up_e,&sb_up_p,&sb_dn_d,&sb_dn_e,&sb_dn_p,&sb_vbare,&sb_vbar,&sb_btn);
	LB->VS=VS;
	BpxTextButton* OkBtn=DSY.addBpxTextButton(PIC1,10,315,"OK",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	BpxTextButton* CancelBtn=DSY.addBpxTextButton(PIC1,180,315,"ÎÒÌÅÍÀ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	CreateNewMapsList(LB);
	ItemChoose=-1;
	do{
		DSY.MarkToDraw();
		ProcessMessages();
		if(LB->CurItem==-1)OkBtn->Enabled=false;
		else OkBtn->Enabled=true;
		DSY.ProcessDialogs();
		DSY.MarkToDraw();
		DSY.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose==mcmOk&&LB->CurItem!=-1){
		UnLoading();
		PreLoadExtendedMap((LB->GetItem(LB->CurItem))->Message);
		MyNation=0;
		for(int i=0;i<8;i++){
			LoadNation("orcs.nds",1<<i,i);
			int xx,yy;
			//chooseCoor(&xx,&yy);
			CITY[i].CreateCity(4,4,i);
			//if(i<NPlayers){
			//	coorX[i]=xx;
			//	coorY[i]=yy;
			//	NATIONS[i].CreateTerrainAt(xx,yy,1);
			//};
		};
		//ItemChoose=mcmSingle;
		//MultiplayerStart=true;
		InitExplosions();
		PostLoadExtendedMap((LB->GetItem(LB->CurItem))->Message);
		CheapMode=false;
		ContinueGame=true;
		for(i=0;i<8;i++){
			RESRC[i][1]=100000000;
			RESRC[i][2]=100000000;
			RESRC[i][3]=100000000;
		};
	};
	//FreeNames();
};
//-----------------OPTIONS---------------//
void ProcessGOptions(){
};	

//---------------------------------------//
void NewMap(int szX,int szY);
extern byte mapPos[16];
void CreateNewMap(int SizeX,int SizeY){
	UnLoading();
	NewMap(SizeX,SizeY);
	MyNation=0;
	for(int i=0;i<8;i++){
		LoadNation("orcs.nds",1<<i,i);
		int xx,yy;
		chooseCoor(&xx,&yy);
		mapPos[i*2]=xx;
		mapPos[i*2+1]=yy;
		CITY[i].CreateCity(xx+4,yy+4,i);
		if(i<NPlayers){
			coorX[i]=xx;
			coorY[i]=yy;
			NATIONS[i].CreateTerrainAt(xx,yy,1);
			if(i==MyNation){
				mapx=xx-(smaplx>>1);
				mapy=yy-(smaply>>1);
				if(mapx<1)mapx=1;
				if(mapy<1)mapy=1;
				if(mapx+smaplx>=msx)mapx=msx-smaplx-1;
				if(mapy+smaply>=msy)mapy=msy-smaply-1;
			};
		};
	};
	ItemChoose=mcmSingle;
	InitExplosions();
};
//--------Choose units Dialog------------//
void ChooseUnit(){
	DialogsSystem DSY(smapx+(nsmaplx<<4)-(352>>1),smapy+(nsmaply<<4)-(352>>1));
	Picture* PIC1=DSY.addPicture(NULL,0,0,&y352x352,&y352x352,&y352x352);
	TextButton* LoadBtn=DSY.addTextButton(PIC1,352>>1,30,"ÂÛÁÎÐ ÎÁÚÅÊÒÀ",&FActive,&FPassive,&FDisable,1);
	ListBox* LB=DSY.addListBox(PIC1,60,100,6,&r224x28e,&f16x16g,&f16x16w,NULL);
	VScrollBar* VS=DSY.addVScrollBar(PIC1,LB->x-DSY.BaseX+LB->oneLx-1,1+LB->y-DSY.BaseY,1000,4,&sb_up_d,&sb_up_e,&sb_up_p,&sb_dn_d,&sb_dn_e,&sb_dn_p,&sb_vbare,&sb_vbar,&sb_btn);
	LB->VS=VS;
	BpxTextButton* OkBtn=DSY.addBpxTextButton(PIC1,10,315,"OK",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	BpxTextButton* CancelBtn=DSY.addBpxTextButton(PIC1,180,315,"ÎÒÌÅÍÀ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	LB->AddItem("Ñòåíà",4096);
	LB->AddItem("Ãðàíò",0);
	LB->AddItem("Ïèçàíò",1);
	LB->AddItem("Êàòàïóëüòà",2);
	LB->AddItem("Îãð",3);
	LB->AddItem("Òðîëëü",4);
	LB->AddItem("Êîëäóí",5);
	LB->AddItem("Ãíîìû",7);
	LB->AddItem("Òàíêåð",9);
	LB->AddItem("Òðàíñïîðò",10);
	LB->AddItem("Ìîðñêîé äüÿâîë",11);
	LB->AddItem("Äæàããåðíàóò",12);
	LB->AddItem("×åðåïàõà",13);
	LB->AddItem("Ðàçâåäêà",14);
	LB->AddItem("Äðàêîí",15);
	LB->AddItem("Ñêåëåò",18);
	LB->AddItem("Ãàäêèé äåìîí",19);
	LB->AddItem("Ôåðìà",20);
	LB->AddItem("Áàðàê",21);
	LB->AddItem("ÍËÎ ëîêàòîð",22);
	LB->AddItem("Ñìîòðîâàÿ áàøíÿ",23);
	LB->AddItem("Õàð÷åâíÿ îãðîâ",24);
	LB->AddItem("Ëàáîðàòîðèÿ àëõèìèêîâ",25);
	LB->AddItem("Ãíåçäî äðàêîíîâ",26);
	LB->AddItem("Âåðôü",27);
	LB->AddItem("Öåíòð ãîðîäà",28);
	LB->AddItem("Ñëåñàðíàÿ",29);
	LB->AddItem("Ëèòåéíûé öåõ",30);
	LB->AddItem("ÍÈÈ ìàãèè",31);
	LB->AddItem("Êóçíèöà",32);
	LB->AddItem("Î÷èñò. çàâîä",33);
	LB->AddItem("Íåôòÿíàÿ ïëàòôîðìà",34);
	LB->AddItem("Öèòàäåëü",35);
	LB->AddItem("Êðåïîñòü",36);
	LB->AddItem("Ñòîðîæåâàÿ áàøíÿ",37);
	LB->AddItem("Âîîðóæåííàÿ áàøíÿ",38);
	LB->AddItem("Òåëåïîðò",39);
	LB->AddItem("Òàðåëêà",40);
	LB->AddItem("Ïðîòèâîÿäåðêà",41);
	LB->AddItem("Áåðòà",42);
	LB->AddItem("ßäåðêà",43);
	ItemChoose=-1;
	do{
		ProcessMessages();
		if(LB->CurItem==-1)OkBtn->Enabled=false;
		else OkBtn->Enabled=true;
		LB->NeedToDraw=true;
		VS->NeedToDraw=true;
		DSY.ProcessDialogs();
		LB->NeedToDraw=true;
		VS->NeedToDraw=true;
		DSY.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose==mcmOk&&LB->CurItem!=-1){
		int choo=(LB->GetItem(LB->CurItem))->Param1;
		if(choo==4096)BuildWall=true;
		else Creator=choo;
		ContinueGame=true;
	};
};
//------MAIN MENU IN MAP EDITOR----------//
int ProcessGMapEditMainMenu(){
	ContinueGame=false;
	DialogsSystem GMM(smapx+(nsmaplx<<4)-(352>>1),smapy+(nsmaply<<4)-(352>>1));
	Picture* PIC1=GMM.addPicture(NULL,0,0,&y352x352,&y352x352,&y352x352);
	TextButton* ExitBtn=GMM.addTextButton(PIC1,352>>1,30,"ÂÎÇÂÐÀÒ Â ÌÅÍÞ",&FActive,&FPassive,&FDisable,1);
	TextButton* SaveBtn=GMM.addTextButton(PIC1,352>>1,30+40,"ÑÎÕÐÀÍÈÒÜ ÊÀÐÒÓ",&FActive,&FPassive,&FDisable,1);
	SaveBtn->UserParam=mcmSave;
	SaveBtn->OnUserClick=&MMItemChoose;
	TextButton* LoadBtn=GMM.addTextButton(PIC1,352>>1,30+80,"ÇÀÃÐÓÇÈÒÜ ÊÀÐÒÓ",&FActive,&FPassive,&FDisable,1);
	LoadBtn->UserParam=mcmLoad;
	LoadBtn->OnUserClick=&MMItemChoose;
	TextButton* NewBtn=GMM.addTextButton(PIC1,352>>1,30+120,"ÍÎÂÀß ÊÀÐÒÀ",&FActive,&FPassive,&FDisable,1);
	TextButton* HelpBtn=GMM.addTextButton(PIC1,352>>1,30+160,"ÏÎÌÎÙÜ",&FActive,&FPassive,&FDisable,1);
	ExitBtn->UserParam=mcmExit;
	ExitBtn->OnUserClick=&MMItemChoose;
	BpxTextButton* ResumeBtn=
		GMM.addBpxTextButton(PIC1,(352-164)>>1,300,"ÏÐÎÄÎËÆÈÒÜ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	ResumeBtn->UserParam=mcmResume;
	ResumeBtn->OnUserClick=&MMItemChoose;
stg:
	ItemChoose=-1;
	do{
		ProcessMessages();
		GMM.ProcessDialogs();
		GMM.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose==mcmLoad){
		ProcessGLoadMap();
		GMM.MarkToDraw();
		if(!ContinueGame)goto stg;
	};
	if(ItemChoose==mcmSave){
		ProcessGSaveMap();
		GMM.MarkToDraw();
		if(!ContinueGame)goto stg;
	};
	return ItemChoose;	
}; 
//----------MAIN MENU IN GAME------------//
int ProcessGMainMenu(){
	ContinueGame=false;
	GameMode=2;
	bool MIMOD=MiniMode;
	int mmmx=mapx;
	int mmmy=mapy;
	ClearMiniMode();
	DialogsSystem GMM(smapx+(nsmaplx<<4)-(352>>1),smapy+(nsmaply<<4)-(352>>1));
	Picture* PIC1=GMM.addPicture(NULL,0,0,&y352x352,&y352x352,&y352x352);
	TextButton* ExitBtn=GMM.addTextButton(PIC1,352>>1,30,"ÏÐÅÐÂÀÒÜ ÈÃÐÓ",&FActive,&FPassive,&FDisable,1);
	ExitBtn->UserParam=mcmExit;
	ExitBtn->OnUserClick=&MMItemChoose;
	TextButton* SaveBtn=GMM.addTextButton(PIC1,352>>1,30+40,"ÑÎÕÐÀÍÈÒÜ ÈÃÐÓ",&FActive,&FPassive,&FDisable,1);
	SaveBtn->UserParam=mcmSave;
	SaveBtn->OnUserClick=&MMItemChoose;
	TextButton* LoadBtn=GMM.addTextButton(PIC1,352>>1,30+80,"ÇÀÃÐÓÇÈÒÜ ÈÃÐÓ",&FActive,&FPassive,&FDisable,1);
	LoadBtn->UserParam=mcmLoad;
	LoadBtn->OnUserClick=&MMItemChoose;
	TextButton* OptionsBtn=GMM.addTextButton(PIC1,352>>1,30+120,"ÏÀÐÀÌÅÒÐÛ",&FActive,&FPassive,&FDisable,1);
	OptionsBtn->UserParam=mcmOptions;
	OptionsBtn->OnUserClick=&MMItemChoose;
	TextButton* HelpBtn=GMM.addTextButton(PIC1,352>>1,30+160,"ÏÎÌÎÙÜ",&FActive,&FPassive,&FDisable,1);
	HelpBtn->UserParam=mcmHelp;
	HelpBtn->OnUserClick=&MMItemChoose;
	BpxTextButton* ResumeBtn=
		GMM.addBpxTextButton(PIC1,(352-164)>>1,300,"ÏÐÎÄÎËÆÈÒÜ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	ResumeBtn->UserParam=mcmResume;
	ResumeBtn->OnUserClick=&MMItemChoose;
stg:
	ItemChoose=-1;
	do{
		ProcessMessages();
		GMM.ProcessDialogs();
		GMM.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose==mcmLoad){
		ProcessGLoadGame();
		GMM.MarkToDraw();
		if(!ContinueGame)goto stg;
	};
	if(ItemChoose==mcmSave){
		ProcessGSaveGame();
		GMM.MarkToDraw();
		if(!ContinueGame)goto stg;
	};
	if(ItemChoose==mcmOptions){
		ProcessOptions(smapx+(nsmaplx<<4)-(288>>1),smapy+(nsmaply<<4)-(288>>1));
		ContinueGame=true;
		ItemChoose=mcmResume;
	};
	if(ItemChoose==mcmHelp){
		ProcessHelp();
	};
	if(MIMOD)SetMiniMode();
	mapx=mmmx;
	mapy=mmmy;
	GameMode=0;
	return ItemChoose;	
};
//---------------------------------------//
extern int HintX;
extern int HintY;
void GSSetup800(){
	CBar(0,0,SCRSizeX,SCRSizeY,0);
	int PanelLx=160;
	int P1Ly=160;
	int P2Ly=160;//2
	int P3Ly=256;//3
	IconLx=48;
	IconLy=40;
	MiniMode=false;
	Shifter=5;
	Multip=0;
	smapx=PanelLx;
	smapy=0;
	smaplx=(RealLx-PanelLx)>>5;
	smaply=RSCRSizeY>>5;
	nsmaplx=smaplx;
	nsmaply=smaply;
	HintX=smapx+4;
	HintY=smapy+(smaply<<5)-30;
	minix=(PanelLx-(msx>>1))>>1;
	miniy=(P1Ly-(msy>>1))>>1;
	PrpNx=div(PanelLx,IconLx).quot;
	PrpNy=div(P2Ly,IconLy).quot;
	PrpX=(PanelLx-IconLx*PrpNx)>>1;
	PrpY=P1Ly+((P2Ly-IconLy*PrpNy)>>1);
	AblNx=PrpNx;
	AblNy=div(P3Ly,IconLy).quot;
	AblX=PrpX;
	AblY=P1Ly+P2Ly+((P3Ly-AblNy*IconLy)>>1);
	GSYS.~DialogsSystem();
	GVPort=GSYS.addViewPort(smapx,smapy,smaplx<<5,smaply<<5);
	Pan1=GSYS.addPicture(NULL,0,0,&w160x160,&w160x160,&w160x160);
	Pan2=GSYS.addPicture(NULL,0,P1Ly,&w160x160,&w160x160,&w160x160);
	Pan3=GSYS.addPicture(NULL,0,P1Ly+P2Ly,&w160x256,&w160x256,&w160x256);
	//Pan2=GSYS.addPicture(NULL,0,P1Ly,&w160x256,&w160x256,&w160x256);
	//Pan3=GSYS.addPicture(NULL,0,P1Ly+P2Ly,&w160x160,&w160x160,&w160x160);
	MiniVPort=GSYS.addViewPort(minix,miniy,msx>>1,msy>>1);
	AblVPort=GSYS.addViewPort(AblX,AblY,AblNx*IconLx,AblNy*IconLy);
	PrpVPort=GSYS.addViewPort(PrpX,PrpY,PrpNx*IconLx,PrpNy*IconLy);
	//Pan4=GSYS.addColoredBar(0,P1Ly+P2Ly+P3Ly,PanelLx,(RSCRSizeY&(1023-31))-P1Ly-P1Ly-P3Ly,0);
	//if((smaply<<5)<RSCRSizeY)
	//	Pan5=GSYS.addColoredBar(PanelLx,smaply<<5,
	//		RSCRSizeX-PanelLx,RSCRSizeY&31,0);
	//else Pan5=NULL;
	LoadPalette("darkw.pal");
};
void GFieldShow();
void GMiniShow();
void CopyToScreen(int x,int y,int Lx,int Ly);
void DrawAllScreen(){
	//COUNTER++;
	GFieldShow();
	Pan1->OnDraw(Pan1);
	GMiniShow();
	Pan2->OnDraw(Pan2);
	ShowProp();
	Pan3->OnDraw(Pan3);
	ShowAbility();
	//Pan4->OnDraw(Pan4);
	if(Pan5)Pan5->OnDraw(Pan5);
	CopyToScreen(0,0,RealLx,RSCRSizeY);
	HandleMouse(mouseX,mouseY);
	MFix();
};
void FastScreenProcess(){
//	SERROR2();
	GFieldShow();
	ProcessMessages();
	GMiniShow();
	Pan2->OnDraw(Pan2);
	Pan3->OnDraw(Pan3);
	ShowProp();
	ShowAbility();
	Pan1->NeedToDraw=false;
	GSYS.ProcessDialogs();
	Pan1->NeedToDraw=false;
	Pan2->NeedToDraw=false;
	Pan3->NeedToDraw=false;
	GVPort->NeedToDraw=true;
	MiniVPort->NeedToDraw=true;
	AblVPort->NeedToDraw=true;
	PrpVPort->NeedToDraw=true;
	HandleMouse(mouseX,mouseY);
	MFix();
//	SERROR2();
	int time0=GetTickCount();
	GSYS.RefreshView();
	time8=GetTickCount()-time0;
//	SERROR1();
};
void HandleAntiNuc();
void PreDrawGameProcess();
void PostDrawGameProcess();
void IAmLeft();
void PlayGame(){
	GSSetup800();
	InitGame();
	DrawAllScreen();
	GameNeedToDraw=false;
	GameExit=false;
	MakeMenu=false;
	InGame=true;
	do{
		time1=GetTickCount();
		ProcessMessages();
		time2=GetTickCount()-time1;
		if(bActive){
			GameMode=0;
			PreDrawGameProcess();
			time3=GetTickCount()-time1-time2;
			ProcessMessages();
			if(GameNeedToDraw){
				//COUNTER++;
				DrawAllScreen();
				GameNeedToDraw=false;
			}else FastScreenProcess();
			time4=GetTickCount()-time1-time2-time3;
			//DrawAllScreen();
			//DrawAllScreen();
			ProcessMessages();
			PostDrawGameProcess();
			time5=GetTickCount()-time1-time2-time3-time4;
			if(MakeMenu){
				switch(MenuType){
				case 1:	if(ProcessGMainMenu()==mcmExit){
							GameExit=true;
							ShutdownMultiplayer();
							//IAmLeft();
						};
						break;
				case 2: ChooseUnit();
					    break;
				case 3: EnterChat();
						break;
				case 4: ProcessHelp();
						break;
				};
				MakeMenu=false;
			};
		}else GameNeedToDraw=true;
	}while(!GameExit);
};
void DrawAllEditScreen(){
	GFieldShow();
	Pan1->OnDraw(Pan1);
	GMiniShow();
	Pan2->OnDraw(Pan2);
	ShowProp();
	Pan3->OnDraw(Pan3);
	ShowAbility();
	//Pan4->OnDraw(Pan4);
	if(Pan5)Pan5->OnDraw(Pan5);
	CopyToScreen(0,0,RealLx,RSCRSizeY);
};
void FastEditScreenProcess(){
	GFieldShow();
	ProcessMessages();
	GMiniShow();
	Pan2->OnDraw(Pan2);
	Pan3->OnDraw(Pan3);
	ShowProp();
	ShowAbility();
	Pan1->NeedToDraw=false;
	GSYS.ProcessDialogs();
	Pan1->NeedToDraw=false;
	Pan2->NeedToDraw=false;
	Pan3->NeedToDraw=false;
	GVPort->NeedToDraw=true;
	MiniVPort->NeedToDraw=true;
	AblVPort->NeedToDraw=true;
	PrpVPort->NeedToDraw=true;
	HandleMouse(mouseX,mouseY);
	MFix();
	GSYS.RefreshView();
};
void EditGame(){
	GSSetup800();
	DrawAllEditScreen();
	GameNeedToDraw=false;
	GameExit=false;
	MakeMenu=false;
	do{
		ProcessMessages();
		if(bActive){
			//SERROR2();
			PreDrawGameProcess();
			//SERROR1();
			ProcessMessages();
			if(GameNeedToDraw){
				DrawAllScreen();
				GameNeedToDraw=false;
			}else FastScreenProcess();
			//DrawAllScreen();
			//SERROR();
			ProcessMessages();
		PostDrawGameProcess();
			if(MakeMenu){
				switch(MenuType){
				case 1:	if(ProcessGMapEditMainMenu()==mcmExit)GameExit=true;
						break;
				case 2: ChooseUnit();
					    break;
				case 3: ProcessResEdit();
						break;
				};
				MakeMenu=false;
			};
		}else GameNeedToDraw=true;
	}while(!GameExit);
};
//--------------ALL GAME IS IN THIS PROCEDURE!-------------//
void AllGame(){
stgame:
	int mret=processMainMenu();
	if(mret==mcmExit)return;
	if(mret==mcmSingle){
		if(EditMapMode)EditGame();
		else PlayGame();
		goto stgame;
	}else goto stgame;
};
extern int PLNAT[8];
void PrepareGameMedia(byte myid){
		rpos=0;
		setCoor();
		UnLoading();
		PreLoadExtendedMap(CurrentMap);
		memcpy(&RESRC[0][0],&RES[0][0],sizeof RES);
		MyNation=0;
		for(int i=0;i<8;i++){
			if(!PLNAT[i])
				LoadNation("orcs.nds",1<<i,i);
			else LoadNation("PEOPLES.nds",1<<i,i);
			MyNation=myid;
			int xx,yy;
			xx=mapPos[i*2];
			yy=mapPos[i*2+1];
			//chooseCoor(&xx,&yy);
			CITY[i].CreateCity(xx+4,yy+4,i);
			if(i<NPlayers){
				//coorX[i]=xx;
				//coorY[i]=yy;
				NATIONS[i].CreateTerrainAt(xx+5,yy+5,1);
				if(i==MyNation){
					mapx=xx-(smaplx>>1);
					mapy=yy-(smaply>>1);
					if(mapx<1)mapx=1;
					if(mapy<1)mapy=1;
					if(mapx+smaplx>=msx)mapx=msx-smaplx-1;
					if(mapy+smaply>=msy)mapy=msy-smaply-1;
				};
			};
		};
		PostLoadExtendedMap(CurrentMap);
		CheapMode=false;
		ItemChoose=mcmSingle;
		MultiplayerStart=true;
		InitExplosions();
		PrepareToGame();
		InitGame();
};
void PrepareEditMedia(byte myid){
		rpos=0;
		setCoor();
		UnLoading();
		NewMap(240,240);
		MyNation=0;
		for(int i=0;i<8;i++){
			if(!PLNAT[i])
				LoadNation("orcs.nds",1<<i,i);
			else LoadNation("PEOPLES.nds",1<<i,i);
			MyNation=myid;
			int xx,yy;
			chooseCoor(&xx,&yy);
			CITY[i].CreateCity(xx+4,yy+4,i);
			if(i<NPlayers){
				coorX[i]=xx;
				coorY[i]=yy;
				NATIONS[i].CreateTerrainAt(xx,yy,1);
				if(i==MyNation){
					mapx=xx-(smaplx>>1);
					mapy=yy-(smaply>>1);
					if(mapx<1)mapx=1;
					if(mapy<1)mapy=1;
					if(mapx+smaplx>=msx)mapx=msx-smaplx-1;
					if(mapy+smaply>=msy)mapy=msy-smaply-1;
				};
			};
		};
		ItemChoose=mcmSingle;
		//MultiplayerStart=true;
		InitExplosions();
		PrepareToEdit();
		ItemChoose=mcmSingle;
		InitGame();
};
void RedrawGameBackground(){
	if(GameMode==1){//main menu
		int mmlx=MainMenuGround.PicPtr[0];
		int mmly=MainMenuGround.PicPtr[1];
		MainMenuGround.Draw((RealLx-mmlx)>>1,(RSCRSizeY-mmly)>>1);
		FlipPages();
	}else{
		DrawAllScreen();		
	};
};
void ProcessHelp(){
	ItemChoose=-1;
	DialogsSystem GMM(smapx+(nsmaplx<<4)-(352>>1),smapy+(nsmaply<<4)-(352>>1));
	Picture* PIC1=GMM.addPicture(NULL,0,0,&y352x352,&y352x352,&y352x352);
	BpxTextButton* OkBtn=GMM.addBpxTextButton(PIC1,352>>1,320,"Oòìåíà",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	TextViewer* TV=GMM.addTextViewer(PIC1,20,20,320,320,"help.txt",&f16x16w);
	do{
		ProcessMessages();
		if(KeyPressed&&(LastKey==13||LastKey==27)){
			KeyPressed=false;
			ItemChoose=mcmOk;
		};
		GMM.MarkToDraw();
		GMM.ProcessDialogs();
		GMM.RefreshView();
	}while(ItemChoose==-1);
	ContinueGame=true;
};




















void xMainMenu(){
	DialogsSystem MMenu((RSCRSizeX-PanelLx)>>1,RSCRSizeY-PanelLy-80);
	Picture* PIC=MMenu.addPicture(NULL,0,0,&Panel1,&Panel1,&Panel1);
	//MMenu.addBpxTextButton(PanelLx>>1,30,"Îäèíî÷íàÿ èãðà",&FActive,&FPassive,&FDisable,&r164x28p,&r164x28e,&w164x28d);
	//MMenu.addBpxTextButton(PanelLx>>1,60,"Èãðà ïî ñåòè",&FActive,&FPassive,&FDisable,&r164x28p,&r164x28e,&w164x28d);
	//MMenu.addBpxTextButton(PanelLx>>1,90,"Âèäåî",&FActive,&FPassive,&FDisable,&r164x28p,&r164x28e,&w164x28d);
	BpxTextButton* ExitBtn=MMenu.addBpxTextButton(PIC,60,220,"ÂÛÕÎÄ",&f16x16g,&f16x16y,&f16x16y,&r164x28p,&r164x28e,&w164x28d);
	MMenu.addTextButton(PIC,PanelLx>>1,30,"Îäèíî÷íàÿ èãðà",&FActive,&FPassive,&FDisable,1);
	MMenu.addTextButton(PIC,PanelLx>>1,70,"Èãðà ïî ñåòè",&FActive,&FPassive,&FDisable,1);
	MMenu.addTextButton(PIC,PanelLx>>1,110,"Âèäåî",&FActive,&FPassive,&FDisable,1);
	//TextButton* ExitBtn=MMenu.addTextButton(PanelLx>>1,150,"Âûõîä",&FActive,&FPassive,&FDisable,1);
	
	MMenu.addHScrollBar(PIC,80,10,10,4,&sb_lf_d,&sb_lf_e,&sb_lf_p,&sb_ri_d,&sb_ri_e,&sb_ri_p,&sb_hbare,&sb_hbar,&sb_btn);
	ListBox* LB=MMenu.addListBox(PIC,200,100,6,&r224x28e,&f16x16g,&f16x16w,NULL);
	VScrollBar* VS=MMenu.addVScrollBar(PIC,LB->x-MMenu.BaseX+LB->oneLx-1,1+LB->y-MMenu.BaseY,1000,4,&sb_up_d,&sb_up_e,&sb_up_p,&sb_dn_d,&sb_dn_e,&sb_dn_p,&sb_vbare,&sb_vbar,&sb_btn);
	LB->VS=VS;
	char uuu[128];
	char uuu1[128];
	uuu[0]=0;
	MMenu.addInputBox(PIC,5,-65,uuu,20,&r224x28e,&f16x16w,&f16x16g);
	MMenu.addInputBox(PIC,5,-195,uuu1,20,&r224x28e,&f16x16w,&f16x16g);
	MMenu.addCheckBox(PIC,-100,0," Item1",1,1,&chk2_on,&chk2_off,&f16x16w,&f16x16g);
	MMenu.addCheckBox(PIC,-100,30," Item2",1,0,&chk2_on,&chk2_off,&f16x16w,&f16x16g);
	MMenu.addCheckBox(PIC,-100,60," Item3",1,0,&chk2_on,&chk2_off,&f16x16w,&f16x16g);
	MMenu.addCheckBox(PIC,-100,90," Item4",1,0,&chk2_on,&chk2_off,&f16x16w,&f16x16g);

	MMenu.addCheckBox(PIC,-200,0," Item1",2,1,&chk2_on,&chk2_off,&f16x16w,&f16x16g);
	MMenu.addCheckBox(PIC,-200,30," Item2",2,0,&chk2_on,&chk2_off,&f16x16w,&f16x16g);
	MMenu.addCheckBox(PIC,-200,60," Item3",2,0,&chk2_on,&chk2_off,&f16x16w,&f16x16g);
	MMenu.addCheckBox(PIC,-200,90," Item4",2,0,&chk2_on,&chk2_off,&f16x16w,&f16x16g);

	MMenu.addCheckBox(PIC,-100,200," xItem1",0,1,&chk1_on,&chk1_off,&f16x16w,&f16x16g);
	MMenu.addCheckBox(PIC,-100,230," xItem2",0,1,&chk1_on,&chk1_off,&f16x16w,&f16x16g);
	MMenu.addCheckBox(PIC,-100,260," xItem3",0,1,&chk1_on,&chk1_off,&f16x16w,&f16x16g);
	MMenu.addCheckBox(PIC,-100,290," xItem4",0,1,&chk1_on,&chk1_off,&f16x16w,&f16x16g);
	LB->AddItem("save1",0);
	LB->AddItem("save2",1);
	LB->AddItem("save3",0);
	LB->AddItem("save4",1);
	LB->AddItem("save5",0);
	LB->AddItem("save6",1);
	LB->AddItem("save7",0);
	LB->AddItem("save8",1);
	LB->AddItem("save9",0);
	LB->AddItem("save10",1);
	LB->AddItem("save11",0);
	LB->AddItem("save12",1);
	LB->AddItem("save13",0);
	LB->AddItem("save14",1);
	//ExitBtn->OnClick=&OnExit;
	LoadPalette("015w.pal");
	MainMenuGround.Draw(0,0);
	FlipPages();
	do{
		//MainMenuGround.Draw(0,0);
		ProcessMessages();
		MMenu.ProcessDialogs();
		//ShowMPtr();
		//if(DDDebug)FlipPages();
		MMenu.RefreshView();
		for(int uu=0;uu<1000000;uu++)ProcessMessages();
		//FlipPages();

	}while(true);
};