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
#include <assert.h>
void CBar(int x,int y,int Lx,int Ly,byte c);
extern SQPicture MainMenuGround;
extern SQPicture Panel1;
extern SQPicture r128x20e;
extern SQPicture r128x20p;
extern SQPicture r80x20e;
extern SQPicture r80x20p;
extern SQPicture w106x28d;
extern SQPicture r106x28e;
extern SQPicture r106x28p;
extern SQPicture w164x28d;
extern SQPicture r164x28e;
extern SQPicture r164x28p;
extern SQPicture w224x28d;
extern SQPicture r224x28e;

extern SQPicture sb_up_e;
extern SQPicture sb_up_p;
extern SQPicture sb_up_d;

extern SQPicture sb_dn_e;
extern SQPicture sb_dn_p;
extern SQPicture sb_dn_d;

extern SQPicture sb_lf_e;
extern SQPicture sb_lf_p;
extern SQPicture sb_lf_d;

extern SQPicture sb_ri_e;
extern SQPicture sb_ri_p;
extern SQPicture sb_ri_d;

extern SQPicture sb_vbar;
extern SQPicture sb_vbare;
extern SQPicture sb_hbar;
extern SQPicture sb_hbare;

extern SQPicture sb_btn;

extern SQPicture chk1_on;
extern SQPicture chk1_off;
extern SQPicture chk2_on;
extern SQPicture chk2_off;

extern SQPicture w160x160;
extern SQPicture w160x256;
extern SQPicture y288x256;
extern SQPicture y352x352;
extern SQPicture y288x128;
extern SQPicture w288x256;
extern SQPicture w352x352;
extern SQPicture w288x128;

extern RLCTable TPassive;
extern RLCTable TActive;
extern RLCTable TDisable;
extern RLCFont FPassive;
extern RLCFont FActive;
extern RLCFont FDisable;
int PanelLx;
int PanelLy;
bool MultiplayerStart;
bool GameNeedToDraw;
extern word NPlayers;
void CloseMPL();
void IBEnumeratePlayers(InputBox** IB);
void LBEnumerateSessions(ListBox* LB);
void AnalyseMessages();
bool CreateNamedSession(char* Name);
bool JoinNameToSession(int ns,char* Name);
void FreeDDObjects( void );
bool StartIGame();
byte MyRace;
//------MAIN MENU IN MAP EDITOR----------//
int ProcessGMapEditoMainMenu(){
	ContinueGame=false;
	DialogsSystem GMM(smapx+(smaplx<<4)-(352>>1),smapy+(smaply<<4)-(352>>1));
	Picture* PIC1=GMM.addPicture(NULL,0,0,&y352x352,&y352x352,&y352x352);
	TextButton* ExitBtn=GMM.addTextButton(PIC1,352>>1,30,"ÏÐÅÐÂÀÒÜ ÈÃÐÓ",&FActive,&FPassive,&FDisable,1);
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
		ProcessGLoadGame();
		GMM.MarkToDraw();
		if(!ContinueGame)goto stg;
	};
	if(ItemChoose==mcmSave){
		ProcessGSaveGame();
		GMM.MarkToDraw();
		if(!ContinueGame)goto stg;
	};
	return ItemChoose;	
};