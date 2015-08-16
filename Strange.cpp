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
#include "WeaponID.h"
#include <time.h>
#include "Nucl.h"
#include "TopZones.h"
#include "Megapolis.h"
#include "3DSurf.h"
#include "MapEdit.h"
const wdrx[8]={0,1,1,1,0,-1,-1,-1};
const wdry[8]={-1,-1,0,1,1,1,0,-1};
extern short TSin[257];
extern short TCos[257];
extern short TAtg[257];
extern int NUCLUSE[4];
word GetOwner(int x,int y);
#include "Strange.h"

StrangeWeapon::StrangeWeapon(){
	Enabled=false;
};
word CheckFriend(byte NI,int x,int y){
	if(x<1||x>=msx||y<1||y>=msy)return 0xFFFF;
	word MID=GetOwner((x<<5)+16,(y<<5)+16);
	if(MID!=0xFFFF){
		OneObject* OB=Group[MID];
		if(OB)return MID;
		if(NI==OB->NNUM)return MID;
	};
	return 0xFFFF;
};
word FindFriend(byte NI,int x,int y){
	word MID=CheckFriend(NI,x,y);
	if(MID!=0xFFFF)return MID;
	MID=CheckFriend(NI,x+1,y);
	if(MID!=0xFFFF)return MID;
	MID=CheckFriend(NI,x-1,y);
	if(MID!=0xFFFF)return MID;
	MID=CheckFriend(NI,x,y-1);
	if(MID!=0xFFFF)return MID;
	MID=CheckFriend(NI,x,y+1);
	if(MID!=0xFFFF)return MID;
	MID=CheckFriend(NI,x-1,y-1);
	if(MID!=0xFFFF)return MID;
	MID=CheckFriend(NI,x+1,y-1);
	if(MID!=0xFFFF)return MID;
	MID=CheckFriend(NI,x-1,y+1);
	if(MID!=0xFFFF)return MID;
	MID=CheckFriend(NI,x+1,y+1);
	if(MID!=0xFFFF)return MID;
	return 0xFFFF;
};
void MakeExpl(int xx,int yy);
void MakeFog(int xx,int yy){
	//if(rando()<2048){
		//CreateExObjD(WPLIST[ID_FIREX],
		//	xx,yy,rando()&255,0,0,NULL);
	//	CreateExObjD(WPLIST[ID_FIREX],
	//		xx+12,yy,rando()&255,0,0,NULL);
	//};
	if(rando()<512)
		CreateExObjD(WPLIST[ID_FOG1],
			xx,yy-50,64,4,0,NULL);
		else 
		CreateExObjD(WPLIST[ID_FOG],
			xx,yy-60,64,4,0,NULL);

};
void MakeRFog(int xx,int yy){
	//if(rando()<2048){
		//CreateExObjD(WPLIST[ID_FIREX],
		//	xx,yy,rando()&255,0,0,NULL);
	//	CreateExObjD(WPLIST[ID_FIREX],
	//		xx+12,yy,rando()&255,0,0,NULL);
	//};
	if(rando()<5000)
		CreateExObjD(WPLIST[ID_FOG1],
			xx,yy-50,64,4,0,NULL);
		else 
		CreateExObjD(WPLIST[ID_FOG],
			xx,yy-60,64,4,0,NULL);

};

