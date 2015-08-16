// City organization module
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
#include <assert.h>
#include "Megapolis.h"
#include "FlyObj.h"
bool FindPortPlace(Nation* NT,int *xx,int *yy);
bool FindOilSpot(int *x,int *y,int r);
extern byte NPresence[64][64];
int DHAPROB;
int DRECLPROB;
int DDIVPROB;
int HANSWPROB;
int HREQRADIUS;
int CROWDMIN;
int HCPROB;
int MaxAllowedComputerAI;
int CMC1;
int CMC2;
int CMC3;
char Mes0[13]="Первая армия";
char Mes1[13]="Вторая армия";
char Mes2[13]="Третья армия";
char Mes3[16]="Четвертая армия";
char Mes4[12]="Пятая армия";
char Mes5[13]="Шестая армия";
char Mes6[14]="Седьмая армия";
char Mes7[14]="Восьмая армия";
char Mes8[14]="Девятая армия";
char Mes9[14]="Десятая армия";
char MesDef[13]="Отряд охраны";
char* mess[11]={Mes0,Mes1,Mes2,Mes3,Mes4,Mes5,Mes6,Mes7,Mes8,Mes9,MesDef};
int BestForce;
int BestNat;
int NextBest;
byte NLocks[64][64];
Cell8x8::Cell8x8(){
	for(int i=0;i<5;i++)UnitsAmount[i]=0;
	Neighbor=0;
};
word FindTerrEnemy(int xCell,int yCell,int mx,int my,int dist,byte Mask);
TotalCInfo  TCInf;
bool CheckZone(int x,int y,int Lx,int Ly){
	for(int i=0;i<Lx;i++)
		for(int j=0;j<Ly;j++){
			if(TrMap[y+j][x+i])return false;
		};
	return true;
};
int Trx;
int Try;
int Trx1;
int Try1;
byte TerrMap[64][64];
int wer(byte x){
	if(x)return 1;
	else return 0;
};
void TerrSpot(int x,int y,byte c){
	if(x>0&&y>0)TerrMap[y][x]+=c;
};
void CreateTerrMap(byte NN){
	memset(&TerrMap[0][0],0,sizeof TerrMap);
	Trx=200;
	Try=200;
	Trx1=-1;
	Try1=-1;
	int maxtx=msx>>2;
	int maxty=msy>>2;
	byte knd;
	for(int i=0;i<MAXOBJECT;i++){
		OneObject* OB=Group[i];
		if(OB&&OB->NNUM==NN){
			knd=OB->Kind;
			if(knd==4||knd==3){
				int xx=OB->x>>2;
				int yy=OB->y>>2;
				TerrSpot(xx,yy,4);
				TerrSpot(xx-1,yy,3);
				TerrSpot(xx+1,yy,3);
				TerrSpot(xx,yy-1,3);
				TerrSpot(xx,yy+1,3);
				TerrSpot(xx-1,yy-1,2);
				TerrSpot(xx+1,yy-1,2);
				TerrSpot(xx-1,yy+1,2);
				TerrSpot(xx+1,yy+1,2);
				for(int p=0;p<3;p++){
					TerrSpot(xx-2,yy+p-1,1);
					TerrSpot(xx+2,yy+p-1,1);
					TerrSpot(xx+p-1,yy-2,1);
					TerrSpot(xx+p-1,yy+2,1);
				};
				if(Trx>xx)Trx=xx;
				if(Try>yy)Try=yy;
				if(Trx1<xx)Trx1=xx;
				if(Try1<yy)Try1=yy;
			};
		};
	};
};
//Блок оценки сил противника
void Forces::SetDefaults(){
	MinX=55255;
	MaxX=0;
	MinY=55255;
	MaxX=0;
	Damage=0;
	Life=0;
	Shield=0;
	Population=0;
	NBuild=0;
	CenterX=0;
	CenterY=0;
	StrongFactor=0;
};
Forces NForces[8];
void GetForces(){
	for(int i=0;i<8;i++)NForces[i].SetDefaults();
	for(i=0;i<MaxObject;i++){
		OneObject* OB=Group[i];
		if(OB){
			Forces* FR=&NForces[OB->NNUM];
			Visuals* VS=OB->Ref.Visual;
			FR->Damage+=VS->info.Basic.MaxDamage;
			FR->Life+=OB->Life;
			FR->Shield+=VS->info.Basic.MaxShield;
			//protection
			if(OB->Kind==1)
				FR->Protection+=VS->info.Basic.MaxShield+
				(VS->info.Basic.MaxDamage>>4);
			if(!OB->capBuilding)FR->Population++;
			else{
				int x=OB->x;
				int y=OB->y;
				if(x>FR->MaxX)FR->MaxX=x;
				if(y>FR->MaxY)FR->MaxY=y;
				if(x<FR->MinX)FR->MinX=x;
				if(y<FR->MinY)FR->MinY=y;
				FR->CenterX+=x;
				FR->CenterY+=y;
				FR->NBuild++;
			};	
		};
	};
	BestNat=-1;
	NextBest=-1;
	BestForce=0;
	for(i=0;i<8;i++){
		Forces* FC=&NForces[i];
		if(FC->NBuild){
			FC->CenterX=div(FC->CenterX,FC->NBuild).quot;
			FC->CenterY=div(FC->CenterY,FC->NBuild).quot;
		};
		if(FC->MinX<FC->MaxX&&FC->MinY<FC->MaxY){
			int Square=(FC->MaxX-FC->MinX)*(FC->MaxY-FC->MinY);
			FC->StrongFactor=div((FC->Damage<<3)+FC->Life+(FC->Shield<<2),10000+Square).quot;
			if(FC->StrongFactor>BestForce){
				NextBest=BestNat;
				BestNat=i;
				BestForce=FC->StrongFactor;
			};
		};
	};
	BestForce=0;
	NextBest=-1;
	for(i=0;i<8;i++){
		Forces* FC=&NForces[i];
		if(i!=BestNat&&FC->StrongFactor>BestForce){
			NextBest=i;
			BestForce=FC->StrongFactor;
		};
	};
	Forces* FC=&NForces[0];
	if(FC->NBuild){
		BestNat=0;
	};
	
};

