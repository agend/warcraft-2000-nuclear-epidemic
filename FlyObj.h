class FlyCell{
public:
	word NFly;
	word FlyList[15];
	FlyCell();
	void GetForces(int x,int y,int *Fx,int *Fy,word MD);
};
extern FlyCell FlyMap[64][64];
void ShowFlyingMonsters();
word SelectFly(byte NI,int xp,int yp,int xp1,int yp1,word *Collect,word* Fly,bool WRITE);