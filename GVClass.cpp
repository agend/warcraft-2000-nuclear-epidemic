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
void PreDrawGameProcess();
void PostDrawGameProcess();
SimpleDialog* DialogsSystem::addViewPort(int x,int y,int Lx,int Ly){
	for(int i=0;i<512&&DSS[i]!=NULL;i++);
	if(i<512){
		SimpleDialog* SD=new SimpleDialog;
		DSS[i]=SD;
		SD->x=x+BaseX;
		SD->y=y+BaseY;
		SD->x1=SD->x+Lx;
		SD->y1=SD->y+Ly;
		SD->Parent=NULL;
		return SD;
	};
	return NULL;
};