void MakeExpl(int xx,int yy){
	if(rando()<512)
		CreateExObjD(WPLIST[ID_EXPL1],
			xx,yy-50,64,4,0,NULL);
		else 
		CreateExObjD(WPLIST[ID_EXPL2],
			xx,yy-60,64,4,0,NULL);

};
void HandleHugeBomb(StrangeWeapon* SW);
void StrangeWeapon::CreateHugeBomb(int ex,int ey,int er1,int er2,int time,word MagID){
	OneObject* OB=Group[MagID];
	Nation* NT=OB->Nat;
	if(OB->Magic<NT->WMagic[0])return;
	OB->Magic-=NT->WMagic[0];
	x=ex<<7;
	y=ey<<7;
	OwnerID=MagID;
	if(OB)OwnerSerial=OB->Serial;
	HugeBomb.Radius1=er1;
	HugeBomb.Radius2=er2;
	HugeBomb.NR1=5;
	HugeBomb.Radius1=er1;
	HugeBomb.Radius2=er2;
	HugeBomb.r1=32;
	HugeBomb.r2=64;
	HugeBomb.vr1=div(er1-32,time).quot;
	HugeBomb.vr2=div(er2-64,time).quot;
	Enabled=true;
	SWH=&HandleHugeBomb;
};
void HandleHugeBomb(StrangeWeapon* SW){
	if(SW->Enabled){
		OneObject* OB=Group[SW->OwnerID];
		for(int j=0;j<0;j++){
			byte angl=rando()&255;
			int rad=rando()&511;
			int xx=SW->x+((rad*TSin[angl])>>8)+90;
			int yy=SW->y+((rad*TCos[angl])>>8)+90;
			byte xxx=byte(xx>>7);
			byte yyy=byte(xx>>7);
			word MID=Mops[yyy][xxx];
			if(MID!=0xFFFF&&int(Group[MID]))
				Group[MID]->MakeDamage(0,3,OB);
			if(rando()<8192)MakeFog(xx,yy);
			//	CreateExObjD(WPLIST[NUCLUSE[rando()&3]],xx,yy,angl,8,255,NULL);
			SW->HugeBomb.r1++;
		};
		int rx=SW->HugeBomb.r2;
		if(rx>30)rx=50;
		for(j=0;j<rx;j++){
			byte angl=rando()&255;
			int rr=SW->HugeBomb.r2+(rando()&127);
			int xx=SW->x+((rr*TSin[angl])>>8)+90;
			int yy=SW->y+((rr*TCos[angl])>>8)+90;
			byte xxx=byte(xx>>7);
			byte yyy=byte(yy>>7);
			word MID=Mops[yyy][xxx];
			if(MID!=0xFFFF&&int(Group[MID]))
				Group[MID]->MakeDamage(0,3,OB);
			if(rando()<8192)MakeFog(xx,yy);
			//CreateExObjD(WPLIST[NUCLUSE[rando()&3]],xx,yy,angl,16,255,NULL);
			SW->HugeBomb.r2++;
		};
		if(SW->HugeBomb.r2>SW->HugeBomb.Radius2)SW->Enabled=false;
	};
};
//SEA OF FIRE: 1
void HandleFireSea(StrangeWeapon* SW);
void StrangeWeapon::CreateFireSea(int ex,int ey,word MagID){
	OneObject* OB=Group[MagID];
	Nation* NT=OB->Nat;
	if(OB->Magic<NT->WMagic[1])return;
	OB->Magic-=NT->WMagic[1];
	Enabled=true;
	x=ex<<7;
	y=ey<<7;
	OwnerID=MagID;
	if(OB)OwnerSerial=OB->Serial;
	else{
		Enabled=false;
		return;
	};
	HugeBomb.r1=100;
	MagAmount=10;
	MagSpeed=1;
	MagMaxConsume=NT->WMagic[1];
	MagConsume=0;
	NNUM=OB->NNUM;
	SWH=&HandleFireSea;
};
void HandleFireSea(StrangeWeapon* SW){
	if(SW->Enabled){
		OneObject* OB=Group[SW->OwnerID];
		if(!OB||(OB&&
		(OB->Serial!=SW->OwnerSerial||OB->Magic<SW->MagSpeed))){
			SW->MagSpeed=0;
			return;
		};
		SW->MagConsume+=SW->MagSpeed;
		//OB->Magic-=SW->MagSpeed;
		if(SW->MagConsume>=SW->MagMaxConsume)SW->MagSpeed=0;
		if(SW->HugeBomb.r1)SW->HugeBomb.r1--;
		else SW->Enabled=false;
		//Body of graphic routine
		for(int i=0;i<8;i++){
			int xx=SW->x+(rando()>>6)-256;
			int yy=SW->y+(rando()>>6)-256;
			if(SW->MagSpeed){
				if(rando()<6000)
					CreateExObjD(WPLIST[ID_FIRE1],
					xx,yy,rando()&255,16,0,NULL);
				else CreateExObjD(WPLIST[ID_FIRE],
					xx,yy,rando()&255,16,0,NULL);
			};
			word MID=Mops[yy>>7][xx>>7];
			if(MID!=0xFFFF){
				OneObject* OB1=Group[MID];
				if(OB1){
					if(rando()>8000)OB1->MakeDamage(0,2,OB);
				};
			};
			MakeFog(xx,yy);
		};
		//end of graphic routine
	};
};

