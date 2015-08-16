#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mgraph.h"
#include "mouse.h"
#include "menu.h"
#include "MapDiscr.h"
class MouseZone{
public:
	int x,y,x1,y1,Index;
	bool Pressed;
	HandlePro* Pro;
	HandlePro* RPro;
	char* Hint;
	MouseZone();
};
MouseZone::MouseZone(){
	Index=-1;
	Hint=NULL;
};
#define NZones 64
MouseZone Zones[NZones];
void InitZones(){
	for(int i=0;i<NZones;i++){
		Zones[i].Index=-1;
		//Zones[i].Hint=NULL;
	};
};
int CreateRZone(int x,int y,int lx,int ly,HandlePro* HPro,HandlePro* RHPro,int Index,char* Hint){
	for(int i=0;i<NZones;i++){
		if(Zones[i].Index==-1)break;
	};
	if(i<NZones){
		MouseZone* Z=&(Zones[i]);
		Z->x=x;
		Z->y=y;
		Z->x1=x+lx-1;
		Z->y1=y+ly-1;
		Z->Pro=HPro;
		Z->RPro=RHPro;
		Z->Index=Index;
		Z->Pressed=false;
		if(int(Z->Hint))
			free(Z->Hint);
		Z->Hint=new char[strlen(Hint)+1];
		strcpy(Z->Hint,Hint);
		return i;
	};
	return -1;
};
int CreateZone(int x,int y,int lx,int ly,HandlePro* HPro,int Index,char* Hint){
	for(int i=0;i<NZones;i++){
		if(Zones[i].Index==-1)break;
	};
	if(i<NZones){
		MouseZone* Z=&(Zones[i]);
		Z->x=x;
		Z->y=y;
		Z->x1=x+lx-1;
		Z->y1=y+ly-1;
		Z->Pro=HPro;
		Z->RPro=NULL;
		Z->Index=Index;
		Z->Pressed=false;
		if(int(Z->Hint))
			free(Z->Hint);
		Z->Hint=new char[strlen(Hint)+1];
		strcpy(Z->Hint,Hint);
		return i;
	};
	return -1;
};
void ControlZones(){
	int i;
	MouseZone* Z;
	if(!Lpressed)
		for(i=0;i<NZones;i++)Zones[i].Pressed=false;
	for(i=0;i<NZones;i++){
		Z=&(Zones[i]);
		if(Z->Index!=-1&&mouseX>=Z->x&&mouseX<=Z->x1&&
			mouseY>=Z->y&&mouseY<=Z->y1)break;
	};
	if(i<NZones){
		if(!Z->Pressed){
			if(Lpressed)Z->Pressed=true;
			if(int(Z->Hint)){
				AssignHint(Z->Hint,3);
			};
			if(Lpressed&&int(Z->Pro))(*Z->Pro)(Z->Index);
			Lpressed=false;
			if(Rpressed&&Z->RPro)(*Z->RPro)(Z->Index);
			Rpressed=false;
		};
	};

};
void DeleteZone(int i){
	if(i<NZones&&i>=0)Zones[i].Index=-1;
};
