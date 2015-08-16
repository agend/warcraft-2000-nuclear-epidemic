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
#include "MapEdit.h"
#include "3DSurf.h"
#include "CWave.h"
#include "CDirSnd.h"
#define MaxSnd 512
extern int WarSound;
extern int WorkSound;
extern int OrderSound;
extern int MidiSound;
static bool SoundOK;
CDirSound* CDS;
char* SoundID[MaxSnd];
word SndTable[MaxSnd][15];
word NSnd[MaxSnd];
word NSounds;
void Errs(LPCSTR s)
{
	MessageBox(hwnd,s,"Sound loading failed...",MB_ICONWARNING|MB_OK);
	assert(false);
};
extern short randoma[8192];
int srpos;
int srando(){
	srpos++;
	srpos&=8191;
	return randoma[srpos];
};
bool IsSound;
void normstr(char* str);
void NLine(FILE* f);
void LoadSounds(char* fn){
	char sss[128];
	char idn[128];
	int srpos=0;
	int nsn,z;
	NSounds=0;
	SoundOK=CDS->DirectSoundOK();
	FILE* f1=fopen(fn,"rt");
	if(!IOresult()){
	  do{
uuux:
		z=fscanf(f1,"%s",idn);
		if(idn[0]=='/'){
			NLine(f1);
			goto uuux;
		};
		z=fscanf(f1,"%d",&nsn);
		NSnd[NSounds]=nsn;
		SoundID[NSounds]=new char[strlen(idn)+1];
		strcpy(SoundID[NSounds],idn);
		if(z!=EOF){
			NLine(f1);
			for(int i=0;i<nsn;i++){
uuu:
				z=fscanf(f1,"%s",sss);
				if(sss[0]=='/'){
					NLine(f1);
					goto uuu;
				};
				normstr(sss);
				if(SoundOK){
					CWave CW(sss);
					int sdid;
					if(CW.WaveOK()){
						sdid=CDS->CreateSoundBuffer(&CW);
						if(CDS->DirectSoundOK()){
							CDS->CopyWaveToBuffer(&CW,sdid);
						}else{
							sprintf(idn,"Could not create sound buffer : %s",sss);
							Errs(idn);
						};
					}else{
						sprintf(idn,"Could not load sound : %s",sss);
						Errs(idn);
					};
					SndTable[NSounds][i]=sdid;
				};
			};
		};
		NSounds++;
	  }while (z!=EOF);
	}else{
		sprintf(sss,"Could not open sounds list : %s",fn);
		Errs(sss);
	};
	//NSounds=0;
	fclose(f1);
	//if(!CDS->DirectSoundOK()){
	//	NSounds=0;
	//};
	
};
void PlayEffect(int n,int pan,int vol){
	if(!SoundOK)return;
	if(n<NSounds){
		if(NSnd[n]>0){
			int maxsnd=NSnd[n];
			int u=maxsnd;
			int nnn=(srando()*maxsnd)>>15;
			bool sndmade=true;
			do{
				int sid=SndTable[n][nnn];
				int poss=CDS->GetPos(sid);
				if(!poss){
					CDS->SetVolume(sid,vol);
					CDS->SetPan(sid,pan);
					CDS->PlaySound(sid);
					sndmade=false;
				}else{
					u--;
					nnn++;
					if(nnn>=maxsnd)nnn=0;
				};
			}while(sndmade&&u);
			if(sndmade&&srando()<2048){
				int nnn=(srando()*maxsnd)>>15;
				CDS->SetVolume(SndTable[n][nnn],vol);
				CDS->SetPan(SndTable[n][nnn],pan);
				CDS->PlaySound(SndTable[n][nnn]);
			};
		};
	};
};
void AddEffect(int x,int y,int id){
	if(!SoundOK)return;
	if(id<0)return;
	int mx0=mapx-8;
	int my0=mapy-8;
	int mx1=mapx+smaplx+16;
	int my1=mapy+smaply+16;
	int maxp=div(8000,smaplx).quot;
	int pan=(x-(smaplx>>1)-mapx)*maxp;
	if(pan<-4000)pan=-4000;
	if(pan>4000)pan=4000;
	//int pan=-9999;
	if(x>=mapx&&y>=mapy&&x<mapx+smaplx&&y<mapy+smaply){
		PlayEffect(id,pan,0);	
	}else{
		if(x>mx0&&y>my0&&x<mx1&&y<my1){
			PlayEffect(id,pan,-800);
		};
	};
};
void AddWarEffect(int x,int y,int id){
	if(!SoundOK)return;
	if(fmap[y][x]<2000)return;
	if(id<0)return;
	int mx0=mapx-8;
	int my0=mapy-8;
	int mx1=mapx+smaplx+16;
	int my1=mapy+smaply+16;
	int maxp=div(8000,smaplx).quot;
	int pan=(x-(smaplx>>1)-mapx)*maxp;
	if(pan<-4000)pan=-4000;
	if(pan>4000)pan=4000;
	//int pan=-9999;
	if(x>=mapx&&y>=mapy&&x<mapx+smaplx&&y<mapy+smaply){
		PlayEffect(id,pan,WarSound);	
	}else{
		if(x>mx0&&y>my0&&x<mx1&&y<my1){
			PlayEffect(id,pan,WarSound-400);
		};
	};
};
void AddWorkEffect(int x,int y,int id){
	if(!SoundOK)return;
	if(fmap[y][x]<2000)return;
	if(id<0)return;
	int mx0=mapx-8;
	int my0=mapy-8;
	int mx1=mapx+smaplx+16;
	int my1=mapy+smaply+16;
	int maxp=div(8000,smaplx).quot;
	int pan=(x-(smaplx>>1)-mapx)*maxp;
	if(pan<-4000)pan=-4000;
	if(pan>4000)pan=4000;
	//int pan=-9999;
	if(x>=mapx&&y>=mapy&&x<mapx+smaplx&&y<mapy+smaply){
		PlayEffect(id,-pan,WorkSound-200);	
	}else{
		if(x>mx0&&y>my0&&x<mx1&&y<my1){
			PlayEffect(id,-pan,WorkSound-600);
		};
	};
};
void AddOrderEffect(int x,int y,int id){
	if(!SoundOK)return;
	if(fmap[y][x]<2000)return;
	if(id<0)return;
	int mx0=mapx-8;
	int my0=mapy-8;
	int mx1=mapx+smaplx+16;
	int my1=mapy+smaply+16;
	int maxp=div(8000,smaplx).quot;
	int pan=(x-(smaplx>>1)-mapx)*maxp;
	if(pan<-4000)pan=-4000;
	if(pan>4000)pan=4000;
	//int pan=-9999;
	if(x>=mapx&&y>=mapy&&x<mapx+smaplx&&y<mapy+smaply){
		PlayEffect(id,-pan,OrderSound);	
	}else{
		if(x>mx0&&y>my0&&x<mx1&&y<my1){
			PlayEffect(id,-pan,OrderSound-400);
		};
	};
};
void AddTEffect(int x,int y,int id){
	if(!SoundOK)return;
	int xx=x;
	if(xx<mapx)xx=mapx+1;
	if(xx>=mapx+smaplx)xx=mapx+smaplx-1;
	AddOrderEffect(xx,mapy+1,id);
};
void CloseSoundSystem(){

};