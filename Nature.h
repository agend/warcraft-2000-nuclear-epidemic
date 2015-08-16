class ResDiscr{
public:
	bool Enabled;
	char Name[32];
	byte Amount;
	byte MotionKind;
	byte WorkKind;
	byte Portion;
	int AnmStandKind;
	int AnmGoKind;
	int Multi;
	int Tiles [16][4];
	ResDiscr();
};
byte GetResourceKind(byte x,byte y);
void LoadRDS();
void CreateResMap();
extern byte rtmap[256][256];//resource type map
extern byte ramap[256][256];//resource amount map
extern int RESRC[8][8];
extern int RESADD[8][8];
bool CheckCost(byte NI,byte ResID,word Amount);
extern ResDiscr RDS[8];
bool CheckOilSpot(byte x,byte y);
void CreateOilSpot(byte x,byte y);
void ShowOilSpot(int x,int y);
void CreateOilSpotAnyway();
void ShowResScreen();
extern word OILMAP[128][128];