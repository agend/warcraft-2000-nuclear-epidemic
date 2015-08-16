//walls discription and building
#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mgraph.h"
#include "mouse.h"
#include "menu.h"
#include "MapDiscr.h"
#include "multipl.h"
#include "mode.h"
#include "fog.h"
#include "walls.h"
#include "3DSurf.h"
#include <assert.h>
const int drrf[9]={7,6,5,0,0,4,1,2,3};
extern byte tiles[32][8192];
extern bool EditMapMode;
int NDWalls;
int NDWBUF[8];
word Links[256][256];
word LIndex[256][256];
WallDiscr Walls[2];
//GLOBAL WALLS SYSTEM
GWSys GWALLS;
void LoadWalls(){
	char gg[128];
	FILE* f=fopen("info.dat","r");
	for(int i=0;i<2;i++){
		fscanf(f,"%s",gg);
		for(int j=0;j<3;j++)
			for(int k=0;k<16;k++)
				fscanf(f,"%d",&Walls[i].All[j*16+k]);
	};
	fscanf(f,"%s",gg);
	for(i=0;i<8;i++)fscanf(f,"%d",&NDWBUF[i]);
	fclose(f);
	memset(Links,255,sizeof Links);
	memset(LIndex,255,sizeof LIndex);
};
WallCluster::WallCluster(){
	ClusterSize=0;
	Cells=NULL;
	OwnerID=0;
	Type=0;
};
int WallCluster::CheckPoint(byte x,byte y){
	if(Cells==NULL)return -1;
	for(int i=0;i<ClusterSize;i++){
		WallCell* WC=&Cells[i];
		if(x==WC->x&&y==WC->y)return i;
	};
	return -1;
};
void WallCluster::PreArrangeTiles(){
	if(ClusterSize==0)return;
	word* tm=new word[ClusterSize];
	for(int i=0;i<ClusterSize;i++){
		WallCell* WC=&Cells[i];
		tm[i]=Links[WC->y][WC->x];
		Links[WC->y][WC->x]=1956;
	};
	for(i=0;i<ClusterSize;i++){
		WallCell* WC=&Cells[i];
		int dd=0;
		if(Links[WC->y+1][WC->x]==1956)dd=1;
		if(Links[WC->y][WC->x+1]==1956)dd+=2;
		if(Links[WC->y-1][WC->x]==1956)dd+=4;
		if(Links[WC->y][WC->x-1]==1956)dd+=8;
		WC->Tile=Walls[1].Good[dd];
	};
	for(i=0;i<ClusterSize;i++){
		WallCell* WC=&Cells[i];
		Links[WC->y][WC->x]=tm[i];
	};
	free(tm);
};
void WallCluster::ArrangeTiles(){
	if(ClusterSize==0)return;
	word wci=Index;
	for(int i=0;i<ClusterSize;i++){
		WallCell* WC=&Cells[i];
		int dd=0;
		if(Links[WC->y+1][WC->x]==wci)dd=1;
		if(Links[WC->y][WC->x+1]==wci)dd+=2;
		if(Links[WC->y-1][WC->x]==wci)dd+=4;
		if(Links[WC->y][WC->x-1]==wci)dd+=8;
		WC->Tile=Walls[1].All[(WC->Stage<<4)+dd];
		word tt=tmap[WC->x-1][WC->y-1];
		if(WC->Health&&tt!=(char)WC->Tile){
			tmap[WC->x-1][WC->y-1]=(char)WC->Tile;
			ClearRender(WC->x,WC->y);
		};
	};
};

