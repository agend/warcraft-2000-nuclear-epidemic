//3D surface rendering
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
#include "dialogs.h"
#include <assert.h>
#include <math.h>
#define MediaMax 8
#define CompoMax 8
#define RepTileMax 4
#define MaxStackSize 32768
extern byte tiles[32][8192];
static int StackSize=8192;
short HiMap[256][256];//Height of surface
word RefMap[256][256];//Reference to cell being rendered
byte RXCoor[MaxStackSize];
byte RYCoor[MaxStackSize];
static int Theta=20;
static int Phi=100;
int STHead;
int STTile;
static char* RendSurf=NULL;
void SetupRenderStack(){
	RendSurf=new char [StackSize<<10];
};
void InitRenderStack(){
	memset(RXCoor,0,sizeof RXCoor);
	memset(RYCoor,0,sizeof RYCoor);
	memset(&RefMap[0][0],255,sizeof RefMap);
	STHead=0;
	STTile=0;
};
void ReRenderMap(){
	memset(&RefMap[0][0],255,sizeof RefMap);
	STHead=0;
	STTile=0;
};
void SetStackSize(int STSize){
	StackSize=STSize;
};
short GetHeight(int x,int y){
	int xx=x;
	int yy=y;
	if(xx<0)xx=0;
	if(yy<0)yy=0;
	if(xx>msx)xx=msx;
	if(yy>msy)yy=msy;
	return HiMap[y][x];
};
static int LightDX;
static int LightDY;
static int LightDZ;
void SetLight(int Ldx,int Ldy,int Ldz){
	int Ab=sqrt(Ldx*Ldx+Ldy*Ldy+Ldz*Ldz);
	LightDX=div(Ldx<<8,Ab).quot;
	LightDY=div(Ldy<<8,Ab).quot;
	LightDZ=div(Ldz<<8,Ab).quot;
	memset(RXCoor,0,sizeof RXCoor);
	memset(&RefMap[0][0],255,sizeof RefMap);
};
void SetAngLight(int Theta,int Phi){
	double teta=Theta*3.1415/180;
	double phi=Phi*3.1415/180;
	SetLight(100*sin(teta)*cos(phi),100*sin(teta)*cos(phi),100*cos(teta));
};
void RotateTeta(){
	Theta+=2;
	SetAngLight(Theta,Phi);
};
void RotateTetaI(){
	Theta-=2;
	SetAngLight(Theta,Phi);
};
void RotatePhi(){
	Phi+=2;
	SetAngLight(Theta,Phi);
};
void RotatePhiI(){
	Phi-=2;
	SetAngLight(Theta,Phi);
};
int GetLight(int x,int y){
	int dy=GetHeight(x+1,y)-GetHeight(x-1,y);
	int dx=GetHeight(x,y+1)-GetHeight(x,y-1);
	int lig=16+(div(dx*LightDX+dy*LightDY+64*LightDZ,sqrt(dx*dx+dy*dy+64*64)).quot>>4); 
	if(lig<2)lig=2;
	if(lig>31)lig=31;
	return 32-lig;
};
void ShowSuperFluentFog32(int x,int y,int z1x,int z2x,int z3x,int z4x);
void ShowSuperFluentFog(int x,int y,int z1x,int z2x,int z3x,int z4x);
void ShowSuperFluentFog2_2(int x,int y,int z1x,int z2x,int z3x,int z4x);
void ShowSuperFluentFog2_2_16(int x,int y,int z1x,int z2x,int z3x,int z4x);
char* RenderSquare(int x,int y){
	word rn=RefMap[y][x];
	if(rn==0xFFFF){
		COUNTER++;
		//Need Rendering;
		if(RXCoor[STHead]){
			int xx=RXCoor[STHead];
			int yy=RYCoor[STHead];
			RefMap[yy][xx]=0xFFFF;
		};
		int rPos=int(RendSurf+(STHead<<10));
		int xScrPtr=int(ScreenPtr);
		int SSX=SCRSizeX;
		ScreenPtr=(void*)rPos;
		SCRSizeX=32;
		int tiptr=int(tiles)+(int(tmap[x-1][y-1])<<10);
		__asm{
			push	esi
			push	edi
			cld
			mov		ecx,256
			mov		esi,tiptr
			mov		edi,rPos
			rep		movsd
			pop		edi
			pop		esi
		};
		//ShowSuperFluentFog32(0,0,GetLight(x-1,y-1),
		//						 GetLight(x,y-1),
		//						 GetLight(x-1,y),
		//						 GetLight(x,y));
		ShowSuperFluentFog2_2(0,0,GetLight(x-1,y-1),
								 GetLight(x,y-1),
								 GetLight(x-1,y),
								 GetLight(x,y));
		ScreenPtr=(void*)(xScrPtr);
		SCRSizeX=SSX;
		RXCoor[STHead]=x;
		RYCoor[STHead]=y;
		RefMap[y][x]=STHead;
		STHead++;
		if(STHead>=StackSize)
			STHead=0;
		return (char*)rPos;
	}else return RendSurf+(int(rn)<<10);
};
extern byte mtiles[16][4096];
char* RenderMiniSquare(int x,int y){
	word rn=RefMap[y][x];
	if(rn==0xFFFF){
		//COUNTER++;
		//Need Rendering;
		if(RXCoor[STHead]){
			int xx=RXCoor[STHead];
			int yy=RYCoor[STHead];
			RefMap[yy][xx]=0xFFFF;
		};
		int rPos=int(RendSurf+(STHead<<8));
		int xScrPtr=int(ScreenPtr);
		int SSX=SCRSizeX;
		ScreenPtr=(void*)rPos;
		SCRSizeX=16;
		int tiptr=int(mtiles)+(int(tmap[x-1][y-1])<<8);
		__asm{
			push	esi
			push	edi
			cld
			mov		ecx,64
			mov		esi,tiptr
			mov		edi,rPos
			rep		movsd
			pop		edi
			pop		esi
		};
		//ShowSuperFluentFog32(0,0,GetLight(x-1,y-1),
		//						 GetLight(x,y-1),
		//						 GetLight(x-1,y),
		//						 GetLight(x,y));
		ShowSuperFluentFog2_2_16(0,0,GetLight(x-1,y-1),
								 GetLight(x,y-1),
								 GetLight(x-1,y),
								 GetLight(x,y));
		ScreenPtr=(void*)(xScrPtr);
		SCRSizeX=SSX;
		RXCoor[STHead]=x;
		RYCoor[STHead]=y;
		RefMap[y][x]=STHead;
		STHead++;
		if(STHead>=(StackSize<<1))
			STHead=0;
		return (char*)rPos;
	}else return RendSurf+(int(rn)<<8);
};
void ClearRender(int x,int y){
	if(x>0&&y>0&&x<=msx&&y<=msy){
		word rm=RefMap[y][x];
		if(rm!=0xFFFF)RXCoor[rm]=0;
		RefMap[y][x]=0xFFFF;
	};
};
void AddHeight(int x,int y,int dh){
	if(x>=0&&y>=0&&x<=msx&&y<=msy){
		HiMap[y][x]+=dh;
		ClearRender(x,y);
		ClearRender(x+1,y);
		ClearRender(x,y+1);
		ClearRender(x+1,y+1);
	};
};
void CreateBlob(int x,int y,int h,int r){
	int x0=x-(r<<1);
	int y0=y-(r<<1);
	int x1=x+(r<<1);
	int y1=y+(r<<1);
	int r2=r*r;
	for(int i=x0;i<x1;i++)
		for(int j=y0;j<y1;j++){
			AddHeight(i,j,h*exp(-double((x-i)*(x-i)+(y-j)*(y-j))/r2));
		};
};
void InitRenderMap(){
	InitRenderStack();
	SetLight(-10,-30,30);
	memset(&HiMap[0][0],0,sizeof HiMap);
};
int GetHig(int x,int y){
	return 0;
	int mx=x>>5;
	int my=y>>5;
	int z1=HiMap[my][mx];
	int z2=HiMap[my][mx+1];
	int z3=HiMap[my+1][mx];
	int z4=HiMap[my+1][mx+1];
	int z1a=(z1+z3)>>1;
	int z2a=(z1+z2)>>1;
	int z3a=(z4+z2)>>1;
	int z4a=(z3+z4)>>1;
	int za=(z1a+z2a+z3a+z4a)>>2;
	int dx=(x&31)-16;
	int dy=(y&31)-16;
	return (za+((dx*(z3a-z1a))>>5)+((dy*(z4a-z2a))>>5))>>1;
};