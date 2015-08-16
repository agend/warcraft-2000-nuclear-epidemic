#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mgraph.h"
#include "mouse.h"
#include "menu.h"
#include "MapDiscr.h"
#include "weaponID.h"
#include <assert.h>
extern int DHAPROB;
extern int DRECLPROB;
extern int DDIVPROB;
extern int HANSWPROB;
extern int HREQRADIUS;
extern int CROWDMIN;
extern int HCPROB;
extern int CMC1;
extern int CMC2;
extern int CMC3;
Weapon* WPLIST[1024];
extern int NUCLUSE[4];
extern Nation WEP;
char* mbm[1024];
int	nmbm=0;
void NLine(FILE* f){
	char tt[8];
	int zz;
	do{
		zz=fscanf(f,"%lc",tt);\
	}while(tt[0]!=10&&zz!=EOF);
};
void Errx(LPCSTR s)
{
	MessageBox(hwnd,s,"Nation loading failed...",MB_ICONWARNING|MB_OK);
	assert(false);
};
int GetCIndex(char* s){
	char gx[128];
	if(!strcmp(s,"NUCLUSE"))
		return 1;
	sprintf(gx,"Unknown constant : %s",s);
	Errx(gx);
	return -1;
};
void GetMode(char* s,int* mode,int line){
	char gx[128];
	if(!strcmp(s,"[MEMBERS]")){
		*mode=1;
		return;
	};
	if(!strcmp(s,"[REFERENCE]")){
		*mode=2;
		return;
	};
	if(!strcmp(s,"[WEAPON]")){
		*mode=3;
		return;
	};
	if(!strcmp(s,"[BUILDINGS]")){
		*mode=4;
		return;
	};
	if(!strcmp(s,"[PRODUCE]")){
		*mode=5;
		return;
	};
	if(!strcmp(s,"[MORERES]")){
		*mode=6;
		return;
	};
	if(!strcmp(s,"[UPGRADE]")){
		*mode=7;
		return;
	};
	if(!strcmp(s,"[UPGRADESTART]")){
		*mode=8;
		return;
	};
	if(!strcmp(s,"[UPGRADELINKS]")){
		*mode=9;
		return;
	};
	if(!strcmp(s,"[DELAYTIME]")){
		*mode=10;
		return;
	};
	if(!strcmp(s,"[WEAPONCENTER]")){
		*mode=11;
		return;
	};
	if(!strcmp(s,"[CHILDWEAPON]")){
		*mode=12;
		return;
	};
	if(!strcmp(s,"[NUCLI]")){
		*mode=13;
		return;
	};
	if(!strcmp(s,"[CONSTANT]")){
		*mode=14;
		return;
	};
	if(!strcmp(s,"[CHILDMONSTER]")){
		*mode=15;
		return;
	};
	if(!strcmp(s,"[VISIBILITY]")){
		*mode=16;
		return;
	};
	if(!strcmp(s,"[LIGHTPOINTS]")){
		*mode=17;
		return;
	};
	if(!strcmp(s,"[DANGERZONE]")){
		*mode=18;
		return;
	};
	if(!strcmp(s,"[NOSEARCHVICTIM]")){
		*mode=19;
		return;
	};
	if(!strcmp(s,"[NOANSWER]")){
		*mode=20;
		return;
	};
	if(!strcmp(s,"[NEEDNOHELP]")){
		*mode=21;
		return;
	};
	if(!strcmp(s,"[LIFESHOTLOST]")){
		*mode=22;
		return;
	};
	if(!strcmp(s,"[ONWATER]")){
		*mode=24;
		return;
	};
	if(!strcmp(s,"[ONCOST]")){
		*mode=25;
		return;
	};
	if(!strcmp(s,"[ONOIL]")){
		*mode=26;
		return;
	};
	if(!strcmp(s,"[CANBUILD]")){
		*mode=27;
		return;
	};
	if(!strcmp(s,"[USEFUL_FOR_DEFENSE]")){
		*mode=28;
		return;
	};
	if(!strcmp(s,"[USEFUL_FOR_ATTACK]")){
		*mode=29;
		return;
	};
	if(!strcmp(s,"[USEFUL_FOR_SCIENCE]")){
		*mode=30;
		return;
	};
	if(!strcmp(s,"[USEFUL_FOR_NARXOZ]")){
		*mode=31;
		return;
	};
	if(!strcmp(s,"[ABSOLUTELYNEED]")){
		*mode=32;
		return;
	};
	if(!strcmp(s,"[BRANCHPERCENT]")){
		*mode=33;
		return;
	};
	if(!strcmp(s,"[AICOEFFICIENTS]")){
		*mode=34;
		return;
	};
	if(!strcmp(s,"[MAXAUTOAMOUNT]")){
		*mode=35;
		return;
	};
	if(!strcmp(s,"[WARRIORS]")){
		*mode=36;
		return;
	};
	if(!strcmp(s,"[AIR]")){
		*mode=37;
		return;
	};
	if(!strcmp(s,"[WEAPONCENTER8]")){
		*mode=38;
		return;
	};
	if(!strcmp(s,"[TELEPORT]")){
		*mode=39;
		return;
	};
	if(!strcmp(s,"[ENABLED]")){
		*mode=40;
		return;
	};
	if(!strcmp(s,"[AUTO_PERFORM_UPGRADE]")){
		*mode=41;
		return;
	};
	if(!strcmp(s,"[ICONS]")){
		*mode=42;
		return;
	};
	if(!strcmp(s,"[UNITS_INTERFACE]")){
		*mode=43;
		return;
	};
	if(!strcmp(s,"[MOREWEAPON]")){
		*mode=44;
		return;
	};
	if(!strcmp(s,"[IDWEAPON]")){
		*mode=45;
		return;
	};
	if(!strcmp(s,"[TRANSPORT]")){
		*mode=46;
		return;
	};
	if(!strcmp(s,"[COMMONMENU]")){
		*mode=47;
		return;
	};
	if(!strcmp(s,"[FUNCTIONALITY]")){
		*mode=48;
		return;
	};
	if(!strcmp(s,"[MAGICREQUEST]")){
		*mode=49;
		return;
	};
	if(!strcmp(s,"[ACCESSCONTROL]")){
		*mode=50;
		return;
	};
	if(!strcmp(s,"[MAGICCONTROL]")){
		*mode=51;
		return;
	};
	if(!strcmp(s,"[RESOURCEABSORBERS]")){
		*mode=52;
		return;
	};
	if(!strcmp(s,"[CANFLYUP]")){
		*mode=53;
		return;
	};
	if(!strcmp(s,"[WALLKILLER]")){
		*mode=54;
		return;
	};
	if(!strcmp(s,"[FEAR]")){
		*mode=55;
		return;
	};
	if(!strcmp(s,"[USEMAGIC]")){
		*mode=56;
		return;
	};
	if(!strcmp(s,"[NUCLEAR]")){
		*mode=57;
		return;
	};
	if(!strcmp(s,"[ANTINUCLEAR]")){
		*mode=58;
		return;
	};
	if(!strcmp(s,"[UFO]")){
		*mode=59;
		return;
	};
	if(!strcmp(s,"[UFOTRANSFORM]")){
		*mode=60;
		return;
	};
	if(!strcmp(s,"[MAGACTOR]")){
		*mode=61;
		return;
	};
	if(!strcmp(s,"[ON_HIT_SOUND]")){
		*mode=62;
		return;
	};
	if(!strcmp(s,"[ON_CLICK_SOUND]")){
		*mode=63;
		return;
	};
	if(!strcmp(s,"[SOUND]")){
		*mode=64;
		return;
	};
	if(!strcmp(s,"[ON_ORDER_SOUND]")){
		*mode=65;
		return;
	};
	if(!strcmp(s,"[ON_ATTACK_SOUND]")){
		*mode=66;
		return;
	};
	if(!strcmp(s,"[ON_DEATH_SOUND]")){
		*mode=67;
		return;
	};
	if(!strcmp(s,"[ON_BORN_SOUND]")){
		*mode=68;
		return;
	};
	if(!strcmp(s,"[ON_TREE_SOUND]")){
		*mode=69;
		return;
	};
	if(!strcmp(s,"[ON_GOLD_SOUND]")){
		*mode=70;
		return;
	};
	if(!strcmp(s,"[SOUNDEFFECTS]")){
		*mode=71;
		return;
	};
	if(!strcmp(s,"[CANREPAIR]")){
		*mode=72;
		return;
	};
	if(!strcmp(s,"[CANSETDEST]")){
		*mode=73;
		return;
	};
	if(!strcmp(s,"[EXTMENU]")){
		*mode=74;
		return;
	};
	if(!strcmp(s,"[RESOURCEUPGRADE]")){
		*mode=75;
		return;
	};
	if(!strcmp(s,"[UNITLOCK]")){
		*mode=76;
		return;
	};
	if(!strcmp(s,"[END]")){
		*mode=255;
		return;
	};
	if(s[0]=='['){
		sprintf(gx,"Unrecognised command in line %d",line);
		Errx(gx);
	};
};

