/*
 *
 *  MAIN COMMAND MODULE. (Andrew)
 *
 */
/* 1) Загрузка образов монстров из списка в файле monsters.lst
 * 2)Создание нации - загрузка:
 * nation.gac - Global Animation Collection
 * nation.gmc - Global Monsters Collection
 */	
#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mgraph.h"
#include "mouse.h"
#include "menu.h"
#include "MapDiscr.h"
#include "fog.h"
#include "Megapolis.h"
#include "FlyObj.h"
#include <assert.h>
#include "walls.h"
#include "mode.h"
#include "GSound.h"
int NMyUnits;
int NThemUnits;
extern int tmtmt;
extern int HREQRADIUS;
extern word FlyMops[256][256];
extern byte NPresence[64][64];
void ShowFog(OneObject* ZZ);
//byte CreateExObjR(Weapon* Wep,short x,short y,
//				 short dx,short dy,short v,byte Mask,OneObject* Send,word Dist);
//byte CreateExObjDPointR(Weapon* Wep,short x,short y,
//				 short dx,short dy,short v,byte Mask,OneObject* Send,byte dsx,byte dsy,word Range);
word MAXOBJECT;
//Если поступает приказ уровня<16 и тварь, которую
//нужно атаковать стоит дальше этого расстояния, то
//приказ не выполняется
static MaxReplyDistance=50;

int PathAsks;
const int drr[9]={7,6,5,0,0,4,1,2,3};
const byte drrb[9]={7,6,5,0,0,4,1,2,3};
const idrx[8]={0,1,1,1,0,-1,-1,-1};
const idry[8]={-1,-1,0,1,1,1,0,-1};
//Массив для оптимизации пути
static int OptA[256][256];
static SIndex;
int counter;
static byte xi[256][256];
static byte yi[256][256];
byte RNTB[64][256];
//HUGE ARRAY!!!!!!!!!!!!
OneObject OBJECTS[8192];
//Организация очереди на выполнение долгого приказа
word ComStc[StSize];
word StHead;
word StTile;
void MCHK(){
/*	if(int(Group[150]->LocalOrder)){
		if(Group[150]->LocalOrder->OrderType==2&&
			Group[150]->LocalOrder->info.MoveToXY.y==0){
			int uuu=0;
			return;
		};
	}*/
};
void InitStack(){
	StHead=0;
	StTile=0;
};
bool CheckAttAbility(OneObject* OB,word Patient){
	if(OB&&Patient!=0xFFFF){
		OneObject* EN=Group[Patient];
		if(!EN)return false;
		GeneralObject* EGO=EN->Ref.General;
		if(EN->NMask&OB->NMask)return false;
		GeneralObject* GO=OB->Ref.General;
		if(GO->WATT&&!(EN->OnWater||EGO->OnCost))return false;
		if(EN->Media==2&&!GO->AATT)return false;
		if(EGO->Submarine&&!GO->UATT)return false;
		if(EN->OnWater&&!GO->SATT)return false;
		return true;
	}else{
		return false;
	};

};
word PopCmd(){
	if(StHead!=StTile){
		int z=StTile;
		StTile++;
		StTile=StTile&StMask;
		OneObject* OBJ=Group[ComStc[z]];
		if(int(OBJ)){
			OBJ->Push=false;
			return ComStc[z];
		}else return 0xFFFF;
	} else return 0xFFFF;
};
void PushCmd(word Cmd){
	if(Group[Cmd]->Push)return;
	ComStc[StHead]=Cmd;
	StHead++;
	StHead=StHead&StMask;
	Group[Cmd]->Push=true;
};
void CarryOutOrder(){
	int i;
	int TimerX=FreeTime;
	do{
		i=PopCmd();
		TimerX-=10;
		if(i==0xFFFF)return;
		OneObject* Ob=Group[i];
		Order1* Ord=Ob->LocalOrder;
		
		if(!int(Ob->InLineCom)&&int(Ob)&&int(Ord))Ord->DoLink(Ob);
		//{
		//	if(Ord->OrderType==2)Ob->SendTo(Ord->info.MoveToXY.x,
		//							  Ord->info.MoveToXY.y,
		//							  Ord->PrioryLevel+128);
		//}
	}while(TimerX>0);
};
RLCTable MImage[maximage];
RLCTable miniMImage[maximage];
int MaxImage;
void Err(LPCSTR s)
{
	MessageBox(hwnd,s,"Nation loading failed...",MB_ICONWARNING|MB_OK);
};
void LoadMonsters(){
	FILE* f=fopen("monsters.lst","r");
	char c[64];
	char cc[64];
	for(int i=0;!feof(f);i++){
		fscanf(f,"%s",c);
		strcpy(cc,c);
		strcat(c,".rlc");
		strcat(cc,".rlx");
		if(c[0]!=0)LoadRLC(c,&MImage[i]);
		if(cc[0]!=0)LoadRLC(cc,&miniMImage[i]);
	};
	ADDSUMM(MImage,1024);
	MaxImage=i;
	for(byte j=0;j<64;j++){
		byte q=0;
		for(int t=0;t<256;t++){
			RNTB[j][t]=q;
			q++;
			if(q>j)q=0;
		};
	};
};

//Загрузка нации из файла
void Nation::CreateNation(LPCSTR NationFile,byte Mask,byte NI){
	char tmp[32];
	NMask=Mask;
	CITY=NULL;
	NNUM=NI;
	NGidot=0;
	NFarms=0;
	CasheSize=0;
	strcpy(tmp,NationFile);
	strcat(tmp,".gac");
	ResFile f=RReset(tmp);
	if(IOresult())Err("Can't open global animation collection.");
	int i;
	RBlockRead(f,&i,4);
	if(i!=0x1B434147)Err("Incorrect format GAC.");
	RBlockRead(f,&NOct,4);
	RBlockRead(f,&NSlides,4);
	Animations=(Octant*)malloc(NOct*sizeof Animation);
	Slides=(OneSlide*)malloc(NSlides*sizeof  OneSlide);
	RBlockRead(f,Animations,NOct*sizeof Animation);
	RBlockRead(f,Slides,NSlides*sizeof  OneSlide);
	for(i=0;i<NOct*8;i++) 
		Animations[i].Movie=(OneSlide*)(
		2*int(Animations[i].Movie)+int(Slides));
	RClose(f);
	ADDSUMM(Animations,NOct*sizeof Animation);
	ADDSUMM(Slides,NSlides*sizeof  OneSlide);
	strcpy(tmp,NationFile);
	strcat(tmp,".gmc");
	f=RReset(tmp);
	if(IOresult())Err("Can't open GMC.");
	RBlockRead(f,&i,4);
	if(i!=0x1B434D47)Err("Incorrect GMC format.");
	RBlockRead(f,&NMon,4);
	int StrSize;
	RBlockRead(f,&StrSize,4);
	RBlockRead(f,&NFinf,4);
	Finf=(MoreAnimation*)malloc(NFinf*8);
	for(i=0;i<NMon;i++){
		Mon[i]=new Visuals;
		RBlockRead(f,&Mon[i]->NameIndex,StrSize);
		Mon[i]->lpFAnim=(MoreAnimation*)(int(Finf)+int(Mon[i]->lpFAnim));
	};
	RBlockRead(f,Finf,NFinf*8);
	for(int j=0;j<NFinf;j++)
			Finf[j].Anm=
			  (Octant*)(int(Finf[j].Anm)+int(Animations));
	ADDSUMM(Finf,NFinf*8);
	RClose(f);
	PathAsks=0;
	memset(&PACount,0,sizeof PACount);
	memset(&PAble,0,sizeof PAble);
};
word LastObject;
//Создать паземный объект типа монсира с координатами (x,y)
//и номером n в таблице монстров данной нации
bool Nation::CreateTerrainAt(int x,int y,int n){
	LastObject=0xFFFF;
	if(Mon[n]->OnWater){
		return CreateOnWater(x,y,n);
	};
	if(Mon[n]->OnAir){
		return CreateOnFly(x,y,n);
	};
	if( Mops[y][x]!=0xFFFF||LLock[y][x])return false;
	for(int i=0;i<MaxObj&&int(Group[i]);i++);
	if(i>MaxObj)return false;
	if(n>=NMon) return false;
	if(i>=MAXOBJECT)MAXOBJECT=i+1;
	LastObject=i;
	Group[i]=OBJECTS+i;
	Cell8x8* CELL=&TCInf[NNUM][y>>2][x>>2];
	OneObject* G=Group[i];
	GeneralObject* GO=Mon[n];
	AddOrderEffect(x,y,GO->BornSound);
	CELL->UnitsAmount[GO->Kind]++;
	G->DefaultSettings(GO);
	G->Media=0;
	G->Teleport=false;
	G->capTeleport=GO->Teleport;
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
	Mops[y][x]=i;
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
	LLock[y][x]=true;
	IncLock(x,y);
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
	G->StandGround=false;
	G->EnemyID=0xFFFF;
	G->Egoist=false;
	if(CITY->IntellectEnabled){
		if(GO->CanAttWall)G->DoWalls=true;
		G->Use_AI=true;
		G->NeedNoHelp=true;
	};
	NGidot++;
	return true;
};
//Загрузить анимацию с данными показателями в один из 5-ти внутр.
//регистров 
void OneObject::LoadAnimation(int Reg,byte WhatFor,word Kind){
	if(ARegs[Reg].Kind==Kind&&ARegs[Reg].WhatFor==WhatFor&&int(ARegs[Reg].Anm))return;
	MoreAnimation* An=Ref.General->lpFAnim;
	int NAn=Ref.General->NAnm;
#ifdef DEBLEVEL1
	assert(NAn<64&&NAn>0);
	assert(WhatFor<32);
#endif
	for(int i=0;i<NAn&&(WhatFor!=An[i].WhatFor||Kind!=An[i].Kind);i++);
	if(i<NAn){
		ARegs[Reg].Kind=Kind;
		ARegs[Reg].WhatFor=WhatFor;
		ARegs[Reg].Anm=An[i].Anm;
	} else{
		for(int i=0;i<NAn&&(WhatFor!=An[i].WhatFor);i++);
		if(i<NAn){
			ARegs[Reg].Kind=Kind;
			ARegs[Reg].WhatFor=WhatFor;
			ARegs[Reg].Anm=An[i].Anm;
		}
#ifdef DEBLEVEL1 
		else assert(0);
#else
		;
		ARegs[Reg].Kind=0;
		ARegs[Reg].WhatFor=0;
		ARegs[Reg].Anm=An[0].Anm;
#endif
	};

};
//Обработка перевернутых спрайтов
void OneObject::LoadCurAnm(int reg){
#ifdef DEBUGLEVEL1
	assert(Direction<8);
	assert(reg<4);
#endif
	if(Direction>4){
		CurAnm=&(ARegs[reg].Anm[8-Direction]);
		Invert=true;
	} else{
		CurAnm=&(ARegs[reg].Anm[Direction]);
		Invert=false;
	};
	CurrentSprite=0;
	Ticks=0;
	TicksPerChange=1;
};
void OneObject::FreeAsmLink(){
	if(!int(InLineCom))return;
	FreeAsmBlock(InLineCom);
	char* k;
	do{
		memcpy(&k,
			&((char*)InLineCom)[OneAsmSize-4],4);
		if(k!=0)FreeAsmBlock(k);
		InLineCom=k;
	}while (int(k)!=0);
	InLineCom=NULL;
	LineOffset=0;
};
//Основные структуры для обработки перемещений
//запрос на перемещение
struct AskMove{
	word ReqID;//индекс перемещаемого
	word PreID;//индекс жаждущего дорваться в эту клетку
	byte x;
	byte y;
	char dx;
	char dy;
};
int NAsk;//Количество запросов
AskMove Ask[8192];//Массив запросов
bool FailLink[8192];//Массив неразрешенных перемещений
word CurInd;
word IDMap[256][256];
word RQMap[256][256];//:3-запещенное направление ..
					 //:13-номер в таблице запросов
