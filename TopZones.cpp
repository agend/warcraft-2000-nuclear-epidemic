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
void CreateRZAMap();
typedef char ZType;
ZType TZones[256][256];
ZType RZAMap[256][256];
const idrx1[8]={0,1,1,1,0,-1,-1,-1};
const idry1[8]={-1,-1,0,1,1,1,0,-1};
void EquZones(ZType id1,ZType id2){
	for(int i=1;i<=msx;i++)
	for(int j=1;j<=msy;j++){
		if(TZones[j][i]==id1)TZones[j][i]=id2;
		if(RZAMap[j][i]==id1)RZAMap[j][i]=id2;
	};
};
void FillTerrainZone(ZType id){
	ZType* zt;
	bool chan;
	do{
		chan=false;
		for(int i=1;i<=msx;i++)
			for(int j=1;j<=msy;j++){
				if(!TrMap[j][i]){
					zt=&TZones[j][i];
					if(*(zt+1)==id||*(zt-1)==id||*(zt+256)==id||*(zt-256)==id||
						*(zt+257)==id||*(zt-257)==id||*(zt+255)==id||*(zt-255)==id)
						if(*zt!=id){
						    *zt=id;
							chan=true;
					};
				};
			};
	}while(chan);
};
void FillWZone(ZType id){
	ZType* zt;
	bool chan;
	do{
		chan=false;
		for(int i=1;i<=msx;i++)
			for(int j=1;j<=msy;j++){
				if(!WMap[j][i]){
					zt=&TZones[j][i];
					if(*(zt+1)==id||*(zt-1)==id||*(zt+256)==id||*(zt-256)==id||
						*(zt+257)==id||*(zt-257)==id||*(zt+255)==id||*(zt-255)==id)
						if(*zt!=id){
							*zt=id;
							chan=true;
					};
				};
			};
	}while(chan);
};
bool FindEmpty(ZType id){
	for(int i=1;i<=msx;i++)
		for(int j=1;j<=msy;j++){
			if((!TrMap[j][i])&&TZones[j][i]==-1){
				TZones[j][i]=id;
				return true;
			};
		};
	return false;
};
bool FindWEmpty(ZType id){
	for(int i=1;i<=msx;i++)
		for(int j=1;j<=msy;j++){
			if((!WMap[j][i])&&TZones[j][i]==-1){
				TZones[j][i]=id;
				return true;
			};
		};
	return false;
};
void CreateTZones(){
	memset(TZones,255,sizeof TZones);
	bool Distributed=false;
	ZType ID=1;
	do{
		if(FindEmpty(ID)){
			FillTerrainZone(ID);
			ID++;
		}else Distributed=true;
	}while(!Distributed);
	Distributed=false;
	do{
		if(FindWEmpty(ID)){
			FillWZone(ID);
			ID++;
		}else Distributed=true;

	}while(!Distributed);
	CreateRZAMap();
};
void SetRAPoint(byte i,byte j){
	ZType* ztp;
	ZType* RZ=&RZAMap[j][i];
	*RZ=-1;
	if(ramap[j][i]||ramap[j+1][i]||ramap[j+1][i+1]||ramap[j][i+1]){
		ztp=(ZType*)(int(&TZones[j][i])-((sizeof ZType)<<9));
		if(*(ztp+1+512)!=-1)  
			*RZ=*(ztp+1+512);
		if(ztp[512-1]!=0xFFFFFFFF)
			*RZ=ztp[512-1];
		if(*(ztp+256+512)!=-1)
			*RZ=*(ztp+256+512);
		if(*(ztp-256+512)!=-1)
			*RZ=*(ztp-256+512);
		if(*(ztp+255+512)!=-1)
			*RZ=*(ztp+255+512);
		if(*(ztp-255+512)!=-1)
			*RZ=*(ztp-255+512);
		if(*(ztp+257+512)!=-1)
			*RZ=*(ztp+257+512);
		if(*(ztp-257+512)!=-1)
			*RZ=*(ztp-257+512);
	};
};
void CreateRZAMap(){
	memset(&RZAMap[0][0],255,sizeof RZAMap);
	for(int i=1;i<=msx;i++)
		for(int j=1;j<=msy;j++)SetRAPoint(i,j);
};
void PutZCell(int x,int y){
	if(x>0&&x<=msx&&y>0&&y<=msy){
		ZType zz1=-1;
		ZType zz2;
		for(int i=0;i<8;i++){
			short idx=idrx1[i]+x;
			short idy=idry1[i]+y;
			zz2=TZones[idy][idx];
			if(zz1!=zz2){
				if(zz1==-1)zz1=zz2;
				if(zz2!=-1&&zz1!=zz2) EquZones(zz2,zz1);
			};
		};
		TZones[y][x]=zz1;
		for(i=0;i<8;i++){
			short idx=idrx1[i];
			short idy=idry1[i];
			SetRAPoint(idx+x,idy+y);
		};
		SetRAPoint(x,y);
	};
};