//Функционалы для определения места для здания
//1-место для стандартного здания (ферма, барак,...)
int FUNC1(int* x,int* y,int Lx,int Ly,byte NI){
	int XX=*x&~3;
	int YY=*y&~3;
	int xxx=XX>>2;
	int yyy=YY>>2;
	if(Lx<2){XX++;YY++;};
	if(XX<=8||YY<=8||XX+Lx>=msx-8||YY+Ly>=msy-8)return -1;
	int FN=0;
	Cell8x8* CL=&TCInf[NI][yyy][xxx];
	word NB=CL->Neighbor;
	if(!NB)return -1;
	if(!CheckZone(XX-1,YY-1,Lx+2,Ly+2))return -1;
	int tmm;
	byte* bb=&TerrMap[yyy][xxx];
	tmm=bb[0];
	if(NB&32768)tmm++;
	if(!tmm)return -1;
	FN+=tmm<<8;
	/*
	if(NB&0x001)FN+=300;
	if(NB&0x002)FN+=300;
	if(NB&0x004)FN+=300;
	if(NB&0x008)FN+=300;
	if(NB&0x010)FN+=200;
	if(NB&0x020)FN+=200;
	if(NB&0x040)FN+=200;
	if(NB&0x080)FN+=200;
	*/
	FN+=rando()&15;  
	//FN+=CL->Peasants<<6;
	*x=XX;
	*y=YY;
	return FN;
};
int FUNC3(int* x,int* y,int Lx,int Ly,byte NI){
	int XX=*x&~3;
	int YY=*y&~3;
	int xxx=XX>>2;
	int yyy=YY>>2;
	if(xxx<2||yyy<2)return -1;
	if(Lx<2){XX++;YY++;};
	if(XX<=0||YY<=0||XX+Lx>=msx||YY+Ly>=msy)return -1;
	int FN=100000;
	Cell8x8* CL=&TCInf[NI][yyy][xxx];
	word NB=CL->Neighbor;
	//if(!NB)return -1;
	if(!CheckZone(XX-1,YY-1,Lx+2,Ly+2))return -1;
	byte* bb=&TerrMap[yyy][xxx];
	if(!bb[0])return -1;
	if(bb[1]&&bb[-1]&&bb[-64]&&bb[64])return -1;
	FN+=1000;
	/*
	if(NB&0x001)FN+=300;
	if(NB&0x002)FN+=300;
	if(NB&0x004)FN+=300;
	if(NB&0x008)FN+=300;
	if(NB&0x010)FN+=200;
	if(NB&0x020)FN+=200;
	if(NB&0x040)FN+=200;
	if(NB&0x080)FN+=200;
	*/
	FN+=rando()&255; 
	//FN+=CL->Peasants<<6;
	*x=XX;
	*y=YY;
	return FN;
};
//for Towers
int FUNC2(int* x,int* y,int Lx,int Ly,byte NI){
	int XX=*x&~3;
	int YY=*y&~3;
	if(Lx<2){XX++;YY++;};
	if(XX<=0||YY<=0||XX+Lx>=msx||YY+Ly>=msy)return -1;
	int FN=0;
	Cell8x8* CL=&TCInf[NI][YY>>2][XX>>2];
	word NB=CL->Neighbor;
	if(!NB)return -1;
	if(!CheckZone(XX-1,YY-1,Lx+2,Ly+2))return -1;
	if(NB&(0x001|0x004|0x010|0x040))return -1;
	if(NB&(0x100|0x002|0x200|0x008|0x400|0x020|0x800|0x080))FN+=1000;
	FN+=rando()&255;                  
	FN+=int(CL->Peasants)<<9;
	*x=XX;
	*y=YY;
	return FN;
};
void OccupyCell(int x,int y,byte NI){
	int xx=x>>2;
	int yy=y>>2;
	CellsInfo* CI=&TCInf[NI];
	if(yy>0){
		CI[0][yy-1][xx-1].Neighbor|=0x008;
		if(xx>0)CI[0][yy-1][xx].Neighbor|=0x010;
		if(xx<63)CI[0][yy-1][xx+1].Neighbor|=0x020;
	};
	if(xx>0)CI[0][yy][xx-1].Neighbor|=0x004;
	if(xx<63)CI[0][yy][xx+1].Neighbor|=0x040;
	if(yy<63){
		CI[0][yy+1][xx].Neighbor|=0x001;
		if(xx>0)CI[0][yy+1][xx-1].Neighbor|=0x002;
		if(xx<63)CI[0][yy+1][xx+1].Neighbor|=0x080;
	};
	if(xx>1)CI[0][yy][xx-2].Neighbor|=0x200;
	if(xx<62)CI[0][yy][xx+2].Neighbor|=0x800;
	if(yy>1)CI[0][yy-2][xx].Neighbor|=0x400;
	if(yy<62)CI[0][yy+2][xx].Neighbor|=0x100;
};
word FindPeasantInCell(int x,int y,byte NI){
	int xx=(x<<2);
	int yy=(y<<2);
	OneObject* OB;
	word MID;
	for(int dx=0;dx<4;dx++)
		for(int dy=0;dy<4;dy++){
			MID=Mops[yy+dy][xx+dx];
			if(MID!=0xFFFF){
				OB=Group[MID];
				if(OB&&OB->NNUM==NI&&!OB->Kind){
					Order1* OR1=OB->LocalOrder;
					if(!int(OR1)||(OR1&&OR1->OrderType!=89))return MID;
				};
			};
		};
	return 0xFFFF;
};
word FindPeasant(int x,int y,byte NI){
	int sx=x>>2;
	int sy=y>>2;
	int sx1=sx;
	int sy1=sy;
	int r=32;
	do{
		for(int x1=sx;x1<=sx1;x1++)
			for(int y1=sy;y1<=sy1;y1++){
				if(x1==sx||x1==sx1||y1==sy||y1==sy1){
					Cell8x8* CL=&TCInf[NI][y1][x1];
					if(CL->Peasants>0){
						word PS=FindPeasantInCell(x1,y1,NI);
						if(PS!=0xFFFF)return PS;
					};
				};
			};
		sx1++;
		sy1++;
		sx--;
		sy--;
		if(sx<0)sx=0;
		if(sy<0)sy=0;
		if(sx1>63)sx1=63;
		if(sy1>63)sy1=63;
		r--;
	}while(r);
	return 0xFFFF;
};
word FindWPeasantInCell(int x,int y,byte NI){
	int xx=(x<<2);
	int yy=(y<<2);
	OneObject* OB;
	word MID;
	for(int dx=0;dx<4;dx++)
		for(int dy=0;dy<4;dy++){
			MID=Mops[yy+dy][xx+dx];
			if(MID!=0xFFFF){
				OB=Group[MID];
				if(OB&&OB->NNUM==NI&&OB->Kind==6){
					Order1* OR1=OB->LocalOrder;
					if(!int(OR1)||(OR1&&OR1->OrderType!=89))return MID;
				};
			};
		};
	return 0xFFFF;
};
word FindWPeasant(int x,int y,byte NI){
	int sx=x>>2;
	int sy=y>>2;
	int sx1=sx;
	int sy1=sy;
	int r=32;
	do{
		for(int x1=sx;x1<=sx1;x1++)
			for(int y1=sy;y1<=sy1;y1++){
				if(x1==sx||x1==sx1||y1==sy||y1==sy1){
					Cell8x8* CL=&TCInf[NI][y1][x1];
					if(CL->WaterPeas>0){
						word PS=FindWPeasantInCell(x1,y1,NI);
						if(PS!=0xFFFF)return PS;
					};
				};
			};
		sx1++;
		sy1++;
		sx--;
		sy--;
		if(sx<0)sx=0;
		if(sy<0)sy=0;
		if(sx1>63)sx1=63;
		if(sy1>63)sy1=63;
		r--;
	}while(r);
	return 0xFFFF;
};
void FreeCell(int x,int y,byte NI){
	int xx=x>>2;
	int yy=y>>2;
	CellsInfo* CI=&TCInf[NI];
	if(yy>0){
		CI[0][xx][yy-1].Neighbor&=~0x001;
		if(xx>0)CI[0][xx-1][yy-1].Neighbor&=~0x080;
		if(xx<63)CI[0][xx+1][yy-1].Neighbor&=~0x002;
	};
	if(xx>0)CI[0][xx-1][yy].Neighbor&=~0x040;
	if(xx<63)CI[0][xx+1][yy].Neighbor&=~0x004;
	if(yy<63){
		CI[0][xx][yy+1].Neighbor&=~0x010;
		if(xx>0)CI[0][xx-1][yy+1].Neighbor&=~0x008;
		if(xx<63)CI[0][xx+1][yy+1].Neighbor&=~0x020;
	};
	if(xx>1)CI[0][xx-2][yy].Neighbor&=~0x800;
	if(xx<62)CI[0][xx+2][yy].Neighbor&=~0x200;
	if(yy>1)CI[0][xx][yy-2].Neighbor&=~0x100;
	if(yy<62)CI[0][xx][yy+2].Neighbor&=~0x400;
};
void City::CreateCity(int x,int y,byte N){
	InX=x>>2;
	InY=y>>2;
	InX1=x;
	InY1=y;
	bool pfn=false;
	do{
		pfn=CheckZone(InX<<2,InY<<2,4,4);
		if(!pfn){
			InX+=((int(rando())&1)<<1)-1;
			InX+=((int(rando())&1)<<1)-1;
			if(InX<0)InX=0;
			if(InY<0)InY=0;
			if(InX>63)InX=63;
			if(InY>63)InY=63;
		};
	}while(!pfn);
	TCInf[N][InY][InX].Neighbor=32768;
	OutX=x>>2;
	OutY=y>>2;
	OutX1=OutX+2;
	OutY1=OutY+2;
	GoldPeasPart=100;
	WoodPeasPart=20;
	BuildPeasPart=30;
	KindMax[0]=128;//Peasants
	KindMax[1]=128;//Towers
	KindMax[2]=512;//Warriors
	KindMax[3]=128;//Farms
	KindMax[4]=128;//Buildings
	//for(int i=0;i<N_KINDS;i++)MKList[i]=new word[KindMax[i]];
	FNC[0]=&FUNC1;
	FNC[1]=&FUNC3;//2;
	FNC[2]=&FUNC1;
	FNC[3]=&FUNC1;
	FNC[4]=&FUNC1;
	UTNum=0;
	PresentProject=false;
	NI=N;
	Nat=&NATIONS[N];
	Nat->CasheSize=0;
	Nat->NFarms=0;
	Nat->NGidot=0;
	for(int i=0;i<NBRANCH;i++)BranchPart[i]=Nat->BranchPercent[i];
	TryTimes=32;
	ExpandSize=2;
	for(i=0;i<32;i++)EnemyList[i]=0xFFFF;
	NATIONS[N].CITY=this;
	IntellectEnabled=false;
};
bool City::CheckTZone(int x,int y,int Lx,int Ly){
	for(int i=0;i<Lx;i++)
		for(int j=0;j<Ly;j++){
			if(TrMap[y+j][x+i])return false;
		};
	return true;
};
bool City::TryToFindPlace(int* x,int* y,int Lx,int Ly,byte Kind){
	CreateTerrMap(NI);
	int xx;
	int yy;
	int Sum=-1;
	int CSum;
	int Bx=0;
	int By=0;
	Functional* FF=FNC[Kind];
	if(Kind==1&&rando()<12000)FF=FNC[0];
	for(int i=0;i<=63;i++)
		for(int j=0;j<63;j++){
			xx=i*4;
			yy=j*4;
			CSum=FF(&xx,&yy,Lx,Ly,NI);
			if(CSum>Sum){
				Bx=xx;
				By=yy;
				Sum=CSum;
			};
		};
		if(Sum<0){
			if(OutX>0)OutX--;
			if(OutX1<(msx>>3))OutX1++;
			if(OutY>0)OutY--;
			if(OutY1<(msy>>3))OutY1++;
			return false;
		};
		*x=Bx;
		*y=By;
		int OBx=Bx>>3;
		int OBy=By>>3;
		if(OBx<=OutX)OutX=OBx-1;
		if(OBy<=OutY)OutY=OBy-1;
		if(OBx>=OutX1)OutX1=OBx+1;
		if(OBy>=OutY1)OutY1=OBy+1;
		return true;
};
void City::EnumUnits(){
	OneObject* OB;
	memset(UnitAmount,0,sizeof UnitAmount);
	memset(ReadyAmount,0,sizeof ReadyAmount);
	memset(UnBusyAmount,0,sizeof UnitAmount);
	memset(PRPIndex,255,sizeof PRPIndex);
	memset(UPGIndex,255,sizeof UPGIndex);
	UTNum=0;
	Amount=0;
	for(int i=0;i<MAXOBJECT;i++){
		OB=Group[i];
		if(OB&&OB->NNUM==NI){
			UnitAmount[OB->NIndex]++;
			if(OB->Stage==OB->NStages)ReadyAmount[OB->NIndex]++;
			if(!OB->capBuilding)Amount++;
			if(OB->Ready&&!int(OB->LocalOrder)){
				if(OB->Ref.General->Warrior){
					int xxw=OB->x;
					int yyw=OB->y;
					if(xxw>=OutX&&xxw<=OutX1&&yyw>=OutY&&yyw<=OutY1)OB->MoveFrom(0);
				};
				UnBusyAmount[OB->NIndex]++;
				Producer[OB->NIndex]=i;
			};
			if(OB->NIndex>UTNum)UTNum=OB->NIndex;
		};
	};
	if(PresentProject&&(!BPR.Founded)){
		UnitAmount[BPR.Type]++;
	};
};
void City::AddProp(byte NI,word NIN,GeneralObject* GO,word prod){
	if(NProp>=100)return;
	if(GO->MaxAutoAmount<=UnitAmount[NIN])return;
	for(int i=0;i<NProp;i++){
		ProposedProject* PRP=&Prop[i];
		if(PRP->NIndex==NIN&&PRP->PKind==0)return;
	};
	ProposedProject* PR=&Prop[NProp];
	NProp++;
	PR->PKind=0;
	PR->NIndex=NIN;
	PR->Percent=div(GO->cost<<8,RESRC[NI][1]).quot;
	PRPIndex[NIN]=NProp-1;
	PR->Cost=GO->cost;
	PR->ProducerIndex=prod;
	for(i=0;i<NBRANCH;i++){
		PR->Useful[i]=GO->Useful[i];
	};
};
void City::AddUpgr(byte NI,word UIN,SimpleUTP* UT,word prod){
	if(NProp>=100)return;
	for(int i=0;i<NProp;i++){
		ProposedProject* PRP=&Prop[i];
		if(PRP->NIndex==UIN&&PRP->PKind==1)return;
	};
	ProposedProject* PR=&Prop[NProp];
	NProp++;
	PR->PKind=1;
	PR->NIndex=UIN;
	PR->Percent=div(UT->Cost<<8,RESRC[NI][1]).quot;
	UPGIndex[UIN]=NProp-1;
	PR->Cost=UT->Cost;
	PR->ProducerIndex=prod;
	for(i=0;i<NBRANCH;i++){
		PR->Useful[i]=0;
	};
	//PR->Useful[2]=1;
};
void City::EnumProp(){
	NProp=0;
	//enumerating monsters could be produced 
	int PC;
	for(int i=0;i<=UTNum;i++){
		if(i==1||UnBusyAmount[i]) PC=Nat->PACount[i];
		else PC=0;
		for(int j=0;j<PC;j++){
			word s=Nat->PAble[i][j];
			//проверить возможность производства
			GeneralObject* GO=Nat->Mon[s];
			if(GO->Enabled){
				bool able=true;
				//int RG=RESRC[Nat->NNUM][1];
				//RESRC[Nat->NNUM][1]-=Nat->CasheSize;
				for(int k=0;k<4;k++){
					byte RID=GO->ResourceID[k];
					if(RID!=255){
						if(GO->ResAmount[k]>RESRC[Nat->NNUM][RID])able=false;
					};
				};
				//RESRC[Nat->NNUM][1]=RG;
				//it is possible to produce this tvar
				if(able)AddProp(NI,s,GO,Producer[i]);
			};
		};
		GeneralObject* GO=Nat->Mon[i];
		if(UnBusyAmount[i]) PC=GO->NUpgrades;
		else PC=0;
		for(j=0;j<PC;j++){
			word s=GO->Upg[j];
			//проверить возможность производства
			SimpleUTP* SU=Nat->UPG.utp[s];
			if((!SU->Stage)&&SU->Enabled&&SU->Cost<=RESRC[Nat->NNUM][1])
				AddUpgr(NI,s,SU,Producer[i]);
		};
	};
};
//returns 0xFFFF if project not found and a project number, if found
/*word City::FindNeedProject(){
	Nation* NT=Nat;
	int TotalMoney=RESRC[NT->NNUM][1];
	int FreeMoney=TotalMoney-NT->CasheSize;
	int NNeed=NT->NNeed;
	if(Amount>8)Nat->NEED->Amount=div(Amount,15).quot+2;
	else Nat->NEED->Amount=1;
	Needness* NEED=NT->NEED;
	word T0ID=NT->TOWN0ID;
	int i=0;
	do{
		int NPR=NEED->GroupSize;
		int RVL=(int(rando())*NPR)>>15;
		Needness* ND1=NEED+RVL;
		if(!ND1->NeedType){
			word TvarID=ND1->MonID;
			//random condition checking
			int uniam=UnitAmount[TvarID];
			if(TvarID==T0ID){
				uniam+=UnitAmount[NT->TOWN1ID];
				uniam+=UnitAmount[NT->TOWN2ID];
			};
			if(ND1->Probability>rando()&&ND1->Amount>uniam){
				//checking the need conditions
				word PropInd=PRPIndex[TvarID];
				if(PropInd!=0xFFFF){
					//project present
					ProposedProject* PRP=&Prop[PropInd];
					//1.Money percent
					if(PRP->Percent<ND1->MoneyPercent){
						if(PresentProject){
							GeneralObject* GO=NT->Mon[TvarID];
							if(!GO->cpbBuilding)return PropInd;
						}else return PropInd;
					};
				};
			};
		}else{
			word UpgID=ND1->MonID;
			//random condition checking
			if(ND1->Probability>rando()){
				//checking the need conditions
				word PropInd=UPGIndex[UpgID];
				if(PropInd!=0xFFFF){
					//project present
					ProposedProject* PRP=&Prop[PropInd];
					//1.Money percent
					if(PRP->Percent<ND1->MoneyPercent)return PropInd;
				};
			};
		};
		NEED+=NPR;
		i+=NPR;
		if(!NPR){
			NEED++;
			i++;
		};
	}while(i<NNeed);
	return 0xFFFF;
};*/
#define FARMID 20
word City::FindNeedProject(){
	//Groups calculating
	Nation* NT=Nat;
	for(int i=0;i<NT->NGrp;i++){
		int gs=NT->GRSize[i];
		word* IDs=NT->GRRef[i];
		int ng=0;
		for(int j=0;j<gs;j++){
			ng+=UnitAmount[IDs[j]];
		};
		NT->GAmount[i]=ng;
	};
	int TotalMoney=RESRC[NT->NNUM][1];
	int FreeMoney=TotalMoney/*-NT->CasheSize*/;
	//Definition of the AI level
	int MaxAI=-1;
	for( i=0;i<NT->N_AI_Levels;i++){
		bool OK=true;
		int nrq=NT->N_AI_Req[i];
		for(int p=0;p<nrq;p++){
			sAI_Req* SRQ=&NT->AI_Req[i][p];
			switch(SRQ->Kind){
				case 0://unit
					if(UnitAmount[SRQ->ObjID]<SRQ->Amount)OK=false;
					break;
				case 2://group
					if(NT->GAmount[SRQ->ObjID]<SRQ->Amount)OK=false;
					break;
			};

		};
		if(OK)MaxAI=i;
	};
	NT->AI_Level=MaxAI;
	//Let us use current AI method
	//----Checking farms
	int NFNeed=div(NT->NGidot,15).quot+1;
	if(NFNeed>1)NFNeed++;
	if(NFNeed>5)NFNeed++;
	if(NFNeed>10)NFNeed++;
	int NF=NT->NFarms;
	if(NFNeed>UnitAmount[FARMID]){
		word FPR=PRPIndex[FARMID];
		if(FPR!=0xFFFF)return FPR;
	};
	if(MaxAI==-1)return 0xFFFF;
	//----Checking other units by script
	sAI_Devlp* SAD=NT->AI_Devlp[MaxAI];
	word ndev=NT->N_AI_Devlp[MaxAI];
	for(i=0;i<ndev;i++){
		sAI_Devlp* sad=&SAD[i];
		if(!sad->Kind){
			//units
			if((sad->ConKind&&NT->GAmount[sad->ConID]<sad->Amount)||
				((!sad->ConKind)&&sad->Amount>UnitAmount[sad->ObjID])){
				word prid=PRPIndex[sad->ObjID];
				if(prid!=0xFFFF){
					//random checking,money checking 
					if(rando()<sad->AtnPercent){
						ProposedProject* PRP=&Prop[prid];
						//1.Money percent
						if(PRP->Percent<sad->GoldPercent){
							if(PresentProject){
								GeneralObject* GO=NT->Mon[sad->ObjID];
								if(!GO->cpbBuilding)return prid;
							}else return prid;
						};
					};
				};
			};
		}else{
			//upgrades
			word UpgID=sad->ObjID;
			//random condition checking
			if(sad->AtnPercent>rando()){
				//checking the need conditions
				word PropInd=UPGIndex[UpgID];
				if(PropInd!=0xFFFF){
					//project present
					ProposedProject* PRP=&Prop[PropInd];
					//1.Money percent
					if(PRP->Percent<sad->GoldPercent)return PropInd;
				};
			};

		};
	};
	return 0xFFFF;
};
void City::RefreshAbility(){
	//Let's disable all checked units/upgrades 
	for(int i=0;i<Nat->NCOND;i++){
		word MID=Nat->CLRef[i][0];
		if(MID<8192){
			//Monster
			Nat->Mon[MID]->Enabled=false;
		}else{
			Nat->UPG.utp1[MID&8191]->Enabled=false;
		};
	};
	//Let us check it now and enable all allowed monsters/upgrades
	for(i=0;i<Nat->NCOND;i++){
		word MID=Nat->CLRef[i][0];
		word NC=Nat->CLSize[i];
		bool enab=true;
		for(int j=0;j<NC;j++){
			word uc=Nat->CLRef[i][j+1];
			if(!ReadyAmount[uc])enab=false;
		};
		if(enab){
			if(MID<8192){
				//Monster
				Nat->Mon[MID]->Enabled=true;
			}else{
				SimpleUTP* sutp=Nat->UPG.utp1[MID&8191];
				if(!sutp->Done)sutp->Enabled=true;
			};
		};
	};
	for(i=0;i<Nat->NMon;i++){
		if(Nat->Mon[i]->LockID!=0xFFFF){
			if(ReadyAmount[Nat->Mon[i]->LockID]>Nat->Mon[i]->NLockUnits)
				Nat->Mon[i]->Enabled=false;
		};
	};
};
void RepairControl(City* CT){
	byte NI=CT->NI;
	word PS;
	for(int i=0;i<MAXOBJECT;i++){
		OneObject* OB=Group[i];
		if(OB&&OB->capBuilding&&(!OB->delay)&&OB->Stage==OB->NStages){
			int maxl=OB->MaxLife;
			if(OB->Life<maxl){
				PS=FindPeasant(OB->x,OB->y,NI);
				if(PS!=0xFFFF){
					OneObject* OBP=Group[PS];
					if(OBP)OBP->BuildObj(OB->Index,128);
					OB->delay=200;
				};
			};
			if(OB->Life<(maxl<<1)){
				PS=FindPeasant(OB->x,OB->y,NI);
				if(PS!=0xFFFF){
					OneObject* OBP=Group[PS];
					if(OBP)OBP->BuildObj(OB->Index,128);
					OB->delay=200;
				};
			};
		};
	};
};
void City::ProcessCreation(){
	EnumUnits();
	int NTH1=ReadyAmount[Nat->TOWNHALL1];
	int NTH2=ReadyAmount[Nat->TOWNHALL2];
	int LUMB=ReadyAmount[Nat->LUMBERMILL];
	if(LUMB)RESADD[NI][2]=15;
	else RESADD[NI][2]=0;
	if(NTH2)RESADD[NI][1]=25;
	else{
		if(NTH1)RESADD[NI][1]=15;
		else RESADD[NI][1]=0;
	};
	RefreshAbility();
	if(Nat->MagicDelay)Nat->MagicDelay--;
	if(Nat->LastAttackTime)Nat->LastAttackTime--;
	if(!IntellectEnabled)return;
	//if(rando()<12000)RepairControl(this);
	int MACAI=(MaxAllowedComputerAI*Nat->AI_Forward)>>8;
	if(NForces[Nat->NNUM].StrongFactor>MACAI)Nat->NationalAI=Nat->AI_Level_MIN;
	else Nat->NationalAI=Nat->AI_Level_MAX;
	bool Develop=rando()<Nat->NationalAI;
	Develop=true;
	if(rando()<Nat->CASH_PUSH_PROBABILITY)Nat->CasheSize=0;
	for(int ii=0;ii<10;ii++)CROWDS[ii].HandleCrowd();
	if(rando()<DHAPROB){
		Defence.GetSize();
		Defence.HandleAttacks(1);
		Defence.HandleDefence();
	};
	if(rando()<DRECLPROB){
		if(rando()<DDIVPROB){
			if(Defence.DisbandHalf()){
				for(int ii=0;ii<10;ii++){
					if((!CROWDS[ii].Active)){
						CROWDS[ii].CreateCrowd(NI);
						CROWDS[ii].Message=mess[ii];
						CROWDS[ii].Usage=2;
						Defence.Disband();
						Defence.CreateCrowd(NI);
						Defence.Message=mess[10];
						return;
					};
				};
			};
		};
		Defence.Disband();
		Defence.CreateCrowd(NI);
		Defence.Message=mess[10];
	};
	//отделяем некорректные запросы о помощи
	byte MyMsk=1<<NI;
	for(ii=0;ii<32;ii++){
		word MID=EnemyList[ii];
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if((!OB)||(OB&&(OB->NMask&MyMsk||OB->Sdoxlo)))EnemyList[ii]=0xFFFF;
		};
	};
	if(rando()<HANSWPROB){
		//отвечаем на запросы о помощи
		for(ii=0;ii<32;ii++){
			word MID=EnemyList[ii];
			if(MID!=0xFFFF)Defence.AddToAttackQueue(MID);
		};
	};
	int ProjNum;
	int ObjID;
	int Branch=777;
	int r,Sum;
	word ANPrio=0;
	EnumProp();
	HandleFly();
	if(!Amount)return;
	//рассылаем свободных крестьян добывать ресурсы
	if(UnBusyAmount[1]>0){
		int NMiss=UnBusyAmount[1];
		for(int p=0;p<MAXOBJECT;p++){
			if(!NMiss)goto iii;
			OneObject* OBB=Group[p];
			if(OBB&&OBB->NNUM==NI&&OBB->NIndex==1&&
				(!int(OBB->LocalOrder))){
				if(rando()<25000)OBB->TakeResource(OBB->x,OBB->y,1,100);
				else OBB->TakeResource(OBB->x,OBB->y,2,100);
				NMiss--;
			};
		};
	};
