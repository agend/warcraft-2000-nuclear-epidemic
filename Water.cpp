//Floating modules discription & management
//Need to rewrite:
//CreatePath      --->    CreateWPath
//
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
#include "Megapolis.h"
#include <assert.h>
#include "GSound.h"
extern byte Locking[1024];//Инф. о блокировании поверхности
//extern MaxReplyDistance;
extern int PathAsks;
extern OneObject OBJECTS[8192];
const int drrw[9]={7,6,5,0,0,4,1,2,3};
const byte drrbw[9]={7,6,5,0,0,4,1,2,3};
const int idrxw[8]={0,1,1,1,0,-1,-1,-1};
const int idryw[8]={-1,-1,0,1,1,1,0,-1};
byte WLock[256][256];//Water locking map
byte WMap[256][256];//Static locking map
void CreateWaterMap(){
	for(int i=0;i<msx;i++)for(int j=0;j<msy;j++){
		byte k=Locking[tmap[j][i]];
		if(k&2){
			WLock[i+1][j+1]=1;
			WMap[i+1][j+1]=1;
		};
		//if(k&2)map[j][i].WaterLock=true;
	};
	for(i=0;i<=msx+1;i++){
		WMap[i][0]=2;
		WMap[i][msy+2]=2;
		WLock[i][0]=2;
		WLock[i][msy+2]=2;
		WMap[i][0]=2;
		WMap[i][msy+1]=2;
		WLock[i][0]=2;
		WLock[i][msy+1]=2;

	};
	for(i=0;i<msy+1;i++){
		WMap[0][i]=2;
		WMap[msx+2][i]=2;
		WLock[0][i]=2;
		WLock[msx+2][i]=2;
		WMap[0][i]=2;
		WMap[msx+1][i]=2;
		WLock[0][i]=2;
		WLock[msx+1][i]=2;
	};
};

struct AskMove{
	word ReqID;//индекс перемещаемого
	word PreID;//индекс жаждущего дорваться в эту клетку
	byte x;
	byte y;
	char dx;
	char dy;
};
extern int NAsk;//Количество запросов
extern AskMove Ask[8192];//Массив запросов
extern bool FailLink[8192];//Массив неразрешенных перемещений
extern word CurInd;
extern word IDMap[256][256];
extern word RQMap[256][256];//:3-запещенное направление ..
					 //:13-номер в таблице запросов