int SearchStr(char** Res,char* s,int count){
	for(int i=0;i<count;i++)
		if(!strcmp(Res[i],s))return i;
	return -1;
};
extern char* SoundID[512];
extern word NSounds;
void LoadWeapon(){
	char gg[128];
	char gx[128],gx1[128],gx2[128],gx3[128];
	char gy[128];
	int	mode=0;
	int line=0;
	int wid=0;
	int cam,zz1,zz2;
	int mindam,maxdam;
	memset(&WPLIST,0,sizeof WPLIST);
	FILE* f=fopen("weapon.nds","rt");
	if(!int(f)){
		Errx("Could not find WEAPON.NDS");
		return;
	};
	do{
		if(fscanf(f,"%s",gg)==EOF){
			Errx("No information found in WEAPON.NDS");
		};
		NLine(f);
		line++;
		if(gg[0]!='/'&&gg[0]!=' '&&gg[0]!=0)break;
	}while(true);
	WEP.CreateNation(gg,255,9);
	int z=0;
	do{
		switch(mode){
		case 0:
			z=fscanf(f,"%s",gg);
			NLine(f);
			line++;
			GetMode(gg,&mode,line);
			break;
		case 1:
			//[MEMBERS]
			z=fscanf(f,"%s",gg);
			GetMode(gg,&mode,line);
			if(mode==1)
				z=fscanf(f,"%d%d%d%d",&cam,&wid,&mindam,&maxdam);
			NLine(f);
			line++;
			if(mode==1&&gg[0]!='/'&&gg[0]!=0){
				mbm[nmbm]=new char[strlen(gg)+1];
				strcpy(mbm[nmbm],gg);
				WEP.Mon[nmbm]->cpbMoving=cam&1;
				WPLIST[nmbm]=new Weapon;
				WPLIST[nmbm]->CreateWeapon(&WEP,nmbm,wid,0);
				WPLIST[nmbm]->MinDamage=mindam;
				WPLIST[nmbm]->MaxDamage=maxdam;
				WPLIST[nmbm]->Transparent=cam&(2+4);
				WPLIST[nmbm]->SoundID=-1;
				nmbm++;
			};
			break;
		case 2:
			//[REFERENCE]
			z=fscanf(f,"%s",gg);
			GetMode(gg,&mode,line);
			if(mode==2)
				z=fscanf(f,"%s",gx);
			NLine(f);
			line++;
			if(mode==2&&gg[0]!='/'&&gg[0]!=0){
				int zz1=SearchStr(mbm,gg,nmbm);
				if(zz1==-1){
					sprintf(gy,"Identifier %s not found.",gg);
					Errx(gy);
				};
				int zz2=SearchStr(mbm,gx,nmbm);
				if(zz2==-1){
					sprintf(gy,"Identifier %s not found.",gx);
					Errx(gy);
				};
				WPLIST[zz1]->NextWeapon=WPLIST[zz2];
			};
			break;
		case 12://[CHILDWEAPON]
			z=fscanf(f,"%s",gg);
			GetMode(gg,&mode,line);
			if(mode==12)
				z=fscanf(f,"%s%d%d%d",gx,&wid,&mindam,&maxdam);
			NLine(f);
			line++;
			if(mode==12&&gg[0]!='/'&&gg[0]!=0){
				zz1=SearchStr(mbm,gx,nmbm);
				if(zz1==-1){
					sprintf(gy,"%d (WEAPON.NDS) : Unknown parent weapon : %s",line,gx);
					Errx(gy);
				};

				mbm[nmbm]=new char[strlen(gg)+1];
				strcpy(mbm[nmbm],gg);
				WPLIST[nmbm]=new Weapon;
				WPLIST[nmbm]->CreateWeapon(&WEP,zz1,wid,0);
				WPLIST[nmbm]->MinDamage=mindam;
				WPLIST[nmbm]->MaxDamage=maxdam;
				nmbm++;
			};
			break;
		case 13://[NUCLI]
			z=fscanf(f,"%s",gg);
			GetMode(gg,&mode,line);
			if(mode==13)
				z=fscanf(f,"%d%d%d",&wid,&mindam,&maxdam);
			NLine(f);
			line++;
			if(mode==13&&gg[0]!='/'&&gg[0]!=0){
				mbm[nmbm]=new char[strlen(gg)+1];
				strcpy(mbm[nmbm],gg);
				WPLIST[nmbm]=new Weapon;
				WPLIST[nmbm]->CreateNWeapon(wid,mindam,maxdam);
				//WPLIST[nmbm]->MinDamage=mindam;
				//WPLIST[nmbm]->MaxDamage=maxdam;
				nmbm++;
			};
			break;
		case 14://[CONSTANT]
			z=fscanf(f,"%s",gg);
			GetMode(gg,&mode,line);
			if(mode==14)
				z=fscanf(f,"%s%s%s%s",gx,gx1,gx2,gx3);
			NLine(f);
			line++;
			if(mode==14&&gg[0]!='/'&&gg[0]!=0){
				zz1=GetCIndex(gg);
				if(zz1==1){
					zz1=SearchStr(mbm,gx,nmbm);
					if(zz1==-1){
						sprintf(gy,"Illegal value for NUCLUSE : %s",gx);
						Errx(gy);
					};
					NUCLUSE[0]=zz1;
					zz1=SearchStr(mbm,gx1,nmbm);
					if(zz1==-1){
						sprintf(gy,"Illegal value for NUCLUSE : %s",gx1);
						Errx(gy);
					};
					NUCLUSE[1]=zz1;
					zz1=SearchStr(mbm,gx2,nmbm);
					if(zz1==-1){
						sprintf(gy,"Illegal value for NUCLUSE : %s",gx2);
						Errx(gy);
					};
					NUCLUSE[2]=zz1;
					zz1=SearchStr(mbm,gx3,nmbm);
					if(zz1==-1){
						sprintf(gy,"Illegal value for NUCLUSE : %s",gx);
						Errx(gy);
					};
					NUCLUSE[3]=zz1;
				};
			};
			break;
		case 45://[IDWEAPON]
			z=fscanf(f,"%s",gg);
			GetMode(gg,&mode,line);
			if(mode==45)
				z=fscanf(f,"%s",gx);
			NLine(f);
			line++;
			if(mode==45&&gg[0]!='/'&&gg[0]!=0){
				int zz1=SearchStr(mbm,gx,nmbm);
				if(zz1==-1){
					sprintf(gy,"Identifier %s not found.",gg);
					Errx(gy);
				};
				if(!strcmp(gg,"ID_FIRE"))ID_FIRE=zz1;else
				if(!strcmp(gg,"ID_FIRE1"))ID_FIRE1=zz1;else
				if(!strcmp(gg,"ID_MAGW"))ID_MAGW=zz1;else
				if(!strcmp(gg,"ID_EXPL1"))ID_EXPL1=zz1;else
				if(!strcmp(gg,"ID_EXPL2"))ID_EXPL2=zz1;else
				if(!strcmp(gg,"ID_GLASSFLY"))ID_GLASSFLY=zz1;else
				if(!strcmp(gg,"ID_GLASSBROKEN"))ID_GLASSBROKEN=zz1;else
				if(!strcmp(gg,"ID_MAGEXP"))ID_MAGEXP=zz1;else
				if(!strcmp(gg,"ID_FOG"))ID_FOG=zz1;else
				if(!strcmp(gg,"ID_FOG1"))ID_FOG1=zz1;else
				if(!strcmp(gg,"ID_FIREX"))ID_FIREX=zz1;else{
					sprintf(gx,"[IDWEAPON] %d :Unknown constant: %s",line,gg);
					Errx(gx);
				};
			};
			break;
		case 64://[SOUND}
			z=fscanf(f,"%s",gg);
			GetMode(gg,&mode,line);
			if(mode==64)
				z=fscanf(f,"%s",gx);
			NLine(f);
			line++;
			if(mode==64&&gg[0]!='/'&&gg[0]!=0){
				zz1=SearchStr(mbm,gg,nmbm);
				if(zz1==-1){
					sprintf(gy,"Unknown weapon ID: %s",gg);
					Errx(gy);
				};
				zz2=SearchStr(SoundID,gx,NSounds);
				if(zz2==-1){
					sprintf(gy,"Unknown Sound ID: %s",gx);
					Errx(gy);
				};
				WPLIST[zz1]->SoundID=zz2;
			};
			break;
		case 255:
			z=EOF;
			break;
		};
	}while(z!=EOF);
	if(mode==0)Errx("[MEMBERS] not found in WEAPON.NDS");
};
void normstr(char* str){
	for(int i=0;str[i]!=0;i++)if(str[i]=='_')str[i]=' ';
};
void InitFlags(GeneralObject* GO){
	GO->OnAir=false;
	GO->delay=8;
	GO->wepX=0;
	GO->wepY=0;
	GO->VisRadius=10;
	GO->VisSpots=3;
	GO->SpotType=1;
	GO->SpotSize=3;
	GO->DangerZone=0;
	GO->NoSearchVictim=false;
	GO->NoAnswer=false;
	GO->LifeShotLost=0;
	GO->Enabled=false;
	GO->OnWater=false;
	GO->OnCost=false;
	GO->OnOil=false;
	GO->cpbBuild=false;
	GO->Teleport=false;
	GO->cpbMoving=true;
	GO->capMagic=false;
	GO->Submarine=false;
	GO->UATT=false;
	GO->SATT=false;
	GO->WATT=false;
	GO->AATT=false;
	GO->P100=false;
	GO->T3X3=false;
	GO->WepSpeed=64;
	GO->WepDelay=16;
	GO->AGold=false;
	GO->AWood=false;
	for(int i=0;i<NBRANCH;i++){
		GO->Useful[i]=0;
		GO->SpUsef[i]=0;
	};
	GO->AbsNeedPrio=0;
	GO->AbsNeedCount=0;
	GO->MaxAutoAmount=2048;
	GO->Warrior=true;
	GO->NIcons=0;
	GO->NWeap=0;
	GO->Transport=false;
	GO->FWEAP=false;
	GO->RefreshLife=false;
	GO->CanFly=false;
	GO->CanAttWall=false;
	GO->UseMagic=false;
	GO->CanFear=false;
	GO->NeedNoHelp=false;
	GO->AntiNuc=false;
	GO->canNucAttack=false;
	GO->UFO=false;
	GO->UFOTrans=false;
	for(i=0;i<12;i++){
		GO->MWeap[i]=NULL;
		GO->SWPIndex[i]=255;
	};
	GO->MagAgainst=0;
	GO->MagWith=0;
	GO->HitSound=-1;
	GO->ClickSound=-1;
	GO->DeathSound=-1;
	GO->OrderSound=-1;
	GO->AttackSound=-1;
	GO->TreeSound=-1;
	GO->GoldSound=-1;
	GO->BornSound=-1;
	GO->CanRepair=false;
	GO->ExtMenu=false;
	GO->CanDest=false;
	GO->LockID=0xFFFF;
	GO->NLockUnits=0;
	memset(GO->Wdx,0,8);
	memset(GO->Wdy,0,8);
};
void LoadAI(char* fn,Nation* NT,char** Mon,word NMon,char** Upg,word NUpg);
void LoadNation(char* fn,byte msk,byte NIndex){
	char gg[128];
	char gx[128];
	char gy[128];
	char gz[128],gg1[128];
	char* mnm [1024];
	int	nmnm=0;
	char* upg [1024];
	char* ico [1024];
	int nico=0;
	int nupg=0;
	int	mode=0;
	int line=0;
	int wid=0;
	int	zz1,knd;
	int parm0,parm1,parm2,parm3,parm4,parm5,parm6;
	int parm7,parm8,parm9,parm10,parm11;
	int mindam,maxdam,gold;
	FILE* f1=fopen(fn,"rt");
	if(!int(f1)){
		sprintf(gg,"Could not open %s",fn);
		Errx(gg);
		return;
	};
	do{
		if(fscanf(f1,"%s",gg)==EOF){
			sprintf(gg,"%s : No information found.",fn);
			Errx(gg);
		};
		NLine(f1);
		line++;
		if(gg[0]!='/'&&gg[0]!=' '&&gg[0]!=0)break;
	}while(true);
	Nation* nat=&NATIONS[NIndex];
	nat->MagicDelay=0;
	nat->CreateNation(gg,msk,NIndex);
	strcpy(nat->SCRIPT,fn);
	nat->NNUM=NIndex;
	nat->UPG.NUpgrades=0;
	nat->DangerSound=-1;
	nat->VictorySound=-1;
	nat->ConstructSound=-1;
	nat->BuildDieSound=-1;
	nat->UnitDieSound=-1;
	nat->LastAttackTime=0;
	nat->MagicDelay=0;
	int z=0;
	do{
		switch(mode){
		case 0:
			z=fscanf(f1,"%s",gg);
			NLine(f1);
			line++;
			GetMode(gg,&mode,line);
			break;
		case 1:
			//[MEMBERS]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			if(mode==1)
				z=fscanf(f1,"%s%d%d%d%d%d%d%d%d%d%d%d%d",gx,&parm0,&parm1,&parm2,&parm3,&parm4,&parm5,&parm6,&parm7,&parm8,&parm9,&parm10,&parm11);
			NLine(f1);
			line++;
			GetMode(gg,&mode,line);
			if(mode==1&&gg[0]!='/'&&gg[0]!=0){
				if(nmnm>=nat->NMon){
					sprintf(gy,"%s : Too many members.",fn);
					Errx(gy);
				};
				for(int j=1;j<4;j++){
					nat->Mon[nmnm]->ResourceID[j]=255;
					nat->Mon[nmnm]->ResAmount[j]=0;
				};
				InitFlags(nat->Mon[nmnm]);
				nat->Mon[nmnm]->Message=new char[strlen(gx)+1];
				normstr(gx);
				strcpy(nat->Mon[nmnm]->Message,gx);
				nat->Mon[nmnm]->NStages=parm4;
				nat->Mon[nmnm]->cost=parm5;
				nat->Mon[nmnm]->Kind=parm0;
				nat->Mon[nmnm]->cpbBase=false;
				nat->Mon[nmnm]->cpbFarm=false;
				nat->Mon[nmnm]->ResourceID[0]=1;
				nat->Mon[nmnm]->ResAmount[0]=parm5;
				if(parm6){
					nat->Mon[nmnm]->ResourceID[1]=2;
					nat->Mon[nmnm]->ResAmount[1]=parm6;
				};
				if(parm7){
					nat->Mon[nmnm]->ResourceID[2]=3;
					nat->Mon[nmnm]->ResAmount[2]=parm7;
				};
				((Visuals*)(nat->Mon[nmnm]))->info.Basic.MinDamage=parm11;
				((Visuals*)(nat->Mon[nmnm]))->info.Basic.MaxDamage=parm10;
				((Visuals*)(nat->Mon[nmnm]))->info.Basic.MaxLife=parm2;
				nat->Mon[nmnm]->capMagic=parm3;
				((Visuals*)(nat->Mon[nmnm]))->info.Basic.AttackRange=parm8;
				((Visuals*)(nat->Mon[nmnm]))->info.Basic.MaxShield=parm9;
				nat->Mon[nmnm]->VisRadius=parm8;
				nat->Mon[nmnm]->SpotSize=parm1;
				nat->Mon[nmnm]->SpotType=1;
				if(parm1>2){
					nat->Mon[nmnm]->SpotType=2;
				};
				mnm[nmnm]=new char[strlen(gg)+1];
				strcpy(mnm[nmnm],gg);
				nmnm++;
			};
			break;
		case 2:
			//[REFERENCE]
			sprintf(gg,"%s : Unexpected section [REFERENCE]",gg);
			Errx(gg);
			break;
		case 3:
			//[WEAPON]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			if(mode==3)z=fscanf(f1,"%d%d%s%s",&parm1,&parm2,gz,gx);
			NLine(f1);
			line++;
			if(mode==3){
				if(gg[0]!='/'&&gg[0]!=0){
					int zz1=SearchStr(mnm,gg,nmnm);
					if(zz1==-1){
						sprintf(gy,"%s : Identifier %s not found.",fn,gg);
						Errx(gy);
					};
					int zz2=SearchStr(mbm,gx,nmbm);
					if(zz2==-1){
						sprintf(gy,"%s : Identifier %s not found.",fn,gy);
						Errx(gy);
					};
					GeneralObject* GO=nat->Mon[zz1];
					if(strchr(gz,'S'))GO->SATT=true;
					if(strchr(gz,'U'))GO->UATT=true;
					if(strchr(gz,'W'))GO->WATT=true;
					if(strchr(gz,'A'))GO->AATT=true;
					if(strchr(gz,'P'))GO->P100=true;
					if(strchr(gz,'T'))GO->T3X3=true;
					if(strchr(gz,'F'))GO->FWEAP=true;
					GO->WepSpeed=parm1;
					GO->WepDelay=parm2;
					nat->AssignWeapon(WPLIST[zz2],zz1);
				};
			};
			break;
		case 4:
			//[BUILDINGS]
			z=fscanf(f1,"%s",gg);
			NLine(f1);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0)break;
			if(mode==4){
				int zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gy,"%s : Undefined identifier %s at line %d",fn,gg,line);
					Errx(gy);
				};
				nat->Mon[zz1]->cpbBuilding=true;
				nat->Mon[zz1]->cpbMoving=false;
				if(!strcmp(gg,"TOWN"))
					nat->Mon[zz1]->cpbBase=true;
				if(!strcmp(gg,"FARM"))
					nat->Mon[zz1]->cpbFarm=true;
		};
			break;
		case 5:
			//[PRODUCE]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				line++;
				break;
			};
			if(mode==5)
				z=fscanf(f1,"%d",&wid);
			NLine(f1);
			line++;
			if(mode==5){
				int zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gy,"%s : Undefined identifier %s at line %d",fn,gg,line);
					Errx(gy);
				};
				nat->PACount[zz1]=wid;
				nat->PAble[zz1]=new word[wid];
				for(int j=0;j<wid;j++){
					z=fscanf(f1,"%s",gg);
					int	zz2=SearchStr(mnm,gg,nmnm);
					if(zz2==-1){
						sprintf(gy,"%s : Undefined identifier %s at line %d",fn,gg,line);
						Errx(gy);
					};
					nat->PAble[zz1][j]=zz2;
					line++;
				};
			};
			break;
		case 6://MORERES
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==6){
				z=fscanf(f1,"%d%d",&parm1,&parm2);
				NLine(f1);
				int zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gy,"%s : Undefined identifier %s at line %d",fn,gg,line);
					Errx(gy);
				};
				for(int op=1;op<4&&nat->Mon[zz1]->ResourceID[0]==255;op++);
				nat->Mon[zz1]->ResourceID[op]=parm1;
				nat->Mon[zz1]->ResAmount[op]=parm2;
			};
			break;
		case 7://UPGRADE
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);	
				break;
			};
			if(mode==7){
				z=fscanf(f1,"%s%d%d%d%d%d%d%d%s",gz,&parm1,&parm2,&parm3,&parm4,&parm5,&parm6,&wid,gy);
				//NLine(f1);
				zz1=SearchStr(upg,gg,nupg);
				if(zz1!=-1){
					sprintf(gx,"( %s ) %d : Duplicate identifier: %s",fn,line,gg);
					Errx(gx);
				};
				upg[nupg]=new char[strlen(gg)+1];
				strcpy(upg[nupg],gg);
				switch(wid){
				case 1:
					nat->UPG.utp1[nupg]=new UTP1;
					z=fscanf(f1,"%d%d",&parm5,&parm6);
					nat->UPG.utp1[nupg]->ResType=byte(parm5);
					nat->UPG.utp1[nupg]->AddVal=byte(parm6);
					nat->UPG.utp1[nupg]->OneTime=true;
					break;
				case 2:
					nat->UPG.utp2[nupg]=new UTP2;
					z=fscanf(f1,"%s%d%d",gx,&parm5,&parm6);
					zz1=SearchStr(mnm,gx,nmnm);
					if(zz1==-1){
						sprintf(gz,"( %s ) %d :Unknown identifier : %s",fn,line,gx);
						Errx(gz);
					};
					nat->UPG.utp2[nupg]->MID=zz1;
					nat->UPG.utp2[nupg]->CharID=parm5;
					nat->UPG.utp2[nupg]->AddVal=parm6;
					nat->UPG.utp1[nupg]->OneTime=true;
					break;
				case 3:
					nat->UPG.utp3[nupg]=new UTP3;
					z=fscanf(f1,"%s",gx);
					zz1=SearchStr(mnm,gx,nmnm);
					if(zz1==-1){
						sprintf(gz,"( %s ) %d :Unknown identifier : %s",fn,line,gx);
						Errx(gz);
					};
					nat->UPG.utp3[nupg]->FinalMID=zz1;
					nat->UPG.utp1[nupg]->OneTime=false;
					break;
				case 4://Enable strange weapon
					nat->UPG.utp3[nupg]=new UTP3;
					z=fscanf(f1,"%d",&parm5);
					nat->UPG.utp3[nupg]->FinalMID=parm5;
					nat->UPG.utp1[nupg]->OneTime=true;
					break;
				default:
					sprintf(gx,"( %s ) %d :Unknuwn upgrade type.",fn,line);
					Errx(gx);
				};
				zz1=SearchStr(mnm,gz,nmnm);
				if(zz1==-1){
					sprintf(gx,"( %s ) %d :Undeclared identifier : %s",fn,line,gz);
					Errx(gx);
				};
				nat->UPG.NUpgrades++;
				nat->UPG.utp[nupg]->MonsterID=zz1;
				nat->UPG.utp[nupg]->IFileID=parm1;
				nat->UPG.utp[nupg]->IFIndex=parm2;
				nat->UPG.utp[nupg]->Time=parm3;
				nat->UPG.utp[nupg]->Cost=parm4;
				nat->UPG.utp[nupg]->Wood=parm5;
				nat->UPG.utp[nupg]->Oil=parm6;
				nat->UPG.utp[nupg]->Enabled=false;
				nat->UPG.utp[nupg]->Stage=0;
				nat->UPG.utp[nupg]->Message=new char[strlen(gy)+1];
				nat->UPG.utp[nupg]->NLinks=0;
				nat->UPG.utp[nupg]->NAuto=0;
				nat->UPG.utp[nupg]->Kind=wid;
				nat->UPG.utp[nupg]->Done=false;
				nat->UPG.utp1[nupg]->Finished=false;
				for(int rr=0;gy[rr]!=0;rr++)if(gy[rr]=='_')gy[rr]=' ';
				strcpy(nat->UPG.utp[nupg]->Message,gy);
				nupg++;
			};
			break;
		case 8://[UPGRADESTART]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			NLine(f1);
			if(mode!=8)break;
			if(gg[0]=='/'||gg[0]==0)break;
			zz1=SearchStr(upg,gg,nupg);
			if(zz1==-1){
				sprintf(gx,"( %s ) %d :Undefined upgrade name : %s",fn,line,gg);
				Errx(gx);
			};
			nat->UPG.utp[zz1]->Enabled=true;
			break;
		case 9://[UPGRADELINKS]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			//NLine(f1);
			if(mode!=9){
				NLine(f1);
				break;
			};
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			z=fscanf(f1,"%d",&parm1);
			zz1=SearchStr(upg,gg,nupg);
			if(zz1==-1){
				sprintf(gz,"( %s ) %d :Undefined upgrade name : %s",fn,line,gg);
				Errx(gz);
			};
			nat->UPG.utp[zz1]->NLinks=parm1;
			nat->UPG.utp[zz1]->Links=new int[parm1];
			for(wid=0;wid<parm1;wid++){
				z=fscanf(f1,"%s",gx);
				parm3=SearchStr(upg,gx,nupg);
				if(parm3==-1){
					sprintf(gz,"( %s ) %d :Undefined identifier : %s",fn,line,gx);
					Errx(gz);
				};
				nat->UPG.utp[zz1]->Links[wid]=parm3;
			};
			NLine(f1);
			break;
		case 10://[DELAYTIME]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==10){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%d",&parm1);
				NLine(f1);
				nat->Mon[zz1]->delay=parm1;
			};
			break;
		case 11://[WEAPONCENTER]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==11){
				z=fscanf(f1,"%d%d",&parm1,&parm2);
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gx,"%d ( %s ) : Unknuwn monster ID : %s",line,fn,gg);
					Errx(gx);
				};
				NLine(f1);
				nat->Mon[zz1]->wepX=parm1;
				nat->Mon[zz1]->wepY=parm2;
			};
			break;
		case 15://[CHILDMONSTER]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			if(mode==15)
				z=fscanf(f1,"%s%s%d%d%d%d%d%d%d",gg1,gx,&knd,&gold,&wid,&mindam,&maxdam,&parm1,&parm2);
			NLine(f1);
			line++;
			GetMode(gg,&mode,line);
			if(mode==15&&gg[0]!='/'&&gg[0]!=0){
				nat->Mon[nmnm]=new Visuals;
				zz1=SearchStr(mnm,gg1,nmnm);
				if(zz1==-1){
					sprintf(gz,"%d ( %s ) : Unknown parent monster : %s",line,fn,gg1);
					Errx(gz);
				};
				*nat->Mon[nmnm]=*nat->Mon[zz1];
				for(int j=1;j<4;j++){
					nat->Mon[nmnm]->ResourceID[j]=255;
					nat->Mon[nmnm]->ResAmount[j]=0;
				};
				InitFlags(nat->Mon[nmnm]);
				nat->Mon[nmnm]->Message=new char[strlen(gx)+1];
				normstr(gx);
				strcpy(nat->Mon[nmnm]->Message,gx);
				nat->Mon[nmnm]->cost=gold;
				nat->Mon[nmnm]->Kind=knd;
				nat->Mon[nmnm]->cpbBase=false;
				nat->Mon[nmnm]->cpbFarm=false;
				nat->Mon[nmnm]->ResourceID[0]=1;
				nat->Mon[nmnm]->ResAmount[0]=gold;
				nat->Mon[nmnm]->OnWater=false;
				((Visuals*)(nat->Mon[nmnm]))->info.Basic.MinDamage=mindam;
				((Visuals*)(nat->Mon[nmnm]))->info.Basic.MaxDamage=maxdam;
				((Visuals*)(nat->Mon[nmnm]))->info.Basic.MaxLife=wid;
				((Visuals*)(nat->Mon[nmnm]))->info.Basic.AttackRange=parm1;
				((Visuals*)(nat->Mon[nmnm]))->info.Basic.MaxShield=parm2;
				mnm[nmnm]=new char[strlen(gg)+1];
				strcpy(mnm[nmnm],gg);
				nmnm++;
				nat->NMon=nmnm;

			};
			break;
		case 16://[VISIBILITY]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==16){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%d%d",&parm1,&parm2);
				NLine(f1);
				nat->Mon[zz1]->VisRadius=parm1;
				nat->Mon[zz1]->VisSpots=parm2;
			};
			break;
		case 17://[LIGHTPOINTS]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==17){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%d%d",&parm1,&parm2);
				NLine(f1);
				nat->Mon[zz1]->SpotType=parm1;
				nat->Mon[zz1]->SpotSize=parm2;
			};
			break;
		case 18://[DANGERZONE]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==18){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%d",&parm1);
				NLine(f1);
				nat->Mon[zz1]->DangerZone=parm1;
			};
			break;
		case 19://[NOSEARCHVICTIM]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==19){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->NoSearchVictim=true;
			};
			break;
		case 20://[NOANSWER]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==20){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->NoAnswer=true;
			};
			break;
		case 21://[NEEDNOHELP]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==21){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->NeedNoHelp=true;
			};
			break;
		case 22://[LIFESHOTLOST]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==22){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%d",&parm1);
				NLine(f1);
				nat->Mon[zz1]->LifeShotLost=parm1;
			};
			break;
		case 23://[STARTACCESS]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==23){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->Enabled=true;
			};
			break;
		case 24://[ONWATER]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==24){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->OnWater=true;
			};
			break;
		case 25://[ONCOST]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==25){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->OnCost=true;
			};
			break;
		case 26://[ONOIL]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==26){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->OnOil=true;
			};
			break;
		case 27://[CANBUILD]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==27){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->cpbBuild=true;
			};
			break;
		case 28://[USEFUL_FOR_DEFENSE
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==28){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%d%d",&parm1,&parm2);
				NLine(f1);
				nat->Mon[zz1]->Useful[0]=parm1;
				nat->Mon[zz1]->SpUsef[0]=parm2;
			};
			break;
		case 29://[USEFUL_FOR_ATTACK]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==29){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%d%d",&parm1,&parm2);
				NLine(f1);
				nat->Mon[zz1]->Useful[1]=parm1;
				nat->Mon[zz1]->SpUsef[1]=parm2;
			};
			break;
		case 30://[USEFUL_FOR_SCIENCE]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==30){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%d%d",&parm1,&parm2);
				NLine(f1);
				nat->Mon[zz1]->Useful[2]=parm1;
				nat->Mon[zz1]->SpUsef[2]=parm2;
			};
			break;
		case 31://[USEFUL_FOR_NARHOZ}
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==31){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%d%d",&parm1,&parm2);
				NLine(f1);
				nat->Mon[zz1]->Useful[3]=parm1;
				nat->Mon[zz1]->SpUsef[3]=parm2;
			};
			break;
		case 32://[ABSOLUTELYNEED]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==32){
				zz1=SearchStr(mnm,gg,nmnm);
				int zz2=SearchStr(upg,gg,nupg);
				if(zz1==-1&&zz2==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%d%d%d%d",&parm1,&parm2,&parm3,&parm4);
				NLine(f1);
				int NAND=nat->NNeed;
				Needness* NDN=&nat->NEED[NAND];
				if(zz1!=-1)NDN->NeedType=0;
				else{
					NDN->NeedType=1;//upgrade
					zz1=zz2;
				};
				NDN->MonID=zz1;
				NDN->GroupSize=parm1;
				NDN->Amount=parm2;
				NDN->Probability=div(32768*parm3,100).quot;
				NDN->MoneyPercent=div(parm4<<8,100).quot;
				//nat->Mon[zz1]->AbsNeedPrio=parm1;
				//nat->Mon[zz1]->AbsNeedCount=parm2;
				nat->NNeed++;
			};
			break;
		case 33://[BRANCHPERCENT]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==33){
				for(int k=0;k<NBRANCH;k++)
					fscanf(f1,"%d",&nat->BranchPercent[k]);
				NLine(f1);
			};
			break;
		case 34://[AICOEFFICIENTS]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==34){
				fscanf(f1,"%d",&parm1);
				if(!strcmp(gg,"CASH_PUSH_PROBABILITY")){
					nat->CASH_PUSH_PROBABILITY=parm1;
				}else
				if(!strcmp(gg,"TAX_PERCENT")){
					nat->TAX_PERCENT=parm1;
				}else
				if(!strcmp(gg,"DEFENCE_HANDLE_ATTACK_PROBABILITY")){
					DHAPROB=parm1;
				}else
				if(!strcmp(gg,"DEFENCE_RECALL_PROBABILITY")){
					DRECLPROB=parm1;
				}else
				if(!strcmp(gg,"DEFENCE_DIVISION_PROBABILITY")){
					DDIVPROB=parm1;
				}else
				if(!strcmp(gg,"HELP_REQUEST_ANSWER_PROBABILITY")){
					HANSWPROB=parm1;
				}else
				if(!strcmp(gg,"HELP_REQUEST_RADIUS")){
					HREQRADIUS=parm1;
				}else
				if(!strcmp(gg,"CROWD_MIN_MEMBERS")){
					CROWDMIN=parm1;
				}else
				if(!strcmp(gg,"HANDLE_CROWD_PROBABILITY")){
					HCPROB=parm1;
				}else
				if(!strcmp(gg,"CROWD_MOVEMENT_C1")){
					CMC1=parm1;
				}else
				if(!strcmp(gg,"CROWD_MOVEMENT_C2")){
					CMC2=parm1;
				}else
				if(!strcmp(gg,"CROWD_MOVEMENT_C3")){
					CMC3=parm1;
				}else
				if(!strcmp(gg,"AI_LEVEL_MIN")){
					nat->AI_Level_MIN=div(parm1*32768,100).quot;
					nat->NationalAI=nat->AI_Level_MIN;
				}else
				if(!strcmp(gg,"AI_LEVEL_MAX")){
					nat->AI_Level_MAX=div(parm1*32768,100).quot;
				}else 
				if(!strcmp(gg,"AI_FORWARD_MULTIPLY")){
					nat->AI_Forward=div(parm1*256,100).quot;
				}else{
					sprintf(gx,"%d ( %s ) : Unknown AI coefficient : %s",line,fn,gg);
					Errx(gx);
				};
				NLine(f1);
			};
			break;
		case 35://[MAXAUTOAMOUNT]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==35){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%d",&parm1);
				NLine(f1);
				nat->Mon[zz1]->MaxAutoAmount=parm1;
			};
			break;
		case 36://[WARRIORS]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==36){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				nat->Mon[zz1]->Warrior=true;
			};
			break;
		case 37://[AIR]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==37){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				nat->Mon[zz1]->OnAir=true;
			};
			break;
		case 38://[WEAPONCENTER8]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==38){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%d%d%d%d",
					&parm1,&parm2,&parm3,&parm4);
				nat->Mon[zz1]->Wdx[0]=parm1;
				nat->Mon[zz1]->Wdy[0]=parm2;
				nat->Mon[zz1]->Wdx[1]=parm3;
				nat->Mon[zz1]->Wdy[1]=parm4;
				z=fscanf(f1,"%d%d%d%d",
					&parm1,&parm2,&parm3,&parm4);
				nat->Mon[zz1]->Wdx[2]=parm1;
				nat->Mon[zz1]->Wdy[2]=parm2;
				nat->Mon[zz1]->Wdx[3]=parm3;
				nat->Mon[zz1]->Wdy[3]=parm4;
				z=fscanf(f1,"%d%d%d%d",
					&parm1,&parm2,&parm3,&parm4);
				nat->Mon[zz1]->Wdx[4]=parm1;
				nat->Mon[zz1]->Wdy[4]=parm2;
				nat->Mon[zz1]->Wdx[5]=parm3;
				nat->Mon[zz1]->Wdy[5]=parm4;
				z=fscanf(f1,"%d%d%d%d",
					&parm1,&parm2,&parm3,&parm4);
				nat->Mon[zz1]->Wdx[6]=parm1;
				nat->Mon[zz1]->Wdy[6]=parm2;
				nat->Mon[zz1]->Wdx[7]=parm3;
				nat->Mon[zz1]->Wdy[7]=parm4;
				NLine(f1);
			};
			break;
		case 39://[TELEPORT]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==39){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->Teleport=true;
			};
			break;
		case 40://[ENABLED]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==40){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->Enabled=true;
			};
			break;
		case 41://[AUTO_PERFORM_UPGRADE]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			//NLine(f1);
			if(mode!=41){
				NLine(f1);
				break;
			};
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			z=fscanf(f1,"%d",&parm1);
			zz1=SearchStr(upg,gg,nupg);
			if(zz1==-1){
				sprintf(gz,"( %s ) %d :Undefined upgrade name : %s",fn,line,gg);
				Errx(gz);
			};
			nat->UPG.utp[zz1]->NAuto=parm1;
			nat->UPG.utp[zz1]->AutoPerf=new int[parm1];
			for(wid=0;wid<parm1;wid++){
				z=fscanf(f1,"%s",gx);
				parm3=SearchStr(upg,gx,nupg);
				if(parm3==-1){
					sprintf(gz,"( %s ) %d :Undefined identifier : %s",fn,line,gx);
					Errx(gz);
				};
				nat->UPG.utp[zz1]->AutoPerf[wid]=parm3;
			};
			NLine(f1);
			break;
		case 42: //[ICONS]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);	
				break;
			};
			if(mode==42){
				WIcon* icc=new WIcon;
				nat->wIcons[nico]=icc;
				nat->NIcons=nico+1;
				z=fscanf(f1,"%s%d%d%d",gg,&parm1,&parm2,&parm3);
				icc->FileID=parm1;
				icc->Spr=parm2;
				icc->Param1=parm3;
				if(parm3>1){
					icc->SubList=new word [parm3];
					for(int i=0;i<parm3;i++){
						z=fscanf(f1,"%d",&parm2);
						icc->SubList[i]=parm2;
					};
				}else icc->SubList=NULL;
				z=fscanf(f1,"%s",gz);
				if(gz[1]=='w'){
					z=fscanf(f1,"%d",&parm6);
				};
				z=fscanf(f1,"%s",gy);
				zz1=SearchStr(ico,gg,nico);
				icc->Message=new char [strlen(gy)+1];
				if(zz1!=-1){
					sprintf(gx,"( %s ) %d : Duplicate icon identifier: %s",fn,line,gg);
					Errx(gx);
				};
				ico[nico]=new char[strlen(gg)+1];
				strcpy(ico[nico],gg);
				if(gz[0]=='/'){
					if(gz[1]=='n'){
						icc->Kind=1;
						sscanf(gz+2,"%d",&parm3);
						icc->Param2=parm3;
					}else
					if(gz[1]=='p'){
						icc->Kind=2;
						z=sscanf(gz+2,"%d",&parm3);
						icc->Param2=parm3;
					}else
					if(gz[1]=='w'){
						icc->Kind=5;
						z=sscanf(gz+2,"%d",&parm3);
						icc->Param2=parm3;
						icc->Param1=parm6;
					}else
					if(gz[1]=='d'){
						icc->Kind=0;
						sscanf(gz+2,"%d",&parm3);
						icc->Param2=parm3;
					}else
					if(gz[1]=='e'){
						icc->Kind=0;
						//scanf(gz+2,"%d",&parm3);
						icc->Param2=0xFFFF;
					}else{
						sprintf(gx,"(%s) %d :Incorrect icon definition, unknown identifier: %s",fn,line,gz);
						Errx(gx);	
					};
				}else{
					//upgrade?
					zz1=SearchStr(upg,gz,nupg);
					if(zz1!=-1){
						icc->Kind=3;
						icc->Param2=zz1;
					}else{
						zz1=SearchStr(mnm,gz,nmnm);
						if(zz1!=-1){
							icc->Kind=4;
							icc->Param2=zz1;
						}else{
							sprintf(gx,"(%s) %d :Incorrect icon definition, unknown identifier: %s",fn,line,gz);
							Errx(gx);	
						};
					};
				};
				for(int rr=0;gy[rr]!=0;rr++)if(gy[rr]=='_')gy[rr]=' ';
				strcpy(icc->Message,gy);
				icc->MagReq=0;
				nico++;
			};
			break;
		case 43: //[UNITS_INTERFACE]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==43){
				z=fscanf(f1,"%d",&wid);
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				GeneralObject* GO=nat->Mon[zz1];
				GO->IRefs=new word[wid];
				GO->NIcons=wid;
				for(int i=0;i<wid;i++){
					z=fscanf(f1,"%s",gz);
					zz1=SearchStr(ico,gz,nico);
					if(zz1!=-1){
						GO->IRefs[i]=zz1;
						WIcon* ico=nat->wIcons[zz1];
						if(ico->Kind==5){
							GO->SWPIndex[ico->Param2]=ico->Param1;
						};
					}else{
						sprintf(gx,"(%s)%d :Unknown identifier :%s",fn,line,gz);
						Errx(gx);
					};
				};
				NLine(f1);
			};
			break;
		case 44://[MOREWEAPON]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			if(mode==44){
				if(gg[0]!='/'&&gg[0]!=0){
					int zz1=SearchStr(mnm,gg,nmnm);
					if(zz1==-1){
						sprintf(gy,"%s : Identifier %s not found.",fn,gg);
						Errx(gy);
					};
					z=fscanf(f1,"%d",&parm1);
					if(!z){
						sprintf(gy,"(%s) %d : Invalid amount of additional weapon.",fn,line);
						Errx(gy);
					};
					GeneralObject* GO=nat->Mon[zz1];
					GO->NWeap=parm1;
					for(int i=0;i<parm1;i++){
						z=fscanf(f1,"%s",gx);
						int zz2=SearchStr(mbm,gx,nmbm);
						if(zz2==-1){
							sprintf(gy,"%s : Identifier %s not found.",fn,gx);
							Errx(gy);
						};
						GO->MWeap[i]=WPLIST[zz2];
					};
					NLine(f1);
					line++;
				};
			};
			break;
		case 46://[TRANSPORT]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==46){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->Transport=true;
			};
			break;
		case 47://[COMMONMENU]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==47){
				if(!strcmp(gg,"COMMON")){
					z=fscanf(f1,"%d",&wid);
					nat->NCmenus=wid;
					nat->Cmenus=new word[wid];
					for(int i=0;i<wid;i++){
						z=fscanf(f1,"%s",gz);
						zz1=SearchStr(ico,gz,nico);
						if(zz1==-1){
							sprintf(gx,"(%s)%d :Unknown identifier :%s",fn,line,gz);
							Errx(gx);
						}else nat->Cmenus[i]=zz1;
					};
				}else
				if(!strcmp(gg,"LAND")){
					z=fscanf(f1,"%d",&wid);
					nat->NLmenus=wid;
					nat->Lmenus=new word[wid];
					for(int i=0;i<wid;i++){
						z=fscanf(f1,"%s",gz);
						zz1=SearchStr(ico,gz,nico);
						if(zz1==-1){
							sprintf(gx,"(%s)%d :Unknown identifier :%s",fn,line,gz);
							Errx(gx);
						}else nat->Lmenus[i]=zz1;
					};
				}else
				if(!strcmp(gg,"AIR")){
					z=fscanf(f1,"%d",&wid);
					nat->NAmenus=wid;
					nat->Amenus=new word[wid];
					for(int i=0;i<wid;i++){
						z=fscanf(f1,"%s",gz);
						zz1=SearchStr(ico,gz,nico);
						if(zz1==-1){
							sprintf(gx,"(%s)%d :Unknown identifier :%s",fn,line,gz);
							Errx(gx);
						}else nat->Amenus[i]=zz1;
					};
				}else 
				if(!strcmp(gg,"WATER")){
					z=fscanf(f1,"%d",&wid);
					nat->NWmenus=wid;
					nat->Wmenus=new word[wid];
					for(int i=0;i<wid;i++){
						z=fscanf(f1,"%s",gz);
						zz1=SearchStr(ico,gz,nico);
						if(zz1==-1){
							sprintf(gx,"(%s)%d :Unknown identifier :%s",fn,line,gz);
							Errx(gx);
						}else nat->Wmenus[i]=zz1;
					}
				}else{
					sprintf(gx,"%d (%s) :Unknown [COMMONMENU] section : %s",line,fn,gg);
					Errx(gx);
				};
				NLine(f1);
			};
			break;
		case 48://[FUNCTIONALITY]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==48){
				z=fscanf(f1,"%s",gy);
				zz1=SearchStr(mnm,gy,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gy);
					Errx(gz);
				};
				if(!strcmp(gg,"TOWN0"))nat->TOWN0ID=zz1;
				else 
				if(!strcmp(gg,"TOWN1"))nat->TOWN1ID=zz1;
				else 
				if(!strcmp(gg,"TOWN2"))nat->TOWN2ID=zz1;
				else 
				if(!strcmp(gg,"PORT"))nat->PortID=zz1;
				else 
				if(!strcmp(gg,"REFINERY"))nat->RefineryID=zz1;
				else 
				if(!strcmp(gg,"TANKER"))nat->TankerID=zz1;
				else 
				if(!strcmp(gg,"OILBASE"))nat->OilBaseID=zz1;
				else 
				if(!strcmp(gg,"SUBMARINE")){
					nat->SubmID=zz1;
					nat->Mon[zz1]->Submarine=true;
				}else 
				if(!strcmp(gg,"FOUNDARY"))nat->FoundaryID=zz1;
				else{
					sprintf(gx,"%d (%s) :Unknown reference name in section [FUNCTIONALITY]: %s",line,fn,gg);
					Errx(gx);
				};
				NLine(f1);
			};
			break;
		case 49://[MAGICREQUEST]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==49){
				z=fscanf(f1,"%d%d%s%s",&parm1,&parm2,gx,gy);
				NLine(f1);
				zz1=SearchStr(ico,gg,nico);
				if(zz1==-1){
					sprintf(gz,"%d ( %s ) : Unknown icon identifier : %s",line,fn,gg);
					Errx(gz);
				};
				WIcon* icc=nat->wIcons[zz1];
				icc->MagReq=parm1;
				if(icc->Kind==5){
					nat->WMagic[icc->Param2]=parm1;
				};
				SWPAR* SWX=&nat->SWP[icc->Param1];
				SWX->MinMagic=parm1;
				SWX->Range=parm2;
				if(!strcmp(gx,"ENABLED"))SWX->Enabled=true;
				else if(!strcmp(gx,"DISABLED"))SWX->Enabled=false;
				else{
					sprintf(gz,"%d ( %s ) : Unknown Weapon status : %s",line,fn,gx);
					Errx(gz);
				};
				if(!strcmp(gy,"FLY"))SWX->Fly=true;
				else if(!strcmp(gy,"DIRECT"))SWX->Fly=false;
				else{
					sprintf(gz,"%d ( %s ) : Unknown Weapon status : %s",line,fn,gy);
					Errx(gz);
				};
			};
			break;
		case 50://[ACCESSCONTROL]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==50){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					zz1=SearchStr(upg,gg,nupg);
					if(zz1==-1){
						sprintf(gx,"%d ( %s ) : Unknown monster/upgrade identifief in [ACCESSCONTROL]: %s",line,fn,gg);
						Errx(gx);
					}else zz1+=8192;
				};
				z=fscanf(f1,"%d",&parm1);
				word nc=nat->NCOND;
				nat->NCOND++;
				nat->CLSize[nc]=parm1;
				nat->CLRef[nc]=new word [parm1+1];
				word* CRF=nat->CLRef[nc];
				CRF[0]=zz1;
				for(int i=0;i<parm1;i++){
					z=fscanf(f1,"%s",gy);
					zz1=SearchStr(mnm,gy,nmnm);
					if(zz1==-1){
						sprintf(gx,"%d ( %s ) : Unknown monster ID in [ACCESSCONTROL] : %s",line,fn,gy);
						Errx(gx);
					};
					CRF[i+1]=zz1;
				};
				NLine(f1);
			};
			break;
		case 51://[MAGICCONTROL]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==51){
				z=fscanf(f1,"%d%d%d%s%s",&parm1,&parm2,&parm3,gx,gy);
				SWPAR* SWX=&nat->SWP[parm1];
				SWX->MinMagic=parm2;
				SWX->Range=parm3;
				if(!strcmp(gx,"ENABLED"))SWX->Enabled=true;
				else if(!strcmp(gx,"DISABLED"))SWX->Enabled=false;
				else{
					sprintf(gz,"%d ( %s ) : Unknown Weapon status : %s",line,fn,gx);
					Errx(gz);
				};
				if(!strcmp(gy,"FLY"))SWX->Fly=true;
				else if(!strcmp(gy,"DIRECT"))SWX->Fly=false;
				else{
					sprintf(gz,"%d ( %s ) : Unknown Weapon status : %s",line,fn,gy);
					Errx(gz);
				};
			};
			break;
		case 52://[RESOURCEABSORBERS]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==52){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gy,"%d ( %s ) : Unknown monster ID : %s",line,fn,gg);
					Errx(gy);
				};
				z=fscanf(f1,"%s",gx);
				if(!strcmp(gx,"GOLD"))nat->Mon[zz1]->AGold=true;
				if(!strcmp(gx,"WOOD"))nat->Mon[zz1]->AWood=true;
				NLine(f1);
			};
			break;
		case 53://[CANFLYUP]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==53){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->CanFly=true;
			};
			break;
		case 54://[WALLKILLER]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==54){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->CanAttWall=true;
			};
			break;
		case 55://[FEAR]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==55){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->CanFear=true;
			};
			break;
		case 56://[USEMAGIC]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==56){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->UseMagic=true;
			};
			break;
		case 57://[NUCLEAR]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==57){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->canNucAttack=true;
			};
			break;
		case 58://[ANTINUCLEAR]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==58){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->AntiNuc=true;
			};
			break;
		case 59://[UFO]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==59){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->UFO=true;
			};
			break;
		case 60://[UFOTRANSFORM]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==60){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%s",gg);
				parm1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->UFOTrans=true;
				nat->Mon[zz1]->IDforUFO=parm1;
			};
			break;
		case 61://[MAGACTOR]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==61){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%d%d",&parm1,&parm2);
				nat->Mon[zz1]->MagAgainst=parm1;
				nat->Mon[zz1]->MagWith=parm2;
				parm1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
			};
			break;
		case 62://[ON_HIT_SOUND]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==62){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%s",&gx);
				parm1=SearchStr(SoundID,gx,NSounds);
				if(parm1==-1){
					sprintf(gy,"Unknown Sound ID: %s",gx);
					Errx(gy);
				};
				nat->Mon[zz1]->HitSound=parm1;
				NLine(f1);
			};
			break;
		case 63://[ON_CLICK_SOUND]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==63){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%s",&gx);
				parm1=SearchStr(SoundID,gx,NSounds);
				if(parm1==-1){
					sprintf(gy,"Unknown Sound ID: %s",gx);
					Errx(gy);
				};
				nat->Mon[zz1]->ClickSound=parm1;
				NLine(f1);
			};
			break;
		case 65://[ON_ORDER_SOUND]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==65){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%s",&gx);
				parm1=SearchStr(SoundID,gx,NSounds);
				if(parm1==-1){
					sprintf(gy,"Unknown Sound ID: %s",gx);
					Errx(gy);
				};
				nat->Mon[zz1]->OrderSound=parm1;
				NLine(f1);
			};
			break;
		case 66://[ON_ATTACK_SOUND]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==66){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%s",&gx);
				parm1=SearchStr(SoundID,gx,NSounds);
				if(parm1==-1){
					sprintf(gy,"Unknown Sound ID: %s",gx);
					Errx(gy);
				};
				nat->Mon[zz1]->AttackSound=parm1;
				NLine(f1);
			};
			break;
		case 67://[ON_DEATH_SOUND]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==67){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%s",&gx);
				parm1=SearchStr(SoundID,gx,NSounds);
				if(parm1==-1){
					sprintf(gy,"Unknown Sound ID: %s",gx);
					Errx(gy);
				};
				nat->Mon[zz1]->DeathSound=parm1;
				NLine(f1);
			};
			break;
		case 68://[ON_BORN_SOUND]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==68){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%s",&gx);
				parm1=SearchStr(SoundID,gx,NSounds);
				if(parm1==-1){
					sprintf(gy,"Unknown Sound ID: %s",gx);
					Errx(gy);
				};
				nat->Mon[zz1]->BornSound=parm1;
				NLine(f1);
			};
			break;
		case 69://[ON_TREE_SOUND]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==69){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%s",&gx);
				parm1=SearchStr(SoundID,gx,NSounds);
				if(parm1==-1){
					sprintf(gy,"Unknown Sound ID: %s",gx);
					Errx(gy);
				};
				nat->Mon[zz1]->TreeSound=parm1;
				NLine(f1);
			};
			break;
		case 70://[ON_GOLD_SOUND]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==70){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				z=fscanf(f1,"%s",&gx);
				parm1=SearchStr(SoundID,gx,NSounds);
				if(parm1==-1){
					sprintf(gy,"Unknown Sound ID: %s",gx);
					Errx(gy);
				};
				nat->Mon[zz1]->GoldSound=parm1;
				NLine(f1);
			};
			break;
		case 71://[SOUNDEFFECTS]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==71){
				z=fscanf(f1,"%s",&gx);
				parm1=SearchStr(SoundID,gx,NSounds);
				if(parm1==-1){
					sprintf(gy,"Unknown Sound ID: %s",gx);
					Errx(gy);
				};
				if(!strcmp(gg,"ATTACK")){
					nat->DangerSound=parm1;
				}else
				if(!strcmp(gg,"VICTORY")){
					nat->VictorySound=parm1;
				}else
				if(!strcmp(gg,"CONSTRUCT")){
					nat->ConstructSound=parm1;
				}else
				if(!strcmp(gg,"BUILDDIE")){
					nat->BuildDieSound=parm1;
				}else
				if(!strcmp(gg,"UNITDIE")){
					nat->UnitDieSound=parm1;
				}else{
					sprintf(gx,"(%s) %d :Unknown sound function : %s",fn,line,gg);
				};
				NLine(f1);
			};
			break;
		case 72://[CANREPAIR]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==72){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->CanRepair=true;
			};
			break;
		case 73://[CANSETDEST]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==73){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->CanDest=true;
			};
			break;
		case 74://[EXTMENU]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==74){
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->ExtMenu=true;
			};
			break;
		case 75://[RESOURCEUPGRADE]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==75){
				z=fscanf(f1,"%s",gx);
				zz1=SearchStr(mnm,gx,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				NLine(f1);
				if(!strcmp(gg,"TOWNHALL1")){
					nat->TOWNHALL1=zz1;
				}else
				if(!strcmp(gg,"TOWNHALL2")){
					nat->TOWNHALL2=zz1;
				}else
				if(!strcmp(gg,"LUMBERMILL")){
					nat->LUMBERMILL=zz1;
				}else{
					sprintf(gz,"( %s ) %d :Invalid [RESOURCEUPGRADE] ID: %s",fn,line,gg);
					Errx(gz);
				};
			};
			break;
		case 76://[LOCKUNIT]
			z=fscanf(f1,"%s",gg);
			GetMode(gg,&mode,line);
			line++;
			if(gg[0]=='/'||gg[0]==0){
				NLine(f1);
				break;
			};
			if(mode==76){
				z=fscanf(f1,"%s%d",gx,&parm1);
				zz1=SearchStr(mnm,gg,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gg);
					Errx(gz);
				};
				parm3=SearchStr(mnm,gx,nmnm);
				if(zz1==-1){
					sprintf(gz,"( %s ) %d :Undefined monster : %s",fn,line,gx);
					Errx(gz);
				};
				NLine(f1);
				nat->Mon[zz1]->LockID=parm3;
				nat->Mon[zz1]->NLockUnits=parm1;
			};
			break;
		case 255://[END]
			z=EOF;
			break;
		};
	}while(z!=EOF);
	LoadAI("Default.ai",nat,mnm,nmnm,upg,nupg);
	for(int i=0;i<nmnm;i++)
		free(mnm[i]);
	for(i=0;i<nupg;i++)
		free(upg[i]);
	for(i=0;i<nico;i++)
		free(ico[i]);
	//Upgrade calculating
	for(i=0;i<nmnm;i++){
		nat->Mon[i]->NUpgrades=0;
		nat->Mon[i]->Upg=NULL;
		int nu=0;
		for(int j=0;j<nupg;j++)
			if(nat->UPG.utp[j]->MonsterID==i)nu++;
		if(nu>0){
			nat->Mon[i]->Upg=new word[nu];
			nat->Mon[i]->NUpgrades=nu;
			nu=0;
			for(int j=0;j<nupg;j++)
				if(nat->UPG.utp[j]->MonsterID==i){
					nat->Mon[i]->Upg[nu]=j;
					nu++;
				};
		};
	};
}
void LoadAI(char* fn,Nation* NT,char** Mon,word NMon,char** Upg,word NUpg){
	char gg[128];
	char gx[128];
	char gy[128];
	FILE* f1=fopen(fn,"rt");
	if(!int(f1)){
		sprintf(gg,"Could not open %s",fn);
		Errx(gg);
		return;
	};
	int i,j,zz1,z,parm1,parm2,parm3;
	char* grp[32];
	int ngrp=0;
	NT->NGrp=0;
	memset(NT->GRSize,0,sizeof NT->GRSize);
	memset(NT->GRRef,0,sizeof NT->GRRef);
	memset(NT->GAmount,0,sizeof NT->GAmount);
	NT->N_AI_Levels=0;
	memset(NT->N_AI_Req,0,sizeof NT->N_AI_Req);
	memset(NT->N_AI_Devlp,0,sizeof NT->N_AI_Devlp);
	memset(NT->AI_Req,0,sizeof NT->AI_Req);
	memset(NT->AI_Devlp,0,sizeof NT->AI_Devlp);
	int line=1;
	int mode=0;
	do{
		z=fscanf(f1,"%s",gg);
		if(z!=EOF){
			if(gg[0]=='#'){
				if(!strcmp(gg,"#DEFGRP"))mode=1;
				else
				if(!strcmp(gg,"#REQ"))mode=2;
				else
				if(!strcmp(gg,"#TODO"))mode=3;
				else
				if(!strcmp(gg,"#ENDREQ")){
						mode=4;
						NT->N_AI_Levels++;
				}else
				if(!strcmp(gg,"#END"))mode=255;
				else{
					sprintf(gx,"%s, line %d :Undefined AI directive : %s",fn,line,gg);
					Errx(gx);
				};
			}else{
				if(gg[0]!='/'){
					switch(mode){
					case 1:		//#DEFGRP
						grp[ngrp]=new char[strlen(gg)+1];
						strcpy(grp[ngrp],gg);
						ngrp++;
						z=fscanf(f1,"%d",&parm1);
						NT->GRSize[NT->NGrp]=parm1;
						NT->GRRef[NT->NGrp]=new word[parm1];
						for(i=0;i<parm1;i++){
							z=fscanf(f1,"%s",gy);
							zz1=SearchStr(Mon,gy,NMon);
							if(zz1!=-1)	NT->GRRef[NT->NGrp][i]=zz1;	
							else{
								sprintf(gx,"%s, line %d :Unknown monster ID in #DEFGRP: %s",fn,line,gy);
								Errx(gx);
							};
						};
						NLine(f1);
						line++;
						NT->NGrp++;
						break;
					case 2:
						{//#REQ
							int kind=0;
							zz1=SearchStr(Mon,gg,NMon);
							if(zz1==-1){
								zz1=SearchStr(Upg,gg,NUpg);
								if(zz1==-1){
									zz1=SearchStr(grp,gg,ngrp);
									if(zz1==-1){
										sprintf(gx,"%s, line %d :Unknown ID: %s",fn,line,gg);
										Errx(gx);
									}else kind=2;
								}else kind=1;
							};
							z=fscanf(f1,"%d",&parm1);
							int nai=NT->N_AI_Levels;
							int nar=NT->N_AI_Req[nai];
							if(nar){
								sAI_Req* sar=NT->AI_Req[nai];
								NT->AI_Req[nai]=new sAI_Req[nar+1];
								memcpy(NT->AI_Req[nai],sar,nar*sizeof sAI_Req);
								free(sar);
								NT->N_AI_Req[nai]++;
							}else{
								NT->AI_Req[nai]=new sAI_Req[nar+1];
								NT->N_AI_Req[nai]++;
							};
							sAI_Req* ARE=&NT->AI_Req[nai][nar];
							ARE->Kind=kind;
							ARE->ObjID=zz1;
							ARE->Amount=parm1;
							NLine(f1);
							line++;
						};
						break;
					case 3:
						{//#TODO
							int kind=0;
							int CKind=0;
							word CID;
							zz1=SearchStr(grp,gg,ngrp);
							if(zz1!=-1){
								CKind=2;
								CID=zz1;
								z=fscanf(f1,"%s",gg);
							};
							zz1=SearchStr(Mon,gg,NMon);
							if(zz1==-1){
								zz1=SearchStr(Upg,gg,NUpg);
								if(zz1==-1){
									sprintf(gx,"%s, line %d :Unknown ID: %s",fn,line,gg);
									Errx(gx);
								}else kind=1;
							};
							z=fscanf(f1,"%d%d%d",&parm1,&parm2,&parm3);
							int nai=NT->N_AI_Levels;
							int ndr=NT->N_AI_Devlp[nai];
							if(ndr){
								sAI_Devlp* sar=NT->AI_Devlp[nai];
								NT->AI_Devlp[nai]=new sAI_Devlp[ndr+1];
								memcpy(NT->AI_Devlp[nai],sar,ndr*sizeof sAI_Devlp);
								free(sar);
								NT->N_AI_Devlp[nai]++;
							}else{
								NT->AI_Devlp[nai]=new sAI_Devlp[ndr+1];
								NT->N_AI_Devlp[nai]++;
							};
							sAI_Devlp* ARE=&NT->AI_Devlp[nai][ndr];
							ARE->Kind=kind;
							ARE->ObjID=zz1; 
							ARE->Amount=parm1;
							ARE->GoldPercent=div(parm2<<8,100).quot;
							ARE->AtnPercent=div(parm3*32768,100).quot;
							if(CKind){
								ARE->ConID=CID;
								ARE->ConKind=2;
							}else{
								ARE->ConID=zz1;
								ARE->ConKind=0;
							};
							NLine(f1);
							line++;
						};
						break;
					};
				}else{
					NLine(f1);
					line++;
				};
			};
			
		};
	}while(z!=EOF&&mode!=255);
	for(int p=0;p<ngrp;p++)free(grp[p]);
};
//void LoadAllNations(){
//	
//};