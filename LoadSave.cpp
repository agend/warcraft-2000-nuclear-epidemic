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
#include "FlyObj.h"
#include <assert.h>
#include "MapEdit.h"
#include "3DSurf.h"
#include "Strange.h"
extern word NucList[128];
extern word NucSN[128];
extern bool NDone[128];
extern word NNuc;
char GameName[128];
extern City CITY[8];
extern bool EUsage[8192];
extern AnmObject* GAnm[8192];
void LoadNation(char* fn,byte msk,byte NIndex);
extern word FlyMops[256][256];
extern byte Locking[1024];//Инф. о блокировании поверхности
extern int RealLx;
extern int RealLy;
extern int Pitch;
extern bool MiniMade;
void SetSumms();
void SAVMES(ResFile ff1,char* mes){
	char mss[16];
	memset(mss,0,16);
	int len=strlen(mes);
	if(len>10)len=16;
	memcpy(mss,mes,len);
	RBlockWrite(ff1,mss,16);
};
void SAVMES1(ResFile ff1,char* mes,int parm){
	char mss[16];
	char xxx[28];
	sprintf(xxx,"%s %d",mes,parm);
	memset(mss,0,16);
	int len=strlen(xxx);
	if(len>16)len=16;
	memcpy(mss,xxx,len);
	RBlockWrite(ff1,mss,16);
};
void LOADMES(ResFile ff1){
	char zzz[16];
	RBlockRead(ff1,zzz,16);
};
void LOutErr(LPCSTR s)
{
	MessageBox(hwnd,s,"Loading failed...",MB_ICONWARNING|MB_OK);
};
void SOutErr(LPCSTR s)
{
	MessageBox(hwnd,s,"Saving failed...",MB_ICONWARNING|MB_OK);
};
void SavePackArray(ResFile ff1,word defval,word* dest,int size){
	word szs=0;
	for(int i=0;i<size;i++)if(dest[i]!=defval)szs++;
	RBlockWrite(ff1,&szs,2);
	RBlockWrite(ff1,&defval,2);
	for(i=0;i<size;i++)if(dest[i]!=defval){
		RBlockWrite(ff1,&i,2);
		RBlockWrite(ff1,&dest[i],2);
	};
};
void LoadPackArray(ResFile ff1,word* dest,int size){
	word szs;
	word defv;
	RBlockRead(ff1,&szs,2);
	RBlockRead(ff1,&defv,2);
	for(int i=0;i<size;i++)dest[i]=defv;
	word ofst=0;
	word vall;
	for(i=0;i<szs;i++){
		RBlockRead(ff1,&ofst,2);
		RBlockRead(ff1,&vall,2);
		dest[ofst]=vall;
	};
};
void Nation::CloseNation(){
	for(int i=0;i<NMon;i++){
		if(Mon[i]){
			Mon[i]->CloseGO();
			free(Mon[i]);
		};
		if(PAble[i])free(PAble[i]);
		PACount[i]=0;
		if(CLSize[i])free(CLRef[i]);
	};
	NGidot=0;
	NFarms=0;
	NCOND=0;
	if(NLmenus)free(Lmenus);
	if(NWmenus)free(Wmenus);
	if(NAmenus)free(Amenus);
	if(NCmenus)free(Cmenus);
	if(Animations)free(Animations);
	if(Slides)free(Slides);
	if(Finf)free(Finf);
	for(i=0;i<NIcons;i++){
		WIcon* icc=wIcons[i];
		if(icc->Message)free(icc->Message);
		if(icc->SubList)free(icc->SubList);
		free(wIcons[i]);
	};
	//free upgrades
	for(i=0;i<UPG.NUpgrades;i++){
		if(UPG.utp1[i]->NLinks)free(UPG.utp1[i]->Links);
		if(UPG.utp1[i]->NAuto)free(UPG.utp1[i]->AutoPerf);
		free(UPG.utp1[i]);
	};
};
void GeneralObject::CloseGO(){
	if(Message)free(Message);
	if(NUpgrades)free(Upg);
	if(NIcons)free(IRefs);
};
void OneObject::CloseObject(){
	if(NInside)free(Inside);
	if(PathX)free(PathX);
	if(PathY)free(PathY);
	CPdestX=0;
	CPdestY=0;
	NIPoints=0;
	CurIPoint=0;
	ClearOrders();
	if(InLineCom)FreeAsmLink();

};
void UnLoading(){
	for(int i=0;i<8;i++)NATIONS[i].CloseNation();
	for(i=0;i<MaxObject;i++)if(Group[i]){
		Group[i]->CloseObject();
		Group[i]=NULL;
	};
	//closing multiple groups selection information 
	for(i=0;i<80;i++){
		if(int(SelSet[i].Member))free(SelSet[i].Member);
		if(int(SelSet[i].SerialN))free(SelSet[i].SerialN);
		SelSet[i].NMemb=0;
	};
	for(i=0;i<8;i++){
		if(int(Selm[i]))free(Selm[i]);
		if(int(SerN[i]))free(SerN[i]);
		Selm[i]=NULL;
		SerN[i]=NULL;
		NSL[i]=0;
	};
	//flying objects clearing
	for(i=0;i<64;i++)
		for(int j=0;j<64;j++){
			FlyCell* FC=&FlyMap[i][j];
			FC->NFly=0;
			for(int k=0;k<15;k++)FC->FlyList[k]=0xFFFF;
		};
	//walls clearing
	memset(Links,255,sizeof Links);
	memset(LIndex,255,sizeof LIndex);
	for(i=0;i<GWALLS.MaxWall;i++){
		WallCluster* WC=&GWALLS.GWC[i];
		if(WC->Cells&&WC->ClusterSize)free(WC->Cells);
	};
	free(GWALLS.GWC);
	GWALLS.MaxWall=1024;
	GWALLS.GWC=new WallCluster[GWALLS.MaxWall];
	memset(GWALLS.GWC,0,GWALLS.MaxWall*sizeof(WallCluster));
	//clear flying objects
	memset(EUsage,0,sizeof EUsage);
	//clear magic
	//cells inf clearing
	memset(&TCInf[0][0],0,sizeof(TCInf));
	NNuc=0;
	memset(NucList,0,sizeof NucList);
	memset(NucSN,0,sizeof NucSN);
	memset(NDone,0,sizeof NDone);
	SetSumms();
};
//SAVING...
static int sfHeader='WSF2';
int sfVersion;
void SaveMap(ResFile ff1){
	int i=0x1B46504D;
	RBlockWrite(ff1,&i,4);			//dd Sign
	SAVMES(ff1,"MAPSIZE");
	RBlockWrite(ff1,&msx,4);		//dd msx
	RBlockWrite(ff1,&msy,4);		//dd msy
	i=0;
	RBlockWrite(ff1,&i,4);
	RBlockWrite(ff1,&i,4);
	SAVMES(ff1,"TILEMAP");
	for(i=0;i<msy;i++){
		SAVMES1(ff1,"TMLINE:",i);
		RBlockWrite(ff1,&tmap[i][0],msx);
	};
	SAVMES(ff1,"DEATHMAP");
	SavePackArray(ff1,0xFFFF,&Died[0][0],65536);
};
extern bool BuildMode;
void LoadMap(ResFile ff1)
{
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
	for(int i=0;i<maxmap;i++)
	for(int j=0;j<maxmap;j++){
		//map[i][j].BuildingID=65535;
		Mops[j][i]=65535;
		Died[j][i]=65535;
		FlyMops[j][i]=65535;
		//map[i][j].FlyID=65535;
	};
	RBlockRead(ff1,&i,4);
	if (i!=0x1B46504D) LOutErr("Incorrect map file format.");
	LOADMES(ff1);
	RBlockRead(ff1,&msx,4);
	RBlockRead(ff1,&msy,4);
	RBlockRead(ff1,&i,4);
	RBlockRead(ff1,&i,4);
	LOADMES(ff1);
	for(i=0;i<msy;i++){
		LOADMES(ff1);
		RBlockRead(ff1,&tmap[i][0],msx);
	};
	LOADMES(ff1);
	LoadPackArray(ff1,&Died[0][0],65536);
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
	//RSCRSizeX=RealLx;
	//RSCRSizeY=RealLy;
	//COPYSizeX=RealLx;
	//smaplx=17;
	//if(RSCRSizeY==768)smaplx=22;
	//smaply=smaplx;
	//smapx=COPYSizeX-(smaplx<<5)-32;
	//smapy=(RSCRSizeY-(smaply<<5))>>1;
	//smapx=7*32;
	//smapy=28;
	//smaplx=19;
	//smaply=19;
	//mapx=1;
	//mapy=1;
	//minix=32;
	//miniy=32;
	MiniMade=false;
	//Nsel=0;	
	CreateResMap();
	CreateWaterMap();
	CreateTZones();
	CreateCMap();
	InitRenderMap();
};
void SaveNations(ResFile ff1){
	int tt=0x4954414E;//'NATI'
	RBlockWrite(ff1,&tt,4);
	SAVMES(ff1," NATSAVE");
	byte nn=8;
	RBlockWrite(ff1,&nn,1);
	for(tt=0;tt<8;tt++){
		SAVMES1(ff1," NATION: ",tt);
		Nation* NT=&NATIONS[tt];
		RBlockWrite(ff1,NT->SCRIPT,16);
		RBlockWrite(ff1,&NT->NMask,1);
		RBlockWrite(ff1,&NT->NNUM,1);
		SAVMES1(ff1," MONSAMOUNT :",NT->NMon);
		RBlockWrite(ff1,&NT->NMon,4);
		//Informtion about monsters(GeneralObject)
		for(int i=0;i<NT->NMon;i++){
			GeneralObject* GO=NT->Mon[i];
			Visuals* VS=(Visuals*)GO;
			SAVMES1(ff1," NMONSTER:",i);
			nn=GO->NoSearchVictim;
			if(GO->NoAnswer)nn=nn|2;
			if(GO->NeedNoHelp)nn=nn|4;
			if(GO->Enabled)nn=nn|8;
			RBlockWrite(ff1,&nn,1);
			RBlockWrite(ff1,&VS->info.Basic.MaxShield,2);
			RBlockWrite(ff1,&VS->info.Basic.MinDamage,2);
			RBlockWrite(ff1,&VS->info.Basic.MaxDamage,2);
			RBlockWrite(ff1,&VS->info.Basic.MaxLife,2);
			RBlockWrite(ff1,&VS->info.Basic.AttackRange,2);
		};
		//Information about city
		SAVMES(ff1,"CITYINFO");
		City* CT=NT->CITY;
		RBlockWrite(ff1,&CT->InX,34);
		nn=CT->PresentProject;
		if(CT->IntellectEnabled)nn|=2;
		RBlockWrite(ff1,&nn,1);
		if(CT->IntellectEnabled){
			//Save AI properties if intellect is enabled
			//1.Information about crowds(10+1 --defence)
			for(i=0;i<11;i++){
				Crowd* CR=&CT->CROWDS[i];
				RBlockWrite(ff1,&CR->Amount,2);
				RBlockWrite(ff1,&CR->NI,1);
				RBlockWrite(ff1,&CR->MLSize,2);
				RBlockWrite(ff1,CR->MemList,CR->MLSize<<1);
				RBlockWrite(ff1,&CR->LastDx,23*4);
				RBlockWrite(ff1,CR->ForAtt,32*2);
			};
			//2.Enemy list
			RBlockWrite(ff1,CT->EnemyList,32*2);
			//End AI Statements
		};
		SAVMES(ff1,"UPGRADEINFO");
		RBlockWrite(ff1,&NT->NGidot,4);
		RBlockWrite(ff1,&NT->NFarms,4);
		//Upgrades information
		Upgrade* UP=&NT->UPG;
		RBlockWrite(ff1,&UP->NUpgrades,4);
		for(i=0;i<UP->NUpgrades;i++){
			nn=UP->utp1[i]->Enabled;
			RBlockWrite(ff1,&nn,1);
			RBlockWrite(ff1,&UP->utp1[i]->Stage,2);
		};
		//end of upgrade information
		//General AI statements
		SAVMES(ff1,"GENERALAI");
		RBlockWrite(ff1,&NT->CasheSize,4);
		RBlockWrite(ff1,&NT->NationalAI,4);
		RBlockWrite(ff1,&NT->SWP,sizeof NT->SWP);
		//end of nation saving
	};
};
bool LoadNations(ResFile ff1){
	int tt;
	RBlockRead(ff1,&tt,4);
	if(tt!=0x4954414E){//'NATI'0x
		LOutErr("Save file is corrupted.Я не доктор по save- файлам.");
		return false;
	};
	LOADMES(ff1);
	byte nn;
	RBlockRead(ff1,&nn,1);
	for(tt=0;tt<8;tt++){
		LOADMES(ff1);
		char stri[16];
		Nation* NT=&NATIONS[tt];
		RBlockRead(ff1,stri,16);
		byte ms;
		byte ni;
		RBlockRead(ff1,&ms,1);
		RBlockRead(ff1,&ni,1);
		LoadNation(stri,ms,ni);
		CITY[ni].CreateCity(20,20,ni);
		LOADMES(ff1);
		RBlockRead(ff1,&NT->NMon,4);
		//Informtion about monsters(GeneralObject)
		for(int i=0;i<NT->NMon;i++){
			GeneralObject* GO=NT->Mon[i];
			Visuals* VS=(Visuals*)GO;
			LOADMES(ff1);
			RBlockRead(ff1,&nn,1);
			GO->NoSearchVictim=false;
			GO->NoAnswer=false;
			GO->NeedNoHelp=false;
			GO->Enabled=false;
			if(nn&1)GO->NoSearchVictim=true;
			if(nn&2)GO->NoAnswer=true;
			if(nn&4)GO->NeedNoHelp=true;
			if(nn&8)GO->Enabled=true;
			RBlockRead(ff1,&VS->info.Basic.MaxShield,2);
			RBlockRead(ff1,&VS->info.Basic.MinDamage,2);
			RBlockRead(ff1,&VS->info.Basic.MaxDamage,2);
			RBlockRead(ff1,&VS->info.Basic.MaxLife,2);
			RBlockRead(ff1,&VS->info.Basic.AttackRange,2);
		};
		//Information about city
		LOADMES(ff1);
		City* CT=NT->CITY;
		RBlockRead(ff1,&CT->InX,34);
		RBlockRead(ff1,&nn,1);
		CT->PresentProject=false;
		CT->IntellectEnabled=false;
		if(nn&1)CT->PresentProject=true;
		if(nn&2)CT->IntellectEnabled=true;
		if(CT->IntellectEnabled){
			//Load AI properties if intellect is enabled
			//1.Information about crowds(10+1 --defence)
			for(i=0;i<11;i++){
				Crowd* CR=&CT->CROWDS[i];
				RBlockRead(ff1,&CR->Amount,2);
				RBlockRead(ff1,&CR->NI,1);
				RBlockRead(ff1,&CR->MLSize,2);
				CR->MemList=new word[CR->MLSize];
				RBlockRead(ff1,CR->MemList,CR->MLSize<<1);
				RBlockRead(ff1,&CR->LastDx,23*4);
				RBlockRead(ff1,CR->ForAtt,32*2);
			};
			//2.Enemy list
			RBlockRead(ff1,CT->EnemyList,32*2);
			//End AI Statements
		};
		LOADMES(ff1);
		RBlockRead(ff1,&NT->NGidot,4);
		RBlockRead(ff1,&NT->NFarms,4);
		//Upgrades information
		Upgrade* UP=&NT->UPG;
		RBlockRead(ff1,&UP->NUpgrades,4);
		for(i=0;i<UP->NUpgrades;i++){
			RBlockRead(ff1,&nn,1);
			UP->utp1[i]->Enabled=nn;
			RBlockRead(ff1,&UP->utp1[i]->Stage,2);
		};
		//end of upgrade information
		//General AI statements
		LOADMES(ff1);
		RBlockRead(ff1,&NT->CasheSize,4);
		RBlockRead(ff1,&NT->NationalAI,4);
		RBlockRead(ff1,&NT->SWP,sizeof NT->SWP);
		//end of nation loading
	};
	return true;
};
extern void BuildObjLink(OneObject* OB);
extern void ProduceObjLink(OneObject* OB);
extern void FlySendToLink(OneObject* OB);
extern void FlyAttackLink(OneObject* OB);
extern void SendInGroupLink(OneObject* OB);
extern void SendToLink(OneObject* OB);
extern void AttackObjLink(OneObject* OB);
extern void ComplexAttackLink(OneObject* OB);
extern void AttackPointLink(OneObject* OB);
extern void MFLink(OneObject* OB);
extern void PatrolLink(OneObject* OB);
extern void TakeResLink(OneObject* OB);
extern void BuildOnWaterLink(OneObject* OB);
extern void GetOilLink(OneObject* OB);
extern void TeleLink(OneObject* OB);
extern void GoToTransportLink(OneObject* OB);
extern void UnloadPassangersLink(OneObject* OB);
extern void PerformUpgradeLink(OneObject* OB);
extern void WSendToLink(OneObject* OB);
extern void WAttackObjLink(OneObject* OB);
extern void WMFLink(OneObject* OB);
extern void BuildWallLink(OneObject* OB);
extern void DamageWallLink(OneObject* OB);
extern void ContinueAttackPointLink(OneObject* OBJ);
extern void ContinueAttackWallLink(OneObject* OBJ);
extern void UFOLink(OneObject* OB);
extern void NuclearAttackLink(OneObject* OB);
void WaitForRepairLink(OneObject* OBJ);
#define OrdCount 28
ReportFn* ORDLIST[OrdCount]={
	&BuildObjLink,
	&ProduceObjLink,
	&FlySendToLink,
	&FlyAttackLink,
	&SendInGroupLink,
	&SendToLink,
	&AttackObjLink,
	&ComplexAttackLink,
	&AttackPointLink,
	&MFLink,
	&PatrolLink,
	&TakeResLink,
	&BuildOnWaterLink,
	&GetOilLink,
	&TeleLink,
	&GoToTransportLink,
	&UnloadPassangersLink,
	&PerformUpgradeLink,
	&WSendToLink,
	&WAttackObjLink,
	&WMFLink,
	&BuildWallLink,
	&DamageWallLink,
	&ContinueAttackPointLink,
	&ContinueAttackWallLink,
	&UFOLink,
	&NuclearAttackLink,
	&WaitForRepairLink
};
int GetOrderKind(ReportFn* RF){
	for(int i=0;i<OrdCount;i++)if(RF==ORDLIST[i])return i;
	return -1;
};
ReportFn* GetOrderRef(int i){
	if(i==-1||i>=OrdCount)return NULL;
	return ORDLIST[i];
};
void SaveObjects(ResFile ff1){
	int i=0x494A424F;//'OBJI'
	int NObjects=0;
	RBlockWrite(ff1,&i,4);
	SAVMES(ff1," OBJECTSINFO");
	for(i=0;i<MAXOBJECT;i++)if(Group[i])NObjects++;
	RBlockWrite(ff1,&NObjects,4);
	RBlockWrite(ff1,&MAXOBJECT,4);
	//Information for every object
	for(int tt=0;tt<MAXOBJECT;tt++){
		OneObject* OB=Group[tt];
		if(OB){
			SAVMES1(ff1," OBJECT:",OB->Index);
			RBlockWrite(ff1,&OB->Index,int(&OB->Media)-int(&OB->Index)+1);
			int ca=int(OB->CurAnm)-int(OB->Nat->Animations);
			RBlockWrite(ff1,&ca,4);
			RBlockWrite(ff1,&OB->NInside,2);
			if(OB->NInside){
				RBlockWrite(ff1,OB->Inside,OB->NInside<<1);
			};
			if(OB->Media==2){//Air unit only
				RBlockWrite(ff1,&OB->RealX,int(&OB->RealDir)-int(&OB->RealX)+1);
			};
			//saving index of selection group
			int pp;
			if(OB->GroupIndex)pp=div(int(OB->GroupIndex)-int(SelSet),sizeof SelGroup).quot;
			else pp=255;
			RBlockWrite(ff1,&pp,1);
			SAVMES(ff1," ORDERINFO");
			//Order information saving
			//1. calculating orders
			pp=0;
			Order1* ORDR=OB->LocalOrder;
			while(ORDR){
				pp++;
				ORDR=ORDR->NextOrder;
			};
			RBlockWrite(ff1,&pp,2);
			if(pp){
				//2.saving order information
				Order1 ORD1;
				ORDR=OB->LocalOrder;
				ORD1=*OB->LocalOrder;
				while(ORDR){
					ORD1.NextOrder=NULL;
					ORD1.DoLink=(ReportFn*)(GetOrderKind(ORDR->DoLink));
					assert(int(ORD1.DoLink)!=-1);
					RBlockWrite(ff1,&ORD1,sizeof ORD1);
					ORDR=ORDR->NextOrder;
					if(ORDR)ORD1=*ORDR;
				};
			};
			//Priory level
			RBlockWrite(ff1,&OB->PrioryLevel,1);
		};
	};
};
void LoadObjects(ResFile ff1){
	//sigature checking
	int i;
	RBlockRead(ff1,&i,4);
	LOADMES(ff1);
	if(i!=0x494A424F)LOutErr("SAVE file is corrupted. Игры не будет.");//'OBJI'
	int NObjects=0;
	RBlockRead(ff1,&NObjects,4);
	RBlockRead(ff1,&MAXOBJECT,4);
	//Information for every object
	for(int tt=0;tt<NObjects;tt++){
		word Indx;
		LOADMES(ff1);
		RBlockRead(ff1,&Indx,2);
		Group[Indx]=OBJECTS+Indx;//assign pointer
		OneObject* OB=Group[Indx];
		OB->Index=Indx;
		//init path information
		OB->PathX=NULL;
		OB->PathY=NULL;
		OB->NIPoints=0;
		OB->CurIPoint=0;
		OB->CPdestX=0;
		OB->CPdestY=0;
		//Loading header information
		RBlockRead(ff1,&OB->NIndex,int(&OB->Media)-int(&OB->NIndex)+1);
	//Default object settings
		int x=OB->x;
		int y=OB->y;
		//1.Nation reference
		OB->Nat=&NATIONS[OB->NNUM];
		Nation* NT=OB->Nat;
		//2.General object reference
		OB->Ref.General=NT->Mon[OB->NIndex];
		GeneralObject* GO=OB->Ref.General;
		int ca;
		RBlockRead(ff1,&ca,4);
		OB->CurAnm=(Octant*)(int(NT->Animations)+ca);
		//3.Inside objects
		if(OB->Transport)OB->Inside=new word[6];
		RBlockRead(ff1,&OB->NInside,2);
		if(OB->NInside)RBlockRead(ff1,OB->Inside,OB->NInside<<1);
		OB->TimeInside=NULL;
		//4. Other default information
		OB->Wars=NULL;//No Legion information
		OB->InLineCom=NULL;
		OB->LineOffset=0;
		//5.Animation registers
		memset(&(OB->ARegs),0,sizeof OB->ARegs);
		//6.Weapon assignment
		OB->Weap=GO->Weap;
		//7.Land/Water/Air information
		OB->LoadAnimation(0,0,0);
		Cell8x8* CELL=&TCInf[OB->NNUM][y>>2][x>>2];
		CELL->UnitsAmount[GO->Kind]++;
		if(GO->OnWater){
			if(!OB->Sdoxlo){
				Mops[y][x]=Indx;
				WLock[y][x]=true;
			};
			//WMap[y][x]=1;
		}else if(GO->OnAir){
			RBlockRead(ff1,&OB->RealX,int(&OB->RealDir)-int(&OB->RealX)+1);
			OB->LoadAnimation(0,0,0);//stand=motion animation
			OB->LoadAnimation(1,2,0);//attack
			OB->LoadAnimation(2,3,0);//death
			if(!OB->Sdoxlo)FlyMops[y][x]=OB->Index;
			int cx=(OB->RealX-256)>>11;
			int cy=(OB->RealY-256)>>11;
			FlyCell* FC=&FlyMap[cy][cx];
			for(int k=0;FC->FlyList[k]!=0xFFFF;k++);
			FC->NFly++;
			FC->FlyList[k]=OB->Index;
		}else{
			if(GO->cpbBuilding){
				if(!OB->Sdoxlo){
					for(byte dx=0;dx<OB->Lx;dx++)
						for(byte dy=0;dy<OB->Lx;dy++){
							LLock[y+dy][x+dx]=1;
							IncLock(x+dx,y+dy);
							TrMap[y+dy][x+dx]=1;
							WMap[y+dy][x+dx]=1;
							WLock[y+dy][x+dx]=1;
							Mops[y+dy][x+dx]=Indx;
						};
				};
			}else{
				if(!OB->Sdoxlo){
					Mops[y][x]=Indx;	
					LLock[y][x]=true;
					IncLock(x,y);
				};
			};
	//end of default settings
		};
		
		//loading index of selection group
		int pp=0;
		RBlockRead(ff1,&pp,1);
		if(pp==255)OB->GroupIndex=NULL;
		else OB->GroupIndex=SelSet+pp;
		//Order information loading
		LOADMES(ff1);
		//1. calculating orders
		RBlockRead(ff1,&pp,2);//number of orders
		Order1 ORD1;
		Order1* PORD=NULL;
		for(int j=0;j<pp;j++){
			//loading orders
			RBlockRead(ff1,&ORD1,sizeof ORD1);
			Order1* ORDR=GetOrdBlock();
			if(!j)OB->LocalOrder=ORDR;

			*ORDR=ORD1;
			ORDR->NextOrder=NULL;
			if(PORD)PORD->NextOrder=ORDR;
			PORD=ORDR;
			//DoLink assignment
			ORDR->DoLink=ORDLIST[int(ORDR->DoLink)];
		};
		//Priory level
		RBlockRead(ff1,&OB->PrioryLevel,1);
	};
};
void SaveSelection(ResFile ff1){
	int i='SELI';
	RBlockWrite(ff1,&i,4);
	SAVMES(ff1," SELECTINFO");
	for(i=0;i<80;i++){
		SelGroup* SG=&SelSet[i];
		RBlockWrite(ff1,&SG->NMemb,(sizeof SelGroup)-8);
		if(SG->NMemb){
			RBlockWrite(ff1,SG->Member,SG->NMemb<<1);
			RBlockWrite(ff1,SG->SerialN,SG->NMemb<<1);
		};
	};
	for(i=0;i<8;i++){
		RBlockWrite(ff1,&NSL[i],2);
		if(NSL[i]){
			RBlockWrite(ff1,Selm[i],NSL[i]<<1);
			RBlockWrite(ff1,SerN[i],NSL[i]<<1);
		};
	};
};
void LoadSelection(ResFile ff1){
	int i;
	RBlockRead(ff1,&i,4);
	LOADMES(ff1);
	if(i!='SELI')LOutErr("Save file is corrupted.");
	for(i=0;i<80;i++){
		SelGroup* SG=&SelSet[i];
		RBlockRead(ff1,&SG->NMemb,(sizeof SelGroup)-8);
		if(SG->NMemb){
			SG->Member=new word[SG->NMemb];
			SG->SerialN=new word[SG->NMemb];
			RBlockRead(ff1,SG->Member,SG->NMemb<<1);
			RBlockRead(ff1,SG->SerialN,SG->NMemb<<1);
		}else{
			SG->Member=NULL;
			SG->SerialN=NULL;
		};
	};
	for(i=0;i<8;i++){
		RBlockRead(ff1,&NSL[i],2);
		if(NSL[i]){
			Selm[i]=new word[NSL[i]];
			SerN[i]=new word[NSL[i]];
			RBlockRead(ff1,Selm[i],NSL[i]<<1);
			RBlockRead(ff1,SerN[i],NSL[i]<<1);
		}else{
			Selm[i]=NULL;
			SerN[i]=NULL;
		};
	};
};
void SaveWalls(ResFile ff1){
	int i='LLAW';
	RBlockWrite(ff1,&i,4);
	//SAVMES(ff1," WALLSINFO");
	RBlockWrite(ff1,&GWALLS.MaxWall,4);
	int cln=0;
	for(i=0;i<GWALLS.MaxWall;i++)if(GWALLS.GWC[i].ClusterSize)cln++;
	RBlockWrite(ff1,&cln,4);
	for(i=0;i<GWALLS.MaxWall;i++)if(GWALLS.GWC[i].ClusterSize){
		WallCluster* WC=&GWALLS.GWC[i];
		RBlockWrite(ff1,&i,4);
		RBlockWrite(ff1,&WC->ClusterSize,4);
		RBlockWrite(ff1,&WC->OwnerID,6);
		RBlockWrite(ff1,WC->Cells,WC->ClusterSize*sizeof(WallCell));
	};
	SavePackArray(ff1,0xFFFF,&Links[0][0],65536);
	SavePackArray(ff1,0xFFFF,&LIndex[0][0],65536);
	SAVMES(ff1," RESOURCES");
	RBlockWrite(ff1,&RESRC[0][0],sizeof RESRC);
	RBlockWrite(ff1,&RESADD[0][0],sizeof RESADD);
	RBlockWrite(ff1,&MAXOBJECT,4);
};
void LoadWalls(ResFile ff1){
	int i;
	RBlockRead(ff1,&i,4);
	//LOADMES(ff1);
	if(i!='LLAW')LOutErr("Save file is corrupted.");
	RBlockRead(ff1,&GWALLS.MaxWall,4);
	free(GWALLS.GWC);
	GWALLS.GWC=new WallCluster[GWALLS.MaxWall];
	memset(GWALLS.GWC,0,GWALLS.MaxWall*sizeof(WallCluster));
	int cln;
	int iii;
	RBlockRead(ff1,&cln,4);
	for(i=0;i<cln;i++){
		RBlockRead(ff1,&iii,4);
		WallCluster* WC=&GWALLS.GWC[iii];
		RBlockRead(ff1,&WC->ClusterSize,4);
		RBlockRead(ff1,&WC->OwnerID,6);
		WC->Cells=new WallCell[WC->ClusterSize];
		RBlockRead(ff1,WC->Cells,WC->ClusterSize*sizeof(WallCell));
	};
	LoadPackArray(ff1,&Links[0][0],65536);
	LoadPackArray(ff1,&LIndex[0][0],65536);
};
extern Nation WEP;
void SaveAnmObj(ResFile ff1){
	SAVMES(ff1," ANIMATEINFO");
	RBlockWrite(ff1,&EUsage[0],8192);
	SAVMES(ff1," ANIMATEMAIN");
	for(int i=0;i<8192;i++){
		if(EUsage[i]){
			SAVMES1(ff1," ANMOBJECT:",i);
			AnmObject AO=*GAnm[i];
			if(AO.Sender){
				AO.Sender=(OneObject*)(AO.Sender->Index);
			}else{
				AO.Sender=(OneObject*)(-1);
			};
			int nwp=1024;
			for(int cwp=0;WPLIST[cwp]!=AO.Wpn&&cwp<nwp;cwp++);
			assert(cwp<nwp);
			AO.Wpn=(Weapon*)cwp;
			RBlockWrite(ff1,&AO,sizeof AO);
		};
	};
};
void LoadAnmObj(ResFile ff1){
	LOADMES(ff1);
	RBlockRead(ff1,&EUsage[0],8192);
	LOADMES(ff1);
	AnmObject AO;
	for(int i=0;i<8192;i++){
		if(EUsage[i]){
			LOADMES(ff1);
			RBlockRead(ff1,&AO,sizeof AO);
			int sen=int(AO.Sender);
			if(sen!=-1)AO.Sender=Group[sen];
			else AO.Sender=NULL;
			AO.Wpn=WPLIST[int(AO.Wpn)];
			Octant* OC=AO.Wpn->Vis->lpFAnim[0].Anm;
			if(AO.Wpn->Vis->cpbRotation)
				if(AO.Direction<5)AO.Kino=OC[AO.Direction].Movie;
			else AO.Kino=OC[8-AO.Direction].Movie;
			else AO.Kino=OC->Movie;
			*GAnm[i]=AO;
		};
	};
};
typedef void SWHandle(StrangeWeapon* SW);
void HandleHugeBomb(StrangeWeapon* SW);
void HandleFireSea(StrangeWeapon* SW);
void HandleBlizzard(StrangeWeapon* SW);
void HandleMExplosion(StrangeWeapon* SW);
void HandleMExplosion1(StrangeWeapon* SW);
void HandleFireWall(StrangeWeapon* SW);
void HandleSuperVision(StrangeWeapon* SW);
void HandleFlyUP(StrangeWeapon* SW);
void HandleNucl(StrangeWeapon* SW);
void HandleFog(StrangeWeapon* SW);

