/* Properties of the selected objects viewering and *
 * corresponding mouse handling                     */
#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mgraph.h"
#include "mouse.h"
#include "menu.h"
#include "MapDiscr.h"
#include "mode.h"
#include "Nature.h"
#include "fonts.h"
#include "TopZones.h"
#include "walls.h"
#include "Megapolis.h"
#include "GSound.h"
#define MaxO 64
void AssignHint1(char* s,int time);
bool SelSoundDobe;
bool SelSoundReady;
int SelSoundType;
word GetOwner(int x,int y);
int CreateRZone(int x,int y,int lx,int ly,HandlePro* HPro,HandlePro* RHPro,int Index,char* Hint);
extern byte fon000[1024*768];
int PrpX;
int PrpY;
int PrpNx;
int PrpNy;
int AblX;
int AblY;
int AblNx;
int AblNy;
static bool PropClean=false;
static bool AbltClean=false;
int IconLx;
int IconLy;
struct MonInf{
	GeneralObject* RF;
	int Life;
	int MaxLife;
	int Magic;
	int MaxMag;
	int N;
	word Last;
	word ID;
};
struct AblInf{
	Nation* NT;
	word OInd;
	word Kind;
	word UPIND;
};
bool FindResource(byte* xx,byte* yy,byte Radius,byte kind,ZType zz);
MonInf MList[MaxO];
AblInf AList[MaxO];
int GetProgress(word ID);
int GetAmount(word ID);
int SubIcon;
static GeneralObject* LastGO;
static Nation* LastNT;
static word LastAmount;
static word LastID;
extern int curptr;
extern int curdx;
extern int curdy;
//uniq properties
bool GetCoord;
UniqMethood* UNIM;
void InitPrpBar(){
	PrpNx=4;
	PrpNy=2;
	PrpX=10;
	PrpY=(msy>>1)+miniy+16;
	AblX=PrpX;
	AblY=PrpY+PrpNy*IconLy+16;
	AblNx=PrpNx;
	AblNy=6;
	SubIcon=-1;
};
void ResFon(int x,int y,int Lx,int Ly){
	int x1=(x&(65535-3));
	int lx=((x+Lx+4)&(65535-3))-x1;
	int StDy=SCRSizeX-lx;
	int FONDy=COPYSizeX-lx;
	int DsOf=int(ScreenPtr)+x1+y*SCRSizeX;
	int SrOf=int(fon000)+x1+y*COPYSizeX;
	int lx1=lx>>2;
	__asm{
		push	esi
		push	edi
		cld
		mov		eax,Ly
		mov		esi,SrOf
		mov		edi,DsOf
		mov		ebx,StDy
jj1:	mov		ecx,lx1
		rep		movsd
		add		esi,FONDy
		add		edi,ebx
		dec		eax	
		jnz		jj1
	};
};
void ScrCopy(int x,int y,int Lx,int Ly){
	//if(DDebug){
//
//		return;
//	};
	int x1=(x&(65535-3));
	int lx=((x+Lx+4)&(65535-3))-x1;
	int StDy=RSCRSizeX-lx;
	int FONDy=SCRSizeX-lx;
	int DsOf=int(RealScreenPtr)+x1+y*RSCRSizeX;
	int SrOf=int(ScreenPtr)+x1+y*SCRSizeX;
	int lx1=lx>>2;
	__asm{
		push	esi
		push	edi
		cld
		mov		eax,Ly
		mov		esi,SrOf
		mov		edi,DsOf
		mov		ebx,StDy
jj1:	mov		ecx,lx1
		rep		movsd
		add		esi,FONDy
		add		edi,ebx
		dec		eax	
		jnz		jj1
	};
};
int NINF,NABL;
void GeneralObject::GetMonsterCostString(char* st){
	char uu[128];
	for(int i=0;i<4;i++){
		if(ResourceID[i]!=255){
			sprintf(uu," %s : %d",RDS[ResourceID[i]].Name,int(ResAmount[i]));
			if(i>0){
				strcat(st,",");
				strcat(st,uu);
			}else {
				strcat(st," ");
				strcat(st,uu);
			};
		};
	};
};
void Nation::GetUpgradeCostString(char* st,word UI){
	char uu[128];
	sprintf(uu," %s : %d",RDS[1].Name,int(UPG.utp[UI]->Cost));
	strcat(st,uu);
};
void HPSEL(int i){
	if(GetKeyState(VK_SHIFT)&0x8000){
		CmdChooseUnSelType(MyNation,i);
	}else{
		CmdChooseSelType(MyNation,i);
	};
};
void ShowProp(){
	NINF=0;
	InitZones();
	word MID;
	OneObject* OBJ;
	GeneralObject* GO;
	MonInf* MI;
	int i,j;
	//ResFon(PrpX,PrpY,PrpNx*IconLx,PrpNy*IconLy);
	word Nsel=NSL[MyNation];
	word* SMon=Selm[MyNation];
	if(!Nsel)return;
	if(SelSoundReady){
		if(SMon[0]!=0xFFFF){
			OneObject* OB=Group[SMon[0]];
			if(OB){
				if(SelSoundType)AddOrderEffect(OB->x,OB->y,OB->Ref.General->OrderSound);
				else AddOrderEffect(OB->x,OB->y,OB->Ref.General->ClickSound);
				SelSoundReady=false;
			};
		};
	};
	for(i=0;i<Nsel;i++){
		MID=SMon[i];
		if(MID!=0xFFFF){
			OBJ=Group[MID];
			if(int(OBJ)&&!OBJ->Sdoxlo){
				GO=OBJ->Ref.General;
				for(j=0;j<NINF;j++){
					if(GO==MList[j].RF)break;
				};
				MI=&(MList[j]);
				if(j>=NINF){
					MI->Life=0;
					MI->MaxLife=0;
					MI->Magic=0;
					MI->MaxMag=0;
					MI->N=0;
					NINF=j+1;
					MI->Last=0;
					MI->ID=0;
				};
				MI->RF=GO;
				MI->Life+=OBJ->Life;
				MI->MaxLife+=OBJ->MaxLife;
				//MI->Magic+=OBJ->Magic;
				if(MI->MaxMag<OBJ->Magic)MI->MaxMag=OBJ->Magic;
				MI->Magic=MI->MaxMag;
				MI->N++;
				if(MI->N==1&&!OBJ->Ready)MI->Last=MID;
				if(OBJ->Ready)MI->Last=MID;
				MI->ID=OBJ->NIndex;
			};
		};
	};
	int PN=PrpNx*PrpNy;
	if(NINF>PN)NINF=PN;
	int xx=0;
	int x1=PrpX;
	int y1=PrpY;
	OneSlide* OC;
	char str[32];
	for(i=0;i<NINF;i++){
		MI=&(MList[i]);
		OBJ=Group[MI->Last];
		OBJ->LoadAnimation(3,0,0);
		OBJ->LoadAnimation(3,8,0);
		OC=OBJ->ARegs[3].Anm[0].Movie;
		ShowRLCItem(x1,y1,&MImage[OC->FileID],OC->spr,MyNation);
		Xbar(x1,y1,IconLx-2,IconLy-2,15);
		Hline(x1+1,y1+IconLy-6,x1+IconLx-4,15);
		int HL;
		if(MI->MaxLife>0) HL=div(MI->Life*(IconLx-4),MI->MaxLife).quot;
		else HL=0;
		MI->Magic=div(MI->Magic,NINF).quot;
		Hline(x1+1,y1+IconLy-5,x1+HL,255);
		Hline(x1+1,y1+IconLy-4,x1+HL,255);
		sprintf(str,"%d",MI->N);
		ShowString(x1+3,y1+3,str,&rlf1_s);
		char sxt[128];
		GetChar(MI->RF,sxt);
		CreateZone(x1,y1,IconLx,IconLy,&HPSEL,MI->ID,sxt);
		xx++;
		if((!OBJ->Ready)&&OBJ->NUstages){
			HL=div(OBJ->Ustage*(IconLx-4),OBJ->NUstages).quot;
			Hline(x1+1,y1+IconLy-8,x1+HL,clrGreen);
			Hline(x1+1,y1+IconLy-7,x1+HL,clrGreen);
		};
		if(xx>=PrpNx){
			xx=0;
			x1=PrpX;
			y1+=IconLy;
		}else x1+=IconLx;
	};
	//show properties of the one kind of monsters
	if(NINF==1){
		MI=&(MList[0]);
		OBJ=Group[MI->Last];
		if(!OBJ)return;
		if(OBJ->Ready){
			x1=PrpX+4;
			y1=PrpY+IconLy+4;
			sprintf(str,"Жизнь:%d/%d",int(MI->Life),int(MI->MaxLife));
			ShowString(x1,y1,str,&f16x16w);
			y1+=16;
			sprintf(str,"Броня:%d",int(OBJ->Ref.Visual->info.Basic.MaxShield));
			ShowString(x1,y1,str,&f16x16w);
			y1+=16;
			sprintf(str,"Удар:%d-%d",(int(OBJ->Ref.Visual->info.Basic.MinDamage)*OBJ->xForce)>>4,(int(OBJ->Ref.Visual->info.Basic.MinDamage+OBJ->Ref.Visual->info.Basic.MaxDamage)*OBJ->xForce)>>4);
			ShowString(x1,y1,str,&f16x16w);
			y1+=16;
			if(MI->Magic){
				sprintf(str,"Магия:%d",MI->Magic);
				ShowString(x1,y1,str,&f16x16w);
				y1+=16;
			};
			if(OBJ->Ref.General->canNucAttack){
				sprintf(str,"Ракета:%d%%",div(5000-OBJ->delay,50).quot);
				ShowString(x1,y1,str,&f16x16w);
				y1+=16;
			};
			if(OBJ->AntiNuc){
				sprintf(str,"Ракет:%d",OBJ->RAmount);
				ShowString(x1,y1,str,&f16x16w);
				y1+=16;
				if(OBJ->RAmount<10){
					sprintf(str,"Следующая:%d%%",div(3000-OBJ->delay,30).quot);
					ShowString(x1,y1,str,&f16x16w);
					y1+=16;
				};
			};
			if(OBJ->Ref.General->cpbFarm){
				Nation* NT=OBJ->Nat;
				City* CT=NT->CITY;
				sprintf(str,"Всего ферм:%d",CT->ReadyAmount[OBJ->NIndex]);
				ShowString(x1,y1,str,&f16x16w);
				y1+=16;
				sprintf(str,"Нужно:%d",div(NT->NGidot,15).quot+1);
				ShowString(x1,y1,str,&f16x16w);
				y1+=16;
				sprintf(str,"Обитателей:%d",NT->NGidot);
				ShowString(x1,y1,str,&f16x16w);
				y1+=16;

			};
			if(OBJ->NInside){
				sprintf(str,"Внутри:%d",OBJ->NInside);
				ShowString(x1,y1,str,&f16x16w);
				y1+=16;
			};
			if(OBJ->Repair){
				ShowString(x1,y1,"Ремонтник",&f16x16w);
				y1+=16;
			};
		};
	};
};
extern bool BuildMode;
extern OneSlide* OSB;
extern byte blx;
extern byte bly;
extern word BuildingID;
extern Nation* BNat;
void CmdUnProduceObj(byte NI,word Type);
void RHPR(int i){
	AblInf* AI=&AList[i];
	Nation* NT=AI->NT;
	word j=AI->OInd;
	GeneralObject* GO=NT->Mon[j];
	if(!GO->cpbBuilding)
		CmdUnProduceObj(MyNation,j);
};
void HPR(int i){
	AblInf* AI=&AList[i];
	Nation* NT=AI->NT;
	word j=AI->OInd;
	GeneralObject* GO=NT->Mon[j];
	if(GO->cpbBuilding){
		for(int jj=0;jj<4;jj++){
			byte RID=GO->ResourceID[jj];
			if(RID!=255){
				if(GO->ResAmount[jj]>RESRC[MyNation][RID]){
					if(RID==1)AssignHint1("Не хватает золота.",20);
					if(RID==2)AssignHint1("Не хватает дерева.",20);
					return;
				};
			};
		};
		BuildMode=true;
		blx=GO->SizeX;
		bly=GO->SizeY;
		BuildingID=j;
		BNat=NT;
		word p=GO->NAnm;
		for(word q=0;q<p;q++){
			if(GO->lpFAnim[q].WhatFor==0)break;
		};
		if(q>=p)q=p-1;
		OSB=GO->lpFAnim[q].Anm->Movie;
	}else{
		if(GetKeyState(VK_SHIFT)&0x8000){
			CmdProduceObj(MyNation,j);
			CmdProduceObj(MyNation,j);
			CmdProduceObj(MyNation,j);
			CmdProduceObj(MyNation,j);
			CmdProduceObj(MyNation,j);
		}else CmdProduceObj(MyNation,j);
	};
};
void HPR1(int i){
	Nation* NT=&NATIONS[MyNation];
	GeneralObject* GO=NT->Mon[i];
	if(GO->cpbBuilding){
		for(int jj=0;jj<4;jj++){
			byte RID=GO->ResourceID[jj];
			if(RID!=255){
				if(GO->ResAmount[jj]>RESRC[MyNation][RID]){
					if(RID==1)AssignHint1("Не хватает золота.",20);
					if(RID==2)AssignHint1("Не хватает дерева.",20);
					return;
				};
			};
		};
		BuildMode=true;
		blx=GO->SizeX;
		bly=GO->SizeY;
		BuildingID=i;
		BNat=NT;
		word p=GO->NAnm;
		for(word q=0;q<p;q++){
			if(GO->lpFAnim[q].WhatFor==0)break;
		};
		if(q>=p)q=p-1;
		OSB=GO->lpFAnim[q].Anm->Movie;
	}else{
		if(GetKeyState(VK_SHIFT)&0x8000){
			CmdProduceObj(MyNation,i);
			CmdProduceObj(MyNation,i);
			CmdProduceObj(MyNation,i);
			CmdProduceObj(MyNation,i);
			CmdProduceObj(MyNation,i);
		}else CmdProduceObj(MyNation,i);
	}
};
void DoUPG(int i){
	AblInf* AI=&AList[i];
	Nation* NT=AI->NT;
	word j=AI->OInd;
	CmdPerformUpgrade(MyNation,j);
};
void DoUPG1(int i){
	CmdPerformUpgrade(MyNation,i);
};
void EnterIn(int i){
	if(i==0xFFFF)SubIcon=-1;
	WIcon* IC=NATIONS[MyNation].wIcons[i];
	if(!IC->SubList){
		SubIcon=-1;
		return;
	};
	SubIcon=i;
};
void OrderMove(int i,int x,int y){
	CmdSendToXY(MyNation,x,y);
};
void OrderAttack(int i,int x,int y){
	word MID=GetOwner((x<<5)+16,(y<<5)+16);
	if(MID!=0xFFFF){
		OneObject* OB=Group[MID];
		byte Msk=1<<MyNation;
		if(OB->NMask&Msk)CmdAttackToXY(MyNation,x,y);
		else CmdAttackObj(MyNation,MID);
	}else CmdAttackToXY(MyNation,x,y);
};
void OrderPatrol(int i,int x,int y){
	CmdPatrol(MyNation,x,y);
};
void CmdSetRprState(byte NI,byte State);
void OrderRepair(int i,int x,int y){
	CmdSetRprState(MyNation,1);
};
void OrderNucAtt(int i,int x,int y){
	CmdNucAtt(MyNation,x,y);
};
void OrderSetDst(int i,int x,int y){
	CmdSetDst(MyNation,x,y);
};
void OrderGetResource(int i,int x,int y){
	byte xx=x;
	byte yy=y;
	bool gl=FindResource(&xx,&yy,32,1,TZones[yy][xx]);
	int xg=xx;
	int yg=yy;
	xx=x;
	yy=y;
	bool wd=FindResource(&xx,&yy,32,2,TZones[yy][xx]);
	int xw=xx;
	int yw=yy;
	byte res=0;
	if(gl&&wd){
		if(abs(xg-x)+abs(yg-y)>abs(xw-x)+abs(yw-y)){
			res=2;
		}else res=1;
	};
	if(gl&&!wd)res=1;
	if((!gl)&&wd)res=2;
	if(res)CmdTakeRes(MyNation,x,y,res);
};
void OrderUnload(int i,int x,int y){
	CmdUnload(MyNation,x,y);
};
void OrderAttPt(int i,int x,int y){
	if(Links[y][x]==0xFFFF)
	CmdContinueAttackPoint(MyNation,x,y);
	else CmdContinueAttackWall(MyNation,x,y);
};
word MaxMagic;
void UNIPARAM(int i){
	curptr=2;
	curdx=16;
	curdy=15;
	GetCoord=true;
	UNIM=NULL;
	switch(i){
	case 1://Move to xy
		UNIM=&OrderMove;
		break;
	case 2://Attack near (x,y)
		UNIM=&OrderAttack;
		break;
	case 5://Patrol
		UNIM=&OrderPatrol;
		break;
	case 6://Repair
		UNIM=&OrderRepair;
		break;
	case 7:
		UNIM=&OrderGetResource;
		break;
	case 8:
		UNIM=&OrderUnload;
		break;
	case 9:
		UNIM=&OrderAttPt;
		break;
	case 10:
		UNIM=&OrderNucAtt;
		break;
	case 11:
		UNIM=&OrderSetDst;
		break;
	};
};
void NOPARAM(int i){
	UNIM=NULL;
	switch(i){
	case 3://Stop
		CmdStop(MyNation);
		break;
	case 4://Stand Ground
		CmdStandGround(MyNation);
		break;
	case 5:
		CmdSetRprState(MyNation,1);
		break;
	case 6:
		CmdSetRprState(MyNation,0);
		break;
	case 7://Build wall
		SetBuildWallMode();
		break;
	};
};
void OrderPointN(int i,int x,int y){
	word MID=GetOwner((x<<5)+16,(y<<5)+16);
	if(MID!=0xFFFF){
		OneObject* OB=Group[MID];
		if(OB->NNUM!=MyNation)CmdComplexAttack(MyNation,MID,i);
		else CmdComplexAttackPoint(MyNation,x,y,i);
	}else 
	  CmdComplexAttackPoint(MyNation,x,y,i);
};
void OrderPoint0(int i,int x,int y){
	OrderPointN(0,x,y);
};
void OrderPoint1(int i,int x,int y){
	OrderPointN(1,x,y);
};
void OrderPoint2(int i,int x,int y){
	OrderPointN(2,x,y);
};
void OrderPoint3(int i,int x,int y){
	OrderPointN(3,x,y);
};
void OrderPoint4(int i,int x,int y){
	OrderPointN(4,x,y);
};
void OrderPoint5(int i,int x,int y){
	OrderPointN(5,x,y);
};
void OrderPoint6(int i,int x,int y){
	OrderPointN(6,x,y);
};
void OrderPoint7(int i,int x,int y){
	OrderPointN(7,x,y);
};
void OrderPoint8(int i,int x,int y){
	OrderPointN(8,x,y);
};
void OrderPoint9(int i,int x,int y){
	OrderPointN(9,x,y);
};
void OrderPoint10(int i,int x,int y){
	OrderPointN(10,x,y);
};
void OrderPoint11(int i,int x,int y){
	OrderPointN(11,x,y);
};
void STRANGEWEAPON(int i){
	if(MaxMagic<NATIONS[MyNation].WMagic[i])return;
	curptr=3;
	curdx=16;
	curdy=16;
	GetCoord=true;
	UNIM=NULL;

	switch(i){
	case 0:
		UNIM=&OrderPoint0;
		break;
	case 1:
		UNIM=&OrderPoint1;
		break;
	case 2:
		UNIM=&OrderPoint2;
		break;
	case 3:
		UNIM=&OrderPoint3;
		break;
	case 4:
		UNIM=&OrderPoint4;
		break;
	case 5:
		UNIM=&OrderPoint5;
		break;
	case 6:
		UNIM=&OrderPoint6;
		break;
	case 7:
		UNIM=&OrderPoint7;
		break;
	case 9:
		UNIM=&OrderPoint9;
		break;
	case 10:
		UNIM=&OrderPoint10;
		break;
	};
};
int ShowUniqAbility(){
	char sxt[128];
	if(MList[0].Last>8192)return false;
	GeneralObject* GO=MList[0].RF;
	OneObject* OB=Group[MList[0].Last];
	Nation* NT=OB->Nat;
	if(NT!=LastNT||MList[0].Last!=LastID||GO!=LastGO)SubIcon=-1;
	LastNT=NT;
	LastID=MList[0].Last;
	MaxMagic=MList[0].MaxMag;
	LastGO=GO;
	int NABL;
	WIcon* SIC;
	if(SubIcon==-1)NABL=GO->NIcons;
	else{
		SIC=NT->wIcons[SubIcon];
		NABL=SIC->Param1;
		if(!SIC->SubList){
			SubIcon=-1;
			NABL=GO->NIcons;
		};
	};
	if(!NABL)return 0;
	int AN=AblNx*AblNy;
	if(NABL>AN)NABL=AN;
	int xx=0;
	int x1=AblX;
	int y1=AblY;
	OneSlide* OC;
	WIcon* MI;
	int IREF;
	char str[32];
	int p,q,spr,fid;
	Visuals* VS;
	for(int i=0;i<NABL;i++){
		bool DrawIt=true;
		if(SubIcon==-1){
			MI=NT->wIcons[GO->IRefs[i]];
			IREF=GO->IRefs[i];
		}else{
			MI=NT->wIcons[SIC->SubList[i]];
			IREF=SIC->SubList[i];
		};
		switch(MI->Kind){
		case 0:
			//directory entry
			ShowRLCItem(x1,y1,&MImage[MI->FileID],MI->Spr,0);
			Xbar(x1,y1,IconLx-2,IconLy-2,0x1B);
			CreateZone(x1,y1,IconLx,IconLy,&EnterIn,IREF,MI->Message);
			break;
		case 1://uniq without param
			ShowRLCItem(x1,y1,&MImage[MI->FileID],MI->Spr,MyNation);
			Xbar(x1,y1,IconLx-2,IconLy-2,0x1B);
			CreateZone(x1,y1,IconLx,IconLy,&NOPARAM,MI->Param2,MI->Message);
			break;
		case 2://uniq with param
			ShowRLCItem(x1,y1,&MImage[MI->FileID],MI->Spr,MyNation);
			Xbar(x1,y1,IconLx-2,IconLy-2,0x1B);
			CreateZone(x1,y1,IconLx,IconLy,&UNIPARAM,MI->Param2,MI->Message);
			break;
		case 3://perform upgrade
			{
				SimpleUTP* SU=NT->UPG.utp[MI->Param2];
				if(SU->Enabled){
					ShowRLCItem(x1,y1,&MImage[SU->IFileID],SU->IFIndex,MyNation);
					Xbar(x1,y1,IconLx-2,IconLy-2,240);
					Xbar(x1+1,y1+1,IconLx-3,IconLy-3,240);
					/*if(SU->Stage){
						int HL=div(int(SU->Stage)*(IconLx-4),SU->Time).quot;
						Hline(x1+1,y1+IconLy-6,x1+IconLx-4,240);
						Hline(x1+1,y1+IconLy-5,x1+HL,240);
						Hline(x1+1,y1+IconLy-4,x1+HL,240);
					};*/
					strcpy(sxt,SU->Message);
					strcat(sxt,"(");
					NT->GetUpgradeCostString(sxt,MI->Param2);
					strcat(sxt,")");
					CreateZone(x1,y1,IconLx,IconLy,&DoUPG1,MI->Param2,sxt);
				}else DrawIt=false;
			};
			break;
		case 4://produce object
			GO=NT->Mon[MI->Param2];
			if(GO->Enabled){
				if(!MI->FileID){
					p=GO->NAnm;
					for(q=0;q<p;q++){
						if(GO->lpFAnim[q].WhatFor==8)break;
					};
					if(q>=p)q=p-1;
					OC=GO->lpFAnim[q].Anm->Movie;
					fid=OC->FileID;
					spr=OC->spr;
				}else{
					fid=MI->FileID;
					spr=MI->Spr;
				};
				ShowRLCItem(x1,y1,&MImage[fid],spr,MyNation);
				Xbar(x1,y1,IconLx-2,IconLy-2,15);
				//int GA=GetAmount(MI->OInd);
				//int GP=GetProgress(MI->OInd);
				//if(GA!=0){
				//	sprintf(str,"%d",GA);
				//	ShowString(x1+2,y1+2,str,&rlf1_s);
				//};
				//int HL=div(GP*(IconLx-4),200).quot;
				//Hline(x1+1,y1+IconLy-6,x1+IconLx-4,15);
				//Hline(x1+1,y1+IconLy-5,x1+HL,255);
				//Hline(x1+1,y1+IconLy-4,x1+HL,255);
				VS=(Visuals*)GO;
				GetChar(GO,sxt);
				GO->GetMonsterCostString(sxt);
				CreateZone(x1,y1,IconLx,IconLy,&HPR1,MI->Param2,sxt);
			}else DrawIt=false;
			break;
		case 5://strange weapon
			if(NT->SWP[MI->Param1].Enabled){
				ShowRLCItem(x1,y1,&MImage[MI->FileID],MI->Spr,MyNation);
				CreateZone(x1,y1,IconLx,IconLy,&STRANGEWEAPON,MI->Param2,MI->Message);
			}else DrawIt=false;
			break;
		};
		if(DrawIt){
			xx++;
			if(xx>=AblNx){
				xx=0;
				x1=PrpX;
				y1+=IconLy;
			}else x1+=IconLx;
		};
	};
	if(GO->ExtMenu)return NABL;
	return -1;
};
bool ShowCommonAbl(){
	bool isAir=false;
	bool isWater=false;
	bool isLand=false;
	for(int i=0;i<NINF;i++){
		GeneralObject* GO=MList[i].RF;
		if(GO->Kind==2&&!(GO->OnAir||GO->OnWater))isLand=true;
		if(GO->Kind==2&&GO->OnWater)isWater=true;
		if(GO->Kind==5)isAir=true;
	};
	char sxt[128];
	if(!(isAir||isLand||isWater))return false;
	Nation* NT=&NATIONS[MyNation];
	word NIcons;
	word* Icons=NULL;
	if(isAir&&!(isLand||isWater)){
		NIcons=NT->NAmenus;
		Icons=NT->Amenus;
	};
	if(isLand&&!(isAir||isWater)){
		NIcons=NT->NLmenus;
		Icons=NT->Lmenus;
	};
	if(isWater&&!(isAir||isLand)){
		NIcons=NT->NWmenus;
		Icons=NT->Wmenus;
	};
	if(!Icons){
		NIcons=NT->NCmenus;
		Icons=NT->Cmenus;
	};
	
	if(NT!=LastNT||LastID!=7777)SubIcon=-1;
	LastID=7777;
	LastNT=NT;
	int NABL;
	WIcon* SIC;
	if(SubIcon==-1)NABL=NIcons;
	else{
		SIC=NT->wIcons[SubIcon];
		NABL=SIC->Param1;
		if(!SIC->SubList){
			SubIcon=-1;
			NABL=NIcons;
		};
	};
	int AN=AblNx*AblNy;
	if(NABL>AN)NABL=AN;
	int xx=0;
	int x1=AblX;
	int y1=AblY;
	OneSlide* OC;
	WIcon* MI;
	int IREF;
	char str[32];
	int p,q,spr,fid;
	Visuals* VS;
	MaxMagic=0;
	for(i=0;i<NABL;i++){
		if(SubIcon==-1){
			MI=NT->wIcons[Icons[i]];
			IREF=Icons[i];
		}else{
			MI=NT->wIcons[SIC->SubList[i]];
			IREF=SIC->SubList[i];
		};
		switch(MI->Kind){
		case 0:
			//directory entry
			ShowRLCItem(x1,y1,&MImage[MI->FileID],MI->Spr,0);
			Xbar(x1,y1,IconLx-2,IconLy-2,0x1B);
			CreateZone(x1,y1,IconLx,IconLy,&EnterIn,IREF,MI->Message);
			break;
		case 1://uniq without param
			ShowRLCItem(x1,y1,&MImage[MI->FileID],MI->Spr,MyNation);
			Xbar(x1,y1,IconLx-2,IconLy-2,0x1B);
			CreateZone(x1,y1,IconLx,IconLy,&NOPARAM,IREF,MI->Message);
			break;
		case 2://uniq with param
			ShowRLCItem(x1,y1,&MImage[MI->FileID],MI->Spr,MyNation);
			Xbar(x1,y1,IconLx-2,IconLy-2,0x1B);
			CreateZone(x1,y1,IconLx,IconLy,&UNIPARAM,MI->Param2,MI->Message);
			break;
		case 3://perform upgrade
			/*{
				SimpleUTP* SU=NT->UPG.utp[MI->Param2];
				ShowRLCItem(x1,y1,&MImage[SU->IFileID],SU->IFIndex,0);
				Xbar(x1,y1,IconLx-2,IconLy-2,240);
				Xbar(x1+1,y1+1,IconLx-3,IconLy-3,240);
				strcpy(sxt,SU->Message);
				strcat(sxt,"(");
				NT->GetUpgradeCostString(sxt,MI->Param2);
				strcat(sxt,")");
				CreateZone(x1,y1,IconLx,IconLy,&DoUPG1,MI->Param2,sxt);
			};*/
			break;
		case 4://produce object
			/*GO=NT->Mon[MI->Param2];
			if(!MI->FileID){
				p=GO->NAnm;
				for(q=0;q<p;q++){
					if(GO->lpFAnim[q].WhatFor==8)break;
				};
				if(q>=p)q=p-1;
				OC=GO->lpFAnim[q].Anm->Movie;
				fid=OC->FileID;
				spr=OC->spr;
			}else{
				fid=MI->FileID;
				spr=MI->Spr;
			};
			ShowRLCItem(x1,y1,&MImage[fid],spr,0);
			Xbar(x1,y1,IconLx-2,IconLy-2,15);
			//int GA=GetAmount(MI->OInd);
			//int GP=GetProgress(MI->OInd);
			//if(GA!=0){
			//	sprintf(str,"%d",GA);
			//	ShowString(x1+2,y1+2,str,&rlf1_s);
			//};
			//int HL=div(GP*(IconLx-4),200).quot;
			//Hline(x1+1,y1+IconLy-6,x1+IconLx-4,15);
			//Hline(x1+1,y1+IconLy-5,x1+HL,255);
			//Hline(x1+1,y1+IconLy-4,x1+HL,255);
			VS=(Visuals*)GO;
			GetChar(GO,sxt);
			GO->GetMonsterCostString(sxt);
			CreateZone(x1,y1,IconLx,IconLy,&HPR1,MI->Param2,sxt);
			*/
			break;
		case 5://strange weapon
			ShowRLCItem(x1,y1,&MImage[MI->FileID],MI->Spr,MyNation);
			Xbar(x1,y1,IconLx-2,IconLy-2,0x1C);
			CreateZone(x1,y1,IconLx,IconLy,&STRANGEWEAPON,MI->Param2,MI->Message);
			break;
		};
		xx++;
		if(xx>=AblNx){
			xx=0;
			x1=PrpX;
			y1+=IconLy;
		}else x1+=IconLx;
	};
	return true;

};
void ShowAbility(){
	char sxt[128];
	//InitZones();
	NABL=0;
	word MID;
	OneObject* OBJ;
	GeneralObject* GO;
	AblInf* MI;
	Nation* NT;
	word s;
	int i,j,k,p,q,r;
	//if(NINF)ResFon(AblX,AblY,AblNx*IconLx,AblNy*IconLy);
	if(!NINF){
		//ResFon(AblX,AblY,AblNx*IconLx,AblNy*IconLy);
		//ScrCopy(PrpX,PrpY,PrpNx*IconLx,PrpNy*IconLy);
		return;
	};
	int nability=0;
	if(NINF==1){
		nability=ShowUniqAbility();
		if(nability==-1)return;
	};
	if(ShowCommonAbl())return;
	//calculating monsters could be produced
	for(i=0;i<NINF;i++){
		OBJ=Group[MList[i].Last];
		if(OBJ->Ready){
			NT=OBJ->Nat;
			k=OBJ->Index;
			p=OBJ->NIndex;
			q=NT->PACount[p];
			for(r=0;r<q;r++){
				s=NT->PAble[p][r];	
				if(NT->Mon[s]->Enabled){
					for(j=0;j<NABL;j++){
						if(AList[j].OInd==s)break;
					};
					AList[j].OInd=s;
					AList[j].NT=NT;
					AList[j].Kind=0;
					if(j>=NABL)NABL=j+1;
				};
			};
		};
	};
	//calculate upgrades
	for(i=0;i<NINF;i++){
		OBJ=Group[MList[i].Last];
		if(OBJ->Ready){
			NT=OBJ->Nat;
			k=OBJ->Index;
			p=OBJ->NIndex;
			GeneralObject* GO=NT->Mon[p];
			q=GO->NUpgrades;
			for(r=0;r<q;r++){
				s=GO->Upg[r];	
				if(NT->UPG.utp[s]->Enabled){
					for(j=0;j<NABL;j++){
						if(AList[j].Kind==1&&AList[j].OInd==s)break;
					};
					AList[j].OInd=s;
					AList[j].NT=NT;
					AList[j].Kind=1;
					if(j>=NABL)NABL=j+1;
				};
			};
		};
	};
	//show abilities
	int AN=AblNx*AblNy;
	if(NABL>AN)NABL=AN;
	int xx=nability;
	int x1=AblX+IconLx*(xx&3);
	int y1=AblY+IconLy*(xx>>2);
	xx=xx&3;
	OneSlide* OC;
	char str[32];
	for(i=0;i<NABL;i++){
		MI=&(AList[i]);
		switch(MI->Kind){
		case 0:{
			GO=(MI->NT)->Mon[MI->OInd];
			p=GO->NAnm;
				for(q=0;q<p;q++){
				if(GO->lpFAnim[q].WhatFor==8)break;
			};
			if(q>=p)q=p-1;
			OC=GO->lpFAnim[q].Anm->Movie;
			ShowRLCItem(x1,y1,&MImage[OC->FileID],OC->spr,MyNation);
			Xbar(x1,y1,IconLx-2,IconLy-2,15);
			int GA=GetAmount(MI->OInd);
			int GP=GetProgress(MI->OInd);
			if(GA!=0){
				sprintf(str,"%d",GA);
				ShowString(x1+2,y1+2,str,&rlf1_s);
			};
			int HL=div(GP*(IconLx-4),GO->NStages<<1).quot;
			Hline(x1+1,y1+IconLy-6,x1+IconLx-4,15);
			Hline(x1+1,y1+IconLy-5,x1+HL,255);
			Hline(x1+1,y1+IconLy-4,x1+HL,255);
			Visuals* VS=(Visuals*)GO;
			GetChar(GO,sxt);
			GO->GetMonsterCostString(sxt);
			CreateRZone(x1,y1,IconLx,IconLy,&HPR,&RHPR,i,sxt);};
			break;
		case 1://upgrade caps
			{
				SimpleUTP* SU=MI->NT->UPG.utp[MI->OInd];
				ShowRLCItem(x1,y1,&MImage[SU->IFileID],SU->IFIndex,MyNation);
				Xbar(x1,y1,IconLx-2,IconLy-2,240);
				Xbar(x1+1,y1+1,IconLx-3,IconLy-3,240);
				if(SU->Stage){
					int HL=div(int(SU->Stage)*(IconLx-4),SU->Time).quot;
					Hline(x1+1,y1+IconLy-6,x1+IconLx-4,240);
					Hline(x1+1,y1+IconLy-5,x1+HL,240);
					Hline(x1+1,y1+IconLy-4,x1+HL,240);
				};
				strcpy(sxt,SU->Message);
				strcat(sxt,"(");
				MI->NT->GetUpgradeCostString(sxt,MI->OInd);
				strcat(sxt,")");
				CreateZone(x1,y1,IconLx,IconLy,&DoUPG,i,sxt);
			};
			break;
		};
		xx++;
		if(xx>=AblNx){
			xx=0;
			x1=PrpX;
			y1+=IconLy;
		}else x1+=IconLx;
	};
};
void PropCopy(){
	int AY=NABL>>2;
	if(NABL&3)AY++;
	int PY=NINF>>2;
	if(NINF&3)PY++;
	if(AY>8)AY=0;
	if(PY>8)PY=0;
	AY=4;
	PY=2;
	if(PY)ScrCopy(PrpX,PrpY,PrpNx*IconLx,PY*IconLy);
	if(AY)ScrCopy(AblX,AblY,AblNx*IconLx,AY*IconLy);
};