iii:
	if(Develop){
		//проверяем выполнение необходимых условий
		ProjNum=FindNeedProject();
		if(ProjNum!=0xFFFF){
			ProposedProject* PRPS=&Prop[ProjNum];
			if(PRPS->PKind==1){
				//Nat->UPG.utp[PRPS->NIndex]->DoUpgrade(NI);
				Group[PRPS->ProducerIndex]->PerformUpgrade(PRPS->NIndex);
				return;
			};
		};
		/*if(ProjNum==0xFFFF){
			//Выбираем отрасль
			Sum=0;
			for(int i=0;i<NBRANCH;i++)Sum+=BranchPart[i];
			r=(int(rando())*Sum)>>15;
			for(i=0;i<NBRANCH&&r>=0;i++)r-=BranchPart[i];
			Branch=i-1;
			assert(Branch>=0);
			//Branch=номер развиваемой отрасли
			//Выбираем объект, который будем создавать
			Sum=0;
			for(i=0;i<NProp;i++)Sum+=Prop[i].Useful[Branch];
			if(!Sum)return;
			r=(int(rando())*Sum)>>15;
			for(i=0;i<NProp&&r>=0;i++)r-=Prop[i].Useful[Branch];
			ProjNum=i-1;
			ProposedProject* PRPS=&Prop[ProjNum];
			if(PRPS->PKind==1){
				//Nat->UPG.utp[PRPS->NIndex]->DoUpgrade(NI);
				Group[PRPS->ProducerIndex]->PerformUpgrade(PRPS->NIndex);
				return;
			};
			ObjID=Prop[ProjNum].NIndex;
			assert(Prop[ProjNum].Useful[Branch]);
		}else */
		if(ProjNum!=0xFFFF){
			ObjID=Prop[ProjNum].NIndex;
			//ObjID- номер объекта, который нужно сделать
			GeneralObject* GO=Nat->Mon[ObjID];
			if(GO->cpbBuilding){
				if(!PresentProject){
					PresentProject=true;
					BPR.PlaceFound=false;
					BPR.Founded=false;
					BPR.PeonsCalled=false;
					BPR.LastStage=0;
					BPR.Progress=0;
					BPR.Lx=GO->SizeX;
					BPR.Ly=GO->SizeY;
					BPR.Index=0xFFFF;
					BPR.Type=ObjID;
					BPR.Producer=Prop[ProjNum].ProducerIndex;
					BPR.Kind=GO->Kind;
				};
			}else{	
				OneObject* OB=Group[Prop[ProjNum].ProducerIndex];
				if(OB)OB->Produce(ObjID);
			};
		};
	};
	if(PresentProject){
		if(!BPR.PlaceFound){
			if(BPR.Type==Nat->PortID){
				byte nind=Nat->NNUM;
				if(NForces[nind].MaxX){
					BPR.x=NForces[nind].CenterX;
					BPR.y=NForces[nind].CenterY;
					if(FindPortPlace(Nat,&BPR.x,&BPR.y)){
						BPR.PlaceFound=true;
						goto PF1;
					}else PresentProject=false;
				}else PresentProject=false;
			}else
			if(BPR.Type==Nat->OilBaseID){
				byte nind=Nat->NNUM;
				if(NForces[nind].MaxX){
					BPR.x=NForces[nind].CenterX;
					BPR.y=NForces[nind].CenterY;
					if(FindOilSpot(&BPR.x,&BPR.y,128)){
						BPR.PlaceFound=true;
						goto PF1;
					}else PresentProject=false;
				}else PresentProject=false;
			}else
			if(TryToFindPlace(&BPR.x,&BPR.y,BPR.Lx,BPR.Ly,BPR.Kind)){
				//BPR.x++;
				//BPR.y++;
				BPR.PlaceFound=true;
				goto PF1;
			};
			return;
		};
PF1:	if(!BPR.Founded){
			bool EstTvari=false;
			for(int i=0;i<BPR.Lx;i++)
				for(int j=0;j<BPR.Ly;j++){
					word MID=Mops[BPR.y+j][BPR.x+i];
					if(MID!=0xFFFF){
						OneObject* OB=Group[MID];
						if(OB)OB->MoveFrom(0);
						EstTvari=true;
					};
				};
				if(!EstTvari){
					if(Nat->CheckBuilding(BPR.Type,BPR.x,BPR.y)){
						word ID=Nat->CreateBuilding(BPR.Type,BPR.x,BPR.y);
						BPR.Index=ID;
						if(ID!=0xFFFF){
							OccupyCell(BPR.x,BPR.y,NI);
							BPR.Founded=true;
						};
					};
				};
		return;
		};
		if(BPR.Founded){
			OneObject* OB=Group[BPR.Index];
			if((!OB)||(OB&&(OB->NNUM!=NI||OB->NIndex!=BPR.Type)))BPR.Founded=false;
		};
		if(!BPR.PeonsCalled){
			if(BPR.Type==Nat->OilBaseID){
				word PS=FindWPeasant(BPR.x,BPR.y,NI);
				if(PS!=0xFFFF){
					OneObject* OB=Group[PS];
					if(OB){
						if(OB->BuildOnWater(BPR.Index,120))
						PresentProject=false;
					};
				};
			}else{
				word PS=FindPeasant(BPR.x,BPR.y,NI);
				if(PS!=0xFFFF){
					OneObject* OB=Group[PS];
					if(OB){
						if(OB->BuildObj(BPR.Index,120))
						PresentProject=false;
					};
				};
				if(UnitAmount[1]>20){
					PS=FindPeasant(BPR.x,BPR.y,NI);
					if(PS!=0xFFFF){
						OneObject* OB=Group[PS];
						if(OB)OB->BuildObj(BPR.Index,120);
					};
				};
				/*
				if(UnitAmount[1]>15){
					PS=FindPeasant(BPR.x,BPR.y,NI);
					if(PS!=0xFFFF){
						OneObject* OB=Group[PS];
						if(OB)OB->BuildObj(BPR.Index,120);
					};
				};
				if(UnitAmount[1]>25){
					PS=FindPeasant(BPR.x,BPR.y,NI);
					if(PS!=0xFFFF){
						OneObject* OB=Group[PS];
						if(OB)OB->BuildObj(BPR.Index,120);
					};
				};
				*/
			};
		};
	};
};
void Nation::GetTax(byte RC_Type,int RC_Amount){
	if(RC_Type==1)CasheSize+=((RC_Amount*TAX_PERCENT)>>15);
	RESRC[NNUM][RC_Type]+=RC_Amount;
};
bool CINFMOD;
void OutCInf(byte x,byte y){
	if(!CINFMOD)return;
	char gg[128];
	Cell8x8* CL=&TCInf[MyNation][y>>2][x>>2];
	FlyCell* FC=&FlyMap[y>>2][x>>2];
	Xbar(smapx+((x&252)-mapx)*32,smapy+((y&252)-mapy)*32,32*4,32*4,255);
	int ssx=smapx+300;
	int ssy=smapy+300;
	sprintf(gg,"Fly     : %d",FC->NFly);
	ShowString(ssx,ssy,gg,&rlf_s);
	ssy+=16;
	sprintf(gg,"Peasants: %d",CL->Peasants);
	ShowString(ssx,ssy,gg,&rlf_s);
	ssy+=16;
	sprintf(gg,"Towers: %d",CL->Towers);
	ShowString(ssx,ssy,gg,&rlf_s);
	ssy+=16;
	sprintf(gg,"Warriors: %d",CL->Warriors);
	ShowString(ssx,ssy,gg,&rlf_s);
	ssy+=16;
	sprintf(gg,"Farms: %d",CL->Farms);
	ShowString(ssx,ssy,gg,&rlf_s);
	ssy+=16;
	sprintf(gg,"Buildings: %d",CL->Buildings);
	ShowString(ssx,ssy,gg,&rlf_s);
	ssy+=16;
	sprintf(gg,"WPeasasnts: %d",CL->WaterPeas);
	ShowString(ssx,ssy,gg,&rlf_s);
	ssy+=16;
	sprintf(gg,"WObjects: %d",CL->WaterObj);
	ShowString(ssx,ssy,gg,&rlf_s);
	ssy+=16;
	int NB=CL->Neighbor;
	sprintf(gg,"U :%d R :%d D :%d L :%d ",NB&1,NB&4,NB&16,NB&64);
	ShowString(ssx,ssy,gg,&rlf_s);
	ssy+=16;
	sprintf(gg,"UR:%d DR:%d DL:%d UL:%d ",NB&2,NB&8,NB&32,NB&128);
	ShowString(ssx,ssy,gg,&rlf_s);
	ssy+=16;
	sprintf(gg,"U2:%d R2:%d D2:%d L2:%d ",NB&0x100,NB&0x200,NB&0x400,NB&0x800);
	ShowString(ssx,ssy,gg,&rlf_s);
	ssy+=16;
	sprintf(gg,"LOCKS:%d",NLocks[y>>2][x>>2]);
	ShowString(ssx,ssy,gg,&rlf_s);
	ssy+=16;
	sprintf(gg,"PRESENCE:%d",NPresence[y>>2][x>>2]);
	ShowString(ssx,ssy,gg,&rlf_s);
	ssy+=16;
};
void City::HelpMe(word ID){
	for(int i=0;i<32;i++){
		if(ID==EnemyList[i])return;
	};
	for(i=0;i<32;i++){
		if(EnemyList[i]==0xFFFF){
			EnemyList[i]=ID;
			return;
		};
	};
	EnemyList[rando()&31]=ID;
};
word FindObjToAtt(){
	for(int i=0;i<MAXOBJECT;i++){
		OneObject* OB=Group[i];
		if(OB&&!OB->NNUM){
			if(rando()<4096&&(OB->Ref.Visual->info.Basic.AttackRange>15||OB->NIndex==1)){
				return OB->Index;
			};
		};
	};
	return 0xFFFF;
};
void City::HandleFly(){
	for(int i=0;i<MAXOBJECT;i++){
		OneObject* OB=Group[i];
		if(rando()<4096&&OB&&OB->NNUM==NI&&OB->Kind==5&&!OB->LocalOrder){
			word ww=FindObjToAtt();
			if(ww!=0xFFFF)
			OB->AttackObj(ww,0);
		};
	};
};
//water AI
//1. постройка порта:
//    -- найти ближайшее месторождение
//	  -- определить море, на котором находится месторождение
//	  -- найти место на берегу
//	  -- построить порт
bool FindOilSpot(int *x,int *y,int r){
	int x1=(*x)>>1;
	int y1=(*y)>>1;
	int msx0=msx>>1;
	int msy0=msy>>1;
	int d=0;
	if(OILMAP[y1][x1])return true;
	do{
		for(int	t=0;t<d;t++){
			if(x1>0&&y1>1&&x1<=msx0&&y1<=msy0
				&&OILMAP[y1][x1]&&!WLock[y1][x1])goto fina;
			x1++;
		};
		for(t=0;t<d;t++){
			if(x1>0&&y1>1&&x1<=msx0&&y1<=msy0
				&&OILMAP[y1][x1]&&!WLock[y1][x1])goto fina;
			y1++;
		};
		for(t=0;t<d;t++){
			if(x1>0&&y1>1&&x1<=msx0&&y1<=msy0
				&&OILMAP[y1][x1]&&!WLock[y1][x1])goto fina;
			x1--;
		};
		for(t=0;t<d;t++){
			if(x1>0&&y1>1&&x1<=msx0&&y1<=msy0
				&&OILMAP[y1][x1]&&!WLock[y1][x1])goto fina;
			y1--;
		};
		d+=2;
		x1--;
		y1--;
	}while(d<r);
	return false;
fina:
	*x=(x1<<1)-1;
	*y=(y1<<1)-1;
	return true;
};
bool FindPortPlace(Nation* NT,int *xx,int *yy){
	int xc=*xx;
	int yc=*yy;
	int mf=1000000;
	int nf,bf;
	int fx,fy;
	if(FindOilSpot(xx,yy,128)){
		int xo=*xx;
		int yo=*yy;
		byte kw=TZones[yo][xo];
		byte kc=TZones[yo][xo];
		int msx0=msx-4;
		int msy0=msy-4;
		for(int px=3;px<msx0;px++)
			for(int py=3;py<msy0;py++){
				if(TZones[py][px]==kw){
					bf=abs(px-xo)+abs(py-yo);
					nf=bf+abs(px-xc)+abs(py-yc);
					if(nf<mf&&bf>8&&NT->CheckBuilding(NT->PortID,px-1,py-1)){
						//topological checking
						for(int qx=0;qx<5;qx++)
							for(int qy=0;qy<5;qy++){
								if(TZones[py+qy-2][px+qx-2]==kc){
									mf=nf;
									fx=px;
									fy=py;
									goto LAB1;
								};
							};
LAB1:;
					};
				};
			};
		//'ve found!?
		if(mf<1000000){
			*xx=fx-1;
			*yy=fy-1;
			return true;
		}else return false;;
	}else return false;
};
//2. постройка <refinery>
//    -- найти море, на котором находится месторождение,
//		 но нет <refinery>
//	  