#define SWPCOUNT 10
SWHandle* STRWEP[SWPCOUNT]={
	&HandleHugeBomb,
	&HandleFireSea,
	&HandleBlizzard,
	&HandleMExplosion,
	&HandleMExplosion1,
	&HandleFireWall,
	&HandleSuperVision,
	&HandleFlyUP,
	&HandleNucl,
	&HandleFog
};
int GetMagNum(SWHandle* SW){
	for(int i=0;i<SWPCOUNT&&SW!=STRWEP[i];i++);
	assert(i<SWPCOUNT);
	return i;
};
void SaveMagic(ResFile ff1){
	bool WPIND[64];
	SAVMES(ff1," MAGIC");
	StrangeWeapon SWPN;
	for(int i=0;i<64;i++)if(SWEAP[i].Enabled)WPIND[i]=1;
	else WPIND[i]=0;
	RBlockWrite(ff1,&WPIND[0],64);
	for(i=0;i<64;i++)if(WPIND[i]){
		SAVMES1(ff1," MAGOBJ:",i);
		SWPN=SWEAP[i];
		SWPN.SWH=(SWHandle*)GetMagNum(SWPN.SWH);
		if(WPIND[i])RBlockWrite(ff1,&SWPN,sizeof SWPN);
	};
};
void LoadMagic(ResFile ff1){
	LOADMES(ff1);
	bool WPIND[64];
	RBlockRead(ff1,&WPIND[0],64);
	StrangeWeapon SWPN;
	for(int i=0;i<64;i++)if(WPIND[i]){
		LOADMES(ff1);
		RBlockRead(ff1,&SWPN,sizeof SWPN);
		SWPN.SWH=(SWHandle*)STRWEP[int(SWPN.SWH)];
		SWEAP[i]=SWPN;
	}else{
		SWEAP[i].Enabled=false;
	};
};
extern word rpos;
extern word NPlayers;
void SaveGame(char* fnm,char* Messtr,int ID){
	ResFile ff1=RRewrite(fnm);
	RBlockWrite(ff1,&sfHeader,4);
	RBlockWrite(ff1,&sfVersion,4);
	RBlockWrite(ff1,&ID,4);
	RBlockWrite(ff1,&NPlayers,2);
	int sl=strlen(Messtr)+1;
	RBlockWrite(ff1,&sl,1);
	RBlockWrite(ff1,Messtr,sl);
	RBlockWrite(ff1,&mapx,4);
	RBlockWrite(ff1,&mapy,4);
	SaveMap(ff1);
	SaveNations(ff1);
	SaveObjects(ff1);
	SaveSelection(ff1);
	SaveWalls(ff1);
	RBlockWrite(ff1,&TCInf[0][0],sizeof(TCInf));
	SaveAnmObj(ff1);
	SaveMagic(ff1);
	RBlockWrite(ff1,&MyNation,4);
	RBlockWrite(ff1,&rpos,2);
	RBlockWrite(ff1,NucList,sizeof NucList);
	RBlockWrite(ff1,NucSN,sizeof NucSN);
	RBlockWrite(ff1,NDone,sizeof NDone);
	RBlockWrite(ff1,&NNuc,sizeof NNuc);
	RClose(ff1);
};
void LoadGame(char* fnm){
	//assert(_CrtCheckMemory());
	ResFile ff1=RReset(fnm);
	int i,y;
	UnLoading();
	RBlockRead(ff1,&i,4);
	if(i!=sfHeader){
		RClose(ff1);
		return;
	};
	RBlockRead(ff1,&i,4);
	if(i!=sfVersion){
		RClose(ff1);
		return;
	};
	RBlockRead(ff1,&i,4);
	RBlockRead(ff1,&y,2);
	int sl=0;
	RBlockRead(ff1,&sl,1);
	RBlockRead(ff1,GameName,sl);
	RBlockRead(ff1,&mapx,4);
	RBlockRead(ff1,&mapy,4);
	LoadMap(ff1);
	LoadNations(ff1);
	LoadObjects(ff1);
	LoadSelection(ff1);
	LoadWalls(ff1);
	LOADMES(ff1);
	RBlockRead(ff1,&RESRC[0][0],sizeof RESRC);
	RBlockRead(ff1,&RESADD[0][0],sizeof RESADD);
	RBlockRead(ff1,&MAXOBJECT,4);
	RBlockRead(ff1,&TCInf[0][0],sizeof(TCInf));
	LoadAnmObj(ff1);
	LoadMagic(ff1);
	RBlockRead(ff1,&MyNation,4);
	RBlockRead(ff1,&rpos,2);
	RBlockRead(ff1,NucList,sizeof NucList);
	RBlockRead(ff1,NucSN,sizeof NucSN);
	RBlockRead(ff1,NDone,sizeof NDone);
	RBlockRead(ff1,&NNuc,sizeof NNuc);
	RClose(ff1);
	//assert(_CrtCheckMemory());
};