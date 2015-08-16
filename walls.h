#pragma pack(1)
struct WallDiscr{
	union
    {
		struct{
			int Good[16];
			int Bad[16];
			int Broken[16];
		};
			int All[48];
	};
};
struct WallCell{
	byte x;
	byte y;
	word Stage;
	word Tile;
	word Health;
	word MaxHealth;
	word WorkerID;
	byte WorkTime;
	byte WorkNeed;
	byte DamNeed;
	word BuildProgress;
};
class WallCluster{
public:
	int ClusterSize;
	WallCell* Cells;
	byte OwnerID;
	byte Type;
	int Index;
	WallCluster();
void AddWall(byte x,byte y);
void PreArrangeTiles();
void ArrangeTiles();
int CheckPoint(byte x,byte y);
void ShowCluster();
void Stand();
void Init();
int	GetDataSize();
void CreateData(byte NI,byte* lpData);
int FindWorkPoint(byte x,byte y,word ID);
int FindDamagePoint(byte x,byte y,word ID);
};
class GWSys{
public:
	WallCluster* GWC;
	int MaxWall;
	GWSys();
	word AddCluster(byte* Data);
	void ProcessClusters();
	void BuildCell(byte x,byte y,byte Owner,byte Incr,word WorkerID);
	void DamageCell(byte x,byte y,byte Owner,word Incr);
};
extern int NDWalls;
extern int NDWBUF[8];
extern word Links[256][256];
extern word LIndex[256][256];
extern WallDiscr Walls[2];
//GLOBAL WALLS SYSTEM
extern GWSys GWALLS;
extern WallCluster TMPCluster;
extern bool BuildWall;
extern bool WStarted;
void SetBuildWallMode();
void WallHandleMouse(int x,int y);
void LoadWalls();