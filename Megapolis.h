#define N_KINDS 5
class Cell8x8{
public:
	union{
		struct{
			byte Peasants;//кол-во крестьян на этой клетке
			byte Towers;//------//------башен
			byte Warriors;
			byte Farms;
			byte Buildings;
			byte Fly;
			byte WaterPeas;
			byte WaterObj;
		};
		byte UnitsAmount[8];
	};
	//byte P_MoveUp;//кол-во пересечений верхней границы
	//byte P_MoveDn;
	//byte P_MoveLf;
	//byte P_MoveRi;
	//byte Protection;  //=4*Towers+Warriors
	//byte Agressivity; //=Warriors
	//byte Importance;  //=Farms+Buildings
	word Neighbor;    //(0x001)  bit0 - UP
					  //(0x002)  bit1 - UP-RIGHT
					  //(0x004)  bit2 - RIGHT
					  //(0x008)  bit3 - DN-RIGHT
					  //(0x010)  bit4 - DN
					  //(0x020)  bit5 - DN-LEFT
					  //(0x040)  bit6 - LEFT
					  //(0x080)  bit7 - UP-LEFT
					  //(0x100)  bit8 - UP+2
					  //(0x200)  bit9 - RIGHT+2
					  //(0x400)  bit10- DN+2
					  //(0x800)  bit11- RIGHT+2
	//byte Resrv[3];
	//word Enemy;
	//word EnemyTime;
	Cell8x8();
};
typedef Cell8x8 CellsInfo [64][64];
typedef CellsInfo TotalCInfo [8];
extern TotalCInfo  TCInf;
class CityProject{
public:

};
class ProposedProject{
public:
	byte PKind;//=0-Monster,==1-Upgrade
	word NIndex;//номер типа монстра/Upgrade index
	word ProducerIndex;//индекс свободного производителя
	byte Percent;//процент денег, употребляемый на производство
	int Cost;//цена 
	int Useful[NBRANCH];
};
struct BuildProject{
	bool Used:1;
	bool PlaceFound:1;
	bool Founded:1;
	bool PeonsCalled:1;
	word LastStage;
	word Progress;
	int  x;
	int  y;
	char Lx;
	char Ly;
	word Index;
	word Type;
	word Producer;
	byte Kind;
};
class CityCell{
public:
	int x,y;
	word WorkTimes;//==0xFFFF if inside the city
	CityCell* NextCell;
};
class CityCluster{
public:
	CityCell* FirstCell;
	void AddCell(int x,int y);
};
typedef int Functional(int* x,int* y,int Lx,int Ly,byte NI);
class City{
public:
	Nation* Nat;
	byte NI;
	int InX,InY,InX1,InY1,OutX,OutY,OutX1,OutY1;
	int TryTimes;
	int ExpandSize;
	int Amount;
	int GoldPeasPart;
	int WoodPeasPart;
	int BuildPeasPart;
	word KindMax[N_KINDS];//макс. кол-во тварей типа Kind
	//word *MKList[N_KINDS];//массив указателей на массивы с индексами монстров данного типа
	Functional* FNC[N_KINDS];//массив функционалов для выбора места, если место вообще не подходит, то возвр. -1
	word BranchPart[NBRANCH];
	word UTNum;//amount of units types
	word UnitAmount[512];//Amount of units of some kind
	word ReadyAmount[512];
	word PRPIndex[512];//index of project to produce this type of monster,if not proposed,0xFFFF
	word UPGIndex[1024];
	word UnBusyAmount[512];//сколько модулей простаивает
	word Producer[512];
	bool PresentProject:1;
	bool IntellectEnabled:1;
	BuildProject BPR;
	word NProp;
	ProposedProject Prop[100];
	Crowd CROWDS[10];
	Crowd Defence;
	word EnemyList[32];
	void CreateCity(int x,int y,byte NI);
	bool CheckTZone(int x,int y,int Lx,int Ly);
	bool TryToFindPlace(int* x,int* y,int Lx,int Ly,byte Kind);
	void EnumUnits();
	void AddProp(byte NI,word NIN,GeneralObject* GO,word prop);
	void AddUpgr(byte NI,word NIN,SimpleUTP* UT,word prod);
	void EnumProp();
	void ProcessCreation();
	void HelpMe(word ID);
	void HandleFly();
	word FindNeedProject();
	void RefreshAbility();
	void CloseCity();
};
class Forces{
public:
	int MinX;
	int MinY;
	int MaxX;
	int MaxY;
	int Damage;
	int Life;
	int Shield;
	int Protection;
	int Population;
	int StrongFactor;
	int CenterX;
	int CenterY;
	int NBuild;
	void SetDefaults();
};
extern Forces NForces[8];
extern int BestForce;
extern int BestNat;
extern int NextBest;
void OutCInf(byte x,byte y);
extern int COUNTER;