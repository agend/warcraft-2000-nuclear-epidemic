//                   Отряды
#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mgraph.h"
#include "mouse.h"
#include "menu.h"
#include "MapDiscr.h"
#include <math.h>
const idrx[8]={0,1,1,1,0,-1,-1,-1};
const idry[8]={-1,-1,0,1,1,1,0,-1};
void Legion::CreateLegionInSquare(byte x,byte y,
								  byte x1,byte y1,byte Mask){
	NMask=Mask;
	byte rx,ry;
	byte rx1,ry1;
	if(x>x1){rx=x1;rx1=x;}else{rx=x;rx1=x1;};
	if(y>y1){ry=y1;ry1=y;}else{ry=y;ry1=y1;};
	if(rx<1)rx=1;
	if(rx>msx)rx=msx;
	if(rx1<1)rx1=1;
	if(rx1>msx)rx1=msx;
	if(ry<1)rx=1;
	if(ry>msy)ry=msy;
	if(ry1<1)ry1=1;
	if(ry1>msy)ry1=msy;
	NNeed=0;
	Member=NULL;
	word NM=0;
//	word MID;
	word NNeed1=NNeed;
	for(byte ky=ry;ky<=ry1;ky++)
		for(byte kx=rx;kx<=rx1;kx++){
			OneObject* OB;
			__asm{
				xor	eax,eax
				mov	al,kx
				mov	ah,ky
				mov	eax,[Mops+eax]
				cmp	ax,0ffffh
				je	NoGo__1
				shl	eax,2
				mov	eax,[Group+eax]
				mov	OB,eax
			};
			if(OB->NMask&NMask)NM++;

NoGo__1:;
		};
	//NM-кол-во выделенных монстров
	word rlx=rx1-rx+1;
	word rly=ry1-ry+1;
	word ActSize=rlx*rly;
	word DistSize=ActSize-(ActSize>>3);
	if(NM<DistSize){//необходима перестройка в квадратную структуру
		//вычисляем размер структуры
		int Sqs=int(sqrt(NM));
		int Glx=Sqs;
		int Gly=Sqs;
		if(Glx*Gly>NM)Glx--;
		if(Glx*Gly<NM)Glx++;else
		{
			if(Glx*Gly>NM)Gly--;
			if(Glx*Gly<NM)Gly++;
		};
		XL1=byte(int(rx+rx1-Glx)>>1);
		YL1=byte(int(ry+ry1-Gly)>>1);
		if(XL1<=0)XL1=1;
		if(YL1<=0)YL1=1;
		if(XL1+Glx>msx)XL1=msx-Glx+1;
		if(YL1+Gly>msy)YL1=msy-Gly+1;
		SizeX1=Glx;
		SizeY1=Gly;
		NNeed=Glx*Gly;
	}else{
		XL1=rx;
		YL1=ry;
		SizeX1=rx1-rx+1;
		SizeY1=ry1-ry+1;
		NNeed=rx*ry;
	}
	NMemb=0;
	//вносим тварей в список
	if(NNeed1<NNeed){
		if(int(Member))free(Member);
		Member=new word[NNeed];
	};
	for(ky=ry;ky<=ry1;ky++)
		for(byte kx=rx;kx<=rx1;kx++){
			OneObject* OB;
			word MID;
			__asm{
				xor	eax,eax
				mov	al,kx
				mov	ah,ky
				mov	eax,[Mops+eax]
				mov	MID,ax
				cmp	ax,0ffffh
				je	NoGo__
				shl	eax,2
				mov	eax,[Group+eax]
				mov	OB,eax
			};
			if(OB->NMask&NMask)Member[NMemb]=MID;
			NMemb++;
NoGo__:;
		};
	maxDeplX=SizeX1<127?(SizeX1<<1):128;
	maxDeplY=SizeY1<127?(SizeY1<<1):128;
	critDeplX=SizeX1<63?(SizeX1<<2):128;
	critDeplY=SizeY1<63?(SizeY1<<2):128;
	XL2=XL1;
	YL2=YL1;
	SizeX2=SizeX1;
	SizeY2=SizeY1;
	minx=0;
	miny=0;
	maxx=0;
	maxy=0;
	NoFire=false;
	NoInitiate=false;
	NoMotion=false;
	Mobil=false;
	needMobil=false;
};
void Legion::CreateLegionFromSelection(byte Mask){
	minx=255;
	miny=255;
	maxx=0;
	maxy=0; 
	word realNm=0;
	word MID;
	OneObject* OB;
	byte xo,yo;
	word Nsel=NSL[MyNation];
	word* SMon=Selm[MyNation];
	for(word k=0;k<Nsel;k++){
		OB=Group[SMon[k]];
		if(int(OB)){
			xo=OB->x;
			yo=OB->y;
			if(xo<minx)minx=xo;
			if(yo<miny)miny=yo;
			if(xo>maxx)maxx=xo;
			if(yo>maxy)maxy=yo;
			realNm++;
			OB->TempFlag=true;
			OB->Wars=this;
			OB->Mobilised=false;
		};
	};
//	if(!NMemb)return;
	NMemb=realNm;
	realNm=0;
	CalculateBestComposition();
	Member=new word[NNeed];
	for(byte ky=miny;ky<=maxy;ky++)
		for(byte kx=minx;kx<=maxx;kx++){
			__asm{
				xor		eax,eax
				mov		al,kx
				mov		ah,ky
				mov		ax,[Mops+eax*2]
				cmp		ax,0FFFFh
				je		NextCycl
				mov		MID,ax
				mov		eax,[Group+eax*4]
				mov		OB,eax
			};
			if(OB->TempFlag){
				OB->TempFlag=false;
				Member[realNm]=MID;
				realNm++;
			};
NextCycl:;
		};
	NoFire=false;
	NoInitiate=false;
	NoMotion=false;
	Mobil=false;
	needMobil=false;
};
//minx,maxx->NNeed,SizeX1,SizeY1
void Legion::CalculateBestComposition(){
	word rx=minx;
	word ry=miny;
	word rx1=maxx;
	word ry1=maxy;
	word rlx=maxx-minx+1;
	word rly=maxy-miny+1;
	word ActSize=rlx*rly;
	word DistSize=ActSize-(ActSize>>3);
	if(NMemb<DistSize){//необходима перестройка в квадратную структуру
		//вычисляем размер структуры
		int Sqs=int(sqrt(NMemb));
		int Glx=Sqs+1;
		int Gly=Sqs+1;
		if(Glx*Gly>NMemb)Glx--;
		if(Glx*Gly<NMemb)Glx++;else
		{
			if(Glx*Gly>NMemb)Gly--;
			if(Glx*Gly<NMemb)Gly++;
		};
		SizeX1=Glx;
		SizeY1=Gly;
		NNeed=Glx*Gly;
	}else{
		SizeX1=rx1-rx+1;
		SizeY1=ry1-ry+1;
		NNeed=SizeX1*SizeY1;
	}
};
void Legion::CalculateArea(){
	if(NMemb){
		minx=255;
		miny=255;
		maxx=0;
		maxy=0;
		byte xo,yo;
		word realNm=0;
		for(word k=0;k<NMemb;k++){
			OneObject* OB=Group[Member[k]];
			if(OB){
				Member[realNm]=Member[k];
				xo=OB->x;
				yo=OB->y;
				if(xo<minx)minx=xo;
				if(xo>maxx)maxx=xo;
				if(yo<miny)miny=yo;
				if(yo>maxy)maxy=yo;
				realNm++;
			};
		};
		NMemb=realNm;
		if(NMemb){
			if(maxx-minx+1>critDeplX||
				maxy-miny+1>critDeplY)needMobil=true;
			else needMobil=false;
		};
	};
};
void Legion::CalculateSoldiers(){
	minx=255;
	miny=255;
	maxx=0;
	maxy=0; 
	word realNm=0;
	word MID;
	OneObject* OB;
	byte xo,yo;
	for(word k=0;k<NMemb;k++){
		MID=Member[k];
		if(MID!=0xFFFF){
			OB=Group[MID];
			if(int(OB)&&OB->Wars==this){
				xo=OB->x;
				yo=OB->y;
				if(xo<minx)minx=xo;
				if(yo<miny)miny=yo;
				if(xo>maxx)maxx=xo;
				if(yo>maxy)maxy=yo;
				realNm++;
				OB->TempFlag=true;
			};
		};
	};
	if(!NMemb)return;
	NMemb=realNm;
	realNm=0;
	CalculateBestComposition();
	for(byte ky=miny;ky<=maxy;ky++)
		for(byte kx=minx;kx<=maxx;kx++){
			__asm{
				xor		eax,eax
				mov		al,kx
				mov		ah,ky
				mov		ax,[Mops+eax*2]
				cmp		ax,0FFFFh
				je		NextCycl
				mov		MID,ax
				mov		eax,[Group+eax*4]
				mov		OB,eax
			};
			if(OB->TempFlag){
				OB->TempFlag=false;
				Member[realNm]=MID;
				realNm++;
			};
NextCycl:;
		};
};
inline word CalcHSpace(byte x,byte y,byte x1){
	if(y<1||y>msy)return 0;
	word NF=0;
	__asm{
		xor		eax,eax
		xor		ebx,ebx
		xor		ecx,ecx
		mov		al,x
		mov		ah,y
		mov		cl,x1
		sub		cl,x
		inc		cl
LpStart:
		cmp		[TrMap+eax],0
		jnz		LockedCell1
		inc		bx
LockedCell1:
		inc		al
		loop	LpStart
		mov		NF,bx
	};
	return	NF;
};
inline word CalcVSpace(byte x,byte y,byte y1){
	if(x<1||x>msx)return 0;
	word NF=0;
	__asm{
		xor		eax,eax
		xor		ebx,ebx
		xor		ecx,ecx
		mov		al,x
		mov		ah,y
		mov		cl,y1
		sub		cl,ah
		inc		cl
LpStart:
		cmp		[TrMap+eax],0
		jnz		LockedCell1
		inc		bx
LockedCell1:
		inc		ah
		loop	LpStart
		mov		NF,bx
	};
	return	NF;
};
//создает зону,в которую солдаты должны переместиться,
//центр зоны-точка с координатами (x,y)
//перед этим должна вызыватьса CalculateSoldiers()
void Legion::CreateFinalZone(byte x,byte y){
	word NFree=0;
	int x0=x-(SizeX1>>1);
	int	y0=y-(SizeY1>>1);
	if(x0<1)x0=1;
	if(y0<1)y0=1;
	int x1=x0+SizeX1-1;
	int y1=y0+SizeY1-1;
	if(x1>msx){
		x1=msx-SizeX1+1;
		x0=x1-SizeX1+1;
	};
	if(y1>msy){
		y1=msy-SizeY1+1;
		y0=y1-SizeY1+1;
	};
	StonesInside=false;
	//считаем кол-во доступных мест в этом квдрате
	__asm{
		xor		eax,eax
		mov		bl,byte ptr x1
		mov		bh,byte ptr y1
		mov		al,byte ptr x0
		mov		ah,byte ptr y0
CycleStart1:
		cmp		byte ptr [TrMap+eax],0
		jne		LockedCell
		inc		NFree
LockedCell:
		inc		al
		cmp		al,bl
		jbe		CycleStart1
		mov		al,byte ptr x0
		inc		ah
		cmp		ah,bh
		jbe		CycleStart1
	};
	word MaxRes=0;
	word Resz;
	byte DIR1;
	word rf=0;
	word rrr;
	while(NMemb>NFree){
		StonesInside=true;
		//пытаемся раздвинуть зону
		//вниз
		Resz=CalcHSpace(x0,y1+1,x1);
		rrr=NFree+Resz>NMemb?NFree+10000-Resz:NFree+Resz;
		if(rrr>rf){
			rf=rrr;
			MaxRes=Resz;
			DIR1=0;
		};
		//вверх
		Resz=CalcHSpace(x0,y0-1,x1);
		rrr=NFree+Resz>NMemb?NFree+10000-Resz:NFree+Resz;
		if(rrr>rf){
			rf=rrr;
			MaxRes=Resz;
			DIR1=1;
		};
		//вправо
		Resz=CalcVSpace(x1+1,y0,y1);
		rrr=NFree+Resz>NMemb?NFree+10000-Resz:NFree+Resz;
		if(rrr>=rf){
			rf=rrr;
			MaxRes=Resz;
			DIR1=2;
		};
		//влево
		Resz=CalcVSpace(x0-1,y0,y1);
		rrr=NFree+Resz>NMemb?NFree+10000-Resz:NFree+Resz;
		if(rrr>rf){
			rf=rrr;
			MaxRes=Resz;
			DIR1=0;
		};
		if(MaxRes==0)break;
		switch(DIR1){
		case 0:	y1++;
				break;
		case 1: y0--;
				break;
		case 2: x1++;
				break;
		case 3: y0--;
				break;
		};
		NFree+=MaxRes;
	};
	destX=x0;
	destY=y0;
	destX1=x1;
	destY1=y1;
};
//посылает солдат в выбранную зону
void Legion::SendSoldiers(){
	word MID;
	OneObject* OB;
	byte tx=destX;
	byte ty=destY;
	for(word k=0;k<NMemb;k++){
		MID=Member[k];
		if(MID!=0xFFFF){
			OB=Group[MID];
			while(TrMap[ty][tx]){
				tx++;
				if(tx>destX1){
					tx=destX;
					ty++;
				};
			};
			OB->ClearOrders();
			OB->SendInGroup(tx,ty,destX,destY,destX1,destY1);
			OB->PathBroken=false;
			tx++;
			if(tx>destX1){
				tx=destX;
				if(ty<msy)ty++;
			};
		};
	};
};
bool Legion::MobilisationIsDone(){
	return false;
};
bool Legion::CompositionIsDone(){
	return false;
};
void Legion::Compose(){
};
void Legion::Mobilisate(){
};
void Legion::MoveTo(byte x,byte y){
};
void Legion::AttackObj(word ObjID){
};
void Legion::Process(){
	if(NMemb==0)return;
	CalculateArea();
	Ordering=GetOrderDegree();
	if(needMoveTo){
		if(Ordering>=3)Order3Time++;else Order3Time=0;
		if(Order3Time>20)needMoveTo=false;
	};
	if(needCompactness&&!TempMobil){
		if(Ordering==0){
			byte XG=FinX;
			byte YG=FinY;
			if(FinX>maxx)XG=maxx;
			if(FinX<minx)XG=maxy;
			if(FinY>maxy)YG=maxy;
			if(FinY<miny)YG=miny;
			CreateFinalZone(XG,YG);
			if(!StonesInside){
				SendSoldiers();
				TempMobil=true;
			}else CreateFinalZone(FinX,FinY);
		};
	};
	if(TempMobil&&Ordering>=2){
		CreateFinalZone(FinX,FinY);
		SendSoldiers();
		TempMobil=false;
	};
};
void Legion::PreProcess(){

};
void OneObject::ClearOrders(){
	if(int(InLineCom))FreeAsmLink();
	while(int(LocalOrder)){
		Order1* Loc1=LocalOrder->NextOrder;
		FreeOrdBlock(LocalOrder);
		LocalOrder=Loc1;
	};
	Attack=0;
}
void SendInGroupLink(OneObject* OBJ);
void OneObject::SendInGroup(byte tx,byte ty,byte x0,byte y0,
							byte x1,byte y1){
	lastX=tx;
	lastY=ty;
	destX=x0;
	destY=y0;
	destX1=x1;
	destY1=y1;
	if(MoveInRect)return;
	Order1* Or1=GetOrdBlock();
	if(!int(Or1))return;
	Or1->NextOrder=LocalOrder;
	Or1->OrderType=22;
	Or1->OrderTime=0;
	Or1->DoLink=&SendInGroupLink;
	Order1* LOR=LocalOrder;
	if(int(InLineCom)){
		FreeAsmLink();
	};
	LocalOrder=Or1;
	//OrderReport=NULL;
	//MessageKind=0;
	//Sender=0xFFFF;
	MoveInRect=true;
	DrawUp=true;
}; 
void SendInGroupLink(OneObject* OBJ){
	if(int(OBJ->Wars)){
		if(!OBJ->Wars->needMoveTo){
			Order1* Loc1=OBJ->LocalOrder->NextOrder;

		};
	};
	byte x=OBJ->x;
	byte y=OBJ->y;
	if(x>=OBJ->destX&&x<=OBJ->destX1&&y>=OBJ->destY&&y<=OBJ->destY1&&OBJ->PathBroken){
		byte nx=x+idrx[OBJ->Direction];
		byte ny=y+idry[OBJ->Direction];
		if(nx>=OBJ->destX&&nx<=OBJ->destX1&&ny>=OBJ->destY&&
			ny<=OBJ->destY1&&!TrMap[ny][nx]){
				OBJ->LoadAnimation(1,1,0);
				AddAsk(OBJ->Index|0x8000,x,y,idrx[OBJ->Direction],idry[OBJ->Direction]);
		};
		OBJ->lastX=x;
		OBJ->lastY=y;
		return;
	};
	if(int(OBJ->InLineCom))OBJ->FreeAsmLink();
	if(OBJ->x!=OBJ->lastX||OBJ->y!=OBJ->lastY)
		OBJ->CreatePath(OBJ->lastX,OBJ->lastY);
	OBJ->MemoryTime=0;
};
void Legion::SendTo(byte x,byte y){
	TempMobil=false;
	StonesInside=false;
	needCompactness=true;
	needMoveTo=true;
	FinX=x;
	FinY=y;
	Order3Time=0;
	CalculateSoldiers();
	CreateFinalZone(x,y);
	SendSoldiers();
};
//0-никакого порядка нет, войско рассеяно
//1-порядок недостаточен для нормального функционирования
//2-порядок, достаточный для перемещения
//3-минимальный беспорядок,твари только на одну клетку не влезли
//4-идеальный порядок
char Legion::GetOrderDegree(){
	short MaxDY1=abs(short(maxy-miny)-short(destY1-destY));
	short MaxDX1=abs(short(maxx-minx)-short(destX1-destX));
	short LX=destX1-destX+1;
	short LY=destY1-destY+1;
	if(MaxDX1==0&&MaxDY1==0)return 4;
	if(MaxDX1<=2&&MaxDY1<=2)return 3;
	if(MaxDX1<=LX&&MaxDY1<=LY)return 2;
	if(MaxDX1<=(LX+LX)&&MaxDY1<=(LY+LY))return 1;
	return 0;
};