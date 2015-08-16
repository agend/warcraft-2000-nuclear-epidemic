class StrangeWeapon;
typedef void SWHandle(StrangeWeapon* SW);
class StrangeWeapon{
public:
	bool Enabled;
	int x;
	int y;
	word OwnerID;
	word OwnerSerial;
	byte MagAmount;
	byte MagSpeed;
	byte MagConsume;
	word MagMaxConsume;
	byte Kind;
	byte NNUM;
	SWHandle* SWH;
	union{
		struct{
			int r;
			int r1;
		}MExp;
		struct{
			int Radius1;
			int Radius2;
			int r1;
			int r2;
			int vr1;
			int vr2;
			int NR1;
			int NR2;
			word PID;
			word PSerialID;
		}HugeBomb;
	};
	StrangeWeapon();
	//void CreateBlizzard
	void CreateHugeBomb(int x,int y,int r1,int r2,int time,word MagID);
	void CreateFireSea(int x,int y,word MagID);
	void CreateBlizzard(int x,int y,word MagID);
	void CreateMExplosion(int ex,int ey,word MagID);
	void CreateMExplosion1(int ex,int ey,word MagID);
	void CreateFireWall(int ex,int ey,word MagID);
	void CreateSuperVision(int ex,int ey,word MagID);
	void CreateFlyUP(int ex,int ey,int er1,int er2,int time,word MagID);
	void CreateNucl(int x,int y,int r1,int r2,int time);
	void CreateFog(int x,int y,int r1,int r2,int time);
	void HandleExpl();
};
extern StrangeWeapon SWEAP[64];