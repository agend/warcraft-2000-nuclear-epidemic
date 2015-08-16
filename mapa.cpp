#include "ddini.h"
#include <stdlib.h>
#include "ResFile.h"
#include "Fastdraw.h"
#include "MapDiscr.h"
#include "mouse.h"
#include "mode.h"
#include "fog.h"
#include "walls.h"
#include "Nature.h"
#include <crtdbg.h>
#include <math.h>
#include "TopZones.h"
#include "Megapolis.h"
#include "FlyObj.h"
#include "fonts.h"
#include "WeaponID.h"
#include "MapEdit.h"
#include "3DSurf.h"
#include "GSound.h"
#include <assert.h>
extern int time1,time2,time3,time4,time5;
int time6,time7,time8;
extern int HintX;
extern int HintY;
extern bool CheapMode;
extern int NMyUnits;
extern int NThemUnits;
extern RLCFont FPassive;
extern RLCFont FActive;
extern RLCFont FDisable;
int GetRLen(char* s,RLCFont* font);
word GetEnemy(int x,int y,byte NI);
word GetFriend(int x,int y,byte NI);
int COUNTER;
extern int BlobMode;
extern RLCTable IMM; 
extern int LASTRAND,LASTIND;
void ShowRMap();
extern int FogMode;
extern int RealLx;
extern int RealLy;
extern int Pitch;
extern Forces NForces[8];
word GetOwner(int x,int y);
extern bool GetCoord;
extern UniqMethood* UNIM;
//Карта неустранимых препятствий
extern int PathAsks;
extern int NMONS;
extern bool AttackMode;
extern int nEused;
extern bool EUsage[1024];
extern Nation* curn;
extern int counter;
extern bool InfoMode;
extern int WaitCycle;
extern int tmtmt;
extern bool EditMapMode;
extern bool HeightEditMode;
extern bool HelpMode;
extern bool ChoosePosition;
extern int LastKey;
const byte CLRT[8]={0xD0,0xD4,0xD8,0xDC,0xE0,0xE4,0xE8,0xAD};
void ShowSuperFluentFog(int x,int y,int z1,int z2,int z3,int z4);
void ShowDestn();
void ShowMiniDestn();
extern byte RNTB[64][256];
byte TrMap[maxmap][maxmap];
word SlMons[8192];
bool GetTask[8192];
void CreateMiniMap();
void ShowSuperFluentFog32_160(int x,int y,int z1x,int z2x,int z3x,int z4x);
word Nsel;
//Каринки для фона
byte tiles[32][8192];
byte mtiles[16][4096];
//описание картинок на карте
byte tmap[maxmap][maxmap];
//карта-ссылки на монстров
MapCell map[maxmap][maxmap];
//карта блокировок
byte LLock[maxmap][maxmap];
//Карта расположения монстров
word Mops[maxmap][maxmap];
word FlyMops[256][256];
//Карта для убитых
word Died[maxmap][maxmap];
bool BuildMode;
byte SpecCmd;
OneSlide* OSB;
byte blx;
byte bly;
word BuildingID;
Nation* BNat;
extern int NAsk;
extern int curptr;
int	smapx;
int	smapy;
int	smaplx;
int	smaply;
int	mapx;
int	mapy;
int	msx;
int msy;
int minix;
int	miniy;
static int StartX=0;
static int StartY=0;
bool MiniMade;
char Prompt[80];
int PromptTime;
byte minimap[maxmap][maxmap];
byte Locking[1024];//Инф. о блокировании поверхности
#define ScreenSizeX32 ScreenSizeX-32
//Массив все монстров на карте
OneObject* Group[8192];
//Mассив для локальных команд
//Индексы использования
bool	AsmUsage[MaxAsmCount];
char	AsmBuf[MaxAsmCount*OneAsmSize];
int		LastAsmRequest;
//Массив для команд первого уровня
bool	OrdUsage[MaxOrdCount];
Order1  OrdBuf[MaxOrdCount];
int		LastOrdRequest;
//network sequence errors enumeration
extern int SeqErrorsCount;
extern int LastRandDif;
extern int curdx;
extern int curdy;
//Фон
byte fon000[1024*768];
void RedSquare(int x,int y){
	if(x<mapx||x>=mapx+smaplx||y<mapy||y>=mapy+smaply)return;
	int sco=int(ScreenPtr)+smapx+((x-mapx)<<5)+
		(((y-mapy)<<5)+smapy)*SCRSizeX;
	int ddx=SCRSizeX+SCRSizeX-32;
	__asm{
		push	edi
		mov		edi,sco
		mov		eax,16
uyu:	mov		ecx,16
uuu:	mov		byte ptr [edi],clrRed
		add		edi,2
		loop	uuu
		add		edi,ddx
		dec		al
		jnz		uyu

		pop		edi
	};
};
void RedMiniSquare(int x,int y){
	if(x<mapx||x>=mapx+smaplx||y<mapy||y>=mapy+smaply)return;
	int sco=int(ScreenPtr)+smapx+((x-mapx)<<4)+
		(((y-mapy)<<4)+smapy)*SCRSizeX;
	int ddx=SCRSizeX+SCRSizeX-16;
	__asm{
		push	edi
		mov		edi,sco
		mov		eax,8
uyu:	mov		ecx,8
uuu:	mov		byte ptr [edi],clrRed
		add		edi,2
		loop	uuu
		add		edi,ddx
		dec		al
		jnz		uyu

		pop		edi
	};
};
void RedBar(int x,int y,int lx,int ly){
	for(int i=0;i<lx;i++)
		for(int j=0;j<ly;j++)
			RedSquare(x+i,y+j);
};
void RedMiniBar(int x,int y,int lx,int ly){
	for(int i=0;i<lx;i++)
		for(int j=0;j<ly;j++)
			RedMiniSquare(x+i,y+j);
};
void WhiteSquare(int x,int y){
	if(x<mapx||x>=mapx+smaplx||y<mapy||y>=mapy+smaply)return;
	int sco=int(ScreenPtr)+smapx+((x-mapx)<<5)+
		(((y-mapy)<<5)+smapy)*SCRSizeX;
	int ddx=SCRSizeX+SCRSizeX-32;
	__asm{
		push	edi
		mov		edi,sco
		mov		eax,16
uyu:	mov		ecx,16
uuu:	mov		byte ptr [edi],255
		add		edi,2
		loop	uuu
		add		edi,ddx
		dec		al
		jnz		uyu

		pop		edi
	};
};
void WhiteMiniSquare(int x,int y){
	if(x<mapx||x>=mapx+smaplx||y<mapy||y>=mapy+smaply)return;
	int sco=int(ScreenPtr)+smapx+((x-mapx)<<4)+
		(((y-mapy)<<4)+smapy)*SCRSizeX;
	int ddx=SCRSizeX+SCRSizeX-16;
	__asm{
		push	edi
		mov		edi,sco
		mov		eax,8
uyu:	mov		ecx,8
uuu:	mov		byte ptr [edi],255
		add		edi,2
		loop	uuu
		add		edi,ddx
		dec		al
		jnz		uyu

		pop		edi
	};
};
void WhiteBar(int x,int y,int lx,int ly){
	for(int i=0;i<lx;i++)
		for(int j=0;j<ly;j++)
			WhiteSquare(x+i,y+j);
};
void WhiteMiniBar(int x,int y,int lx,int ly){
	for(int i=0;i<lx;i++)
		for(int j=0;j<ly;j++)
			WhiteMiniSquare(x+i,y+j);
};
void OutErr(LPCSTR s)
{
	MessageBox(hwnd,s,"Loading failed...",MB_ICONWARNING|MB_OK);
};
bool ManualFogCheck(int xx,int yy,int dx){
	if(EditMapMode)return true;
	int x,y;
	for(int i=0;i<dx;i++)
		for(int j=0;j<dx;j++){
			x=xx+i;
			y=yy+j;
			if(x<=1||x>=msx||y<=1||y>=msy)return false;
			if(fmap[y][x]<1000)return false;
		};
	return true;
};
void AssignHint1(char* s,int time);
bool CheckGold(int xx,int yy,GeneralObject* GO){
	if(!GO->AGold)return true;
	for(int i=0;i<15;i++)
		for(int j=0;j<15;j++){
			int x=xx+i-5;
			int y=yy+j-5;
			if(x>1&&y>1&&x<=msx&&y<=msy){
				if(rtmap[y][x]==1&&ramap[y][x]){
					AssignHint1("Слишком близко к золоту.",4);
					return false;
				};
			};
		};
	return true;
};
//Получить блок для InLineCom
char* GetAsmBlock(){
	if(LastAsmRequest>=MaxAsmCount)LastAsmRequest=0;
	for(int i=LastAsmRequest;i<MaxAsmCount&&AsmUsage[i];i++);
	if(i<MaxAsmCount){
		LastAsmRequest=i+1;
		AsmUsage[i]=true;
		return &AsmBuf[i*OneAsmSize];
	} else{
		for(int i=0;i<LastAsmRequest&&AsmUsage[i];i++);
		if(i<LastAsmRequest){
			LastAsmRequest=i+1;
			AsmUsage[i]=true;
			return &AsmBuf[i*OneAsmSize];
		} else return NULL;
	};
};
void FreeAsmBlock(char* p ){
	int i=(int(p)-int(AsmBuf))>>OneAShift;
	AsmUsage[i]=false;
	LastAsmRequest=i;
};
void InitAsmBuf(){
	memset(AsmUsage,0,sizeof AsmUsage);
	memset(AsmBuf,0,sizeof AsmBuf);
	LastAsmRequest=0;
};
//Получить блок для LocalOrder
int CHSM1;
void GetOrdSum(){
	return;
	CHSM1=0;
	for(int i=0;i<MaxOrdCount;i++){
		if(OrdUsage[i])CHSM1+=i;
	};
};
void ChkOrdSum(){
	return;
	int xxx=CHSM1;
	GetOrdSum();
	assert(xxx=CHSM1);
};
Order1* GetOrdBlock(){
	ChkOrdSum();
	if(LastOrdRequest>=MaxOrdCount)LastOrdRequest=0;
	for(int i=LastOrdRequest;i<MaxOrdCount&&OrdUsage[i];i++);
	if(i<MaxOrdCount){
		LastOrdRequest=i+1;
		OrdUsage[i]=true;
		GetOrdSum();
		return &OrdBuf[i];
	} else{
		for(int i=0;i<LastOrdRequest&&OrdUsage[i];i++);
		if(i<LastOrdRequest){
			LastOrdRequest=i+1;
			OrdUsage[i]=true;
			GetOrdSum();
			return &OrdBuf[i];
		} else{
			GetOrdSum();			
			return NULL;
		};
	};
};
void OneObject::FreeOrdBlock(Order1* p ){
	ChkOrdSum();
	byte ot=p->OrderType;
	if(ot==3){
		Attack=false;
		EnemyID=0xFFFF;
		EnemyDist=5000;
	};
	if(ot==22)MoveInRect=false;
	int i=div(int(p)-int(OrdBuf),sizeof Order1).quot;
	OrdUsage[i]=false;
	LastOrdRequest=i;
	GetOrdSum();
};
void InitOrdBuf(){
	memset(OrdUsage,0,sizeof OrdUsage);
	memset(OrdBuf,0,sizeof OrdBuf);
	//LastOrdRequest=0;
};