//Добавить запрос в систему запросов на перемещение
void AddAsk(word ReqID,byte x,byte y,char zdx,char zdy){
	//DEBUGGING
	OneObject* OB=Group[ReqID];
#ifdef DEBLEVEL1
	assert(OB->x==x&&OB->y==y);
	assert(!OB->AskMade);
	assert((!OB->OnWater)&&abs(zdx)<2&&abs(zdy)<2);
	if(OB->OnWater) assert(abs(zdx)<3&&abs(zdy)<3);
	else assert(abs(zdx)<2&&abs(zdy)<2);
#endif
	OB->AskMade=true;
	//ENDDEBUG
	__asm{
//		inc		NAsk
		mov		eax,NAsk
		shl		eax,3
		add		eax,offset Ask
		mov		bx,ReqID
		mov		word ptr [eax],bx
		mov		word ptr [eax+2],0FFFFh
		mov		bl,x
		mov		bh,y
		mov		word ptr [eax+4],bx
		mov		bl,zdx
		mov		bh,zdy
		mov		word ptr [eax+6],bx
		inc		NAsk
	};
};
//Обработка запросов, сначала освобождаем все старые клетки
//потом заполняем и проверяем
void HandleAsks(){
	if(!NAsk)return;
	__asm{
		cmp		CurInd,0
		jne		NoFill
		mov		edi,offset IDMap
		mov		ecx,32768
		cld	
		xor		eax,eax
		rep		stosd
NoFill:
		inc		CurInd
	};
	int NN=NAsk;
	int	ZZ=0;
	//освобождаем место
	__asm{
		mov	esi,offset Ask
		xor		eax,eax
		xor		ecx,ecx
		xor		ebx,ebx
		xor		edx,edx
C1Begin:
		mov		al,[esi+6]//dx
		inc		al
		mov		ah,al
		shl		al,1
		add		al,ah
		add		al,[esi+7]//dy
		inc		al
		xor		ah,ah
		mov		bl,byte ptr [drrb+eax]//direction
		mov		cx,[esi+4]//x,y=>offset
		mov		ax,CurInd
		mov		[IDMap+ecx*2],ax
		add		bl,4
		and		bl,7
		mov		dx,word ptr ZZ
		shl		dx,3
		add		dl,bl
		mov		[RQMap+ecx*2],dx
		mov		byte ptr [LLock+ecx],0//разблокировали старое положение
		add		esi,8//size of Ask[0] 
		inc		ZZ
		dec		NN
		jnz		C1Begin
		//очищаем массив сбоев
		mov		edi,offset FailLink
		mov		ecx,NAsk
		shr		ecx,2
		inc		ecx
		cld
		mov		eax,01010101h
		rep		stosd
		//Проверяем возможность перемещения
		mov		eax,NAsk
		mov		NN,eax
		mov		esi,offset Ask
		xor		ecx,ecx
		xor		eax,eax
		mov		ZZ,0
C2Begin:

		mov		cx,[esi+4]//x,y -> offset
		add		cl,[esi+6]//x+dx,y+dy -> offset
		add		ch,[esi+7]
		cmp		byte ptr [LLock+ecx],0
		jnz		C2isLocked
		mov		ax,word ptr CurInd
		cmp		word ptr [IDMap+ecx*2],ax
		jne		C2notLocked
		//direction
		mov		dx,[RQMap+ecx*2]
		and		dl,7
		mov		al,[esi+6]
		inc		al
		mov		ah,al
		shl		al,1
		add		al,ah
		add		al,[esi+7]
		inc		al
		xor		ah,ah
		mov		al,[drrb+eax]//=direction
		cmp		al,dl
		je		C2isLocked //переход в этом направлении запрещен
		//ставим ссылку на данного монстра в поле PreID 
		//у монстра, который ранее занимал эту клетку
		mov		dx,[RQMap+ecx*2]
		and		edx,0FFF8h
		mov		ax,word ptr ZZ
		mov		word ptr [Ask+2+edx],ax
C2notLocked:
		mov		[LLock+ecx],1//блокируем клетку
		inc		ZZ
		add		esi,8
		dec		NN
		jnz		C2Begin
		jmp		CycEnd
		//Клетка заблокирована, определяем монстров,перемещение 
		//которых запрещено отныне
C2isLocked:
		mov		ax,[esi+4]
		mov		byte ptr [LLock+eax],1
		mov		ax,word ptr ZZ
		xor		edx,edx
FailLoop:
		mov		byte ptr [FailLink+eax],0
		mov		bx,word ptr [Ask+2+eax*8]
		mov		word ptr [Ask+2+eax*8],0FFFFh
		mov		dx,word ptr [Ask+4+eax*8]
		mov		[LLock+edx],1//блокируем старую позицию
		mov		ax,bx
		cmp		ax,0FFFFh
		jne		FailLoop
		inc		ZZ
		add		esi,8
		dec		NN
		jnz		C2Begin
CycEnd:
	};
	//результатом вышеуказанного прострада было
	//правильное заполнение массива FailLink,
	//то есть определение, каким монстрам перемещение 
	//разрешено, а каким нет
};
void PrepareProcessing(){
	NAsk=0;
	//Only for DEBUGGIONG 
	for(int i=0;i<MAXOBJECT;i++){
		OneObject* OB=Group[i];
		if(OB)OB->AskMade=false;
	};
};
inline void Spot(int x,int y){
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
void HandleDWall(OneObject* OB);
void OneObject::MakePreProcess(){
	if(Media||Absent)return;
	if(capMagic){
		if(Magic<capMagic&&!(tmtmt&7))Magic++;
	};
	ShowFog(this);
	if(RefreshLife&&Life<MaxLife){
		if(!(tmtmt&3))Life++;
	};
	if(NNUM==MyNation){
		int x0,y0,dx,dx1;
		switch(SpotType){
		case 0:
			if(Lx>1){
			for(byte ux=0;ux<Lx;ux++)
				for(byte uy=0;uy<Ly;uy++)
					Spot(x+ux,y+uy);
			}else Spot(x,y);
			break;
		case 1:
			x0=x+(Lx>>1);
			y0=y+(Ly>>1);
			dx=SpotSize;
			Spot(x0,y0);
			Spot(x0+dx,y0);
			Spot(x0-dx,y0);
			Spot(x0,y0+dx);
			Spot(x0,y0-dx);
			break;
		case 2:
			x0=x+(Lx>>1);
			y0=y+(Ly>>1);
			dx=SpotSize;
			dx1=dx-(dx>>2);
			Spot(x0,y0);
			Spot(x0+dx,y0);
			Spot(x0-dx,y0);
			Spot(x0,y0+dx);
			Spot(x0,y0-dx);
			Spot(x0+dx1,y0+dx1);
			Spot(x0-dx1,y0-dx1);
			Spot(x0+dx1,y0-dx1);
			Spot(x0-dx1,y0+dx);
			break;
		};
	};
	if(MTime){
		if(MTime==1){
			xForce=16;
		};
		MTime--;
	};
	if(!NoSearchVictim)SearchVictim();
	if(DoWalls)HandleDWall(this);
	if(delay)delay--;
	else{
		if(AntiNuc){
			if(RAmount<20)RAmount++;
			delay=3000;
		};
	};
	Ticks++;
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
		if(Sdoxlo){
			if(TicksPerChange==250){;
			     Died[y][x]=0xFFFF;
				 OneObject yy=*Group[Index];
				 OneObject* OO=Group[Index];
				 Group[Index]=NULL;
				 OneObject OOO=*OO;
				 //free(OO);
				 return;
			}else{
				TicksPerChange=250;
				return;
			};
		};
		if(TicksPerChange!=255)CurrentSprite=0;
		Addx=0;
		Addy=0;
		Removed=false;
		StandTime++;
		if(Life<=0){
			Die();
			return;
		};
		if(Life==1&&(!capBuilding)&&rando()<1024)Life=0;
		//if(!Important)
		Order1* LO=LocalOrder;
		if(!LO){
			PrioryLevel=0;
			if(Repair){
				WaitForRepair();
			};
		};
		if(int(InLineCom)){
			if(LO&&LO->OrderType>127){
				//FreeAsmLink();
				LocalOrder->DoLink(this);
			};
			bool	Done;
			if(InLineCom)
			do{
				byte* p=(byte*)(int(InLineCom)+LineOffset);
				byte b=p[0];
				Done=false;
				switch(b){
				//case 0:
				case 1:
					//Return
					FreeAsmLink();	
					//MessageFlags=MessageFlags|2;
					Done=true;
					break;
				case 3:
					//TakeNextOrder();
					{
						if(int(LocalOrder)){
							Order1* Loc1=LocalOrder->NextOrder;
							FreeOrdBlock(LocalOrder);
							LocalOrder=Loc1;
						};
						LineOffset++;
						Done=true;
					};
					break;
				case 4:
					//set coordinates
					//[04][dx][dy]
					assert(x>0&&x<=msx&&y>0&&y<=msy);
					AddAsk(Index,x,y,p[1],p[2]);
					LineOffset+=3;
					Done=true;
					Removed=true;
					break;
				case 5:
					//set direction
					Direction=p[1];
					LineOffset+=2;
					break;
				case 6:
					LoadAnimation(p[1],p[2],p[3]);
					LineOffset+=5;
					break;
				case 7:
					//jump...
					{int pp=*(int*)(int(p)+1);
					FreeAsmBlock(InLineCom);
					LineOffset=0;
					InLineCom=(char*)pp;};
					break;
				case 8:
					//Use register to animate
					LoadCurAnm(p[1]);
					Ticks=0;
					TicksPerChange=1;
					LineOffset+=2;
					Done=true;
					break;
				default:
					assert(0);
					break;
				};
			} while(!Done);
			//if(int(OrderReport))OrderReport(this);
			//OneObject* OBJ=Group[0];
			//if(OBJ->Removed&&(OBJ->InLineCom)[OBJ->LineOffset]!=4)
			//	int ggg=0;
			//if(LO&&LO->OrderType>127&&!AskMade){
			//	//FreeAsmLink();
			//	LocalOrder->DoLink(this);
			//};
		} else{ 
			if(TicksPerChange!=255)LoadCurAnm(0);
			Important=false;
			if(int(LocalOrder)){
				if(LocalOrder->OrderTime&&!Push){
					PushCmd(Index);
					return;
				};
				LocalOrder->DoLink(this);
//				if(LocalOrder->OrderType==77)
//					MoveFrom(LocalOrder->info.MoveFrom.dir);
			} else	if(TicksPerChange!=255)CurrentSprite=0;
		};
};
void MakePostProcess(){
	HandleAsks();
	for(int i=0;i<NAsk;i++){
		AskMove ASK=Ask[i];
		OneObject* OBJ=Group[ASK.ReqID&8191];
		if(FailLink[i]){
			byte ax=ASK.x;
			byte ay=ASK.y;
			DecLock(ax,ay);
			char oox=ax>>2;
			char ooy=ay>>2;
			byte Kind=OBJ->Kind;
			assert(ax==OBJ->x&&ay==OBJ->y);
			OBJ->Direction=drrb[(ASK.dx+1)*3+ASK.dy+1];
			TrMap[ay][ax]=0;
			//LLock[ay][ax]=0;
			assert(abs(ASK.dx)<2&&abs(ASK.dy)<2);
			byte anx=ax+ASK.dx;
			byte any=ay+ASK.dy;
			IncLock(anx,any);
			OBJ->x=anx;
			OBJ->y=any;
			char onx=anx>>2;
			char ony=any>>2;
			Mops[OBJ->y][OBJ->x]=OBJ->Index;
			if(Mops[ay][ax]==OBJ->Index)Mops[ay][ax]=0xFFFF;
			Ask[i].x=OBJ->x;
			Ask[i].y=OBJ->y;
			OBJ->Addx=-ASK.dx<<5;
			OBJ->Addy=-ASK.dy<<5;
			OBJ->LoadCurAnm(1);
			OBJ->isMoving=true;
			OBJ->StandTime=0;
			//if(OBJ->Removed)OBJ->LineOffset+=3;
			Ask[i].x=OBJ->x;
			Ask[i].y=OBJ->y;
			if(oox!=onx||ooy!=ony){
				Cell8x8* OldCell=&TCInf[OBJ->NNUM][ooy][oox];	
				Cell8x8* NewCell=&TCInf[OBJ->NNUM][ony][onx];
				OldCell->UnitsAmount[Kind]--;
				NewCell->UnitsAmount[Kind]++;
			};
		}else{
			OBJ->StandTime++;
			if(!(ASK.ReqID&0x8000)){
				byte ax=ASK.x+ASK.dx;
				byte ay=ASK.y+ASK.dy;
				int mopsa=Mops[ay][ax];
				if(mopsa!=0xFFFF){
					OneObject* OBJ2=Group[mopsa];
					//if(OBJ->StandTime>1)
						OBJ2->MoveFrom(OBJ->Direction);
				}else{;
					OBJ->FreeAsmLink();
					OBJ->LineOffset=0;
				};
			};
			OBJ->LoadAnimation(0,OBJ->AnmStandKind,0);
			OBJ->LoadCurAnm(0);
			OBJ->PathBroken=true;
		};
	};
};
void OneObject::MakeProcess(){
};
//Inline- команды построения внутренних команд
int Ofst;
char* NowBuf;
inline void ChkOfst(int size){
	if(Ofst>=OneAsmSize-size-5-4){
		char* NN=GetAsmBlock();
		if(int(NN)){
			NowBuf[Ofst]=7;
			memcpy(&NowBuf[Ofst+1],&NN,4);
			memcpy(&NowBuf[OneAsmSize-4],&NN,4);
			NowBuf=NN;
			Ofst=0;
			memcpy(&NowBuf[OneAsmSize-4],&Ofst,4);
		}else
			NowBuf[Ofst]=0;
	};
};
inline void cmSetXY(char x,char y){
	ChkOfst(3);
	assert(abs(x)<2&&abs(y)<2);
	NowBuf[Ofst]=4;
	NowBuf[Ofst+1]=x;
	NowBuf[Ofst+2]=y;
	Ofst+=3;
};
inline void cmSetXYDir(byte x,byte y,byte dir,byte n){
	ChkOfst(4);
	NowBuf[Ofst]=18;
	NowBuf[Ofst+1]=x;
	NowBuf[Ofst+2]=y;
	NowBuf[Ofst+3]=(dir&7)|(n<<4);
	Ofst+=4;
};
inline void cmSetXYDirX(byte x,byte y,char dx,char dy,byte n){
	ChkOfst(4);
	assert(dx<2&&dy<2);
	byte dr=drr[(dx+1)*3+dy+1];
	NowBuf[Ofst]=18;
	NowBuf[Ofst+1]=x;
	NowBuf[Ofst+2]=y;
	NowBuf[Ofst+3]=(dr&7)|(n<<4);
	Ofst+=4;
};

inline void cmChkXY(byte x,byte y){
	ChkOfst(3);
	NowBuf[Ofst]=44;
	NowBuf[Ofst+1]=x;
	NowBuf[Ofst+2]=y;
	Ofst+=3;
};
inline void cmSetDir(int dx,int dy){
	if(dx==0&&dy==0)return;
	byte dr=drr[(dx+1)*3+dy+1];
	ChkOfst(2);
	NowBuf[Ofst]=5;
	NowBuf[Ofst+1]=dr;
	Ofst+=2;
};
void cmSetDirD(byte dr){
	ChkOfst(2);
	NowBuf[Ofst]=5;
	NowBuf[Ofst+1]=dr&7;
	Ofst+=2;
};
inline void cmLoadAnm(byte stype,byte dtype,word kind){
	ChkOfst(5);
	NowBuf[Ofst]=6;
	NowBuf[Ofst+1]=dtype;
	NowBuf[Ofst+2]=stype;
	NowBuf[Ofst+3]=byte(kind);
	NowBuf[Ofst+4]=0;
	Ofst+=5;
};
inline void cmPerfAnm(byte n){
	ChkOfst(2);
	NowBuf[Ofst]=8;
	NowBuf[Ofst+1]=n;
	Ofst+=2;
};
inline void cmRet(){
	NowBuf[Ofst]=1;
	Ofst+=1;
};
inline void cmDone(){
	NowBuf[Ofst]=3;
	Ofst+=1;
};
//Переместить объект в точку (x,y)
typedef byte xxx[64];
void COrd(Order1* ordr){
	if(!int(ordr))return;
	if((int(ordr)-int(OrdBuf))/sizeof Order1>=MaxOrdCount){
		int RRRR=(int(ordr)-int(OrdBuf))/sizeof Order1;
	};
};
byte circ[16];
inline void FillCirc(int x,int y){
	__asm{
		xor		ebx,ebx
		mov		bh,byte ptr y
		//inc		bh
		mov		bl,byte ptr x
		//inc		bl
		add		ebx,offset TrMap
		mov		al,[ebx-256]
		mov		circ[0],al
		mov		al,[ebx-255]
		mov		circ[1],al
		mov		al,[ebx+1]
		mov		circ[2],al
		mov		al,[ebx+257]
		mov		circ[3],al
		mov		al,[ebx+256]
		mov		circ[4],al
		mov		al,[ebx+255]
		mov		circ[5],al
		mov		al,[ebx-1]
		mov		circ[6],al
		mov		al,[ebx-257]
		mov		circ[7],al
		mov		eax,dword ptr circ[0]
		mov		dword ptr circ[8],eax
		mov		eax,dword ptr circ[4]
		mov		dword ptr circ[12],eax
	}
};
void SendToLink(OneObject* OBJ);
void OneObject::SendTo(int x2,int y2,int Prio){
#define MaxP 160
	if(Prio>=16)StandGround=false;
	if(OnWater){
		WSendTo(x2,y2,Prio);
		return;
	};
	if(Media==2){
		SendFlyTo(x2,y2,Prio);
		return;
	};
	if(PrioryLevel>Prio)return;
	if(!cpbMoving)return;
	int x1=x2;
	int y1=y2;
	if(TrMap[y1][x1]){
		//Находим ближайшую незанятую точку
		bool fnf=true;
		for(int i=1;i<30;i++){
			int z=i<<1;
			for(int j=0;j<z;j++)
				if(!TrMap[y1-i][x1-i+j]){
					x1-=i-j;
					y1-=i;
					goto Em_Found;
				};
			for(j=0;j<z;j++)
				if(!TrMap[y1-i+j][x1+i]){
					y1-=i-j;
					x1+=i;
					goto Em_Found;
				};
			for(j=0;j<z;j++)
				if(!TrMap[y1+i][x1+i-j]){
					y1+=i;
					x1+=i-j;
					goto Em_Found;
				};
			for(j=0;j<z;j++)
				if(!TrMap[y1+i-j][x1-i]){
					y1+=i-j;
					x1-=i;
					goto Em_Found;
				};
		};
	};
Em_Found:
	if(x==x1&&y==y1){
		if(int(LocalOrder)){
			if(!MemoryTime){
				if(MemoryTime)MemoryTime--;
				if(LocalOrder->OrderType==2){
					Order1* Loc1=LocalOrder->NextOrder;
					FreeOrdBlock(LocalOrder);
					LocalOrder=Loc1;
				};
			};
		};
		if(int(InLineCom))FreeAsmLink();
		//LoadAnimation(0,0,0);
		//LoadCurAnm(0);
		return;
	};
	//if(XYChanged)StandTime=0;
	MemoryTime=5;
	Order1* Or1=GetOrdBlock();
	if(!int(Or1))return;
	Or1->PrioryLevel=Prio&127;
	Or1->NextOrder=NULL;
	Or1->OrderType=2;
	Or1->OrderTime=0;
	Or1->DoLink=&SendToLink;
	Or1->info.MoveToXY.x=x1;
	Or1->info.MoveToXY.y=y1;
	Order1* LOR=LocalOrder;
	if(int(LOR)){
		//if(LOR->OrderType=2&&StandTime>5)TrMap[y][x]=1;
		ClearOrders();//reeOrdBlock(LocalOrder);
		if(LOR->OrderType!=2)StandTime=0; 
	}else StandTime=0;
	//StandTime=0;
	if(int(InLineCom)){
		FreeAsmLink();
		//LoadCurAnm(0);
	};
	LocalOrder=Or1;
	//OrderReport=NULL;
	//MessageKind=0;
	//Sender=0xFFFF;
	//if(Prio<128){
		//PushCmd(Index);
	//	return;
	//};
	//CreatePath(x1,y1);
	PrioryLevel=Prio&127;
}; 
void SendToLink(OneObject* OBJ){
	byte x1=OBJ->LocalOrder->info.MoveToXY.x;
	byte y1=OBJ->LocalOrder->info.MoveToXY.y;
	byte Prio=OBJ->LocalOrder->PrioryLevel;
	if(TrMap[y1][x1])OBJ->SendTo(x1,y1,Prio);else{
		byte x=OBJ->x;
		byte y=OBJ->y;
		if(x==x1&&y==y1){
			if(int(OBJ->LocalOrder)){
				if(OBJ->MemoryTime)OBJ->MemoryTime--;
				if(!OBJ->MemoryTime){
					//if(OBJ->LocalOrder->OrderType==2){
					Order1* Loc1=OBJ->LocalOrder->NextOrder;
					OBJ->FreeOrdBlock(OBJ->LocalOrder);
					OBJ->LocalOrder=Loc1;
					//};
				};
			};
			if(int(OBJ->InLineCom))OBJ->FreeAsmLink();
			//OBJ->LoadAnimation(0,0,0);
			//OBJ->LoadCurAnm(0);
			return;
		}else{
			if(int(OBJ->InLineCom)){
				OBJ->FreeAsmLink();
				//OBJ->LoadCurAnm(0);
			};
			OBJ->CreatePath(x1,y1);
			OBJ->MemoryTime=0;
		};
	};
};
//Атаковать объект с заданным индексом
void AttackObjLink(OneObject* OBJ);
void OneObject::AttackObj(word OID,int Prio){
	//assert(OID!=0xFFFF);
	if(!CheckAttAbility(this,OID))return;
	if(Prio>=16)StandGround=false;
	if(OnWater){
		WAttackObj(OID,Prio);
		return;
	};
	if(Media==2){
		FlyAttack(OID,Prio);
		return;
	};
	if(Prio<PrioryLevel/*&&!Attack*/)return;
	if(!Ready)return;
	if(!(cpbMoving||int(Weap)))return;
	ClearOrders();
	OneObject* OB=Group[OID];
	if(!int(OB)||OB->Sdoxlo)return;
	byte x1=OB->x;
	byte y1=OB->y;
	int sx=x1-x;
	int	sy=y1-y;
	int dx=abs(sx);
	int dy=abs(sy);
	int dst;
	if(dx>dy)dst=dx;
			else dst=dy;
	if((Attack&&dst>EnemyDist&&Prio<8)||(!Prio)&&dst>HREQRADIUS)return;
	if(Prio<16&&dst>MaxReplyDistance)return;
	Important=true;
	if(Attack){
		EnemyID=OID;
		EnemySN=OB->Serial;
		EnemyDist=dst;
		//SearchSupport(OID);
		return;
	};
	EnemyDist=dst;
	//SearchSupport(OID);
	if(NMask&OB->NMask)return;
	Order1* Or1=GetOrdBlock();
	assert(Or1);
	if(!int(Or1))return;
	Or1->PrioryLevel=Prio&127;
	Or1->NextOrder=LocalOrder;
	Or1->OrderType=3;//Атака
	Or1->OrderTime=0;
	Or1->DoLink=&AttackObjLink;
	Or1->info.MoveToObj.ObjIndex=OID;
	EnemyID=OID;
	EnemySN=OB->Serial;
	Attack=true;
	Order1* LOR=LocalOrder;
	if(int(InLineCom))FreeAsmLink();
	LocalOrder=Or1;
	//OrderReport=NULL;
	//MessageKind=0;
	//Sender=0xFFFF;
	PrioryLevel=Prio&127;
	if(CrowdRef)CrowdRef->AddToAttackQueue(OID);
};	
void AttackObjLink(OneObject* OBJ){
	word OID=OBJ->EnemyID;
	word OSN=OBJ->EnemySN;
	Order1* Or1;
	if(OID>8192){
		if(OBJ->InLineCom)OBJ->FreeAsmLink();
		Or1=OBJ->LocalOrder->NextOrder;
		OBJ->FreeOrdBlock(OBJ->LocalOrder);
		OBJ->LocalOrder=Or1;
		OBJ->Important=false;
		return;
	};
	OneObject* OB=Group[OID];
	OBJ->Important=true;
	//if(!OBJ->NeedNoHelp)OBJ->SearchSupport(OID);
	if((!int(OB))||OB->Sdoxlo||OB->NMask&OBJ->NMask||OSN!=OB->Serial){
		if(int(OBJ->InLineCom))OBJ->FreeAsmLink();
		if(int(OBJ->LocalOrder)){
			Or1=OBJ->LocalOrder->NextOrder;
			OBJ->FreeOrdBlock(OBJ->LocalOrder);
			OBJ->LocalOrder=Or1;
			OBJ->Important=false;
			return;
		};
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
	int sy=yo-OBJ->y;
	int ssx=((xo-OBJ->x)<<5)-OBJ->wepX;
	int	ssy=((yo-OBJ->y)<<5)-OBJ->wepY;
	int dx=abs(sx);
	int dy=abs(sy);
	int dst;
	Visuals* VS=OBJ->Ref.Visual;
	if(dx>dy)dst=dx;
			else dst=dy;
	OBJ->EnemyDist=dst;
	if(dst<2&&OBJ->cpbMoving&&!OBJ->delay){
		OBJ->Direction=drr[(sx+1)*3+sy+1];
		OBJ->LoadAnimation(2,2,0);
		OBJ->LoadCurAnm(2);
		int Midm=(VS->info.Basic.MinDamage*OBJ->xForce)>>4;
		int Madm=(VS->info.Basic.MaxDamage*OBJ->xForce)>>4;
		//Dam=(Dam*OBJ->xForce)>>4;
		//if(OB->Life>Dam)OB->Life-=Dam;else OB->Life=0;
		OB->MakeDamage(Madm,Midm,OBJ);
		AddWarEffect(OB->x,OB->y,VS->HitSound);
		OBJ->isMoving=false;
		OBJ->delay=OBJ->MaxDelay;
		//TrMap[OBJ->y][OBJ->x]=1;
	}else{ 
		if(OBJ->delay&&OBJ->Use_AI&&VS->CanFear&&OBJ->cpbMoving&&!OBJ->StandGround){
			if(sx>0)sx=1;
			if(sy>0)sy=1;
			if(sx<0)sx=-1;
			if(sy<0)sy=-1;
			if(!TrMap[OBJ->y-sy][OBJ->x-sx]){
				OBJ->CreatePath(OBJ->x-sx,OBJ->y-sy);
				return;
			};
			if(!TrMap[OBJ->y][OBJ->x-sx]){
				OBJ->CreatePath(OBJ->x-sx,OBJ->y);
				return;
			};
			if(!TrMap[OBJ->y-sy][OBJ->x]){
				OBJ->CreatePath(OBJ->x,OBJ->y-sy);
				return;
			};
			return;
		};
		if(dst<VS->info.Basic.AttackRange&&dst>VS->DangerZone){
			if(OBJ->Weap&&!OBJ->delay){
				OBJ->isMoving=false;
				OBJ->delay=OBJ->Ref.General->WepDelay;
				if(OBJ->cpbMoving){
					OBJ->LoadAnimation(2,2,0);
					OBJ->LoadCurAnm(2);
					//TrMap[OBJ->y][OBJ->x]=1;
					OBJ->Direction=
						CreateUniExObj(OBJ->Weap,((OBJ->x<<5)+OBJ->wepX)<<2,((OBJ->y<<5)+OBJ->wepY)<<2,OBJ->Ref.General->WepSpeed,OBJ->NMask,OBJ,OB->x,OB->y,OB->Index);
					//OBJ->MakeDamage(0,OBJ->Ref.General->LifeShotLost,OB);
				}else{
					CreateUniExObj(OBJ->Weap,((OBJ->x<<5)+OBJ->wepX)<<2,((OBJ->y<<5)+OBJ->wepY)<<2,OBJ->Ref.General->WepSpeed,OBJ->NMask,OBJ,OB->x,OB->y,OB->Index);
					//OBJ->MakeDamage(0,OBJ->Ref.General->LifeShotLost,OB);
				};
			};
		}else
		if(OBJ->cpbMoving&&!OBJ->StandGround)OBJ->CreateSimplePath(xo,yo);
	};
};
//Complex Атаковать объект с заданным индексом
void ComplexAttackLink(OneObject* OBJ);
void OneObject::ComplexAttack(word OID,byte wep,int Prio){
	if(Prio>=16)StandGround=false;
	if(OnWater){
		WAttackObj(OID,Prio);
		return;
	};
	if(Media==2){
		FlyAttack(OID,Prio);
		return;
	};
	if(Ref.General->NWeap<=wep)return;
	if(Prio<PrioryLevel&&!Attack)return;
	if(!Ready)return;
	if(!(cpbMoving||int(Weap)))return;
	OneObject* OB=Group[OID];
	if(!int(OB)||OB->Sdoxlo)return;
	if(NMask&OB->NMask)return;
	Order1* Or1=GetOrdBlock();
	if(!int(Or1))return;
	Or1->PrioryLevel=Prio&127;
	Or1->NextOrder=LocalOrder;
	Or1->OrderType=34;//Атака
	Or1->OrderTime=0;
	Or1->DoLink=&ComplexAttackLink;
	Or1->info.MoveToObj.ObjIndex=OID;
	Or1->info.MoveToObj.wep=wep;
	Order1* LOR=LocalOrder;
	if(int(InLineCom))FreeAsmLink();
	LocalOrder=Or1;
	//OrderReport=NULL;
	//MessageKind=0;
	//Sender=0xFFFF;
	PrioryLevel=Prio&127;
};	
void ComplexAttackLink(OneObject* OBJ){
	word OID=OBJ->LocalOrder->info.MoveToObj.ObjIndex;
	byte wep=OBJ->LocalOrder->info.MoveToObj.wep;
	OneObject* OB=Group[OID];
	OBJ->Important=true;
	//if(!OBJ->NeedNoHelp)OBJ->SearchSupport(OID);
	Order1* Or1;
	if((!int(OB))||OB->Sdoxlo||OB->NMask&OBJ->NMask){
		if(int(OBJ->InLineCom))OBJ->FreeAsmLink();
		if(int(OBJ->LocalOrder)){
			Or1=OBJ->LocalOrder->NextOrder;
			OBJ->FreeOrdBlock(OBJ->LocalOrder);
			OBJ->LocalOrder=Or1;
			OBJ->Important=false;
			return;
		};
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
	int sy=yo-OBJ->y;
	int ssx=((xo-OBJ->x)<<5)-OBJ->wepX;
	int	ssy=((yo-OBJ->y)<<5)-OBJ->wepY;
	int dx=abs(sx);
	int dy=abs(sy);
	int dst;
	if(dx>dy)dst=dx;
			else dst=dy;
	OBJ->EnemyDist=dst;
	Visuals* VS=OBJ->Ref.Visual;
	SWPAR* SWX=&OBJ->Nat->SWP[VS->SWPIndex[wep]];
	if(dst<2&&OBJ->cpbMoving&&!OBJ->delay){
		OBJ->Direction=drr[(sx+1)*3+sy+1];
		OBJ->LoadAnimation(2,2,0);
		OBJ->LoadCurAnm(2);
		int Midm=OBJ->Ref.Visual->info.Basic.MinDamage;
		int Madm=OBJ->Ref.Visual->info.Basic.MaxDamage;
		int Dam=(((tmtmt&3)*(Madm-Midm))>>2)+Midm;
		//Dam=(Dam*OBJ->xForce)>>4;
		//if(OB->Life>Dam)OB->Life-=Dam;else OB->Life=0;
		OB->MakeDamage(Madm,Midm,OBJ);
		OBJ->isMoving=false;
		OBJ->delay=OBJ->MaxDelay;
	}else{ 
		Visuals* VS=OBJ->Ref.Visual;
		if(dst<SWX->Range&&dst>VS->DangerZone){
			if(OBJ->Weap&&!OBJ->delay){
				OBJ->isMoving=false;
				OBJ->delay=OBJ->MaxDelay;
				if(OBJ->cpbMoving){
					OBJ->LoadAnimation(2,2,0);
					OBJ->LoadCurAnm(2);
					if(SWX->Fly)
						OBJ->Direction=
							//CreateExObjR(OBJ->Ref.General->MWeap[wep],((OBJ->x<<5)+OBJ->wepX)<<2,((OBJ->y<<5)+OBJ->wepY)<<2,ssx,ssy,64,OBJ->NMask,OBJ,SWX->Range);
							CreateUniExObj(OBJ->Ref.General->MWeap[wep],((OBJ->x<<5)+OBJ->wepX)<<2,((OBJ->y<<5)+OBJ->wepY)<<2,OBJ->Ref.General->WepSpeed,OBJ->NMask,OBJ,OB->x,OB->y,OB->Index);
					else
						OBJ->Direction=
						//CreateExObjR(OBJ->Ref.General->MWeap[wep]->NextWeapon,((OB->x<<5)+512)<<2,((OB->y<<5)+512)<<2,ssx,ssy,0,OBJ->NMask,OBJ,SWX->Range);
						CreateUniExObj(OBJ->Ref.General->MWeap[wep]->NextWeapon,((OB->x<<5)+16)<<2,((OB->y<<5)+16)<<2,0,OBJ->NMask,OBJ,255,255,0xFFFF);
					//OBJ->MakeDamage(0,OBJ->Ref.General->LifeShotLost,OB);
				}else{
					if(SWX->Fly)CreateUniExObj(OBJ->Ref.General->MWeap[wep],((OBJ->x<<5)+OBJ->wepX)<<2,((OBJ->y<<5)+OBJ->wepY)<<2,OBJ->Ref.General->WepSpeed,OBJ->NMask,OBJ,OB->x,OB->y,OB->Index);
					else CreateUniExObj(OBJ->Ref.General->MWeap[wep]->NextWeapon,((OB->x<<5)+16)<<2,((OB->y<<5)+16)<<2,0,OBJ->NMask,OBJ,255,255,0xFFFF);
					//OBJ->MakeDamage(0,OBJ->Ref.General->LifeShotLost,OB);
				};
			};
			Order1* OR1=OBJ->LocalOrder;
			OBJ->LocalOrder=OR1->NextOrder;
			OBJ->FreeOrdBlock(OR1);
		}else
		if(OBJ->cpbMoving&&!OBJ->StandGround)OBJ->CreatePath(xo,yo);
	};
};
//Атаковать point
void AttackPointLink(OneObject* OBJ);
void OneObject::AttackPoint(byte x,byte y,byte wep,int Prio){
	if(Prio>=16)StandGround=false;
	if(OnWater){
		//WAttackObj(OID,Prio);
		return;
	};
	if(Media==2){
		//FlyAttack(OID,Prio);
		return;
	};
	if(Ref.General->NWeap<=wep)return;
	if(Prio<PrioryLevel)return;
	if(!Ready)return;
	if(!(cpbMoving||int(Weap)))return;
	Order1* Or1=GetOrdBlock();
	if(!int(Or1))return;
	Or1->PrioryLevel=Prio&127;
	Or1->NextOrder=LocalOrder;
	Or1->OrderType=33;//Атака
	Or1->OrderTime=0;
	Or1->DoLink=&AttackPointLink;
	Or1->info.AttackXY.x=x;
	Or1->info.AttackXY.y=y;
	Or1->info.AttackXY.wep=wep;
	//EnemyID=OID;
	Order1* LOR=LocalOrder;
	if(int(InLineCom))FreeAsmLink();
	LocalOrder=Or1;
	//OrderReport=NULL;
	//MessageKind=0;
	//Sender=0xFFFF;
	PrioryLevel=Prio&127;
};	
void AttackPointLink(OneObject* OBJ){
	int xd=OBJ->LocalOrder->info.AttackXY.x;
	int yd=OBJ->LocalOrder->info.AttackXY.y;
	byte wep=OBJ->LocalOrder->info.AttackXY.wep;
	//word OID=OBJ->EnemyID;
	//OneObject* OB=Group[OID];
	OBJ->Important=true;
	//if(!OBJ->NeedNoHelp)OBJ->SearchSupport(OID);
	int ssx=((xd-OBJ->x)<<5)-OBJ->wepX;
	int	ssy=((yd-OBJ->y)<<5)-OBJ->wepY;
	int sx=xd-OBJ->x;
	int sy=yd-OBJ->y;
	int dx=abs(sx);
	int dy=abs(sy);
	int dst;
	if(dx>dy)dst=dx;
			else dst=dy;
	OBJ->EnemyDist=dst;
	Visuals* VS=OBJ->Ref.Visual;
	SWPAR* SWX=&OBJ->Nat->SWP[VS->SWPIndex[wep]];
	if(dst<SWX->Range&&dst>VS->DangerZone){
		if(wep<OBJ->Ref.General->NWeap){
			OBJ->isMoving=false;
			if(OBJ->cpbMoving){
				OBJ->LoadAnimation(2,2,0);
				OBJ->LoadCurAnm(2);
				if(SWX->Fly)
					OBJ->Direction=
						CreateUniExObj(OBJ->Ref.General->MWeap[wep],((OBJ->x<<5)+OBJ->wepX)<<2,((OBJ->y<<5)+OBJ->wepY)<<2,OBJ->Ref.General->WepSpeed,OBJ->NMask,OBJ,xd,yd,0xFFFF);
						//CreateExObjDPointR(OBJ->Ref.General->MWeap[wep],((OBJ->x<<5)+OBJ->wepX)<<2,((OBJ->y<<5)+OBJ->wepY)<<2,ssx,ssy,64,OBJ->NMask,OBJ,xd,yd,SWX->Range);
				else 
					OBJ->Direction=
						CreateUniExObj(OBJ->Ref.General->MWeap[wep]->NextWeapon,((xd<<5)+16)<<2,((yd<<5)+16)<<2,0,OBJ->NMask,OBJ,xd,yd,0xFFFF);
						//CreateExObjDPointR(OBJ->Ref.General->MWeap[wep]->NextWeapon,((xd<<5)+16)<<2,((yd<<5)+16)<<2,ssx,ssy,64,OBJ->NMask,OBJ,xd,yd,SWX->Range);
				//OBJ->MakeDamage(OBJ->Ref.General->LifeShotLost,OB);
			}else{
				if(SWX->Fly)CreateUniExObj(OBJ->Ref.General->MWeap[wep],((OBJ->x<<5)+OBJ->wepX)<<2,((OBJ->y<<5)+OBJ->wepY)<<2,OBJ->Ref.General->WepSpeed,OBJ->NMask,OBJ,xd,yd,0xFFFF);
				else CreateUniExObj(OBJ->Ref.General->MWeap[wep]->NextWeapon,((xd<<5)+16)<<2,((yd<<5)+16)<<2,0,OBJ->NMask,OBJ,xd,yd,0xFFFF);
				//CreateExObj(OBJ->Weap,((OBJ->x<<5)+OBJ->wepX)<<2,((OBJ->y<<5)+OBJ->wepY)<<2,ssx,ssy,64,OBJ->NMask,OBJ);
				//OBJ->MakeDamage(OBJ->Ref.General->LifeShotLost,OB);
			};
			Order1* Loc1=OBJ->LocalOrder;
			OBJ->LocalOrder=Loc1->NextOrder;
			OBJ->FreeOrdBlock(Loc1);
		};
	}else
		if(OBJ->cpbMoving&&!OBJ->StandGround)OBJ->CreatePath(xd,yd);
	
};
//Атаковать point
void ContinueAttackPointLink(OneObject* OBJ);
void OneObject::ContinueAttackPoint(byte x,byte y,int Prio){
	if(Prio>=16)StandGround=false;
	if(OnWater){
		//WAttackObj(OID,Prio);
		return;
	};
	if(Media==2){
		//FlyAttack(OID,Prio);
		return;
	};
	if(Ref.Visual->info.Basic.AttackRange<2||!Weap)return;
	
	if(Prio<PrioryLevel)return;
	if(!Ready)return;
	if(!(cpbMoving||int(Weap)))return;
	Order1* Or1=GetOrdBlock();
	if(!int(Or1))return;
	ClearOrders();
	Or1->PrioryLevel=Prio&127;
	Or1->NextOrder=LocalOrder;
	Or1->OrderType=34;//Атака
	Or1->OrderTime=0;
	Or1->DoLink=&ContinueAttackPointLink;
	Or1->info.AttackXY.x=x;
	Or1->info.AttackXY.y=y;
	//EnemyID=OID;
	Order1* LOR=LocalOrder;
	if(int(InLineCom))FreeAsmLink();
	LocalOrder=Or1;
	//OrderReport=NULL;
	//MessageKind=0;
	//Sender=0xFFFF;
	PrioryLevel=Prio&127;
};	
void ContinueAttackPointLink(OneObject* OBJ){
	int xd=OBJ->LocalOrder->info.AttackXY.x;
	int yd=OBJ->LocalOrder->info.AttackXY.y;
	//word OID=OBJ->EnemyID;
	//OneObject* OB=Group[OID];
	OBJ->Important=true;
	//if(!OBJ->NeedNoHelp)OBJ->SearchSupport(OID);
	int ssx=((xd-OBJ->x)<<5)-OBJ->wepX;
	int	ssy=((yd-OBJ->y)<<5)-OBJ->wepY;
	int sx=xd-OBJ->x;
	int sy=yd-OBJ->y;
	int dx=abs(sx);
	int dy=abs(sy);
	int dst;
	if(dx>dy)dst=dx;
			else dst=dy;
	OBJ->EnemyDist=dst;
	Visuals* VS=OBJ->Ref.Visual;
	if(dst<VS->info.Basic.AttackRange&&dst>VS->DangerZone){
		if(!OBJ->delay){
			OBJ->delay=OBJ->Ref.General->WepDelay;
			OBJ->isMoving=false;
			if(OBJ->cpbMoving){
				OBJ->LoadAnimation(2,2,0);
				OBJ->LoadCurAnm(2);
				OBJ->Direction=
					CreateUniExObj(OBJ->Weap,((OBJ->x<<5)+OBJ->wepX)<<2,((OBJ->y<<5)+OBJ->wepY)<<2,OBJ->Ref.General->WepSpeed,OBJ->NMask,OBJ,xd,yd,0xFFFF);
			}else{
				CreateUniExObj(OBJ->Weap,((OBJ->x<<5)+OBJ->wepX)<<2,((OBJ->y<<5)+OBJ->wepY)<<2,OBJ->Ref.General->WepSpeed,OBJ->NMask,OBJ,xd,yd,0xFFFF);
			};
		};
	}else
		if(OBJ->cpbMoving&&!OBJ->StandGround)OBJ->CreatePath(xd,yd);
	
};
//Атаковать стену
void ContinueAttackWallLink(OneObject* OBJ);
void OneObject::ContinueAttackWall(byte x,byte y,int Prio){
	if(Prio>=16)StandGround=false;
	if(OnWater){
		//WAttackObj(OID,Prio);
		return;
	};
	if(Media==2){
		//FlyAttack(OID,Prio);
		return;
	};
	if(Ref.Visual->info.Basic.AttackRange<2||!Weap)return;
	
	if(Prio<PrioryLevel)return;
	if(!Ready)return;
	if(!(cpbMoving||int(Weap)))return;
	Order1* Or1=GetOrdBlock();
	if(!int(Or1))return;
	ClearOrders();
	Or1->PrioryLevel=Prio&127;
	Or1->NextOrder=LocalOrder;
	Or1->OrderType=34;//Атака
	Or1->OrderTime=0;
	Or1->DoLink=&ContinueAttackWallLink;
	Or1->info.AttackXY.x=x;
	Or1->info.AttackXY.y=y;
	//EnemyID=OID;
	Order1* LOR=LocalOrder;
	if(int(InLineCom))FreeAsmLink();
	LocalOrder=Or1;
	//OrderReport=NULL;
	//MessageKind=0;
	//Sender=0xFFFF;
	PrioryLevel=Prio&127;
};	
void ContinueAttackWallLink(OneObject* OBJ){
	int xd=OBJ->LocalOrder->info.AttackXY.x;
	int yd=OBJ->LocalOrder->info.AttackXY.y;
	//word OID=OBJ->EnemyID;
	//OneObject* OB=Group[OID];
	OBJ->Important=true;
	//if(!OBJ->NeedNoHelp)OBJ->SearchSupport(OID);
	word LNK=Links[yd][xd];
	if(LNK==0xFFFF){
		if(Links[yd][xd-1]!=0xFFFF)xd--;
		else if(Links[yd-1][xd]!=0xFFFF)yd--;
		else if(Links[yd][xd+1]!=0xFFFF)xd++;
		else if(Links[yd+1][xd]!=0xFFFF)yd++;
	};
	if(Links[yd][xd]==0xFFFF){
		OBJ->ClearOrders();
		return;
	};
	OBJ->LocalOrder->info.AttackXY.x=xd;
	OBJ->LocalOrder->info.AttackXY.y=yd;
	int ssx=((xd-OBJ->x)<<5)-OBJ->wepX;
	int	ssy=((yd-OBJ->y)<<5)-OBJ->wepY;
	int sx=xd-OBJ->x;
	int sy=yd-OBJ->y;
	int dx=abs(sx);
	int dy=abs(sy);
	int dst;
	if(dx>dy)dst=dx;
			else dst=dy;
	OBJ->EnemyDist=dst;
	Visuals* VS=OBJ->Ref.Visual;
	if(dst<VS->info.Basic.AttackRange&&dst>VS->DangerZone){
		if(!OBJ->delay){
			OBJ->delay=OBJ->Ref.General->WepDelay;
			OBJ->isMoving=false;
			if(OBJ->cpbMoving){
				OBJ->LoadAnimation(2,2,0);
				OBJ->LoadCurAnm(2);
				OBJ->Direction=
					CreateUniExObj(OBJ->Weap,((OBJ->x<<5)+OBJ->wepX)<<2,((OBJ->y<<5)+OBJ->wepY)<<2,OBJ->Ref.General->WepSpeed,OBJ->NMask,OBJ,xd,yd,0xFFFF);
			}else{
				CreateUniExObj(OBJ->Weap,((OBJ->x<<5)+OBJ->wepX)<<2,((OBJ->y<<5)+OBJ->wepY)<<2,OBJ->Ref.General->WepSpeed,OBJ->NMask,OBJ,xd,yd,0xFFFF);
			};
		};
	}else
		if(OBJ->cpbMoving&&!OBJ->StandGround)OBJ->CreatePath(xd,yd);
	
};
//Процедуры оптимизации уже сгенерированного пути
//1. Простейшая оптимизация-анализ самопересечения 
//Используется,когда совсем мало времени(not used now)
byte xx[MaxP+160];
byte yy[MaxP+160];
int Optima1(int Npt){
	//Создаем след на карте
//	return Npt;
	if(!Npt)return 0;
	int NN;
	__asm{
		mov		eax,SIndex
		xor		ebx,ebx
		xor		ecx,ecx
		xor		esi,esi
		xor		edi,edi
		mov		esi,Npt
F_begin:
		mov		dx,word ptr xx[edi]
		mov		bx,word ptr yy[edi]
		mov		ch,bl
		mov		cl,dl
		mov		byte ptr [xi+ecx],dh
		mov		byte ptr [yi+ecx],bh
		inc		edi
		dec		esi
		jnz		F_Begin
		mov		byte ptr[xi+ecx],0//end of array
		//Теперь путь готов, осталось его только собрать по цепочке
		mov		bl,byte ptr [xx]
		mov		bh,byte ptr [yy]
		xor		eax,eax
F_Lp1:
		mov		byte ptr [xx+eax],bl
		mov		byte ptr [yy+eax],bh
		mov		dl,byte ptr [xi+ebx]
		mov		dh,byte ptr [yi+ebx]
		mov		bx,dx
		inc		eax
		or		bl,bl
		jnz		F_Lp1
		mov		NN,eax
	};
	return NN;
};
bool ChkStright(int x0,int y0,int x1,int y1){
	int dx,dy,x,y;
	x=x0;y=y0;
	do{
		dx=x1-x;
		dy=y1-y;
		if(dx>1)dx=1;
		if(dx<-1)dx=-1;
		if(dy>1)dy=1;
		if(dy<-1)dy=-1;
		if(TrMap[y+dy][x+dx])return false;
		x+=dx;
		y+=dy;
	}while(x!=x1&&y!=y1);
	return true;
};
void OneObject::CreatePath(int x1,int y1){
	if(CPdestX==x1&&CPdestY==y1){
		if(PathX){
			if(CurIPoint<NIPoints){
				byte cx=PathX[CurIPoint];
				byte cy=PathY[CurIPoint];
				int dis=DistTo(cx,cy);
				if(dis<2)CurIPoint++;
				//else if(dis<4&&ChkStright(x,y,cx,cy))CurIPoint++;
				if(CurIPoint<NIPoints){
					CreateSimplePath(PathX[CurIPoint],PathY[CurIPoint]);
				}else{
					int xx1=x;
					int yy1=y;
					x=cx;
					y=cy;
					CreatePrePath(x1,y1);
					x=xx1;
					y=yy1;
				};
			};
		}else CreateSimplePath(x1,y1);
	}else{
		CreatePrePath(x1,y1);
	};
	CPdestX=x1;
	CPdestY=y1;
};
void OneObject::CreateSimplePath(int x1,int y1){
	PathAsks++;
	TrMap[y][x]=0;
	int sdx=x1-x;
	int	sdy=y1-y;
	int	Cum=0;
	int Pps=0;
	int sx=sdx;
	if(sx>1)sx=1;else
	if(sx<-1)sx=-1;
	int sy=sdy;
	if(sy>1)sy=1;else 
	if(sy<-1)sy=-1;
	int dx=abs(sdx);
	int dy=abs(sdy);
	int	Mdx=dx;
	int	Mdy=dy;
	int	Mx=x;
	int	My=y;
	int	xx1=x;
	int yy1=y;
	int rx=sx;
	int ry=sy;
	//if(dx>dy)ry=0;
	//if(dy>dx)rx=0;
	if(!TrMap[y+ry][x+rx]){
		LoadAnimation(1,AnmGoKind,0);
		AddAsk(Index,x,y,rx,ry);
		return;
	};
	//соединяем линией начальную и конечную точки. 
	//Оптимизация только по скорости
	__asm{
		mov		ax,word ptr Mdx
		mov		bx,word ptr Mdy
		xor		edx,edx  //Pps
		xor		ecx,ecx  //Cum
		mov		si,word ptr Mx
		mov		di,word ptr My
		cmp		bx,ax
		jae		Lp5xx
		//dx>dy
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		or		ax,ax
		jz		LoopsEnd
		cmp		sy,0
		jl		Lp3xx
		cmp		sx,0
		jl		Lp2begin
		//dx>dy,sx>0,sy>0
Lp1begin:
		inc		si	//x++
		add		cx,bx
		cmp		cx,word ptr Mdx
		jb		Lp1_1
		sub		cx,word ptr Mdx
		inc		di  //y++
Lp1_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		ax
		jnz		Lp1begin
		jmp		LoopsEnd
Lp2begin: //dx>dy,sx<0,sy>0
		dec		si	//x--
		add		cx,bx
		cmp		cx,word ptr Mdx
		jb		Lp2_1
		sub		cx,word ptr Mdx
		inc		di //y++
Lp2_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		ax
		jnz		Lp2begin
		jmp		LoopsEnd
Lp3xx:	//dy<0
		cmp		sx,0
		jl		Lp4begin
Lp3begin: //dx>dy,sx>0,sy<0
		inc		si	//x++
		add		cx,bx
		cmp		cx,word ptr Mdx
		jb		Lp3_1
		sub		cx,word ptr Mdx
		dec		di //y--
Lp3_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		ax
		jnz		Lp3begin
		jmp		LoopsEnd
Lp4begin: //dx>dy,sx<0,sy<0
		dec		si	//x--
		add		cx,bx
		cmp		cx,word ptr Mdx
		jb		Lp4_1
		sub		cx,word ptr Mdx
		dec		di //y--
Lp4_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		ax
		jnz		Lp4begin
		jmp		LoopsEnd
Lp5xx:	//dx<dy
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		or		bx,bx
		jz		LoopsEnd
		cmp		sx,0
		jl		Lp7xx
		cmp		sy,0
		jl		Lp6begin
Lp5Begin:
		inc		di	//y++
		add		cx,ax
		cmp		cx,word ptr dy
		jb		Lp5_1
		sub		cx,word ptr dy
		inc		si	//x++
Lp5_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		bx
		jnz		Lp5begin
		jmp		LoopsEnd
Lp6Begin://sx>0,sy<0
		dec		di	//y++
		add		cx,ax
		cmp		cx,word ptr dy
		jb		Lp6_1
		sub		cx,word ptr dy
		inc		si	//x++
Lp6_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		bx
		jnz		Lp6begin
		jmp		LoopsEnd
Lp7xx:	//dx<0
		cmp		sy,0
		jl		Lp8begin
Lp7Begin://dx<0,dy>0
		inc		di	//y++
		add		cx,ax
		cmp		cx,word ptr dy
		jb		Lp7_1
		sub		cx,word ptr dy
		dec		si	//x--
Lp7_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		bx
		jnz		Lp7begin
		jmp		LoopsEnd
Lp8Begin://dx<0,dy<0
		dec		di	//y--
		add		cx,ax
		cmp		cx,word ptr dy
		jb		Lp8_1
		sub		cx,word ptr dy
		dec		si	//x--
Lp8_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		bx
		jnz		Lp8begin
loopsEnd:
		//shr		edx,1
		mov		Pps,edx
	};
		Pps--;
/*this is a place for new code*/
	//Обходим справа
	bool RightPrefer=true;
	int Rtx;//current point 
	int Rty;
	int Ltx;
	int Lty;
	byte Rpx[MaxP];//right path
	byte Rpy[MaxP];
	byte Lpx[MaxP];//left path
	byte Lpy[MaxP];
	int Rpp=1;//index of current point
	bool LDoing,RDoing;//=true if last point reached
	byte Rdirc;//currend direction
	byte Ldirc;
	int Rmaxalt;//maximum alteration,right path
	int Lmaxalt;//maximum alteration,left path
	int Rppm=0;
	int Lppm=0;
	int Rcum=0;
	int Rcum1=0;
	int Lcum=0;
	int Lcum1=0;
	byte Trr=TrMap[y][x];
	TrMap[y][x]=0;
	//Идем, пока не упремся в стенку
	__asm{
		xor		ebx,ebx
		mov		ecx,Pps
		xor		edx,edx //Rpp
		mov		al,[xx+edx]
		mov		[Rpx+edx],al
		mov		[Lpx+edx],al
		mov		al,[yy+edx]
		mov		[Rpy+edx],al
		mov		[Lpy+edx],al
		inc		edx
uuu_Loop:
		mov		al,[xx+edx]
		mov		[Rpx+edx],al
		mov		[Lpx+edx],al
		mov		bl,al
		mov		al,[yy+edx]
		mov		[Rpy+edx],al
		mov		[Lpy+edx],al
		mov		bh,al
//		add		bx,0101h
		cmp		byte ptr[TrMap+ebx],0
		jnz		uuu_end
		inc		edx
		loop	uuu_Loop		
uuu_end:
		//shr		edx,1
		mov		Rpp,edx
	};
	Rtx=xx[Rpp-1];
	Rty=yy[Rpp-1];
	/*Rpx[0]=Rtx;
	Rpy[0]=Rty;
	Lpx[0]=Rtx;
	Lpy[0]=Rty;
	do{ 
		Rtx=xx[Rpp];
		Rty=yy[Rpp];
		Rpx[Rpp]=Rtx;
		Rpy[Rpp]=Rty;
		Lpx[Rpp]=Rtx;
		Lpy[Rpp]=Rty;
		Rpp++;
	}while(!TrMap[Rty][Rtx]&&(Rtx!=x1||Rty!=y1));*/
	// Если dx>dy,то на каждом шагу dx изменяетя строго на 1
	if(Rtx!=x1||Rty!=y1){
		//LLock[y][x]=false;
		Rpp-=1;
		Rtx=xx[Rpp];
		Rty=yy[Rpp];
		Ltx=xx[Rpp];
		Lty=yy[Rpp];
		int Ppi=Rpp+1;
		LDoing=true;
		RDoing=true;
		//Ищем, пока находимся в занятой зоне
		while(TrMap[yy[Ppi]][xx[Ppi]]&&Ppi<Pps)Ppi++;
		if(Ppi>Pps)RDoing=false;//Кон.точка недостижима
		int Xls=xx[Ppi-1];
		int Yls=yy[Ppi-1];
		//Уперлись...Вычисляем направление движения
		Rdirc=drr[(xx[Rpp+1]-xx[Rpp]+1)*3+yy[Rpp+1]-yy[Rpp]+1];
		Ldirc=Rdirc;
		//Выбираем начальное направление-right
		FillCirc(Rtx,Rty);
		int dirc1=(Rdirc+1)&7;
		for(int z=0;circ[dirc1]&&z<7;dirc1++,z++);
		Rdirc=dirc1&7;
		//-left
		dirc1=8+((Ldirc+7)&7);
		for(z=0;circ[dirc1]&&z<7;dirc1--,z++);
		Ldirc=dirc1&7;
		//Идем по правому краю до тех пор пока вновь не пересе-
		//чемся с прямой линией, соединяющей начальную и конечную
		//точки
		Rmaxalt=0;
		Lmaxalt=0;
		while(Rpp<MaxP-8&&LDoing&&RDoing){
			//пытаемся повернуть направо
			FillCirc(Rtx,Rty);
			int dirc1=(Rdirc+7)&7;
			for(int z=0;z<6&&circ[dirc1];dirc1++,z++);
			Rdirc=dirc1&7;
			Rpp++;
			int Tdx=idrx[Rdirc];
			int Tdy=idry[Rdirc];
			Rcum-=sdy*Tdx;
			Rcum+=sdx*Tdy;
			Rtx+=Tdx;
			Rty+=Tdy;
			Rpx[Rpp]=Rtx;Rpy[Rpp]=Rty;
			//the same, but left
			FillCirc(Ltx,Lty);
			dirc1=8+((Ldirc+1)&7);
			for(z=0;z<6&&circ[dirc1];dirc1--,z++);
			Ldirc=dirc1&7;
			Tdx=idrx[Ldirc];
			Tdy=idry[Ldirc];
			Lcum+=sdy*Tdx;
			Lcum-=sdx*Tdy;
			Ltx+=Tdx;
			Lty+=Tdy;
			Lpx[Rpp]=Ltx;Lpy[Rpp]=Lty;
			//Вычисляем значение y на прямой линии, соотв. 
			//данному x
			if(Rcum<=0&&Rcum1>=0){
				if(dx>dy){
					if(sx>0){
						if(Rtx>=Xls)RDoing=false;
					}else if(Rtx<=Xls)RDoing=false;
				}else{
					if(sy>0){
						if(Rty>=Yls)RDoing=false;
					}else if(Rty<=Yls)RDoing=false;
				};
				if(!RDoing)RightPrefer=true;
			};
			//Вычисляем максимальное отклонение от прямой линии
			if(Rcum>=Rmaxalt){
				Rmaxalt=Rcum;
				Rppm=Rpp;
			};
			Rcum1=Rcum;
			//the same for left
			if(Lcum<=0&&Lcum1>=0){
				if(dx>dy){
					if(sx>0){
						if(Ltx>=Xls)LDoing=false;
					}else if(Ltx<=Xls)LDoing=false;
				}else{
					if(sy>0){
						if(Lty>=Yls)LDoing=false;
					}else if(Lty<=Yls)LDoing=false;
				};
				if(!LDoing)RightPrefer=false;
			};
			//Вычисляем максимальное отклонение от прямой линии
			if(Lcum>=Lmaxalt){
				Lmaxalt=Lcum;
				Lppm=Rpp;
			};
			Lcum1=Lcum;
		};
		//LLock[y][x]=true;
		if(Rpp<MaxP-9){
			if(RightPrefer){
				if(Rppm+1<Rpp)Rppm+=1;else Rppm=Rpp;
				memcpy(xx,Rpx,(Rppm+1));
				memcpy(yy,Rpy,(Rppm+1));
				Pps=Rppm;
			}else{
				if(Lppm+1<Rpp)Lppm+=1;else Lppm=Rpp;
				memcpy(xx,Lpx,(Lppm+1));
				memcpy(yy,Lpy,(Lppm+1));
				Pps=Lppm;
			};
		}else return;
		//Оптимизация пути
		//Pps=Optima1(Pps);
	};
	TrMap[y][x]=Trr;
	NowBuf=GetAsmBlock();
	if(!int(NowBuf))return;
	Ofst=0;
	memcpy(&NowBuf[OneAsmSize-4],&Ofst,4);
	InLineCom=NowBuf;
	LineOffset=0;
	cmLoadAnm(AnmGoKind,1,0);
	//cmLoadAnm(1,1,0);
	cmLoadAnm(AnmStandKind,0,0);
	for(int i=1;i<=Pps;i++){
		//byte (*XXX)[64];
		//memcpy(&XXX,&NowBuf,4);
		//cmSetDir(xx[i]-xx[i-1],yy[i]-yy[i-1]);
		if(i==1)cmPerfAnm(0);//-------??????????????????????
		cmSetXY(xx[i]-xx[i-1],yy[i]-yy[i-1]);
		//cmPerfAnm(1);
		//cmSetXYDirX(xx[i],yy[i],xx[i]-xx[i-1],yy[i]-yy[i-1],1);
		//cmPerfAnm(1);
	};
	cmLoadAnm(AnmStandKind,0,0);
	cmPerfAnm(0);
	//cmDone();
	cmRet();
};
int GetRAngle(int dx,int dy,int Angle){
	int ang;
	int sx=abs(dx);
	int sy=abs(dy);
	if(!(dx||dy))return 0;
	if(sx>sy){
		ang=div(dy<<8,sx).quot;
		if(dx<0)ang=1024-ang;
	}else{
		ang=512-div(dx<<8,sy).quot;
		if(dy<0)ang=2048-ang;
	};
	ang=Angle-ang;
	while(ang<0)ang+=2048;
	if(ang>1792)ang-=2048;
	return ang;
};
int GetLAngle(int dx,int dy,int Angle){
	int ang;
	int sx=abs(dx);
	int sy=abs(dy);
	if(!(dx||dy))return 0;
	if(sx>sy){
		ang=div(dy<<8,sx).quot;
		if(dx<0)ang=1024-ang;
	}else{
		ang=512-div(dx<<8,sy).quot;
		if(dy<0)ang=2048-ang;
	};
	ang-=Angle;
	while(ang<0)ang+=2048;
	if(ang>1792)ang-=2048;
	return ang;
};
void OneObject::CreatePrePath(int x1,int y1){
	PathAsks++;
	TrMap[y][x]=0;
	int sdx=x1-x;
	int	sdy=y1-y;
	int	Cum=0;
	int Pps=0;
	int sx=(sdx>0)?1:-1;
	int sy=(sdy>0)?1:-1;
	int dx=abs(sdx);
	int dy=abs(sdy);
	int	Mdx=dx;
	int	Mdy=dy;
	int	Mx=x;
	int	My=y;
	int	xx1=x;
	int yy1=y;
	int rx=sx;
	int ry=sy;
	if(dx>dy)ry=0;
	if(dy>dx)rx=0;
	int Angle0=GetLAngle(x1-x,y1-y,0);
	int Angle;
	int ddx,ddy;
	int Lvp=0;
	int Rvp=0;
	bool LvpLast=false;
	bool RvpLast=false;
	if(PathX)free(PathX);
	if(PathY)free(PathY);
	PathX=NULL;
	PathY=NULL;
	NIPoints=0;
	CurIPoint=0;
	NeedPath=false;
	//соединяем линией начальную и конечную точки. 
	//Оптимизация только по скорости
	__asm{
		mov		ax,word ptr Mdx
		mov		bx,word ptr Mdy
		xor		edx,edx  //Pps
		xor		ecx,ecx  //Cum
		mov		si,word ptr Mx
		mov		di,word ptr My
		cmp		bx,ax
		jae		Lp5xx
		//dx>dy
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		or		ax,ax
		jz		LoopsEnd
		cmp		sy,0
		jl		Lp3xx
		cmp		sx,0
		jl		Lp2begin
		//dx>dy,sx>0,sy>0
Lp1begin:
		inc		si	//x++
		add		cx,bx
		cmp		cx,word ptr Mdx
		jb		Lp1_1
		sub		cx,word ptr Mdx
		inc		di  //y++
Lp1_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		ax
		jnz		Lp1begin
		jmp		LoopsEnd
Lp2begin: //dx>dy,sx<0,sy>0
		dec		si	//x--
		add		cx,bx
		cmp		cx,word ptr Mdx
		jb		Lp2_1
		sub		cx,word ptr Mdx
		inc		di //y++
Lp2_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		ax
		jnz		Lp2begin
		jmp		LoopsEnd
Lp3xx:	//dy<0
		cmp		sx,0
		jl		Lp4begin
Lp3begin: //dx>dy,sx>0,sy<0
		inc		si	//x++
		add		cx,bx
		cmp		cx,word ptr Mdx
		jb		Lp3_1
		sub		cx,word ptr Mdx
		dec		di //y--
Lp3_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		ax
		jnz		Lp3begin
		jmp		LoopsEnd
Lp4begin: //dx>dy,sx<0,sy<0
		dec		si	//x--
		add		cx,bx
		cmp		cx,word ptr Mdx
		jb		Lp4_1
		sub		cx,word ptr Mdx
		dec		di //y--
Lp4_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		ax
		jnz		Lp4begin
		jmp		LoopsEnd
Lp5xx:	//dx<dy
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		or		bx,bx
		jz		LoopsEnd
		cmp		sx,0
		jl		Lp7xx
		cmp		sy,0
		jl		Lp6begin
Lp5Begin:
		inc		di	//y++
		add		cx,ax
		cmp		cx,word ptr dy
		jb		Lp5_1
		sub		cx,word ptr dy
		inc		si	//x++
Lp5_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		bx
		jnz		Lp5begin
		jmp		LoopsEnd
Lp6Begin://sx>0,sy<0
		dec		di	//y++
		add		cx,ax
		cmp		cx,word ptr dy
		jb		Lp6_1
		sub		cx,word ptr dy
		inc		si	//x++
Lp6_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		bx
		jnz		Lp6begin
		jmp		LoopsEnd
Lp7xx:	//dx<0
		cmp		sy,0
		jl		Lp8begin
Lp7Begin://dx<0,dy>0
		inc		di	//y++
		add		cx,ax
		cmp		cx,word ptr dy
		jb		Lp7_1
		sub		cx,word ptr dy
		dec		si	//x--
Lp7_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		bx
		jnz		Lp7begin
		jmp		LoopsEnd
Lp8Begin://dx<0,dy<0
		dec		di	//y--
		add		cx,ax
		cmp		cx,word ptr dy
		jb		Lp8_1
		sub		cx,word ptr dy
		dec		si	//x--
Lp8_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		bx
		jnz		Lp8begin
loopsEnd:
		//shr		edx,1
		mov		Pps,edx
	};
		Pps--;
/*this is a place for new code*/
	//Обходим справа
	bool RightPrefer=true;
	int Rtx;//current point 
	int Rty;
	int Ltx;
	int Lty;
	byte Rpx[MaxP];//right path
	byte Rpy[MaxP];
	byte Lpx[MaxP];//left path
	byte Lpy[MaxP];
	int Rpp=1;//index of current point
	bool LDoing;//=true if last point reached
	bool RDoing;
	byte Rdirc;//currend direction
	byte Ldirc;
	int Rmaxalt;//maximum alteration,right path
	int Lmaxalt;//maximum alteration,left path
	int Rppm=0;
	int Lppm=0;
	int Rcum=0;
	int Rcum1=0;
	int Lcum=0;
	int Lcum1=0;
	byte Trr=TrMap[y][x];
	TrMap[y][x]=0;
	//Идем, пока не упремся в стенку
	__asm{
		xor		ebx,ebx
		mov		ecx,Pps
		xor		edx,edx //Rpp
		mov		al,[xx+edx]
		mov		[Rpx+edx],al
		mov		[Lpx+edx],al
		mov		al,[yy+edx]
		mov		[Rpy+edx],al
		mov		[Lpy+edx],al
		inc		edx
uuu_Loop:
		mov		al,[xx+edx]
		mov		[Rpx+edx],al
		mov		[Lpx+edx],al
		mov		bl,al
		mov		al,[yy+edx]
		mov		[Rpy+edx],al
		mov		[Lpy+edx],al
		mov		bh,al
//		add		bx,0101h
		cmp		byte ptr[TrMap+ebx],0
		jnz		uuu_end
		inc		edx
		loop	uuu_Loop		
uuu_end:
		//shr		edx,1
		mov		Rpp,edx
	};
	Rtx=xx[Rpp-1];
	Rty=yy[Rpp-1];
	
	// Если dx>dy,то на каждом шагу dx изменяетя строго на 1
	if(Rtx!=x1||Rty!=y1){
		//LLock[y][x]=false;
		Rpp-=1;
		Rtx=xx[Rpp];
		Rty=yy[Rpp];
		Ltx=xx[Rpp];
		Lty=yy[Rpp];
		int Ppi=Rpp+1;
		LDoing=true;
		RDoing=true;
		//Ищем, пока находимся в занятой зоне
		while(TrMap[yy[Ppi]][xx[Ppi]]&&Ppi<Pps)Ppi++;
		if(Ppi>Pps)LDoing=false;//Кон.точка недостижима
		int Xls=xx[Ppi-1];
		int Yls=yy[Ppi-1];
		//Уперлись...Вычисляем направление движения
		Rdirc=drr[(xx[Rpp+1]-xx[Rpp]+1)*3+yy[Rpp+1]-yy[Rpp]+1];
		Ldirc=Rdirc;
		//Выбираем начальное направление-right
		FillCirc(Rtx,Rty);
		int dirc1=(Rdirc+1)&7;
		for(int z=0;circ[dirc1]&&z<7;dirc1++,z++);
		Rdirc=dirc1&7;
		//-left
		dirc1=8+((Ldirc+7)&7);
		for(z=0;circ[dirc1]&&z<7;dirc1--,z++);
		Ldirc=dirc1&7;
		//Идем по правому краю до тех пор пока вновь не пересе-
		//чемся с прямой линией, соединяющей начальную и конечную
		//точки
		Rmaxalt=0;
		Lmaxalt=0;
		while(Rpp<MaxP-8&&LDoing&&RDoing){
			//пытаемся повернуть направо
			FillCirc(Rtx,Rty);
			int dirc1=(Rdirc+7)&7;
			for(int z=0;z<6&&circ[dirc1];dirc1++,z++);
			Rdirc=dirc1&7;
			Rpp++;
			int Tdx=idrx[Rdirc];
			int Tdy=idry[Rdirc];
			Rcum-=sdy*Tdx;
			Rcum+=sdx*Tdy;
			Rtx+=Tdx;
			Rty+=Tdy;
			Rpx[Rpp]=Rtx;Rpy[Rpp]=Rty;
			Angle=GetLAngle(Rtx-x,Rty-y,Angle0);
			if(Angle>Rmaxalt){
				Rmaxalt=Angle;
				Rppm=Rpp;
			};
			//проверяем условие прямой видимости
			ddx=x1-Rtx;
			ddy=y1-Rty;
			if(ddx>1)ddx=1;
			if(ddx<-1)ddx=-1;
			if(ddy>1)ddy=1;
			if(ddy<-1)ddy=-1;
			if(!TrMap[Rty+ddy][Rtx+ddx]){
				if(!RvpLast){
					Rvp=Rpp;
					RvpLast=true;
				};
			}else RvpLast=false;
			//the same, but left
			FillCirc(Ltx,Lty);
			dirc1=8+((Ldirc+1)&7);
			for(z=0;z<6&&circ[dirc1];dirc1--,z++);
			Ldirc=dirc1&7;
			Tdx=idrx[Ldirc];
			Tdy=idry[Ldirc];
			Lcum+=sdy*Tdx;
			Lcum-=sdx*Tdy;
			Ltx+=Tdx;
			Lty+=Tdy;
			Lpx[Rpp]=Ltx;Lpy[Rpp]=Lty;
			Angle=GetRAngle(Ltx-x,Lty-y,Angle0);
			if(Angle>Lmaxalt){
				Lmaxalt=Angle;
				Lppm=Rpp;
			};
			//проверяем условие прямой видимости
			ddx=x1-Ltx;
			ddy=y1-Lty;
			if(ddx>1)ddx=1;
			if(ddx<-1)ddx=-1;
			if(ddy>1)ddy=1;
			if(ddy<-1)ddy=-1;
			if(!TrMap[Lty+ddy][Ltx+ddx]){
				if(!LvpLast){
					Lvp=Rpp;
					LvpLast=true;
				};
			}else LvpLast=false;
			//Вычисляем значение y на прямой линии, соотв. 
			//данному x
			if(Rcum<=0&&Rcum1>=0){
				if(dx>dy){
					if(sx>0){
						if(Rtx>=Xls)RDoing=false;
					}else if(Rtx<=Xls)RDoing=false;
				}else{
					if(sy>0){
						if(Rty>=Yls)RDoing=false;
					}else if(Rty<=Yls)RDoing=false;
				};
				if(!RDoing)RightPrefer=true;
			};
			Rcum1=Rcum;
			//the same for left
			if(Lcum<=0&&Lcum1>=0){
				if(dx>dy){
					if(sx>0){
						if(Ltx>=Xls)LDoing=false;
					}else if(Ltx<=Xls)LDoing=false;
				}else{
					if(sy>0){
						if(Lty>=Yls)LDoing=false;
					}else if(Lty<=Yls)LDoing=false;
				};
				if(!LDoing)RightPrefer=false;
			};
			Lcum1=Lcum;
		};
		//LLock[y][x]=true;
		if(Rpp<MaxP-9){
			if(RightPrefer){
				//if(Rppm+1<Rpp)Rppm+=1;else Rppm=Rpp;
				//memcpy(xx,Rpx,(Rppm+1));
				//memcpy(yy,Rpy,(Rppm+1));
				Pps=Rppm;
			}else{
				//if(Lppm+1<Rpp)Lppm+=1;else Lppm=Rpp;
				//memcpy(xx,Lpx,(Lppm+1));
				//memcpy(yy,Lpy,(Lppm+1));
				Pps=Lppm;
				Rvp=Lvp;
				
			};
			if(Rvp<Pps)Rvp=Pps;
			NIPoints=Rvp-Pps+1;
			int maxp=NIPoints;
			CurIPoint=0;
			if(NIPoints>2){
				NIPoints=2+((NIPoints-2)>>2);
			};
			PathX=new byte [NIPoints];
			PathY=new byte [NIPoints];
			if(RightPrefer){
				if(maxp<=2){
					memcpy(PathX,&Rpx[Pps],NIPoints);
					memcpy(PathY,&Rpy[Pps],NIPoints);
				}else{
					int np=((maxp-2)>>2)+1;
					for(int nn=0;nn<np;nn++){
						PathX[nn]=Rpx[Pps+(nn<<2)];
						PathY[nn]=Rpy[Pps+(nn<<2)];
					};
					PathX[nn]=Rpx[Pps+maxp-1];
					PathY[nn]=Rpy[Pps+maxp-1];
				};
			}else{
				if(maxp<=2){
					memcpy(PathX,&Lpx[Pps],NIPoints);
					memcpy(PathY,&Lpy[Pps],NIPoints);
				}else{
					int np=((maxp-2)>>2)+1;
					for(int nn=0;nn<np;nn++){
						PathX[nn]=Lpx[Pps+(nn<<2)];
						PathY[nn]=Lpy[Pps+(nn<<2)];
					};
					PathX[nn]=Lpx[Pps+maxp-1];
					PathY[nn]=Lpy[Pps+maxp-1];
				};
			};
			/*if(rando()<10000){
				bool ttt=rando()<13000;
				int vx,vy;
				for(int i=0;i<NIPoints;i++){
					vx=0;
					vy=0;
					int xxx=PathX[i];
					int yyy=PathY[i];
					for(int dd=0;dd<8;dd++){
						int dx=idrx[dd];
						int dy=idry[dd];
						if(TrMap[yyy+dy][xxx+dx]){
							vx-=dx;
							vy-=dy;
						};
					};
					if(vx>1)vx=-1;
					if(vx<-1)vx=-1;
					if(vy>1)vy=1;
					if(vy<-1)vy=-1;
					if(!TrMap[xxx+vx][yyy+vy]){
						if(ttt&&!TrMap[xxx+vx+vx][yyy+vy+vy]){
							PathX[i]=xxx+vx+vx;
							PathY[i]=yyy+vy+vy;
						}else{
							PathX[i]=xxx+vx;
							PathY[i]=yyy+vy;
						};
					};
				};
			};*/
			NeedPath=true;
		}else return;
	};
};
inline void ReMemCirc(byte x,byte y){
	__asm{
		xor		ebx,ebx
		mov		bh,byte ptr y
		mov		bl,byte ptr x
		add		ebx,offset TrMap
		mov		al,circ[0]
		mov		[ebx-256],al
		mov		al,circ[1]
		mov		[ebx-255],al
		mov		al,circ[2]
		mov		[ebx+1],al
		mov		al,circ[3]
		mov		[ebx+257],al
		mov		al,circ[4]
		mov		[ebx+256],al
		mov		al,circ[5]
		mov		[ebx+255],al
		mov		al,circ[6]
		mov		[ebx-1],al
		mov		al,circ[7]
		mov		[ebx-257],al
		xor		ebx,ebx
		mov		bh,byte ptr y
		mov		bl,byte ptr x
		add		ebx,offset LLock
		mov		al,circ[8]
		mov		[ebx-256],al
		mov		al,circ[9]
		mov		[ebx-255],al
		mov		al,circ[10]
		mov		[ebx+1],al
		mov		al,circ[11]
		mov		[ebx+257],al
		mov		al,circ[12]
		mov		[ebx+256],al
		mov		al,circ[13]
		mov		[ebx+255],al
		mov		al,circ[14]
		mov		[ebx-1],al
		mov		al,circ[15]
		mov		[ebx-257],al
	}
};
inline void MemCirc(byte x,byte y){
	__asm{
		xor		ebx,ebx
		mov		bh,byte ptr y
		mov		bl,byte ptr x
		add		ebx,offset TrMap
		mov		al,[ebx-256]
		mov		circ[0],al
		mov		al,[ebx-255]
		mov		circ[1],al
		mov		al,[ebx+1]
		mov		circ[2],al
		mov		al,[ebx+257]
		mov		circ[3],al
		mov		al,[ebx+256]
		mov		circ[4],al
		mov		al,[ebx+255]
		mov		circ[5],al
		mov		al,[ebx-1]
		mov		circ[6],al
		mov		al,[ebx-257]
		mov		circ[7],al
		xor		ebx,ebx
		mov		bh,byte ptr y
		mov		bl,byte ptr x
		add		ebx,offset LLock
		mov		al,[ebx-256]
		mov		circ[8],al
		mov		al,[ebx-255]
		mov		circ[9],al
		mov		al,[ebx+1]
		mov		circ[10],al
		mov		al,[ebx+257]
		mov		circ[11],al
		mov		al,[ebx+256]
		mov		circ[12],al
		mov		al,[ebx+255]
		mov		circ[13],al
		mov		al,[ebx-1]
		mov		circ[14],al
		mov		al,[ebx-257]
		mov		circ[15],al
	}
};
inline void FCirc(byte x,byte y,byte x0,byte y0,byte x1,byte y1){
	__asm{
		xor		ebx,ebx
		mov		bl,x
		mov		bh,y
		cmp		bl,x0
		jne		lb1
		mov		TrMap[ebx-257],1
		mov		TrMap[ebx-1],1
		mov		TrMap[ebx+255],1
		mov		LLock[ebx-257],1
		mov		LLock[ebx-1],1
		mov		LLock[ebx+255],1
lb1:	cmp		bl,x1
		jne		lb2
		mov		TrMap[ebx-255],1
		mov		TrMap[ebx+1],1
		mov		TrMap[ebx+257],1
		mov		LLock[ebx-255],1
		mov		LLock[ebx+1],1
		mov		LLock[ebx+257],1
lb2:	cmp		bh,y0
		jne		lb3
		mov		TrMap[ebx-257],1
		mov		TrMap[ebx-256],1
		mov		TrMap[ebx-255],1
		mov		LLock[ebx-257],1
		mov		LLock[ebx-256],1
		mov		LLock[ebx-255],1
lb3:	cmp		bh,y1
		jne		lb4
		mov		TrMap[ebx+255],1
		mov		TrMap[ebx+256],1
		mov		TrMap[ebx+257],1
		mov		LLock[ebx+255],1
		mov		LLock[ebx+256],1
		mov		LLock[ebx+257],1
lb4:
	};
};

//Order Type=77
//Свалить с этой точки.Нефиг зря стоять.dir-направление,
//в котором движентся обЪект,который предлагает данному
//свалить с дороги. в этом направлении сдвигаться лишь в
//крайнем случае
void MFLink(OneObject* OBJ);
void OneObject::MoveFrom(int dir){
	//Npush++;
	if(!cpbMoving)return;
	if(StandTime<3)return;
	bool initMXY=true;
	if(int(LocalOrder))
		if(LocalOrder->OrderType==77)initMXY=false;
	if(initMXY){
		Order1* Or1=GetOrdBlock();
		if(!int(Or1))return;
		Or1->PrioryLevel=0;
		Or1->NextOrder=LocalOrder;
		Or1->OrderType=77;
		Or1->OrderTime=0;
		Or1->info.MoveFrom.dir=dir;
		//if(int(LocalOrder))FreeOrdBlock(LocalOrder);
		if(int(InLineCom)){
			FreeAsmLink();
			//LoadCurAnm(0);
		};
		LocalOrder=Or1;
		Or1->DoLink=&MFLink;
		//OrderReport=NULL;
		//MessageKind=0;
		//Sender=0xFFFF;
		//MFLink(this);
	};
};
void MFLink(OneObject* OBJ){
	byte Direction=OBJ->Direction;
	byte dir=OBJ->LocalOrder->info.MoveFrom.dir;
	if(int(OBJ->InLineCom))OBJ->FreeAsmLink();
	OBJ->InLineCom=0;
	Order1* Loc1=OBJ->LocalOrder->NextOrder;
	OBJ->FreeOrdBlock(OBJ->LocalOrder);
	OBJ->LocalOrder=Loc1;
	int dx0=idrx[Direction];
	int	dy0=idry[Direction];
	int x=OBJ->x;
	int y=OBJ->y;
	if(OBJ->DrawUp){
		MemCirc(x,y);
		FCirc(x,y,OBJ->destX,OBJ->destY,OBJ->destX1,OBJ->destY1);
	};
	bool Dfound=false;
	int ddr;
	int tx;
	int ty;
	if(LLock[y+dy0][x+dx0]/*||dir==Direction*/){
		if((tmtmt&3)<2){
			//Turn right
			ddr=(Direction+1)&7;
			int z=0;
			do{
				dx0=idrx[ddr];
				dy0=idry[ddr];
				ddr=(ddr+1)&7;
				z++;
				tx=x+dx0;
				ty=y+dy0;
				if(tx>=0&&ty>0&&tx<msx&&ty<msy&&!LLock[ty][tx])break;
			}while(z<8);
			ddr=(ddr+7)&7;
			if(z<8)Dfound=true;
		} else{
			//Turn left
			int ddr=(Direction+7)&7;
			int z=0;
			do{
				dx0=idrx[ddr];
				dy0=idry[ddr];
				ddr=(ddr+7)&7;
				z++;
			}while(LLock[y+dy0][x+dx0]&&z<8);
			ddr=(ddr+1)&7;
			if(z<8)Dfound=true;
		};
	}else{
		Dfound=true;
		ddr=Direction;
	};
	if(!Dfound&&!LLock[y+dy0][x+dx0]/*&&dir==Direction*/){
		Dfound=true;
		ddr=Direction;
		dx0=idrx[ddr];
		dy0=idry[ddr];
	};
	if(!Dfound){
		int z=rando();
		if(z&1){
			z=z>>1;
			//Turn right
			ddr=(Direction+(z&1))&7;
			int z=0;
			do{
				dx0=idrx[ddr];
				dy0=idry[ddr];
				ddr=(ddr+1)&7;
				z++;
				tx=x+dx0;
				ty=y+dy0;
				if(tx>=0&&ty>0&&tx<msx&&ty<msy&&!TrMap[ty][tx])break;
			}while(z<8);
			ddr=(ddr+7)&7;
			if(z<8)Dfound=true;
		} else{
			//Turn left
			z=z>>1;
			int ddr=(Direction+8-(z&1))&7;
			int z=0;
			do{
				dx0=idrx[ddr];
				dy0=idry[ddr];
				ddr=(ddr+7)&7;
				z++;
			}while(TrMap[y+dy0][x+dx0]&&z<8);
			ddr=(ddr+1)&7;
			if(z<8)Dfound=true;
		};
		//ddr=rando()&7;
		//dx0=idrx[ddr];
		//dy0=idry[ddr];
		//ddr=(ddr+1)&7;
		//tx=x+dx0;
		//ty=y+dy0;
		if(tx>=0&&tx<msx&&ty>=0&&ty<msy)Dfound=true;
	};
	if(Dfound){
		int difR=8+ddr-Direction;
		difR&=7;
		int difL=8+Direction-ddr;
		difL&=7;
		byte minDr=difR<difL?difR:difL;
		if(int(OBJ->InLineCom))OBJ->FreeAsmLink();
		
		OBJ->LoadAnimation(0,OBJ->AnmStandKind,0);
		OBJ->LoadAnimation(1,OBJ->AnmGoKind,0);
		AddAsk(OBJ->Index,x,y,dx0,dy0);
	};
	if(OBJ->DrawUp)
		ReMemCirc(x,y);
};
void MakeExpl(int xx,int yy);
void RestoreLock(int x,int y,int lx,int ly);
void OneObject::Die(){
	if(PathX){
		free(PathX);
		free(PathY);
		PathX=NULL;
		PathY=NULL;
		NIPoints=0;
		CurIPoint=0;
	};
	GeneralObject* GO=Ref.General;
	if(!capBuilding)Nat->NGidot--;
	if(GO->DeathSound==-1){
		if(capBuilding)AddOrderEffect(x,y,Nat->BuildDieSound);
		else AddOrderEffect(x,y,Nat->UnitDieSound);

	}else{
		AddWarEffect(x,y,GO->DeathSound);
	};
	if(int(InLineCom))FreeAsmLink();
	do{
		if(int(LocalOrder)){
			Order1* Loc1=LocalOrder->NextOrder;
	
			FreeOrdBlock(LocalOrder);
			LocalOrder=Loc1;
		};
	}while(int(LocalOrder));
	Cell8x8* CELL=&TCInf[NNUM][y>>2][x>>2];
	CELL->UnitsAmount[Kind]--;
	if(NInside){
		for(int i=0;i<NInside;i++){
			word MID=Inside[i];
			OneObject* OB=Group[MID];
			if(OB->InLineCom)OB->FreeAsmLink();
			OB->ClearOrders();
			Group[MID]=NULL;
		};
		free(Inside);
	};
	if(OnWater){
		if(capBuilding){
			for(byte dx=0;dx<Lx;dx++)
			for(byte dy=0;dy<Ly;dy++){
				WLock[y+dy][x+dx]=0;
				WMap[y+dy][x+dx]=0;
				Mops[y+dy][x+dx]=0xFFFF;
			};
		}else{
			WMap[y][x]=0;
			WLock[y][x]=0;
			Mops[y][x]=0xFFFF;
		};
	}else{
		if(Media==2){
			int cx=x>>2;
			int cy=y>>2;
			FlyCell* FC=&FlyMap[cy][cx];
			for(int pp=0;pp<15;pp++){
				if(FC->FlyList[pp]==Index){
					FC->FlyList[pp]=0xFFFF;
					FC->NFly--;
				};
			};
			MakeExpl((RealX>>2)+(rando()&255)-128,(RealY>>2)+(rando()&255)-128);
			MakeExpl((RealX>>2)+(rando()&255)-128,(RealY>>2)+(rando()&255)-128);
			MakeExpl((RealX>>2)+(rando()&255)-128,(RealY>>2)+(rando()&255)-128);
			memset(&FlyMops[0][0],255,sizeof FlyMops);
			LoadCurAnm(2);
			TicksPerChange=1;
			Sdoxlo=true;
			Group[Index]=NULL;
			return;
		};
		RestoreLock(x,y,Lx,Lx);
		//for(byte dx=0;dx<Lx;dx++)
		//	for(byte dy=0;dy<Ly;dy++){
		//		LLock[y+dy][x+dx]=0;
		//		DecLock(x+dx,y+dy);
		//		TrMap[y+dy][x+dx]=0;
		//		Mops[y+dy][x+dx]=0xFFFF;
		//	};
	};
	if(capBuilding){
		if(Ref.General->cpbFarm&&Ready)Nat->NFarms--;
		OneObject* OO=Group[Index];
		Group[Index]=NULL;
		int n=Lx*Ly*2;
		for(int i=0;i<n;i++){
			short xx=(int(x)<<7)+(rando()>>8)*Lx;
			short yy=(int(y)<<7)+(rando()>>8)*Ly;
			CreateExObjD(WPLIST[3],xx,yy,
				 rando()&255,15,0,NULL);
		};
		//free(OO);
		return;
	};
	LoadAnimation(3,3,0);
	LoadCurAnm(3);
	Died[y][x]=Index;
	Sdoxlo=true;
	Ticks=0;
	TicksPerChange=1;
};
void OneObject::Eliminate(){
	if(int(InLineCom))FreeAsmLink();
	do{
		if(int(LocalOrder)){
			Order1* Loc1=LocalOrder->NextOrder;
			FreeOrdBlock(LocalOrder);
			LocalOrder=Loc1;
		};
	}while(int(LocalOrder));
	if(PathX){
		free(PathX);
		free(PathY);
		PathX=NULL;
		PathY=NULL;
		NIPoints=0;
		CurIPoint=0;
	};
	Cell8x8* CELL=&TCInf[NNUM][y>>2][x>>2];
	CELL->UnitsAmount[Kind]--;
	if(NInside){
		for(int i=0;i<NInside;i++){
			word MID=Inside[i];
			OneObject* OB=Group[MID];
			if(OB->InLineCom)OB->FreeAsmLink();
			OB->ClearOrders();
			Group[MID]=NULL;
		};
		free(Inside);
	};
	if(OnWater){
		if(capBuilding){
			for(byte dx=0;dx<Lx;dx++)
			for(byte dy=0;dy<Ly;dy++){
				WLock[y+dy][x+dx]=0;
				WMap[y+dy][x+dx]=0;
				Mops[y+dy][x+dx]=0xFFFF;
			};
		}else{
			WMap[y][x]=1;
			WLock[y][x]=1;
			Mops[y][x]=0xFFFF;
		};
	}else
		if(Media==2){
			memset(&FlyMops[0][0],255,sizeof FlyMops);
			LoadCurAnm(2);
			TicksPerChange=1;
			Sdoxlo=true;
			return;
		};
		for(byte dx=0;dx<Lx;dx++)
			for(byte dy=0;dy<Ly;dy++){
				LLock[y+dy][x+dx]=0;
				DecLock(x+dx,y+dy);
				TrMap[y+dy][x+dx]=0;
				Mops[y+dy][x+dx]=0xFFFF;
			};
	if(capBuilding){
		if(Ref.General->cpbFarm)Nat->NFarms--;
		OneObject* OO=Group[Index];
		Group[Index]=NULL;
		//int n=Lx*Ly*2;
		//for(int i=0;i<n;i++){
		//	short xx=(int(x)<<7)+(rando()>>8)*Lx;
		//	short yy=(int(y)<<7)+(rando()>>8)*Ly;
		//	CreateExObjD(WPLIST[3],xx,yy,
		//		 rando()&255,15,0,NULL);
		//};
		//free(OO);
		return;
	};
	Nat->NGidot--;
	LoadAnimation(3,3,0);
	LoadCurAnm(3);
	//Died[y][x]=Index;
	Sdoxlo=true;
	Ticks=0;
	TicksPerChange=1;
	Group[Index]=NULL;
};
void OneObject::MakeDamage(int Fundam,int Persist,OneObject* Sender){
	int shield=Ref.Visual->info.Basic.MaxShield;
	int dam=0;//Persist;
	if(Sender&&!NeedNoHelp)SearchSupport(Sender->Index);
	if(Fundam>shield)dam+=Fundam-shield;
	dam-=(dam*(tmtmt&7))>>5;
	dam+=Persist;
	if(Nat->CITY&&Sender)Nat->CITY->HelpMe(Sender->Index);
	if(!Nat->LastAttackTime){
		Nat->LastAttackTime=100;
		if(NNUM==MyNation&&dam>0)AddTEffect(x,y,Nat->DangerSound);
	};
	if(dam<=0)return;
	if(Life>dam)Life-=dam;else{
		if(Sender)InFire=Sender->Ref.General->FWEAP;
		else InFire=true;
		Life=0;
		return;
	};
	if(!int(Sender)||Sender==this||Egoist)return;
	if(!NoAnswer)AttackObj(Sender->Index,1);
};
void OneObject::SearchSupport(word OBJ){
	if(Egoist)return;
	word MID;
	char dx;
	char dy;
	byte MyMsk=NMask;
	int x0=x-6;
	int y0=y-6;
	int x1=x+6;
	int y1=y+6;
	if(x0<1)x0=1;
	if(y0<1)y0=1;
	if(x1>=msx)x1=msx-1;
	if(y1>=msy)y1=msy-1;
	for(int xx=x0;xx<=x1;xx++)
		for(int yy=y0;yy<=y1;yy++){
			MID=Mops[yy][xx];
			if(MID!=0xFFFF){
				OneObject* OO=Group[MID];
				if(OO->NMask&MyMsk){//свой!
					//занят?
					if(!OO->NoAnswer)
						OO->AttackObj(OBJ,0);
				};
			};
		};
	return;


	for(int j=0;j<8;j++){
		x1=x+idrx[j];
		y1=y+idry[j];
		MID=Mops[y1][x1];
		if(MID!=0xFFFF){
			OneObject* OO=Group[MID];
			if(OO->NMask&MyMsk){//свой!
				//занят?
				if(!OO->NoAnswer)
					OO->AttackObj(OBJ,0);
			};
		};
	};
	for(j=0;j<8;j++){
		dx=(rando()&15)-7;
		dy=(rando()&15)-7;
		x1=x+dx;
		y1=y+dy;
		if(x1>0&&y1>0&&x1<msx&&y1<msy){
			MID=Mops[y1][x1];
			if(MID!=0xFFFF){
				OneObject* OO=Group[MID];
				if(OO->NMask&MyMsk){//свой!
					//занят?
					if(!OO->NoAnswer)
						OO->AttackObj(OBJ,0);
					//else if(!(rando()&3))OO->AttackObj(OBJ,1);
				};
			};
		};
	};
};
word FindTerrEnemy(int xCell,int yCell,int mx,int my,int dist,byte Mask){
	int dst=500;
	int dista;
	word BMID=0xFFFF;
	word MID;
	int xx=xCell<<2;
	int yy=yCell<<2;
	for(int x=0;x<4;x++)
		for(int y=0;y<4;y++){
			MID=Mops[yy+y][xx+x];
			if(MID!=0xFFFF){
				OneObject* OB=Group[MID];
				if(OB&&!(OB->NMask&Mask)){
					dista=OB->DistTo(mx,my);
					if(dista<dst&&dista<dist){
						BMID=MID;
						dst=dista;
					};
				};
			};
		};
	return BMID;
};
word FindAirEnemy(int xCell,int yCell,int mx,int my,int dist,byte Mask){
	int dst=500;
	int dista;
	word BMID=0xFFFF;
	word MID;
	FlyCell* FC=&FlyMap[yCell][xCell];
	if(!FC->NFly)return 0xFFFF;
	for(int p=0;p<15;p++){
		MID=FC->FlyList[p];
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if(OB&&!(OB->NMask&Mask)){
				dista=OB->DistTo(mx,my);
				if(dista<dst&&dista<dist){
					BMID=MID;
					dst=dista;
				};
			};
		};
	};
	return BMID;
};
/*void OneObject::SearchVictim(){
	if(Egoist)return;
	byte x1;
	byte y1;
	word MID;
	char dx;
	char dy;
	byte MyMsk=NMask;
	for(int j=0;j<8;j++){
		x1=x+idrx[j];
		y1=y+idry[j];
		MID=Mops[y1][x1];
		if(MID!=0xFFFF){
			OneObject* OO=Group[MID];
			if(!(OO->NMask&MyMsk))//враг!
				AttackObj(MID,2);
		};
	};
	char LXR=(VisRadius<<1)+1;
	char VR=VisRadius;
	char NR=VisSpots;
	for(j=0;j<NR;j++){
		dx=char(RNTB[LXR][rando()&255])-VR;
		dy=char(RNTB[LXR][rando()&255])-VR;
		x1=x+dx;
		y1=y+dy;
		if(x1>0&&y1>0&&x1<msx&&y1<msy){
			MID=Mops[y1][x1];
			if(MID!=0xFFFF){
				OneObject* OO=Group[MID];
				if(!(OO->NMask&MyMsk))//враг
						AttackObj(MID,2);
			};
		};
	};
};*/
void OneObject::SearchVictim(){
	if(Egoist)return;
	//check neighbor
	int xc=x>>2;
	int yc=y>>2;
	char VR=Ref.Visual->info.Basic.AttackRange;
	if((!capBuilding)&&VR<5)VR=5;
	word MID;
	byte EnMask=~NMask;
	if(NPresence[yc][xc]&EnMask){
		MID=FindTerrEnemy(xc,yc,x,y,VR,NMask);
		if(MID==0xFFFF)
			MID=FindAirEnemy(xc,yc,x,y,VR,NMask);
		if(MID!=0xFFFF){
			AttackObj(MID,6);
			return;
		};
	};
	//check remote neighbor
	char LXR=((VR>>1)&254);
	int VR1=VR>>2;
	int x0;
	int y0;
	int x1;
	int y1;
	int dx;
	int dy;
	byte MyMsk=NMask;
	char NR=VisSpots;
	for(int j=0;j<NR;j++){
		dx=char(RNTB[LXR][rando()&255])-VR1;
		dy=char(RNTB[LXR][rando()&255])-VR1;
		x0=xc+dx;
		y0=yc+dy;
		if(x0>0&&y0>0&&x0<64&&y0<64&&NPresence[y0][x0]&EnMask){
			MID=FindTerrEnemy(x0,y0,x,y,VR,NMask);
			if(MID==0xFFFF)MID=FindAirEnemy(x0,y0,x,y,VR,NMask);
			if(MID!=0xFFFF){
				AttackObj(MID,6);
				return;
			};
		};
	};
};
void WinnerControl(){
	if(!(tmtmt&15)){
		NMyUnits=0;
		NThemUnits=0;
		for(int i=0;i<MAXOBJECT;i++){
			OneObject* OB=Group[i];
			if(OB&&!OB->Ref.General->UFO){
				if(OB->NNUM==MyNation)NMyUnits++;
			else NThemUnits++;
			};
		};
	};
};
void OneObject::DefaultSettings(GeneralObject* GO){
	Ustage=0;
	NUstages=0;
	Magic=0;
	capMagic=GO->capMagic;
	xForce=16;
	//DoubleForce=false;
	//TripleForce=false;
	//QuadraForce=false;
	MTime=0;
	NInside=0;
	Transport=GO->Transport;
	if(Transport)Inside=new word[18];
	else Inside=NULL;
	TimeInside=NULL;
	Serial=rando();
	Absent=false;
	Slow=false;
	Invisible=false;
	InFire=false;
	AbRes=0;
	MagSrcID=0xFFFF;
	if(GO->AGold)AbRes|=2;
	if(GO->AWood)AbRes|=4;
	RefreshLife=GO->RefreshLife;
	DoWalls=false;
	Use_AI=false;
	AntiNuc=GO->AntiNuc;
	if(AntiNuc){
		RAmount=1;
		delay=3000;
	}else RAmount=0;
	UFOTrans=GO->UFOTrans;
	DstX=0;
	DstY=0;
	capBuilding=GO->cpbBuilding;
	Repair=false;
	PathX=NULL;
	PathY=NULL;
	NIPoints=0;
	CurIPoint=0;
	CPdestX=0;
	CPdestY=0;
};
void PatrolLink(OneObject* OBJ);
void OneObject::Patrol(int x,int y,int x1,int y1,byte Prio){
	if(Prio<PrioryLevel)return;
	if(!Ready)return;
	if(!(cpbMoving))return;
	Order1* Or1=GetOrdBlock();
	if(!int(Or1))return;
	Or1->PrioryLevel=Prio&127;
	Or1->NextOrder=LocalOrder;
	Or1->OrderType=81;//Атака
	Or1->OrderTime=0;
	Or1->DoLink=&PatrolLink;
	Or1->info.Patrol.x=x;
	Or1->info.Patrol.y=y;
	Or1->info.Patrol.x1=x1;
	Or1->info.Patrol.y1=y1;
	Or1->info.Patrol.dir=1;
	Order1* LOR=LocalOrder;
	if(int(InLineCom))FreeAsmLink();
	LocalOrder=Or1;
	PrioryLevel=Prio&127;
	//OrderReport=NULL;
	//MessageKind=0;
	//Sender=0xFFFF;
};	
void PatrolLink(OneObject* OBJ){
	byte x=OBJ->LocalOrder->info.Patrol.x;
	byte y=OBJ->LocalOrder->info.Patrol.y;
	byte x1=OBJ->LocalOrder->info.Patrol.x1;
	byte y1=OBJ->LocalOrder->info.Patrol.y1;
	byte dir=OBJ->LocalOrder->info.Patrol.dir;
	OBJ->ClearOrders();
	if(OBJ->InLineCom)OBJ->FreeAsmLink();
	OBJ->PrioryLevel=0;
	if(dir)OBJ->SendTo(x1,y1,0);
	else OBJ->SendTo(x,y,0);
	Order1* OR1=OBJ->LocalOrder;
	if(OR1){
		Order1* OR2=GetOrdBlock();
		if(OR2){
			OR1->NextOrder=OR2;
			OR2->info.Patrol.x=x;
			OR2->info.Patrol.y=y;
			OR2->info.Patrol.x1=x1;
			OR2->info.Patrol.y1=y1;
			OR2->info.Patrol.dir=!dir;
			OR2->DoLink=&PatrolLink;
			OR2->OrderType=81;
			OR2->NextOrder=NULL;
		};
	};
};
void OneObject::EnableDoubleForce(){

};
void OneObject::DisableDoubleForce(){
};
void OneObject::EnableTripleForce(){
};
void OneObject::DiasableTripleForce(){
};
void OneObject::EnableQuadraForce(){
};
void OneObject::DisableQuadraForce(){
};
void WaitForRepairLink(OneObject* OBJ);
void OneObject::WaitForRepair(){
	if(!Ref.General->CanRepair)return;
	Order1* Or1=GetOrdBlock();
	if(!int(Or1))return;
	Or1->PrioryLevel=0;
	Or1->NextOrder=LocalOrder;
	Or1->OrderType=103;//Атака
	Or1->OrderTime=0;
	Or1->DoLink=&WaitForRepairLink;
	Or1->info.MoveToXY.x=x;
	Or1->info.MoveToXY.y=y;
	PrioryLevel=0;
	LocalOrder=Or1;
};
void WaitForRepairLink(OneObject* OBJ){
	if(!OBJ->Repair){
		Order1* LO1=OBJ->LocalOrder;
		OBJ->LocalOrder=LO1->NextOrder;
		OBJ->FreeOrdBlock(LO1);
		return;
	};
	int xx=OBJ->x;
	int yy=OBJ->y;
	int xxx,yyy;
	for(int i=0;i<4;i++){
		xxx=xx+(rando()&15)-8;
		yyy=yy+(rando()&15)-8;
		if(xxx>1&&yyy>1&&xxx<msx&&yy<msy){
			word MID=Mops[yyy][xxx];
			if(MID!=0xFFFF){
				OneObject* OB=Group[MID];
				if(OB&&OB->capBuilding&&OB->Life<OB->MaxLife){
					if(!OB->capTeleport)OBJ->BuildObj(OB->Index,16);
				return;
				};
			};
		};
	};
	xx=OBJ->LocalOrder->info.MoveToXY.x;
	yy=OBJ->LocalOrder->info.MoveToXY.y;
	if(rando()<4000&&(xx!=OBJ->x||yy!=OBJ->y))OBJ->SendTo(xx,yy,0);
};
//####################################################//
//################                 ###################//
//##############   NUCLEAR SECTION   #################//
//################                 ###################//
//####################################################//
extern bool EUsage[8192];
extern word LastAnmIndex;
extern AnmObject* GAnm[8192];
word NucList[128];
word NucSN[128];
bool NDone[128];
word NNuc;
void InitNucList(){
	memset(NucList,255,sizeof NucList);
	memset(NucSN,255,sizeof NucSN);
	NNuc=0;
};
void RegisterNuc(word ID){
	if(!EUsage[ID])return;
	for(int i=0;i<128;i++){
		if(NucList[i]==0xFFFF){
			NucList[i]=ID;
			NucSN[i]=GAnm[ID]->ASerial;
			NDone[i]=false;
			NNuc++;
			return;
		};
	};
};
void HandleAntiNuc(){
	if(!NNuc)return;
	for(int i=0;i<128;i++){
		if(NucList[i]!=0xFFFF){
			if(!EUsage[NucList[i]]){
				NucList[i]=0xFFFF;
				NNuc--;
			}else{
				AnmObject* EO=GAnm[NucList[i]];
				if(EO->ASerial!=NucSN[i]){
					NucList[i]=0xFFFF;
					NNuc--;
				}else
				if(!NDone[i]){
					byte NMS=EO->NMask;
					int dpx=EO->destX;
					int dpy=EO->destY;
					word AID=0xFFFF;
					int adist=1000;
					int diss;
					//find nearest enemy antinuclear;
					for(int j=0;j<MAXOBJECT;j++){
						OneObject* OB=Group[j];
						if(OB&&OB->RAmount&&OB->AntiNuc&&!(OB->NMask&NMS)){
							diss=abs(dpx-int(OB->x))+abs(dpy-int(OB->y));
							if(diss<adist){
								AID=j;
								adist=diss;
							};
						};
					};
					if(AID!=0xFFFF){
						OneObject* OB=Group[AID];
						OB->RAmount--;
						CreateLeadingObject(OB->Weap,((OB->x<<5)+OB->wepX)<<2,((OB->y<<5)+OB->wepY)<<2,128,OB->NMask,OB,NucList[i]);
						NDone[i]=true;
					};
				};
			};
		};
	};
};
void ShowNucl(){
	if(!NNuc)return;
	for(int i=0;i<128;i++){
		if(NucList[i]!=0xFFFF){
			AnmObject* EO=GAnm[NucList[i]];
			int xx=minix+(EO->x>>16);
			int yy=miniy+(EO->y>>16);
			Hline(xx-1,yy,xx+1,255);
			Vline(xx,yy-1,yy+1,255);
			xx=minix+(EO->destX>>1);
			yy=minix+(EO->destY>>1);
			Hline(xx-2,yy,xx-1,clrYello);
			Hline(xx+1,yy,xx+2,clrYello);
			Vline(xx,yy-2,yy-1,clrYello);
			Vline(xx,yy+1,yy+2,clrYello);
		};
	};
};
void NuclearAttackLink(OneObject* OB);
void OneObject::NuclearAttack(byte x,byte y){
	if(delay)return;
	if(!Ref.General->canNucAttack)return;
	ClearOrders();
	Order1* Or1=GetOrdBlock();
	if(!int(Or1))return;
	Or1->PrioryLevel=120;
	Or1->NextOrder=NULL;
	Or1->OrderType=200;
	Or1->OrderTime=0;
	Or1->DoLink=&NuclearAttackLink;
	Or1->info.MoveToXY.x=x;
	Or1->info.MoveToXY.y=y;
	Or1->info.MoveToXY.Times=0;//stage
	Or1->info.MoveToXY.PrevDist=0;//animation
	LocalOrder=Or1;
	LoadAnimation(1,4,0);
	LoadCurAnm(1);
	PrioryLevel=120;
};
void NuclearAttackLink(OneObject* OB){
	word anm=OB->LocalOrder->info.MoveToXY.PrevDist;
	byte stag=OB->LocalOrder->info.MoveToXY.Times;
	int x=OB->LocalOrder->info.MoveToXY.x;
	int y=OB->LocalOrder->info.MoveToXY.y;
	OB->LocalOrder->info.MoveToXY.PrevDist++;
	if(!stag){
		//Opening the can
		int csp=OB->CurrentSprite;
		if(csp<OB->CurAnm->count-1){
			OB->CurrentSprite=anm>>4;	
		}else OB->LocalOrder->info.MoveToXY.Times=1;
	}else{
		CreateStrangeObject(11,OB->NNUM,OB->x+1,OB->y+1,0xFFFF);
		OB->Ref.Visual->info.Basic.AttackRange=250;
		CreateUniExObj(OB->Weap,((OB->x<<5)+OB->wepX)<<2,((OB->y<<5)+OB->wepY)<<2,OB->Ref.General->WepSpeed,OB->NMask,OB,x,y,OB->Index);
		if(LastAnmIndex!=-1)RegisterNuc(LastAnmIndex);
		OB->Ref.Visual->info.Basic.AttackRange=0;
		OB->ClearOrders();
		OB->LoadAnimation(0,0,0);
		OB->LoadCurAnm(0);
		OB->delay=5000;
	};
};
//**************************************************//
//                                                  //
//            INITIALISATION OF THE GAME            //
//                                                  //
//**************************************************//
void InitGame(){
	InitNucList();
};