//END SEA OF FIRE: 1
//BLIZZARD: 2
void HandleBlizzard(StrangeWeapon* SW);
void StrangeWeapon::CreateBlizzard(int ex,int ey,word MagID){
	OneObject* OB=Group[MagID];
	Nation* NT=OB->Nat;
	if(OB->Magic<NT->WMagic[2])return;
	OB->Magic-=NT->WMagic[2];
	Enabled=true;
	x=ex<<7;
	y=ey<<7;
	OwnerID=MagID;
	if(OB)OwnerSerial=OB->Serial;
	else{
		Enabled=false;
		return;
	};
	MagAmount=10;
	MagSpeed=1;
	MagMaxConsume=NT->WMagic[2];
	MagConsume=0;
	NNUM=OB->NNUM;
	SWH=&HandleBlizzard;
};
void HandleBlizzard(StrangeWeapon* SW){
	if(SW->Enabled){
		OneObject* OB=Group[SW->OwnerID];
		if(!OB||(OB&&
		(OB->Serial!=SW->OwnerSerial||OB->Magic<SW->MagSpeed))){
			SW->Enabled=false;
			return;
		};
		SW->MagConsume+=SW->MagSpeed;
		//OB->Magic-=SW->MagSpeed;
		if(SW->MagConsume>=SW->MagMaxConsume)SW->Enabled=false;
		//Body of graphic routine
		for(int i=0;i<8;i++){
			int xx=SW->x+(rando()>>6)-256;
			int yy=SW->y+(rando()>>6)-256;
			CreateExObjD(WPLIST[ID_GLASSFLY],
				xx-100-100,yy-500-100,50,64,255,NULL);
		};
		//end of graphic routine
	};
};

//END BLIZZARD: 2
//MAGIC EXPLOSION: 3
void HandleMExplosion(StrangeWeapon* SW);
void StrangeWeapon::CreateMExplosion(int ex,int ey,word MagID){
	OneObject* OB=Group[MagID];
	Nation* NT=OB->Nat;
	if(OB->Magic<NT->WMagic[3])return;
	Enabled=true;
	x=ex<<7;
	y=ey<<7;
	OwnerID=MagID;
	if(OB)OwnerSerial=OB->Serial;
	else{
		Enabled=false;
		return;
	};
	MagAmount=10;
	MagSpeed=1;
	MagMaxConsume=100;
	MagConsume=0;
	NNUM=OB->NNUM;
	MExp.r=1;
	MExp.r1=300;
	SWH=&HandleMExplosion;
};
void HandleMExplosion(StrangeWeapon* SW){
	if(SW->Enabled){
		OneObject* OB=Group[SW->OwnerID];
		if(!OB||(OB&&
		(OB->Serial!=SW->OwnerSerial||OB->Magic<SW->MagSpeed))){
			SW->Enabled=false;
			return;
		};
		SW->MagConsume+=SW->MagSpeed;
		OB->Magic-=rando()&1;
		if(SW->MagConsume>=SW->MagMaxConsume)SW->Enabled=false;
		//Body of graphic routine
		int rr=SW->MExp.r;
		SW->MExp.r+=8;
		int n=(rr>>3)+4;
		for(int i=0;i<n;i++){
			byte angl=rando()&255;
			int xx=SW->x+((rr*TSin[angl])>>8);
			int yy=SW->y+((rr*TCos[angl])>>8);
			word MID=Mops[yy>>7][xx>>7];
			if(MID!=0xFFFF){
				OneObject* OBX=Group[MID];
				if(OBX){
					if(OBX->NNUM==OB->NNUM){
						if(OBX->xForce>=32&&OBX->MTime<980){
							OBX->xForce=48;
							OBX->MTime=1000;
						}else if(OBX->xForce>=48&&OBX->MTime<980){
							OBX->xForce=64;
							OBX->MTime=1000;
						}else{
							if(OBX->xForce>16){
								OBX->MTime=1000;
							}else{;
								OBX->xForce=32;
								OBX->MTime=1000;
							};
						};
					};
				};
			};
			//byte xxx=byte(xx>>7);
			//byte yyy=byte(yy>>7);
			CreateExObjD(WPLIST[ID_MAGEXP],
				xx,yy,angl,16,255,NULL);
		};
		//end of graphic routine
	};
};

