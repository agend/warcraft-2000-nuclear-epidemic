//MULTIPLAYER ORGANIZATION
#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mgraph.h"
#include "mouse.h"
#include "menu.h"
#include "MapDiscr.h"
#include "math.h"
#include "walls.h"
#include "FlyObj.h"
extern int RESRC[8][8];
extern int tmtmt;
extern word rpos;
word GetOwner(int x,int y);
//Nations presence array
byte NPresence[64][64];
Nation NATIONS[8];
byte MyNation;
word* Selm[8];
word* SerN[8];
word NSL[8];
bool CmdDone[8192];
typedef  bool CHOBJ(OneObject* OB,int N);
void CreateUnit(Nation* NT,byte x,byte y,word ID);
//Execute buffer 
DWORD Signatur[2049];
byte ExBuf[8192];
int EBPos;

//
void InitEBuf(){
	EBPos=0;
};

//[1][ni][x][y][x1][y1]
void CmdCreateSelection(byte NI,byte x,byte y,byte x1,byte y1){
	ExBuf[EBPos]=1;
	ExBuf[EBPos+1]=NI;
	ExBuf[EBPos+2]=x;
	ExBuf[EBPos+3]=y;
	ExBuf[EBPos+4]=x1;
	ExBuf[EBPos+5]=y1;
	EBPos+=6;
};
//[2][ni][x][y]
void CmdSendToXY(byte NI,byte x,byte y){
	ExBuf[EBPos]=2;
	ExBuf[EBPos+1]=NI;
	ExBuf[EBPos+2]=x;
	ExBuf[EBPos+3]=y;
	EBPos+=4;
};
//[3][ni][w:ObjID]
void CmdAttackObj(byte NI,word ObjID){
	ExBuf[EBPos]=3;
	ExBuf[EBPos+1]=NI;
	*(word*)(&ExBuf[EBPos+2])=ObjID;
	EBPos+=4;
};
//[4][ni][x][y][w:Type]
void CmdCreateTerrain(byte NI,byte x,byte y,word Type){
	ExBuf[EBPos]=4;
	ExBuf[EBPos+1]=NI;
	ExBuf[EBPos+2]=x;
	ExBuf[EBPos+3]=y;
	*(word*)(&ExBuf[EBPos+4])=Type;
	EBPos+=6;
};
//[5][ni][x][y][w:Type]
void CmdCreateBuilding(byte NI,byte x,byte y,word Type){
	ExBuf[EBPos]=5;
	ExBuf[EBPos+1]=NI;
	ExBuf[EBPos+2]=x;
	ExBuf[EBPos+3]=y;
	*(word*)(&ExBuf[EBPos+4])=Type;
	EBPos+=6;
};
//[6][ni][w:ObjID]
void CmdProduceObj(byte NI,word Type){
	ExBuf[EBPos]=6;
	ExBuf[EBPos+1]=NI;
	*(word*)(&ExBuf[EBPos+2])=Type;
	EBPos+=4;
};
//[7][ni][index]
void CmdMemSelection(byte NI,byte Index){
	ExBuf[EBPos]=7;
	ExBuf[EBPos+1]=NI;
	ExBuf[EBPos+2]=Index;
	EBPos+=3;
};
//[8][ni][Index]
void CmdRememSelection(byte NI,byte Index){
	ExBuf[EBPos]=8;
	ExBuf[EBPos+1]=NI;
	ExBuf[EBPos+2]=Index;
	EBPos+=3;
};
//[9][ni][ObjID]
void CmdBuildObj(byte NI,word ObjID){
	ExBuf[EBPos]=9;
	ExBuf[EBPos+1]=NI;
	*(word*)(&ExBuf[EBPos+2])=ObjID;
	EBPos+=4;
};
//[0A][NI][kind][owner][count:16][x1][y1][x2][y2]...
//building wall
void CmdBuildWall(byte NI){
	if(!TMPCluster.ClusterSize)return;
	ExBuf[EBPos]=0xA;
	ExBuf[EBPos+1]=NI;
	TMPCluster.CreateData(NI,&ExBuf[EBPos+2]);
	EBPos+=TMPCluster.GetDataSize()+2;
};
//[0B][NI][LINK INDEX]
void CmdRepairWall(byte NI,word LIN){
	ExBuf[EBPos]=0xB;
	ExBuf[EBPos+1]=NI;
	*(word*)(&ExBuf[EBPos+2])=LIN;
	EBPos+=4;
};
//[0C][NI][LINK INDEX:16]
void CmdDamageWall(byte NI,word LIN){
	ExBuf[EBPos]=0xC;
	ExBuf[EBPos+1]=NI;
	*(word*)(&ExBuf[EBPos+2])=LIN;
	EBPos+=4;
};
//[0D][NI][x][y][ResID]
void CmdTakeRes(byte NI,byte x,byte y,byte ResID){
	ExBuf[EBPos]=0xD;
	ExBuf[EBPos+1]=NI;
	ExBuf[EBPos+2]=x;
	ExBuf[EBPos+3]=y;
	ExBuf[EBPos+4]=ResID;
	EBPos+=5;
};
//[0E][NI][UpgradeIndex:16]
void CmdPerformUpgrade(byte NI,word UI){
	ExBuf[EBPos]=0xE;
	ExBuf[EBPos+1]=NI;
	*(word*)(&ExBuf[EBPos+2])=UI;
	EBPos+=4;
};
//[0F][NI][OID:16]
void CmdGetOil(byte NI,word UI){
	ExBuf[EBPos]=0xF;
	ExBuf[EBPos+1]=NI;
	*(word*)(&ExBuf[EBPos+2])=UI;
	EBPos+=4;
};
//[10][NI][x][y][x1][y1][Kind]
void CmdCreateKindSelection(byte NI,byte x,byte y,byte x1,byte y1,byte Kind){
	ExBuf[EBPos]=16;
	ExBuf[EBPos+1]=NI;
	ExBuf[EBPos+2]=x;
	ExBuf[EBPos+3]=y;
	ExBuf[EBPos+4]=x1;
	ExBuf[EBPos+5]=y1;
	ExBuf[EBPos+6]=Kind;
	EBPos+=7;
};
//[11][NI][x][y][x1][y1][Type]
void CmdCreateTypeSelection(byte NI,byte x,byte y,byte x1,byte y1,word Type){
	ExBuf[EBPos]=17;
	ExBuf[EBPos+1]=NI;
	ExBuf[EBPos+2]=x;
	ExBuf[EBPos+3]=y;
	ExBuf[EBPos+4]=x1;
	ExBuf[EBPos+5]=y1;
	*(word*)(&ExBuf[EBPos+6])=Type;
	EBPos+=8;
};
//[12][ni][x:16][y:16][x1:16][y1:16]
void CmdCreateGoodSelection(byte NI,word x,word y,word x1,word y1){
	bool Addon=false;
	if(GetKeyState(VK_SHIFT)&0x8000)Addon=true;
	ExBuf[EBPos]=18;
	ExBuf[EBPos+1]=NI;
	*(word*)(&ExBuf[EBPos+2])=x;
	*(word*)(&ExBuf[EBPos+4])=y;
	*(word*)(&ExBuf[EBPos+6])=x1;
	*(word*)(&ExBuf[EBPos+8])=y1;
	ExBuf[EBPos+10]=Addon;
	EBPos+=11;
};
//[13][NI][x][y][x1][y1][Kind]
void CmdCreateGoodKindSelection(byte NI,word x,word y,word x1,word y1,byte Kind){
	bool Addon=false;
	if(GetKeyState(VK_SHIFT)&0x8000)Addon=true;
	ExBuf[EBPos]=19;
	ExBuf[EBPos+1]=NI;
	*(word*)(&ExBuf[EBPos+2])=x;
	*(word*)(&ExBuf[EBPos+4])=y;
	*(word*)(&ExBuf[EBPos+6])=x1;
	*(word*)(&ExBuf[EBPos+8])=y1;
	ExBuf[EBPos+10]=Kind;
	ExBuf[EBPos+11]=Addon;
	EBPos+=12;
};
//[14][NI][x][y][x1][y1][Type]
void CmdCreateGoodTypeSelection(byte NI,word x,word y,word x1,word y1,word Type){
	bool Addon=false;
	if(GetKeyState(VK_SHIFT)&0x8000)Addon=true;
	ExBuf[EBPos]=20;
	ExBuf[EBPos+1]=NI;
	*(word*)(&ExBuf[EBPos+2])=x;
	*(word*)(&ExBuf[EBPos+4])=y;
	*(word*)(&ExBuf[EBPos+6])=x1;
	*(word*)(&ExBuf[EBPos+8])=y1;
	*(word*)(&ExBuf[EBPos+10])=Type;
	ExBuf[EBPos+12]=Addon;
	EBPos+=13;
};
//[15][NI][x][y]
void CmdSetDst(byte NI,byte x,byte y){
	ExBuf[EBPos]=21;
	ExBuf[EBPos+1]=NI;
	ExBuf[EBPos+2]=x;
	ExBuf[EBPos+3]=y;
	EBPos+=4;
};
//[16][NI][x][y]
void CmdSendToPoint(byte NI,byte x,byte y){
	ExBuf[EBPos]=22;
	ExBuf[EBPos+1]=NI;
	ExBuf[EBPos+2]=x;
	ExBuf[EBPos+3]=y;
	EBPos+=4;
};
//[17][ni][x][y]
void CmdAttackToXY(byte NI,byte x,byte y){
	ExBuf[EBPos]=23;
	ExBuf[EBPos+1]=NI;
	ExBuf[EBPos+2]=x;
	ExBuf[EBPos+3]=y;
	EBPos+=4;
};
//[18][ni]
void CmdStop(byte NI){
	ExBuf[EBPos]=24;
	ExBuf[EBPos+1]=NI;
	EBPos+=2;
};
//[19][ni]
void CmdStandGround(byte NI){
	ExBuf[EBPos]=25;
	ExBuf[EBPos+1]=NI;
	EBPos+=2;
};
//[1A][ni][x][y]
void CmdPatrol(byte NI,byte x,byte y){
	ExBuf[EBPos]=26;
	ExBuf[EBPos+1]=NI;
	ExBuf[EBPos+2]=x;
	ExBuf[EBPos+3]=y;
	EBPos+=4;
};
//[1B][ni][x][y]
void CmdRepair(byte NI,byte x,byte y){
	ExBuf[EBPos]=27;
	ExBuf[EBPos+1]=NI;
	ExBuf[EBPos+2]=x;
	ExBuf[EBPos+3]=y;
	EBPos+=4;
};
//[1C][ni][x][y]
void CmdGetResource(byte NI,byte x,byte y){
	ExBuf[EBPos]=28;
	ExBuf[EBPos+1]=NI;
	ExBuf[EBPos+2]=x;
	ExBuf[EBPos+3]=y;
	EBPos+=4;
};
//[1D][ni][x][y][kind]
void CmdComplexAttackPoint(byte NI,byte x,byte y,byte kind){
	ExBuf[EBPos]=29;
	ExBuf[EBPos+1]=NI;
	ExBuf[EBPos+2]=x;
	ExBuf[EBPos+3]=y;
	ExBuf[EBPos+4]=kind;
	EBPos+=5;
};
//[1E][ni][ID:16][kind]
void CmdComplexAttack(byte NI,word ID,byte kind){
	ExBuf[EBPos]=30;
	ExBuf[EBPos+1]=NI;
	*(word*)(&ExBuf[EBPos+2])=ID;
	ExBuf[EBPos+4]=kind;
	EBPos+=5;
};
//[1F][ni][ID:16]
void CmdSendToTransport(byte NI,word ID){
	ExBuf[EBPos]=31;
	ExBuf[EBPos+1]=NI;
	*(word*)(&ExBuf[EBPos+2])=ID;
	EBPos+=4;
};
//[20)][ni][x][y]
void CmdUnload(byte NI,byte x,byte y){
	ExBuf[EBPos]=32;
	ExBuf[EBPos+1]=NI;
	ExBuf[EBPos+2]=x;
	ExBuf[EBPos+3]=y;
	EBPos+=4;
};
//[21][ni]
void CmdDie(byte NI){
	ExBuf[EBPos]=33;
	ExBuf[EBPos+1]=NI;
	EBPos+=2;
};
//[22][ni][x][y]
void CmdContinueAttackPoint(byte NI,byte x,byte y){
	ExBuf[EBPos]=34;
	ExBuf[EBPos+1]=NI;
	ExBuf[EBPos+2]=x;
	ExBuf[EBPos+3]=y;
	EBPos+=4;
};
//[23][ni][x][y]
void CmdContinueAttackWall(byte NI,byte x,byte y){
	ExBuf[EBPos]=35;
	ExBuf[EBPos+1]=NI;
	ExBuf[EBPos+2]=x;
	ExBuf[EBPos+3]=y;
	EBPos+=4;
};
//[24][ni]
void CmdSitDown(byte NI){
	ExBuf[EBPos]=36;
	ExBuf[EBPos+1]=NI;
	EBPos+=2;
};
//[25][ni][x][y]
void CmdNucAtt(byte NI,byte x,byte y){
	ExBuf[EBPos]=37;
	ExBuf[EBPos+1]=NI;
	ExBuf[EBPos+2]=x;
	ExBuf[EBPos+3]=y;
	EBPos+=4;
};
//[26][ni][w:ObjID]
void CmdUnProduceObj(byte NI,word Type){
	ExBuf[EBPos]=38;
	ExBuf[EBPos+1]=NI;
	*(word*)(&ExBuf[EBPos+2])=Type;
	EBPos+=4;
};
//[27][ni][w:ObjID]
void CmdSetRprState(byte NI,byte State){
	ExBuf[EBPos]=39;
	ExBuf[EBPos+1]=NI;
	ExBuf[EBPos+2]=State;
	EBPos+=3;
};
//[28][ni][ID:32][Length:8][Name...]
void CmdSaveNetworkGame(byte NI,int ID,char* Name){
	ExBuf[EBPos]=40;
	ExBuf[EBPos+1]=NI;
	*(int*)(&ExBuf[EBPos+2])=ID;
	byte Len=strlen(Name);
	ExBuf[EBPos+6]=Len;
	strcpy((char*)(&ExBuf[EBPos+7]),Name);
	EBPos+=7+Len;
};
//[2A][ni][ID:32][Length:8][Name...]
void CmdLoadNetworkGame(byte NI,int ID,char* Name){
	ExBuf[EBPos]=41;
	ExBuf[EBPos+1]=NI;
	*(int*)(&ExBuf[EBPos+2])=ID;
	byte Len=strlen(Name);
	ExBuf[EBPos+6]=Len;
	strcpy((char*)(&ExBuf[EBPos+7]),Name);
	EBPos+=7+Len;
};
//[2B][ni][ID]
void CmdChooseSelType(byte NI,word ID){
	ExBuf[EBPos]=42;
	ExBuf[EBPos+1]=NI;
	*(word*)(&ExBuf[EBPos+2])=ID;
	EBPos+=4;
};
//[2C][ni][ID]
void CmdChooseUnSelType(byte NI,word ID){
	ExBuf[EBPos]=43;
	ExBuf[EBPos+1]=NI;
	*(word*)(&ExBuf[EBPos+2])=ID;
	EBPos+=4;
};
void InitSelection(){
	int sz=(MaxObject>>2)+1;
	if(sz>2048)sz=2048;
	__asm{
		push	edi
		mov		ecx,sz
		cld
		xor		eax,eax
		mov		edi,offset CmdDone
		rep		stosd
		pop		edi
	};
};
void CreateSelection(byte NI,byte x,byte y,byte x1,byte y1){
	word MID;
	OneObject* OB;
	word ns=0;
	word Nsel=NSL[NI];
	word* ser=SerN[NI];
	word* SMon=Selm[NI];
	for(int i=0;i<Nsel;i++){
			MID=SMon[i];
			if(MID!=0xFFFF){
				OB=Group[MID];
				if(int(OB)&&OB->NNUM==NI&&OB->Serial==ser[i])OB->Selected=false;
			};
	};
	if(int(Selm[NI]))free(Selm[NI]);
	if(int(SerN[NI]))free(SerN[NI]);
	for(i=x;i<=x1;i++)
		for(byte j=y;j<=y1;j++){
			MID=Mops[j][i];
			if(MID!=0xFFFF){
				OB=Group[MID];
				if(int(OB)&&OB->NNUM==NI&&OB->Serial==ser[i])ns++;
			};
		};
	word nfs=SelectFly(NI,x,y,x1,y1,NULL,NULL,false);
	Selm[NI]=new word[ns+nfs];
	SerN[NI]=new word[ns+nfs];
	word* SM=Selm[NI];
	word* SN=SerN[NI];
	word ns1=ns;
	ns=0;
	for(i=x;i<=x1;i++)
		for(byte j=y;j<=y1;j++){
			MID=Mops[j][i];
			if(MID!=0xFFFF){
				OB=Group[MID];
				if(int(OB)&&OB->NNUM==NI&&!OB->Selected){
					SM[ns]=MID;
					SN[ns]=OB->Serial;
					ns++;
					OB->Selected=true;
				};
			};
		};
	SelectFly(NI,x,y,x1,y1,&SM[ns],&SN[ns],true);
	NSL[NI]=ns+nfs;
};
void CreateKindSelection(byte NI,byte x,byte y,byte x1,byte y1,byte Kind){
	word MID;
	OneObject* OB;
	word ns=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	for(int i=0;i<Nsel;i++){
			MID=SMon[i];
			if(MID!=0xFFFF){
				OB=Group[MID];
				if(int(OB)&&OB->NNUM==NI)OB->Selected=false;
			};
	};
	if(int(Selm[NI]))free(Selm[NI]);
	for(i=x;i<=x1;i++)
		for(byte j=y;j<=y1;j++){
			MID=Mops[j][i];
			if(MID!=0xFFFF){
				OB=Group[MID];
				if(int(OB)&&OB->NNUM==NI&&OB->Kind==Kind)ns++;
			};
		};
	Selm[NI]=new word[ns];
	word* SM=Selm[NI];
	ns=0;
	for(i=x;i<=x1;i++)
		for(byte j=y;j<=y1;j++){
			MID=Mops[j][i];
			if(MID!=0xFFFF){
				OB=Group[MID];
				if(int(OB)&&OB->NNUM==NI&&(!OB->Selected)&&OB->Kind==Kind){
					SM[ns]=MID;
					ns++;
					OB->Selected=true;
				};
			};
		};
	NSL[NI]=ns;
};
void CreateTypeSelection(byte NI,byte x,byte y,byte x1,byte y1,word Type){
	word MID;
	OneObject* OB;
	word ns=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	for(int i=0;i<Nsel;i++){
			MID=SMon[i];
			if(MID!=0xFFFF){
				OB=Group[MID];
				if(int(OB)&&OB->NNUM==NI)OB->Selected=false;
			};
	};
	if(int(Selm[NI]))free(Selm[NI]);
	for(i=x;i<=x1;i++)
		for(byte j=y;j<=y1;j++){
			MID=Mops[j][i];
			if(MID!=0xFFFF){
				OB=Group[MID];
				if(int(OB)&&OB->NNUM==NI&&OB->NIndex==Type)ns++;
			};
		};
	Selm[NI]=new word[ns];
	word* SM=Selm[NI];
	ns=0;
	for(i=x;i<=x1;i++)
		for(byte j=y;j<=y1;j++){
			MID=Mops[j][i];
			if(MID!=0xFFFF){
				OB=Group[MID];
				if(int(OB)&&OB->NNUM==NI&&(!OB->Selected)&&OB->NIndex==Type){
					SM[ns]=MID;
					ns++;
					OB->Selected=true;
				};
			};
		};
	NSL[NI]=ns;
};
bool FnKind(OneObject* OB,int N){
	return (OB->Kind==N);
};
bool FnType(OneObject* OB,int N){
	return (OB->NIndex==N);
};
bool PtInside(int x,int y,int x1,int y1,int xp,int yp){
	if(xp>=x&&xp<=x1&&yp>=y&&yp<=y1)return true;
	else return false;
};
void GetRect(OneObject* ZZ,int* x,int* y,int* Lx);
bool ObjInside(OneObject* OB,int xx,int yy,int xx1,int yy1){
	if(!OB)return false;
	int ox,oy,olx;
	GetRect(OB,&ox,&oy,&olx);
	int ox1=ox+olx-1;
	int oy1=oy+olx-1;
	if( PtInside(ox,oy,ox1,oy1,xx,yy)||
		PtInside(ox,oy,ox1,oy1,xx,yy1)||
		PtInside(ox,oy,ox1,oy1,xx1,yy)||
		PtInside(ox,oy,ox1,oy1,xx1,yy1)||
		PtInside(xx,yy,xx1,yy1,ox,oy)||
		PtInside(xx,yy,xx1,yy1,ox1,oy)||
		PtInside(xx,yy,xx1,yy1,ox,oy1)||
		PtInside(xx,yy,xx1,yy1,ox1,oy1))
		return true;
	else return false;
};
word GoodSelectFly(byte NI,int xr,int yr,int xr1,int yr1,word *Collect,word* Ser,bool WRITE,CHOBJ* FN,int NN);
void CreateGoodSelection(byte NI,word xx,word yy,word xx1,word yy1,CHOBJ* FN,int NN,bool Addon){
	int x=xx>>5;
	int y=yy>>5;
	int x1=xx1>>5;
	int y1=yy1>>5;
	if(x>1)x--;
	if(y>1)y--;
	if(x1<msx)x1++;
	if(y1<msy)y1++;
	//if(xx1&31)x1++;
	//if(yy1&31)y1++;
	word MID;
	OneObject* OB;
	word ns=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	word* ser=SerN[NI];
	if((!Addon)&&Nsel){
		for(int i=0;i<Nsel;i++){
				MID=SMon[i];
				if(MID!=0xFFFF){
					OB=Group[MID];
					if(int(OB)&&OB->NNUM==NI&&ser[i]==OB->Serial)OB->Selected=false;
				};
		};
		if(int(Selm[NI]))free(Selm[NI]);
		if(int(SerN[NI]))free(SerN[NI]);
		Selm[NI]=NULL;
		SerN[NI]=NULL;
	};
	for(int j=y;j<=y1;j++)
		for(int i=x;i<=x1;i++){
			MID=Mops[j][i];
			if(MID!=0xFFFF){
				OB=Group[MID];
				if(int(OB)&&OB->NNUM==NI&&((!FN)||(FN&&FN(OB,NN)))&&ObjInside(OB,xx,yy,xx1,yy1))ns++;
			};
		};
	int Olds=0;
	if(Addon)Olds=Nsel;
	word nfs=GoodSelectFly(NI,xx,yy,xx1,yy1,NULL,NULL,false,FN,NN);
	Selm[NI]=new word[Olds+ns+nfs];
	SerN[NI]=new word[Olds+ns+nfs];
	if(Addon&&Nsel){
		memcpy(Selm[NI],SMon,Olds<<1);
		memcpy(SerN[NI],ser,Olds<<1);
	};
	word* SM=Selm[NI];
	word* SR=SerN[NI];
	word ns1=ns;
	ns=Olds;
	for(j=y;j<=y1;j++)
		for(int i=x;i<=x1;i++){
			MID=Mops[j][i];
			if(MID!=0xFFFF){
				OB=Group[MID];
				if(int(OB)&&OB->NNUM==NI
					&&((!FN)||(FN&&FN(OB,NN)))&&ObjInside(OB,xx,yy,xx1,yy1)&&
					!OB->Selected){
					SM[ns]=MID;
					SR[ns]=OB->Serial;
					ns++;
					OB->Selected=true;
				};
			};
		};
	GoodSelectFly(NI,xx,yy,xx1,yy1,&SM[ns],&SR[ns],true,FN,NN);
	NSL[NI]=ns+nfs;
	if(Addon&&Nsel){
		free(SMon);
		free(ser);
	};
};
void RefreshSelected(byte NI){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	word* ser=SerN[NI];
	if(!Nsel)return;
	word MID;
	for(int k=0;k<Nsel;k++){
		MID=SMon[k];
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if(OB&&OB->Serial==ser[k]){
				SMon[RNSel]=SMon[k];
				ser[RNSel]=ser[k];
				RNSel++;
			};
		};
	};
	Nsel=RNSel;
	NSL[NI]=Nsel;
};
void SendSelectedToXY(byte NI,byte x,byte y){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	int Sqs=int(sqrt(Nsel));
	int Glx=Sqs;
	int Gly=Sqs;
	if(Glx*Gly>Nsel)Glx--;
	if(Glx*Gly<Nsel)Glx++;else{
		if(Glx*Gly>Nsel)Gly--;
		if(Glx*Gly<Nsel)Gly++;
	};
	int gx1=x-(Glx>>1);
	int gy1=y-(Gly>>1);
	if(gx1<=0)gx1=1;
	if(gy1<=0)gy1=1;
	if(gx1+Glx>msx)gx1=msx-Glx+1;
	if(gy1+Gly>msy)gy1=msy-Gly+1;
	int zx=gx1;
	int zy=gy1;
	for(int i=0;i<Nsel;i++){
		if(zx-gx1+1>Glx){zx=gx1;zy++;};
		MID=SMon[i];
		if(!(CmdDone[MID]||MID==0xFFFF)&&int(Group[MID]))
			Group[SMon[i]]->SendTo(zx,zy,16);
		zx++;
	};
};
void AttackToXY(byte NI,byte x,byte y){
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	int Sqs=int(sqrt(Nsel));
	int Glx=Sqs;
	int Gly=Sqs;
	if(Glx*Gly>Nsel)Glx--;
	if(Glx*Gly<Nsel)Glx++;else{
		if(Glx*Gly>Nsel)Gly--;
		if(Glx*Gly<Nsel)Gly++;
	};
	int gx1=x-(Glx>>1);
	int gy1=y-(Gly>>1);
	if(gx1<=0)gx1=1;
	if(gy1<=0)gy1=1;
	if(gx1+Glx>msx)gx1=msx-Glx+1;
	if(gy1+Gly>msy)gy1=msy-Gly+1;
	int zx=gx1;
	int zy=gy1;
	for(int i=0;i<Nsel;i++){
		if(zx-gx1+1>Glx){zx=gx1;zy++;};
		MID=SMon[i];
		if(!(CmdDone[MID]||MID==0xFFFF)&&int(Group[MID]))
			Group[SMon[i]]->SendTo(zx,zy,129);
		zx++;
	};
};
void AttackSelected(byte NI,word ObjID){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(MID!=0xFFFF&&int(Group[MID]))
			Group[MID]->AttackObj(ObjID,16);
	};
};
void BuildWithSelected(byte NI,word ObjID){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if((!(MID==0xFFFF||CmdDone[MID]))&&int(Group[MID]))
			if(Group[MID]->BuildObj(ObjID,16))
				CmdDone[MID]=true;
	};
};
void CreateTerrainMons(byte NI,byte x,byte y,word Type){
	//NATIONS[NI].CreateTerrainAt(x,y,Type);
	CreateUnit(&NATIONS[NI],x,y,Type);
};
void CreateBuilding(byte NI,byte x,byte y,word Type){
	if(!NATIONS[NI].CheckBuilding(Type,x,y))return;
	int j=NATIONS[NI].CreateBuilding(Type,x,y);
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(j>=0){
		for(int i=0;i<Nsel;i++){
			word MID=SMon[i];
			if(!(MID==0xFFFF||CmdDone[MID])){
				OneObject* OBJ=Group[MID];
				if(OBJ){
					if(int(OBJ)&&!OBJ->Sdoxlo)
					if(OBJ->BuildObj(j,16))
						CmdDone[MID]=true;
				};
			};
		};
	};
};
void ProduceObject(byte NI,word Type){
	int maxp=1000;
	int kk=-1;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	for(int k=0;k<Nsel;k++){
		word MID=SMon[k];
		if(!(MID==0xFFFF/*||CmdDone[MID]*/)){
			OneObject* OB=Group[SMon[k]];
			if(int(OB)){
				int pp=OB->CheckAbility(Type);
				if(pp==-1&&OB->Ready){
					OB->Produce(Type);
					CmdDone[MID]=true;
					return;
				};
				if(pp>0&&pp<maxp){
					maxp=pp;
					kk=k;
				};
			};
		};
	};
	if(kk!=-1){
		word MID=SMon[kk];
		if(!(MID==0xFFFF/*||CmdDone[MID]*/)){
			OneObject* OB=Group[MID];
			if(OB){
				OB->Produce(Type);
				CmdDone[MID]=true;
			};
		};
	};
};
void UnProduce(OneObject* OB,word Type){
	if(!(OB->capBuilding&&OB->LocalOrder))return;
	Order1* ORR=OB->LocalOrder;
	Order1* POR=NULL;
	do{
		if(ORR->OrderType==13){
			if(ORR->info.Produce.ObjIndex==Type){
				if(POR){
					POR->NextOrder=ORR->NextOrder;
				}else{
					OB->LocalOrder=ORR->NextOrder;
				};
				OB->FreeOrdBlock(ORR);
				GeneralObject* GO=OB->Nat->Mon[Type];
				for(int p=0;p<4;p++){
					int rt=GO->ResourceID[p];
					if(rt!=255){
						RESRC[OB->NNUM][rt]+=GO->ResAmount[p];
					};
				};
				return;
			};
		};
		POR=ORR;
		ORR=ORR->NextOrder;
	}while(ORR);
};
void UnProduceObject(byte NI,word Type){
	int maxp=0;
	int kk=-1;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	for(int k=0;k<Nsel;k++){
		word MID=SMon[k];
		if(!(MID==0xFFFF||CmdDone[MID])){
			OneObject* OB=Group[SMon[k]];
			if(int(OB)){
				int pp=OB->CheckAbility(Type);
				if(pp>0&&pp>maxp){
					maxp=pp;
					kk=k;
				};
			};
		};
	};
	if(kk!=-1){
		word MID=SMon[kk];
		if(!(MID==0xFFFF||CmdDone[MID])){
			OneObject* OB=Group[MID];
			if(OB){
				UnProduce(OB,Type);
				CmdDone[MID]=true;
			};
		};
	};
};
void MemSelection(byte NI,byte Index){
	SelSet[NI*10+Index].CreateFromSelection(NI);	
};
void RememSelection(byte NI,byte Index){
	SelSet[NI*10+Index].SelectMembers(NI);	
};
int CreateWall(byte NI,byte* lp){
	word ci=GWALLS.AddCluster(lp);
	word* dw=(word*)lp;
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return (dw[1]<<1)+5;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(!(MID==0xFFFF||CmdDone[MID])){
			OneObject* OB=Group[MID];
			if(OB&&OB->BuildWall(ci,16))CmdDone[MID]=true;
		};
	};
	return (dw[1]<<1)+5;
};
void RepairWall(byte NI,word ci){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(!(MID==0xFFFF||CmdDone[MID])){
			OneObject* OB=Group[MID];
			if(OB&&OB->BuildWall(ci,16))CmdDone[MID]=true;
		};
	};
};
void DamageWall(byte NI,word ci){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(!(MID==0xFFFF||CmdDone[MID])){
			OneObject* OB=Group[MID];
			if(OB&&OB->DamageWall(ci,16))CmdDone[MID]=true;
		};
	};
};
void TakeRes(byte NI,byte x,byte y,byte ResID){
int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(!(MID==0xFFFF&&CmdDone[MID])){
			OneObject* OB=Group[MID];
			if(OB&&OB->TakeResource(x,y,ResID,16))CmdDone[MID]=true;
		};
	};
};
void GetOil(byte NI,word OID){
int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(!(MID==0xFFFF&&CmdDone[MID])){
			OneObject* OB=Group[MID];
			if(OB&&OB->GetOil(OID,16))CmdDone[MID]=true;
		};
	};
};
void PerformUpgr(byte NI,word UI){
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(MID!=0xFFFF&&int(Group[MID]))
			Group[SMon[i]]->PerformUpgrade(UI);
	};
};
void SendToPoint(byte NI,byte x,byte y){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(MID!=0xFFFF&&int(Group[MID]))
			Group[MID]->SendTo(x,y,16);
	};
};
void SetDestination(byte NI,byte x,byte y){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(MID!=0xFFFF&&int(Group[MID]))
			Group[MID]->SetDstPoint(x,y);
	};
};
void Stopp(byte NI){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(MID!=0xFFFF&&int(Group[MID])){
			OneObject* OB=Group[MID];
			if(OB){
				OB->ClearOrders();
				OB->LoadAnimation(0,OB->AnmStandKind,0);
				OB->LoadCurAnm(0);
			};
		};
	};
};
void StandGround(byte NI){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(MID!=0xFFFF&&int(Group[MID])){
			OneObject* OB=Group[MID];
			OB->StandGround=true;
			if(!OB->Media){
				TrMap[OB->y][OB->x]=1;
			}else if (OB->Media==1){
				WMap[OB->y][OB->x]=1;
			};
		};
	};
};
void PatrolGroup(byte NI,byte x1,byte y1){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(MID!=0xFFFF&&int(Group[MID])){
			OneObject* OB=Group[MID];
			if(OB)OB->Patrol(OB->x,OB->y,x1,y1,0);
		};
	};
};
void GroupAttackPoint(byte NI,byte x,byte y,byte kind){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(MID!=0xFFFF&&int(Group[MID])){
			OneObject* OB=Group[MID];
			if(OB)OB->AttackPoint(x,y,kind,16);
		};
	};
};
void SendToTransp(byte NI,word ID){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(MID!=0xFFFF&&int(Group[MID])){
			OneObject* OB=Group[MID];
			if(OB)OB->GoToTransport(ID,16);
		};
	};
};
void UnloadTransp(byte NI,byte x1,byte y1){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(MID!=0xFFFF&&int(Group[MID])){
			OneObject* OB=Group[MID];
			if(OB)OB->UnloadPassangers(x1,y1);
		};
	};
};
void GroupComplexAttack(byte NI,word ID,byte kind){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(MID!=0xFFFF&&int(Group[MID])){
			OneObject* OB=Group[MID];
			if(OB)OB->ComplexAttack(ID,kind,16);
		};
	};
};
void DieSelected(byte NI){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(MID!=0xFFFF&&int(Group[MID])){
			OneObject* OB=Group[MID];
			if(OB){
				OB->InFire=true;
				OB->Die();
			};
		};
	};
};
void ContinueAttPoint(byte NI,byte x,byte y){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(MID!=0xFFFF&&int(Group[MID]))
			Group[MID]->ContinueAttackPoint(x,y,16);
	};
};
void ContinueAttWall(byte NI,byte x,byte y){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(MID!=0xFFFF&&int(Group[MID]))
			Group[MID]->ContinueAttackWall(x,y,16);
	};
};
void SitDown(byte NI){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(MID!=0xFFFF&&int(Group[MID])){
			OneObject* OB=Group[MID];
			if(OB)OB->MakeMeSit();
		};
	};
};
void UseNuclo(byte NI,byte x,byte y){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(MID!=0xFFFF&&int(Group[MID]))
			Group[MID]->NuclearAttack(x,y);
	};
};
void SetRepairState(byte NI,byte state){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(MID!=0xFFFF&&int(Group[MID])){
			OneObject* OB=Group[MID];
			if(OB&&OB->Ref.General->CanRepair)OB->Repair=state;
		};
	};
};
//*******************************************************************************//
//****************                                                 **************//
//*******                          SAVING IPX GAME                         ******//
//****************                                                 **************//
//*******************************************************************************//
void LoadGame(char* fnm);
void SaveGame(char* fnm,char* Messtr,int ID);
#define MaxSFNames 128
extern int sfVersion;
static int   NSFNames;
char* SFNames[MaxSFNames];
char* PLNames[MaxSFNames];
int   PLID[MaxSFNames];
//1. Creating list of save files
void ClearSFNames(){
	for(int i=0;i<NSFNames;i++){
		free(SFNames[i]);
		free(PLNames[i]);
	};
	NSFNames=0;
};
void InstallSFName(WIN32_FIND_DATA* FD){
	ResFile ff1=RReset(FD->cFileName);
	char nam[128];
	if(!IOresult()){
		int sig,lap;
		RBlockRead(ff1,&sig,4);
		if(sig=='WSF2'){
			RBlockRead(ff1,&sig,4);
			RBlockRead(ff1,&lap,4);
			if(sig==sfVersion&&NSFNames<MaxSFNames){
				int nlen=0;
				RBlockRead(ff1,&nlen,2);
				nlen=0;
				RBlockRead(ff1,&nlen,1);
				RBlockRead(ff1,nam,nlen);
				RClose(ff1);
				PLNames[NSFNames]=new char[strlen(nam)+1];
				SFNames[NSFNames]=new char[strlen(FD->cFileName)+1];
				PLID[NSFNames]=lap;
				strcpy(PLNames[NSFNames],nam);
				strcpy(SFNames[NSFNames],FD->cFileName);
				NSFNames++;
			}else RClose(ff1);
		}else RClose(ff1);
	};
};
void CreateSFList(){
	NSFNames=0;
	WIN32_FIND_DATA FD;
	HANDLE HF=FindFirstFile("*.sav",&FD);
	if(HF!=INVALID_HANDLE_VALUE){
		InstallSFName(&FD);
		while(FindNextFile(HF,&FD))InstallSFName(&FD);
	};
};
//2. Search for name of the game,returns -1 if name not found
int FindPLName(char* Name){
	for(int i=0;i<NSFNames;i++){
		if(!strcmp(PLNames[i],Name))return i;
	};
	return -1;
};
//2. Search for name of the save file,returns -1 if name not found
int FindSFName(char* SName){
	for(int i=0;i<NSFNames;i++){
		if(!strcmp(SFNames[i],SName))return i;
	};
	return -1;
};
//Searching for the best name of the save file with the given name
void FindBestSFName(char* SName,char* Name){
	int i=FindPLName(Name);
	if(i==-1){
		int j=0;
		char fname[128]="";
		do{
			sprintf(fname,"save%d.sav",j);
			i=FindSFName(fname);
			if(i==-1){
				strcpy(SName,fname);
				return;
			};
			j++;
		}while(true);
	}else{
		strcpy(SName,SFNames[i]);
		return;
	};
};
void SaveNetworkGame(byte NI,int ID,char* Name){
	CreateSFList();
	char fname[128];
	FindBestSFName(fname,Name);
	SaveGame(fname,Name,ID);
	ClearSFNames();
};
int FindNetGame(int ID,char* name){
	for(int i=0;i<NSFNames;i++){
		if(PLID[i]==ID&&!strcmp(name,PLNames[i]))return i;
	};
	return -1;
};
void IAmLeft();
void LoadNetworkGame(byte NI,int ID,char* Name){
	CreateSFList();
	int i=FindNetGame(ID,Name);
	if(i!=-1){
		LoadGame(SFNames[i]);
	}else{
		IAmLeft();
	};
	ClearSFNames();
};
void ChooseUnSelectType(byte NI,word ID){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	word* SNM=SerN[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if(OB&&OB->NIndex!=ID){
				SMon[RNSel]=SMon[i];
				SNM[RNSel]=SNM[i];
				RNSel++;
			}else{
				OB->Selected=false;
			};
		};
	};
	NSL[NI]=RNSel;
};
void ChooseSelectType(byte NI,word ID){
	int RNSel=0;
	word Nsel=NSL[NI];
	word* SMon=Selm[NI];
	word* SNM=SerN[NI];
	if(!Nsel)return;
	word MID;
	for(int i=0;i<Nsel;i++){
		MID=SMon[i];
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if(OB&&OB->NIndex==ID){
				SMon[RNSel]=SMon[i];
				SNM[RNSel]=SNM[i];
				RNSel++;
			}else{
				OB->Selected=false;
			};
		};
	};
	NSL[NI]=RNSel;
};
void ExecuteBuffer(){
	InitSelection();
	for(int p=0;p<8;p++)RefreshSelected(p);
	int pos=0;
	int len;
	char sss[128];
	while(pos<EBPos){
		byte cmd=ExBuf[pos];
		pos++;
		switch(cmd){
		case 1://Create selection
			CreateSelection(ExBuf[pos],ExBuf[pos+1],ExBuf[pos+2],
				ExBuf[pos+3],ExBuf[pos+4]);
			pos+=5;
			break;
		case 2://Send group to (X,Y)
			SendSelectedToXY(ExBuf[pos],ExBuf[pos+1],ExBuf[pos+2]);
			pos+=3;
			break;
		case 3://Attack object with group
			AttackSelected(ExBuf[pos],*(word*)(&ExBuf[pos+1]));
			pos+=3;
			break;
		case 4://Create terrain object
			CreateTerrainMons(ExBuf[pos],ExBuf[pos+1],ExBuf[pos+2],
				*(word*)(&ExBuf[pos+3]));
			pos+=5;
			break;
		case 5://create building
			CreateBuilding(ExBuf[pos],ExBuf[pos+1],ExBuf[pos+2],
				*(word*)(&ExBuf[pos+3]));
			pos+=5;
			break;
		case 6://produce object
			ProduceObject(ExBuf[pos],*(word*)(&ExBuf[pos+1]));
			pos+=3;
			break;
		case 7://member selection
			MemSelection(ExBuf[pos],ExBuf[pos+1]);
			pos+=2;
			break;
		case 8://remember selection
			RememSelection(ExBuf[pos],ExBuf[pos+1]);
			pos+=2;
			break;
		case 9://build or repair object
			BuildWithSelected(ExBuf[pos],*(word*)(&ExBuf[pos+1]));
			pos+=3;
			break;
		case 10:
			pos+=CreateWall(ExBuf[pos],&ExBuf[pos+1]);
			break;
		case 11:
			RepairWall(ExBuf[pos],*(word*)(&ExBuf[pos+1]));
			pos+=3;
			break;
		case 12:
			DamageWall(ExBuf[pos],*(word*)(&ExBuf[pos+1]));
			pos+=3;
			break;
		case 13:
			TakeRes(ExBuf[pos],ExBuf[pos+1],ExBuf[pos+2],ExBuf[pos+3]);
			pos+=4;
			break;
		case 14:
			PerformUpgr(ExBuf[pos],*(word*)(&ExBuf[pos+1]));
			pos+=3;
			break;
		case 15:
			GetOil(ExBuf[pos],*(word*)(&ExBuf[pos+1]));
			pos+=3;
			break;
		case 16://Create kind selection
			CreateKindSelection(ExBuf[pos],ExBuf[pos+1],ExBuf[pos+2],
				ExBuf[pos+3],ExBuf[pos+4],ExBuf[pos+5]);
			pos+=6;
			break;
		case 17://Create type selection
			CreateTypeSelection(ExBuf[pos],ExBuf[pos+1],ExBuf[pos+2],
				ExBuf[pos+3],ExBuf[pos+4],*(word*)(&ExBuf[pos+5]));
			pos+=7;
			break;
		case 18://good selection
			CreateGoodSelection(ExBuf[pos],
				*(word*)(&ExBuf[pos+1]),
				*(word*)(&ExBuf[pos+3]),
				*(word*)(&ExBuf[pos+5]),
				*(word*)(&ExBuf[pos+7]),
				NULL,0,ExBuf[pos+9]);
			pos+=10;
			break;
		case 19://good kind selection
			CreateGoodSelection(ExBuf[pos],
				*(word*)(&ExBuf[pos+1]),
				*(word*)(&ExBuf[pos+3]),
				*(word*)(&ExBuf[pos+5]),
				*(word*)(&ExBuf[pos+7]),
				&FnKind,ExBuf[pos+9],ExBuf[pos+10]);
			pos+=11;
			break;
		case 20://good type selection
			CreateGoodSelection(ExBuf[pos],
				*(word*)(&ExBuf[pos+1]),
				*(word*)(&ExBuf[pos+3]),
				*(word*)(&ExBuf[pos+5]),
				*(word*)(&ExBuf[pos+7]),
				&FnType,*(word*)(&ExBuf[pos+9]),ExBuf[pos+11]);
			pos+=12;
			break;
		case 21://set destination
			SetDestination(ExBuf[pos],ExBuf[pos+1],ExBuf[pos+2]);
			pos+=3;
			break;
		case 22://send to point
			SendToPoint(ExBuf[pos],ExBuf[pos+1],ExBuf[pos+2]);
			pos+=3;
			break;
		case 23://Send group to (X,Y)
			AttackToXY(ExBuf[pos],ExBuf[pos+1],ExBuf[pos+2]);
			pos+=3;
			break;
		case 24://STOP
			Stopp(ExBuf[pos]);
			pos++;
			break;
		case 25://STAND GROUND
			StandGround(ExBuf[pos]);
			pos++;
			break;
		case 26://Patrol
			PatrolGroup(ExBuf[pos],ExBuf[pos+1],ExBuf[pos+2]);
			pos+=3;
			break;
		case 28://
			break;
		case 29://Complex attack point
			GroupAttackPoint(ExBuf[pos],ExBuf[pos+1],ExBuf[pos+2],ExBuf[pos+3]);
			pos+=4;
			break;
		case 30://Complex attack object
			GroupComplexAttack(ExBuf[pos],*(word*)(&ExBuf[pos+1]),ExBuf[pos+3]);
			pos+=4;
			break;
		case 31://GoToTransport
			SendToTransp(ExBuf[pos],*(word*)(&ExBuf[pos+1]));
			pos+=3;
			break;
		case 32://Unload Transport
			UnloadTransp(ExBuf[pos],ExBuf[pos+1],ExBuf[pos+2]);
			pos+=3;
			break;
		case 33://Die selected
			DieSelected(ExBuf[pos]);
			pos++;
			break;
		case 34://attack point
			ContinueAttPoint(ExBuf[pos],ExBuf[pos+1],ExBuf[pos+2]);
			pos+=3;
			break;
		case 35://attack point
			ContinueAttWall(ExBuf[pos],ExBuf[pos+1],ExBuf[pos+2]);
			pos+=3;
			break;
		case 36://sit down
			SitDown(ExBuf[pos]);
			pos++;
			break;
		case 37:
			UseNuclo(ExBuf[pos],ExBuf[pos+1],ExBuf[pos+2]);
			pos+=3;
			break;
		case 38://produce object
			UnProduceObject(ExBuf[pos],*(word*)(&ExBuf[pos+1]));
			pos+=3;
			break;
		case 39:
			SetRepairState(ExBuf[pos],ExBuf[pos+1]);
			pos+=2;
			break;
		case 40://save network game
			len=ExBuf[pos+5];
			memcpy(sss,&ExBuf[pos+6],len);
			sss[len]=0;
			SaveNetworkGame(ExBuf[pos],*(int*)(&ExBuf[pos+1]),sss);
			pos+=6+len;
			break;
		case 41://load network game
			len=ExBuf[pos+5];
			memcpy(sss,&ExBuf[pos+6],len);
			sss[len]=0;
			LoadNetworkGame(ExBuf[pos],*(int*)(&ExBuf[pos+1]),sss);
			tmtmt=0;
			rpos=0;
			pos+=len+6;
			break;
		case 42://select type
			ChooseSelectType(ExBuf[pos],*(word*)(&ExBuf[pos+1]));
			pos+=3;
			break;
		case 43://select type
			ChooseUnSelectType(ExBuf[pos],*(word*)(&ExBuf[pos+1]));
			pos+=3;
			break;
		};
	};
	InitEBuf();
	memset(&NPresence[0][0],0,sizeof NPresence);
	for(int i=0;i<MaxObject;i++){
		OneObject* OB=Group[i];
		if(OB)NPresence[OB->y>>2][OB->x>>2]|=OB->NMask;
	};
};