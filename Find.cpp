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
bool FindLEmpty(int* x,int* y,int r){
	int x1=*x;
	int y1=*y;
	int d=0;
	if(!LLock[y1][x1])return true;
	do{
		for(int	t=0;t<d;t++){
			if(x1>0&&y1>1&&x1<=msx&&y1<=msy
				&&!LLock[y1][x1])goto fina;
			x1++;
		};
		for(t=0;t<d;t++){
			if(x1>0&&y1>1&&x1<=msx&&y1<=msy
				&&!LLock[y1][x1])goto fina;
			y1++;
		};
		for(t=0;t<d;t++){
			if(x1>0&&y1>1&&x1<=msx&&y1<=msy
				&&!LLock[y1][x1])goto fina;
			x1--;
		};
		for(t=0;t<d;t++){
			if(x1>0&&y1>1&&x1<=msx&&y1<=msy
				&&!LLock[y1][x1])goto fina;
			y1--;
		};
		d+=2;
		x1--;
		y1--;
	}while(d<r);
	return false;
fina:
	*x=x1;
	*y=y1;
	return true;
};
bool FindTLEmpty(int* x,int* y,int r,ZType z1){
	int x1=*x;
	int y1=*y;
	int d=0;
	if(!LLock[y1][x1]&&TZones[y1][x1]==z1)return true;

	do{
		for(int	t=0;t<d;t++){
			if(x1>0&&y1>1&&x1<=msx&&y1<=msy
				&&!LLock[y1][x1])goto fina;
			x1++;
		};
		for(t=0;t<d;t++){
			if(x1>0&&y1>1&&x1<=msx&&y1<=msy&&TZones[y1][x1]==z1
				&&!LLock[y1][x1])goto fina;
			y1++;
		};
		for(t=0;t<d;t++){
			if(x1>0&&y1>1&&x1<=msx&&y1<=msy&&TZones[y1][x1]==z1
				&&!LLock[y1][x1])goto fina;
			x1--;
		};
		for(t=0;t<d;t++){
			if(x1>0&&y1>1&&x1<=msx&&y1<=msy&&TZones[y1][x1]==z1
				&&!LLock[y1][x1])goto fina;
			y1--;
		};
		d+=2;
		x1--;
		y1--;
	}while(d<r);
	return false;
fina:
	*x=x1;
	*y=y1;
	return true;
};
bool WFindTLEmpty(int* x,int* y,int r,ZType z1){
	int x1=(*x)&254;
	int y1=(*y)&254;
	int d=0;
	if(!WLock[y1][x1]&&TZones[y1][x1]==z1)return true;

	do{
		for(int	t=0;t<d;t++){
			if(x1>0&&y1>1&&x1<=msx&&y1<=msy
				&&!WMap[y1][x1])goto fina;
			x1+=2;
		};
		for(t=0;t<d;t++){
			if(x1>0&&y1>1&&x1<=msx&&y1<=msy&&TZones[y1][x1]==z1
				&&!WMap[y1][x1])goto fina;
			y1+=2;
		};
		for(t=0;t<d;t++){
			if(x1>0&&y1>1&&x1<=msx&&y1<=msy&&TZones[y1][x1]==z1
				&&!WMap[y1][x1])goto fina;
			x1-=2;
		};
		for(t=0;t<d;t++){
			if(x1>0&&y1>1&&x1<=msx&&y1<=msy&&TZones[y1][x1]==z1
				&&!WMap[y1][x1])goto fina;
			y1-=2;
		};
		d+=2;
		x1-=2;
		y1-=2;
	}while(d<r);
	return false;
fina:
	*x=x1;
	*y=y1;
	return true;
};
bool WFindTLEmptyDirect(int* x,int* y,ZType z1,int xd,int yd){
	int x1=(*x)&254;
	int y1=(*y)&254;
	int x2=xd&254;
	int y2=yd&254;
	do{
		int dx=x2-x1;
		int dy=y2-y1;
		if(dx>1)dx=1;
		if(dy>1)dy=1;
		if(dx<-1)dx=-1;
		if(dy<-1)dy=-1;
		x1+=dx+dx;
		y1+=dy+dy;
		if(!WLock[y1][x1]&&TZones[y1][x1]==z1){
			*x=x1;
			*y=y1;
			return true;
		};
	}while(!(x1==x2&&y1==y2));
	return false;
};