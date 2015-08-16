#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mgraph.h"
#include "mouse.h"
#include "menu.h"
#include "MapDiscr.h"
#include "FlyObj.h"
SelGroup SelSet[80];
SelGroup::SelGroup(){
	NMemb=0;
	Member=NULL;
	SerialN=NULL;
	CanMove=true;
	CanSearchVictim=true;
	CanHelpToFriend=true;
	Egoizm=false;
};
void SelGroup::CreateFromSelection(byte NI){
	if(int(Member))DeleteMembers(NI);
	Member=new word[NSL[NI]];
	SerialN=new word[NSL[NI]];
	word Nsel=NSL[NI];
	memcpy(Member,Selm[NI],Nsel<<1);
	memcpy(SerialN,SerN[NI],Nsel<<1);
	int k;
	word MID;
	word NG1;
	OneObject* OB;
	word* mem1;
	for(k=0;k<Nsel;k++){
		MID=Member[k];
		if(k!=0xFFFF){
			OB=Group[k];
			if(OB){
				//if(OB->GroupIndex){
				//	NG1=OB->GroupIndex->NMemb;
				//	mem1=OB->GroupIndex->Member;
				//	for(int i=0;i<NG1;i++)
				//		if(mem1[i]==MID)mem1[i]=0xFFFF;
				//};
				OB->Egoist=false;
				OB->GroupIndex=NULL;//this;
			};
		};
	};
	NMemb=Nsel;
};
void SelGroup::DeleteMembers(byte NI){
	word MID;
	OneObject* OB;
	word Nsel=NSL[NI];
	word * SMon=Selm[NI];
	for(int k=0;k<Nsel;k++){
		MID=Member[k];
		if(k!=0xFFFF){
			OB=Group[k];
			if(OB){
				OB->Egoist=false;
				OB->GroupIndex=NULL;
			};
		};
	};
	if(int(Member))free(Member);
	if(int(SerialN))free(SerialN);
	SelGroup();
};
void SelGroup::SelectMembers(byte NI){
	word MID;
	OneObject* OB;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	word* ser=SerN[NI];
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(MID!=0xFFFF){
			OB=Group[MID];
			if(int(OB)&&OB->Serial==ser[i]){
				OB->Egoist=false;
				OB->Selected=false;
			};
		};
	};
	word NR=0;
	if(Nsel){
		free(SMon);
		free(ser);
	};
	SMon=new word[NMemb];
	ser=new word[NMemb];
	Selm[NI]=SMon;
	SerN[NI]=ser;
	for(i=0;i<NMemb;i++){
		MID=Member[i];
		if(MID!=0xFFFF){
			SMon[NR]=MID;
			ser[NR]=SerialN[i];
			NR++;
			OB=Group[MID];
			if(int(OB)&&OB->Serial==SerialN[i]){
				OB->Egoist=false;
				OB->Selected=true;
			};
		};
	};
	NSL[NI]=NR;

};
inline bool PxInside(int x,int y,int x1,int y1,int xp,int yp){
	if(xp>=x&&xp<=x1&&yp>=y&&yp<=y1)return true;
	else return false;
};
const sdrx[8]={0,1,1,1,0,-1,-1,-1};
const sdry[8]={-1,-1,0,1,1,1,0,-1};
word GetOwner(int x,int y){
	int xx=x>>7;
	int yy=y>>7;
	for(int xx1=xx-1;xx1<xx+1;xx1++)
		for(int yy1=yy-1;yy1<yy+1;yy1++)
			if(xx1>0&&yy1>0&&xx1<64&&yy1<64){
				FlyCell* FC=&FlyMap[yy1][xx1];
				if(FC->NFly){
					for(int p=0;p<15;p++){
						word MID=FC->FlyList[p];
						if(MID!=0xFFFF){
							OneObject* OB=Group[MID];
							if(OB&&OB->Media==2){
								int xs=(OB->RealX>>4)-60;
								int ys=(OB->RealY>>4)-60+10;
								int xs1=xs+120;
								int ys1=ys+120;
								if(PxInside(xs,ys,xs1,ys1,x,y))
									return MID;
							};
						};
					};
				};
			};
	//search for terrain(on water)monsters
	xx=x>>5;
	yy=y>>5;
	for(xx1=xx-1;xx1<xx+1;xx1++)
		for(int yy1=yy-1;yy1<yy+1;yy1++){
			word MID=Mops[yy1][xx1];
			if(MID!=0xFFFF){
				OneObject* OB=Group[MID];
				int ox=(int(OB->x)<<5)+int(OB->Addx);
				int oy=(int(OB->y)<<5)+int(OB->Addy);
				if(OB->isMoving&&!OB->capBuilding){
					int nsp=OB->CurAnm->count;
					if(nsp>3){
						ox+=div(32*OB->CurrentSprite*sdrx[OB->Direction],nsp).quot;
						oy+=div(32*OB->CurrentSprite*sdry[OB->Direction],nsp).quot;
					};
				};
				//if(PxInside(ox-25,oy-25,ox+(OB->Lx<<5)+25,oy+(OB->Lx<<5)+25,x,y))
				return MID;
			};
		};
	return 0xFFFF;
};
void GetRect(OneObject* ZZ,int* x,int* y,int* Lx);
word GetEnemy(int x,int y,byte NI){
	byte nms=1<<NI;
	int ux,uy,ulx;
	int xx=x>>7;
	int yy=y>>7;
	for(int xx1=xx-1;xx1<=xx+1;xx1++)
		for(int yy1=yy-1;yy1<=yy+1;yy1++)
			if(xx1>0&&yy1>0&&xx1<64&&yy1<64){
				FlyCell* FC=&FlyMap[yy1][xx1];
				if(FC->NFly){
					for(int p=0;p<15;p++){
						word MID=FC->FlyList[p];
						if(MID!=0xFFFF){
							OneObject* OB=Group[MID];
							if(OB&&OB->Media==2&&!(OB->NMask&nms)){
								GetRect(OB,&ux,&uy,&ulx);
								int xs=(OB->RealX>>4)-60;
								int ys=(OB->RealY>>4)-60+10;
								int xs1=xs+120;
								int ys1=ys+120;
								if(PxInside(ux,uy,ux+ulx-1,uy+ulx-1,x,y))
									return MID;
							};
						};
					};
				};
			};
	//search for terrain(on water)monsters
	xx=x>>5;
	yy=y>>5;
	for(xx1=xx-1;xx1<=xx+1;xx1++)
		for(int yy1=yy-1;yy1<=yy+1;yy1++){
			word MID=Mops[yy1][xx1];
			if(MID!=0xFFFF){
				OneObject* OB=Group[MID];
				if(OB&&!(OB->NMask&nms)){
					GetRect(OB,&ux,&uy,&ulx);
					if(PxInside(ux-4,uy-4,ux+ulx+3,uy+ulx+3,x,y))
						return MID;
				};
			};
		};
	return 0xFFFF;
};
word GetFriend(int x,int y,byte NI){
	int ux,uy,ulx;
	int xx=x>>7;
	int yy=y>>7;
	for(int xx1=xx-1;xx1<=xx+1;xx1++)
		for(int yy1=yy-1;yy1<=yy+1;yy1++)
			if(xx1>0&&yy1>0&&xx1<64&&yy1<64){
				FlyCell* FC=&FlyMap[yy1][xx1];
				if(FC->NFly){
					for(int p=0;p<15;p++){
						word MID=FC->FlyList[p];
						if(MID!=0xFFFF){
							OneObject* OB=Group[MID];
							if(OB&&OB->Media==2&&OB->NNUM==NI){
								GetRect(OB,&ux,&uy,&ulx);
								int xs=(OB->RealX>>4)-60;
								int ys=(OB->RealY>>4)-60+10;
								int xs1=xs+120;
								int ys1=ys+120;
								if(PxInside(ux-4,uy+4,ux+ulx+3,uy+ulx+3,x,y))
									return MID;
							};
						};
					};
				};
			};
	//search for terrain(on water)monsters
	xx=x>>5;
	yy=y>>5;
	for(xx1=xx-1;xx1<=xx+1;xx1++)
		for(int yy1=yy-1;yy1<=yy+1;yy1++){
			word MID=Mops[yy1][xx1];
			if(MID!=0xFFFF){
				OneObject* OB=Group[MID];
				if(OB&&OB->NNUM==NI){
					GetRect(OB,&ux,&uy,&ulx);
					if(PxInside(ux,uy,ux+ulx-1,uy+ulx-1,x,y))
						return MID;
				};
			};
		};
	return 0xFFFF;
};