//END MAGIC EXPLOSION : 3
//MAGIC EXPLOSION 1 : 4
void HandleMExplosion1(StrangeWeapon* SW);
void StrangeWeapon::CreateMExplosion1(int ex,int ey,word MagID){
	OneObject* OB=Group[MagID];
	Nation* NT=OB->Nat;
	if(OB->Magic<NT->WMagic[4])return;
	Enabled=true;
	x=ex<<7;
	y=ey<<7;
	OwnerID=MagID;
	if(OB)OwnerSerial=OB->Serial;
	else{
		Enabled=false;
		return;
	};
	MagAmount=10;
	MagSpeed=1;
	MagMaxConsume=100;
	MagConsume=0;
	NNUM=OB->NNUM;
	MExp.r=20;
	MExp.r1=0;
	SWH=&HandleMExplosion1;
};
void HandleMExplosion1(StrangeWeapon* SW){
	if(SW->Enabled){
		OneObject* OB=Group[SW->OwnerID];
		if(!OB||(OB&&
		(OB->Serial!=SW->OwnerSerial||OB->Magic<SW->MagSpeed))){
			SW->Enabled=false;
			return;
		};
		SW->MagConsume+=SW->MagSpeed;
		OB->Magic-=rando()&1;
		if(SW->MagConsume>=SW->MagMaxConsume)SW->Enabled=false;
		//Body of graphic routine
		int rr=SW->MExp.r;
		SW->MExp.r+=8;
		SW->MExp.r1+=2;
		//int n=(rr>>3)+4;
		for(int i=0;i<20;i++){
			byte angl=(((rando()&7)<<5)+SW->MExp.r1)&255;
			rr-=8;
			int xx=SW->x+((rr*TSin[angl])>>8);
			int yy=SW->y+((rr*TCos[angl])>>8);
			//byte xxx=byte(xx>>7);
			//byte yyy=byte(yy>>7);
			word MID=Mops[yy>>7][xx>>7];
			if(MID!=0xFFFF){
				OneObject* OBX=Group[MID];
				if(OBX){
					if(OBX->NNUM!=OB->NNUM){
						OBX->xForce=8;
						OBX->MTime=300;
					};
				};
			};
			CreateExObjD(WPLIST[ID_MAGEXP],
				xx,yy,angl,0,255,NULL);
		};
		//end of graphic routine
	};
};