bool ChkWTile(int til){
	if(til!=0&&til!=1&&(til<162||til>176)&&(til<178||til>192))return false;
	else return true;
}
void WallCluster::AddWall(byte x,byte y){
	int til=tmap[x-1][y-1];
	if(TrMap[y][x])return;
	if(!ChkWTile(til))return;
	int zz=CheckPoint(x,y);
	if(zz!=-1){
		ClusterSize=zz+1;
		PreArrangeTiles();
		return;
	};
	WallCell* WCL=new WallCell[ClusterSize+1];
	if(int(Cells)){
		WallCell* WOL=Cells;
		memcpy(WCL,WOL,ClusterSize*sizeof WallCell);
		free(Cells);
	};
	Cells=WCL;
	WCL=&Cells[ClusterSize];
	ClusterSize++;
	WCL->x=x;
	WCL->y=y;
	WCL->Stage=2;
	WCL->Health=1;
	PreArrangeTiles();
};
extern byte mtiles[16][4096];
void WallCluster::ShowCluster(){
	if(MiniMode){
		for(int i=0;i<ClusterSize;i++){
			WallCell* WC=&Cells[i];
			byte x=WC->x;
			byte y=WC->y;
			if(x>=mapx&&y>=mapy&& x<mapx+smaplx&&y<mapy+smaply){
				int tofs=int(mtiles)+256*WC->Tile;
				int	sofs=int(ScreenPtr)+smapx+(int(x-mapx)<<4)+
					((int(y-mapy)<<4)+smapy)*SCRSizeX;
				int	Addo=SCRSizeX-16;
				__asm{
					cld
					mov		esi,tofs
					mov		edi,sofs
					mov		ecx,16
iii:				movsd
					movsd	
					movsd
					movsd
					add		edi,Addo
					loop	iii
				};
			};
		};
		return;
	};
	for(int i=0;i<ClusterSize;i++){
		WallCell* WC=&Cells[i];
		byte x=WC->x;
		byte y=WC->y;
		if(x>=mapx&&y>=mapy&& x<mapx+smaplx&&y<mapy+smaply){
			int tofs=int(tiles)+1024*WC->Tile;
			int	sofs=int(ScreenPtr)+smapx+(int(x-mapx)<<5)+
				((int(y-mapy)<<5)+smapy)*SCRSizeX;
			int	Addo=SCRSizeX-32;
			__asm{
				cld
				mov		esi,tofs
				mov		edi,sofs
				mov		ecx,32
ziii:			movsd
				movsd	
				movsd
				movsd
				movsd
				movsd
				movsd
				movsd
				add		edi,Addo
				loop	ziii
			};
		};
	};
};
void WallCluster::Stand(){

};
int CheckNeed(byte x,byte y,word LNK1){
	word LNK=Links[y][x];
	if(LNK==LNK1&&LNK!=0xFFFF){
		WallCluster* WC=&GWALLS.GWC[LNK];
		WallCell* OC=&WC->Cells[LIndex[y][x]];
		if(OC->Health<OC->MaxHealth)return LIndex[y][x];
	};
	return -1;
};
int CheckDNeed(byte x,byte y,word LNK1){
	word LNK=Links[y][x];
	if(LNK==LNK1&&LNK!=0xFFFF){
		WallCluster* WC=&GWALLS.GWC[LNK];
		WallCell* OC=&WC->Cells[LIndex[y][x]];
		return LIndex[y][x];
	};
	return -1;
};
int WallCluster::FindWorkPoint(byte x,byte y,word ID){
	int xx=x;
	int	yy=y;
	int dis=1000;
	int lp=-1;
	WallCell* WW;
	int rr=CheckNeed(xx+1,yy,Index);
	if(rr!=-1)return rr;
	rr=CheckNeed(xx-1,yy,Index);
	if(rr!=-1)return rr;
	rr=CheckNeed(xx,yy+1,Index);
	if(rr!=-1)return rr;
	rr=CheckNeed(xx+1,yy,Index);
	if(rr!=-1)return rr;
	rr=CheckNeed(xx+1,yy+1,Index);
	if(rr!=-1)return rr;
	rr=CheckNeed(xx+1,yy-1,Index);
	if(rr!=-1)return rr;
	rr=CheckNeed(xx-1,yy-1,Index);
	if(rr!=-1)return rr;
	rr=CheckNeed(xx-1,yy+1,Index);
	if(rr!=-1)return rr;
	for(int i=0;i<ClusterSize;i++){
		WallCell* WC=&Cells[i];
		if(WC->Stage!=32&&WC->WorkNeed>0&&(WC->WorkTime==0||WC->WorkerID==ID)){
			int dis1=abs(xx-WC->x)+abs(yy-WC->y)/*+((WC->MaxHealth-WC->Health)>>7)*/;
			if(dis1<dis){
				lp=i;
				dis=dis1;
				WW=WC;
			};
		};
	};
	if(lp==-1){
		for(int i=0;i<ClusterSize;i++){
			WallCell* WC=&Cells[i];
			if(WC->Stage!=32&&WC->WorkNeed>0){
				int dis1=abs(xx-WC->x)+abs(yy-WC->y)/*+((WC->MaxHealth-WC->Health)>>7)*/;
				if(dis1<dis){
					lp=i;
					dis=dis1;
					WW=WC;
				};
			};
		};
	};
	if(lp!=-1){
		WW->WorkerID=ID;
		WW->WorkTime+=5;
	};
	return lp;
};
int WallCluster::FindDamagePoint(byte x,byte y,word ID){
	int xx=x;
	int	yy=y;
	int dis=1000;
	int lp=-1;
	WallCell* WW;
	int rr=CheckDNeed(xx+1,yy,Index);
	if(rr!=-1)return rr;
	rr=CheckDNeed(xx-1,yy,Index);
	if(rr!=-1)return rr;
	rr=CheckDNeed(xx,yy+1,Index);
	if(rr!=-1)return rr;
	rr=CheckDNeed(xx+1,yy,Index);
	if(rr!=-1)return rr;
	rr=CheckDNeed(xx+1,yy+1,Index);
	if(rr!=-1)return rr;
	rr=CheckDNeed(xx+1,yy-1,Index);
	if(rr!=-1)return rr;
	rr=CheckDNeed(xx-1,yy-1,Index);
	if(rr!=-1)return rr;
	rr=CheckDNeed(xx-1,yy+1,Index);
	if(rr!=-1)return rr;
	for(int i=0;i<ClusterSize;i++){
		WallCell* WC=&Cells[i];
		if(WC->Stage!=32&&WC->DamNeed>0&&(WC->WorkTime==0||WC->WorkerID==ID)){
			int dis1=abs(xx-WC->x)+abs(yy-WC->y);
			if(dis1<dis){
				lp=i;
				dis=dis1;
				WW=WC;
			};
		};
	};
	if(lp==-1){
		for(int i=0;i<ClusterSize;i++){
			WallCell* WC=&Cells[i];
			if(WC->Stage!=32&&WC->DamNeed>0){
				int dis1=abs(xx-WC->x)+abs(yy-WC->y);
				if(dis1<dis){
					lp=i;
					dis=dis1;
					WW=WC;
				};
			};
		};
	};
	if(lp!=-1){
		WW->WorkerID=ID;
		WW->WorkTime+=5;
	};
	return lp;
};
void WallCluster::Init(){
	if(Cells)free(Cells);
	Cells=NULL;
	ClusterSize=0;
};
int WallCluster::GetDataSize(){
	return 4+2*ClusterSize;
};
//data format: [kind][owner][count:16][x1][y1][x2][y2]...
void WallCluster::CreateData(byte NI,byte* lpData){
	 byte* db=lpData;
	 word* dw=(word*) lpData;
	 db[0]=Type;
	 db[1]=NI;
	 dw[1]=word(ClusterSize);
	 db+=4;
	 for(int i=0;i<ClusterSize;i++){
		 WallCell* WC=&Cells[i];
		 db[0]=WC->x;
		 db[1]=WC->y;
		 db+=2;
	 };
};
//Sample of cluster
WallCluster TMPCluster;
bool BuildWall;
bool WStarted;
void SetBuildWallMode(){
	BuildWall=true;
	WStarted=false;
	TMPCluster.Init();
};
bool ManualFogCheck(int x,int y,int dx);
void WallHandleMouse(int x,int y){
	int SHF=5;
	if(MiniMode)SHF=4;
	if(x<smapx||y<smapy||x>=smapx+(smaplx<<SHF)||y>=smapy+(smaply<<SHF))return;
	if(BuildWall){
		if(Lpressed&&!WStarted)WStarted=true;
		if(WStarted){
			int xxx=mapx+((x-smapx)>>SHF);
			int yyy=mapy+((y-smapy)>>SHF);
			if(EditMapMode)TMPCluster.AddWall(xxx,yyy);
			else if(fmap[yyy][xxx]>2000)
				TMPCluster.AddWall(xxx,yyy);
		};
		if(WStarted&&!Lpressed){
			BuildWall=false;
			CmdBuildWall(MyNation);
		};
	};
};

