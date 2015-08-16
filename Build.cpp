#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mgraph.h"
#include "mouse.h"
#include "menu.h"
#include "MapDiscr.h"
#include "Nature.h"
#include "Megapolis.h"
#include "GSound.h"
bool CheapMode;
void PerformAction(OneObject* OB,int x,int y);
extern const int drr[9]={7,6,5,0,0,4,1,2,3};
extern word LastObject;
void AssignHint1(char* s,int time);
bool Nation::CheckBuilding(word ID,byte x,byte y){
	if(!Mon[ID]->cpbBuilding)return false;
	int	sx=Mon[ID]->SizeX;
	if(Mon[ID]->OnCost){
		int IsCost=false;
		for(int dx=0;dx<sx;dx++)
			for(int dy=0;dy<sx;dy++){
				int ti=tmap[x+dx-1][y+dy-1];
				if(ti<29||ti>50)ti=0;
				if(ti)IsCost++;
				if((Mops[y+dy][x+dx]!=0xFFFF)||(WLock[y+dy][x+dx]&&!ti))return false;
			};
			if(IsCost<3)return false;else return true;
	}else
		if(Mon[ID]->OnOil){
			byte xp=x+1;
			byte yp=y+1;
			if((xp&1)||(yp&1))return false;
			if(OILMAP[yp>>1][xp>>1]&&!WLock[yp][xp])
				return true;
			return false;
		}else
			for(int dx=0;dx<sx;dx++)
				for(int dy=0;dy<sx;dy++)
					if(LLock[y+dy][x+dx])return false;
	return true;
};
int Nation::CreateBuilding(word ID,byte x,byte y){
	GeneralObject* GO=Mon[ID];
	byte NIN=NNUM;
	if(!CheapMode){
		if(!CheckCost(NIN,GO->ResourceID[0],GO->ResAmount[0]))return -1;
		if(!CheckCost(NIN,GO->ResourceID[1],GO->ResAmount[1]))return -1;
		if(!CheckCost(NIN,GO->ResourceID[2],GO->ResAmount[2]))return -1;
		if(!CheckCost(NIN,GO->ResourceID[3],GO->ResAmount[3]))return -1;
		if(GO->ResourceID[0]!=255)RESRC[NNUM][GO->ResourceID[0]]-=GO->ResAmount[0];
		if(GO->ResourceID[1]!=255)RESRC[NNUM][GO->ResourceID[1]]-=GO->ResAmount[1];
		if(GO->ResourceID[2]!=255)RESRC[NNUM][GO->ResourceID[2]]-=GO->ResAmount[2];
		if(GO->ResourceID[3]!=255)RESRC[NNUM][GO->ResourceID[3]]-=GO->ResAmount[3];
	};
	if(!CheckBuilding(ID,x,y))return -1;
	byte sx=Mon[ID]->SizeX;
	for(int i=0;i<MaxObj&&int(Group[i]);i++);
	if(i>MaxObj)return -1;
	if(ID>=NMon) return -1;
	Group[i]=new OneObject;
	OneObject* G=Group[i];
	if(i>=MAXOBJECT)MAXOBJECT=i+1;
	//if(GO->cpbFarm)NFarms++;
	G->DefaultSettings(GO);
	G->Teleport=false;
	G->capTeleport=GO->Teleport;
	G->NNUM=NNUM;
	G->Media=0;
	G->CrowdRef=NULL;
	G->Kind=GO->Kind;
	Cell8x8* CELL=&TCInf[NNUM][y>>2][x>>2];
	CELL->UnitsAmount[GO->Kind]++;
	G->VisRadius=GO->VisRadius;
	G->VisSpots=GO->VisSpots;
	G->SpotType=GO->SpotType;
	G->SpotSize=GO->SpotSize;
	G->OnWater=GO->OnWater;
	G->DangerZone=GO->DangerZone;
	G->NoSearchVictim=GO->NoSearchVictim;
	G->NoAnswer=GO->NoAnswer;
	G->NeedNoHelp=GO->NeedNoHelp;
	G->Nat=this;
	G->Ready=false;
	G->wepX=GO->wepX;
	G->wepY=GO->wepY;
	G->MaxDelay=GO->delay;
	G->delay=0;
	G->Lx=GO->SizeX;
	G->Ly=GO->SizeY;
	G->NIndex=ID;
	G->GroupIndex=NULL;
	G->TempFlag=false;
	G->Mobilised=false;
	G->Wars=NULL;
	G->Index=i;
	Visuals* m;
	m=(Visuals*)Mon[ID];
	G->Selected=false;
	G->Borg=false;
	G->Life=2;
	G->MaxLife=m->info.Basic.MaxLife;
	G->Ref.Visual=m;
	G->x=x;
	G->y=y;
	G->Push=false;
	G->Direction=0;
	memset(&(G->ARegs),0,sizeof G->ARegs);
	G->LoadAnimation(0,6,0);
	G->LoadCurAnm(0);
	G->LocalOrder=NULL;
	//G->OrderReport=NULL;
	//G->MessageFlags=0;
	G->PrioryLevel=0;
	//G->MessageKind=0;
	G->InLineCom=NULL;
	G->LineOffset=0;
	G->Ticks=0;
	G->TicksPerChange=255;
	G->Wait=0;
	G->Addx=0;
	G->Addy=0;
	G->Npush=0;
	G->StandTime=100;
	G->Sdoxlo=false;
	G->Weap=Mon[ID]->Weap;
	G->NMask=NMask;
	G->Important=false;
	G->EnemyDist=5000;
	G->Attack=false;
	G->EnemyID=0xFFFF;
	G->Egoist=false;
	G->NStages=GO->NStages;
	G->Stage=0;
	G->NextStage();
	G->capBuilding=true;
	G->AnmGoKind=1;
	G->AnmStandKind=0;
	for(byte dx=0;dx<sx;dx++)
		for(byte dy=0;dy<sx;dy++){
			LLock[y+dy][x+dx]=1;
			IncLock(x+dx,y+dy);
			TrMap[y+dy][x+dx]=1;
			WMap[y+dy][x+dx]=1;
			WLock[y+dy][x+dx]=1;
			Mops[y+dy][x+dx]=i;
		};
	return i;
};
void OneObject::NextStage(){
	if(!capBuilding||Life==MaxLife)return;
	if(Stage<NStages){
		Stage++;
		LoadCurAnm(0);
		TicksPerChange=255;
		Life=div(Stage*MaxLife,NStages).quot;
		CurrentSprite=div(Stage*(CurAnm->count-1),NStages).quot;
		if(Stage==NStages){
			if((!Ready)&&Ref.General->cpbFarm)Nat->NFarms++;
			Ready=true;
		};
	}else{
		Life+=20;
		LoadCurAnm(0);
		TicksPerChange=255;
		if(Life>MaxLife)Life=MaxLife;
		CurrentSprite=CurAnm->count-1;
		Ready=true;
	};
};
void BuildObjLink(OneObject* OBJ);
bool OneObject::BuildObj(word OID,int Prio){
	if(OnWater){
		return BuildOnWater(OID,Prio);
	};
	if(Prio<PrioryLevel&&Prio<100)return false;
	if(!capBuild)return false;
	if(capBuilding)return false;
	OneObject* OB=Group[OID];
	if(!int(OB)||OB->Sdoxlo)return false;
	if(!OB->capBuilding)return false ;
	if(!(OB->NMask&NMask))return false;
	if(OB->Life==MaxLife)return false;
	byte x1=OB->x;
	byte y1=OB->y;
	int sx=x1-x;
	int	sy=y1-y;
	int dx=abs(sx);
	int dy=abs(sy);
	int dst;
	if(dx>dy)dst=dx;
			else dst=dy;
	Important=true;
	Order1* Or1=GetOrdBlock();
	if(!int(Or1))return false;
	Or1->PrioryLevel=Prio&127;
	Or1->NextOrder=LocalOrder;
	Or1->OrderType=89;//Build
	Or1->OrderTime=0;
	Or1->DoLink=&BuildObjLink;
	Or1->info.MoveToObj.ObjIndex=OID;
	Order1* LOR=LocalOrder;
	if(int(InLineCom))FreeAsmLink();
	LocalOrder=Or1;
	//OrderReport=NULL;
	//MessageKind=0;
	//Sender=0xFFFF;
	PrioryLevel=Prio&127;
	return true;
};	
void BuildObjLink(OneObject* OBJ){
	word OID=OBJ->LocalOrder->info.MoveToObj.ObjIndex;
	OneObject* OB=Group[OID];
	//byte NI=OBJ->NNUM;
	//if(OB->cost>RESRC[NI][1])return;
	OBJ->Important=true;
	Order1* Or1;
	if((!int(OB))||OB->Sdoxlo||OB->Life==OB->MaxLife){
		if(int(OBJ->InLineCom))OBJ->FreeAsmLink();
		if(int(OBJ->LocalOrder)){
		//	RESRC[NI][1]-=OB->cost;
			Or1=OBJ->LocalOrder->NextOrder;
			OBJ->FreeOrdBlock(OBJ->LocalOrder);
			OBJ->LocalOrder=Or1;
			OBJ->Important=false;
			return;
		};
	};
	TrMap[OBJ->y][OBJ->x]=0;
	byte x0=OB->x;
	byte y0=OB->y;
	byte olx=OB->Lx;
	byte oly=OB->Ly;
	byte xo=OBJ->x;
	byte yo=OBJ->y;
	if(xo<x0)xo=x0;else if(xo>=x0+olx)xo=x0+olx-1;
	if(yo<y0)yo=y0;else if(yo>=y0+oly)yo=y0+oly-1;
	int sx=xo-OBJ->x;
	int	sy=yo-OBJ->y;
	int dx=abs(sx);
	int dy=abs(sy);
	int dst;
	if(dx>dy)dst=dx;
			else dst=dy;
	if(dst<2){
		OBJ->Direction=drr[(sx+1)*3+sy+1];
		OBJ->LoadAnimation(2,2,0);
		AddWorkEffect(OBJ->x,OBJ->y,OBJ->Ref.General->TreeSound);
		OBJ->LoadCurAnm(2);
		OB->NextStage();
		TrMap[OBJ->y][OBJ->x]=1;
		OBJ->isMoving=false;
	}else{
		byte x0=OB->x-1;
		byte y0=OB->y-1;
		byte olx=OB->Lx+2;
		byte oly=OB->Ly+2;
		byte xo=OBJ->x;
		byte yo=OBJ->y;
		if(xo<x0)xo=x0;else if(xo>=x0+olx)xo=x0+olx-1;
		if(yo<y0)yo=y0;else if(yo>=y0+oly)yo=y0+oly-1;
		if(TrMap[yo][xo]){//search for nearest empty point
			int dst=1000; 
			int dst1;
			olx--;
			oly--;
			for(byte pp=0;pp<olx;pp++){
				if(!TrMap[y0+pp][x0]){
					dst1=OBJ->DistTo(x0,y0+pp);
					if(dst1<dst){
						xo=x0;
						yo=y0+pp;
						dst=dst1;
					};
				};
			};
			for(pp=0;pp<olx;pp++){
				if(!TrMap[y0+pp+1][x0+olx]){
					dst1=OBJ->DistTo(x0+olx,y0+pp+1);
					if(dst1<dst){
						xo=x0+olx;
						yo=y0+pp+1;
						dst=dst1;
					};
				};
			};
			for(pp=0;pp<olx;pp++){
				if(!TrMap[y0+olx][x0+pp]){
					dst1=OBJ->DistTo(x0+pp,y0+olx);
					if(dst1<dst){
						xo=x0+pp;
						yo=y0+olx;
						dst=dst1;
					};
				};
			};
			for(pp=0;pp<olx;pp++){
				if(!TrMap[y0][x0+pp+1]){
					dst1=OBJ->DistTo(x0+pp+1,y0);
					if(dst1<dst){
						xo=x0+pp+1;
						yo=y0;
						dst=dst1;
					};
				};
			};
		};
		OBJ->FreeAsmLink();
		OBJ->CreatePath(xo,yo);
	};
};
//return 0-unable
//       n-busy(n=number of tasks)
//		 -1-ready
int OneObject::CheckAbility(word ID){
	if(!capBuilding)return 0;
	Nation* NT=Nat;
	word NInd=NIndex;
	int p=NT->PACount[NInd];
	for(int i=0;i<p;i++){
		if(NT->PAble[NInd][i]==ID)break;
	};
	if(i>=p)return 0;
	int j=0;
	Order1* Ord1=LocalOrder;
	while(int(Ord1)){
		if(Ord1->OrderType==13)j++;
		Ord1=Ord1->NextOrder;
	};
	if(j==0)return -1;
	return j;	
};
void ProduceObjLink(OneObject* OBJ);
void OneObject::Produce(word ID){
	if(!(capBuilding&&Ready))return;
	if(int(Nat->NFarms)*15<Nat->NGidot+1){
		if(NNUM==MyNation){
			AssignHint1("Недостаточно ферм.",30);
		};
		return;
	};
	byte NI=Nat->NNUM;
	int xcost=Nat->Mon[ID]->cost;
	if(RESRC[NI][1]<xcost){
		AssignHint1("Недостаточно золота.",30);
		return;
	};
	RESRC[NI][1]-=xcost;
	Order1* Or1=GetOrdBlock();
	Or1->PrioryLevel=0;
	Or1->NextOrder=LocalOrder;
	Or1->OrderType=13;//Produce
	Or1->OrderTime=0;
	Or1->DoLink=&ProduceObjLink;
	Or1->info.Produce.ObjIndex=ID;
	Order1* LOR=LocalOrder;
	if(int(InLineCom))FreeAsmLink();
	LocalOrder=Or1;
	LocalOrder->info.Produce.PStep=1;
	LocalOrder->info.Produce.Progress=0;
	LocalOrder->info.Produce.NStages=Nat->Mon[ID]->NStages<<1;
	//OrderReport=NULL;
	//MessageKind=0;
	//Sender=0xFFFF;
};	
void ProduceObjLink(OneObject* OBJ){
	word OI=OBJ->LocalOrder->info.Produce.ObjIndex;
	Nation* NT=OBJ->Nat;
	if(int(NT->NFarms)*15<NT->NGidot+1){
		if(NT->NNUM==MyNation){
			sprintf(Prompt,"Недостаточно ферм.");
			PromptTime=100;
		};
		return;
	};
	byte NI=NT->NNUM;
	int xcost=NT->Mon[OI]->cost;
	//if(RESRC[NI][1]<xcost)return;
	byte step=OBJ->LocalOrder->info.Produce.PStep;
	word xxx=OBJ->LocalOrder->info.Produce.Progress;
	word nxxx=OBJ->LocalOrder->info.Produce.NStages;
//	byte NI=OBJ->NNUM;
//	if(OB->cost>RESRC[NI][1])return;
	xxx+=step;
	OBJ->LocalOrder->info.Produce.Progress=xxx;
	if(xxx<nxxx)return;
	//search for place   
	byte x0=OBJ->x-1;
	byte y0=OBJ->y-1;
	byte x1=OBJ->x+OBJ->Lx;
	byte y1=OBJ->y+OBJ->Ly;
	byte yy,xx;
	GeneralObject* GO=NT->Mon[OI];
	if(GO->OnWater){
		x0&=254;
		y0&=254;
		x1=(x1+1)&254;
		y1=(y1+1)&254;
		int uu=32;
		do{
			xx=x0;
			for(yy=y0;yy<=y1;yy+=2)
				if(!WLock[yy][xx])goto zzz;
			xx=x1;
			for(yy=y0;yy<=y1;yy+=2)
				if(!WLock[yy][xx])goto zzz;
			yy=y0;
			for(xx=x0+2;xx<x1;xx+=2)
				if(!WLock[yy][xx])goto zzz;
			yy=y1;
			for(xx=x0+2;xx<x1;xx+=2)
				if(!WLock[yy][xx])goto zzz;
			if(x0>2)x0-=2;
			if(y0>2)y0-=2;
			if(x1<msx-2)x1+=2;
			if(y1<msy-2)y1+=2;
			uu--;
		}while(uu);
	}else{
		do{
			xx=x0;
			for(yy=y0;yy<=y1;yy++)
				if(!LLock[yy][xx])goto zzz;
			xx=x1;
			for(yy=y0;yy<=y1;yy++)
				if(!LLock[yy][xx])goto zzz;
			yy=y0;
			for(xx=x0+1;xx<x1;xx++)
				if(!LLock[yy][xx])goto zzz;
			yy=y1;
			for(xx=x0+1;xx<x1;xx++)
				if(!LLock[yy][xx])goto zzz;
			if(x0>1)x0--;
			if(y0>1)y0--;
			if(x1<msx)x1++;
			if(y1<msy)y1++;
		}while(true);
	};
zzz:
	if(GO->OnWater){
		if(!WLock[yy][xx]){
			//RESRC[NI][1]-=xcost;
			OBJ->Nat->CreateTerrainAt(xx,yy,OBJ->LocalOrder->info.Produce.ObjIndex);
			if(OBJ->DstX&&LastObject!=0xFFFF)PerformAction(Group[LastObject],OBJ->DstX,OBJ->DstY);
		};
	}else
	if(!LLock[yy][xx]){
		//RESRC[NI][1]-=xcost;
		OBJ->Nat->CreateTerrainAt(xx,yy,OBJ->LocalOrder->info.Produce.ObjIndex);
		if(OBJ->DstX&&LastObject!=0xFFFF)PerformAction(Group[LastObject],OBJ->DstX,OBJ->DstY);
	};
	if(int(OBJ->InLineCom))OBJ->FreeAsmLink();
	if(int(OBJ->LocalOrder)){
		Order1* Or1=OBJ->LocalOrder->NextOrder;
		OBJ->FreeOrdBlock(OBJ->LocalOrder);
		OBJ->LocalOrder=Or1;
		OBJ->Important=false;
	};
};
int GetAmount(word ID){
	word CID=0;
	word Nsel=NSL[MyNation];
	word* SMon=Selm[MyNation];
	for(int i=0;i<Nsel;i++){
		word MID=SMon[i];
		if(MID!=0xFFFF){
			OneObject* OBJ=Group[MID];
			if(int(OBJ)){
				Order1* OR1=OBJ->LocalOrder;
				while(int(OR1)){
					if(OR1->OrderType==13&&
						OR1->info.Produce.ObjIndex==ID)CID++;
					OR1=OR1->NextOrder;
				};
			};
		};
	};
	return CID;
};
int GetProgress(word ID){	
	word CID=0;
	word Nsel=NSL[MyNation];
	word* SMon=Selm[MyNation];
	for(int i=0;i<Nsel;i++){
		word MID=SMon[i];
		if(MID!=0xFFFF){
			OneObject* OBJ=Group[MID];
			if(int(OBJ)){
				Order1* OR1=OBJ->LocalOrder;
				if(int(OR1)&&OR1->OrderType==13&&
					OR1->info.Produce.ObjIndex==ID&&
					OR1->info.Produce.Progress>CID)
					CID=OR1->info.Produce.Progress;
			};
		};
	};
	return CID;
};