//END MAGIC EXPLOSION1 : 4
//MAGIC FIREWALL : 5
void HandleFireWall(StrangeWeapon* SW);
void StrangeWeapon::CreateFireWall(int ex,int ey,word MagID){
	OneObject* OB=Group[MagID];
	Nation* NT=OB->Nat;
	if(OB->Magic<NT->WMagic[5])return;
	word MD=CheckFriend(NNUM,ex,ey);
	if(MD==0xFFFF)return; 
	OneObject* POB=Group[MD];
	if(POB){
		HugeBomb.PID=MD;
		HugeBomb.PSerialID=POB->Serial;
	}else return;
	Enabled=true;
	x=ex<<7;
	y=ey<<7;
	OwnerID=MagID;
	if(OB)OwnerSerial=OB->Serial;
	else{
		Enabled=false;
		return;
	};
	MagAmount=10;
	MagSpeed=1;
	MagMaxConsume=300;
	MagConsume=0;
	NNUM=OB->NNUM;
	MExp.r=1;
	MExp.r1=300;
	SWH=&HandleFireWall;
};
void HandleFireWall(StrangeWeapon* SW){
	if(SW->Enabled){
		OneObject* OB=Group[SW->OwnerID];
		if(!OB||(OB&&
		(OB->Serial!=SW->OwnerSerial||OB->Magic<SW->MagSpeed))){
			SW->Enabled=false;
			return;
		};
		OneObject* POB=Group[SW->HugeBomb.PID];
		if(!POB||(POB&&SW->HugeBomb.PSerialID!=POB->Serial)){
			SW->Enabled=false;
			return;
		};
		SW->MagConsume+=SW->MagSpeed;
		if(rando()<8000)OB->Magic-=rando()&1;
		if(SW->MagConsume>=SW->MagMaxConsume)SW->Enabled=false;
		//Body of graphic routine
		int rr=400;
		//SW->MExp.r+=8;
		//int n=(rr>>5)+4;
		int mx=(int(POB->x)<<5)+POB->Addx;
		int my=(int(POB->y)<<5)+POB->Addy;
		if(POB->isMoving){
			int nsp=POB->CurAnm->count;
			if(nsp>3){
				mx+=div(34*POB->CurrentSprite*wdrx[POB->Direction],nsp).quot;
				my+=div(32*POB->CurrentSprite*wdry[POB->Direction],nsp).quot;
			};
		};
		mx=mx<<2;
		my=my<<2;
		for(int i=0;i<2;i++){
			byte angl=rando()&255;
			int xx=mx+((rr*TSin[angl])>>8);
			int yy=my+((rr*TCos[angl])>>8);
			//byte xxx=byte(xx>>7);
			//byte yyy=byte(yy>>7);
			word MID=Mops[yy>>7][xx>>7];
			if(MID!=0xFFFF){
				OneObject* OB1=Group[MID];
				if(OB1&&!(OB1->NMask&POB->NMask)){
					OB1->MakeDamage(0,8,OB);
				};
			};
			MakeFog(xx,yy);
		};
		//end of graphic routine
	};
};
//END FIREWALL : 6
//SUPER VISION :7
void HandleSuperVision(StrangeWeapon* SW);
void StrangeWeapon::CreateSuperVision(int ex,int ey,word MagID){
	OneObject* OB=Group[MagID];
	Nation* NT=OB->Nat;
	if(OB->Magic<NT->WMagic[7])return;
	Enabled=true;
	x=ex<<7;
	y=ey<<7;
	OwnerID=MagID;
	if(OB)OwnerSerial=OB->Serial;
	else{
		Enabled=false;
		return;
	};
	MagAmount=10;
	MagSpeed=1;
	MagMaxConsume=10000;
	MagConsume=0;
	NNUM=OB->NNUM;
	MExp.r=1;
	MExp.r1=300;
	SWH=&HandleSuperVision;
};
void HandleSuperVision(StrangeWeapon* SW){
	if(SW->Enabled){
		OneObject* OB=Group[SW->OwnerID];
		if(!OB||(OB&&
		(OB->Serial!=SW->OwnerSerial||OB->Magic<SW->MagSpeed))){
			SW->Enabled=false;
			return;
		};
		SW->MagConsume+=SW->MagSpeed;
		if(!(rando()&7))OB->Magic-=rando()&1;
		if(SW->MagConsume>=SW->MagMaxConsume)SW->Enabled=false;
		//Body of graphic routine
		for(int i=0;i<16;i++){
			int xx=(SW->x>>7);
			int yy=(SW->y>>7);
			int dx=-16+(rando()&31);
			int dy=-16+(rando()&31);
			if(SW->NNUM==MyNation&&dx*dx+dy*dy<256)
				for(int fx=-2;fx<2;fx++)
					for(int fy=-2;fy<2;fy++){
						int hx=xx+dx+fx;
						int hy=yy+dy+fy;
						if(hx>1&&hx<msx&&hy>1&&hy<msy){
							if(fmap[hy][hx]<3800)fmap[hy][hx]+=600;
						};
					};
		};
		
		//end of graphic routine
	};
};
//END SUPER VISION : 7
void HandleFlyUP(StrangeWeapon* SW);
void StrangeWeapon::CreateFlyUP(int ex,int ey,int er1,int er2,int time,word MagID){
	OneObject* OB=Group[MagID];
	Nation* NT=OB->Nat;
	if(OB->Magic<NT->WMagic[8])return;
	OB->Magic-=NT->WMagic[9];
	x=ex<<7;
	y=ey<<7;
	OwnerID=MagID;
	if(OB)OwnerSerial=OB->Serial;
	HugeBomb.Radius1=er1;
	HugeBomb.Radius2=er2;
	HugeBomb.NR1=5;
	HugeBomb.Radius1=er1;
	HugeBomb.Radius2=er2;
	HugeBomb.r1=32;
	HugeBomb.r2=64;
	HugeBomb.vr1=div(er1-32,time).quot;
	HugeBomb.vr2=div(er2-64,time).quot;
	Enabled=true;
	SWH=&HandleFlyUP;
};
void HandleFlyUP(StrangeWeapon* SW){
	if(SW->Enabled){
		OneObject* OB=Group[SW->OwnerID];
		for(int j=0;j<0;j++){
			byte angl=rando()&255;
			int rad=rando()&511;
			int xx=SW->x+((rad*TSin[angl])>>8);
			int yy=SW->y+((rad*TCos[angl])>>8);
			byte xxx=byte(xx>>7);
			byte yyy=byte(xx>>7);
			word MID=Mops[yyy][xxx];
			if(MID!=0xFFFF){
				OneObject* OB=Group[MID];
				if(OB)OB->MakeMeFly(SW->OwnerID);
			};
			MakeFog(xx,yy);
			SW->HugeBomb.r1++;
		};
		int rx=SW->HugeBomb.r2;
		if(rx>30)rx=50;
		for(j=0;j<rx;j++){
			byte angl=rando()&255;
			int rr=SW->HugeBomb.r2+(rando()&127);
			int xx=SW->x+((rr*TSin[angl])>>8);
			int yy=SW->y+((rr*TCos[angl])>>8);
			byte xxx=byte(xx>>7);
			byte yyy=byte(yy>>7);
			word MID=Mops[yyy][xxx];
			if(MID!=0xFFFF){
				OneObject* OB=Group[MID];
				if(OB)OB->MakeMeFly(SW->OwnerID);
			};
			MakeFog(xx,yy);
			SW->HugeBomb.r2++;
		};
		if(SW->HugeBomb.r2>SW->HugeBomb.Radius2)SW->Enabled=false;
	};
};
void HandleNucl(StrangeWeapon* SW);
void StrangeWeapon::CreateNucl(int ex,int ey,int er1,int er2,int time){
	x=ex<<7;
	y=ey<<7;
	HugeBomb.Radius1=er1;
	HugeBomb.Radius2=er2;
	HugeBomb.NR1=5;
	HugeBomb.Radius1=er1;
	HugeBomb.Radius2=er2;
	HugeBomb.r1=32;
	HugeBomb.r2=64;
	HugeBomb.vr1=div(er1-32,time).quot;
	HugeBomb.vr2=div(er2-64,time).quot;
	Enabled=true;
	SWH=&HandleNucl;
};
void HandleNucl(StrangeWeapon* SW){
	if(SW->Enabled){
		OneObject* OB=Group[SW->OwnerID];
		for(int j=0;j<0;j++){
			byte angl=rando()&255;
			int rad=rando()&511;
			int xx=SW->x+((rad*TSin[angl])>>8);
			int yy=SW->y+((rad*TCos[angl])>>8);
			byte xxx=byte(xx>>7);
			byte yyy=byte(xx>>7);
			word MID=Mops[yyy][xxx];
			if(MID!=0xFFFF&&int(Group[MID]))
				Group[MID]->Die();
			MakeExpl(xx,yy);
			//MakeRFog(xx,yy);
			if(rando()<2048){
				PutPoint(xxx,yyy,1,5);
			};
			if(rando()<3024){
				CreateBlob(xxx,yyy,-45-(rando()&7),2);
			};
			//	CreateExObjD(WPLIST[NUCLUSE[rando()&3]],xx,yy,angl,8,255,NULL);
			SW->HugeBomb.r1++;
		};
		int rx=SW->HugeBomb.r2;
		if(rx>30)rx=50;
		for(j=0;j<rx;j++){
			byte angl=rando()&255;
			int rr=SW->HugeBomb.r2+(rando()&127);
			int xx=SW->x+((rr*TSin[angl])>>8);
			int yy=SW->y+((rr*TCos[angl])>>8);
			byte xxx=byte(xx>>7);
			byte yyy=byte(yy>>7);
			word MID=Mops[yyy][xxx];
			if(MID!=0xFFFF&&int(Group[MID]))
				Group[MID]->Die();
			MakeExpl(xx,yy);
			//MakeFog(xx,yy);
			if(rando()<1900){
				PutPoint(xxx,yyy,1,5);
			};
			if(rando()<1024){
				CreateBlob(xxx,yyy,-45-(rando()&7),2);
			};
			//CreateExObjD(WPLIST[NUCLUSE[rando()&3]],xx,yy,angl,16,255,NULL);
			SW->HugeBomb.r2++;
		};
		if(SW->HugeBomb.r2>SW->HugeBomb.Radius2)SW->Enabled=false;
	};
};
void HandleFog(StrangeWeapon* SW);
void StrangeWeapon::CreateFog(int ex,int ey,int er1,int er2,int time){
	x=ex<<7;
	y=ey<<7;
	HugeBomb.Radius1=er1;
	HugeBomb.Radius2=er2;
	HugeBomb.NR1=5;
	HugeBomb.Radius1=er1;
	HugeBomb.Radius2=er2;
	HugeBomb.r1=32;
	HugeBomb.r2=64;
	HugeBomb.vr1=div(er1-32,time).quot;
	HugeBomb.vr2=div(er2-64,time).quot;
	Enabled=true;
	SWH=&HandleFog;
};
void HandleFog(StrangeWeapon* SW){
	if(SW->Enabled){
		OneObject* OB=Group[SW->OwnerID];
		for(int j=0;j<0;j++){
			byte angl=rando()&255;
			int rad=rando()&511;
			int xx=SW->x+((rad*TSin[angl])>>8);
			int yy=SW->y+((rad*TCos[angl])>>8);
			byte xxx=byte(xx>>7);
			byte yyy=byte(xx>>7);
			word MID=Mops[yyy][xxx];
			MakeFog(xx,yy);
			//	CreateExObjD(WPLIST[NUCLUSE[rando()&3]],xx,yy,angl,8,255,NULL);
			SW->HugeBomb.r1++;
		};
		int rx=SW->HugeBomb.r2;
		if(rx>30)rx=50;
		for(j=0;j<rx;j++){
			byte angl=rando()&255;
			int rr=SW->HugeBomb.r2+(rando()&127);
			int xx=SW->x+((rr*TSin[angl])>>8);
			int yy=SW->y+((rr*TCos[angl])>>8);
			byte xxx=byte(xx>>7);
			byte yyy=byte(yy>>7);
			word MID=Mops[yyy][xxx];
			MakeFog(xx,yy);
			//CreateExObjD(WPLIST[NUCLUSE[rando()&3]],xx,yy,angl,16,255,NULL);
			SW->HugeBomb.r2++;
		};
		if(SW->HugeBomb.r2>SW->HugeBomb.Radius2)SW->Enabled=false;
	};
};
//FIRE PROTECTOR
/*void HandleFireProtection(StrangeWeapon* SW);
void StrangeWeapon::CreateFireProtection(int ex,int ey,word MagID){
	OneObject* OB=Group[MagID];
	Nation* NT=OB->Nat;
	if(OB->Magic<NT->WMagic[5])return;
	Enabled=true;
	x=ex<<7;
	y=ey<<7;
	OwnerID=MagID;
	if(OB)OwnerSerial=OB->Serial;
	else{
		Enabled=false;
		return;
	};
	MagAmount=10;
	MagSpeed=1;
	MagMaxConsume=10000;
	MagConsume=0;
	NNUM=OB->NNUM;
	MExp.r=1;
	MExp.r1=300;
	SWH=&HandleSuperVision;
};
void HandleSuperVision(StrangeWeapon* SW){
	if(SW->Enabled){
		OneObject* OB=Group[SW->OwnerID];
		if(!OB||(OB&&
		(OB->Serial!=SW->OwnerSerial||OB->Magic<SW->MagSpeed))){
			SW->Enabled=false;
			return;
		};
		SW->MagConsume+=SW->MagSpeed;
		if(!(rando()&7))OB->Magic-=rando()&1;
		if(SW->MagConsume>=SW->MagMaxConsume)SW->Enabled=false;
		//Body of graphic routine
		if(SW->NNUM==MyNation){
			for(int i=0;i<128;i++){
				int xx=(SW->x>>7);
				int yy=(SW->y>>7);
				int dx=-16+(rando()&31);
				int dy=-16+(rando()&31);
				if(dx*dx+dy*dy<256)
					for(int fx=-2;fx<2;fx++)
						for(int fy=-4;fy<5;fy++){
							int hx=xx+dx+fx;
							int hy=yy+dy+fy;
							if(hx>1&&hx<msx&&hy>1&&hy<msy)
								fmap[hy][hx]+=(rando()&127)-32;
						};
			};
		};
		
		//end of graphic routine
	};
};*/
//END FIRE PROTECTOR
//USAGE
StrangeWeapon SWEAP[64];
void AddHugeBomb(int ex,int ey,int er1,int er2,int time,word ID){
	for(int i=0;SWEAP[i].Enabled&&i<64;i++);
	if(i<64)
		SWEAP[i].CreateHugeBomb(ex,ey,er1,er2,time,ID);
};
void AddFireSea(int ex,int ey,word ID){
	for(int i=0;SWEAP[i].Enabled&&i<64;i++);
	if(i<64)
		SWEAP[i].CreateFireSea(ex,ey,ID);
};
void AddBlizzard(int ex,int ey,word ID){
	for(int i=0;SWEAP[i].Enabled&&i<64;i++);
	if(i<64)
		SWEAP[i].CreateBlizzard(ex,ey,ID);
};
void AddMExpl(int ex,int ey,word ID){
	for(int i=0;SWEAP[i].Enabled&&i<64;i++);
	if(i<64)
		SWEAP[i].CreateMExplosion(ex,ey,ID);
};
void AddMExpl1(int ex,int ey,word ID){
	for(int i=0;SWEAP[i].Enabled&&i<64;i++);
	if(i<64)
		SWEAP[i].CreateMExplosion1(ex,ey,ID);
};
void AddFireWall(int ex,int ey,word ID){
	for(int i=0;SWEAP[i].Enabled&&i<64;i++);
	if(i<64)
		SWEAP[i].CreateFireWall(ex,ey,ID);
};
void AddSuperVision(int ex,int ey,word ID){
	for(int i=0;SWEAP[i].Enabled&&i<64;i++);
	if(i<64)
		SWEAP[i].CreateSuperVision(ex,ey,ID);
};
void AddFlyBomb(int ex,int ey,int er1,int er2,int time,word ID){
	for(int i=0;SWEAP[i].Enabled&&i<64;i++);
	if(i<64)
		SWEAP[i].CreateFlyUP(ex,ey,er1,er2,time,ID);
};
void AddNuclExpl(int ex,int ey,int er1,int er2,int time,word ID){
	for(int i=0;SWEAP[i].Enabled&&i<64;i++);
	if(i<64)
		SWEAP[i].CreateNucl(ex,ey,er1,er2,time);
};
void AddFog(int ex,int ey,int er1,int er2,int time,word ID){
	for(int i=0;SWEAP[i].Enabled&&i<64;i++);
	if(i<64)
		SWEAP[i].CreateFog(ex,ey,er1,er2,time);
};
void CreateStrangeObject(int i,byte NI,int x,int y,word ID){
	switch(i){
	case 1:
		AddHugeBomb(x,y,50,300,50,ID);
		break;
	case 2:
		AddHugeBomb(x,y,150,600,50,ID);
		break;
	case 3:
		AddFireSea(x,y,ID);
		break;
	case 4:
		AddBlizzard(x,y,ID);
		break;
	case 5:
		AddMExpl(x,y,ID);
		break;
	case 6:
		AddMExpl1(x,y,ID);
		break;
	case 7:
		AddFireWall(x,y,ID);
		break;
	case 8:
		AddSuperVision(x,y,ID);
		break;
	case 9:
		AddFlyBomb(x,y,50,300,50,ID);
		break;
	case 10:
		AddNuclExpl(x,y,300,1000,50,ID);
		break;
	case 11:
		AddFog(x,y,50,300,50,ID);
		break;
	};
};
void HandleSW(){
	for(int i=0;i<64;i++){
		StrangeWeapon* SW=&SWEAP[i];
		if(SW->Enabled&&SW->SWH)
		SW->SWH(&SWEAP[i]);
	};
};

