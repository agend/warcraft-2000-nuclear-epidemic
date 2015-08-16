//---------------Map editor source------------------
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
#include "3DSurf.h"
#define MediaMax 8
#define CompoMax 8
#define RepTileMax 32
bool ChkWTile(int til);
extern byte minimap[maxmap][maxmap];
extern byte tiles[32][8192];
extern byte Locking[1024];
extern int sfVersion;
void NLine(FILE* f);
byte CMap[256][256];
int NMedia;
byte MComp[MediaMax][8];
int NComp;
byte ComSet[CompoMax][2];
word* CTiles[CompoMax*32];
byte MMedia[MediaMax][MediaMax];
byte DefMedia[MediaMax];
void CreateUnit(Nation* NT,byte x,byte y,word ID);
void LoadMediaFile(){
	int x1,x2;
	FILE* f=fopen("tcomp.txt","rt");
	fscanf(f,"%d",&NMedia);
	NLine(f);
	for(int i=0;i<NMedia;i++){
		fscanf(f,"%d",&x1);
		DefMedia[i]=byte(x1);
	};
	NLine(f);
	fscanf(f,"%d",&NComp);
	NLine(f);
	for(i=0;i<NComp;i++){
		fscanf(f,"%d%d",&x1,&x2);
		NLine(f);
		ComSet[i][0]=byte(x1);
		ComSet[i][1]=byte(x2);
		for(int j=0;j<16;j++){
			fscanf(f,"%d",&x1);
			int gg=(i<<4)+j;
			CTiles[gg]=new word[x1+1];
			CTiles[gg][0]=x1;
			for(int k=0;k<x1;k++){
				fscanf(f,"%d",&x2);
				CTiles[gg][k+1]=x2;
			};
		};
		NLine(f);
	};
	fclose(f);
	//creation media composition sets (MComp)
	for(i=0;i<NMedia;i++){
		int MC=0;
		for(int j=0;j<NComp;j++){
			if(ComSet[j][0]==i){
				MC++;
				MComp[i][MC]=ComSet[j][1];
			};
			if(ComSet[j][1]==i){
				MC++;
				MComp[i][MC]=ComSet[j][0];
			};
		};
		MComp[i][0]=MC;
	};
	//Middle media creation
	//MMedia[Media1][Media2]==0xFF-full compartible 
	//MMedia[Media1][Media2]==0xFE-incompartible
	//						< 0xFE -middle media
	for(i=0;i<MediaMax;i++){
		MMedia[i][i]=0xFF;
		for(int j=0;j<i;j++){
			//1. Full compartability checking
			bool fn=true;
			int nmc=MComp[i][0];
			for(int p=1;p<=nmc;p++)if(MComp[i][p]==j){
				MMedia[i][j]=0xFF;
				MMedia[j][i]=0xFF;
				fn=false;
			};
			if(fn){
				byte nmc2=MComp[j][0];
				byte mm1;
				//2. middle compartability
				for(p=1;p<=nmc;p++){
					mm1=MComp[i][p];
					for(int t=1;t<=nmc2;t++){
						if(MComp[j][t]==mm1){
							fn=false;
							goto www;
						};
					};
				};
www:			if(fn){
					MMedia[i][j]=0xFE;
					MMedia[j][i]=0xFE;
				}else{
					MMedia[i][j]=mm1;
					MMedia[j][i]=mm1;
				};
			};
		};
	};
};
void CreateCMap(){
	int NCM=NComp<<4;
	int yy=0;
	for(int x=1;x<=msx;x++)
		for(int y=1;y<=msy;y++){
			byte mt=tmap[x-1][y-1];
			for(int f=0;f<NCM;f++){
				yy=CTiles[f][0];
				for(int g=1;g<=yy;g++){
					if(CTiles[f][g]==mt)goto uuu;
				};
			};
uuu:		if(f<NCM){
				int md=f>>4;
				byte c1=ComSet[md][0];
				byte c2=ComSet[md][1];
				if(f&1)CMap[y-1][x-1]=c2;else CMap[y-1][x-1]=c1;
				if(f&2)CMap[y-1][x]=c2;else CMap[y-1][x]=c1;
				if(f&4)CMap[y][x-1]=c2;else CMap[y][x-1]=c1;
				if(f&8)CMap[y][x]=c2;else CMap[y][x]=c1;
			}else{
				CMap[y-1][x-1]=0;
				CMap[y][x-1]=0;
				CMap[y-1][x]=0;
				CMap[y][x]=0;
			};
		};
};
bool SetBestMedia(int x,int y,int dx,int dy){
	if(x<0||y<0||x>=msx||y>=msy)return false;
	byte fm=CMap[y+dy][x+dx];//fundamental
	byte dm=DefMedia[fm];//default
	byte goodm=0xFF;//final compartible media
	byte cm,mm,pm=fm;//cm-current media, 
						//mm-media compartability,  
						//pm-previous media
	bool Change=false;
	int px,py;
	//1. Checking of the cell validity
	bool CValid=true;
	for(px=0;px<2;px++)
		for(py=0;py<2;py++)if(px||py){
			cm=CMap[y+((dy+py)&1)][x+((dx+px)&1)];
			if(cm!=fm){
				if(fm!=pm&&pm!=cm){
					CValid=false;
					//goto zzz1;
				};
				pm=cm;
			};
			mm=MMedia[fm][cm];
			if(mm!=0xFF)CValid=false;
		};
//zzz:
	if(!CValid){
//zzz1:
		byte Good=0;
		//cell is invalid - need changes
		for(px=0;px<2;px++)
			for(py=0;py<2;py++)if(px||py){
				cm=CMap[y+((dy+py)&1)][x+((dx+px)&1)];
				mm=MMedia[fm][cm];
				if(mm!=0xFF){
					//incompartible media
					if(mm==0xFE&&Good<1){
						Good=1;
						goodm=dm;
					}else{
						if(mm==dm&&Good<4){
							Good=3;
							goodm=mm;
						}else{
							if(Good<3){
								Good=2;
								goodm=dm;
							};
						};
					};
				}else{
					if(cm==dm){
						Good=5;
						goodm=cm;
					}else{
						if(Good<5){
							Good=4;
							goodm=cm;
						};
					};
				};
			};
		if(goodm==0xFF)goodm=dm;
		assert(goodm!=254);
		for(px=0;px<2;px++)
			for(py=0;py<2;py++)if(px||py){
				cm=CMap[y+((dy+py)&1)][x+((dx+px)&1)];
				if(cm!=fm){
					CMap[y+((dy+py)&1)][x+((dx+px)&1)]=goodm;
					Change=true;
				};
		};
	}; 
	return Change;
};
void DefResource(int i,int j);
void SetRAPoint(byte i,byte j);
void SetTiles(int x,int y,int DX){
	int xx=x+DX-1;
	int yy=y+DX-1;
	word til;
	for(int gx=x-1;gx<=xx;gx++)
		for(int gy=y-1;gy<=yy;gy++){
			if(gx>=0&&gy>=0&&gx<msx&&gy<=msy){
				byte m1=CMap[gy][gx];
				byte m2=CMap[gy][gx+1];	
				byte m4=CMap[gy+1][gx];
				byte m8=CMap[gy+1][gx+1];
				byte mx=DefMedia[m1];
				byte mss=1;
				if(m2!=m1)mx=m2;
				else mss|=2;
				if(m4!=m1)mx=m4;
				else mss|=4;
				if(m8!=m1)mx=m8;
				else mss|=8;
				int fx;
				for(int p=0;p<NComp;p++){
					if(ComSet[p][0]==mx&&ComSet[p][1]==m1){
						fx=(p<<4)+mss;
						til=CTiles[fx][1+(int(rando()*int(CTiles[fx][0]))>>15)];
						goto ppp;
					};
					if(ComSet[p][1]==mx&&ComSet[p][0]==m1){
						fx=(p<<4)+(byte(~mss)&15);
						til=CTiles[fx][1+(int(rando()*int(CTiles[fx][0]))>>15)];
						goto ppp;
					};
				};

ppp:			if(!ChkWTile(til))GWALLS.DamageCell(gx+1,gy+1,0,30000);
				tmap[gx][gy]=til;
				DefResource(gx+1,gy+1);
				SetRAPoint(gx+1,gy+1);
				char *wwww=(char*)(&tiles);
				minimap[gy>>1][gx>>1]=wwww[33+/*33*16+1+*/(int(tmap[gx][gy])<<10)];
				word MP=Mops[gy+1][gx+1];
				int xx;
				int yy;
				word UT;
				byte NI;
				if(MP!=0xFFFF){
					OneObject* OB=Group[MP];
					xx=OB->x;
					yy=OB->y;
					UT=OB->NIndex;
					NI=OB->NNUM;
					OB->Eliminate();
				};
				byte LOC=Locking[til];
				byte LL=LLock[gy+1][gx+1];
				if(LOC&1){
					if(!LL){
						LLock[gy+1][gx+1]=1;
						IncLock(gx+1,gy+1);
					};
					TrMap[gy+1][gx+1]=1;
				}else{
					if(LL)DecLock(gx+1,gy+1);
					LLock[gy+1][gx+1]=0;
					TrMap[gy+1][gx+1]=0;
					PutZCell(gx+1,gy+1);
				};
				if(LOC&2){
					WLock[gy+1][gx+1]=1;
					WMap[gy+1][gx+1]=1;
				}else{
					WLock[gy+1][gx+1]=0;
					WMap[gy+1][gx+1]=0;
					//PutZCell(gx+1,gy+1);
				};
				if(MP!=0xFFFF){
					CreateUnit(&NATIONS[NI],xx,yy,UT);
				};
				ClearRender(gx+1,gy+1);
			};
		};
};
void CreateMiniMap();
void PutPoint(int x,int y,int DX,byte Type){
	for(int x1=x;x1<x+DX;x1++)
		for(int y1=y;y1<y+DX;y1++){
			if(y1>=0&&x1>=0&&x1<=msx&&y1<=msy){
				CMap[y1][x1]=Type;
			};
		};
	int xx1=x-1;
	int yy1=y-1;
	int DX1=DX;
	//Modifying nearby nature
	for(int dd=DX-1;dd<DX+4;dd++){
		bool Change=false;
		if(SetBestMedia(xx1,yy1,1,1))Change=true;
		if(SetBestMedia(xx1+DX1,yy1,0,1))Change=true;
		if(SetBestMedia(xx1,yy1+DX1,1,0))Change=true;
		if(SetBestMedia(xx1+DX1,yy1+DX1,0,0))Change=true;
		for(int g=1;g<DX1;g++){
			if(SetBestMedia(xx1+g,yy1,0,1))Change=true;
			if(SetBestMedia(xx1,yy1+g,1,0))Change=true;
			if(SetBestMedia(xx1+g,yy1+DX1,0,0))Change=true;
			if(SetBestMedia(xx1+DX1,yy1+g,0,0))Change=true;
		};
		xx1--;
		yy1--;
		DX1+=2;
		if(!Change)goto qqq;
	};
	
qqq:SetTiles(xx1+2,yy1+2,DX1-2);
	//CreateMiniMap();
	//SetTiles(x-4,y-4,DX+6);
};
void ChkWCell(int x,int y){
	if(x<1||y<1||x>msx||y>msy)return;
	if(CMap[y-1][x-1]!=3||CMap[y-1][x]!=3||CMap[y][x-1]!=3||CMap[y][x]!=3){
		if(CMap[y-1][x-1]==3)PutPoint(x-1,y-1,1,1);
		if(CMap[y][x-1]==3)PutPoint(x-1,y,1,1);
		if(CMap[y-1][x]==3)PutPoint(x,y-1,1,1);
		if(CMap[y][x]==3)PutPoint(x,y,1,1);
	};
};
void WaterCorrection(){
	int mx2=msx>>1;
	int my2=msy>>1;
	for(int dx=0;dx<mx2;dx++)
		for(int dy=0;dy<my2;dy++){
			if(!WMap[dy<<1][dx<<1]){
				ChkWCell((dx<<1)+2,(dy<<1));
				ChkWCell((dx<<1)-2,(dy<<1));
				ChkWCell((dx<<1),(dy<<1)+2);
				ChkWCell((dx<<1),(dy<<1)-2);
			};
		};
};