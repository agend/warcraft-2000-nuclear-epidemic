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
#include "Nucl.h"
#include "TopZones.h"
#include "Megapolis.h"
#include "FlyObj.h"
#include "assert.h"
#include "Mode.h"
#include "GSound.h"
extern byte Quality;
typedef  bool CHOBJ(OneObject* OB,int N);
int MyHeight;
extern word FlyMops[256][256];
void FlyCell::GetForces(int x,int y,int *Fx,int *Fy,word MD){
	if(!NFly){
		*Fx=0;
		*Fy=0;
		return;
	};
	word MID;
	int Frx=0;
	int Fry=0;
	for(int i=0;i<15;i++){
		MID=FlyList[i];
		if(MID!=0xFFFF&&MID!=MD){
			OneObject* OB=Group[MID];
			if(OB&&OB->Media==2){
				if(abs(int(OB->Height)-MyHeight)<8){
					int xx=OB->RealX;
					int yy=OB->RealY;
					while(xx==x&&yy==y){
						xx+=3;//int(rando()&7)-3;
						yy+=3;//int(rando()&7)-3;
					};
					int dist=abs(x-xx)+abs(y-yy);
					if(dist<(16*85)){
						//ottalkivanie
						if(dist){
							Frx+=div((x-xx)<<5,dist).quot;
							Fry+=div((y-yy)<<5,dist).quot;
						};
					};
				};
			};
		};
	};
	*Fx=Frx;
	*Fy=Fry;
	return;
};
FlyCell::FlyCell(){
	NFly=0;
	memset(FlyList,255,sizeof FlyList);
};
FlyCell FlyMap[64][64];
extern word LastObject;
bool Nation::CreateOnFly(int x,int y,int n){
	int cx=x>>2;
	int cy=y>>2;
	FlyCell* FC=&FlyMap[cy][cx];
	if(FC->NFly>=15)return false;
	int Rex=(x<<9)+256;
	int Rey=(y<<9)+256;
	for(int i=0;i<MaxObj&&int(Group[i]);i++);
	if(i>MaxObj)return false;
	if(n>=NMon) return false;
	if(i>=MAXOBJECT)MAXOBJECT=i+1;
	for(int k=0;FC->FlyList[k]!=0xFFFF;k++);
	FC->NFly++;
	FC->FlyList[k]=i;
	Group[i]=OBJECTS+i;
	LastObject=i;
	Cell8x8* CELL=&TCInf[NNUM][y>>2][x>>2];
	OneObject* G=Group[i];
	GeneralObject* GO=Mon[n];
	AddOrderEffect(x,y,GO->BornSound);
	CELL->UnitsAmount[GO->Kind]++;
	G->DefaultSettings(GO);
	G->Teleport=false;
	G->capTeleport=GO->Teleport;
	G->RealX=Rex;
	G->RealY=Rey;
	G->DestX=Rex;
	G->DestY=Rey;
	G->RealVx=0;
	G->RealVy=0;
	G->RealDir=rando()&255;
	G->Media=2;
	G->OnWater=false;
	G->Kind=GO->Kind;
	G->VisRadius=GO->VisRadius;
	G->VisSpots=GO->VisSpots;
	G->SpotType=GO->SpotType;
	G->SpotSize=GO->SpotSize;
	G->DangerZone=GO->DangerZone;
	G->NoSearchVictim=GO->NoSearchVictim;
	G->NoAnswer=GO->NoAnswer;
	G->NeedNoHelp=GO->NeedNoHelp;
	G->Nat=this;
	G->Ready=true;
	G->wepX=GO->wepX;
	G->wepY=GO->wepY;
	G->MaxDelay=GO->delay;
	G->delay=0;
	G->NearBase=0xFFFF;
	G->capBase=GO->cpbBase;
	G->RStage=0;
	G->RType=0;
	G->RAmount=0;
	G->NNUM=NNUM;
	G->NIndex=n;
	G->AnmStandKind=0;
	G->AnmGoKind=1;
	G->capBuild=GO->cpbBuild;
	G->GroupIndex=NULL;
	G->cpbMoving=GO->cpbMoving;
	if(!GO->SizeX)GO->SizeX=1;
	if(!GO->SizeY)GO->SizeY=1;
	G->Lx=GO->SizeX;
	G->Ly=GO->SizeY;
	G->TempFlag=false;
	G->Mobilised=false;
	G->Wars=NULL;
	G->Index=i;
	//Mops[y][x]=i;
	Visuals* m;
	m=(Visuals*)Mon[n];
	G->Selected=false;
	G->Borg=false;
	G->Life=m->info.Basic.MaxLife;
	G->MaxLife=m->info.Basic.MaxLife;
	G->Ref.Visual=m;
	G->x=x;
	G->y=y;
	G->CrowdRef=NULL;
	G->Push=false;
	//LLock[y][x]=true;
	//IncLock(x,y);
	G->Direction=rando() & 7;
	memset(&(G->ARegs),0,sizeof G->ARegs);
	G->LoadAnimation(0,0,0);
	G->LoadCurAnm(0);
	G->LocalOrder=NULL;
	//G->OrderReport=NULL;
	//G->MessageFlags=0;
	G->PrioryLevel=0;
	//G->MessageKind=0;
	G->InLineCom=NULL;
	G->LineOffset=0;
	G->Ticks=0;
	G->TicksPerChange=10;
	G->Wait=0;
	G->Addx=0;
	G->Addy=0;
//	G->inMotion=false;
	G->Npush=0;
	G->StandTime=100;
	G->Sdoxlo=false;
	G->Weap=Mon[n]->Weap;
	G->NMask=NMask;
	G->Important=false;
	G->EnemyDist=5000;
	G->Attack=false;
	G->EnemyID=0xFFFF;
	G->Egoist=false;
	G->Height=0;
	G->LoadAnimation(0,0,0);//stand=motion animation
	G->LoadAnimation(1,2,0);//attack
	G->LoadAnimation(2,3,0);//death
	G->BestDist=32*16*6;
	G->FlyAttackMode=false;
	NGidot++;
	if(GO->UFO)G->MakeMeUFO();
	return true;
};
void OneObject::MakeMeFly(word MagID){
	if(MagID==0xFFFF||Media==2)return;
	if(!Ref.General->CanFly)return;
	MagSrcID=MagID;
	int cx=x>>2;
	int cy=y>>2;
	ClearOrders();
	if(InLineCom)FreeAsmLink();
	FlyCell* FC=&FlyMap[cy][cx];
	if(FC->NFly>=15)return;
	//Deleting from the terra
	if(LLock[y][x])DecLock(x,y);
	LLock[y][x]=0;
	Mops[y][x]=0xFFFF;
	int Rex=(x<<9)+256;
	int Rey=(y<<9)+256;
	for(int k=0;FC->FlyList[k]!=0xFFFF;k++);
	FC->NFly++;
	FC->FlyList[k]=Index;
	//Cell8x8* CELL=&TCInf[NNUM][y>>2][x>>2];
	//OneObject* G=Group[i];
	//GeneralObject* GO=Mon[n];
	//CELL->UnitsAmount[GO->Kind]++;
	//G->DefaultSettings(GO);
	//G->Teleport=false;
	//G->capTeleport=GO->Teleport;
	RealX=Rex;
	RealY=Rey;
	DestX=Rex;
	DestY=Rey;
	RealVx=0;
	RealVy=-128;
	RealDir=((Direction<<5)+16)&255;
	Media=2;
	OnWater=false;
	Ready=true;
	delay=0;
	NearBase=0xFFFF;
	memset(&(ARegs),0,sizeof ARegs);
	LoadAnimation(0,0,0);
	LoadCurAnm(0);
	LocalOrder=NULL;
	PrioryLevel=0;
	InLineCom=NULL;
	LineOffset=0;
	Ticks=0;
	Addx=0;
	Addy=0;
	Npush=0;
	StandTime=100;
	Sdoxlo=false;
	EnemyDist=5000;
	Attack=false;
	EnemyID=0xFFFF;
	Egoist=false;
	Height=0;
	LoadAnimation(0,0,0);//stand=motion animation
	LoadAnimation(1,2,0);//attack
	LoadAnimation(2,3,0);//death
	BestDist=32*16*6;
	FlyAttackMode=false;
};
void MakeFog(int xx,int yy);
void OneObject::MakeMeSit(){
	if(!(Media==2&&Ref.General->CanFly))return;
	MakeFog((RealX>>2)+(rando()&255)-128,(RealY>>2)+(rando()&255)-128);
	MakeFog((RealX>>2)+(rando()&255)-128,(RealY>>2)+(rando()&255)-128);
	MakeFog((RealX>>2)+(rando()&255)-128,(RealY>>2)+(rando()&255)-128);
	//MakeFog((RealX>>2)+(rando()&255)-128,(RealY>>2)+(rando()&255)-128);
	//MakeFog((RealX>>2)+(rando()&255)-128,(RealY>>2)+(rando()&255)-128);
	MakeFog((RealX>>2)+(rando()&255)-128,(RealY>>2)+(rando()&255)-128);
	if(LLock[y][x]){
		Die();
		return;
	};
	ClearOrders();
	if(InLineCom)FreeAsmLink();
	int cx=x>>2;
	int cy=y>>2;
	FlyCell* FC=&FlyMap[cy][cx];
	for(int pp=0;pp<15;pp++){
		if(FC->FlyList[pp]==Index){
			FC->FlyList[pp]=0xFFFF;
			FC->NFly--;
		};
	};
	Media=0;
	FlyMops[y][x]=0xFFFF;
	Mops[y][x]=Index;
};
void OneObject::ProcessMotion(){
	int cx=(RealX-256)>>11;
	int cy=(RealY-256)>>11;
	int mx=msx>>2;
	int my=msy>>2;
	if(cx<0){
		RealX=256+512;
		cx=0;
	};
	if(cy<0){
		RealY=256+512;
		cy=0;
	};
	if(cx>=mx){
		RealX=(msx<<9)-256;
		cx=mx;
	};
	if(cy>=my){
		RealY=(msy<<9)-256;
		cy=my;
	};
	int Fx=0;
	int Fy=0;
	int dfx;
	int dfy;
	MyHeight=Height;
	FlyCell* FC=&FlyMap[cy][cx];
	FC->GetForces(RealX,RealY,&dfx,&dfy,Index);
	Fx+=dfx;
	Fy+=dfy;
	if(cx>0){
		FC[-1].GetForces(RealX,RealY,&dfx,&dfy,Index);
		Fx+=dfx;
		Fy+=dfy;
	};
	if(cy>0){
		FC[-64].GetForces(RealX,RealY,&dfx,&dfy,Index);
		Fx+=dfx;
		Fy+=dfy;
	};
	if(cx<=mx){
		FC[1].GetForces(RealX,RealY,&dfx,&dfy,Index);
		Fx+=dfx;
		Fy+=dfy;
	};
	if(cy<=my){
		FC[64].GetForces(RealX,RealY,&dfx,&dfy,Index);
		Fx+=dfx;
		Fy+=dfy;
	};
	int rdx=DestX-RealX;
	int rdy=DestY-RealY;
	int ard=abs(rdx)+abs(rdy);
	if(ard<128){
		rdx=0;
		rdy=0;
	};
	if(ard>256){
		rdx=div(rdx<<8,ard).quot;
		rdy=div(rdy<<8,ard).quot;
	};
	Fx+=rdx>>3;
	Fy+=rdy>>3;
	int af=abs(Fx)+abs(Fy);
	if(af>32){
		Fx=div(Fx<<5,af).quot;
		Fy=div(Fy<<5,af).quot;
	};
	int ax=-(RealVx>>3)+Fx;
	int ay=-(RealVy>>3)+Fy;
	ard=abs(ax)+abs(ay);
	if(ard>32){
		ax=div(ax<<5,ard).quot;
		ay=div(ay<<5,ard).quot;
	};
	RealVx+=ax;
	RealVy+=ay;
	if((!(ax||ay))&&(abs(RealVx)<8&&abs(RealVy<8))){
		RealVx=0;
		RealVy=0;
	};
	/*ard=sqrt((RealVx*RealVx)+(RealVy*RealVy));
	if(ard>MaxSpeed){
		RealVx=div(RealVx*MaxSpeed,ard).quot;
		RealVy=div(RealVy*MaxSpeed,ard).quot;
	};*/
	int rxx=RealX;
	int ryy=RealY;
	RealX+=RealVx;
	RealY+=RealVy;
	int ocx=cx;
	int ocy=cy;
	cx=(RealX-256)>>11;
	cy=(RealY-256)>>11;
	if(cx<0){
		RealX=256+512;
		cx=0;
	};
	if(cy<0){
		RealY=256+512;
		cy=0;
	};
	if(cx>=mx){
		RealX=(msx<<9)-256;
		cx=mx-1;
	};
	if(cy>=my){
		RealY=(msy<<9)-256;
		cy=my-1;
	};
	if(ocx!=cx||ocy!=cy){
		FlyCell* FC1=&FlyMap[cy][cx];
		if(FC1->NFly>=15){
			RealX=rxx;
			RealY=ryy;
		}else{
			word MD=Index;
			for(int p=0;p<15;p++)
				if(FC->FlyList[p]==MD){
					FC->FlyList[p]=0xFFFF;
					break;
				};
			FC->NFly--;
			for(p=0;p<15;p++)
				if(FC1->FlyList[p]==0xFFFF){
					FC1->FlyList[p]=MD;
					break;
				};
			FC1->NFly++;
		};
	};
	int RDIR;
	Height=0;
	if(abs(RealVx)>32||abs(RealVy)>32){
		short adx=abs(RealVx);
		short ady=abs(RealVy);
		Height=(adx+ady)>>3;
		if(adx>ady)RDIR=byte(TAtg[div(ady*256,adx).quot]);else
				   RDIR=64-byte(TAtg[div(adx*256,ady).quot]);
		if(RealVx<0)RDIR=128-RDIR;
		if(RealVy<0)RDIR=256-RDIR;
		byte ddr=(256+RDIR-RealDir)&255;
		if(ddr<128){
			if(ddr<5)RealDir=RDIR;
			else RealDir+=5;
		}else{
			ddr=256-ddr;
			if(ddr<5)RealDir=RDIR;
			else RealDir-=5;
		};
		RealDir&=255;
		Direction=((RealDir+80)&255)>>5;
	};
	x=RealX>>9;
	y=RealY>>9;
	byte csp=CurrentSprite;
	LoadCurAnm(0);
	CurrentSprite=csp;
	TicksPerChange=2;
};
void OneObject::ProcessAttackMotion(){
	int cx=(RealX-256)>>11;
	int cy=(RealY-256)>>11;
	int mx=msx>>2;
	int my=msy>>2;
	if(cx<0){
		RealX=256+512;
		cx=0;
	};
	if(cy<0){
		RealY=256+512;
		cy=0;
	};
	if(cx>=mx){
		RealX=(msx<<9)-256;
		cx=mx;
	};
	if(cy>=my){
		RealY=(msy<<9)-256;
		cy=my;
	};
	int Fx=0;
	int Fy=0;
	int dfx;
	int dfy;
	MyHeight=Height;
	FlyCell* FC=&FlyMap[cy][cx];
	FC->GetForces(RealX,RealY,&dfx,&dfy,Index);
	Fx+=dfx;
	Fy+=dfy;
	if(cx>0){
		FC[-1].GetForces(RealX,RealY,&dfx,&dfy,Index);
		Fx+=dfx;
		Fy+=dfy;
	};
	if(cy>0){
		FC[-64].GetForces(RealX,RealY,&dfx,&dfy,Index);
		Fx+=dfx;
		Fy+=dfy;
	};
	if(cx<=mx){
		FC[1].GetForces(RealX,RealY,&dfx,&dfy,Index);
		Fx+=dfx;
		Fy+=dfy;
	};
	if(cy<=my){
		FC[64].GetForces(RealX,RealY,&dfx,&dfy,Index);
		Fx+=dfx;
		Fy+=dfy;
	};
	int rdx=DestX-RealX;
	int rdy=DestY-RealY;
	int ard=abs(rdx)+abs(rdy);
	bool DIRFORWARD=true;
	if(ard<BestDist){
		DIRFORWARD=false;
		if(ard>256){
			rdx=div(rdx<<8,ard).quot;
			rdy=div(rdy<<8,ard).quot;
		};
		Fx-=rdx>>4;
		Fy-=rdy>>4;
	}else{
		if(ard>BestDist+1024){
			if(ard>256){
				rdx=div(rdx<<8,ard).quot;
				rdy=div(rdy<<8,ard).quot;
			};
			Fx+=rdx>>3;
			Fy+=rdy>>3;
		}else{
			RealVx-=RealVx>>3;
			RealVy-=RealVy>>3;
		};
	};
	/*int af=abs(Fx)+abs(Fy);
	if(af>32){
		Fx=div(Fx<<5,af).quot;
		Fy=div(Fy<<5,af).quot;
	};*/
	int ax=-(RealVx>>3)+Fx;
	int ay=-(RealVy>>3)+Fy;
	ard=abs(ax)+abs(ay);
	if(ard>32){
		ax=div(ax<<5,ard).quot;
		ay=div(ay<<5,ard).quot;
	};
	RealVx+=ax;
	RealVy+=ay;
	if((!(ax||ay))&&(abs(RealVx)<8&&abs(RealVy<8))){
		RealVx=0;
		RealVy=0;
	};
	/*ard=sqrt((RealVx*RealVx)+(RealVy*RealVy));
	if(ard>MaxSpeed){
		RealVx=div(RealVx*MaxSpeed,ard).quot;
		RealVy=div(RealVy*MaxSpeed,ard).quot;
	};*/
	int rxx=RealX;
	int ryy=RealY;
	RealX+=RealVx;
	RealY+=RealVy;
	int ocx=cx;
	int ocy=cy;
	cx=(RealX-256)>>11;
	cy=(RealY-256)>>11;
	if(cx<0){
		RealX=256+512;
		cx=0;
	};
	if(cy<0){
		RealY=256+512;
		cy=0;
	};
	if(cx>=mx){
		RealX=(msx<<9)-256;
		cx=mx;
	};
	if(cy>=my){
		RealY=(msy<<9)-256;
		cy=my;
	};
	if(ocx!=cx||ocy!=cy){
		FlyCell* FC1=&FlyMap[cy][cx];
		if(FC1->NFly>=15){
			RealX=rxx;
			RealY=ryy;
		}else{
			word MD=Index;
			for(int p=0;p<15;p++)
				if(FC->FlyList[p]==MD){
					FC->FlyList[p]=0xFFFF;
					break;
				};
			FC->NFly--;
			for(p=0;p<15;p++)
				if(FC1->FlyList[p]==0xFFFF){
					FC1->FlyList[p]=MD;
					break;
				};
			FC1->NFly++;
		};
	};
	int RDIR;
	Height=0;
	int Dx=-RealX+DestX;
	int Dy=-RealY+DestY;
	Height=(abs(RealVx)+abs(RealVy))>>3;
	if(abs(Dx)>32||abs(Dy)>32){
		int adx=abs(Dx);
		int ady=abs(Dy);
		if(adx>ady)RDIR=byte(TAtg[div(ady*256,adx).quot]);else
				   RDIR=64-byte(TAtg[div(adx*256,ady).quot]);
		if(Dx<0)RDIR=128-RDIR;
		if(Dy<0)RDIR=256-RDIR;
		byte ddr=(256+RDIR-RealDir)&255;
		if(ddr<128){
			if(ddr<16)RealDir=RDIR;
			else RealDir+=16;
		}else{
			ddr=256-ddr;
			if(ddr<16)RealDir=RDIR;
			else RealDir-=16;
		};
		RealDir&=255;
		Direction=((RealDir+80)&255)>>5;
	};
	x=RealX>>9;
	y=RealY>>9;
	byte csp=CurrentSprite;
	//LoadCurAnm(0);
	CurrentSprite=csp;
	TicksPerChange=2;
};
inline void Spota(int x,int y){
	if(x>0&&y>0&&x<msx&&y<msy){
		__asm{
			xor		eax,eax
			mov		al,byte ptr x
			mov		ah, byte ptr y
			shl		eax,1
			mov		word ptr[fmap+eax],16383;
		};
	};
};
extern int tmtmt;
void OneObject::ProcessFly(){
	if(Media!=2)return;
	if(NNUM==MyNation){
		int x0,y0,dx,dx1;
		switch(SpotType){
		case 0:
			if(Lx>1){
			for(byte ux=0;ux<Lx;ux++)
				for(byte uy=0;uy<Ly;uy++)
					Spota(x+ux,y+uy);
			}else Spota(x,y);
			break;
		case 1:
			x0=x+(Lx>>1);
			y0=y+(Ly>>1);
			dx=SpotSize;
			Spota(x0,y0);
			Spota(x0+dx,y0);
			Spota(x0-dx,y0);
			Spota(x0,y0+dx);
			Spota(x0,y0-dx);
			break;
		case 2:
			x0=x+(Lx>>1);
			y0=y+(Ly>>1);
			dx=SpotSize;
			dx1=dx-(dx>>2);
			Spota(x0,y0);
			Spota(x0+dx,y0);
			Spota(x0-dx,y0);
			Spota(x0,y0+dx);
			Spota(x0,y0-dx);
			Spota(x0+dx1,y0+dx1);
			Spota(x0-dx1,y0-dx1);
			Spota(x0+dx1,y0-dx1);
			Spota(x0-dx1,y0+dx);
			break;
		};
	};
	if(MagSrcID!=0xFFFF){
		OneObject* OB=Group[MagSrcID];
		if(OB->Magic){
			if(!(tmtmt&31))OB->Magic--;
			if(OB->Magic<8){
				MakeMeSit();
				return;
			};
		}else{
			MakeMeSit();
			return;
		};
	};
	byte tc=Ticks;
	if(FlyAttackMode)ProcessAttackMotion();
	else ProcessMotion();
	Ticks=tc;
	Ticks++;
	if(Sdoxlo)TicksPerChange=1;
	if(Ticks<TicksPerChange){
		Ticks++;
		if(TicksPerChange!=255)return;
	};
	Ticks=0;
	int Nsp=CurAnm->count-1;
	if(CurrentSprite<Nsp&&TicksPerChange!=255){
		CurrentSprite++;
		//Ticks=0;
		//TicksPerChange=1;
		return;
	};
	CurrentSprite=0;
	if(Sdoxlo){
		Group[Index]=NULL;
		return;
	};
	if(!NoSearchVictim)SearchVictim();
/*	Addx=0;
	Addy=0;
	Removed=false;
	StandTime++;*/
	if(Life<=0){
		Die();
		return;
	};
	LoadCurAnm(0);
	//if(!Important)
	//if(!NoSearchVictim)SearchVictim();
	Order1* LO=LocalOrder;
	if(!LO)PrioryLevel=0;
	//if(TicksPerChange!=255)LoadCurAnm(0);
	//Important=false;
	if(int(LocalOrder))
	LocalOrder->DoLink(this);
	else{
		DestX=RealX;
		DestY=RealY;
	};
};
void DrawMarker(OneObject* OB);
void DrawMiniMarker(OneObject* OB);
void ShowFlyHealth(OneObject* OB){
	if(MiniMode)DrawMiniMarker(OB);
	else DrawMarker(OB);
	return;
	int	scx=smapx+(OB->RealX>>4-32)-(mapx<<5)-32+20;
	int	scy=smapy+(OB->RealY>>4-32)-(mapy<<5)-16-64-16-OB->Height;
	int dy=64;
	int dx=24;
	Xbar(scx+2,scy+dy-4,dx-4,4,14);
	Xbar(scx-1,scy+dy-3,3,2,clrGreen);
	Xbar(scx+2-4+dx,scy+dy-3,3,2,clrGreen);
	int h=div(OB->Life*(dx-4),OB->MaxLife).quot;
	Hline(scx+3,scy+1-4+dy,scx+1+h,255);
	Hline(scx+3,scy+2-4+dy,scx+1+h,255);
};
/*void ShowFlyingMonsters(){
	word MonsL[1024];
	word MHeight[1024];
	word NMonst;
	int mx=msx>>2-1;
	int my=msy>>2-1;
	int x0=(mapx>>2)-1;
	if(x0<0)x0=0;
	int y0=(mapy>>2)-1;
	if(y0<0)y0=0;
	int x1=mapx+smaplx;
	int y1=mapy+smaply;
	if(x1&3)x1=(x1>>2)+2;
	else x1=(x1>>2)+1;
	if(y1&3)y1=(y1>>2)+2;
	else y1=(y1>>2)+1;
	if(x1>mx)x1=mx;
	if(y1>my)y1=my;
	int mxm=(smaplx<<5)+64;
	int mym=(smaply<<5)+64;
	for(int i=y0;i<=y1;i++)
		for(int j=x0;j<=x1;j++){
			FlyCell* FC=&FlyMap[i][j];
			if(FC->NFly){
				for(int p=0;p<15;p++){
					word MID=FC->FlyList[p];
					if(MID!=0xFFFF){
						OneObject* OB=Group[MID];
						if(OB&&OB->Media==2){
							int xs=(OB->RealX>>4)-(mapx<<5)-16;
							int ys=(OB->RealY>>4)-(mapy<<5)-16;
							if(xs>-64&&ys>-64&&xs<mxm&&ys<mym){
								Octant* Oc2=OB->CurAnm;
								OneSlide* Oc1=&(Oc2->Movie[OB->CurrentSprite]);
								byte clr=OB->NNUM&3;
								int spr=Oc1->spr;
								if(OB->Selected)ShowFlyHealth(OB);
								int ady=OB->Height;
								if(spr<4096)
									if(OB->Invert){
										switch(Quality){
										case 2:
											ShowRLCItemShadow(xs+32+smapx-Oc1->dx-4,ys+smapy+Oc1->dy+32+ady,
											&MImage[Oc1->FileID],spr+4096);
											break;
										case 0:
											ShowRLCItemShadow(xs+32+smapx-Oc1->dx-4-4,ys+smapy+Oc1->dy+32-4+ady,
												&MImage[Oc1->FileID],spr+4096);
											ShowRLCItemShadow(xs+32+smapx-Oc1->dx-4-4,ys+smapy+Oc1->dy+32+ady,
												&MImage[Oc1->FileID],spr+4096);
											ShowRLCItemShadow(xs+32+smapx-Oc1->dx-4,ys+smapy+Oc1->dy+32-4+ady,
												&MImage[Oc1->FileID],spr+4096);
											ShowRLCItemShadow(xs+32+smapx-Oc1->dx-4,ys+smapy+Oc1->dy+32+ady,
												&MImage[Oc1->FileID],spr+4096);
											break;
										case 1:
											ShowRLCItemShadow(xs+32+smapx-Oc1->dx-4,ys+smapy+Oc1->dy+32-4+ady,
												&MImage[Oc1->FileID],spr+4096);
											ShowRLCItemShadow(xs+32+smapx-Oc1->dx-4,ys+smapy+Oc1->dy+32+ady,
												&MImage[Oc1->FileID],spr+4096);
											break;
										};
										ShowRLCItem(xs+32+smapx-Oc1->dx,ys+smapy+Oc1->dy-ady,
										&MImage[Oc1->FileID],spr+4096,clr);
									}else{
										switch(Quality){
											case 2:
												ShowRLCItemShadow(xs+smapx+Oc1->dx-4,ys+smapy+Oc1->dy+32+ady,
													&MImage[Oc1->FileID],spr);
												break;
											case 0:
												ShowRLCItemShadow(xs+smapx+Oc1->dx-4-4,ys+smapy+Oc1->dy+32-4+ady,
													&MImage[Oc1->FileID],spr);
												ShowRLCItemShadow(xs+smapx+Oc1->dx-4-4,ys+smapy+Oc1->dy+32+ady,
													&MImage[Oc1->FileID],spr);
												ShowRLCItemShadow(xs+smapx+Oc1->dx-4,ys+smapy+Oc1->dy+32-4+ady,
													&MImage[Oc1->FileID],spr);
												ShowRLCItemShadow(xs+smapx+Oc1->dx-4,ys+smapy+Oc1->dy+32+ady,
													&MImage[Oc1->FileID],spr);
												break;
											case 1:
												ShowRLCItemShadow(xs+smapx+Oc1->dx-4,ys+smapy+Oc1->dy+32-4+ady,
													&MImage[Oc1->FileID],spr);
												ShowRLCItemShadow(xs+smapx+Oc1->dx-4,ys+smapy+Oc1->dy+32+ady,
													&MImage[Oc1->FileID],spr);
												break;
										};
										ShowRLCItem(xs+smapx+Oc1->dx,ys+smapy+Oc1->dy-ady,
										&MImage[Oc1->FileID],spr,clr);
									};
							};
						};
					};
				};
			};
		};
};*/
void ShowFlyingMonsters(){
	word MonsL[1024];
	word MHeight[1024];
	word NMonst=0;
	int mx=msx>>2-1;
	int my=msy>>2-1;
	int x0=(mapx>>2)-1;
	if(x0<0)x0=0;
	int y0=(mapy>>2)-1;
	if(y0<0)y0=0;
	int x1=mapx+smaplx;
	int y1=mapy+smaply;
	if(x1&3)x1=(x1>>2)+2;
	else x1=(x1>>2)+1;
	if(y1&3)y1=(y1>>2)+2;
	else y1=(y1>>2)+1;
	if(x1>mx)x1=mx;
	if(y1>my)y1=my;
	int mxm=(smaplx<<5)+64;
	int mym=(smaply<<5)+64;
	for(int i=y0;i<=y1;i++)
		for(int j=x0;j<=x1;j++){
			FlyCell* FC=&FlyMap[i][j];
			if(FC->NFly){
				for(int p=0;p<15;p++){
					word MID=FC->FlyList[p];
					if(MID!=0xFFFF){
						OneObject* OB=Group[MID];
						if(OB&&OB->Media==2&&NMonst<1024){
							MonsL[NMonst]=MID;
							MHeight[NMonst]=OB->Height;
							NMonst++;
						};
					};
				};
			};
		};
	for(i=0;i<NMonst;i++){
		word MinH=0xFFFF;
		word MD=0xFFFF;
		word JJ=0;
		for(int j=0;j<NMonst;j++){
			word H=MHeight[j];
			if(H<MinH){
				MinH=H;
				MD=MonsL[j];
				JJ=j;
			};
		};
		if(MD!=0xFFFF){
			MHeight[JJ]=0xFFFF;
			OneObject* OB=Group[MD];
			MyHeight=OB->Height;
			int xs=(OB->RealX>>4)-(mapx<<5)-16;
			int ys=(OB->RealY>>4)-(mapy<<5)-16;
			if(xs>-64&&ys>-64&&xs<mxm&&ys<mym){
			Octant* Oc2=OB->CurAnm;
			OneSlide* Oc1=&(Oc2->Movie[OB->CurrentSprite]);
			byte clr=OB->NNUM;
			int spr=Oc1->spr;
			if(OB->Selected)ShowFlyHealth(OB);
			int ady=OB->Height;
			if(spr<4096)
				if(OB->Invert){
					switch(Quality){
						case 2:
							ShowRLCItemShadow(xs+32+smapx-Oc1->dx-4,ys+smapy+Oc1->dy+32+ady,
								&MImage[Oc1->FileID],spr+4096);
							break;
						case 0:
							ShowRLCItemShadow(xs+32+smapx-Oc1->dx-4-4,ys+smapy+Oc1->dy+32-4+ady,
								&MImage[Oc1->FileID],spr+4096);
							ShowRLCItemShadow(xs+32+smapx-Oc1->dx-4-4,ys+smapy+Oc1->dy+32+ady,
								&MImage[Oc1->FileID],spr+4096);
							ShowRLCItemShadow(xs+32+smapx-Oc1->dx-4,ys+smapy+Oc1->dy+32-4+ady,
								&MImage[Oc1->FileID],spr+4096);
							ShowRLCItemShadow(xs+32+smapx-Oc1->dx-4,ys+smapy+Oc1->dy+32+ady,
								&MImage[Oc1->FileID],spr+4096);
							break;
						case 1:
							ShowRLCItemShadow(xs+32+smapx-Oc1->dx-4,ys+smapy+Oc1->dy+32-4+ady,
								&MImage[Oc1->FileID],spr+4096);
							ShowRLCItemShadow(xs+32+smapx-Oc1->dx-4,ys+smapy+Oc1->dy+32+ady,
								&MImage[Oc1->FileID],spr+4096);
							break;
						};
							ShowRLCItem(xs+32+smapx-Oc1->dx,ys+smapy+Oc1->dy-ady,
								&MImage[Oc1->FileID],spr+4096,clr);
					}else{
						switch(Quality){
							case 2:
								ShowRLCItemShadow(xs+smapx+Oc1->dx-4,ys+smapy+Oc1->dy+32+ady,
									&MImage[Oc1->FileID],spr);
								break;
							case 0:
								ShowRLCItemShadow(xs+smapx+Oc1->dx-4-4,ys+smapy+Oc1->dy+32-4+ady,
									&MImage[Oc1->FileID],spr);
								ShowRLCItemShadow(xs+smapx+Oc1->dx-4-4,ys+smapy+Oc1->dy+32+ady,
									&MImage[Oc1->FileID],spr);
								ShowRLCItemShadow(xs+smapx+Oc1->dx-4,ys+smapy+Oc1->dy+32-4+ady,
									&MImage[Oc1->FileID],spr);
								ShowRLCItemShadow(xs+smapx+Oc1->dx-4,ys+smapy+Oc1->dy+32+ady,
									&MImage[Oc1->FileID],spr);
								break;
							case 1:
								ShowRLCItemShadow(xs+smapx+Oc1->dx-4,ys+smapy+Oc1->dy+32-4+ady,
									&MImage[Oc1->FileID],spr);
								ShowRLCItemShadow(xs+smapx+Oc1->dx-4,ys+smapy+Oc1->dy+32+ady,
									&MImage[Oc1->FileID],spr);
								break;
							};
							ShowRLCItem(xs+smapx+Oc1->dx,ys+smapy+Oc1->dy-ady,
								&MImage[Oc1->FileID],spr,clr);
					};
				};
		};
	};
};
void miniShowFlyingMonsters(){
	word MonsL[1024];
	word MHeight[1024];
	word NMonst=0;
	int mx=msx>>2-1;
	int my=msy>>2-1;
	int x0=(mapx>>2)-1;
	if(x0<0)x0=0;
	int y0=(mapy>>2)-1;
	if(y0<0)y0=0;
	int x1=mapx+smaplx;
	int y1=mapy+smaply;
	if(x1&3)x1=(x1>>2)+2;
	else x1=(x1>>2)+1;
	if(y1&3)y1=(y1>>2)+2;
	else y1=(y1>>2)+1;
	if(x1>mx)x1=mx;
	if(y1>my)y1=my;
	int mxm=(smaplx<<5)+64;
	int mym=(smaply<<5)+64;
	for(int i=y0;i<=y1;i++)
		for(int j=x0;j<=x1;j++){
			FlyCell* FC=&FlyMap[i][j];
			if(FC->NFly){
				for(int p=0;p<15;p++){
					word MID=FC->FlyList[p];
					if(MID!=0xFFFF){
						OneObject* OB=Group[MID];
						if(OB&&OB->Media==2&&NMonst<1024){
							MonsL[NMonst]=MID;
							MHeight[NMonst]=OB->Height;
							NMonst++;
						};
					};
				};
			};
		};
	for(i=0;i<NMonst;i++){
		word MinH=0xFFFF;
		word MD=0xFFFF;
		word JJ=0;
		for(int j=0;j<NMonst;j++){
			word H=MHeight[j];
			if(H<MinH){
				MinH=H;
				MD=MonsL[j];
				JJ=j;
			};
		};
		if(MD!=0xFFFF){
			MHeight[JJ]=0xFFFF;
			OneObject* OB=Group[MD];
			MyHeight=OB->Height;
			int xs=(OB->RealX>>5)-(mapx<<4)-8;
			int ys=(OB->RealY>>5)-(mapy<<4)-8;
			if(xs>-64&&ys>-64&&xs<mxm&&ys<mym){
			Octant* Oc2=OB->CurAnm;
			OneSlide* Oc1=&(Oc2->Movie[OB->CurrentSprite]);
			byte clr=OB->NNUM;
			int spr=Oc1->spr;
			if(OB->Selected)ShowFlyHealth(OB);
			int ady=OB->Height>>1;
			if(spr<4096)
				if(OB->Invert){
					switch(Quality){
						case 2:
							ShowRLCItemShadow(xs+16+smapx-(Oc1->dx>>1)-2,ys+smapy+(Oc1->dy>>1)+16+ady,
								&miniMImage[Oc1->FileID],spr+4096);
							break;
							/*
						case 0:
							ShowRLCItemShadow(xs+32+smapx-Oc1->dx-4-4,ys+smapy+Oc1->dy+32-4+ady,
								&MImage[Oc1->FileID],spr+4096);
							ShowRLCItemShadow(xs+32+smapx-Oc1->dx-4-4,ys+smapy+Oc1->dy+32+ady,
								&MImage[Oc1->FileID],spr+4096);
							ShowRLCItemShadow(xs+32+smapx-Oc1->dx-4,ys+smapy+Oc1->dy+32-4+ady,
								&MImage[Oc1->FileID],spr+4096);
							ShowRLCItemShadow(xs+32+smapx-Oc1->dx-4,ys+smapy+Oc1->dy+32+ady,
								&MImage[Oc1->FileID],spr+4096);
							break;
						case 1:
							ShowRLCItemShadow(xs+32+smapx-Oc1->dx-4,ys+smapy+Oc1->dy+32-4+ady,
								&MImage[Oc1->FileID],spr+4096);
							ShowRLCItemShadow(xs+32+smapx-Oc1->dx-4,ys+smapy+Oc1->dy+32+ady,
								&MImage[Oc1->FileID],spr+4096);
							break;*/
						};

						ShowRLCItem(xs+16+smapx-(Oc1->dx>>1),ys+smapy+(Oc1->dy>>1)-ady,
							&miniMImage[Oc1->FileID],spr+4096,clr);
					}else{
						switch(Quality){
							case 2:
								ShowRLCItemShadow(xs+smapx+(Oc1->dx>>1)-2,ys+smapy+(Oc1->dy>>1)+16+ady,
									&miniMImage[Oc1->FileID],spr);
								break;
								/*
							case 0:
								ShowRLCItemShadow(xs+smapx+Oc1->dx-4-4,ys+smapy+Oc1->dy+32-4+ady,
									&MImage[Oc1->FileID],spr);
								ShowRLCItemShadow(xs+smapx+Oc1->dx-4-4,ys+smapy+Oc1->dy+32+ady,
									&MImage[Oc1->FileID],spr);
								ShowRLCItemShadow(xs+smapx+Oc1->dx-4,ys+smapy+Oc1->dy+32-4+ady,
									&MImage[Oc1->FileID],spr);
								ShowRLCItemShadow(xs+smapx+Oc1->dx-4,ys+smapy+Oc1->dy+32+ady,
									&MImage[Oc1->FileID],spr);
								break;
							case 1:
								ShowRLCItemShadow(xs+smapx+Oc1->dx-4,ys+smapy+Oc1->dy+32-4+ady,
									&MImage[Oc1->FileID],spr);
								ShowRLCItemShadow(xs+smapx+Oc1->dx-4,ys+smapy+Oc1->dy+32+ady,
									&MImage[Oc1->FileID],spr);
								break;*/
							};
						ShowRLCItem(xs+smapx+(Oc1->dx>>1),ys+smapy+(Oc1->dy>>1)-ady,
							&miniMImage[Oc1->FileID],spr,clr);
					};
				};
		};
	};
};
inline bool PInside(int x,int y,int x1,int y1,int xp,int yp){
	if(xp>=x&&xp<=x1&&yp>=y&&yp<=y1)return true;
	else return false;
};
word SelectFly(byte NI,int xr,int yr,int xr1,int yr1,word *Collect,word* Ser,bool WRITE){
	int x,y,x1p,y1p,x1,y1;
	int xp=xr>>5;
	int yp=yr>>5;
	int xp1=xr1>>5;
	int yp1=yr1>>5;
	if(xp>xp1){
		x=xp1;
		x1=xp;
	}else{
		x1=xp1;
		x=xp;
	};
	if(yp>yp1){
		y=yp1;
		y1=yp;
	}else{
		y1=yp1;
		y=yp;
	};
	x1p=x1;
	y1p=y1;
	int mx=msx>>2-1;
	int my=msy>>2-1;
	int x0=(x>>2)-1;
	if(x0<0)x0=0;
	int y0=(y>>2)-1;
	if(y0<0)y0=0;
	if(x1&3)x1=(x1>>2)+2;
	else x1=(x1>>2)+1;
	if(y1&3)y1=(y1>>2)+2;
	else y1=(y1>>2)+1;
	if(x1>mx)x1=mx;
	if(y1>my)y1=my;
	int mxm=(smaplx<<5)+64;
	int mym=(smaply<<5)+64;
	int NSLC=0;
	x=xr;
	y=yr;
	x1p=xr1;
	y1p=yr1;
	for(int i=y0;i<=y1;i++)
		for(int j=x0;j<=x1;j++){
			FlyCell* FC=&FlyMap[i][j];
			if(FC->NFly){
				for(int p=0;p<15;p++){
					word MID=FC->FlyList[p];
					if(MID!=0xFFFF){
						OneObject* OB=Group[MID];
						if(OB&&OB->Media==2&&OB->NNUM==NI&&!OB->Selected){
							int xs=(OB->RealX>>4)-40;
							int ys=(OB->RealY>>4)-40+10;
							int xs1=xs+80;
							int ys1=ys+80;
							if(PInside(x,y,x1p,y1p,xs,ys)||
							   PInside(x,y,x1p,y1p,xs1,ys)||
							   PInside(x,y,x1p,y1p,xs,ys1)||
							   PInside(x,y,x1p,y1p,xs1,ys1)||
							   PInside(xs,ys,xs1,ys1,x,y)||
							   PInside(xs,ys,xs1,ys1,x1p,y)||
							   PInside(xs,ys,xs1,ys1,x,y1p)||
							   PInside(xs,ys,xs1,ys1,x1p,y1p)){
								if(WRITE){
									Collect[NSLC]=MID;
									Ser[NSLC]=OB->Serial;
									OB->Selected=true;
								};
								NSLC++;
							};
						};
					};
				};
			};
		};
	return NSLC;
};
word GoodSelectFly(byte NI,int xr,int yr,int xr1,int yr1,word *Collect,word* Ser,bool WRITE,CHOBJ* FN,int NN){
	int x,y,x1p,y1p,x1,y1;
	int xp=xr>>5;
	int yp=yr>>5;
	int xp1=xr1>>5;
	int yp1=yr1>>5;
	if(xp>xp1){
		x=xp1;
		x1=xp;
	}else{
		x1=xp1;
		x=xp;
	};
	if(yp>yp1){
		y=yp1;
		y1=yp;
	}else{
		y1=yp1;
		y=yp;
	};
	x1p=x1;
	y1p=y1;
	int mx=msx>>2-1;
	int my=msy>>2-1;
	int x0=(x>>2)-1;
	if(x0<0)x0=0;
	int y0=(y>>2)-1;
	if(y0<0)y0=0;
	if(x1&3)x1=(x1>>2)+2;
	else x1=(x1>>2)+1;
	if(y1&3)y1=(y1>>2)+2;
	else y1=(y1>>2)+1;
	if(x1>mx)x1=mx;
	if(y1>my)y1=my;
	int mxm=(smaplx<<5)+64;
	int mym=(smaply<<5)+64;
	int NSLC=0;
	x=xr;
	y=yr;
	x1p=xr1;
	y1p=yr1;
	for(int i=y0;i<=y1;i++)
		for(int j=x0;j<=x1;j++){
			FlyCell* FC=&FlyMap[i][j];
			if(FC->NFly){
				for(int p=0;p<15;p++){
					word MID=FC->FlyList[p];
					if(MID!=0xFFFF){
						OneObject* OB=Group[MID];
						if(OB&&(!OB->Selected)&&OB->Media==2&&OB->NNUM==NI&&((!FN)||FN(OB,NN))){
							int xs=(OB->RealX>>4)-40;
							int ys=(OB->RealY>>4)-40+10;
							int xs1=xs+80;
							int ys1=ys+80;
							if(PInside(x,y,x1p,y1p,xs,ys)||
							   PInside(x,y,x1p,y1p,xs1,ys)||
							   PInside(x,y,x1p,y1p,xs,ys1)||
							   PInside(x,y,x1p,y1p,xs1,ys1)||
							   PInside(xs,ys,xs1,ys1,x,y)||
							   PInside(xs,ys,xs1,ys1,x1p,y)||
							   PInside(xs,ys,xs1,ys1,x,y1p)||
							   PInside(xs,ys,xs1,ys1,x1p,y1p)){
								if(WRITE){
									Collect[NSLC]=MID;
									Ser[NSLC]=OB->Serial;
									OB->Selected=true;
								};
								NSLC++;
							};
						};
					};
				};
			};
		};
	return NSLC;
};
void FlySendToLink(OneObject* OB);
void OneObject::SendFlyTo(int x,int y,byte Prio){
	if(PrioryLevel>Prio)return;
	if(!cpbMoving)return;
	Order1* Or1=GetOrdBlock();
	if(!int(Or1))return;
	Or1->PrioryLevel=Prio&127;
	Or1->NextOrder=NULL;
	Or1->OrderType=2;
	Or1->OrderTime=0;
	Or1->DoLink=&FlySendToLink;
	Or1->info.MoveToXY.x=x;
	Or1->info.MoveToXY.y=y;
	Order1* LOR=LocalOrder;
	if(int(LOR)){
		ClearOrders();
		if(LOR->OrderType!=2)StandTime=0; 
	}else StandTime=0;
	if(int(InLineCom)){
		FreeAsmLink();
	};
	LocalOrder=Or1;
	//OrderReport=NULL;
	//MessageKind=0;
	//Sender=0xFFFF;
	DestX=(int(x)<<9)+256;
	DestY=(int(y)<<9)+256;
	PrioryLevel=Prio&127;
};
void FlySendToLink(OneObject* OB){
	OB->FlyAttackMode=false;
	int dst=abs(OB->RealX-OB->DestX)+abs(OB->RealY-OB->DestY);
	if(dst<2048){
		if(int(OB->LocalOrder)){
			Order1* Loc1=OB->LocalOrder->NextOrder;
			OB->FreeOrdBlock(OB->LocalOrder);
			OB->LocalOrder=Loc1;
		};
	};
};
void FlyAttackLink(OneObject* OB);
void OneObject::FlyAttack(word OID,byte Prio){
	assert(OID!=0xFFFF);
	if(PrioryLevel>Prio)return;
	if(!cpbMoving)return;
	OneObject* OBB=Group[OID];
	if(!OBB)return;
	if(OBB->Sdoxlo||OBB->NMask&NMask)return;
	Order1* Or1=GetOrdBlock();
	if(!int(Or1))return;
	Or1->PrioryLevel=Prio&127;
	Or1->NextOrder=NULL;
	Or1->OrderType=3;
	Or1->OrderTime=0;
	Or1->DoLink=&FlyAttackLink;
	Or1->info.MoveToObj.ObjIndex=OID;
	Order1* LOR=LocalOrder;
	if(int(LOR)){
		ClearOrders();
		if(LOR->OrderType!=2)StandTime=0; 
	}else StandTime=0;
	if(int(InLineCom)){
		FreeAsmLink();
	};
	LocalOrder=Or1;
	EnemyID=OID;
	EnemySN=OBB->Serial;
	//OrderReport=NULL;
	//MessageKind=0;
	//Sender=0xFFFF;
	DestX=(int(x)<<9)+256;
	DestY=(int(y)<<9)+256;
	PrioryLevel=Prio&127;
	FlyAttackMode=true;
};
void FlyAttackLink(OneObject* OBJ){
	OBJ->LoadCurAnm(0);
	OBJ->FlyAttackMode=1;
	word MID=OBJ->LocalOrder->info.MoveToObj.ObjIndex;
	if(MID!=0xFFFF){
		OneObject* OB=Group[MID];
		if(OB&&!((OB->NMask&OBJ->NMask)||OB->Sdoxlo||OB->Serial!=OBJ->EnemySN)){
			if(OB->Media!=2){
				OBJ->DestX=(OB->x<<9)+256;
				OBJ->DestY=(OB->y<<9)+256;
			}else{
				OBJ->DestX=OB->RealX;
				OBJ->DestY=OB->RealY;
			};
		}else{
			Order1* Loc1=OBJ->LocalOrder->NextOrder;
			OBJ->FreeOrdBlock(OBJ->LocalOrder);
			OBJ->LocalOrder=Loc1;
			OBJ->FlyAttackMode=0;
			return;
		};
		int wcx=OBJ->RealX+((int(OBJ->wepX)+int(OBJ->Ref.General->Wdx[OBJ->Direction]))<<4)-256;
		int wcy=OBJ->RealY+((int(OBJ->wepY)+int(OBJ->Ref.General->Wdy[OBJ->Direction]))<<4)-(OBJ->Height<<4)-256;
		int adx=OBJ->DestX-wcx-256;
		int ady=OBJ->DestY-wcy-256;
		if(OBJ->delay)OBJ->delay--;
		if(OBJ->Weap&&abs(adx)+abs(ady)<OBJ->BestDist+2048&&!OBJ->delay){
			OBJ->delay=OBJ->MaxDelay;
			OBJ->LoadCurAnm(1);
			//OBJ->Direction=
			CreateUniExObj(OBJ->Weap,(wcx>>2),(wcy>>2),64,OBJ->NMask,OBJ,OB->x,OB->y,OB->Index);
			OBJ->MakeDamage(0,OBJ->Ref.General->LifeShotLost,OB);
		};
	}else{
		if(OBJ->InLineCom)OBJ->FreeAsmLink();
		Order1* Loc1=OBJ->LocalOrder->NextOrder;
		OBJ->FreeOrdBlock(OBJ->LocalOrder);
		OBJ->LocalOrder=Loc1;
		OBJ->FlyAttackMode=0;			
		return;
	};
};
void SetFlyMarkers(){
	for(int i=0;i<MaxObj;i++){
		OneObject* OB=Group[i];
		if(OB&&OB->Media==2&&!OB->Sdoxlo)
		FlyMops[OB->RealY>>9][OB->RealX>>9]=OB->Index;
	};
};
void ClearFlyMarkers(){
	for(int i=0;i<MaxObj;i++){
		OneObject* OB=Group[i];
		if(OB&&OB->Media==2)
		FlyMops[OB->RealY>>9][OB->RealX>>9]=0xFFFF;
	};
};
void UFOLink(OneObject* OB);
void OneObject::MakeMeUFO(){
	if(!cpbMoving)return;
	ClearOrders();
	NMask=255;
	if(int(InLineCom)){
		FreeAsmLink();
	};
	Order1* Or1=GetOrdBlock();
	if(!int(Or1))return;
	Or1->NextOrder=NULL;
	Or1->OrderType=92;
	Or1->OrderTime=0;
	Or1->DoLink=&UFOLink;
	Or1->info.UFO.xd=msx>>1;
	Or1->info.UFO.yd=msy>>1;
	Or1->info.UFO.time=0;
	Or1->info.UFO.BuildID=0xFFFF;
	Or1->info.UFO.BSN=0xFFFF;
	Order1* LOR=LocalOrder;
	LocalOrder=Or1;
	//OrderReport=NULL;
	//MessageKind=0;
	//Sender=0xFFFF;
	PrioryLevel=255;
};
void UFOLink(OneObject* OB){
	word BuildID=OB->LocalOrder->info.UFO.BuildID;
	byte Time=OB->LocalOrder->info.UFO.time;
	if(BuildID==0xFFFF){
		if(Time){
			OB->LocalOrder->info.UFO.time--;
		}else{
			OB->LocalOrder->info.UFO.time=50;
			OB->LocalOrder->info.UFO.xd=8+((int(rando())*(msx-16))>>15);
			OB->LocalOrder->info.UFO.yd=8+((int(rando())*(msy-16))>>15);
			OB->DestX=(int(OB->LocalOrder->info.UFO.xd)<<9)+256;
			OB->DestY=(int(OB->LocalOrder->info.UFO.yd)<<9)+256;
		};
	}else{
		word BID=OB->LocalOrder->info.UFO.BuildID;
		word BSN=OB->LocalOrder->info.UFO.BSN;
		OneObject* FOB=Group[BID];
		if(FOB&&FOB->Serial==BSN){
			int xd=(int(FOB->x)<<9)+256+512;
			int yd=(int(FOB->y)<<9)+256+512;
			int dstt=abs(xd-OB->RealX)+abs(yd-OB->RealY);
			if(dstt<64){
				//Transforming
				Visuals* GO=(Visuals*)(FOB->Nat->Mon[FOB->Ref.General->IDforUFO]);
				OneObject* G=FOB;
				CreateStrangeObject(11,OB->NNUM,(OB->RealX>>9)+1,(OB->RealY>>9)+1,OB->Index);
				FOB->DefaultSettings(GO);
				G->NIndex=FOB->Ref.General->IDforUFO;
				G->Teleport=false;
				G->capTeleport=GO->Teleport;
				G->VisRadius=GO->VisRadius;
				G->VisSpots=GO->VisSpots;
				G->SpotType=GO->SpotType;
				G->SpotSize=GO->SpotSize;
				G->DangerZone=GO->DangerZone;
				G->NoSearchVictim=GO->NoSearchVictim;
				G->NoAnswer=GO->NoAnswer;
				G->NeedNoHelp=GO->NeedNoHelp;
				G->Ready=true;
				G->wepX=GO->wepX;
				G->wepY=GO->wepY;
				G->MaxDelay=GO->delay;
				G->delay=0;
				G->NearBase=0xFFFF;
				G->capBase=GO->cpbBase;
				G->RStage=0;
				G->RType=0;
				G->RAmount=0;
				G->AnmGoKind=1;
				G->capBuild=GO->cpbBuild;
				G->GroupIndex=NULL;
				G->cpbMoving=GO->cpbMoving;
				G->AbRes=0;
				if(GO->AGold)G->AbRes|=2;
				if(GO->AWood)G->AbRes|=4;
				if(!GO->SizeX)GO->SizeX=1;
				if(!GO->SizeY)GO->SizeY=1;
				G->Lx=GO->SizeX;
				G->Ly=GO->SizeY;
				G->TempFlag=false;
				G->Mobilised=false;
				G->Wars=NULL;
				G->Selected=false;
				G->Borg=false;
				G->Life=GO->info.Basic.MaxLife;
				G->MaxLife=GO->info.Basic.MaxLife;
				G->Ref.Visual=GO;
				G->Push=false;
				memset(&(G->ARegs),0,sizeof G->ARegs);
				G->LoadAnimation(0,0,0);
				G->LoadCurAnm(0);
				G->LocalOrder=NULL;
				G->Attack=false;
				//G->OrderReport=NULL;
				//G->MessageFlags=0;
				G->PrioryLevel=0;
				//G->MessageKind=0;
				G->Ticks=0;
				G->TicksPerChange=10;
				G->Wait=0;
				G->Addx=0;
				G->Addy=0;
				G->Npush=0;
				G->StandTime=100;
				G->Sdoxlo=false;
				G->Weap=GO->Weap;
			}else{
				OB->DestX=xd;
				OB->DestY=yd;
			};
		}else{
			OB->LocalOrder->info.UFO.BuildID=0xFFFF;
			OB->LocalOrder->info.UFO.BSN=0xFFFF;
			OB->LocalOrder->info.UFO.time=0;
		};
	};
};
void ProcessUFO(){
	//find buildings to produce
	for(int i=0;i<MAXOBJECT;i++){
		OneObject* OB=Group[i];
		if(OB&&OB->UFOTrans&&OB->Ready){
			//Let us find UFO
			for(int j=0;j<MaxObject;j++){
				OneObject* UOB=Group[j];
				if(UOB&&UOB->Ref.General->UFO){
					Order1* OR1=UOB->LocalOrder;
					if(OR1&&OR1->OrderType==92&&OR1->info.UFO.BuildID==0xFFFF){
						OR1->info.UFO.BuildID=i;
						OR1->info.UFO.BSN=OB->Serial;
						OB->UFOTrans=false;
						goto uuu;
					};
				};
			};
		};
uuu:;
	};
};