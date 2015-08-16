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
bool FindLEmpty(int* x,int* y,int r);
void GoToTransportLink(OneObject* OBJ);
bool CheckCostPoint(int x,int y,word TZ){
	if(x>0&&y>0&&x<=msx&&y<=msy){
		if((!TrMap[y][x])&&TZones[y][x]==TZ)return true;
		else return false;
	}else return false;
};
bool CheckTransportOnCost(OneObject* OB,word TZ){
	int xx=OB->x;
	int yy=OB->y;
	if( CheckCostPoint(xx+3,yy,TZ)||
		CheckCostPoint(xx,yy+3,TZ)||
		CheckCostPoint(xx-3,yy,TZ)||
		CheckCostPoint(xx,yy-3,TZ)||
		CheckCostPoint(xx+2,yy+2,TZ)||
		CheckCostPoint(xx-2,yy-2,TZ)||
		CheckCostPoint(xx+2,yy-2,TZ)||
		CheckCostPoint(xx-2,yy+2,TZ)
	)return true;
	return false;
};
bool GetTransCoor(OneObject* OB,OneObject* ME,int* x,int* y){
	int bx,by,xx,yy,dd;
	int tz=TZones[ME->y][ME->x];
	for(int i=1;i<4;i++){
		int x0=int(OB->x)-i;
		int y0=int(OB->y)-i;
		int lx=i+i+1;
		int dis=1000;
		for( int j=0;j<lx;j++){
			xx=x0;
			yy=y0+j;
			if((!TrMap[yy][xx])&&TZones[yy][xx]==tz){
				dd=ME->DistTo(xx,yy);
				if(dis>dd){
					dis=dd;
					bx=xx;
					by=yy;
				};
			};
		};
		for(j=0;j<lx;j++){
			xx=x0+lx-1;
			yy=y0+j;
			if((!TrMap[yy][xx])&&TZones[yy][xx]==tz){
				dd=ME->DistTo(xx,yy);
				if(dis>dd){
					dis=dd;
					bx=xx;
					by=yy;
				};
			};
		};
		for(j=1;j<lx-1;j++){
			xx=x0+j;
			yy=y0;
			if((!TrMap[yy][xx])&&TZones[yy][xx]==tz){
				dd=ME->DistTo(xx,yy);
				if(dis>dd){
					dis=dd;
					bx=xx;
					by=yy;
				};
			};
		};
		for(j=1;j<lx-1;j++){
			xx=x0+j;
			yy=y0+lx-1;
			if((!TrMap[yy][xx])&&TZones[yy][xx]==tz){
				dd=ME->DistTo(xx,yy);
				if(dis>dd){
					dis=dd;
					bx=xx;
					by=yy;
				};
			};
		};
		if(dis<1000){
			*x=bx;
			*y=by;
			return true;
		};
	};
	return false;
};
bool OneObject::GoToTransport(word MID,byte Prio){
	if(Media||capBuilding)return false;
	OneObject* OB=Group[MID];
	if(OB->NNUM!=NNUM)return false;
	if(!OB->Transport||NInside>=18)return false;
	Order1* OR1=GetOrdBlock();
	OR1->NextOrder=LocalOrder;
	OR1->DoLink=&GoToTransportLink;
	LocalOrder=OR1;
	OR1->PrioryLevel=Prio&127;
	OR1->OrderType=149;
	OR1->OrderTime=0;
	OR1->info.MoveToObj.ObjIndex=MID;
	OR1->info.MoveToObj.SN=OB->Serial;
	OR1->info.MoveToObj.PrevDist=30000;
	return true;
};
void GoToTransportLink(OneObject* OBJ){
	word MID=OBJ->LocalOrder->info.MoveToObj.ObjIndex;
	word SN=OBJ->LocalOrder->info.MoveToObj.SN;
	OneObject* OB=Group[MID];
	if((!OB)||(OB&&(OB->Serial!=SN||OB->NInside>=18))){
		Order1* OR1=OBJ->LocalOrder;
		OBJ->LocalOrder=OR1->NextOrder;
		OBJ->FreeOrdBlock(OR1);
		return;
	};
	int xx=0;
	int yy=0;
	if(!CheckTransportOnCost(OB,TZones[OBJ->y][OBJ->x])){
		Order1* OR1=OB->LocalOrder;
		if(OR1&&OR1->OrderType==2){
			byte x1=OR1->info.MoveToXY.x;
			byte y1=OR1->info.MoveToXY.y;
			if(OB->DistTo(x1,y1)>OB->DistTo(OBJ->x,OBJ->y))
				OB->SendTo(OBJ->x,OBJ->y,16);
		}else OB->SendTo(OBJ->x,OBJ->y,16);
	}else if(GetTransCoor(OB,OBJ,&xx,&yy)){
		if(OBJ->x==xx&&OBJ->y==yy){
			//'ve come
			OB->Inside[OB->NInside]=OBJ->Index;
			Mops[OBJ->y][OBJ->x]=0xFFFF;
			LLock[OBJ->y][OBJ->x]=0;
			TrMap[OBJ->y][OBJ->x]=0;
			DecLock(OBJ->x,OBJ->y);
			TCInf[OBJ->NNUM][OBJ->y>>2][OBJ->x>>2].UnitsAmount[OBJ->Kind]--;
			OBJ->Absent=true;
			OB->NInside++;
			if(OBJ->InLineCom)OBJ->FreeAsmLink();
			OBJ->ClearOrders();
			OBJ->FreeAsmLink();
			//now inside
			return;
		};
		if(!OBJ->InLineCom)OBJ->CreatePath(xx,yy);
		return;
	};
	//OBJ->LocalOrder->info.MoveToObj.PrevDist=dis;
	//move more
	//if(!OBJ->InLineCom)OBJ->CreatePath(OB->x,OB->y);
};
void UnloadPassangersLink(OneObject* OB);
bool OneObject::UnloadPassangers(byte x ,byte y) {
	if(!(Transport&&NInside))return false;
	if(LLock[y][x])return false;
	Order1* OR1=GetOrdBlock();
	OR1->NextOrder=LocalOrder;
	OR1->DoLink=&GoToTransportLink;
	LocalOrder=OR1;
	OR1->PrioryLevel=16;
	OR1->OrderType=110;
	OR1->OrderTime=0;
	OR1->info.MoveToXY.x=x;
	OR1->info.MoveToXY.y=y;
	OR1->info.MoveToXY.PrevDist=30000;
	OR1->DoLink=&UnloadPassangersLink;
	OR1->info.MoveToXY.Times=0;
	return true;
};
bool WFindTLEmpty(int* x,int* y,int r,ZType z1);
bool WFindTLEmptyDirect(int* x,int* y,ZType z1,int xd,int yd);
void UnloadPassangersLink(OneObject* OBJ){
	int xx=OBJ->LocalOrder->info.MoveToXY.x;
	int yy=OBJ->LocalOrder->info.MoveToXY.y;
	WMap[OBJ->y][OBJ->x]=0;
	if(WMap[yy][xx]){
		if(WFindTLEmptyDirect(&xx,&yy,TZones[OBJ->y][OBJ->x],OBJ->x,OBJ->y)){
			OBJ->LocalOrder->info.MoveToXY.x=xx;
			OBJ->LocalOrder->info.MoveToXY.y=yy;
		}else{
			OBJ->LocalOrder->info.MoveToXY.x=OBJ->x;
			OBJ->LocalOrder->info.MoveToXY.y=OBJ->y;
		};
	};
	xx=OBJ->LocalOrder->info.MoveToXY.x;
	yy=OBJ->LocalOrder->info.MoveToXY.y;
	int dis1=OBJ->LocalOrder->info.MoveToXY.PrevDist;
	int dst=OBJ->DistTo(xx,yy);
	OBJ->LocalOrder->info.MoveToXY.PrevDist=dst;
	if(!dst){
		OBJ->LocalOrder->info.MoveToXY.Times++;
		//if(OBJ->LocalOrder->info.MoveToXY.Times>=8){
			//is there a small piece of land ?
			int x1=OBJ->x;
			int y1=OBJ->y;
			if(FindLEmpty(&x1,&y1,10)){
				int NINS=OBJ->NInside;
				for(int i=0;i<NINS;i++){
					x1=OBJ->x;
					y1=OBJ->y;
					if(FindLEmpty(&x1,&y1,10)){
						OBJ->NInside--;
						word MID=OBJ->Inside[OBJ->NInside];
						OneObject* OB=Group[MID];
						if(OB){
							OB->x=x1;
							OB->y=y1;
							OB->Absent=false;
							LLock[y1][x1]=true;
							IncLock(x1,y1);
							Mops[y1][x1]=OB->Index;
							TCInf[OBJ->NNUM][y1>>2][x1>>2].UnitsAmount[OBJ->Kind]++;
							OB->ClearOrders();
							if(OB->InLineCom)OB->FreeAsmLink();
						};
					};
				};
				Order1* OR1=OBJ->LocalOrder;
				OBJ->LocalOrder=OR1->NextOrder;
				OBJ->FreeOrdBlock(OR1);
				return;
			};
		//}else {
		//	OBJ->LocalOrder->info.MoveToXY.Times++;
		//	OBJ->CreateWPath(OBJ->LocalOrder->info.MoveToXY.x,OBJ->LocalOrder->info.MoveToXY.y);
		//	return;
		//};
	};
	//OBJ->LocalOrder->info.MoveToXY.Times=0;
	OBJ->CreateWPath(OBJ->LocalOrder->info.MoveToXY.x,OBJ->LocalOrder->info.MoveToXY.y);
};
