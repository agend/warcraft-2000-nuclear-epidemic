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
#include "mode.h"
void TeleLink(OneObject* OBJ){
	if(!OBJ->Teleport)return;
	for(int i=1;i<3;i++)
		for(int j=1;j<4;j++){
			int yy=OBJ->y+j;
			int xx=OBJ->x+i;
			word MID=Mops[yy][xx];
			if(!(MID==OBJ->Index||MID==0xFFFF)){
				OneObject* OB=Group[MID];
				if(OB&&!OB->capBuilding){
					word mtype=OB->NIndex;
					word N=OB->NNUM;
					//OB->Die();
					OBJ->MakeDamage(0,18,NULL);
					int xd=OBJ->DstX;
					int yd=OBJ->DstY;
					int Lx=1;
					int dx;
					int dy;
					do{
						for(dy=yd;dy<yd+Lx;dy++)
							for(dx=xd;dx<xd+Lx;dx++)
								if(dx>0&&dy>0&&dx<=msx&&dy<=msy&&!LLock[dy][dx])goto telp;
						Lx++;
					}while(Lx<50);
telp:				OB->ClearOrders();
					OB->x=dx;
					OB->y=dy;
					Mops[OB->y][OB->x]=OB->Index;
					LLock[OB->y][OB->x]=1;
					Mops[yy][xx]=0xFFFF;
					CreateExObjD(WPLIST[21],xx<<7,yy<<7,0,0,255,NULL);
				};
			};
			Mops[yy][xx]=OBJ->Index;
			LLock[yy][xx]=0;
			TrMap[yy][xx]=0;
		};
};
void OneObject::SetDstPoint(byte xd,byte yd){
	if(!Ready)return;
	if(!Ref.General->CanDest)return;
	if(x>0&&y>0&&x<=msx&&y<=msy){
		if(xd>=x&&yd>=y&&xd<x+Lx&&yd<y+Ly){
			DstX=0;
			DstY=0;
			Teleport=false;
			return;
		};
		DstX=xd;
		DstY=yd;
		if(capTeleport){
			if(LocalOrder){
				Teleport=true;
				return;
			};
			Teleport=true;
			for(int i=1;i<3;i++)
				for(int j=1;j<4;j++){
					if(Mops[y+j][x+i]==Index){
						//Mops[y+j][x+i]=Index;
						LLock[y+j][x+i]=0;
						TrMap[y+j][x+i]=0;
					};
				};
			Order1* ORD=GetOrdBlock();
			if(ORD){
				LocalOrder=ORD;
				ORD->OrderType=100;
				ORD->DoLink=&TeleLink;
			};
			return;
		};
	};
};
byte DestnX[512];
byte DestnY[512];
int NDestn;
void InitDestn(){
	NDestn=0;
};
void AddDestn(byte x,byte y){
	if(NDestn<512){
		DestnX[NDestn]=x;
		DestnY[NDestn]=y;
		NDestn++;
	};
};
extern RLCTable RCross;
extern RLCTable mRCross;
extern int tmtmt;
void miniShowDestn(){
	if((tmtmt&7)<3)return;
	int smx=mapx+smaplx;
	int smy=mapy+smaply;
	byte x,y;
	for(int i=0;i<NDestn;i++){
		x=DestnX[i];
		y=DestnY[i];
		if(x>=mapx&&y>mapy&&x<smx&&y<smy){
			ShowRLCItem(smapx+(int(x-mapx)<<4),smapy+(int(y-mapy)<<4),&RCross,0,MyNation);
		};
	};
};
void ShowDestn(){
	if(MiniMode){
		miniShowDestn();
		return;
	};
	if((tmtmt&7)<3)return;
	int smx=mapx+smaplx;
	int smy=mapy+smaply;
	byte x,y;
	for(int i=0;i<NDestn;i++){
		x=DestnX[i];
		y=DestnY[i];
		if(x>=mapx&&y>mapy&&x<smx&&y<smy){
			ShowRLCItem(smapx+(int(x-mapx)<<5),smapy+(int(y-mapy)<<5),&RCross,0,MyNation);
		};
	};
};
void ShowMiniDestn(){
	if((tmtmt&7)<5)return;
	int x,y;
	for(int i=0;i<NDestn;i++){
		x=minix+(DestnX[i]>>1);
		y=miniy+(DestnY[i]>>1);
		Hline(x,y,x,clrYello);
		//Vline(x,y-1,y+1,clrYello);
	};
};
void PerformAction(OneObject* OB,int x,int y){
	if(OB->capBuilding)return;
	if(OB->capBuild){
		byte rk=GetResourceKind(x,y);
		if(rk!=0)OB->TakeResource(x,y,rk,16);
		else OB->SendTo(x,y,16);
	}else OB->SendTo(x,y,0);
};