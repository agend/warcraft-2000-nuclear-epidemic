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
#include "TopZones.h"
#include <assert.h>
#include "MapEdit.h"
#include "3DSurf.h"
#include "GSound.h"
Nation NATUR;
void FindEmptyPoint(byte* x,byte* y);
byte rtmap[256][256];//resource type map
byte ramap[256][256];//resource amount map
int RESRC[8][8];
int RESADD[8][8];
OneSlide OILOC;
word OILMAP[128][128];
const int drrn[9]={7,6,5,0,0,4,1,2,3};
class ResDiscr{
public:
	bool Enabled;
	char Name[32];
	byte Amount;
	byte MotionKind;
	byte WorkKind;
	byte Portion;
	int AnmStandKind;
	int AnmGoKind;
	int Multi;
	int Tiles [16][4];
	ResDiscr();
};
byte TRType[256];
byte TRMask[256];
ResDiscr::ResDiscr(){
	Enabled=false;
	Amount=0;
	Portion=0;
	MotionKind=0;
	WorkKind=0;
	Name[0]=0;
	AnmStandKind=0;
	AnmGoKind=1;
	memset(&TRType,0,256);
	memset(&TRMask,0,256);
};
ResDiscr RDS[8];
void Errr(LPCSTR s)
{
	MessageBox(hwnd,s,"Nature loading failed...",MB_ICONWARNING|MB_OK);
	assert(false);
};
void LoadRDS(){
	//NATUR.CreateNation("NATUR",255,255);
	//OILOC=*NATUR.Mon[0]->lpFAnim->Anm->Movie;
	memset(&OILMAP[0][0],0,sizeof OILMAP);
	memset(&RESRC[0][0],0,64*4);
	memset(&RESADD[0][0],0,64*4);
	for(int h=0;h<8;h++){
		RESRC[h][1]=5000;
	};
	FILE* f=fopen("nres.dat","r");
	if(!int(f)){
		Errr("Could not find NRES.DAT");
		return;
	};
	bool eoff=false;
	char s1[16];
	int	z0,z1,z2,z3,z4,z5,z6,z7;
	int	mode=0;
	do{
		switch(mode){
		case 0:
			fscanf(f,"%s",s1);
			if(!strcmp(s1,"[CELLS]"))mode=1;

			else{
				fscanf(f,"%d%d",&z0,&z1);
				strcpy(RDS[z0].Name,s1);
				RDS[z0].Enabled=true;
				for(z3=0;z3<8;z3++)RESRC[z3][z0]=z1;
			};
			break;
		case 1:
			fscanf(f,"%s%d%d%d%d%d%d%d%d",s1,&z0,&z1,&z2,&z3,&z4,&z5,&z6,&z7);
			if(!strcmp(s1,"[NEW]")){
				RDS[z0].Enabled=true;
				RDS[z0].MotionKind=z1;
				RDS[z0].WorkKind=z2;
				RDS[z0].Amount=z3;
				RDS[z0].Portion=z4;
				RDS[z0].Multi=z5;
				RDS[z0].AnmStandKind=z6;
				RDS[z0].AnmGoKind=z7;
				for(z5=0;z5<16;z5++){
					fscanf(f,"%d%d%d%d",&z1,&z2,&z3,&z4);
					RDS[z0].Tiles[z5][0]=z1;
					RDS[z0].Tiles[z5][1]=z2;
					RDS[z0].Tiles[z5][2]=z3;
					RDS[z0].Tiles[z5][3]=z4;
					TRType[z1]=byte(z0);
					TRMask[z1]=byte(z5);
					TRType[z2]=byte(z0);
					TRMask[z2]=byte(z5);
					TRType[z3]=byte(z0);
					TRMask[z3]=byte(z5);
					TRType[z4]=byte(z0);
					TRMask[z4]=byte(z5);
				};
			}else mode=2;
			break;
		};
	}while(mode!=2);
	fclose(f) ;
};
void DefResource(int i,int j){
	if(i>1&&j>1&&i<msx&&j<msy){
		int til=int(tmap[i-1][j-1]);
			byte msk=TRMask[til];
			byte kind=TRType[til];
			rtmap[j][i]=0;
			rtmap[j+1][i]=0;
			rtmap[j][i+1]=0;
			rtmap[j+1][i+1]=0;
			ramap[j][i]=0;
			ramap[j+1][i]=0;
			ramap[j][i+1]=0;
			ramap[j+1][i+1]=0;
			if(kind!=0){
				if(msk&1){
					rtmap[j][i]=kind;
					ramap[j][i]=RDS[kind].Amount-(rando()&31);
				};
				if(msk&2){
					rtmap[j][i+1]=kind;
					ramap[j][i+1]=RDS[kind].Amount-(rando()&31);
				};
				if(msk&4){
					rtmap[j+1][i]=kind;
					ramap[j+1][i]=RDS[kind].Amount-(rando()&31);
				};
				if(msk&8){
					rtmap[j+1][i+1]=kind;
					ramap[j+1][i+1]=RDS[kind].Amount-(rando()&31);
				};
			};
	};
};
void CreateResMap(){
	memset(&rtmap[0][0],0,65536);
	memset(&ramap[0][0],0,65536);
	for(int i=1;i<msx;i++)
		for(int j=1;j<msy;j++){
			int til=int(tmap[i-1][j-1]);
			byte msk=TRMask[til];
			byte kind=TRType[til];
			if(kind!=0){
				if(msk&1){
					rtmap[j][i]=kind;
					ramap[j][i]=RDS[kind].Amount-(rando()&31);
				};
				if(msk&2){
					rtmap[j][i+1]=kind;
					ramap[j][i+1]=RDS[kind].Amount-(rando()&31);
				};
				if(msk&4){
					rtmap[j+1][i]=kind;
					ramap[j+1][i]=RDS[kind].Amount-(rando()&31);
				};
				if(msk&8){
					rtmap[j+1][i+1]=kind;
					ramap[j+1][i+1]=RDS[kind].Amount-(rando()&31);
				};
			};
		};
};
void CreateSurface(byte x,byte y,byte rk){
	byte dd=0;
	if(ramap[y][x])dd+=1;
	if(ramap[y][x+1])dd+=2;
	if(ramap[y+1][x])dd+=4;
	if(ramap[y+1][x+1])dd+=8;
	//byte rk=rtmap[y][x];
	int til=RDS[rk].Tiles[dd][rando()&3];
	if(dd==0){
		TrMap[y][x]=0;
		LLock[y][x]=0;
		DecLock(x,y);
		PutZCell(x,y);
	};
	word tt=tmap[x-1][y-1];
	if(tt!=til){
		tmap[x-1][y-1]=byte(til);
		ClearRender(x,y);
	};
};
byte TakeResource(byte x,byte y,byte rk){
	byte rt=rtmap[y][x]; 
	byte ra=ramap[y][x];
	if(ra>0){
		ramap[y][x]--;
		if(ra==1){
			if(x>1&&y>1)CreateSurface(x-1,y-1,rk);
			if(x>1)CreateSurface(x-1,y,rk);
			if(y>1)CreateSurface(x,y-1,rk);
			CreateSurface(x,y,rk);
			if(rk==1)CMap[y][x]=1;
			else CMap[y][x]=0;
			rtmap[y][x]=0;
			//PutZCell(x,y);
		};
		return 1;
	};
	return 0;
};
byte GetResourceKind(byte x,byte y){
	if(ramap[y][x])return rtmap[y][x];
	if(ramap[y+1][x])return rtmap[y+1][x];
	if(ramap[y][x+1])return rtmap[y][x+1];
	if(ramap[y+1][x+1])return rtmap[y+1][x+1];
	return 0;
};
int TakeResFromCell(byte x,byte y){
	byte rk=GetResourceKind(x,y);
	if(rk==0)return 0;
	int rr=TakeResource(x,y,rk);
	rr+=TakeResource(x,y+1,rk);
	rr+=TakeResource(x+1,y,rk);
	rr+=TakeResource(x+1,y+1,rk);
	return rr;
};
//Pseudo-local variables
byte zx,zy,sx,sy;
bool RAble(byte x,byte y,ZType zz){
	if(zx<x)sx=x-1;else sx=x;
	if(zy<y)sy=y-1;else sy=y;
	if(RZAMap[sy][sx]!=zz)return false;
	FindEmptyPoint(&sx,&sy);
	if(sx) return true;else return false;
};
extern int COUNTER;
bool FindResource(byte* xx,byte* yy,byte Radius,byte kind,ZType zz){
	byte x1=*xx;
	byte y1=*yy;
	zx=x1;
	zy=y1;
	if(x1==0||y1==0)return false;
	byte x0=x1;
	byte y0=y1;
	x1++;
	y1++;
	byte xr=0;
	byte yr=0;
	byte r=Radius;
	byte* pt;
	while(r>0){
		byte dx=x1-x0;
		byte dy=y1-y0;
		xr=x0;yr=y0;
		pt=(byte*)(int(rtmap)+x0+(int(y0)<<8));
		for(byte i=0;i<dx;i++){
			if(*pt==kind&&RAble(xr,yr,zz))goto FRes;
			pt++;
			xr++;
		};
		for(i=0;i<dy;i++){
			if(*pt==kind&&RAble(xr,yr,zz))goto FRes;
			pt+=256;
			yr++;
		};
		for(i=0;i<dx;i++){
			if(*pt==kind&&RAble(xr,yr,zz))goto FRes;
			pt--;
			xr--;
		};
		for(i=0;i<dy;i++){
			if(*pt==kind&&RAble(xr,yr,zz))goto FRes;
			pt-=256;
			yr--;
		};
		if(x0>1)x0--;
		if(y0>1)y0--;
		if(x1<msx)x1++;
		if(y1<msy)y1++;
		r--;
	};
	return false;
FRes:
	*xx=sx;
	*yy=sy;
	return true;
};
/*
bool CheckNearRes(byte* xx,byte* yy,byte kind){
	byte x=*xx;
	byte y=*yy;
	if(x==0||y==0)return false;
	__asm{
		xor		eax,eax
		mov		al,x
		mov		ah,y
		mov		bl,kind
		add		eax,offset rtmap
		cmp		[eax-256],bl
		jne		qw1
		dec		y
		jmp		qw_end
qw1:	cmp		[eax-255],bl
		jne		qw2
		dec		y
		jmp		qw_end
qw2:	cmp		[eax+2],bl
		jne		qw3
		inc		x
		jmp		qw_end
qw3:	cmp		[eax+258],bl
		jne		qw4
		inc		x
		jmp		qw_end
qw4:	cmp		[eax-1],bl
		jne		qw5
		dec		x
		jmp		qw_end
qw5:	cmp		[eax+255],bl
		jne		qw6
		dec		x
		jmp		qw_end
qw6:	cmp		[eax+512],bl
		jne		qw7
		inc		y
		jmp		qw_end
qw7:	cmp		[eax+513],bl
		jne		qw8
		inc		y
		jmp		qw_end
qw8:	cmp		[eax-256],bl
		jne		qw9
		dec		x
		jmp		qw_end
qw9:	cmp		[eax-257],bl
		jne		qw10
		dec		x
		dec		y
		jmp		qw_end
qw10:	cmp		[eax-254],bl
		jne		qw11
		inc		x
		dec		y
		jmp		qw_end
qw11:	cmp		[eax+514],bl
		jne		qw12
		inc		x
		inc		y
		jmp		qw_end
qw12:	cmp		[eax+511],bl
		jne		qw13
		//dec		x
		inc		y
		jmp		qw_end
qw13:	mov		x,255
qw_end:
	};
	*xx=x;
	*yy=y;
	if(x!=255)return true; else return false;
};*/
bool CheckNearRes(byte* xx,byte* yy,byte kind){
	byte x=*xx;
	byte y=*yy;
	if(x==0||y==0)return false;
	__asm{
		xor		eax,eax
		mov		al,x
		mov		ah,y
		mov		bl,kind
		add		eax,offset rtmap
		cmp		[eax-256],bl
		jne		qw1
		dec		y
		jmp		qw_end
qw1:	cmp		[eax-255],bl
		jne		qw2
		dec		y
		jmp		qw_end
qw2:	cmp		[eax+2],bl
		jne		qw3
		inc		x
		jmp		qw_end
qw3:	cmp		[eax+258],bl
		jne		qw4
		inc		x
		jmp		qw_end
qw4:	cmp		[eax-1],bl
		jne		qw5
		dec		x
		jmp		qw_end
qw5://	cmp		[eax+255],bl
	//	jne		qw6
	//	dec		x
	//	jmp		qw_end
qw6:	cmp		[eax+512],bl
		jne		qw7
		inc		y
		jmp		qw_end
qw7:	cmp		[eax+513],bl
		jne		qw8
		inc		y
		jmp		qw_end
qw8://	cmp		[eax-256],bl
	//	jne		qw9
	//	dec		x
	//	jmp		qw_end
qw9://	cmp		[eax-257],bl
	//	jne		qw10
	//	dec		x
	//	dec		y
	//	jmp		qw_end
qw10://	cmp		[eax-254],bl
	//	jne		qw11
	//	inc		x
	//	dec		y
	//	jmp		qw_end
qw11://	cmp		[eax+514],bl
	//	jne		qw12
	//	inc		x
	//	inc		y
	//	jmp		qw_end
qw12://	cmp		[eax+511],bl
	//	jne		qw13
		//dec		x
	//	inc		y
	//	jmp		qw_end
qw13:	mov		x,255
qw_end:
	};
	*xx=x;
	*yy=y;
	if(x!=255)return true; else return false;
};
word OneObject::FindNearestBase(){
	OneObject* OB;
	byte ResID=LocalOrder->info.TakeRes.ResID;
	byte msk=1<<ResID;
	int dist=0xFFFF;
	word OID=0xFFFF;
	for(int i=0;i<MAXOBJECT;i++){
		OB=Group[i];
		if(int(OB)&&(OB->AbRes&msk)&&OB->NNUM==NNUM){
			int dist1=abs(int(x)-int(OB->x))+
				abs(int(y)-int(OB->y));
			if(dist1<dist){
				OID=i;
				dist=dist1;
			};
		};
	};
	return OID;
};
void TakeResLink(OneObject* OBJ);
bool OneObject::TakeResource(byte x,byte y,byte ResID,int Prio){
	if(Prio<PrioryLevel)return false;
	if(!capBuild)return false;
	if(capBuilding)return false;
	//if(RAmount>0&&RType!=ResID)return false;
	RType=ResID;
	RStage=0;
	Order1* Or1=GetOrdBlock();
	if(!int(Or1))return false;
	Or1->PrioryLevel=Prio&127;
	Or1->NextOrder=LocalOrder;
	Or1->OrderType=238;//Take resource
	Or1->OrderTime=0;
	Or1->DoLink=&TakeResLink;
	Or1->info.TakeRes.x=x;
	Or1->info.TakeRes.y=y;
	Or1->info.TakeRes.ResID=ResID;
	Order1* LOR=LocalOrder;
	if(int(InLineCom))FreeAsmLink();
	LocalOrder=Or1;
	//OrderReport=NULL;
	//MessageKind=0;
	//Sender=0xFFFF;
	PrioryLevel=Prio&127;
	return true;
};	
void TakeResLink(OneObject* OBJ){
	assert(!OBJ->AskMade/*TakeResLink*/);
	byte x,y,ResID;
	int dist;
	switch(OBJ->RStage){
		case 0://go to the point and search the resource
			x=OBJ->LocalOrder->info.TakeRes.x;
			y=OBJ->LocalOrder->info.TakeRes.y;
			ResID=OBJ->LocalOrder->info.TakeRes.ResID;
			if(RDS[ResID].Portion<=OBJ->RAmount){
				OBJ->RStage=1;
				OBJ->NearBase=OBJ->FindNearestBase();
				if(OBJ->NearBase==0xFFFF){
					if(int(OBJ->LocalOrder)){
						Order1* Or1=OBJ->LocalOrder->NextOrder;
						OBJ->FreeOrdBlock(OBJ->LocalOrder);
						OBJ->LocalOrder=Or1;
						OBJ->Important=false;
					};
				};
				return;
			};
			//далеко ль путь держим?
			dist=abs(int(x)-int(OBJ->x))+
				abs(int(y)-int(OBJ->y));
			if(dist<10){
				byte xx1=OBJ->x;
				byte yy1=OBJ->y;
				if(CheckNearRes(&xx1,&yy1,ResID)){
					byte dir=drrn[(xx1-OBJ->x+1)*3+yy1-OBJ->y+1];
					if(dir>7){
						dir=0;
					};
					OBJ->LoadAnimation(0,OBJ->AnmStandKind,0);
					OBJ->Direction=dir;
					OBJ->LoadAnimation(2,2,0);
					OBJ->LoadCurAnm(2);
					OBJ->isMoving=false;
					OBJ->RAmount+=TakeResFromCell(xx1,yy1);
					if(ResID==1){
						//gold
						AddWorkEffect(OBJ->x,OBJ->y,OBJ->Ref.General->GoldSound);
					}else{
						//wood
						AddWorkEffect(OBJ->x,OBJ->y,OBJ->Ref.General->TreeSound);
					};
					TrMap[OBJ->y][OBJ->x]=1;
				}else{
					if(LLock[y][x]){
						xx1=OBJ->x;
						yy1=OBJ->y;
						if(FindResource(&xx1,&yy1,120,ResID,TZones[OBJ->y][OBJ->x])){
							OBJ->LocalOrder->info.TakeRes.x=xx1;
							OBJ->LocalOrder->info.TakeRes.y=yy1;
							OBJ->FreeAsmLink();
							OBJ->CreateSimplePath(xx1,yy1);
						};
					}else if(!int(OBJ->InLineCom)){
						OBJ->FreeAsmLink();
						OBJ->CreateSimplePath(x,y);
					};
				};
			}else if(!int(OBJ->InLineCom)){
				OBJ->FreeAsmLink();
				OBJ->CreateSimplePath(x,y);
			};
			break;
		default:
			if(OBJ->RAmount==0)OBJ->RStage=0;
			if(OBJ->NearBase==0xFFFF){
			OBJ->NearBase=OBJ->FindNearestBase();
				if(OBJ->NearBase==0xFFFF){
					if(int(OBJ->LocalOrder)){
						Order1* Or1=OBJ->LocalOrder->NextOrder;
						OBJ->FreeOrdBlock(OBJ->LocalOrder);
						OBJ->LocalOrder=Or1;
						OBJ->Important=false;
						return;
					};
				};
			};
			word OID=OBJ->NearBase;
			OneObject* OB=Group[OID];
			if(!int(OB)||OB->Sdoxlo){
				OBJ->NearBase=0xFFFF;
				return;
			};
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
			int rk=OBJ->RType;
					OBJ->AnmStandKind=RDS[rk].AnmStandKind;
					OBJ->AnmGoKind=RDS[rk].AnmGoKind;
			if(dx>dy)dst=dx;
				else dst=dy;
			if(dst<2){
				int ADDRESRC=int(OBJ->RAmount)*RDS[OBJ->RType].Multi+
					RESADD[OBJ->NNUM][OBJ->RType];
				OBJ->Nat->GetTax(OBJ->RType,ADDRESRC);
				OBJ->RAmount=0;
				OBJ->RStage=0;
				OBJ->AnmStandKind=0;
				OBJ->AnmGoKind=1;
			}else if(!int(OBJ->InLineCom)){
				OBJ->FreeAsmLink();
				OBJ->CreateSimplePath(xo,yo);
			};
			
	};
};
word OneObject::FindNearestPort(){
	OneObject* OB;
	GeneralObject* GO;
	int dist=0xFFFF;
	word OID=0xFFFF;
	for(int i=0;i<MAXOBJECT;i++){
		OB=Group[i];
		if(OB)GO=OB->Ref.General;
		if(int(OB)&&GO->OnCost&&OB->NNUM==NNUM){
			int dist1=DistTo(OB->x,OB->y);
			if(dist1<dist){
				OID=i;
				dist=dist1;
			};
		};
	};
	return OID;
};
void BuildOnWaterLink(OneObject* OBJ);
bool OneObject::BuildOnWater(word OID,int Prio){
	OneObject* OB=Group[OID];
	if(!int(OB))return false;
	if(!OB->OnWater)return false;
	if(Prio<PrioryLevel)return false;
	if(!capBuild)return false;
	if(capBuilding)return false;
	//if(RAmount>0&&RType!=ResID)return false;
	RStage=0;
	Order1* Or1=GetOrdBlock();
	if(!int(Or1))return false;
	Or1->PrioryLevel=Prio&127;
	Or1->NextOrder=LocalOrder;
	Or1->OrderType=39;//Build on water
	Or1->OrderTime=0;
	Or1->DoLink=&BuildOnWaterLink;
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
void BuildOnWaterLink(OneObject* OBJ){
	byte x,y,ResID;
	word OI;
	int dist;
	byte xd;
	byte yd;
	word dstx;
	word dst1;
	OneObject* OB;
	switch(OBJ->RStage){
		case 0://go to the object to build
			OI=OBJ->LocalOrder->info.MoveToObj.ObjIndex;
			OB=Group[OI];
			if(OB->Life==OB->MaxLife){
				Order1* Or1=OBJ->LocalOrder->NextOrder;
				OBJ->FreeOrdBlock(OBJ->LocalOrder);
				OBJ->LocalOrder=Or1;
				OBJ->Important=false;
				if(OI==OBJ->Nat->OilBaseID){
					OBJ->GetOil(OI,16);
				};
				return;
			};
			x=OB->x+1;
			y=OB->y+1;
			xd=x;
			yd=y;
			dstx=1000;
			if(!WLock[y-2][x]){
				dst1=OBJ->DistTo(x,y-2);
				if(dst1<dstx){
					dstx=dst1;
					xd=x;
					yd=y-2;
				};
			};
			if(!WLock[y+2][x]){
				dst1=OBJ->DistTo(x,y+2);
				if(dst1<dstx){
					dstx=dst1;
					xd=x;
					yd=y+2;
				};
			};
			if(!WLock[y][x-2]){
				dst1=OBJ->DistTo(x-2,y);
				if(dst1<dstx){
					dstx=dst1;
					xd=x-2;
					yd=y;
				};
			};
			if(!WLock[y][x+2]){
				dst1=OBJ->DistTo(x+2,y);
				if(dst1<dstx){
					dstx=dst1;
					xd=x+2;
					yd=y;
				};
			};
			if(OBJ->DistTo(x,y)<=2){
				OB->NextStage();
				OB->NextStage();
				if(OB->Life==OB->MaxLife){
					Order1* Or1=OBJ->LocalOrder->NextOrder;
					OBJ->FreeOrdBlock(OBJ->LocalOrder);
					OBJ->LocalOrder=Or1;
					OBJ->Important=false;
					return;
				};
				OBJ->RStage=1;
				OBJ->FindNearestPort();
				return;
			};
			OBJ->FreeAsmLink();
			OBJ->CreateWPath(xd,yd);
			break;
		default:
			if(OBJ->NearBase==0xFFFF)
				OBJ->NearBase=OBJ->FindNearestPort();
			if(OBJ->NearBase==0xFFFF){
				if(int(OBJ->LocalOrder)){
					Order1* Or1=OBJ->LocalOrder->NextOrder;
					OBJ->FreeOrdBlock(OBJ->LocalOrder);
					OBJ->LocalOrder=Or1;
					OBJ->Important=false;
					return;
				};
			};
			word OID=OBJ->NearBase;
			OneObject* OB=Group[OID];
			if(!int(OB)||OB->Sdoxlo||!OB->capBase){
				OBJ->NearBase=0xFFFF;
				return;
			};
			x=(OB->x+1)&254;
			y=(OB->y+1)&254;
			xd=x;
			yd=y;
			dstx=1000;
			if(!WLock[y-2][x]){
				dst1=OBJ->DistTo(x,y-2);
				if(dst1<dstx){
					dstx=dst1;
					xd=x;
					yd=y-2;
				};
			};
			if(!WLock[y+2][x]){
				dst1=OBJ->DistTo(x,y+2);
				if(dst1<dstx){
					dstx=dst1;
					xd=x;
					yd=y+2;
				};
			};
			if(!WLock[y][x-2]){
				dst1=OBJ->DistTo(x-2,y);
				if(dst1<dstx){
					dstx=dst1;
					xd=x-2;
					yd=y;
				};
			};
			if(!WLock[y][x+2]){
				dst1=OBJ->DistTo(x+2,y);
				if(dst1<dstx){
					dstx=dst1;
					xd=x+2;
					yd=y;
				};
			};
			byte x0=OB->x+1;
			byte y0=OB->y+1;
			OBJ->AnmStandKind=0;
			OBJ->AnmGoKind=1;
			if(OBJ->DistTo(x0,y0)<=3)
				OBJ->RStage=0;
			else OBJ->CreateWPath(xd,yd);
	};
};
void GetOilLink(OneObject* OBJ);
bool OneObject::GetOil(word OID,int Prio){
	OneObject* OB=Group[OID];
	if(!int(OB))return false;
	if(!OB->OnWater)return false;
	if(Prio<PrioryLevel)return false;
	if(!capBuild)return false;
	if(capBuilding)return false;
	//if(RAmount>0&&RType!=ResID)return false;
	RStage=0;
	Order1* Or1=GetOrdBlock();
	if(!int(Or1))return false;
	Or1->PrioryLevel=Prio&127;
	Or1->NextOrder=LocalOrder;
	Or1->OrderType=39;//Build on water
	Or1->OrderTime=0;
	Or1->DoLink=&GetOilLink;
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
void GetOilLink(OneObject* OBJ){
	byte x,y,ResID;
	word OI;
	int dist;
	byte xd,x0,y0,x1,y1,xx,yy;
	byte yd;
	word dstx;
	int uu=32;
	word dst1;
	OneObject* OB;
	switch(OBJ->RStage){
		case 0://go to the object to build
			OI=OBJ->LocalOrder->info.MoveToObj.ObjIndex;
			OB=Group[OI];
			//if(OB->Life==OB->MaxLife){
			//	Order1* Or1=OBJ->LocalOrder->NextOrder;
			//	OBJ->FreeOrdBlock(OBJ->LocalOrder);
			//	OBJ->LocalOrder=Or1;
			//	OBJ->Important=false;
			//	return;
			//};
			x=OB->x+1;
			y=OB->y+1;
			xd=x;
			yd=y;
			dstx=1000;
			if(!WLock[y-2][x]){
				dst1=OBJ->DistTo(x,y-2);
				if(dst1<dstx){
					dstx=dst1;
					xd=x;
					yd=y-2;
				};
			};
			if(!WLock[y+2][x]){
				dst1=OBJ->DistTo(x,y+2);
				if(dst1<dstx){
					dstx=dst1;
					xd=x;
					yd=y+2;
				};
			};
			if(!WLock[y][x-2]){
				dst1=OBJ->DistTo(x-2,y);
				if(dst1<dstx){
					dstx=dst1;
					xd=x-2;
					yd=y;
				};
			};
			if(!WLock[y][x+2]){
				dst1=OBJ->DistTo(x+2,y);
				if(dst1<dstx){
					dstx=dst1;
					xd=x+2;
					yd=y;
				};
			};
			if(OBJ->DistTo(x,y)<=2&&OB->Ready){
				OB->AnmStandKind=4;
				OB->LoadAnimation(0,4,0);
				OB->LoadCurAnm(0);
				Mops[OBJ->y][OBJ->x]=0xFFFF;
				WLock[OBJ->y][OBJ->x]=0;
				WMap[OBJ->y][OBJ->x]=0;
				OBJ->x=OB->x;
				OBJ->y=OB->y;
				OB->Ready=false;
				OB->Wait=100;
				OBJ->RStage=2;
				return;
			};
			OBJ->FreeAsmLink();
			OBJ->CreateWPath(xd,yd);
			break;
		case 2:
			OI=OBJ->LocalOrder->info.MoveToObj.ObjIndex;
			OB=Group[OI];
			if(OB->Life==0){
				Order1* Or1=OBJ->LocalOrder->NextOrder;
				OBJ->FreeOrdBlock(OBJ->LocalOrder);
				OBJ->LocalOrder=Or1;
				OBJ->Important=false;
				OBJ->Die();
				return;
			};
			if(OB->Wait){
				OB->Wait--;
				return;
			};
			x0=((OB->x+1)&254)-2;
			y0=((OB->y+1)&254)-2;
			x1=(x0+4);
			y1=(y0+4);
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
zzz:
			OB->Ready=true;
			OB->AnmStandKind=0;
			OB->LoadAnimation(0,0,0);
			OB->LoadCurAnm(0);
			OBJ->x=xx;
			OBJ->y=yy;
			Mops[yy][xx]=OBJ->Index;
			WLock[yy][xx]=1;
			OBJ->NearBase=OBJ->FindNearestPort();
			OBJ->RStage=1;
			break;
		default:
			if(OBJ->NearBase==0xFFFF)
				OBJ->NearBase=OBJ->FindNearestPort();
			if(OBJ->NearBase==0xFFFF){
				if(int(OBJ->LocalOrder)){
					Order1* Or1=OBJ->LocalOrder->NextOrder;
					OBJ->FreeOrdBlock(OBJ->LocalOrder);
					OBJ->LocalOrder=Or1;
					OBJ->Important=false;
					return;
				};
			};
			word OID=OBJ->NearBase;
			OneObject* OB=Group[OID];
			if(!int(OB)||OB->Sdoxlo){
				OBJ->NearBase=0xFFFF;
				return;
			};
			x=(OB->x+1)&254;
			y=(OB->y+1)&254;
			xd=x;
			yd=y;
			dstx=1000;
			if(!WLock[y-2][x]){
				dst1=OBJ->DistTo(x,y-2);
				if(dst1<dstx){
					dstx=dst1;
					xd=x;
					yd=y-2;
				};
			};
			if(!WLock[y+2][x]){
				dst1=OBJ->DistTo(x,y+2);
				if(dst1<dstx){
					dstx=dst1;
					xd=x;
					yd=y+2;
				};
			};
			if(!WLock[y][x-2]){
				dst1=OBJ->DistTo(x-2,y);
				if(dst1<dstx){
					dstx=dst1;
					xd=x-2;
					yd=y;
				};
			};
			if(!WLock[y][x+2]){
				dst1=OBJ->DistTo(x+2,y);
				if(dst1<dstx){
					dstx=dst1;
					xd=x+2;
					yd=y;
				};
			};
			byte x0=OB->x+1;
			byte y0=OB->y+1;
			OBJ->AnmStandKind=0;
			OBJ->AnmGoKind=1;
			if(OBJ->DistTo(x0,y0)<=3){
				OBJ->RStage=0;
				RESRC[OBJ->NNUM][3]+=10;
			}else OBJ->CreateWPath(xd,yd);
	};
};
extern int RealLx;
void ShowRMap(){
	//int x=smapx+smaplx*32-140;
	int x=RealLx-200;
	//if(MiniMode)x=RealLx-200;//smapx+(smaplx<<4)-140;
	int y=smapy;
	char gg[100];
	for(int i=0;i<3;i++){
		if(RDS[i].Enabled){
			sprintf(gg,"%s : %d",RDS[i].Name,RESRC[MyNation][i]);
			ShowString(x,y,gg,&rlf_s);
			y+=16;
		};
	};
	//sprintf(gg,"Cash: %d",NATIONS[MyNation].CasheSize);
	//ShowString(x,y,gg,&rlf_s);
	//y+=16;
	return;
	char ggg[32];
	for(i=0;i<smaplx;i++)
		for(int j=0;j<smaply;j++){
			byte res=CMap[j+mapy-1][i+mapx-1];
			if(res!=255){
				sprintf(ggg,"%d",RZAMap[j+mapy][i+mapx]);//[res);
				ShowString(smapx+32*i-8+16,smapy+32*j-8+16,ggg,&rlf_s);
				//Hline(smapx+32*i-3,smapy+32*j,smapx+32*i+3,res);
				//Vline(smapx+32*i,smapy+32*j-3,smapy+32*j+3,res);
			};
		};
};
bool CheckCost(byte NI,byte ResID,word Amount){
		if(ResID==255)return true;
		if(RESRC[NI][ResID]>=Amount)return true;
		else return false;
};
bool CheckOilSpot(byte x,byte y){
	if(x>2&&x<msx-2&&y>2&&y<msy-2){
		if(!(WLock[y][x]||WLock[y+1][x]||WLock[y][x+1]||
			WLock[y-1][x]||WLock[y][x+1]||OILMAP[y>>1][x>>1]))
			return true;
	};
	return false;
};
void CreateOilSpot(byte x,byte y){
	OILMAP[y>>1][x>>1]=60000;
};
void ShowOilSpot(int x,int y){
	ShowRLCItem(x+OILOC.dx,y+OILOC.dy,&MImage[OILOC.FileID],OILOC.spr,0);
};
void CreateOilSpotAnyway(){
	for(int i=0;i<500;i++){
		byte x=rando()&255;
		byte y=rando()&255;
		if(CheckOilSpot(x,y)){
			CreateOilSpot(x,y);
			return;
		};
	};
};
void ShowResScreen(){
	for(int i=0;i<smaplx;i++)
		for(int j=0;j<smaply;j++){
			byte x=i+mapx;
			byte y=j+mapy;
			if(!((x&1)||(y&1))){
				if(OILMAP[y>>1][x>>1]){
					ShowOilSpot(smapx+(i<<5)-32,smapy+(j<<5)-32);
				};
			};
		};
};