GWSys::GWSys(){
	MaxWall=1024;
	GWC=new WallCluster[MaxWall];
	memset(GWC,0,MaxWall*sizeof WallCluster);
};
//data format: [kind][owner][count:16][x1][y1][x2][y2]...
word GWSys::AddCluster(byte* Data){
	for(int i=0;i<MaxWall&&GWC[i].ClusterSize;i++);
	if(i>=MaxWall){
		WallCluster* OWC=GWC;
		GWC=new WallCluster[MaxWall+1024];
		memcpy(GWC,OWC,MaxWall*sizeof WallCluster);
		memset(&GWC[MaxWall],0,1024*sizeof WallCluster);
		i=MaxWall;
		MaxWall+=1024;
		free(OWC);
	};
	WallCluster* WC=&GWC[i];
	word CLIN=i;
	byte* db=Data;
	word* dw=(word*)Data;
	WC->Type=db[0];
	WC->OwnerID=db[1];
	WC->ClusterSize=int(dw[1]);
	WC->Index=CLIN;
	WC->Cells=new WallCell[WC->ClusterSize];
	WallCell* OC=WC->Cells;
	db+=4;
	for(i=0;i<WC->ClusterSize;i++){
		OC->x=db[0];
		OC->y=db[1];
		OC->Stage=2;
		OC->Health=1;
		OC->MaxHealth=16000;
		OC->WorkTime=0;
		OC->WorkerID=0xFFFF;
		OC->BuildProgress=1;
		Links[OC->y][OC->x]=CLIN;
		LIndex[OC->y][OC->x]=i;
		if(Mops[OC->y][OC->x]==0xFFFF){
			LLock[OC->y][OC->x]=1;
			IncLock(OC->x,OC->y);
			TrMap[OC->y][OC->x]=1;
		};
		OC++;
		db+=2;
	};
	OC=WC->Cells;
	WC->ArrangeTiles();
	for(i=0;i<WC->ClusterSize;i++){
		if(OC->Health)tmap[OC->x-1][OC->y-1]=(char)OC->Tile;
		ClearRender(OC->x,OC->y);
		OC++;
	};
	return CLIN;
};
void GWSys::ProcessClusters(){
	WallCluster* WC=GWC;
	bool TTF,NNT;
	for(int i=0;i<MaxWall;i++){
		if(WC->OwnerID==MyNation)NNT=true;else NNT=false;
		if(WC->ClusterSize!=0){
			WallCell* OC=WC->Cells;
			TTF=false;
			int j=WC->ClusterSize;
			for(int k=0;k<j;k++){
				if(NNT)fmap[OC->y][OC->x]=10000;
				word MID=Mops[OC->y][OC->x];
				if(MID!=0xFFFF){
					OneObject* OB=Group[MID];
					if(OB){
						OB->MoveFrom(0);
					};
				}else{
					if(OC->Health){
						LLock[OC->y][OC->x]=1;
						IncLock(OC->x,OC->y);
						TrMap[OC->y][OC->x]=1;
					};
				};
				if(OC->Health>=OC->MaxHealth)OC->WorkNeed=0;
				{
					byte xx=OC->x;
					byte yy=OC->y;
					byte wn=0;
					_asm{
						xor		eax,eax
						xor		ebx,ebx
						mov		bl,xx
						mov		bh,yy
						cmp		byte ptr [LLock+ebx-256],0
						jnz		rtr1
						inc		eax
rtr1:					cmp		byte ptr [LLock+ebx-255],0
						jnz		rtr2
						inc		eax
rtr2:					cmp		byte ptr [LLock+ebx+1],0
						jnz		rtr3
						inc		eax
rtr3:					cmp		byte ptr [LLock+ebx+257],0
						jnz		rtr4
						inc		eax
rtr4:					cmp		byte ptr [LLock+ebx+256],0
						jnz		rtr5
						inc		eax
rtr5:					cmp		byte ptr [LLock+ebx+255],0
						jnz		rtr6
						inc		eax
rtr6:					cmp		byte ptr [LLock+ebx-1],0
						jnz		rtr7
						inc		eax
rtr7:					cmp		byte ptr [LLock+ebx-257],0
						jnz		rtr8
						inc		eax
rtr8:					mov		wn,al
					};
					if(OC->Health>=OC->MaxHealth)OC->WorkNeed=0;
					else OC->WorkNeed=wn;
					OC->DamNeed=wn;
					if(OC->WorkTime>0)OC->WorkTime--;
					if(OC->WorkTime==0)OC->WorkerID=0xFFFF;
					if(OC->Health==0&&OC->Stage!=32){
						Links[OC->y][OC->x]=0xFFFF;
						LLock[OC->y][OC->x]=0;
						DecLock(OC->x,OC->y);
						TrMap[OC->y][OC->x]=0;
						OC->Stage=32;
					};
				};
				OC++;
			};
		};
		WC->ArrangeTiles();
		WC++;
	};
};
void GWSys::DamageCell(byte x,byte y,byte Owner,word Incr){
	word LID=Links[y][x];
	if(LID==0xFFFF)return;
	WallCluster* WC=&GWC[LID];
//	if(WC->OwnerID!=Owner)return;
	word CID=LIndex[y][x];
	if(CID>WC->ClusterSize)return;
	WallCell* OC=&WC->Cells[CID];
	OC->BuildProgress+=Incr;
	if(OC->Health>=Incr)OC->Health-=Incr;else OC->Health=0;
	if(OC->Stage==0&&OC->Health<=(OC->MaxHealth>>1))OC->Stage=1;
	int dd=0;
	//if(Links[y+1][x]!=0xFFFF)dd+=1;
	//if(Links[y][x+1]!=0xFFFF)dd+=2;
	//if(Links[y-1][x]!=0xFFFF)dd+=4;
	//if(Links[y][x-1]!=0xFFFF)dd+=8;
	if(OC->Health==0){
		OC->Tile=rando()&1;//NDWBUF[rando()&7];
		tmap[x-1][y-1]=(byte)OC->Tile;
		ClearRender(x,y);
		//TrMap[y][x]=0;
		//LLock[y][x]=0;
	};
};
void GWSys::BuildCell(byte x,byte y,byte Owner,byte xIncr,word WorkerID){
	word Incr=xIncr;
	if(EditMapMode)Incr=8000;else Incr=xIncr<<4;
	word LID=Links[y][x];
	if(LID==0xFFFF)return;
	WallCluster* WC=&GWC[LID];
	if(WC->OwnerID!=Owner)return;
	word CID=LIndex[y][x];
	if(CID>WC->ClusterSize)return;
	WallCell* OC=&WC->Cells[CID];
	OC->BuildProgress+=Incr;
	OC->WorkerID=WorkerID;
	OC->WorkTime=20;
	if(OC->Health+Incr<OC->MaxHealth)OC->Health+=Incr;
	else OC->Health=OC->MaxHealth;
	if(OC->Stage==1&&OC->Health>=OC->MaxHealth)OC->Stage=0;
	if(OC->Stage==2&&OC->Health>=(OC->MaxHealth>>1))OC->Stage=1;
	int dd=0;
	if(Links[y+1][x]!=0xFFFF)dd+=1;
	if(Links[y][x+1]!=0xFFFF)dd+=2;
	if(Links[y-1][x]!=0xFFFF)dd+=4;
	if(Links[y][x-1]!=0xFFFF)dd+=8;
	OC->Tile=Walls[WC->Type].All[(OC->Stage<<4)+dd];
	if(OC->Health)tmap[x-1][y-1]=(byte)OC->Tile;
};
void BuildWallLink(OneObject* OBJ);
bool OneObject::BuildWall(word OID,int Prio){
	if(Prio<PrioryLevel)return false;
	if(!capBuild)return false;
	if(capBuilding)return false;
	Important=true;
	Order1* Or1=GetOrdBlock();
	if(!int(Or1))return false;
	//Or1->PrioryLevel=Prio&127;
	Or1->NextOrder=LocalOrder;
	Or1->OrderType=227;//Build wall
	Or1->OrderTime=0;
	Or1->DoLink=&BuildWallLink;
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
void FindEmptyPoint(byte* x,byte* y){
	byte xx=*x;
	byte yy=*y;
	__asm{
		xor		eax,eax
		mov		al,xx
		mov		ah,yy
		add		eax,offset LLock
		cmp		byte ptr [eax-256],0
		jne		dr_1
		dec		yy
		jmp		dr_end
dr_1:	
		cmp		byte ptr [eax+1],0
		jne		dr_2
		inc		xx
		jmp		dr_end
dr_2:
		cmp		byte ptr [eax+256],0
		jne		dr_3
		inc		yy
		jmp		dr_end
dr_3:
		cmp		byte ptr [eax-1],0
		jne		dr_4
		dec		xx
		jmp		dr_end
dr_4:
		cmp		byte ptr [eax-255],0
		jne		dr_5
		inc		xx
		dec		yy
		jmp		dr_end
dr_5:
		cmp		byte ptr [eax+257],0
		jne		dr_6
		inc		yy
		inc		xx
		jmp		dr_end
dr_6:
		cmp		byte ptr [eax+255],0
		jne		dr_7
		dec		xx
		inc		yy
		jmp		dr_end
dr_7:
		cmp		byte ptr [eax-257],0
		jne		dr_8
		dec		yy
		dec		xx
		jmp		dr_end
dr_8:	mov		xx,0
		mov		yy,0;
dr_end:
	};
	*x=xx;
	*y=yy;
};
void BuildWallLink(OneObject* OBJ){
	word OID=OBJ->LocalOrder->info.MoveToObj.ObjIndex;
	WallCluster* WC=&GWALLS.GWC[OID];
	int wi=WC->FindWorkPoint(OBJ->x,OBJ->y,OBJ->Index);
	if(wi==-1)return;
	OBJ->Important=true;
	Order1* Or1;
	TrMap[OBJ->y][OBJ->x]=0;
	if(wi==-1){
		if(int(OBJ->InLineCom))OBJ->FreeAsmLink();
		if(int(OBJ->LocalOrder)){
			Or1=OBJ->LocalOrder->NextOrder;
			OBJ->FreeOrdBlock(OBJ->LocalOrder);
			OBJ->LocalOrder=Or1;
			OBJ->Important=false;
			return;
		};
	};
	WallCell* OC=&WC->Cells[wi];
	int x0=OC->x;
	int y0=OC->y;
	int sx=x0-OBJ->x;
	int	sy=y0-OBJ->y;
	int dx=abs(sx);
	int dy=abs(sy);
	int dst;
	if(dx>dy)dst=dx;
			else dst=dy;
	if(dst<2){
		OBJ->FreeAsmLink();
		OBJ->Direction=drrf[(sx+1)*3+sy+1];
		OBJ->LoadAnimation(2,2,0);
		OBJ->LoadCurAnm(2);
		GWALLS.BuildCell(x0,y0,OBJ->NNUM,250,OBJ->Index);
		OBJ->isMoving=false;
		TrMap[OBJ->y][OBJ->x]=1;
	}else if(!int(OBJ->InLineCom)){
		byte xx=byte(x0);
		byte yy=byte(y0);
		FindEmptyPoint(&xx,&yy);
		if(xx>0)OBJ->CreateSimplePath(xx,yy);
	};
};
bool SearchWall(int* x,int* y){
	int xx=*x;
	int yy=*y;
	int x1,y1;
	for(int i=0;i<8;i++){
		x1=xx+(rando()&31)-16;
		y1=yy+(rando()&31)-16;
		if(x1>0&&y1>0&&x1<msx&&y1<msy){
			if(Links[y1][x1]!=0xFFFF){
				*x=x1;
				*y=y1;
				return true;
			};
		};
	};
	return false;
};
void HandleDWall(OneObject* OB){
	int xx=OB->x;
	int yy=OB->y;
	if(SearchWall(&xx,&yy)){
		OB->ContinueAttackWall(xx,yy,128);
	};
};
void DamageWallLink(OneObject* OBJ);
bool OneObject::DamageWall(word OID,int Prio){
	if(Prio<PrioryLevel)return false;
	if(Ref.Visual->info.Basic.AttackRange>1)return false;
	if(capBuilding)return false;
	Important=true;
	Order1* Or1=GetOrdBlock();
	if(!int(Or1))return false;
	Or1->PrioryLevel=Prio&127;
	Or1->NextOrder=LocalOrder;
	Or1->OrderType=228;//destroy wall
	Or1->OrderTime=0;
	Or1->DoLink=&DamageWallLink;
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
void DamageWallLink(OneObject* OBJ){
	word OID=OBJ->LocalOrder->info.MoveToObj.ObjIndex;
	WallCluster* WC=&GWALLS.GWC[OID];
	int wi=WC->FindDamagePoint(OBJ->x,OBJ->y,OBJ->Index);
	if(wi==-1)return;
	OBJ->Important=true;
	Order1* Or1;
	TrMap[OBJ->y][OBJ->x]=0;
	if(wi==-1){
		if(int(OBJ->InLineCom))OBJ->FreeAsmLink();
		if(int(OBJ->LocalOrder)){
			Or1=OBJ->LocalOrder->NextOrder;
			OBJ->FreeOrdBlock(OBJ->LocalOrder);
			OBJ->LocalOrder=Or1;
			OBJ->Important=false;
			return;
		};
	};
	WallCell* OC=&WC->Cells[wi];
	int x0=OC->x;
	int y0=OC->y;
	int sx=x0-OBJ->x;
	int	sy=y0-OBJ->y;
	int dx=abs(sx);
	int dy=abs(sy);
	int dst;
	if(dx>dy)dst=dx;
			else dst=dy;
	if(dst<2){
		OBJ->FreeAsmLink();
		OBJ->Direction=drrf[(sx+1)*3+sy+1];
		OBJ->LoadAnimation(2,2,0);
		OBJ->LoadCurAnm(2);
		GWALLS.DamageCell(x0,y0,OBJ->NNUM,250);
		OBJ->isMoving=false;
		TrMap[OBJ->y][OBJ->x]=1;
	}else if(!int(OBJ->InLineCom)){
		byte xx=byte(x0);
		byte yy=byte(y0);
		FindEmptyPoint(&xx,&yy);
		if(xx>0)OBJ->CreateSimplePath(xx,yy);
	};
};