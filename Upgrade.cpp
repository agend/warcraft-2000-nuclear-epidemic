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

Upgrade::Upgrade(){
	NUpgrades=0;
};
void PerformUpgradeLink(OneObject* OBJ);
void OneObject::PerformUpgrade(word NewU){
	if(Nat->UPG.NUpgrades<=NewU||!Ready)return;
	word NUP=Ref.General->NUpgrades;
	word* UPL=Ref.General->Upg;
	bool canU=false;
	for(int pp=0;pp<NUP;pp++)if(UPL[pp]==NewU)canU=true;
	if(!canU)return;
	if(!Nat->UPG.utp[NewU]->Enabled)return;
	if(!CheckCost(NNUM,1,Nat->UPG.utp[NewU]->Cost))return;
	Order1* Or1=GetOrdBlock();
	Or1->PrioryLevel=0;
	Or1->NextOrder=LocalOrder;
	Or1->OrderType=75;//Upgrade
	Or1->OrderTime=0;
	Or1->DoLink=&PerformUpgradeLink;
	Or1->info.PUpgrade.NewUpgrade=NewU;
	Or1->info.PUpgrade.Stage=1;
	Or1->info.PUpgrade.NStages=Nat->UPG.utp[NewU]->Time;
	//Nat->UPG.utp[NewU]->Stage=1;
	Order1* LOR=LocalOrder;
	if(int(InLineCom))FreeAsmLink();
	LocalOrder=Or1;
	//OrderReport=NULL;
	//MessageKind=0;
	//Sender=0xFFFF;
	SimpleUTP* SU=Nat->UPG.utp[NewU];
	//if(SU->OneTime)
	if(SU->OneTime){
		SU->Done=true;
		SU->Enabled=false;
	};
	Ustage=0;
	NUstages=SU->Time;
	Ready=false;
};	
void PerformUpgradeLink(OneObject* OBJ){
	word OI=OBJ->LocalOrder->info.PUpgrade.NewUpgrade;
	Nation* NT=OBJ->Nat;
	byte NI=NT->NNUM;
	SimpleUTP* SU=NT->UPG.utp[OI];
	if(!CheckCost(NI,1,SU->Cost))return;
	OBJ->LocalOrder->info.PUpgrade.Stage++;
	OBJ->Ustage=OBJ->LocalOrder->info.PUpgrade.Stage;
	if(OBJ->LocalOrder->info.PUpgrade.Stage>=OBJ->LocalOrder->info.PUpgrade.NStages){
		OBJ->Ready=true;
		for(int i=0;i<SU->NLinks;i++)
			NT->UPG.utp[SU->Links[i]]->Enabled=true;
		if(SU->NLinks)SU->Enabled=false;
		for(i=0;i<SU->NAuto;i++)
			NT->UPG.utp[SU->AutoPerf[i]]->DoUpgrade(NI,OBJ);
		RESRC[NT->NNUM][1]-=SU->Cost;
		SU->DoUpgrade(NT->NNUM,OBJ);
		SU->Finished=true;
		if(int(OBJ->LocalOrder)){
			Order1* Or1=OBJ->LocalOrder->NextOrder;
			OBJ->FreeOrdBlock(OBJ->LocalOrder);
			OBJ->LocalOrder=Or1;
			OBJ->Important=false;
			return;
		};
	};
};
void SimpleUTP::DoUpgrade(byte NI,OneObject *OB){
	switch(Kind){
	case 1:{
			UTP1* ut1=(UTP1*)this;
			RESADD[NI][ut1->ResType]+=ut1->AddVal;
		};
		break;
	case 2:{
			UTP2* ut2=(UTP2*)this;
			//CharID:
			//0-Damage
			//1-Health
			//2-AttackRange
			Visuals* VS=(Visuals*) NATIONS[NI].Mon[ut2->MID];
			switch(ut2->CharID){
				case 0://Damage
					VS->info.Basic.MinDamage+=ut2->AddVal;
					//VS->info.Basic.MaxDamage+=ut2->AddVal;
					break;
				case 1://Health
					VS->info.Basic.MaxLife+=ut2->AddVal;
					break;
				case 2://attack range
					if(VS->info.Basic.AttackRange>1)
						VS->info.Basic.AttackRange+=ut2->AddVal;
					break;
				case 3://Shield
					VS->info.Basic.MaxShield+=ut2->AddVal;
					if(VS->info.Basic.MaxShield>99)
						VS->info.Basic.MaxShield=99;
					break;
				case 4://Visibility radius
					VS->SpotSize+=ut2->AddVal;
					break;
				case 5://Refresh life
					VS->RefreshLife=true;
					break;
			};
			for(int i=0;i<MAXOBJECT;i++){
				OneObject* OB=Group[i];
				if(int(OB)&&OB->Ref.Visual==VS){
					OB->MaxLife=
						VS->info.Basic.MaxLife;
					OB->Life=
						VS->info.Basic.MaxLife;
					OB->SpotSize=
						VS->SpotSize;
				};
			};
		};
		break;
	case 3:{
		UTP3* ut3=(UTP3*)this;
		Visuals* GO=(Visuals*)NATIONS[NI].Mon[ut3->FinalMID];
		OB->Ref.Visual=GO;
		OneObject* G=OB;
		G->DefaultSettings(GO);
		while(G->LocalOrder){
			Order1* ord1=G->LocalOrder->NextOrder;
			G->FreeOrdBlock(G->LocalOrder);
			G->LocalOrder=ord1;
		};
		G->NIndex=ut3->FinalMID;
		G->Teleport=false;
		G->capTeleport=GO->Teleport;
		G->VisRadius=GO->VisRadius;
		G->VisSpots=GO->VisSpots;
		G->SpotType=GO->SpotType;
		G->SpotSize=GO->SpotSize;
		G->DangerZone=GO->DangerZone;
		G->NoSearchVictim=GO->NoSearchVictim;
		G->NoAnswer=GO->NoAnswer;
		G->NeedNoHelp=GO->NeedNoHelp;
		G->Ready=true;
		G->wepX=GO->wepX;
		G->wepY=GO->wepY;
		G->MaxDelay=GO->delay;
		G->delay=0;
		G->NearBase=0xFFFF;
		G->capBase=GO->cpbBase;
		G->RStage=0;
		G->RType=0;
		G->RAmount=0;
		G->AnmGoKind=1;
		G->capBuild=GO->cpbBuild;
		G->GroupIndex=NULL;
		G->cpbMoving=GO->cpbMoving;
		G->AbRes=0;
		if(GO->AGold)G->AbRes|=2;
		if(GO->AWood)G->AbRes|=4;
		if(!GO->SizeX)GO->SizeX=1;
		if(!GO->SizeY)GO->SizeY=1;
		G->Lx=GO->SizeX;
		G->Ly=GO->SizeY;
		G->TempFlag=false;
		G->Mobilised=false;
		G->Wars=NULL;
		G->Selected=false;
		G->Borg=false;
		G->Life=GO->info.Basic.MaxLife;
		G->MaxLife=GO->info.Basic.MaxLife;
		G->Ref.Visual=GO;
		G->Push=false;
		memset(&(G->ARegs),0,sizeof G->ARegs);
		G->LoadAnimation(0,0,0);
		G->LoadCurAnm(0);
		G->LocalOrder=NULL;
		G->Attack=false;
		//G->OrderReport=NULL;
		//G->MessageFlags=0;
		G->PrioryLevel=0;
		//G->MessageKind=0;
		G->Ticks=0;
		G->TicksPerChange=10;
		G->Wait=0;
		G->Addx=0;
		G->Addy=0;
		G->Npush=0;
		G->StandTime=100;
		G->Sdoxlo=false;
		G->Weap=GO->Weap;}
		break;
	case 4://enable strange weapon
		UTP3* ut3=(UTP3*)this;
		Nation* NT=&NATIONS[NI];
		NT->SWP[ut3->FinalMID].Enabled=true;
		ut3->Enabled=false;
		break;
	};
};