void LoadLock()
{
	ResFile f=RReset("lock.dat");
	if(IOresult())OutErr("Can't load locking.");
	RBlockRead(f,Locking,RFileSize(f));
	RClose(f);
};
void LoadFon()
{
	if(RealLx==1024){
		ResFile ff1=RReset("fon1024p.raw");
		if(IOresult()) OutErr("Can't load fon1024p.raw");
		RBlockRead(ff1,fon000,RFileSize(ff1));
		RClose(ff1);
		return;
	};
	ResFile ff1=RReset("fon.000");
	if(IOresult()) OutErr("Can't load fon.000");
	RBlockRead(ff1,fon000,RFileSize(ff1));
	RClose(ff1);
};
void ShowFon()
{
	return;
	//if(SCRSizeY==768)return;
	int addX=RSCRSizeX-COPYSizeX;
	int SZX=(COPYSizeX)>>2;
	__asm{
		push	esi
		push	edi
		mov		esi,offset fon000
		mov		edi,RealScreenPtr
		mov		ebx,RSCRSizeY
		cld
uuu1:
		mov		ecx,SZX
		rep		movsd
		add		edi,addX
		dec		ebx
		jnz		uuu1
		pop		edi
		pop		esi
	}
};
void ShowFon1()
{
	//if(SCRSizeY==768)return;
	int addX=SCRSizeX-COPYSizeX;
	int SZX=(COPYSizeX)>>2;
	__asm{
		push	esi
		push	edi
		mov		esi,offset fon000
		mov		edi,ScreenPtr
		mov		ebx,RSCRSizeY
		cld
uuu1:
		mov		ecx,SZX
		rep		movsd
		add		edi,addX
		dec		ebx
		jnz		uuu1
		pop		edi
		pop		esi
	}	
};
void ShowBorder()
{
	return;
	/*int Start=smapx-32+smapy*ScreenSizeX;
	int	Ly=smaply*32;
	__asm{
		push	esi
		push	edi
		mov		ebx,Ly
		mov		esi,offset fon000
		add		esi,Start
		mov		edi,ScreenPtr
		add		edi,Start
		cld
Uuy1:	mov		ecx,8
		rep		movsd
		add		esi,ScreenSizeX-32
		add		edi,ScreenSizeX-32
		dec		ebx
		jnz		Uuy1
		pop		edi
		pop		esi
	}
	Start=smapx+smaplx*32+smapy*ScreenSizeX;
	__asm{
		push	esi
		push	edi
		mov		ebx,Ly
		mov		esi,offset fon000
		add		esi,Start
		mov		edi,ScreenPtr
		add		edi,Start
		cld
Uuy2:	mov		ecx,8
		rep		movsd
		add		esi,ScreenSizeX-32
		add		edi,ScreenSizeX-32
		dec		ebx
		jnz		Uuy2
		pop		edi
		pop		esi
	}*/
};
void LoadTiles()
{
	ResFile ff1=RReset("new_dark.bpx");//tiles.t00");
	if (IOresult()) OutErr("Can't load tiles.");
	RBlockRead(ff1,tiles,RFileSize(ff1));
	int ntiles=RFileSize(ff1)>>10;
	RClose(ff1);
	for(int i=0;i<ntiles;i++){
		for(int ux=0;ux<16;ux++)
			for(int uy=0;uy<16;uy++){
				((byte*)(mtiles))[(i<<8)+(ux)+(uy<<4)]=
					((byte*)(tiles))[(i<<10)+(ux<<1)+(uy<<6)];
			};
	};
	
};
void SaveWalls(ResFile ff1);
void LoadWalls(ResFile ff1);
char MapName[128];
extern short HiMap[256][256];//Height of surface
byte mapPos[16];
int RES[8][8];
void SaveExtendedMap(char* s){
	ResFile f1=RRewrite(s);
	int i='APAM';
	RBlockWrite(f1,&i,4);
	RBlockWrite(f1,MapName,128);
	RBlockWrite(f1,mapPos,16);
	//Saving Tiles Map
	RBlockWrite(f1,&msx,4);
	RBlockWrite(f1,&msy,4);
	RBlockWrite(f1,&RES[0][0],sizeof RES);
	for(i=0;i<msx;i++)RBlockWrite(f1,&tmap[i][0],msy);
	//Saving Height map
	for(i=0;i<=msy;i++)RBlockWrite(f1,&HiMap[i][0],(msx+1)<<1);
	//Oil spots
	int NU=0;
	for(int xx=0;xx<128;xx++)
		for(int yy=0;yy<128;yy++)if(OILMAP[yy][xx])NU++;
	RBlockWrite(f1,&NU,4);
	for(xx=0;xx<128;xx++)
		for(int yy=0;yy<128;yy++)if(OILMAP[yy][xx]){
			RBlockWrite(f1,&xx,1);
			RBlockWrite(f1,&yy,1);
			RBlockWrite(f1,&OILMAP[yy][xx],2);
		};
	//saving units
	NU=0;
	for(i=0;i<8192;i++){
		if(Group[i])NU++;
	};
	RBlockWrite(f1,&NU,4);
	for(i=0;i<8192;i++){
		OneObject* OB=Group[i];
		if(OB){
			RBlockWrite(f1,&OB->NIndex,2);
			RBlockWrite(f1,&OB->NNUM,1);
			RBlockWrite(f1,&OB->x,1);
			RBlockWrite(f1,&OB->y,1);
		};
	};
	SaveWalls(f1);
	RClose(f1);
};
void PreLoadExtendedMap(char* s){
	ResFile f1=RReset(s);
	InitRenderMap();
	int i='APAM';
	RBlockRead(f1,&i,4);
	if(i!='APAM'){
		RClose(f1);
		return;
	};
	//Loading tiles map
	RBlockRead(f1,MapName,128);
	RBlockRead(f1,mapPos,16);
	RBlockRead(f1,&msx,4);
	RBlockRead(f1,&msy,4);
	RBlockRead(f1,&RES[0][0],sizeof RES);
	memcpy(&RESRC[0][0],&RES[0][0],sizeof RES);
	for(i=0;i<msx;i++)RBlockRead(f1,&tmap[i][0],msy);
	//Loading height map
	for(i=0;i<=msy;i++)RBlockRead(f1,&HiMap[i][0],(msx+1)<<1);
	int NOIL;
	byte x,y;
	word AMOUNT;
	RBlockRead(f1,&NOIL,4);
	for(i=0;i<NOIL;i++){
		RBlockRead(f1,&x,1);
		RBlockRead(f1,&y,1);
		RBlockRead(f1,&AMOUNT,2);
		OILMAP[y][x]=AMOUNT;
	};
	PromptTime=0;
	MAXOBJECT=0;
	memset(&fmap[0][0],0,sizeof fmap);
	memset(Group,0,sizeof Group);
	memset(&TrMap[0][0],0,sizeof TrMap);
	memset(&LLock[0][0],0,sizeof LLock);
	memset(&WLock[0][0],0,sizeof WLock);
	memset(&WMap[0][0],0,sizeof WMap);
	memset(&map[0][0],0,sizeof map);
	memset(NLocks,0,sizeof NLocks);
	BuildMode=false;
	for(i=0;i<maxmap;i++)
	for(int j=0;j<maxmap;j++){
		//map[i][j].BuildingID=65535;
		Mops[j][i]=65535;
		Died[j][i]=65535;
		FlyMops[j][i]=65535;
		//map[i][j].FlyID=65535;
	};
	for(i=0;i<msx;i++)for(int j=0;j<msy;j++){
		byte k=Locking[tmap[j][i]];
		if(k&1){
			LLock[i+1][j+1]=1;
			IncLock(j+1,i+1);
			TrMap[i+1][j+1]=1;
		};
		//if(k&2)map[j][i].WaterLock=true;
	};
	for(i=0;i<=msx+1;i++){
		TrMap[i][0]=2;
		TrMap[i][msy+1]=2;
		LLock[i][0]=2;
		IncLock(0,i);
		LLock[i][msy+1]=2;
		IncLock(msy+1,i);
		WMap[i][0]=2;
		WMap[i][msy+1]=2;
		WLock[i][0]=2;
		WLock[i][msy+1]=2;
		WMap[i][0]=2;
		WMap[i][msy+2]=2;
		WLock[i][0]=2;
		WLock[i][msy+2]=2;

	};
	for(i=0;i<msy+1;i++){
		TrMap[0][i]=2;
		TrMap[msx+1][i]=2;
		LLock[0][i]=2;
		IncLock(i,0);
		LLock[msx+1][i]=2;
		IncLock(i,msx+1);
		WMap[0][i]=2;
		WMap[msx+1][i]=2;
		WLock[0][i]=2;
		WLock[msx+1][i]=2;
		WMap[0][i]=2;
		WMap[msx+2][i]=2;
		WLock[0][i]=2;
		WLock[msx+2][i]=2;
	};
	RClose(f1);
	Nsel=0;	
	CreateResMap();
	CreateWaterMap();
	CreateTZones();
	CreateCMap();
	CreateMiniMap();
};
void CreateUnit(Nation* NT,byte x,byte y,word ID){
	GeneralObject* GO=NT->Mon[ID];	
	if(!GO->cpbBuilding){
		NT->CreateTerrainAt(x,y,ID);
	}else{
		CheapMode=true;
		int oi=NT->CreateBuilding(ID,x,y);
		CheapMode=false;
		if(oi!=-1){
			OneObject* OB=Group[oi];
			OB->LoadCurAnm(0);
			OB->TicksPerChange=255;
			OB->Life=OB->MaxLife;
			OB->CurrentSprite=OB->CurAnm->count-1;
			if(OB->Ref.General->cpbFarm)NT->NFarms++;
			OB->Ready=true;
			OB->Stage=OB->Stage=OB->NStages;
		};
	};
};
void PostLoadExtendedMap(char* s){
	memset(&fmap[0][0],10,sizeof fmap);
	ResFile f1=RReset(s);
	int pos=12+16+128+8*8*4+(msx*msy)+(msx+1)*(msy+1)*2;
	RSeek(f1,pos);
	//Load units
	int NU;
	RBlockRead(f1,&NU,4);
	pos+=(NU+1)<<2;
	RSeek(f1,pos);
	RBlockRead(f1,&NU,4);
	byte NNUM,xu,yu;
	word NIndex;
	for(int i=0;i<NU;i++){
		RBlockRead(f1,&NIndex,2);
		RBlockRead(f1,&NNUM,1);
		RBlockRead(f1,&xu,1);
		RBlockRead(f1,&yu,1);
		Nation* NT=&NATIONS[NNUM];
		CreateUnit(NT,xu,yu,NIndex);
	};
	//walls
	LoadWalls(f1);
	RClose(f1);
	memset(&fmap[0][0],0,sizeof fmap);
};
void RestoreLock(int x,int y,int lx,int ly){
	for(int j=x-1;j<x+lx-1;j++)for(int i=y-1;i<y+ly-1;i++){
		byte k=Locking[tmap[j][i]];
		if(k&1){
			if(!LLock[i+1][j+1])IncLock(j+1,i+1);
			LLock[i+1][j+1]=1;
			TrMap[i+1][j+1]=1;
		}else{
			if(LLock[i+1][j+1])DecLock(j+1,i+1);
			LLock[i+1][j+1]=0;
			TrMap[i+1][j+1]=0;
		};
		k=Locking[tmap[j][i]];
		if(k&2){
			WLock[i+1][j+1]=1;
			WMap[i+1][j+1]=1;
		}else{
			WLock[i+1][j+1]=0;
			WMap[i+1][j+1]=0;
		};
		Mops[i+1][j+1]=0xFFFF;
	};
	
};
void LoadMap(LPCSTR s)
{
	PromptTime=0;
	MAXOBJECT=0;
	memset(Group,0,sizeof Group);
	memset(TrMap,0,sizeof TrMap);
	memset(LLock,0,sizeof LLock);
	memset(WLock,0,sizeof WLock);
	memset(WMap,0,sizeof WMap);
	memset(map,0,sizeof map);
	memset(NLocks,0,sizeof NLocks);
	BuildMode=false;
	for(int i=0;i<maxmap;i++)
	for(int j=0;j<maxmap;j++){
		//map[i][j].BuildingID=65535;
		Mops[j][i]=65535;
		Died[j][i]=65535;
		FlyMops[j][i]=65535;
		//map[i][j].FlyID=65535;
	};
	ResFile ff1=RReset(s);
	if (IOresult())OutErr("Can't load map file.");
	RBlockRead(ff1,&i,4);
	if (i!=0x1B46504D) OutErr("Incorrect map file format.");
	RBlockRead(ff1,&msx,4);
	RBlockRead(ff1,&msy,4);
	RBlockRead(ff1,&i,4);
	RBlockRead(ff1,&i,4);
	for(i=0;i<msy;i++)RBlockRead(ff1,&tmap[i][0],msx);
	for(i=0;i<msx;i++)for(int j=0;j<msy;j++){
		byte k=Locking[tmap[j][i]];
		if(k&1){
			LLock[i+1][j+1]=1;
			IncLock(j+1,i+1);
			TrMap[i+1][j+1]=1;
		};
		//if(k&2)map[j][i].WaterLock=true;
	};
	for(i=0;i<=msx+1;i++){
		TrMap[i][0]=2;
		TrMap[i][msy+1]=2;
		LLock[i][0]=2;
		IncLock(0,i);
		LLock[i][msy+1]=2;
		IncLock(msy+1,i);

	};
	for(i=0;i<msy+1;i++){
		TrMap[0][i]=2;
		TrMap[msx+1][i]=2;
		LLock[0][i]=2;
		IncLock(i,0);
		LLock[msx+1][i]=2;
		IncLock(i,msx+1);
	};
	RClose(ff1);
	RSCRSizeX=RealLx;
	RSCRSizeY=RealLy;
	COPYSizeX=RealLx;
	smaplx=17;
	if(RSCRSizeY==768)smaplx=22;
	smaply=smaplx;
	smapx=COPYSizeX-(smaplx<<5)-32;
	smapy=(RSCRSizeY-(smaply<<5))>>1;
	//smapx=7*32;
	//smapy=28;
	//smaplx=19;
	//smaply=19;
	mapx=1;
	mapy=1;
	minix=32;
	miniy=32;
	MiniMade=false;
	Nsel=0;	
	CreateResMap();
	CreateWaterMap();
	CreateTZones();
	CreateCMap();
	InitRenderMap();
};
void NewMap(int szX,int szY)
{
	PromptTime=0;
	MAXOBJECT=0;
	memset(Group,0,sizeof Group);
	memset(TrMap,0,sizeof TrMap);
	memset(LLock,0,sizeof LLock);
	memset(WLock,0,sizeof WLock);
	memset(map,0,sizeof map);
	memset(NLocks,0,sizeof NLocks);
	BuildMode=false;
	for(int i=0;i<maxmap;i++)
	for(int j=0;j<maxmap;j++){
		//map[i][j].BuildingID=65535;
		Mops[j][i]=65535;
		Died[j][i]=65535;
		FlyMops[j][i]=65535;
		//map[i][j].FlyID=65535;
	};
	memset(&tmap[0][0],0,sizeof tmap);
	memset(&HiMap[0][0],0,sizeof HiMap);
	for(i=0;i<msx;i++)for(int j=0;j<msy;j++){
		byte k=Locking[tmap[j][i]];
		if(k&1){
			LLock[i+1][j+1]=1;
			IncLock(j+1,i+1);
			TrMap[i+1][j+1]=1;
		};
		//if(k&2)map[j][i].WaterLock=true;
	};
	for(i=0;i<=msx+1;i++){
		TrMap[i][0]=2;
		TrMap[i][msy+1]=2;
		LLock[i][0]=2;
		IncLock(0,i);
		LLock[i][msy+1]=2;
		IncLock(msy+1,i);

	};
	for(i=0;i<msy+1;i++){
		TrMap[0][i]=2;
		TrMap[msx+1][i]=2;
		LLock[0][i]=2;
		IncLock(i,0);
		LLock[msx+1][i]=2;
		IncLock(i,msx+1);
	};
	MiniMade=false;
	Nsel=0;	
	CreateResMap();
	CreateWaterMap();
	CreateTZones();
	CreateCMap();
	InitRenderMap();
	SetTiles(0,0,256);
};
void CreateMiniMap()
{
	int i,j;
	char *yy=(char*)(&tiles);
	for(i=0;i<msx;i++)
		for(j=0;j<msy;j++)
			minimap[i>>1][j>>1]=yy[33+/*33*16+1+*/(int(tmap[j][i])<<10)];
	MiniMade=true;
};
const drx[8]={0,1,1,1,0,-1,-1,-1};
const dry[8]={-1,-1,0,1,1,1,0,-1};
extern bool HealthMode;
void GetRect(OneObject* ZZ,int* x,int* y,int* Lx){
	int scx,scy;
	if(ZZ->OnWater){
		scx=(int(ZZ->x)<<5)+(ZZ->Addx)-16;
		scy=(int(ZZ->y)<<5)+(ZZ->Addy)-16;
		int dy=64;
		int dx=64;
		if(ZZ->isMoving){
			int nsp=ZZ->CurAnm->count;
			if(nsp>3){
				scx+=div((int(ZZ->CurrentSprite)<<6)*drx[ZZ->Direction],nsp).quot;
				scy+=div((int(ZZ->CurrentSprite)<<6)*dry[ZZ->Direction],nsp).quot;
			};
		};
		if(ZZ->capBuilding){
			scx+=32;
			scy+=32;
		};
		*x=scx;
		*y=scy;
		*Lx=dy;
		return;
	};
	if(ZZ->Media==2){
		scx=(ZZ->RealX>>4-32)-32;
		scy=(ZZ->RealY>>4-32)-16-16+10-ZZ->Height;
		*x=scx;
		*y=scy;
		*Lx=64;
		return;
	};
	scx=(int(ZZ->x)<<5)+ZZ->Addx;
	scy=(int(ZZ->y)<<5)+ZZ->Addy;
	int dx=ZZ->Lx<<5;
	if(ZZ->isMoving){
		int nsp=ZZ->CurAnm->count;
		if(nsp>3){
			scx+=div((int(ZZ->CurrentSprite)<<5)*drx[ZZ->Direction],nsp).quot;
			scy+=div((int(ZZ->CurrentSprite)<<5)*dry[ZZ->Direction],nsp).quot;
		};
	};
	*x=scx;
	*y=scy;
	*Lx=dx;
};
void DrawMarker(OneObject* OB){
	if(OB){
		int x,y,Lx;
		GetRect(OB,&x,&y,&Lx);
		x+=smapx-(int(mapx)<<5);
		y+=smapy-(int(mapy)<<5);
		byte cl=clrYello;
		byte c1=0;
		byte c2=2;
		byte dc=4;
		int ddy=-12;
		Hline(x+c1,y,x+c2,cl);
		Hline(x+Lx-1-c2,y,x+Lx-1-c1,cl);
		Hline(x+c1,y+Lx-1,x+c2,cl);
		Hline(x+Lx-1-c2,y+Lx-1,x+Lx-1-c1,cl);
		Vline(x,y+c1,y+c2,cl);
		Vline(x+Lx-1,y+c1,y+c2,cl);
		Vline(x,y+Lx-1-c2,y+Lx-1-c1,cl);
		Vline(x+Lx-1,y+Lx-1-c2,y+Lx-1-c1,cl);
		if(HealthMode){
			int mhs=Lx-c2-c2-dc-dc;
			int hs=mhs;
			if(OB->MaxLife>0){
				hs=div(mhs*OB->Life,OB->MaxLife).quot;
			};
			Vline(-2+x+c2+dc,y+ddy,y+ddy+2,255);
			Vline(-2+x+Lx-1-c2,y+ddy,y+ddy+2,255);
			Vline(-2+x+c2+dc+1,y+ddy,y+ddy,255);
			Vline(-2+x+c2+dc+1,y+ddy+2,y+ddy+2,255);
			Vline(-2+x+Lx-1-c2-1,y+ddy,y+ddy,255);
			Vline(-2+x+Lx-1-c2-1,y+ddy+2,y+ddy+2,255);
			Hline(-2+x+c2+dc+1,y+ddy+1,x+c2+hs+dc+1,clrGreen);
		};
		/*
		Hline(x+1,y+1,x+1,cl);
		Hline(x+1,y+Lx-2,x+1,cl);
		Hline(x+Lx-2,y+1,x+Lx-2,cl);
		Hline(x+Lx-2,y+Lx-2,x+Lx-2,cl);*/
	};
};
void DrawMiniMarker(OneObject* OB){
	if(OB){
		int x,y,Lx;
		GetRect(OB,&x,&y,&Lx);
		x=x>>1;
		y=y>>1;
		Lx=Lx>>1;
		x+=smapx-(int(mapx)<<4);
		y+=smapy-(int(mapy)<<4);
		byte cl=clrYello;
		byte c1=0;
		byte c2=2;
		byte dc=4;
		int ddy=-12;
		Hline(x+c1,y,x+c2,cl);
		Hline(x+Lx-1-c2,y,x+Lx-1-c1,cl);
		Hline(x+c1,y+Lx-1,x+c2,cl);
		Hline(x+Lx-1-c2,y+Lx-1,x+Lx-1-c1,cl);
		Vline(x,y+c1,y+c2,cl);
		Vline(x+Lx-1,y+c1,y+c2,cl);
		Vline(x,y+Lx-1-c2,y+Lx-1-c1,cl);
		Vline(x+Lx-1,y+Lx-1-c2,y+Lx-1-c1,cl);
		if(HealthMode){
			int mhs=Lx-c2-c2-dc-dc;
			int hs=mhs;
			if(OB->MaxLife>0){
				hs=div(mhs*OB->Life,OB->MaxLife).quot;
			};
			Vline(-2+x+c2+dc,y+ddy,y+ddy+2,255);
			Vline(-2+x+Lx-1-c2,y+ddy,y+ddy+2,255);
			Vline(-2+x+c2+dc+1,y+ddy,y+ddy,255);
			Vline(-2+x+c2+dc+1,y+ddy+2,y+ddy+2,255);
			Vline(-2+x+Lx-1-c2-1,y+ddy,y+ddy,255);
			Vline(-2+x+Lx-1-c2-1,y+ddy+2,y+ddy+2,255);
			Hline(-2+x+c2+dc+1,y+ddy+1,x+c2+hs+dc+1,clrGreen);
		};
		/*
		Hline(x+1,y+1,x+1,cl);
		Hline(x+1,y+Lx-2,x+1,cl);
		Hline(x+Lx-2,y+1,x+Lx-2,cl);
		Hline(x+Lx-2,y+Lx-2,x+Lx-2,cl);*/
	};
};
void ShowHealth(OneObject* ZZ){
	if(!ZZ->Selected||ZZ->NNUM!=MyNation)return;
	DrawMarker(ZZ);
	return; 
};
void ShowMiniHealth(OneObject* ZZ){
	if(!ZZ->Selected||ZZ->NNUM!=MyNation)return;
	DrawMiniMarker(ZZ);
	return; 
};
extern void MakeFog(int x,int y);
void ShowFog(OneObject* ZZ){
	if(!(ZZ->InFire||ZZ->capBuilding))return;
	if(ZZ->InFire){
		if(rando()<2000)MakeFog((int(ZZ->x)<<7),(int(ZZ->y)<<7));
		return;
	};
	if(ZZ->Stage!=ZZ->NStages)return;
	int Life=ZZ->Life;
	int maxl=ZZ->Ref.Visual->info.Basic.MaxLife;
	int m1=maxl>>1;
	int m3=m1>>1;
	int m2=m1+m3;
	int xx,yy;
	int DX=int(ZZ->Lx-1)<<3;
	if(!DX)DX=1;
	if(Life<m1){
		if(rando()<8000){
			xx=(int(ZZ->x)<<7)+(int(RNTB[DX][rando()&255])<<4);
			yy=(int(ZZ->y)<<7)+(int(RNTB[DX][rando()&255])<<4);
			MakeFog(xx,yy);
		};
		if(Life<m2){
			if(rando()<8000){
				xx=(int(ZZ->x)<<7)+(int(RNTB[DX][rando()&255])<<4);
				yy=(int(ZZ->y)<<7)+(int(RNTB[DX][rando()&255])<<4);
				MakeFog(xx,yy);
			};
		};
	};
};
void ShowProperty(OneObject* OB){
	int mx=smapx+(int(OB->x-mapx)<<5)+OB->Addx;
	int my=smapy+(int(OB->y-mapy)<<5)+OB->Addy;
	if(OB->isMoving){
		int nsp=OB->CurAnm->count;
		if(nsp>3){
			mx+=div(34*OB->CurrentSprite*drx[OB->Direction],nsp).quot;
			my+=div(32*OB->CurrentSprite*dry[OB->Direction],nsp).quot;
		};
	};
	if(OB->xForce>16&&OB->xForce<=32){
		ShowRLCItem(mx,my-32,&IMM,3,0);
	};
	if(OB->xForce>32&&OB->xForce<=48){
		ShowRLCItem(mx,my-32,&IMM,3,0);
		ShowRLCItem(mx+10,my-32+10,&IMM,1,0);
	};
	if(OB->xForce>48){
		ShowRLCItem(mx,my-32,&IMM,3,0);
		ShowRLCItem(mx+10,my-32+10,&IMM,1,0);
		ShowRLCItem(mx,my-32+12,&IMM,1,0);
	};
	if(OB->xForce<16){
		ShowRLCItem(mx,my-32,&IMM,0,0);
	};
	if(OB->Invisible){
		ShowRLCItem(mx,my-32,&IMM,3,0);
	};

};
static int tmt=0;
void CBar(int x,int y,int Lx,int Ly,byte c);
void ShowMap()
{
	int i;
	int j;
	int scr;
	int til;
	int uuu=(tmt&31);
	tmt++;
	for(j=0;j<smaply;j++)
		for(i=0;i<smaplx;i++)if(fmap[j+mapy][i+mapx]>512){
			scr=int(ScreenPtr)+(i<<5)+smapx+((j<<5)+smapy)*SCRSizeX;
			til=int(&tiles)+(int(tmap[i+mapx-1][j+mapy-1])<<10);
			int SDX=SCRSizeX-32;
			//if(uuu<10)if(TrMap[j+mapy][i+mapx])til=int(&tiles)+1024*98;
			//if(uuu>10&&uuu<21)
			//if(LLock[j+mapy][i+mapx])til=int(&tiles)+1024*98;
			/*__asm{
				push	esi
				push	edi
				mov		eax,31
				cld
				mov		edi,scr
				mov		esi,til
Lab12:			mov		ecx,31
				rep		movsb
				inc		esi;
				inc		edi
				add		edi,ScreenSizeX32
				dec		eax
				jnz		Lab12
				pop		edi
				pop		esi
			}*/
			__asm{
				push	esi
				push	edi
				mov		eax,32
				cld
				mov		edi,scr
				mov		esi,til
Lab12:			mov		ecx,8
				rep		movsd
				/*fild	qword ptr [esi]
				fist	qword ptr [edi]
				fild	qword ptr [esi+8]
				fist	qword ptr [edi+8]
				fild	qword ptr [esi+16]
				fist	qword ptr [edi+16]
				fild	qword ptr [esi+24]
				fist	qword ptr [edi+24]
				add		esi,32
				add		edi,32*/
				add		edi,SDX
				dec		eax
				jnz		Lab12
				pop		edi
				pop		esi
			};
				/*if(LLock[j+mapy][i+mapx])
					Xbar(smapx+i*32+2,smapy+j*32+2,28,28,15);
				if(TrMap[j+mapy][i+mapx]){
					Xbar(smapx+i*32,smapy+j*32,32,32,15);
					Xbar(smapx+i*32+1,smapy+j*32+1,30,30,15);
				};*/
	}
	SetRLCWindow(0,smapy,COPYSizeX+128,32*smaply,SCRSizeX);
	ShowResScreen();
	//SetRLCWindow(smapx,smapy,32*smaplx,32*smaply,32*ScreenSizeX);
	for(j=0;j<smaply;j++){
		for(i=-1;i<=smaplx;i++){
			int p=Died[j+mapy][i+mapx];
			if(p!=0xFFFF){
				OneObject* GG=Group[p];
				Octant* Oc2=GG->CurAnm;
				OneSlide* Oc1=&(Oc2->Movie[GG->CurrentSprite]);
				byte clr=GG->NNUM;
				int spr=Oc1->spr;
				if(spr<4096)
				if(Group[p]->Invert) 
				ShowRLCItem((i+1)*32+smapx-Oc1->dx,j*32+smapy+Oc1->dy,
					&MImage[Oc1->FileID],spr+4096,clr);
				else
					ShowRLCItem(i*32+smapx+Oc1->dx,j*32+smapy+Oc1->dy,
						&MImage[Oc1->FileID],spr,clr);
				//ShowHealth(GG);
				//ShowFog(GG);
			};
		};
		for(i=-1;i<=smaplx;i++){
			int p=Mops[j+mapy][i+mapx];
			if(p!=0xFFFF){
				OneObject* GG=Group[p];
				int	ddy=GG->Addy;
				if(ddy<0){
					byte clr=GG->NNUM;
					Octant* Oc2=GG->CurAnm;
					int	ddx=GG->Addx;
					OneSlide* Oc1=&(Oc2->Movie[GG->CurrentSprite]);
					int spr=Oc1->spr;
					if(spr<4096)
					if(Group[p]->Invert) 
						ShowRLCItem(ddx+(i+1)*32+smapx-Oc1->dx,ddy+j*32+smapy+Oc1->dy,
							&MImage[Oc1->FileID],spr+4096,clr);
					else
						ShowRLCItem(ddx+i*32+smapx+Oc1->dx,ddy+j*32+smapy+Oc1->dy,
							&MImage[Oc1->FileID],spr,clr);
					ShowHealth(GG);
					ShowProperty(GG);
				};
			};
		};
		for(i=-3;i<smaplx;i++){
			int p=i+mapx>0?Mops[j+mapy][i+mapx]:0xffff;
			if(p!=0xFFFF){
				OneObject* GG=Group[p];
				int	ddy=GG->Addy;
				if(ddy==0){
					byte clr=GG->NNUM;
					if(!(GG->capBuilding&&(GG->x!=i+mapx||GG->y!=j+mapy))){
						Octant* Oc2=GG->CurAnm;
						int	ddx=GG->Addx;
						OneSlide* Oc1=&(Oc2->Movie[GG->CurrentSprite]);
						int spr=Oc1->spr;
						if(spr<4096)
						if(Group[p]->Invert) 
							ShowRLCItem(ddx+(i+1)*32+smapx-Oc1->dx,ddy+j*32+smapy+Oc1->dy,
								&MImage[Oc1->FileID],spr+4096,clr);
						else
							ShowRLCItem(ddx+i*32+smapx+Oc1->dx,ddy+j*32+smapy+Oc1->dy,
								&MImage[Oc1->FileID],spr,clr);
						ShowHealth(GG);
						ShowProperty(GG);
					};
				
				};
			};
		};
		for(i=0;i<smaplx;i++){
			int p=Mops[j+mapy][i+mapx];
			if(p!=0xFFFF){
				OneObject* GG=Group[p];
				int	ddy=GG->Addy;
				if(ddy>0){
					byte clr=GG->NNUM;
					Octant* Oc2=GG->CurAnm;
					int	ddx=GG->Addx;
					OneSlide* Oc1=&(Oc2->Movie[GG->CurrentSprite]);
					int spr=Oc1->spr;
					if(spr<4096)
					if(Group[p]->Invert) 
						ShowRLCItem(ddx+(i+1)*32+smapx-Oc1->dx,ddy+j*32+smapy+Oc1->dy,
							&MImage[Oc1->FileID],spr+4096,clr);
					else
						ShowRLCItem(ddx+i*32+smapx+Oc1->dx,ddy+j*32+smapy+Oc1->dy,
							&MImage[Oc1->FileID],spr,clr);
					ShowHealth(GG);
					ShowProperty(GG);
				};
			};
		}
	};
	ProcessExpl();
	ShowFlyingMonsters();
	ProcessFog();
	SetRLCWindow(0,0,COPYSizeX,RSCRSizeY,SCRSizeX);
	//void* SCR1=ScreenPtr;
	//if(!DDDebug)ScreenPtr=RealScreenPtr;
	//ShowBorder();
	if(!MiniMade)CreateMiniMap();
	int ssx;
	int ssy;
	if(msx>128)ssx=msx>>3;else ssx=msx>>2;
	if(msy>128)ssy=msy>>1;else ssy=msy;
	for(i=0;i<ssy;i++){
		//if(
		scr=int(ScreenPtr)+minix+(i+miniy)*SCRSizeX;
		//til=int(&TrMap)+i*maxmap;
		til=int(&minimap)+i*maxmap;
		//til=int(AsmUsage)+i*128;
		//til=int(EUsage)+i*128;
		//til=int(&T[0][0])+i*256;
		__asm{
			push	esi
			push	edi
			mov		ecx,ssx
			cld
			mov		esi,til
			mov		edi,scr
			rep		movsd
uiyt:
		/*	fild	qword ptr [esi]
			add		esi,8
			fist	qword ptr [edi]
			add		edi,8
			loop	uiyt*/
			pop		edi
			pop		esi
		}
	}
	CBar(minix-1,miniy-1,msx+2,msy+2,255);
	/*for(i=0;i<msy;i++){
		scr=int(RealScreenPtr)+minix+(i+miniy+140)*ScreenSizeX;
		//til=int(&TrMap)+i*maxmap;
		til=int(&minimap)+i*maxmap;
		til=int(OrdUsage)+i*128;
		//til=int(EUsage)+i*128;
		__asm{
			push	esi
			push	edi
			mov		ecx,ssx
			cld
			mov		esi,til
			mov		edi,scr
			rep		movsd
			pop		edi
			pop		esi
		}
	}*/
	if(BuildMode){
		if(mouseX>=smapx&&mouseY>=smapy&&mouseX<smapx*(smaplx-blx+1)*32&&mouseY<smapy+(smaply-bly+1)*32){
			int xxx=mouseX&(65535-31);
			int yyy=mouseY&(65535-31);
			xxx=mapx+((xxx-smapx)>>5);	
			yyy=mapy+((yyy-smapy)>>5);
			if(!(CheckGold(xxx,yyy,BNat->Mon[BuildingID])&&ManualFogCheck(xxx,yyy,BNat->Mon[BuildingID]->SizeX)&&BNat->CheckBuilding(BuildingID,xxx,yyy)))
				RedBar(xxx,yyy,blx,bly);else
				WhiteBar(xxx,yyy,blx,bly);
			xxx=mouseX&(65535-31);
			yyy=mouseY&(65535-31);
			ShowRLCItem(xxx+OSB->dx,yyy+OSB->dy,
			&MImage[OSB->FileID],OSB->spr,MyNation);
			
		};
	};
	if(BuildWall)TMPCluster.ShowCluster();
	ShowProp();
	ShowAbility();
	word mp;
	OneObject* OO;
	byte mxx,myy,val;
	for(int g=0;g<MAXOBJECT;g++){
		OO=Group[g];
		if(OO){
			mxx=OO->x>>1;
			myy=OO->y>>1;
			val=CLRT[OO->NNUM];
			scr=int(ScreenPtr)+minix+mxx+(myy+miniy)*SCRSizeX;
			__asm{
				mov	eax,scr
				mov	bl,val
				mov	[eax],bl
			};
		};
	};
	/*
	for(i=0;i<msx;i++)for(j=0;j<msy;j++){
		mp=Mops[j][i];
		if(mp!=0xFFFF){
			scr=int(ScreenPtr)+minix+i+(j+miniy)*SCRSizeX;
			byte val=Group[mp]->NMask;
			__asm{
				mov	eax,scr
				mov	bl,val
				mov	[eax],bl
			};
		};
	};
	*/
	AcceptMiniMap();
	ShowFoggedBattle();
	Xbar(minix+(mapx>>1),miniy+(mapy>>1),smaplx>>1,smaply>>1,14);
	//ScreenPtr=SCR1;
	int x=mouseX;
	int y=mouseY;
	ssy=smapy+3;
	//if(PromptTime>0){
	//	ShowString(smapx,smapy+smaply*32-32,Prompt,&f16x16w);
	//	PromptTime--;
	//};
	ProcessHint();
	ssy+=20;
	char ggg[600];
	int uxxx=mapx+((mouseX-smapx)>>5);
	int uyyy=mapy+((mouseY-smapy)>>5);
	OutCInf(uxxx,uyyy);
	int sy1=RSCRSizeY-128;
	ShowString(5,sy1,"\x1 Авто атака",&f16x16w);
	sy1+=16;
	ShowString(5,sy1,"\x1 Помогать товарищу",&f16x16w);
	sy1+=16;
	ShowString(5,sy1,"\x1 Отвечать на атаку",&f16x16w);
	sy1+=16;
	ShowString(5,sy1,"\x3\x4 Поражение стен:10%",&f16x16r);
	sy1+=16;
	//ShowString(20,RSCRSizeY-64,"\x1\x2\x3\x4\x5\x6\x7",&f16x16w);
	sprintf(ggg,"(%d,%d)",uxxx,uyyy);
	ShowString(smapx,smapy+smaply*32-64-32,ggg,&f16x16w);
	Nation* NT=&NATIONS[MyNation];
	//_CrtMemState CMS;
	//_CrtMemCheckpoint(&CMS);
	//sprintf(ggg,"ALLOCATED:%d" ,CMS.lTotalCount);
	//ShowString(smapx,ssy,ggg,&f16x16w);
	//ssy+=16;
	//sprintf(ggg,"PITCH:%d" ,Pitch);
	//ShowString(smapx,ssy,ggg,&f16x16w);
	//ssy+=16;
	sprintf(ggg,"Farms:%d,Gidot:%d" ,NT->NFarms,NT->NGidot);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"%d,%d" ,LASTIND,LASTRAND);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"Time:%d" ,tmtmt);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"Sequene errors:%d (%d)" ,SeqErrorsCount,LastRandDif);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	ShowRMap();
	if(InfoMode)return;
	sprintf(ggg,"Strong factor:%d" ,NForces[MyNation].StrongFactor);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	//if(AttackMode)ShowString(smapx,ssy,"Режим атаки",&f16x16w);
	//else ShowString(smapx,ssy,"Режим перемещения",&f16x16w);
	//ssy+=16;
	if(EgoFlag)ShowString(smapx,ssy,"Режим сваливания от врага",&f16x16w);
	else ShowString(smapx,ssy,"Режим борьбы с врагом",&f16x16w);
	ssy+=16;
	sprintf(ggg,"FPS:%d, Waiting: %d" ,Flips,WaitCycle);
	counter=0;
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"Всего тварюк:%d",NMONS);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=20;
	int sapx=smapx+4;
	if(!NSL[MyNation])return;
	OneObject* OBB=Group[Selm[MyNation][0]];
	if(int(OBB)){
		sprintf(ggg,"Номер избранной тварюки :%d,(x=%d,y=%d)",
			OBB->Index,
			OBB->x,
			OBB->y);
		ShowString(smapx,ssy,ggg,&f16x16w);
		ssy+=16;
		sprintf(ggg,"Нумер племени:%d",OBB->NMask);
		ShowString(sapx,ssy,ggg,&f16x16w);
		ssy+=16;
		sprintf(ggg,"Vx,Vy=%d,%d",OBB->RealVx,OBB->RealVy);
		ShowString(sapx,ssy,ggg,&f16x16w);
		ssy+=16;
		/*sprintf(ggg,"Возгордилась ль тварь?:%d",OBB->Important&1);
		ShowString(sapx,ssy,ggg,&f16x16w);
		ssy+=16;*/
		//sprintf(ggg,"Жлобъ:%d",OBB->Egoist&1);
		//ShowString(sapx,ssy,ggg,&f16x16w);
		//ssy+=16;
		if(OBB->CrowdRef){
			ssy+=10;
			Crowd* CR=OBB->CrowdRef;
			ShowString(sapx,ssy,CR->Message,&f16x16w);
			ssy+=16;
			sprintf(ggg,"Атаковать:");
			char gxx[10];
			for(int u=0;u<32;u++){
				int pp=CR->ForAtt[u];
				if(pp!=0xFFFF){
					sprintf(gxx,"%d(%d) ",pp,u);
					strcat(ggg,gxx);
				};
			};
			ShowString(sapx,ssy,ggg,&f16x16w);
			ssy+=16;
			sprintf(ggg,"Численность:%d",CR->NMembers);
			ShowString(sapx,ssy,ggg,&f16x16w);
			ssy+=16;
			//sprintf(ggg,"SizeX:%d",L->SizeX1);
			//ShowString(sapx,ssy,ggg,&f16x16w);
			//ssy+=16;
		};
		if(OBB->MoveInRect){
			sprintf(ggg,"Квадрат назначения:(%d,%d)-(%d,%d)",
				OBB->destX,
				OBB->destY,
				OBB->destX1,
				OBB->destY1);
			ShowString(sapx,ssy,ggg,&f16x16w);
			ssy+=16;
			sprintf(ggg,"Точка назначения:(%d,%d)",
				OBB->lastX,
				OBB->lastY);
			ShowString(sapx,ssy,ggg,&f16x16w);
			ssy+=20;
		};
		if(int(OBB->LocalOrder)){
			ShowString(sapx-4,ssy,"Што мне нонче делать.",&f16x16w);
			ssy+=16;
			Order1* LOL=OBB->LocalOrder;
			do{
				switch(LOL->OrderType){
				case 2:	sprintf(ggg,"Повеление:(Подь ка в:x=%d,y=%d)",
							LOL->info.MoveToXY.x,
							LOL->info.MoveToXY.y);
						break;
				case 77:sprintf(ggg,"Повеление:(Уйди с дороги, окаянный!:dir=%d)%d",
							LOL->info.MoveFrom.dir);
						break;
				case 3:sprintf(ggg,"Повеление:(Мочить тварь:%d)",
							OBB->EnemyID);
						break;
				default:sprintf(ggg,"Повеление:(Дурницi якiсь:%d)",
							LOL->OrderType);
				};
				ShowString(sapx,ssy,ggg,&rlf_s);
				ssy+=16;
				LOL=LOL->NextOrder;
				if(ssy>500)return;
			}while(int(LOL));
		};
		ssy=smapy;
		sapx=smapx+smaplx*16+100;
		/*if(int(OBB->Wars)){
			ShowString(sapx,ssy,"Legion information",&f16x16w);
			ssy+=20;
			Legion* L=OBB->Wars;
			sprintf(ggg,"Members:%d",L->NMemb);
			ShowString(sapx,ssy,ggg,&f16x16w);
			ssy+=16;
			sprintf(ggg,"Full complectation:%d",L->NNeed);
			ShowString(sapx,ssy,ggg,&f16x16w);
			ssy+=16;
			sprintf(ggg,"Minx:%d",L->minx);
			ShowString(sapx,ssy,ggg,&f16x16w);
			ssy+=16;
			sprintf(ggg,"Miny:%d",L->miny);
			ShowString(sapx,ssy,ggg,&f16x16w);
			ssy+=16;
			sprintf(ggg,"Maxx:%d",L->maxx);
			ShowString(sapx,ssy,ggg,&f16x16w);
			ssy+=16;
			sprintf(ggg,"Maxy:%d",L->maxy);
			ShowString(sapx,ssy,ggg,&f16x16w);
			ssy+=16;
			sprintf(ggg,"SizeX:%d",L->SizeX1);
			ShowString(sapx,ssy,ggg,&f16x16w);
			ssy+=16;
			sprintf(ggg,"SizeY:%d",L->SizeY1);
			ShowString(sapx,ssy,ggg,&f16x16w);
			ssy+=16;
			sprintf(ggg,"Порядок:%d",L->Ordering);
			ShowString(sapx,ssy,ggg,&f16x16w);
			ssy+=16;
			int wdx=0;
			int nn=0;
			while(ssy<600-20&&nn<L->NMemb){
				int zz=L->Member[nn];
				sprintf(ggg,"%d",zz);
				ShowString(sapx+wdx*32,ssy,ggg,&f16x16w);
				wdx++;
				if(wdx>5){
					wdx=0;
					ssy+=16;
				};
				nn++;
			};
		};*/
	};
};
void ShowWinner(){
	char sss[128]="";
	if(!NMyUnits){
		strcpy(sss,"ВЫ ПРОИГРАЛИ !");
	}else{
		if(!NThemUnits)
			strcpy(sss,"ПОЗДРАВЛЯЕМ ПОБЕДИТЕЛЯ!");
	};
	int lx=GetRLen(sss,&FPassive)>>1;
	if(lx)ShowString(smapx+(smaplx<<(Shifter-1))-lx,smapy+(smaply<<(Shifter-1)),sss,&FPassive);
};
//final procedures for graphics output
void miniGFieldShow();
void GFieldShow(){
	if(MiniMode){
		miniGFieldShow();
		return;
	};
	int i;
	int j;
	int scr;
	int til;
	int uuu=(tmt&31);
	char ggg[600];
	tmt++;
	time1=GetTickCount();
	if(FogMode){
		for(j=0;j<smaply;j++)
			for(i=0;i<smaplx;i++)if(fmap[j+mapy][i+mapx]>512){
				int til=int(RenderSquare(i+mapx,j+mapy));
				if(til){
					scr=int(ScreenPtr)+(i<<5)+smapx+((j<<5)+smapy)*SCRSizeX;
					int SDX=SCRSizeX-32;
					__asm{
						push	esi
						push	edi
						mov		eax,32
						cld
						mov		edi,scr
						mov		esi,til
Lab12:					mov		ecx,8
						rep		movsd
						add		edi,SDX
						dec		eax
						jnz		Lab12
						pop		edi
						pop		esi
					};	
				};
			};
	}else{
		for(j=0;j<smaply;j++)
			for(i=0;i<smaplx;i++){
				int til=int(RenderSquare(i+mapx,j+mapy));
				if(til){
					scr=int(ScreenPtr)+(i<<5)+smapx+((j<<5)+smapy)*SCRSizeX;
					int SDX=SCRSizeX-32;
					__asm{
						push	esi
						push	edi
						mov		eax,32
						cld
						mov		edi,scr
						mov		esi,til
Labx12:					mov		ecx,8
						rep		movsd
						add		edi,SDX
						dec		eax
						jnz		Labx12
						pop		edi
						pop		esi
					};	
				};
			};
	};
	time6=GetTickCount()-time1;
			/*
			scr=int(ScreenPtr)+(i<<5)+smapx+((j<<5)+smapy)*SCRSizeX;
			til=int(&tiles)+(int(tmap[i+mapx-1][j+mapy-1])<<10);
			int SDX=SCRSizeX-32;
			__asm{
				push	esi
				push	edi
				mov		eax,32
				cld
				mov		edi,scr
				mov		esi,til
Lab12:			mov		ecx,8
				rep		movsd
				add		edi,SDX
				dec		eax
				jnz		Lab12
				pop		edi
				pop		esi
			};*/
	
	SetRLCWindow(smapx,smapy,smaplx<<5,smaply<<5,SCRSizeX);
	if(EditMapMode){
		for(i=0;i<8;i++){
			int x=mapPos[i<<1];
			int y=mapPos[(i<<1)+1];
			if(x>=mapx&&y>=mapy&&x<mapx+smaplx&&y<mapy+smaply){
				sprintf(ggg,"%d",i+1);
				ShowString(smapx+((x-mapx)<<5)+8,smapy+((y-mapy)<<5)+2,ggg,&f32x32w);
			};
		};
	};
	ShowResScreen();
	for(j=0;j<smaply;j++){
		for(i=-1;i<=smaplx;i++){
			int p=Died[j+mapy][i+mapx];
			if(p!=0xFFFF){
				OneObject* GG=Group[p];
				Octant* Oc2=GG->CurAnm;
				OneSlide* Oc1=&(Oc2->Movie[GG->CurrentSprite]);
				byte clr=GG->NNUM;
				int spr=Oc1->spr;
				if(spr<4096)
				if(Group[p]->Invert) 
				ShowRLCItem((i+1)*32+smapx-Oc1->dx,j*32+smapy+Oc1->dy,
					&MImage[Oc1->FileID],spr+4096,clr);
				else
					ShowRLCItem(i*32+smapx+Oc1->dx,j*32+smapy+Oc1->dy,
						&MImage[Oc1->FileID],spr,clr);
			};
		};
		for(i=-1;i<=smaplx;i++){
			int p=Mops[j+mapy][i+mapx];
			if(p!=0xFFFF){
				OneObject* GG=Group[p];
				if(GG){
					int	ddy=GG->Addy;
					if(ddy<0){
						byte clr=GG->NNUM;
						Octant* Oc2=GG->CurAnm;
						int	ddx=GG->Addx;
						OneSlide* Oc1=&(Oc2->Movie[GG->CurrentSprite]);
						int spr=Oc1->spr;
						//-----------
						int hy=GetHig((int(GG->x)<<5)+GG->Addx,(int(GG->y)<<5)+GG->Addy);
						//-----------
						if(spr<4096)
						if(Group[p]->Invert) 
							ShowRLCItem(ddx+(i+1)*32+smapx-Oc1->dx,ddy+j*32+smapy+Oc1->dy,
								&MImage[Oc1->FileID],spr+4096,clr);
						else
							ShowRLCItem(ddx+i*32+smapx+Oc1->dx,ddy+j*32+smapy+Oc1->dy,
								&MImage[Oc1->FileID],spr,clr);
						ShowHealth(GG);
						ShowProperty(GG);
					};
				};
			};
		};
		for(i=-3;i<smaplx;i++){
			int p=i+mapx>0?Mops[j+mapy][i+mapx]:0xffff;
			if(p!=0xFFFF){
				OneObject* GG=Group[p];
				if(GG){
					int	ddy=GG->Addy;
					if(ddy==0){
						byte clr=GG->NNUM;
						if(!(GG->capBuilding&&(GG->x!=i+mapx||GG->y!=j+mapy))){
							Octant* Oc2=GG->CurAnm;
							//-----------
							int hy=GetHig((int(GG->x)<<5)+GG->Addx,(int(GG->y)<<5)+GG->Addy);
							//-----------
							int	ddx=GG->Addx;
							OneSlide* Oc1=&(Oc2->Movie[GG->CurrentSprite]);
							int spr=Oc1->spr;
							if(spr<4096)
							if(Group[p]->Invert) 
								ShowRLCItem(ddx+(i+1)*32+smapx-Oc1->dx,ddy+j*32+smapy+Oc1->dy,
									&MImage[Oc1->FileID],spr+4096,clr);
							else
								ShowRLCItem(ddx+i*32+smapx+Oc1->dx,ddy+j*32+smapy+Oc1->dy,
									&MImage[Oc1->FileID],spr,clr);
							ShowHealth(GG);
							ShowProperty(GG);
						};
					
					};
				};
			};
		};
		for(i=0;i<smaplx;i++){
			int p=Mops[j+mapy][i+mapx];
			if(p!=0xFFFF){
				OneObject* GG=Group[p];
				if(GG){
					int	ddy=GG->Addy;
				if(ddy>0){
						byte clr=GG->NNUM;
						Octant* Oc2=GG->CurAnm;
						//-----------
						int hy=GetHig((int(GG->x)<<5)+GG->Addx,(int(GG->y)<<5)+GG->Addy);
						//-----------
						int	ddx=GG->Addx;
						OneSlide* Oc1=&(Oc2->Movie[GG->CurrentSprite]);
						int spr=Oc1->spr;
						if(spr<4096)
						if(Group[p]->Invert) 
							ShowRLCItem(ddx+(i+1)*32+smapx-Oc1->dx,ddy+j*32+smapy+Oc1->dy,
								&MImage[Oc1->FileID],spr+4096,clr);
						else
							ShowRLCItem(ddx+i*32+smapx+Oc1->dx,ddy+j*32+smapy+Oc1->dy,
								&MImage[Oc1->FileID],spr,clr);
						ShowHealth(GG);
						ShowProperty(GG);
					};
				};
			};
		}
	};
	//for(int jj=0;jj<10;jj++)
	//ShowSuperFluentFog(smapx+32,smapy+32,31,31,31,0);
	ProcessExpl();
	ShowFlyingMonsters();
	if(FogMode)ProcessFog();
	int v1=3;
	int v2=158;
	//ShowSuperFluentFog32_160(200,200,v1,v1,v1,v2);
	//ShowSuperFluentFog32_160(200+32,200,v1,v1,v2,v1);
	//ShowSuperFluentFog32_160(200,200+32,v1,v2,v1,v1);
	//ShowSuperFluentFog32_160(200+32,200+32,v2,v1,v1,v1);
	if(EditMapMode){
		if(ChoosePosition)
			sprintf(ggg,"Игрок %d : Выбор стартовой позиции.",MyNation);
		else sprintf(ggg,"Игрок %d.",MyNation+1);
		ShowString(smapx+32,smapy+64,ggg,&f16x16w);
	};
	SetRLCWindow(0,0,COPYSizeX,RSCRSizeY,SCRSizeX);
	if(BuildMode){
		if(mouseX>=smapx&&mouseY>=smapy&&mouseX<smapx*(smaplx-blx+1)*32&&mouseY<smapy+(smaply-bly+1)*32){
			int xxx=mouseX&(65535-31);
			int yyy=mouseY&(65535-31);
			xxx=mapx+((xxx-smapx)>>5);	
			yyy=mapy+((yyy-smapy)>>5);
			if(!(CheckGold(xxx,yyy,BNat->Mon[BuildingID])&&ManualFogCheck(xxx,yyy,BNat->Mon[BuildingID]->SizeX)&&BNat->CheckBuilding(BuildingID,xxx,yyy)))
				RedBar(xxx,yyy,blx,bly);else
				WhiteBar(xxx,yyy,blx,bly);
			xxx=mouseX&(65535-31);
			yyy=mouseY&(65535-31);
			ShowRLCItem(xxx+OSB->dx,yyy+OSB->dy,
				&MImage[OSB->FileID],OSB->spr,MyNation&3);
			
		};
	};
	if(BuildWall)TMPCluster.ShowCluster();
	if(FogMode)ShowFoggedBattle();
	ProcessHint();
	ShowDestn();
	time7=GetTickCount()-time1-time6;
	if(SeqErrorsCount){
		ShowString(HintX,HintY-100,"ВНИМАНИЕ!!! НАРУШЕНИЕ СИНХРОНИЗАЦИИ !!!!",&FActive);
		SeqErrorsCount=0;
	};
	if(EditMapMode){
		for(i=0;i<8;i++){
			RESRC[i][1]=100000;
			RESRC[i][2]=100000;
			RESRC[i][3]=100000;
		};
	}else ShowWinner();
	//information
	if(HelpMode){
		int scx=smapx+64;
		int scy=smapy+128;
		if(EditMapMode){
			ShowString(scx+100,scy,"Реактор карты. (F1 - убрать надписи)",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"'P' - выбор объекта",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"F1  - Help",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"F2  - Трава",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"F3  - Пустыня",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"F4  - Лес",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"F5  - Вода",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"F6  - Камни",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"F7  - Создать выпуклость",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"F8  - Создать впадину",&f16x16w);
			scy+=20;
			//ShowString(scx,scy,"F8  - Сглаживание",&f16x16w);
			//scy+=20;
			ShowString(scx,scy,"F9  - режим управления",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"'1'..'9' - Выбор размера пера",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"'W' - Постройка стены",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"'T' - Выбор стартовой позиции",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"'R' - Начальные ресурсы",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"NUM 1..8 - Выбор нации",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"L  - Режим карты",&f16x16w);
			scy+=20;
		};
	};
	ShowRMap();
	if(EditMapMode)return;
	if(InfoMode)return;
	int ssy=smapy+3;
	ssy+=20;
	sprintf(ggg,"LastKey=%d" ,LastKey);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	int uxxx=mapx+((mouseX-smapx)>>5);
	int uyyy=mapy+((mouseY-smapy)>>5);
	OutCInf(uxxx,uyyy);
	sprintf(ggg,"(%d,%d)",uxxx,uyyy);
	ShowString(smapx,smapy+smaply*32-64-32,ggg,&f16x16w);
	Nation* NT=&NATIONS[MyNation];
	sprintf(ggg,"AI=%d" ,NT->AI_Level+1);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"Farms:%d,Gidot:%d" ,NT->NFarms,NT->NGidot);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"%d,%d" ,LASTIND,LASTRAND);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"CNTR: %d" ,COUNTER);
	ShowString(smapx,ssy,ggg,&f16x16w);
	COUNTER=0;
	ssy+=16;
	sprintf(ggg,"Time 2: %d" ,time2);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"Time 3: %d" ,time3);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"Time 4: %d" ,time4);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"Time 5: %d" ,time5);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"Time 6: %d" ,time6);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"Time 7: %d" ,time7);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"Time 8: %d" ,time8);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"Time:%d" ,tmtmt);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"Sequene errors:%d (%d)" ,SeqErrorsCount,LastRandDif);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	//if(!InfoMode)return;
	sprintf(ggg,"Strong factor:%d" ,NForces[MyNation].StrongFactor);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"RSCRSizeX:%d" ,RSCRSizeX);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"minix=%d,miniy=%d" ,minix,miniy);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"mx=%d,my=%d,lp=%d" ,mouseX,mouseY,Lpressed);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"LPitch:%d" ,ddsd.lPitch);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	if(EgoFlag)ShowString(smapx,ssy,"Режим сваливания от врага",&f16x16w);
	else ShowString(smapx,ssy,"Режим борьбы с врагом",&f16x16w);
	ssy+=16;
	sprintf(ggg,"FPS:%d, Waiting: %d" ,Flips,WaitCycle);
	counter=0;
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"Всего тварюк:%d",NMONS);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=20;
	int sapx=smapx+4;
	if(!NSL[MyNation])return;
	OneObject* OBB=Group[Selm[MyNation][0]];
	if(int(OBB)){
		sprintf(ggg,"Номер избранной тварюки :%d,(x=%d,y=%d)",
			OBB->Index,
			OBB->x,
			OBB->y);
		ShowString(smapx,ssy,ggg,&f16x16w);
		ssy+=16;
		sprintf(ggg,"Нумер племени:%d",OBB->NMask);
		ShowString(sapx,ssy,ggg,&f16x16w);
		ssy+=16;
		sprintf(ggg,"Vx,Vy=%d,%d",OBB->RealVx,OBB->RealVy);
		ShowString(sapx,ssy,ggg,&f16x16w);
		ssy+=16;
		if(OBB->CrowdRef){
			ssy+=10;
			Crowd* CR=OBB->CrowdRef;
			ShowString(sapx,ssy,CR->Message,&f16x16w);
			ssy+=16;
			sprintf(ggg,"Атаковать:");
			char gxx[10];
			for(int u=0;u<32;u++){
				int pp=CR->ForAtt[u];
				if(pp!=0xFFFF){
					sprintf(gxx,"%d(%d) ",pp,u);
					strcat(ggg,gxx);
				};
			};
			ShowString(sapx,ssy,ggg,&f16x16w);
			ssy+=16;
			sprintf(ggg,"Численность:%d",CR->NMembers);
			ShowString(sapx,ssy,ggg,&f16x16w);
			ssy+=16;
		};
		if(OBB->MoveInRect){
			sprintf(ggg,"Квадрат назначения:(%d,%d)-(%d,%d)",
				OBB->destX,
				OBB->destY,
				OBB->destX1,
				OBB->destY1);
			ShowString(sapx,ssy,ggg,&f16x16w);
			ssy+=16;
			sprintf(ggg,"Точка назначения:(%d,%d)",
				OBB->lastX,
				OBB->lastY);
			ShowString(sapx,ssy,ggg,&f16x16w);
			ssy+=20;
		};
		if(int(OBB->LocalOrder)){
			ShowString(sapx-4,ssy,"Што мне нонче делать.",&f16x16w);
			ssy+=16;
			Order1* LOL=OBB->LocalOrder;
			do{
				switch(LOL->OrderType){
				case 2:	sprintf(ggg,"Повеление:(Подь ка в:x=%d,y=%d)",
							LOL->info.MoveToXY.x,
							LOL->info.MoveToXY.y);
						break;
				case 77:sprintf(ggg,"Повеление:(Уйди с дороги, окаянный!:dir=%d)%d",
							LOL->info.MoveFrom.dir);
						break;
				case 3:sprintf(ggg,"Повеление:(Мочить тварь:%d)",
							OBB->EnemyID);
						break;
				default:sprintf(ggg,"Повеление:(Дурницi якiсь:%d)",
							LOL->OrderType);
				};
				ShowString(sapx,ssy,ggg,&rlf_s);
				ssy+=16;
				LOL=LOL->NextOrder;
				if(ssy>500)return;
			}while(int(LOL));
		};
		ssy=smapy;
		sapx=smapx+smaplx*16+100;
	};
};
void miniShowFlyingMonsters();
void miniShowSuperFog();
void miniGFieldShow(){
	int i;
	int j;
	int scr;
	int til;
	int uuu=(tmt&31);
	char ggg[600];
	tmt++;
	if(FogMode){
		for(j=0;j<smaply;j++)
			for(i=0;i<smaplx;i++)if(fmap[j+mapy][i+mapx]>512){
				//int til=int(&mtiles[0][0])+(int(tmap[i+mapx-1][j+mapy-1])<<8);
				int til=int(RenderMiniSquare(i+mapx,j+mapy));
				if(til){
					scr=int(ScreenPtr)+(i<<4)+smapx+((j<<4)+smapy)*SCRSizeX;
					int SDX=SCRSizeX-16;
					__asm{
						push	esi
						push	edi
						mov		eax,16
						cld
						mov		edi,scr
						mov		esi,til
Lab12:					mov		ecx,4
						rep		movsd
						add		edi,SDX
						dec		eax
						jnz		Lab12
						pop		edi
						pop		esi
					};	
				};
				//if(LLock[j+mapy][i+mapx])
				//	Xbar((i<<4)+smapx+6,(j<<4)+smapy+6,4,4,255);
			};
	}else{
		for(j=0;j<smaply;j++)
			for(i=0;i<smaplx;i++){
				//int til=int(&mtiles[0][0])+(int(tmap[i+mapx-1][j+mapy-1])<<8);
				int til=int(RenderMiniSquare(i+mapx,j+mapy));
				if(til){
					scr=int(ScreenPtr)+(i<<4)+smapx+((j<<4)+smapy)*SCRSizeX;
					int SDX=SCRSizeX-16;
					__asm{
						push	esi
						push	edi
						mov		eax,16
						cld
						mov		edi,scr
						mov		esi,til
Lab13:					mov		ecx,4
						rep		movsd
						add		edi,SDX
						dec		eax
						jnz		Lab13
						pop		edi
						pop		esi
					};	
				};
				//if(LLock[j+mapy][i+mapx])
				//	Xbar((i<<4)+smapx+6,(j<<4)+smapy+6,4,4,255);
			};
	};
	SetRLCWindow(smapx,smapy,(smaplx<<4),(smaply<<4),SCRSizeX);
	//ShowResScreen();
	for(j=0;j<smaply;j++){
		for(i=-1;i<=smaplx;i++){
			int p=Died[j+mapy][i+mapx];
			if(p!=0xFFFF){
				OneObject* GG=Group[p];
				Octant* Oc2=GG->CurAnm;
				OneSlide* Oc1=&(Oc2->Movie[GG->CurrentSprite]);
				byte clr=GG->NNUM;
				int spr=Oc1->spr;
				if(spr<4096)
				if(Group[p]->Invert) 
				ShowRLCItem(((i+1)<<4)+smapx-(Oc1->dx>>1),(j<<4)+smapy+(Oc1->dy>>1),
					&miniMImage[Oc1->FileID],spr+4096,clr);
				else
					ShowRLCItem((i<<4)+smapx+(Oc1->dx>>1),(j<<4)+smapy+(Oc1->dy>>1),
						&miniMImage[Oc1->FileID],spr,clr);
			};
		};
		for(i=-1;i<=smaplx;i++){
			int p=Mops[j+mapy][i+mapx];
			if(p!=0xFFFF){
				OneObject* GG=Group[p];
				int	ddy=GG->Addy>>1;
				if(ddy<0){
					byte clr=GG->NNUM;
					Octant* Oc2=GG->CurAnm;
					int	ddx=GG->Addx>>1;
					OneSlide* Oc1=&(Oc2->Movie[GG->CurrentSprite]);
					int spr=Oc1->spr;
					//-----------
					//int hy=GetHig((int(GG->x)<<5)+GG->Addx,(int(GG->y)<<5)+GG->Addy);
					//-----------
					if(spr<4096)
					if(Group[p]->Invert) 
						ShowRLCItem(ddx+((i+1)<<4)+smapx-(Oc1->dx>>1),ddy+(j<<4)+smapy+(Oc1->dy>>1),
							&miniMImage[Oc1->FileID],spr+4096,clr);
					else
						ShowRLCItem(ddx+(i<<4)+smapx+(Oc1->dx>>1),ddy+(j<<4)+smapy+(Oc1->dy>>1),
							&miniMImage[Oc1->FileID],spr,clr);
					ShowMiniHealth(GG);
					//ShowProperty(GG);
				};
			};
		};
		for(i=-3;i<smaplx;i++){
			int p=i+mapx>0?Mops[j+mapy][i+mapx]:0xffff;
			if(p!=0xFFFF){
				OneObject* GG=Group[p];
				int	ddy=GG->Addy>>1;
				if(ddy==0){
					byte clr=GG->NNUM;
					if(!(GG->capBuilding&&(GG->x!=i+mapx||GG->y!=j+mapy))){
						Octant* Oc2=GG->CurAnm;
						//-----------
						//int hy=GetHig((int(GG->x)<<5)+GG->Addx,(int(GG->y)<<5)+GG->Addy);
						//-----------
						int	ddx=GG->Addx>>1;
						OneSlide* Oc1=&(Oc2->Movie[GG->CurrentSprite]);
						int spr=Oc1->spr;
						if(spr<4096)
						if(Group[p]->Invert) 
							ShowRLCItem(ddx+((i+1)<<4)+smapx-(Oc1->dx>>1),ddy+(j<<4)+smapy+(Oc1->dy>>1),
								&miniMImage[Oc1->FileID],spr+4096,clr);
						else
							ShowRLCItem(ddx+(i<<4)+smapx+(Oc1->dx>>1),ddy+(j<<4)+smapy+(Oc1->dy>>1),
								&miniMImage[Oc1->FileID],spr,clr);
						ShowMiniHealth(GG);
						//ShowProperty(GG);
					};
				
				};
			};
		};
		for(i=0;i<smaplx;i++){
			int p=Mops[j+mapy][i+mapx];
			if(p!=0xFFFF){
				OneObject* GG=Group[p];
				int	ddy=GG->Addy>>1;
				if(ddy>0){
					byte clr=GG->NNUM;
					Octant* Oc2=GG->CurAnm;
					//-----------
					//int hy=GetHig((int(GG->x)<<5)+GG->Addx,(int(GG->y)<<5)+GG->Addy);
					//-----------
					int	ddx=GG->Addx>>1;
					OneSlide* Oc1=&(Oc2->Movie[GG->CurrentSprite]);
					int spr=Oc1->spr;
					if(spr<4096)
					if(Group[p]->Invert) 
						ShowRLCItem(ddx+((i+1)<<4)+smapx-(Oc1->dx>>1),ddy+(j<<4)+smapy+(Oc1->dy>>1),
							&miniMImage[Oc1->FileID],spr+4096,clr);
					else
						ShowRLCItem(ddx+(i<<4)+smapx+(Oc1->dx>>1),ddy+(j<<4)+smapy+(Oc1->dy>>1),
							&miniMImage[Oc1->FileID],spr,clr);
					ShowMiniHealth(GG);
					//ShowProperty(GG);
				};
			};
		}
	};
	//for(int jj=0;jj<10;jj++)
	//ShowSuperFluentFog(smapx+32,smapy+32,31,31,31,0);
	ProcessExpl();
	miniShowFlyingMonsters();
	if(FogMode){
		ProcessFog();
	};
	int v1=3;
	int v2=158;
	//ShowSuperFluentFog32_160(200,200,v1,v1,v1,v2);
	//ShowSuperFluentFog32_160(200+32,200,v1,v1,v2,v1);
	//ShowSuperFluentFog32_160(200,200+32,v1,v2,v1,v1);
	//ShowSuperFluentFog32_160(200+32,200+32,v2,v1,v1,v1);
	if(EditMapMode){
		if(ChoosePosition)
			sprintf(ggg,"Игрок %d : Выбор стартовой позиции.",MyNation);
		else sprintf(ggg,"Игрок %d.",MyNation+1);
		ShowString(smapx+32,smapy+64,ggg,&f16x16w);
	};
	SetRLCWindow(0,0,COPYSizeX,RSCRSizeY,SCRSizeX);
	if(BuildMode){
		if(mouseX>=smapx&&mouseY>=smapy&&mouseX<smapx*(smaplx-blx+1)*32&&mouseY<smapy+(smaply-bly+1)*32){
			int xxx=mouseX&(65535-15);
			int yyy=mouseY&(65535-15);
			xxx=mapx+((xxx-smapx)>>4);	
			yyy=mapy+((yyy-smapy)>>4);
			if(!(CheckGold(xxx,yyy,BNat->Mon[BuildingID])&&ManualFogCheck(xxx,yyy,BNat->Mon[BuildingID]->SizeX)&&BNat->CheckBuilding(BuildingID,xxx,yyy)))
				RedMiniBar(xxx,yyy,blx,bly);else
				WhiteMiniBar(xxx,yyy,blx,bly);
			xxx=mouseX&(65535-15);
			yyy=mouseY&(65535-15);
			ShowRLCItem(xxx+(OSB->dx>>1),yyy+(OSB->dy>>1),
				&miniMImage[OSB->FileID],OSB->spr,MyNation);
			
		};
	};
	if(BuildWall)TMPCluster.ShowCluster();
	if(FogMode)
		miniShowSuperFog();
	ProcessHint();
	ShowDestn();
	if(SeqErrorsCount){
		for(i=0;i<20;i++)
				Hline(smapx,i*32+smapy,RSCRSizeX,255);
	};
	if(EditMapMode){
		for(i=0;i<8;i++){
			RESRC[i][1]=100000;
			RESRC[i][2]=100000;
			RESRC[i][3]=100000;
		};
	}else ShowWinner();
	//information
	if(HelpMode){
		int scx=smapx+64;
		int scy=smapy+128;
		if(EditMapMode){
			ShowString(scx+100,scy,"Реактор карты. (F1 - убрать надписи)",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"'P' - выбор объекта",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"F1  - Help",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"F2  - Трава",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"F3  - Пустыня",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"F4  - Лес",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"F5  - Вода",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"F6  - Камни",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"F7  - Создать выпуклость",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"F8  - Создать впадину",&f16x16w);
			scy+=20;
			//ShowString(scx,scy,"F8  - Сглаживание",&f16x16w);
			//scy+=20;
			ShowString(scx,scy,"F9  - режим управления",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"'1'..'9' - Выбор размера пера",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"'W' - Постройка стены",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"'L' - Выбор стартовой позиции",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"'R' - Начальные ресурсы",&f16x16w);
			scy+=20;
			ShowString(scx,scy,"NUM 1..8 - Выбор нации",&f16x16w);
			scy+=20;
		};
	};
	ShowRMap();
	if(EditMapMode&&NSL[MyNation]){
		OneObject* OBB=Group[Selm[MyNation][0]];
		if(int(OBB)){
			if(EditMapMode){
				if(OBB->PathX){
					for(int i=OBB->CurIPoint;i<OBB->NIPoints;i++){
						int xx=smapx+((int(OBB->PathX[i])-mapx)<<4);
						int yy=smapy+((int(OBB->PathY[i])-mapy)<<4);
						Xbar(xx+13,yy+13,6,6,255);
					};
				};
			};
		};
	};
	if(EditMapMode)return;
	if(InfoMode)return;
	int ssy=smapy+3;
	ssy+=20;
	sprintf(ggg,"LastKey=%d" ,LastKey);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	int uxxx=mapx+((mouseX-smapx)>>5);
	int uyyy=mapy+((mouseY-smapy)>>5);
	OutCInf(uxxx,uyyy);
	sprintf(ggg,"(%d,%d)",uxxx,uyyy);
	ShowString(smapx,smapy+smaply*32-64-32,ggg,&f16x16w);
	Nation* NT=&NATIONS[MyNation];
	sprintf(ggg,"AI=%d" ,NT->AI_Level+1);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"Farms:%d,Gidot:%d" ,NT->NFarms,NT->NGidot);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"%d,%d" ,LASTIND,LASTRAND);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"%d" ,COUNTER);
	COUNTER=0;
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"Time:%d" ,tmtmt);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"Sequene errors:%d (%d)" ,SeqErrorsCount,LastRandDif);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	//if(!InfoMode)return;
	sprintf(ggg,"Strong factor:%d" ,NForces[MyNation].StrongFactor);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"RSCRSizeX:%d" ,RSCRSizeX);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"minix=%d,miniy=%d" ,minix,miniy);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"mx=%d,my=%d,lp=%d" ,mouseX,mouseY,Lpressed);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"LPitch:%d" ,ddsd.lPitch);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	if(EgoFlag)ShowString(smapx,ssy,"Режим сваливания от врага",&f16x16w);
	else ShowString(smapx,ssy,"Режим борьбы с врагом",&f16x16w);
	ssy+=16;
	sprintf(ggg,"FPS:%d, Waiting: %d" ,Flips,WaitCycle);
	counter=0;
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=16;
	sprintf(ggg,"Всего тварюк:%d",NMONS);
	ShowString(smapx,ssy,ggg,&f16x16w);
	ssy+=20;
	int sapx=smapx+4;
	if(!NSL[MyNation])return;
	OneObject* OBB=Group[Selm[MyNation][0]];
	if(int(OBB)){
		sprintf(ggg,"Номер избранной тварюки :%d,(x=%d,y=%d)",
			OBB->Index,
			OBB->x,
			OBB->y);
		ShowString(smapx,ssy,ggg,&f16x16w);
		ssy+=16;
		sprintf(ggg,"Нумер племени:%d",OBB->NMask);
		ShowString(sapx,ssy,ggg,&f16x16w);
		ssy+=16;
		sprintf(ggg,"Vx,Vy=%d,%d",OBB->RealVx,OBB->RealVy);
		ShowString(sapx,ssy,ggg,&f16x16w);
		ssy+=16;
		if(OBB->CrowdRef){
			ssy+=10;
			Crowd* CR=OBB->CrowdRef;
			ShowString(sapx,ssy,CR->Message,&f16x16w);
			ssy+=16;
			sprintf(ggg,"Атаковать:");
			char gxx[10];
			for(int u=0;u<32;u++){
				int pp=CR->ForAtt[u];
				if(pp!=0xFFFF){
					sprintf(gxx,"%d(%d) ",pp,u);
					strcat(ggg,gxx);
				};
			};
			ShowString(sapx,ssy,ggg,&f16x16w);
			ssy+=16;
			sprintf(ggg,"Численность:%d",CR->NMembers);
			ShowString(sapx,ssy,ggg,&f16x16w);
			ssy+=16;
		};
		if(OBB->MoveInRect){
			sprintf(ggg,"Квадрат назначения:(%d,%d)-(%d,%d)",
				OBB->destX,
				OBB->destY,
				OBB->destX1,
				OBB->destY1);
			ShowString(sapx,ssy,ggg,&f16x16w);
			ssy+=16;
			sprintf(ggg,"Точка назначения:(%d,%d)",
				OBB->lastX,
				OBB->lastY);
			ShowString(sapx,ssy,ggg,&f16x16w);
			ssy+=20;
		};
		if(int(OBB->LocalOrder)){
			ShowString(sapx-4,ssy,"Што мне нонче делать.",&f16x16w);
			ssy+=16;
			Order1* LOL=OBB->LocalOrder;
			do{
				switch(LOL->OrderType){
				case 2:	sprintf(ggg,"Повеление:(Подь ка в:x=%d,y=%d)",
							LOL->info.MoveToXY.x,
							LOL->info.MoveToXY.y);
						break;
				case 77:sprintf(ggg,"Повеление:(Уйди с дороги, окаянный!:dir=%d)%d",
							LOL->info.MoveFrom.dir);
						break;
				case 3:sprintf(ggg,"Повеление:(Мочить тварь:%d)",
							OBB->EnemyID);
						break;
				default:sprintf(ggg,"Повеление:(Дурницi якiсь:%d)",
							LOL->OrderType);
				};
				ShowString(sapx,ssy,ggg,&rlf_s);
				ssy+=16;
				LOL=LOL->NextOrder;
				if(ssy>500)return;
			}while(int(LOL));
		};
		ssy=smapy;
		sapx=smapx+smaplx*16+100;
	};
};
void ShowNucl();
void GMiniShow(){
if(!MiniMade)CreateMiniMap();
	int ssx;
	int ssy;
	if(msx>=128)ssx=msx>>3;else ssx=msx>>2;
	if(msy>=128)ssy=msy>>1;else ssy=msy;
	for(int i=0;i<ssy;i++){
		int scr=int(ScreenPtr)+minix+(i+miniy)*SCRSizeX;
		int til=int(&minimap)+i*maxmap;
		//int til=int(OrdUsage)+i*128;
		__asm{
			push	esi
			push	edi
			mov		ecx,ssx
			cld
			mov		esi,til
			mov		edi,scr
			rep		movsd
uiyt:		pop		edi
			pop		esi
		}
	}
	word mp;
	OneObject* OO;
	byte mxx,myy,val;
	for(int g=0;g<MAXOBJECT;g++){
		OO=Group[g];
		if(OO){
			mxx=(OO->x-1)>>1;
			myy=(OO->y-1)>>1;
			val=CLRT[OO->NNUM];
			int scr=int(ScreenPtr)+minix+mxx+(myy+miniy)*SCRSizeX;
			__asm{
				mov	eax,scr
				mov	bl,val
				mov	[eax],bl
			};
		};
	};
	if(EditMapMode){
		int x=mapPos[(MyNation<<1)]>>1;
		int y=mapPos[(MyNation<<1)+1]>>1;
		SetRLCWindow(minix,miniy,msx>>1,msy>>1,SCRSizeX);
		Hline(minix+x-2,miniy+y,minix+x-2+4,255);
		Vline(minix+x,miniy+y-2,miniy+y-2+4,255);
		SetRLCWindow(0,0,COPYSizeX,RSCRSizeY,SCRSizeX);
	};
	if(FogMode)AcceptMiniMap();
	ShowNucl();
	Xbar(minix+(mapx>>1),miniy+(mapy>>1),smaplx>>1,smaply>>1,14);
	ShowMiniDestn();
	int mx2=msx>>1;
	int my2=msy>>1;
	Hline(minix-1,miniy-1,minix+mx2,15);
	Hline(minix-1,miniy+my2,minix+mx2,15);
	Vline(minix-1,miniy-1,miniy+my2,15);
	Vline(minix+mx2,miniy,miniy+my2,15);
};
//------------------------------------
#define step 1
void SetEgoFlag(bool Ego){
	word MID;
	OneObject* OB;
	for(int i=0;i<Nsel;i++){
		MID=SlMons[i];
		if(MID!=0xFFFF){
			OB=Group[MID];
			if(int(OB))OB->Egoist=Ego;
		};
	};
};
extern int  ReliefBrush;
extern int  TerrBrush;
bool SelSoundDone;
extern int SelSoundType;
extern bool SelSoundReady;
void DoSelSound(int type){
	if(!SelSoundDone){
		SelSoundDone=true;
		SelSoundReady=true;
		SelSoundType=type;
	};
};
extern bool fixed;
extern bool realLpressed;
extern bool realRpressed;
void MFix(){
	fixed=true;
	if(Lpressed)Lpressed=realLpressed;
	if(Rpressed)Rpressed=realRpressed;
};
void HandleMouse(int x,int y)
{	
	bool cooren=false;
	bool mini=false;
	int xmx,ymy;
	if(x>=minix&&y>=miniy&&x<minix+(msx>>1)&&y<miniy+(msy>>1)){
		SetRLCWindow(minix,miniy,msx>>1,msy>>1,SCRSizeX);
		Hline(x-4,y,x-4+8,255);
		Vline(x,y-4,y-4+8,255);
		SetRLCWindow(0,0,COPYSizeX,RSCRSizeY,SCRSizeX);
		cooren=true;
		xmx=(x-minix)<<1;
		ymy=(y-miniy)<<1;
		if(xmx<1)xmx=1;
		if(ymy<1)ymy=1;
		if(xmx>=msx)xmx=msx-1;
		if(ymy>=msy)ymy=msy-1;
		xmx=(xmx<<5)+16;
		ymy=(ymy<<5)+16;
		mini=true;
	}else{
		if(x>smapx&&y>smapy&&x<smapx+(smaplx<<5)&&y<smapy+(smaply<<5)){
			xmx=(mapx<<5)+((x-smapx)<<(5-Shifter));
			ymy=(mapy<<5)+((y-smapy)<<(5-Shifter));
			cooren=true;
		};
	};
	if(GetCoord){
		if(Lpressed){
			if(cooren){
				if(UNIM)UNIM(0,xmx>>5,ymy>>5);
			};
			Lpressed=false;
			curptr=0;
			curdx=0;
			curdy=0;
			GetCoord=false;
			DoSelSound(1);
			MFix();
			return;
		}
		if(Rpressed){
			curptr=0;
			curdx=0;
			curdy=0;
			GetCoord=false;
			MFix();
			return;
		};
	};
	if(SpecCmd==1)
		CmdCreateGoodSelection(MyNation,0,0,msx<<5,msy<<5);
	if(SpecCmd==2)CmdCreateGoodSelection(MyNation,mapx<<5,mapy<<5,(mapx+smaplx-1)<<5,(mapy+smaply-1)<<5);
	//Select all peasants
	if(SpecCmd==3)CmdCreateGoodKindSelection(MyNation,0,0,msx<<5,msy<<5,0);
	//On screen
	if(SpecCmd==4)CmdCreateGoodKindSelection(MyNation,mapx<<5,mapy<<5,(mapx+smaplx-1)<<5,(mapy+smaply-1)<<5,0);
	//Select all warriors
	if(SpecCmd==5)CmdCreateGoodKindSelection(MyNation,0,0,msx<<5,msy<<5,2);
	//On screen
	if(SpecCmd==6)CmdCreateGoodKindSelection(MyNation,mapx<<5,mapy<<5,(mapx+smaplx-1)<<5,(mapy+smaply-1)<<5,2);
	//Select all towers
	if(SpecCmd==7)CmdCreateGoodKindSelection(MyNation,0,0,msx<<5,msy<<5,1);
	//On screen
	if(SpecCmd==8)CmdCreateGoodKindSelection(MyNation,mapx<<5,mapy<<5,(mapx+smaplx-1)<<5,(mapy+smaply-1)<<5,1);
	//Select all buildings
	if(SpecCmd==9)CmdCreateGoodKindSelection(MyNation,0,0,msx<<5,msy<<5,4);
	//On screen
	if(SpecCmd==10)CmdCreateGoodKindSelection(MyNation,mapx<<5,mapy<<5,(mapx+smaplx-1)<<5,(mapy+smaply-1)<<5,4);
	//Select all dragons
	if(SpecCmd==13)CmdCreateGoodKindSelection(MyNation,0,0,msx<<5,msy<<5,5);
	//On screen
	if(SpecCmd==14)CmdCreateGoodKindSelection(MyNation,mapx<<5,mapy<<5,(mapx+smaplx-1)<<5,(mapy+smaply-1)<<5,5);
	if(SpecCmd==200)CmdDie(MyNation);
	if(SpecCmd==201)CmdSitDown(MyNation);
	if(SpecCmd==11){
		word NSel=NSL[MyNation];
		if(NSel){
			word MID=Selm[MyNation][0];
			if(MID!=0xFFFF){
				OneObject* OB=Group[MID];
				if(OB)CmdCreateGoodTypeSelection(MyNation,0,0,msx<<5,msy<<5,OB->NIndex);
			};
		};
	};
	if(SpecCmd==12){
		word NSel=NSL[MyNation];
		if(NSel){
			word MID=Selm[MyNation][0];
			if(MID!=0xFFFF){
				OneObject* OB=Group[MID];
				if(OB)CmdCreateGoodTypeSelection(MyNation,mapx<<5,mapy<<5,(mapx+smaplx-1)<<5,(mapy+smaply-1)<<5,OB->NIndex);
			};
		};
	};
	SpecCmd=0;
	byte MyMask=NATIONS[MyNation].NMask;
	ControlZones();
	WallHandleMouse(x,y);
	if(BuildWall)goto Edgetest;
	int xxx;
	int yyy;
	if(cooren){
		xxx=xmx>>5;
		yyy=ymy>>5;

		if(Lpressed&&!mini){
			if(ChoosePosition){
				mapPos[(MyNation<<1)]=xxx;
				mapPos[(MyNation<<1)+1]=yyy;
				ChoosePosition=false;
			};
			if(HeightEditMode){
				CreateBlob(xxx-1,yyy-1,4*BlobMode,ReliefBrush);
				goto Edgetest;
			};
			if(MEditMode){
				PutPoint(xxx-1,yyy-1,TerrBrush,EditMedia);
				goto Edgetest;
			};
			if(Creator<200){
				//for(int i=0;i<2;i++)for(int j=0;j<2;j++)
				CmdCreateTerrain(MyNation,xxx,yyy,Creator);
				MFix();
				return;
			};
			if(Creator==254){
				OILMAP[yyy>>1][xxx>>1]=60000;
			};
			if(BuildMode){
				if(CheckGold(xxx,yyy,BNat->Mon[BuildingID])&&ManualFogCheck(xxx,yyy,NATIONS[MyNation].Mon[BuildingID]->SizeX))
					CmdCreateBuilding(MyNation,xxx,yyy,BuildingID);
				if(!(GetKeyState(VK_SHIFT)&0x8000))BuildMode=false;
				Lpressed=false;
				MFix();
				return;
			};
		};
	};
	if(cooren){
		int gx=xmx;
		int gy=ymy;
		int gx1=StartX;
		int gy1=StartY;
		if(gx>gx1){int z=gx;gx=gx1;gx1=z;};
		if(gy>gy1){int z=gy;gy=gy1;gy1=z;};
		int xxx=xmx>>5;
		int yyy=ymy>>5;
		int xxx1=StartX>>5;
		int yyy1=StartY>>5;
		if(xxx1<xxx){int z=xxx;xxx=xxx1;xxx1=z;};
		if(yyy1<yyy){int z=yyy;yyy=yyy1;yyy1=z;};
		if(Lpressed&&!mini){
			if(!SelSoundDone){
				SelSoundDone=true;
				SelSoundReady=true;
				SelSoundType=0;
			};
			SetRLCWindow(smapx,smapy,smaplx<<Shifter,smaply<<Shifter,SCRSizeX);
			Xbar(smapx+((StartX-(mapx<<5))>>(5-Shifter)),smapy+((StartY-(mapy<<5))>>(5-Shifter)),
				x-(smapx+((StartX-(mapx<<5))>>(5-Shifter))),
				y-(smapy+((StartY-(mapy<<5))>>(5-Shifter))),254);
			int nn=0;
			CmdCreateGoodSelection(MyNation,gx,gy,gx1,gy1);
		} else{
			if(!mini){
				StartX=xmx;
				StartY=ymy;
				SelSoundReady=false;
				SelSoundDone=false;
			};
		};
		if(Rpressed&&NSL[MyNation]>0&&!mini){
			DoSelSound(1);
			Rpressed=false;
			if(GetKeyState(VK_CONTROL)&0x8000){
				CmdSetDst(MyNation,xxx,yyy);
				return;
			};
			byte rk=GetResourceKind(xxx,yyy);
			if(rk!=0){
				CmdTakeRes(MyNation,xxx,yyy,rk);
				CmdSendToXY(MyNation,xxx,yyy);
				goto Edgetest;
			};
			word LNK=Links[yyy][xxx];
			if(LNK!=0xFFFF){
				WallCluster* WC=&GWALLS.GWC[LNK];
				if(WC->OwnerID==MyNation)
					CmdRepairWall(MyNation,LNK);
				else CmdDamageWall(MyNation,LNK);
			}else{
				word Att=GetEnemy(gx,gy,MyNation);//Mops[yyy][xxx];
				word Fri=GetFriend(gx,gy,MyNation);
				if(Fri!=0xFFFF){
					OneObject* FOBJ=Group[Fri];
					if(FOBJ->Transport){
						CmdSendToTransport(MyNation,Fri);
						return;
					};
					if(FOBJ->capTeleport&&FOBJ->Ready){
						CmdSendToPoint(MyNation,FOBJ->x+1,FOBJ->y+2);
						return;
					};
					if((FOBJ->NMask&MyMask)&&FOBJ->Ref.General->OnOil&&FOBJ->Stage==FOBJ->NStages)
						CmdGetOil(MyNation,Fri);
					else if(FOBJ->capBuilding&&FOBJ->Life<FOBJ->MaxLife)
						CmdBuildObj(MyNation,Fri);
					else CmdSendToXY(MyNation,xxx,yyy);
				};
				if(Att!=0xFFFF){
					OneObject* AOBJ=Group[Att];
					if(int(AOBJ)){
						if(AOBJ->capTeleport&&AOBJ->Ready){
							CmdSendToPoint(MyNation,AOBJ->x+1,AOBJ->y+2);
							return;
						};
						if(!(AOBJ->NMask&MyMask))
							CmdAttackObj(MyNation,Att);
						else CmdSendToXY(MyNation,xxx,yyy);
					}else CmdSendToXY(MyNation,xxx,yyy);
				}else CmdSendToXY(MyNation,xxx,yyy);
			};
		}else{
			if(Rpressed&&NSL[MyNation]>0)CmdSendToXY(MyNation,xmx>>5,ymy>>5);
		};
	};
Edgetest:
	if ((x<6)&&(mapx-step>0)) mapx-=step;
	if ((y<6)&&(mapy-step>0)) mapy-=step;
	if ((x>RealLx-29)&&(mapx+smaplx+step<msx)) mapx+=step;
	if ((y>RealLy-6)&&(mapy+smaply+step<msy)) 
		mapy+=step;
	if((Lpressed)&&(mouseX>minix)&&(mouseY>miniy)&&
		(mouseX<minix+(msx>>1))&&(mouseY<miniy+(msy>>1))){
		mapx=(x-minix-(smaplx>>2))<<1;
		mapy=(y-miniy-(smaply>>2))<<1;
		if(mapx<=0)mapx=1;
		if(mapy<=0)mapy=1;
		if(mapx+smaplx>msx+1)mapx=msx-smaplx+1;
		if(mapy+smaply>msy+1)mapy=msy-smaply+1;
	}
	MFix();
};
void doooo(){
	Group[0]->Selected=true;
	SlMons[0]=0;
	Nsel=1;
	mapx=20;
	mapy=20;
	Group[0]->SendTo(32,32,0);
};
void ReRenderMap();
void SetMiniMode(){
	if(MiniMode)return;
	ReRenderMap();
	mapx=mapx-(smaplx>>1);
	mapy=mapy-(smaply>>1);
	if(mapx<1)mapx=1;
	if(mapy<1)mapy=1;
	smaplx=smaplx<<1;
	smaply=smaply<<1;
	MiniMode=true;
	Shifter=4;
	Multip=1;
	if(mapx+smaplx>msx)mapx=msx-smaplx;
	if(mapy+smaply>msy)mapy=msy-smaply;
};
void ClearMiniMode(){
	MiniMode=false;
	ReRenderMap();
	Shifter=5;
	Multip=0;
	mapx+=(smaplx>>2);
	mapy+=(smaply>>2);
	smaplx=(RSCRSizeX-160)>>5;
	smaply=RSCRSizeY>>5;
};

