/* Horisontal and vertical lines,rectangles*/
#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mgraph.h"
#include "mouse.h"
#include "menu.h"
#include "MapDiscr.h"
void Hline(int x,int y,int xend,char c){
	if(y<WindY||y>WindY1)return;
	int xr;
	int Lxr;
	if(xend>=x){
		if(x>WindX1)return;
		if(x<WindX)xr=WindX; else xr=x;
		if(xend>WindX1)Lxr=WindX1-xr+1;else Lxr=xend-xr+1;
	}else{
		if(xend>WindX1)return;
		if(xend<WindX)xr=WindX; else xr=xend;
		if(x>WindX1)Lxr=WindX1-xr+1;else Lxr=x-xr+1;
	};
	int scr=int(ScreenPtr)+xr+y*ScrWidth;
	if(Lxr<=0)return;
	__asm{
		mov		edi,scr
		mov		al,c
		mov		ecx,Lxr
		cld
		rep		stosb
	};
};
void Vline(int x,int y,int yend,char c){
	if(x<WindX||x>WindX1)return;
	int yr;
	int Lyr;
	if(yend>=y){
		if(y>WindY1)return;
		if(y<WindY)yr=WindY; else yr=y;
		if(yend<WindY)return;
		if(yend>WindY1)Lyr=WindY1-yr+1;else Lyr=yend-yr+1;
	}else{
		if(yend>WindY1)return;
		if(yend<WindY)yr=WindY; else yr=yend;
		if(y>WindY1)Lyr=WindY1-yr+1;else Lyr=y-yr+1;
	};
	int scr=int(ScreenPtr)+x+yr*ScrWidth;
	if(Lyr<=0)return;
	__asm{
		mov		edi,scr
		mov		al,c
		mov		ecx,Lyr
		mov		edx,ScrWidth
		dec		edx
		cld
uuuuu:	stosb
		add		edi,edx
		loop	uuuuu
	};
};
void Xbar(int x,int y,int lx,int ly,char c){
	Hline(x,y,x+lx-1,c);
	Hline(x,y+ly-1,x+lx-1,c);
	Vline(x,y,y+ly-1,c);
	Vline(x+lx-1,y,y+ly-1,c);
};