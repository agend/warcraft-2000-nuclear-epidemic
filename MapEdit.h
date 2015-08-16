void LoadMediaFile();
void CreateCMap();
void PutPoint(int x,int y,int DX,byte Type);
extern bool MEditMode;
extern byte EditMedia;
extern byte CMap[256][256];
void SetTiles(int x,int y,int DX);