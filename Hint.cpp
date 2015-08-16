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
static int HintTime;
static int HintTime1;
void ResFon(int x,int y,int Lx,int Ly);
void ScrCopy(int x,int y,int Lx,int Ly);
char HintStr[128];
char HintStr1[128];
int HintX;
int HintY;
int HintLx;
int HintLy;
void SetupHint(){
	HintY=smapy+smaply*32-20;
	HintX=smapx;
	HintLx=720;
	HintLy=16;
	HintTime=0;
};
void ShowHint(){
	//ResFon(HintX,HintY,HintLx,HintLy);
	if(HintTime)ShowString(HintX,HintY,HintStr,&rlf_s);
	if(HintTime1)ShowString(HintX,HintY-20,HintStr1,&rlf_s);
	//ScrCopy(HintX,HintY,HintLx,HintLy);
};
void HideHint(){
	//ResFon(HintX,HintY,HintLx,HintLy);
	//ScrCopy(HintX,HintY,HintLx,HintLy);
};
void AssignHint(char* s,int time){
	strcpy(HintStr,s);
	ShowHint();
	HintTime=time;
};
void AssignHint1(char* s,int time){
	strcpy(HintStr1,s);
	HintTime1=time;
};
void GetChar(GeneralObject* GO,char* s){
	Visuals* VS=(Visuals*)GO;
	char GG[128];
	char ADD[128];
	GG[0]=0;
	strcat(GG,VS->Message);
	switch(GO->Kind){
	case 1:
		sprintf(ADD," : Удар:%d-%d, Защита:%d, Макс.прочность:%d",
			int(VS->info.Basic.MinDamage),
			int(VS->info.Basic.MaxDamage+VS->info.Basic.MinDamage),
			int(VS->info.Basic.MaxShield),
			int(VS->info.Basic.MaxLife));
		strcat(GG,ADD);
		break;
	case 2:
		sprintf(ADD," : Удар:%d-%d, Защита:%d, Макс.прочность:%d",
			int(VS->info.Basic.MinDamage),
			int(VS->info.Basic.MaxDamage+VS->info.Basic.MinDamage),
			int(VS->info.Basic.MaxShield),
			int(VS->info.Basic.MaxLife));
		strcat(GG,ADD);
		break;
	case 4:
		sprintf(ADD," :Защита:%d, Макс.прочность:%d",
			int(VS->info.Basic.MaxShield),
			int(VS->info.Basic.MaxLife));
		strcat(GG,ADD);
	};
	strcpy(s,GG);
};
void GetMonsterCapsString(char* st);
void ProcessHint(){
	ShowHint();
	if(HintTime){
		HintTime--;
	};
	if(HintTime1){
		HintTime1--;
	};
};
