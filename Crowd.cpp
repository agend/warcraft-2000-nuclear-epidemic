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
#include <math.h>
extern int DHAPROB;
extern int DRECLPROB;
extern int DDIVPROB;
extern int HANSWPROB;
extern int HREQRADIUS;
extern int CROWDMIN;
extern int HCPROB;
extern int CMC1;
extern int CMC2;
extern int CMC3;
word FindEnemyInCell(int x,int y,byte NI,byte Kind){
	int xx=(x<<2);
	int yy=(y<<2);
	OneObject* OB;
	word MID;
	for(int dx=0;dx<4;dx++)
		for(int dy=0;dy<4;dy++){
			MID=Mops[yy+dy][xx+dx];
			if(MID!=0xFFFF){
				OB=Group[MID];
				if(OB&&OB->NNUM==NI&&OB->Kind==Kind){
					Order1* OR1=OB->LocalOrder;
					if(!int(OR1)||(OR1&&OR1->OrderType!=89))return MID;
				};
			};
		};
	return 0xFFFF;
};
word FindEnemy(int x,int y,byte NI){
	int sx=x>>2;
	int sy=y>>2;
	int sx1=sx;
	int sy1=sy;
	int r=3;
	do{
		for(int x1=sx;x1<=sx1;x1++)
			for(int y1=sy;y1<=sy1;y1++){
				if(x1==sx||x1==sx1||y1==sy||y1==sy1){
					Cell8x8* CL=&TCInf[NI][y1][x1];
					if(CL->Towers>0){
						word PS=FindEnemyInCell(x1,y1,NI,1);
						if(PS!=0xFFFF)return PS;
					};
					if(CL->Buildings>0){
						word PS=FindEnemyInCell(x1,y1,NI,4);
						if(PS!=0xFFFF)return PS;
					};
					if(CL->Peasants>0){
						word PS=FindEnemyInCell(x1,y1,NI,0);
						if(PS!=0xFFFF)return PS;
					};
					if(CL->Warriors>0){
						word PS=FindEnemyInCell(x1,y1,NI,2);
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
int Crowd::Funct(int x,int y,int Lx,int Ly){
	if(x<0||x+Lx>63||y<0||y+Ly>63)return 10000000;
	int NL=0;
	for(int dx=0;dx<Lx;dx++)
		for(int dy=0;dy<Ly;dy++)
			NL+=NLocks[y+dy][x+dx];
	NL=CMC1*NL;
	int DX=x+((Lx-BestLx)>>1)-BestX;
	int DY=y+((Ly-BestLy)>>1)-BestY;
	int AM=DX*DX+DY*DY;
	int HH=(DX*LastDx+DY*LastDy);
	int ADDO;
	if(AM!=0&&LastAbs!=0){
		ADDO=div(HH*HH*CMC2,AM*LastAbs).quot;
		if(HH<0)ADDO=-ADDO;
		NL-=ADDO;
	};

	DX=abs(DX);
	DY=abs(DY);
	if(DY>DX)DX=DY;
	NL+=CMC3*abs(DX);
	//NL+=rando()&15;
	NL+=(int(sqrt((x-DestX)*(x-DestX)+(y-DestY)*(y-DestY)))<<3);
	return NL;
};
void Crowd::GetSize(){
	word MID;
	OneObject* OB;
	int minx=64;
	int miny=64;
	int maxx=0;
	int maxy=0;
	NInside=0;
	NMembers=0;
	for(int i=0;i<MLSize;i++){
		MID=MemList[i];
		if(MID!=0xFFFF){
			OB=Group[MID];
			if(OB&&OB->CrowdRef==this){
				int xx=OB->x>>2;
				int yy=OB->y>>2;
				if(xx>maxx)maxx=xx;
				if(yy>maxy)maxy=yy;
				if(xx<minx)minx=xx;
				if(yy<miny)miny=yy;
				//if(xx>=BestX&&xx<BestX1&&yy>=BestY&&yy<BestY1)NInside++;
				if(!OB->LocalOrder)NInside++;
				NMembers++;
			}else MemList[i]=0xFFFF;
		};
	};
	RealX=minx;
	RealY=miny;
	RealLx=maxx-minx+1;
	RealLy=maxy-miny+1;
	RealX1=maxx;
	RealY1=maxy;
};
void Crowd::FindNewPosition(){
	int BLX=int(sqrt(NMembers))+1;
	int BLX4=BLX>>2;
	if(BLX&3)BLX4++;
	int mx=(msx>>2)-BLX4;
	int my=(msy>>2)-BLX4;
	int sum=1000000;
	int BX=0;
	int BY=0;
	for(int i=0;i<=mx;i++)
		for(int j=0;j<my;j++){
			int FN=Funct(i,j,BLX4,BLX4);
			if(FN<sum){
				BX=i;
				BY=j;
				sum=FN;
			};
		};
	LastDx=BX-BestX;
	LastDy=BY-BestY;
	LastAbs=LastDx*LastDx+LastDy*LastDy;
	BestX=BX;
	BestY=BY;
	BestLx=BLX4;
	BestLy=BLX4;
	BestX1=BX+BLX4-1;
	BestY1=BY+BLX4-1;
	FUNC=sum;
};
void Crowd::SendCrowd(){
	int x=(BestX<<2)+(BestLx<<1);
	int y=(BestY<<2)+(BestLy<<1);
	int RNSel=0;
	word Nsel=MLSize;
	word* SMon=MemList;
	if(!Nsel)return;
	word MID;
	for(int k=0;k<Nsel;k++){
		MID=SMon[k];
		if(MID!=0xFFFF&&int(Group[MID])){
			SMon[RNSel]=SMon[k];
			RNSel++;
		};
	};
	Nsel=RNSel;
	MLSize=Nsel;
	if(!Nsel)return;
	int Sqs=int(sqrt(Nsel));
	int Glx=Sqs;
	int Gly=Sqs;
	if(Glx*Gly>Nsel)Glx--;
	if(Glx*Gly<Nsel)Glx++;else{
		if(Glx*Gly>Nsel)Gly--;
		if(Glx*Gly<Nsel)Gly++;
	};
	int gx1=x-(Glx>>1);
	int gy1=y-(Gly>>1);
	if(gx1<=0)gx1=1;
	if(gy1<=0)gy1=1;
	if(gx1+Glx>msx)gx1=msx-Glx+1;
	if(gy1+Gly>msy)gy1=msy-Gly+1;
	int zx=gx1;
	int zy=gy1;
	for(int i=0;i<Nsel;i++){
		if(zx-gx1+1>Glx){zx=gx1;zy++;};
		MID=SMon[i];
		if(MID!=0xFFFF&&int(Group[MID]))
			Group[SMon[i]]->SendTo(zx,zy,0);
		zx++;
	};
};
void Crowd::CreateCrowd(byte N){
	//int Best=BestNat;
	//if(NI==BestNat){
	//	Best=NextBest;
	//};
	//if(Best<0||Best>8)return;
	word MLS[1024];
	word NMem=0;
	word MID;
	NI=N;
	Usage=1;
	OneObject* OB;
	for(int i=1;i<msy;i++){
		for(int j=0;j<msx;j++){
			MID=Mops[i][j];
			if(MID!=0xFFFF){
				OB=Group[MID];
				if(OB&&OB->NNUM==NI&&(!OB->CrowdRef)&&OB->Kind==2&&NMem<1023){
					MLS[NMem]=MID;
					NMem++;
					//OB->CrowdRef=this;
				};
			};
		};
	};
	for(i=0;i<32;i++)ForAtt[i]=0xFFFF;
	if(NMem>CROWDMIN){
		Active=true;
		MLSize=NMem;
		if(MemList)free(MemList);
		MemList=new word[MLSize];
		memcpy(MemList,MLS,MLSize<<1);
		for(i=0;i<NMem;i++){
			MID=MLS[i];
			OB=Group[MID];
			if(OB)OB->CrowdRef=this;
		};
		LastDx=int(rando())&7-3;
		LastDy=int(rando())&7-3;
		GetSize();
		int BLX=int(sqrt(NMembers))+1;
	    int BLX4=BLX>>2;
	    if(BLX&3)BLX4++;
		BestX=RealX+((RealLx-BestLx)>>1);
		BestY=RealY+((RealLy-BestLy)>>1);
		BestLx=BLX4;
		BestLy=BLX4;
		BestX1=BestX+BestLx-1;
		BestY1=BestY+BestLy-1;
		Forces* FR=&NForces[0];
		if(FR->NBuild){
			DestX=(FR->CenterX)>>2;
			DestY=(FR->CenterY)>>2;
		}else{
			DestX=msx>>3;
			DestY=msy>>3;
		};
	};
};
void Crowd::HandleCrowd(){
	if(Active&&!(rando()<HCPROB)){
		GetSize();
		if(!HandleAttacks(1)){
			if(NMembers<2)
				Disband();
			if(NInside+(NInside>>3)>=NMembers){
				FindNewPosition();
				SendCrowd();
			};
		};
	};
};
void Crowd::HandleDefence(){
word MID,MID1;
	OneObject *OB,*US;
	bool ChkFar=false;;
	Forces* FR=&NForces[NI];
	int minx,miny,maxx,maxy,xu,yu,dst;
	if(FR->NBuild){
		minx=FR->MinX;
		miny=FR->MinY;
		maxx=FR->MaxX;
		maxy=FR->MaxY;
		ChkFar=true;
	};
	bool Present=false;
	for(int kk=0;kk<32;kk++)if(ForAtt[kk]!=0xFFFF)return;
	for(kk=0;kk<MLSize;kk++){
		MID=MemList[kk];
		if(MID!=0xFFFF){
			OB=Group[MID];
			if(OB){
				int xx=OB->x>>2;
				int yy=OB->y>>2;
				Cell8x8* CC=&TCInf[NI][yy][xx];
				if(CC->Farms||CC->Buildings||CC->Towers||CC->Peasants)OB->MoveFrom(0);
				if(ChkFar){
					xu=OB->x;
					yu=OB->y;
					if(xu>maxx)xu=maxx;
					if(yu>maxy)yu=maxy;
					if(xu<minx)xu=minx;
					if(yu<miny)yu=miny;
					dst=OB->DistTo(xu,yu);
					if(dst>6&&!OB->LocalOrder)OB->SendTo(xu,yu,1);
					if(dst>15){
						if(!OB->Attack){
							OB->SendTo(xu,yu,1);
						}else{
							if(OB->EnemyDist>12){
							OB->SendTo(xu,yu,129);
							};
						};
					};
				};
			};
		};
	};
};
void Crowd::Disband(){
	word MID;
	OneObject* OB;
	for(int i=0;i<MLSize;i++){
		MID=MemList[i];
		if(MID!=0xFFFF){
			OB=Group[MID];
			if(OB&&OB->CrowdRef==this)OB->CrowdRef=NULL;
		};
		MemList[i]=0xFFFF;
	};
	Active=false;
};
bool Crowd::DisbandHalf(){
	word MID;
	OneObject* OB;
	Nation* NT=&NATIONS[NI];
	int NBusy=NT->NFarms*4;
	if(NBusy>50)NBusy=50;
	int NAtt=NT->NFarms*3;
	if(NBusy+NAtt>MLSize)return false;
	NAtt=MLSize-NBusy;
	for(int i=0;i<NAtt;i++){
		MID=MemList[i];
		if(MID!=0xFFFF){
			OB=Group[MID];
			if(OB&&OB->CrowdRef==this)OB->CrowdRef=NULL;
			MemList[i]=0xFFFF;
		};
	};
	return true;
};
Crowd::Crowd(){
	Active=false;
	for(int i=0;i<32;i++)ForAtt[i]=0xFFFF;
	MemList=NULL;
};
void Crowd::AddToAttackQueue(word ID){
	for(int i=0;i<32;i++){
		if(ID==ForAtt[i])return;
	};
	for(i=0;i<32;i++){
		if(ForAtt[i]==0xFFFF){
			ForAtt[i]=ID;
			return;
		};
	};
	ForAtt[rando()&31]=ID;
};
/*bool Crowd::AddToCrowd(word ID){
	OneObject* OB=Group[ID];
	if(OB->NNUM!=NI)return;
	if(OB->CrowdRef)return;
	for(int i=0;i<MLSize;i++){
		if(MemList[i]==0xFFFF){
			MemList[i]=ID;
			OB->CrowdRef=this;
		};
	};
};*/
bool Crowd::HandleAttacks(byte Prio){
	word MID,MID1;
	OneObject *OB,*US;
	Nation* NATT=&NATIONS[NI];
	bool Present=false;
	int kk=rando()&31;
	byte MyMsk=1<<NI;
	MID=ForAtt[kk];
	if(MID!=0xFFFF){
		OB=Group[MID];
		if(OB){
			kk=FindEnemy(OB->x,OB->y,OB->NNUM);
			if(kk!=0xFFFF)AddToAttackQueue(kk);
		};
	};
	for(int i=0;i<32;i++){
		MID=ForAtt[i];
		if(MID!=0xFFFF){
			OB=Group[MID];
			if(OB&&(!(OB->NMask&MyMsk))&&!OB->Sdoxlo){
				Present=true;
				int mind=2000;
				int cxd=0;
				for(int j=0;j<MLSize;j++){
					MID1=MemList[j];
					if(MID1!=0xFFFF){
						US=Group[MID1];
						if(US){
							if(US->Magic>60&&(!NATT->MagicDelay)&&US->DistTo(OB->x,OB->y)<12){
								bool MagDone=false;
									GeneralObject* UGO=US->Ref.General;
									word ms=1;
									for(int qq=0;qq<UGO->NWeap;qq++){
										if(rando()<8192&&UGO->MagAgainst&ms){
											byte swi=UGO->SWPIndex[qq];
											if(swi!=255){
												SWPAR* SWX=&NATT->SWP[swi];
												if(SWX&&SWX->Enabled&&US->Magic>SWX->MinMagic){
													//Do them all!
													US->ComplexAttack(OB->Index,qq,16);
													MagDone=true;
													NATT->MagicDelay=4;
												};
											};
										};
										ms=ms<<1;
									};
									if(!MagDone)US->AttackObj(MID,Prio);
							}else{
								US->AttackObj(MID,Prio);
							};
							cxd=US->DistTo(OB->x,OB->y);
							if(cxd<mind)mind=cxd;
						};
					};
				};
				if(mind>30)ForAtt[i]=0xFFFF;
			}else ForAtt[i]=0xFFFF;
		};
	};
	return Present;
};