//Добавить запрос в систему запросов на перемещение
void AddWAsk(word ReqID,byte x,byte y,char zdx,char zdy){
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
		sal			bl,1//moving on 2 cells
		mov		bh,zdy
		sal			bh,1
		mov		word ptr [eax+6],bx
		inc		NAsk
	};
};
//Обработка запросов, сначала освобождаем все старые клетки
//потом заполняем и проверяем
void HandleWAsks(){
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
		add		al,2
		mov		ah,al
		shr		al,1
		add		al,ah
		mov		ah,[esi+7]//dy
		add		ah,2
		shr		ah,1
		add		al,ah
		xor		ah,ah
		mov		bl,byte ptr [drrbw+eax]//direction
		mov		cx,[esi+4]//x,y=>offset
		mov		ax,CurInd
		mov		[IDMap+ecx*2],ax
		add		bl,4
		and		bl,7
		mov		dx,word ptr ZZ
		shl		dx,3
		add		dl,bl
		mov		[RQMap+ecx*2],dx
		mov		byte ptr [WLock+ecx],0//разблокировали старое положение
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
		cmp		byte ptr [WLock+ecx],0
		jnz		C2isLocked
		mov		ax,word ptr CurInd
		cmp		word ptr [IDMap+ecx*2],ax
		jne		C2notLocked
		//direction
		mov		dx,[RQMap+ecx*2]
		and		dl,7
		mov		al,[esi+6]
		add		al,2
		mov		ah,al
		shr		al,1
		add		al,ah
		mov		ah,[esi+7]
		add		ah,2
		shr		ah,1
		add		al,ah
		xor		ah,ah
		mov		al,[drrbw+eax]//=direction
		cmp		al,dl
		je		C2isLocked //переход в этом направлении запрещен
		//ставим ссылку на данного монстра в поле PreID 
		//у монстра, который ранее занимал эту клетку
		mov		dx,[RQMap+ecx*2]
		and		edx,0FFF8h
		mov		ax,word ptr ZZ
		mov		word ptr [Ask+2+edx],ax
C2notLocked:
		mov		[WLock+ecx],1//блокируем клетку
		inc		ZZ
		add		esi,8
		dec		NN
		jnz		C2Begin
		jmp		CycEnd
		//Клетка заблокирована, определяем монстров,перемещение 
		//которых запрещено отныне
C2isLocked:
		mov		ax,[esi+4]
		mov		byte ptr [WLock+eax],1
		mov		ax,word ptr ZZ
		xor		edx,edx
FailLoop:
		mov		byte ptr [FailLink+eax],0
		mov		bx,word ptr [Ask+2+eax*8]
		mov		word ptr [Ask+2+eax*8],0FFFFh
		mov		dx,word ptr [Ask+4+eax*8]
		mov		[WLock+edx],1//блокируем старую позицию
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
inline void WSpot(int x,int y){
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
void OneObject::MakeWPreProcess(){
	if(Media!=1)return;
	if(NNUM==MyNation){
		int x0,y0,dx,dx1;
		switch(SpotType){
		case 0:
			if(Lx>1){
			for(byte ux=0;ux<Lx;ux++)
				for(byte uy=0;uy<Ly;uy++)
					WSpot(x+ux,y+uy);
			}else WSpot(x,y);
			break;
		case 1:
			x0=x+(Lx>>1);
			y0=y+(Ly>>1);
			dx=SpotSize;
			WSpot(x0,y0);
			WSpot(x0+dx,y0);
			WSpot(x0-dx,y0);
			WSpot(x0,y0+dx);
			WSpot(x0,y0-dx);
			break;
		case 2:
			x0=x+(Lx>>1);
			y0=y+(Ly>>1);
			dx=SpotSize;
			dx1=dx-(dx>>2);
			WSpot(x0,y0);
			WSpot(x0+dx,y0);
			WSpot(x0-dx,y0);
			WSpot(x0,y0+dx);
			WSpot(x0,y0-dx);
			WSpot(x0+dx1,y0+dx1);
			WSpot(x0-dx1,y0-dx1);
			WSpot(x0+dx1,y0-dx1);
			WSpot(x0-dx1,y0+dx);
			break;
		};
	};
	//WMap[y][x]=1;
	if(delay)delay--;
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
				 WLock[y][x]=0;
				 WMap[y][x]=0;
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
		//if(!Important)
		if(!NoSearchVictim)SearchVictim();
		Order1* LO=LocalOrder;
		if(!LO)PrioryLevel=0;
		if(int(InLineCom)){
			//MessageFlags=1;
			bool	Done;
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
					AddWAsk(Index,x,y,p[1],p[2]);
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
			if(LO&&LO->OrderType>127){
				//FreeAsmLink();
				LocalOrder->DoLink(this);
			};
		} else{ 
			if(TicksPerChange!=255)LoadCurAnm(0);
			Important=false;
			if(int(LocalOrder)){
				LocalOrder->DoLink(this);
//				if(LocalOrder->OrderType==77)
//					MoveFrom(LocalOrder->info.MoveFrom.dir);
			} else	if(TicksPerChange!=255)CurrentSprite=0;
		};
};
void MakeWPostProcess(){
	HandleWAsks();
	for(int i=0;i<NAsk;i++){
		AskMove ASK=Ask[i];
		OneObject* OBJ=Group[ASK.ReqID&8191];
		if(FailLink[i]){
			byte ax=ASK.x;
			byte ay=ASK.y;
			char oox=ax>>2;
			char ooy=ay>>2;
			byte adx=ASK.dx+2;
			byte ady=ASK.dy+2;
			byte Kind=OBJ->Kind;
			OBJ->Direction=drrbw[(adx>>1)+adx+(ady>>1)];
			WMap[ay][ax]=0;
			OBJ->x=ax+ASK.dx;
			OBJ->y=ay+ASK.dy;
			char onx=OBJ->x>>2;
			char ony=OBJ->y>>2;
			Mops[OBJ->y][OBJ->x]=OBJ->Index;
			if(Mops[ay][ax]==OBJ->Index)Mops[ay][ax]=0xFFFF;
			OBJ->Addx=-ASK.dx<<5;
			OBJ->Addy=-ASK.dy<<5;
			OBJ->LoadCurAnm(1);
			OBJ->isMoving=true;
			OBJ->StandTime=0;
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
					if(OBJ->StandTime>1)
						OBJ2->WMoveFrom(OBJ->Direction);
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
byte wcirc[16];
inline void FillWCirc(int x,int y){
	__asm{
		xor		ebx,ebx
		mov		bh,byte ptr y
		mov		bl,byte ptr x
		add		ebx,offset WMap
		mov		al,[ebx-512]
		mov		wcirc[0],al
		mov		al,[ebx-510]
		mov		wcirc[1],al
		mov		al,[ebx+2]
		mov		wcirc[2],al
		mov		al,[ebx+514]
		mov		wcirc[3],al
		mov		al,[ebx+512]
		mov		wcirc[4],al
		mov		al,[ebx+510]
		mov		wcirc[5],al
		mov		al,[ebx-2]
		mov		wcirc[6],al
		mov		al,[ebx-514]
		mov		wcirc[7],al
		mov		eax,dword ptr wcirc[0]
		mov		dword ptr wcirc[8],eax
		mov		eax,dword ptr wcirc[4]
		mov		dword ptr wcirc[12],eax
	}
};
//Inline- команды построения внутренних команд
extern int Ofst;
extern char* NowBuf;
inline void ChkWOfst(int size){
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
inline void cmWSetXY(byte x,byte y){
	ChkWOfst(3);
	NowBuf[Ofst]=4;
	NowBuf[Ofst+1]=x;
	NowBuf[Ofst+2]=y;
	Ofst+=3;
};
inline void cmWSetXYDir(byte x,byte y,byte dir,byte n){
	ChkWOfst(4);
	NowBuf[Ofst]=18;
	NowBuf[Ofst+1]=x;
	NowBuf[Ofst+2]=y;
	NowBuf[Ofst+3]=(dir&7)|(n<<4);
	Ofst+=4;
};
inline void cmWSetXYDirX(byte x,byte y,char dx,char dy,byte n){
	ChkWOfst(4);
	byte dr=drrw[(dx+1)*3+dy+1];
	NowBuf[Ofst]=18;
	NowBuf[Ofst+1]=x;
	NowBuf[Ofst+2]=y;
	NowBuf[Ofst+3]=(dr&7)|(n<<4);
	Ofst+=4;
};

inline void cmWChkXY(byte x,byte y){
	ChkWOfst(3);
	NowBuf[Ofst]=44;
	NowBuf[Ofst+1]=x;
	NowBuf[Ofst+2]=y;
	Ofst+=3;
};
inline void cmWSetDir(int dx,int dy){
	if(dx==0&&dy==0)return;
	byte dr=drrw[(dx+1)*3+dy+1];
	ChkWOfst(2);
	NowBuf[Ofst]=5;
	NowBuf[Ofst+1]=dr;
	Ofst+=2;
};
void cmWSetDirD(byte dr){
	ChkWOfst(2);
	NowBuf[Ofst]=5;
	NowBuf[Ofst+1]=dr&7;
	Ofst+=2;
};
inline void cmWLoadAnm(byte stype,byte dtype,word kind){
	ChkWOfst(5);
	NowBuf[Ofst]=6;
	NowBuf[Ofst+1]=dtype;
	NowBuf[Ofst+2]=stype;
	NowBuf[Ofst+3]=byte(kind);
	NowBuf[Ofst+4]=0;
	Ofst+=5;
};
inline void cmWPerfAnm(byte n){
	ChkWOfst(2);
	NowBuf[Ofst]=8;
	NowBuf[Ofst+1]=n;
	Ofst+=2;
};
inline void cmWRet(){
	NowBuf[Ofst]=1;
	Ofst+=1;
};
inline void cmWDone(){
	NowBuf[Ofst]=3;
	Ofst+=1;
};

void WSendToLink(OneObject* OBJ);
void OneObject::WSendTo(int x2,int y2,int Prio){
#define MaxP 160
	if(PrioryLevel>Prio)return;
	if(!cpbMoving)return;
	int x1=x2&254;
	int y1=y2&254;
	if(WMap[y1][x1]){
		//Находим ближайшую незанятую точку
		bool fnf=true;
		for(int i=1;i<30;i++){
			int z=i<<1;
			for(int j=0;j<z;j++)
				if(!WMap[y1-i-i][x1-i-i+j+j]){
					x1-=i+i-j-j;
					y1-=i+i;
					goto Em_Found;
				};
			for(j=0;j<z;j++)
				if(!WMap[y1-i-i+j+j][x1+i+i]){
					y1-=i+i-j-j;
					x1+=i+i;
					goto Em_Found;
				};
			for(j=0;j<z;j++)
				if(!WMap[y1+i+i][x1+i+i-j-j]){
					y1+=i+i;
					x1+=i+i-j-j;
					goto Em_Found;
				};
			for(j=0;j<z;j++)
				if(!WMap[y1+i+i-j][x1-i-i]){
					y1+=i+i-j-j;
					x1-=i+i;
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
	Or1->DoLink=&WSendToLink;
	Or1->info.MoveToXY.x=x1;
	Or1->info.MoveToXY.y=y1;
	Order1* LOR=LocalOrder;
	if(int(LOR)){
		//if(LOR->OrderType=2&&StandTime>5)WMap[y][x]=1;
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
void WSendToLink(OneObject* OBJ){
	byte x1=OBJ->LocalOrder->info.MoveToXY.x;
	byte y1=OBJ->LocalOrder->info.MoveToXY.y;
	byte Prio=OBJ->LocalOrder->PrioryLevel;
	if(WMap[y1][x1])OBJ->SendTo(x1,y1,Prio);else{
		byte x=OBJ->x;
		byte y=OBJ->y;
		if(x==x1&&y==y1){
			if(int(OBJ->LocalOrder)){
				if(OBJ->MemoryTime)OBJ->MemoryTime--;
				if(!OBJ->MemoryTime){
					if(OBJ->LocalOrder->OrderType==2){
						Order1* Loc1=OBJ->LocalOrder->NextOrder;
						OBJ->FreeOrdBlock(OBJ->LocalOrder);
						OBJ->LocalOrder=Loc1;
					};
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
			OBJ->CreateWPath(x1,y1);
			OBJ->MemoryTime=0;
		};
	};
};
//Атаковать объект с заданным индексом
void WAttackObjLink(OneObject* OBJ);
void OneObject::WAttackObj(word OID,int Prio){
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
	if((Attack&&dst>EnemyDist&&Prio<8))return;
	//if(Prio<16&&dst>MaxReplyDistance)return;
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
	Or1->DoLink=&WAttackObjLink;
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
/*



	if(Prio<PrioryLevel&&!Attack)return;
	if(Prio>=16)StandGround=false;
	if(!Ready)return;
	if(!(cpbMoving||int(Weap)))return;
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
	if(dst>EnemyDist)return;
	if(Prio<16&&dst>30)return;
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
	if(!int(Or1))return;
	Or1->PrioryLevel=Prio&127;
	Or1->NextOrder=LocalOrder;
	Or1->OrderType=3;//Атака
	Or1->OrderTime=0;
	Or1->DoLink=&WAttackObjLink;
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
	PrioryLevel=Prio&127;*/
};	
void WAttackObjLink(OneObject* OBJ){
	word OID=OBJ->EnemyID;
	Order1* Or1;
	if(OID>8192){
		if(int(OBJ->InLineCom))OBJ->FreeAsmLink();
		if(int(OBJ->LocalOrder)){
			Or1=OBJ->LocalOrder->NextOrder;
			OBJ->FreeOrdBlock(OBJ->LocalOrder);
			OBJ->LocalOrder=Or1;
			OBJ->Important=false;
			return;
		};
	};
	OneObject* OB=Group[OID];
	OBJ->Important=true;
	if(!OBJ->NeedNoHelp)OBJ->SearchSupport(OID);
	if((!int(OB))||OB->Sdoxlo||OB->Serial!=OBJ->EnemySN){
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
	int	sy=yo-OBJ->y;
	int dx=abs(sx);
	int dy=abs(sy);
	int dst;
	if(dx>dy)dst=dx;
			else dst=dy;
	OBJ->EnemyDist=dst;
	if(dst<2&&OBJ->cpbMoving&&!OBJ->delay){
		OBJ->Direction=drrw[(sx+1)*3+sy+1];
		OBJ->LoadAnimation(2,2,0);
		OBJ->LoadCurAnm(2);
		int Midm=OBJ->Ref.Visual->info.Basic.MinDamage;
		int Madm=OBJ->Ref.Visual->info.Basic.MaxDamage;
		int Dam=(rando()*(Madm-Midm)>>15)+Midm;
		//Dam=(Dam*OBJ->xForce)>>4;
		//if(OB->Life>Dam)OB->Life-=Dam;else OB->Life=0;
		OB->MakeDamage(Madm,Midm,OBJ);
		OBJ->isMoving=false;
		OBJ->delay=OBJ->MaxDelay;
	}else{ 
		Visuals* VS=OBJ->Ref.Visual;
		if(dst<VS->info.Basic.AttackRange&&dst>VS->DangerZone){
			if(OBJ->Weap&&!OBJ->delay){
				OBJ->isMoving=false;
				OBJ->delay=OBJ->Ref.General->WepDelay;
				//OBJ->delay=OBJ->MaxDelay;
				if(OBJ->cpbMoving){
					OBJ->LoadAnimation(2,2,0);
					OBJ->LoadCurAnm(2);
					OBJ->Direction=
						CreateUniExObj(OBJ->Weap,((OBJ->x<<5)+OBJ->wepX)<<2,((OBJ->y<<5)+OBJ->wepY)<<2,OBJ->Ref.General->WepSpeed,OBJ->NMask,OBJ,OB->x,OB->y,OB->Index);
					//OBJ->Direction=
					//	AttackByWeapon(OBJ->Weap,((OBJ->x<<5)+OBJ->wepX)<<2,((OBJ->y<<5)+OBJ->wepY)<<2,OBJ,OB->Index);
						//CreateExObj(OBJ->Weap,((OBJ->x<<5)+OBJ->wepX)<<2,((OBJ->y<<5)+OBJ->wepY)<<2,sx,sy,64,OBJ->NMask,OBJ);
				}else
					CreateUniExObj(OBJ->Weap,((OBJ->x<<5)+OBJ->wepX)<<2,((OBJ->y<<5)+OBJ->wepY)<<2,OBJ->Ref.General->WepSpeed,OBJ->NMask,OBJ,OB->x,OB->y,OB->Index);
					//AttackByWeapon(OBJ->Weap,((OBJ->x<<5)+OBJ->wepX)<<2,((OBJ->y<<5)+OBJ->wepY)<<2,OBJ,OB->Index);
					//CreateExObj(OBJ->Weap,((OBJ->x<<5)+OBJ->wepX)<<2,((OBJ->y<<5)+OBJ->wepY)<<2,sx,sy,64,OBJ->NMask,OBJ);
			};
		}else
		if(OBJ->cpbMoving&&!OBJ->StandGround)OBJ->CreateWPath(xo,yo);
	};
};
extern byte xx[MaxP];
extern byte yy[MaxP];
void OneObject::CreateSimpleWPath(int txt1,int tyt1){
	PathAsks++;
	int x1=txt1&254;
	int	y1=tyt1&254;
	WMap[y][x]=0;
	int sdx=(x1-x)>>1;
	int	sdy=(y1-y)>>1;
	int	Cum=0;
	int Pps=0;
	int sx=sdx;
	int sy=sdy;
	if(sx>1)sx=1;
	if(sx<-1)sx=-1;
	if(sy>1)sy=1;
	if(sy<-1)sy=-1;
	sx=sx<<1;
	sy=sy<<1;
	int dx=abs(sdx);
	int dy=abs(sdy);
	int	Mdx=dx>>1;
	int	Mdy=dy>>1;
	int	Mx=x;
	int	My=y;
	int	xx1=x;
	int yy1=y;
	//int rx=sx;
	//int ry=sy;
	//if(dx>dy)ry=0;
	//if(dy>dx)rx=0;
	if(!WMap[y+sy][x+sx]){
		LoadAnimation(1,AnmGoKind,0);
		AddAsk(Index,x,y,sx,sy);
		return;
	};
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
		add		si,2	//x++
		add		cx,bx
		cmp		cx,word ptr Mdx
		jb		Lp1_1
		sub		cx,word ptr Mdx
		add		di,2  //y++
Lp1_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		ax
		jnz		Lp1begin
		jmp		LoopsEnd
Lp2begin: //dx>dy,sx<0,sy>0
		sub		si,2	//x--
		add		cx,bx
		cmp		cx,word ptr Mdx
		jb		Lp2_1
		sub		cx,word ptr Mdx
		add		di,2 //y++
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
		add		si,2	//x++
		add		cx,bx
		cmp		cx,word ptr Mdx
		jb		Lp3_1
		sub		cx,word ptr Mdx
		sub		di,2 //y--
Lp3_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		ax
		jnz		Lp3begin
		jmp		LoopsEnd
Lp4begin: //dx>dy,sx<0,sy<0
		sub		si,2	//x--
		add		cx,bx
		cmp		cx,word ptr Mdx
		jb		Lp4_1
		sub		cx,word ptr Mdx
		sub		di,2 //y--
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
		add		di,2	//y++
		add		cx,ax
		cmp		cx,word ptr dy
		jb		Lp5_1
		sub		cx,word ptr dy
		add		si,2	//x++
Lp5_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		bx
		jnz		Lp5begin
		jmp		LoopsEnd
Lp6Begin://sx>0,sy<0
		sub		di,2	//y--
		add		cx,ax
		cmp		cx,word ptr dy
		jb		Lp6_1
		sub		cx,word ptr dy
		add		si,2	//x++
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
		add		di,2	//y++
		add		cx,ax
		cmp		cx,word ptr dy
		jb		Lp7_1
		sub		cx,word ptr dy
		sub		si,2	//x--
Lp7_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		bx
		jnz		Lp7begin
		jmp		LoopsEnd
Lp8Begin://dx<0,dy<0
		sub		di,2	//y--
		add		cx,ax
		cmp		cx,word ptr dy
		jb		Lp8_1
		sub		cx,word ptr dy
		sub		si,2	//x--
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
	byte Trr=WMap[y][x];
	WMap[y][x]=0;
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
		cmp		byte ptr[WMap+ebx],0
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
		//WLock[y][x]=false;
		Rpp-=1;
		Rtx=xx[Rpp];
		Rty=yy[Rpp];
		Ltx=xx[Rpp];
		Lty=yy[Rpp];
		int Ppi=Rpp+1;
		LDoing=true;
		RDoing=true;
		//Ищем, пока находимся в занятой зоне
		while(WMap[yy[Ppi]][xx[Ppi]]&&Ppi<Pps)Ppi++;
		if(Ppi>Pps)LDoing=false;//Кон.точка недостижима
		int Xls=xx[Ppi-1];
		int Yls=yy[Ppi-1];
		//Уперлись...Вычисляем направление движения
		char dtx=xx[Rpp+1]-xx[Rpp]+2;
		char dty=(yy[Rpp+1]-yy[Rpp]+2)>>1;
		Rdirc=drrw[dtx+dty+(dtx>>1)];
		Ldirc=Rdirc;
		//Выбираем начальное направление-right
		FillWCirc(Rtx,Rty);
		int dirc1=(Rdirc+1)&7;
		for(int z=0;wcirc[dirc1]&&z<7;dirc1++,z++);
		Rdirc=dirc1&7;
		//-left
		dirc1=8+((Ldirc+7)&7);
		for(z=0;wcirc[dirc1]&&z<7;dirc1--,z++);
		Ldirc=dirc1&7;
		//Идем по правому краю до тех пор пока вновь не пересе-
		//чемся с прямой линией, соединяющей начальную и конечную
		//точки
		Rmaxalt=0;
		Lmaxalt=0;
		while(Rpp<MaxP-8&&(LDoing&&RDoing)){
			//пытаемся повернуть направо
			FillWCirc(Rtx,Rty);
			int dirc1=(Rdirc+7)&7;
			for(int z=0;z<6&&wcirc[dirc1];dirc1++,z++);
			Rdirc=dirc1&7;
			Rpp++;
			int Tdx=idrxw[Rdirc];
			int Tdy=idryw[Rdirc];
			Rcum-=sdy*Tdx;
			Rcum+=sdx*Tdy;
			Rtx+=Tdx+Tdx;
			Rty+=Tdy+Tdy;
			Rpx[Rpp]=Rtx;Rpy[Rpp]=Rty;
			//the same, but left
			FillWCirc(Ltx,Lty);
			dirc1=8+((Ldirc+1)&7);
			for(z=0;z<6&&wcirc[dirc1];dirc1--,z++);
			Ldirc=dirc1&7;
			Tdx=idrxw[Ldirc];
			Tdy=idryw[Ldirc];
			Lcum+=sdy*Tdx;
			Lcum-=sdx*Tdy;
			Ltx+=Tdx+Tdx;
			Lty+=Tdy+Tdy;
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
	//WMap[y][x]=Trr;
	NowBuf=GetAsmBlock();
	Ofst=0;
	memcpy(&NowBuf[OneAsmSize-4],&Ofst,4);
	InLineCom=NowBuf;
	LineOffset=0;
	cmWLoadAnm(AnmGoKind,1,0);
	//cmLoadAnm(1,1,0);
	cmWLoadAnm(AnmStandKind,0,0);
	for(int i=1;i<=Pps;i++){
		//byte (*XXX)[64];
		//memcpy(&XXX,&NowBuf,4);
		//if(i==1)cmPerfAnm(0);
		cmWSetXY((xx[i]-xx[i-1])>>1,(yy[i]-yy[i-1])>>1);
	};
	cmWLoadAnm(AnmStandKind,0,0);
	cmWPerfAnm(0);
	//cmDone();
	cmWRet();
};
int GetRAngle(int dx,int dy,int Angle);
int GetLAngle(int dx,int dy,int Angle);
void OneObject::CreatePreWPath(int txt1,int tyt1){
	PathAsks++;
	int x1=txt1&254;
	int	y1=tyt1&254;
	WMap[y][x]=0;
	int sdx=(x1-x)>>1;
	int	sdy=(y1-y)>>1;
	int	Cum=0;
	int Pps=0;
	int sx=sdx;
	int sy=sdy;
	if(sx>1)sx=1;
	if(sx<-1)sx=-1;
	if(sy>1)sy=1;
	if(sy<-1)sy=-1;
	sx=sx<<1;
	sy=sy<<1;
	int dx=abs(sdx);
	int dy=abs(sdy);
	int	Mdx=dx;//>>1;
	int	Mdy=dy;//>>1;
	int	Mx=x;
	int	My=y;
	int	xx1=x;
	int yy1=y;
	int rx=sx;
	int ry=sy;
	//if(dx>dy)ry=0;
	//if(dy>dx)rx=0;
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
		add		si,2	//x++
		add		cx,bx
		cmp		cx,word ptr Mdx
		jb		Lp1_1
		sub		cx,word ptr Mdx
		add		di,2  //y++
Lp1_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		ax
		jnz		Lp1begin
		jmp		LoopsEnd
Lp2begin: //dx>dy,sx<0,sy>0
		sub		si,2	//x--
		add		cx,bx
		cmp		cx,word ptr Mdx
		jb		Lp2_1
		sub		cx,word ptr Mdx
		add		di,2 //y++
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
		add		si,2	//x++
		add		cx,bx
		cmp		cx,word ptr Mdx
		jb		Lp3_1
		sub		cx,word ptr Mdx
		sub		di,2 //y--
Lp3_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		ax
		jnz		Lp3begin
		jmp		LoopsEnd
Lp4begin: //dx>dy,sx<0,sy<0
		sub		si,2	//x--
		add		cx,bx
		cmp		cx,word ptr Mdx
		jb		Lp4_1
		sub		cx,word ptr Mdx
		sub		di,2 //y--
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
		add		di,2	//y++
		add		cx,ax
		cmp		cx,word ptr dy
		jb		Lp5_1
		sub		cx,word ptr dy
		add		si,2	//x++
Lp5_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		bx
		jnz		Lp5begin
		jmp		LoopsEnd
Lp6Begin://sx>0,sy<0
		sub		di,2	//y--
		add		cx,ax
		cmp		cx,word ptr dy
		jb		Lp6_1
		sub		cx,word ptr dy
		add		si,2	//x++
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
		add		di,2	//y++
		add		cx,ax
		cmp		cx,word ptr dy
		jb		Lp7_1
		sub		cx,word ptr dy
		sub		si,2	//x--
Lp7_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		bx
		jnz		Lp7begin
		jmp		LoopsEnd
Lp8Begin://dx<0,dy<0
		sub		di,2	//y--
		add		cx,ax
		cmp		cx,word ptr dy
		jb		Lp8_1
		sub		cx,word ptr dy
		sub		si,2	//x--
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
	byte Trr=WMap[y][x];
	WMap[y][x]=0;
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
		cmp		byte ptr[WMap+ebx],0
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
		//WLock[y][x]=false;
		Rpp-=1;
		Rtx=xx[Rpp];
		Rty=yy[Rpp];
		Ltx=xx[Rpp];
		Lty=yy[Rpp];
		int Ppi=Rpp+1;
		LDoing=true;
		RDoing=true;
		//Ищем, пока находимся в занятой зоне
		while(WMap[yy[Ppi]][xx[Ppi]]&&Ppi<Pps)Ppi++;
		if(Ppi>Pps)LDoing=false;//Кон.точка недостижима
		int Xls=xx[Ppi-1];
		int Yls=yy[Ppi-1];
		//Уперлись...Вычисляем направление движения
		char dtx=xx[Rpp+1]-xx[Rpp]+2;
		char dty=(yy[Rpp+1]-yy[Rpp]+2)>>1;
		Rdirc=drrw[dtx+dty+(dtx>>1)];
		Ldirc=Rdirc;
		//Выбираем начальное направление-right
		FillWCirc(Rtx,Rty);
		int dirc1=(Rdirc+1)&7;
		for(int z=0;wcirc[dirc1]&&z<7;dirc1++,z++);
		Rdirc=dirc1&7;
		//-left
		dirc1=8+((Ldirc+7)&7);
		for(z=0;wcirc[dirc1]&&z<7;dirc1--,z++);
		Ldirc=dirc1&7;
		//Идем по правому краю до тех пор пока вновь не пересе-
		//чемся с прямой линией, соединяющей начальную и конечную
		//точки
		Rmaxalt=0;
		Lmaxalt=0;
		while(Rpp<MaxP-8&&(LDoing&&RDoing)){
			//пытаемся повернуть направо
			FillWCirc(Rtx,Rty);
			int dirc1=(Rdirc+7)&7;
			for(int z=0;z<6&&wcirc[dirc1];dirc1++,z++);
			Rdirc=dirc1&7;
			Rpp++;
			int Tdx=idrxw[Rdirc];
			int Tdy=idryw[Rdirc];
			Rcum-=sdy*Tdx;
			Rcum+=sdx*Tdy;
			Rtx+=Tdx+Tdx;
			Rty+=Tdy+Tdy;
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
			ddx=ddx<<1;
			ddy=ddy<<1;
			if(!WMap[Rty+ddy][Rtx+ddx]){
				if(!RvpLast){
					Rvp=Rpp;
					RvpLast=true;
				};
			}else RvpLast=false;
			//the same, but left
			FillWCirc(Ltx,Lty);
			dirc1=8+((Ldirc+1)&7);
			for(z=0;z<6&&wcirc[dirc1];dirc1--,z++);
			Ldirc=dirc1&7;
			Tdx=idrxw[Ldirc];
			Tdy=idryw[Ldirc];
			Lcum+=sdy*Tdx;
			Lcum-=sdx*Tdy;
			Ltx+=Tdx+Tdx;
			Lty+=Tdy+Tdy;
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
			ddx=ddx<<1;
			ddy=ddy<<1;
			if(!WMap[Lty+ddy][Ltx+ddx]){
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
				NIPoints=2+((NIPoints-2)>>1);
			};
			PathX=new byte [NIPoints];
			PathY=new byte [NIPoints];
			if(RightPrefer){
				if(maxp<=2){
					memcpy(PathX,&Rpx[Pps],NIPoints);
					memcpy(PathY,&Rpy[Pps],NIPoints);
				}else{
					int np=((maxp-2)>>1)+1;
					for(int nn=0;nn<np;nn++){
						PathX[nn]=Rpx[Pps+(nn<<1)];
						PathY[nn]=Rpy[Pps+(nn<<1)];
					};
					PathX[nn]=Rpx[Pps+maxp-1];
					PathY[nn]=Rpy[Pps+maxp-1];
				};
			}else{
				if(maxp<=2){
					memcpy(PathX,&Lpx[Pps],NIPoints);
					memcpy(PathY,&Lpy[Pps],NIPoints);
				}else{
					int np=((maxp-2)>>1)+1;
					for(int nn=0;nn<np;nn++){
						PathX[nn]=Lpx[Pps+(nn<<1)];
						PathY[nn]=Lpy[Pps+(nn<<1)];
					};
					PathX[nn]=Lpx[Pps+maxp-1];
					PathY[nn]=Lpy[Pps+maxp-1];
				};
			};
			NeedPath=true;
		}else return;
	};
};
void OneObject::CreateWPath(int x1,int y1){
	if(CPdestX==x1&&CPdestY==y1){
		if(PathX){
			if(CurIPoint<NIPoints){
				byte cx=PathX[CurIPoint];
				byte cy=PathY[CurIPoint];
				int dis=DistTo(cx,cy);
				if(dis<2)CurIPoint++;
				//else if(dis<4&&ChkStright(x,y,cx,cy))CurIPoint++;
				if(CurIPoint<NIPoints){
					CreateSimpleWPath(PathX[CurIPoint],PathY[CurIPoint]);
				}else{
					int xx1=x;
					int yy1=y;
					x=cx;
					y=cy;
					CreatePreWPath(x1,y1);
					x=xx1;
					y=yy1;
				};
			};
		}else CreateSimpleWPath(x1,y1);
	}else{
		CreatePreWPath(x1,y1);
	};
	CPdestX=x1;
	CPdestY=y1;
};
extern word LastObject;
bool Nation::CreateOnWater(int xp,int yp,int n){
	int x=xp&254;
	int y=yp&254;
	if( Mops[y][x]!=0xFFFF||WLock[y][x])return false;
	for(int i=0;i<MaxObj&&int(Group[i]);i++);
	if(i>=MaxObj)return false;
	if(n>=NMon) return false;
	if(i>=MAXOBJECT)MAXOBJECT=i+1;
	Group[i]=OBJECTS+i;
	LastObject=i;
	OneObject* G=Group[i];
	GeneralObject* GO=Mon[n];
	AddOrderEffect(x,y,GO->BornSound);
	Cell8x8* CELL=&TCInf[NNUM][y>>2][x>>2];
	CELL->UnitsAmount[GO->Kind]++;
	G->DefaultSettings(GO);
	G->Kind=GO->Kind;
	G->Media=1;
	G->Teleport=false;
	G->capTeleport=GO->Teleport;
	G->OnWater=true;
	G->CrowdRef=NULL;
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
	G->Push=false;
	WLock[y][x]=true;
	//WMap[y][x]=1;
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
	G->NInside=0;
	if(GO->Transport){
		G->Transport=true;

	};
	NGidot++;
	return true;
};
inline void WReMemCirc(byte x,byte y){
	__asm{
		xor		ebx,ebx
		mov		bh,byte ptr y
		mov		bl,byte ptr x
		add		ebx,offset WMap
		mov		al,wcirc[0]
		mov		[ebx-512],al
		mov		al,wcirc[1]
		mov		[ebx-510],al
		mov		al,wcirc[2]
		mov		[ebx+2],al
		mov		al,wcirc[3]
		mov		[ebx+514],al
		mov		al,wcirc[4]
		mov		[ebx+512],al
		mov		al,wcirc[5]
		mov		[ebx+510],al
		mov		al,wcirc[6]
		mov		[ebx-2],al
		mov		al,wcirc[7]
		mov		[ebx-514],al
		xor		ebx,ebx
		mov		bh,byte ptr y
		mov		bl,byte ptr x
		add		ebx,offset WLock
		mov		al,wcirc[8]
		mov		[ebx-512],al
		mov		al,wcirc[9]
		mov		[ebx-510],al
		mov		al,wcirc[10]
		mov		[ebx+2],al
		mov		al,wcirc[11]
		mov		[ebx+514],al
		mov		al,wcirc[12]
		mov		[ebx+512],al
		mov		al,wcirc[13]
		mov		[ebx+510],al
		mov		al,wcirc[14]
		mov		[ebx-2],al
		mov		al,wcirc[15]
		mov		[ebx-514],al
	}
};
inline void WMemCirc(byte x,byte y){
	__asm{
		xor		ebx,ebx
		mov		bh,byte ptr y
		mov		bl,byte ptr x
		add		ebx,offset WMap
		mov		al,[ebx-512]
		mov		wcirc[0],al
		mov		al,[ebx-510]
		mov		wcirc[1],al
		mov		al,[ebx+2]
		mov		wcirc[2],al
		mov		al,[ebx+514]
		mov		wcirc[3],al
		mov		al,[ebx+512]
		mov		wcirc[4],al
		mov		al,[ebx+510]
		mov		wcirc[5],al
		mov		al,[ebx-2]
		mov		wcirc[6],al
		mov		al,[ebx-514]
		mov		wcirc[7],al
		xor		ebx,ebx
		mov		bh,byte ptr y
		mov		bl,byte ptr x
		add		ebx,offset WLock
		mov		al,[ebx-512]
		mov		wcirc[8],al
		mov		al,[ebx-510]
		mov		wcirc[9],al
		mov		al,[ebx+2]
		mov		wcirc[10],al
		mov		al,[ebx+514]
		mov		wcirc[11],al
		mov		al,[ebx+512]
		mov		wcirc[12],al
		mov		al,[ebx+510]
		mov		wcirc[13],al
		mov		al,[ebx-2]
		mov		wcirc[14],al
		mov		al,[ebx-514]
		mov		wcirc[15],al
	}
};
inline void WFCirc(byte x,byte y,byte x0,byte y0,byte x1,byte y1){
	__asm{
		xor		ebx,ebx
		mov		bl,x
		mov		bh,y
		cmp		bl,x0
		jne		lb1
		mov		WMap[ebx-514],1
		mov		WMap[ebx-2],1
		mov		WMap[ebx+510],1
		mov		WLock[ebx-514],1
		mov		WLock[ebx-2],1
		mov		WLock[ebx+510],1
lb1:	cmp		bl,x1
		jne		lb2
		mov		WMap[ebx-510],1
		mov		WMap[ebx+2],1
		mov		WMap[ebx+514],1
		mov		WLock[ebx-510],1
		mov		WLock[ebx+2],1
		mov		WLock[ebx+514],1
lb2:	cmp		bh,y0
		jne		lb3
		mov		WMap[ebx-514],1
		mov		WMap[ebx-512],1
		mov		WMap[ebx-510],1
		mov		WLock[ebx-514],1
		mov		WLock[ebx-512],1
		mov		WLock[ebx-510],1
lb3:	cmp		bh,y1
		jne		lb4
		mov		WMap[ebx+510],1
		mov		WMap[ebx+512],1
		mov		WMap[ebx+514],1
		mov		WLock[ebx+510],1
		mov		WLock[ebx+512],1
		mov		WLock[ebx+514],1
lb4:
	};
};

//Order Type=77
//Свалить с этой точки.Нефиг зря стоять.dir-направление,
//в котором движентся обЪект,который предлагает данному
//свалить с дороги. в этом направлении сдвигаться лишь в
//крайнем случае
void WMFLink(OneObject* OBJ);
void OneObject::WMoveFrom(int dir){
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
		Or1->DoLink=&WMFLink;
		//OrderReport=NULL;
		//MessageKind=0;
		//Sender=0xFFFF;
		//MFLink(this);
	};
};
void WMFLink(OneObject* OBJ){
	byte Direction=OBJ->Direction;
	byte dir=OBJ->LocalOrder->info.MoveFrom.dir;
	if(int(OBJ->InLineCom))OBJ->FreeAsmLink();
	OBJ->InLineCom=0;
	Order1* Loc1=OBJ->LocalOrder->NextOrder;
	OBJ->FreeOrdBlock(OBJ->LocalOrder);
	OBJ->LocalOrder=Loc1;
	int dx0=idrxw[Direction]<<1;
	int	dy0=idryw[Direction]<<1;
	int x=OBJ->x;
	int y=OBJ->y;
	if(OBJ->DrawUp){
		WMemCirc(x,y);
		WFCirc(x,y,OBJ->destX,OBJ->destY,OBJ->destX1,OBJ->destY1);
	};
	bool Dfound=false;
	int ddr;
	int tx;
	int ty;
	if(WLock[y+dy0][x+dx0]/*||dir==Direction*/){
		if(rando()<RAND_MAX/2){
			//Turn right
			ddr=(Direction+1)&7;
			int z=0;
			do{
				dx0=idrxw[ddr]<<1;
				dy0=idryw[ddr]<<1;
				ddr=(ddr+1)&7;
				z++;
				tx=x+dx0;
				ty=y+dy0;
				if(tx>=0&&ty>0&&tx<msx&&ty<msy&&!WLock[ty][tx])break;
			}while(z<8);
			ddr=(ddr+7)&7;
			if(z<8)Dfound=true;
		} else{
			//Turn left
			int ddr=(Direction+7)&7;
			int z=0;
			do{
				dx0=idrxw[ddr]<<1;
				dy0=idryw[ddr]<<1;
				ddr=(ddr+7)&7;
				z++;
			}while(WLock[y+dy0][x+dx0]&&z<8);
			ddr=(ddr+1)&7;
			if(z<8)Dfound=true;
		};
	}else{
		Dfound=true;
		ddr=Direction;
	};
	if(!Dfound&&!WLock[y+dy0][x+dx0]/*&&dir==Direction*/){
		Dfound=true;
		ddr=Direction;
		dx0=idrxw[ddr]<<1;
		dy0=idryw[ddr]<<1;
	};
	if(!Dfound){
		int z=rando();
		if(z&1){
			z=z>>1;
			//Turn right
			ddr=(Direction+(z&1))&7;
			int z=0;
			do{
				dx0=idrxw[ddr]<<1;
				dy0=idryw[ddr]<<1;
				ddr=(ddr+1)&7;
				z++;
				tx=x+dx0;
				ty=y+dy0;
				if(tx>=0&&ty>0&&tx<msx&&ty<msy&&!WMap[ty][tx])break;
			}while(z<8);
			ddr=(ddr+7)&7;
			if(z<8)Dfound=true;
		} else{
			//Turn left
			z=z>>1;
			int ddr=(Direction+8-(z&1))&7;
			int z=0;
			do{
				dx0=idrxw[ddr]<<1;
				dy0=idryw[ddr]<<1;
				ddr=(ddr+7)&7;
				z++;
			}while(WMap[y+dy0][x+dx0]&&z<8);
			ddr=(ddr+1)&7;
			if(z<8)Dfound=true;
		};
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
		WReMemCirc(x,y);
};