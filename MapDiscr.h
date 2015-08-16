/*                    Map discription
 *    
 * This file describes map cells, animations, monsters, buildings,
 * flying monsters, on-water monsters...
 */
#pragma pack(1)
typedef unsigned short word;
#include "AntiBug.h"
#include "Icons.h"
#define NBRANCH 4
class Weapon;
class SelGroup;
//Описание одной клетки на карте(без картинки)
struct MapCell{
	//word Flags;		//Описывают состояние клетки
//	bool LandLock:1;	//Клетка недоступна для наземных
	bool WaterLock:1;	//Клетка недоступна для водных
	bool AirLock:1;		//Клетка недоступна для летающих
	bool TempLock:1;    //Признак временной недоступности
	byte LayerID;		//Нижний слой на карте-объект типа МЕСТОРОЖДЕНИЕ
    word MonsterID;		//Номер монстра в таблице объектов  
	word BuildingID;	//Номер здания в таблице объектов
	word FlyID;			//Летающий объект
//	word TileID;		//Номер картинки, описывающей поверхность
};//8 bytes

//Элемент в таблице объектов
struct ObjectRef{
	unsigned ObjID:8;	//Тип объекта
	unsigned Location:2;//Наземный/Водный/Летающий/Всех типов
	void* lpObj;		//Ссылка на объект
};
//Максимальное количество объектов
#define MaxObject 8192
//Массив объектов
typedef ObjectRef ObjArray[MaxObject];
//Описание анимации
struct OneSlide{
	word  FileID;   //Номер файла анимации
	small dx;		//Смещение спрайта по горизонтали
	small dy;		//-----/----- по вертикали
	word  spr;		//Номер спрайта в данном файле
	word SoundID;   //Номер звука
};
typedef OneSlide MovieType[256];
typedef MovieType* lpOneMovie ;
struct  Octant{
	byte count;
	byte ticks;
	small gdx;
	small gdy;
	OneSlide* Movie;
};
typedef Octant Animation[8];
//Информация о анимации и ее применении
struct MoreAnimation{
	Octant* Anm;
	word WhatFor;		//Как будет использоваться данная анимация
	word Kind;          // 
	//word Reserved;
};
typedef MoreAnimation AnimArray[32]; 
typedef word WordArray[32768];
typedef WordArray* lpWordArray;
struct ObjIcon{
	//word FileID;
	word spr;
};
#include "Upgrade.h"
//Описание типов объектов(идентификаторы)
#define EmptyID 0
//#define MonsterID 1
class Crowd{
public:
	char* Message;
	word Amount;
	byte NI;
	bool Active:1;
	word MLSize;
	word* MemList;
	int LastDx;
	int	LastDy;
	int LastAbs;
	int BestX;
	int BestY;
	int	BestLx;
	int	BestLy;
	int BestX1;
	int BestY1;
	int RealX;
	int RealY;
	int RealLx;
	int RealLy;
	int RealX1;
	int RealY1;
	int PrevDX;
	int PrevDY;
	int DestX;
	int DestY;
	int NInside;
	int NMembers;
	int Usage;//=0-резерв
			  //=1-отряд охраны
			  //=2-отряд захватчиков

	int Stage;//=0-
	word ForAtt[32];
	int FUNC;
	Crowd();
	void CreateCrowd(byte NI);
	void Disband();
	bool DisbandHalf();
	void FindNewPosition();
	void HandleCrowd();
	int  Funct(int x,int y,int Lx,int Ly);
	void SendCrowd();
	void GetSize();
	void AddToAttackQueue(word ID);
	bool HandleAttacks(byte PRIO);
	void HandleDefence();
};
//Описание базового типа, общего для всех объектов
class GeneralObject{
public:
	char* Message;
	bool cpbBase:1;
	bool cpbFarm:1;
	bool OnCost:1;
	bool OnOil:1;
	bool NoSearchVictim:1;
	bool NoAnswer:1;
	bool NeedNoHelp:1;
	bool Enabled:1;
	bool OnWater:1;
	bool Warrior:1;
	bool OnAir:1;
	bool Teleport:1;
	bool Transport:1;
	bool SATT:1;
	bool UATT:1;
	bool WATT:1;
	bool AATT:1;
	bool P100:1;
	bool T3X3:1;
	bool FWEAP:1;//огненное оружие
	bool AGold:1;
	bool AWood:1;
	bool Submarine:1;
	bool RefreshLife:1;
	bool CanFly:1;
	bool CanAttWall:1;
	bool CanFear:1;
	bool UseMagic:1;
	bool canNucAttack:1;
	bool AntiNuc:1;
	bool UFO:1;
	bool UFOTrans:1;
	bool CanRepair:1;
	bool ExtMenu:1;
	bool CanDest:1;
	word MagAgainst;
	word MagWith;
	word IDforUFO;
	word capMagic;
	byte Kind;
	word MaxAutoAmount;
	word Useful[NBRANCH];//полезность объекта для каждой из отраслей
	word SpUsef[NBRANCH];//удельная полезность
	word AbsNeedPrio;
	word AbsNeedCount;
	word LifeShotLost;
	int	 cost;
	word delay;
	short wepX;
	short wepY;
	byte WepSpeed;
	byte WepDelay;
	byte  VisRadius;
	byte  VisSpots;
	byte  SpotType;
	byte  SpotSize;
	byte  DangerZone;
	word NUpgrades;
	word* Upg;
	char Wdx[8];
	char Wdy[8];
	byte NIcons;
	word* IRefs;
	word NWeap;
	byte NStages;
	Weapon* MWeap[12];
	byte SWPIndex[12];
	short HitSound;
	short ClickSound;
	short OrderSound;
	short DeathSound;
	short BornSound;
	short AttackSound;
	short TreeSound;
	short GoldSound;
	word LockID;
	byte NLockUnits;
	/////DON'T CHANGE STRUCTURE AND ORDER UNDER THIS LINE/////
	/////          L O A D I N G  Z O N E                /////
	word NameIndex;
	word PreCreateMessage;
	word PostCreateMessage;
	//ObjIcon Icon;
	//Индекс переназначения
	//Ключ допуска к производству
	int ProduceAccess;
	//Ключ доступа для создания 
    int CreationAccess; 
	//Если Object1.ProdiuceAcess & Object2.CreationAccess!=0 то
	// Oject1 может произвести Object2
	//Класс объекта
	word ObjType;
	//  =0 - Монстр
	//  =1 - Здание
	//  =2 - Умение
	//  =3 - Усовершенствование

	//При создании данный объект может делать доступными другие
	//объекты,которые ранее были недоступны, можно открывать доступ не более,
	//чем к четырем объектам. OpenAccess содержит индексы открываемых
	//объектов. Это и есть знания.
	word OpenAccess[2];
	//Изобретения. В системе есть массив списков изобретений.
	//каждый элемент в списке содержит номер открываемого объекта,
	//минимальный уровень цивилизованности, необходимый для
	//изобретения данного объекта. Вероятность того, что хоть 
	//что-то будет изобретено при создании, определяется
	//мудростью.
	byte SizeX;
	byte SizeY;
	word Visdom;
	//Звуки для различных событий
	Weapon* Weap;
	//word sndChoose;			//Звук при выборе объекта
	//word sndUnderAttack;    //Звук,когда объекта обижают
	//Способности объекта
	bool cpbRotation:1;		//Повороты
	bool cpbMoving:1;		//Движение
	bool cpbSpending:1;     //Время для досуга и прикола
	bool cpbNearAttack:1;	//Ближний бой
	bool cpbFarAttack:1;	//Поражение на расстоянии
	bool cpbCarry:1;        //Способность нести предмет
	bool cpbPatrol:1;       //Способность патрулировать
	bool cpbBuild:1;		//Строительство
	bool cpbCreation:1;     //Создание монстра
	bool cpbResearch:1;     //Способность исследовать неизвестное знание
	bool cpbResource:1;     //Использование в качестве ресурса
	bool cpbDamage:1;       //Наличие анимации для поврежденного состояния  
	bool cpbDeath:1;        //Наличие анимации для гибели
	bool cbpAccess:1;       //Доступно в данный момент
	//bool cpbOneAccess:1;    //После создания cpbAccess=false
	bool cpbBuilding:1;		//Является зданием
	word  Resrvd;
	//Ресурсы, необходимые для постройки(производства)
	byte ResourceID[4];     //Индекс необходимого ресурса
	word ResAmount[4];      //Сколько необходимо для постройки 
	word NAnm;			//Количество доступных анимаций
	MoreAnimation *lpFAnim;//Анимация с описанием применения 
	void GetMonsterCostString(char* st);
	void CloseGO();
};
//Класс видимых объектов-монстры,здания
class Visuals : public GeneralObject
{
public:
//Базовые показатели
	union{
		struct{
			word MaxLife;
			word MaxShield;
			word Strength;
			word PsychoForce;
			word Dextrity;
			word MinDamage;
			word MaxDamage;
			word Productivity;
			word AttackRange;
			word Reserved1;
			word Reserved2;
			word Reserved3;
			word Reserved4;
			word Reserved5;
			word Reserved6;
			word Reserved7;
		} Basic;
		word Index[16];
	} info;
	//Информация об оружии дальнего поражения
};
//Upgrade-замена одного типа объектов на другой
class ObjectUpgrade : public GeneralObject
{
public:
	word SourceTypeIndex[8];
	word FinalTypeIndex[8];
};
//Upgrade - для Visuals-улучшение параметров для группы объектов;
class ParameterUpgrade : public GeneralObject
{
	word ObjectIndex[8];
	byte ChangedParameter[8];
	byte AdditionalValue[8];
};

class OneObject;
typedef void ReportFn(OneObject* Sender);
//Описание командного блока
//Приказы 1-го уровня
struct Order1{
	//Указатель на отложенный приказ, если NULL то нет 
	//отложенного приказа 
	Order1* NextOrder;
	//Уровень приоритета выполнения команды
	//0-нижайший уровень приоритета
	//Данное задание может быть отложено только в том случае,
	//если приоритет прерывания выше приоритета выполнения
	byte PrioryLevel;
	byte OrderType;
	byte OrderTime;//=0 if very fast
	//=1 - стоять на месте 
	//=2 - перейти в точку с координатами (x,y)
	//=3 - следовать за объектом (obj)
	//=4 - преследовать и атаковать объект (obj)
	//=5 - построить объект
	//=6 - создать объект
	//=7 - патрулировать
	//Информация по каждому типу приказов
	ReportFn* DoLink;
	union{
		struct{
			byte VisibilityRadius;
		} Stand;
		struct{
			byte x;
			byte y;
			word PrevDist;
			byte Times;
		} MoveToXY;
		struct{
			byte xd;
			byte yd;
			byte time;
			word BuildID;
			word BSN;
		} UFO;
		struct{
			byte x;
			byte y;
			byte wep;
		} AttackXY;
		struct{
			word ObjIndex;
			word SN;
			word PrevDist;
			byte wep;
		} MoveToObj;
		struct{
			byte x;
			byte y;
			byte x1;
			byte y1;
			byte dir;
		} Patrol;
		struct{
			word ObjIndex;
			word Progress;
			word NStages;
			byte PStep;
		}Produce;
		struct{
			word OldUpgrade;
			word NewUpgrade;
			word Stage;
			word NStages;
		}PUpgrade;
		struct{
			byte dir;
		}MoveFrom;
		struct{
			byte x;
			byte y;
			byte ResID;
		}TakeRes;

	}info;
};
class Legion;
class Nation;
//Информация для каждого индивидуума
class OneObject{
public:
	Nation* Nat;
	Crowd* CrowdRef;
	union{
		GeneralObject* General;
		Visuals* Visual;
		ObjectUpgrade* OUpgrade;
		ParameterUpgrade* PUpgrade;
	} Ref;
	//-----------new path algoritm variables----------------
	bool NeedPath;
	byte CPdestX;
	byte CPdestY;
	byte* PathX;
	byte* PathY;
	word NIPoints;
	word CurIPoint;
//--------------------------------------------//
//-----------begin of saving cluster----------//
//--------------------------------------------//
	word Index;//номер этого монстра
	word NIndex;//индекс в национальном списке
	bool Selected:1;
	bool Borg:1;
	bool Invert:1;
	bool Push:1;
	bool Sdoxlo:1;
	//bool inMotion:1;//Выполняется приказ о перемещении
	bool isMoving:1;
	bool XYChanged:1;
	bool Removed:1;
	bool Important:1;//Делает что-то важное
	bool Attack:1;
	bool Egoist:1;
	bool NoMotion:1;//не сдвигаться ни прт каких условиях,кроме приказа по армии
	bool NoFire:1;//не открывать огонь по врагу и не привлекать внимания
	bool NoInitiate:1;//не инициировать сражение
	bool WasInMobilZone:1;//Был в зоне мобилизации
	bool TempFlag:1;//временный флаг, используемый для сортировки отряда
	bool Mobilised:1;
	bool MoveInRect:1;//установлен, если среди риказов есть приказ о перемещении группы
	bool DrawUp:1;//СТРРPPОИТЬСЯ!!!
	bool PathBroken:1;//Если хоть раз была преграда на пути
	bool capBuilding:1;
	bool cpbMoving:1;
	bool capBuild:1;
	bool capBase:1;
	bool Ready:1;
	bool NoSearchVictim:1;
	bool NoAnswer:1;
	bool NeedNoHelp:1;
	bool OnWater:1;
	bool AskMade:1;
	bool FlyAttackMode:1;
	bool Teleport:1;
	bool capTeleport:1;
	bool StandGround:1;
	bool Invisible:1;
	//bool WATT:1;
	//bool AATT:1;
	//bool P100:1;
	//bool T3X3:1;
	//bool DoubleForce:1;
	//bool TripleForce:1;
	//bool QuadraForce:1;
	bool Slow:1;
	bool Transport:1;
	bool Absent:1;
	bool InFire:1;
	bool RefreshLife:1;
	bool DoWalls:1;
	bool Use_AI:1;
	bool AntiNuc:1;
	bool UFOTrans:1;
	bool Repair:1;
	word MagSrcID;
	byte AbRes;
	byte xForce;
	word MTime;
	word capMagic;
	word Magic;
	word NUstages;
	word Ustage;
	word Serial;
	byte Kind;
	byte Lx;
	byte Ly;
	byte Wait;
	word Life;
	word MaxLife;
	byte x;
	byte y;
	byte DstX;
	byte DstY;
	byte destX;
	byte destY;
	byte destX1;
	byte destY1;
	byte lastX;
	byte lastY;
	short wepX;
	short wepY;
	word delay;
	byte MaxDelay;
	byte Npush;
	byte MemoryTime;
	byte StandTime;
	byte NStages;
	byte Stage;
	byte NNUM;
	char Addx;
	char Addy;
	word EnemyDist;
	word EnemyID;
	word EnemySN;
	byte NMask;
	byte RStage;
	byte RType;
	byte RAmount;
	word NearBase;
	byte  VisRadius;
	byte  VisSpots;
	byte  SpotType;
	byte  SpotSize;
	byte  DangerZone;
	//Текущее видимое состояние монстра
	byte Direction;				// Направление монстра 0..7
	byte CurrentSprite;			// Номер спрайта, который должен
								// рисоваться
	byte Ticks;					// Прошло тиков с момента смены кадра
	byte TicksPerChange;		// Тиков на смену кадра
	byte AnmStandKind;
	byte AnmGoKind;
	byte Media;//=0-terrain,=1-on water,=2-on air
//-----------------------------------------------//
//-------------end of saving cluster-------------//
//-----------------------------------------------//
	Octant* CurAnm;				// Текущая выполняемая анимация
	word* Inside;
	word* TimeInside;
	word NInside;
	//Flying objects only:
	int  RealX;//1 pixel=16 length units
	int  RealY;
	int  DestX;
	int  DestY;
	int  RealVx;
	int  RealVy;
	int  BestDist;
	int  BestHeight;
	int	 Height;
	byte RealDir;
	//for all objects
	Legion* Wars;
	//byte bx;
	//byte by;
	SelGroup* GroupIndex;
	
//Указатель на текущий приказ 1-го уровня
//Типы приказов:
//	Перейти(x,y)->(x1,y1) по оптимальному пути;
//	Атаковать объект(obj_index)- удаленный или близкий
//	Умереть
//	Создать экземпляр объекта(obj)...
	Order1* LocalOrder;
//Указатель на процедуру, которой сообщается о текущем состоянии 
//объекта в процессе выполнения приказа
	//ReportFn* OrderReport;
//Значения MessageType:
//	00000001 - Анимация завершена
//	00000010 - Все команды из буфера исполнены
//	00000100 - Изменились координаты объекта
//	00001000 - Невозможно осуществить перемещение
//Те события, которые вызывают зту процедуру, обрабатываются 
//в соответствии с флагами
	//word MessageFlags;
//Описание прерываний
	byte PrioryLevel;
	//byte MessageKind;
//  =0 - не было прерываний
//	=1 - Помогите! меня бьют!
//	=2 - Я убью тебя!
//	=3 - задание прервано
	//word Sender;
//  =\0xFFFF - прерывание от игрока, иначе номер
//  объекта, пославшего сообщение.

//Указатель на буфер команд, которые выполняются последовательно
//процедурой просчета следующего состояния,то есть видоизменяют 
//предыдущие переменные. Если InLineCom=NULL-нечего исполнять 
	char* InLineCom;
	int LineOffset;//Указатель на текущую команду(относительный)
/*			Описание команд псевдоассемблера (уровень 0)
	00					1		Неправильная команда(comUNKNOWN)
	01					1		Окончание процедуры, возврат(comRET)
	02					1		Отложить дальнейшее выполнение 
								до окончания выполнения текущей
								анимации
	03					1		Локальное задание уровня 1 выполнено	
	04 db(x) db(y)		3		Установить текущую координату
	05 db(direction)	2		Установить текущее направление

	06 db(dtype)				Загрузить регистр (stype) анимацией 
	   db(stype)				типа (dtype) с атрибутом (kind)
	   dw(kind)			5		cm MoreAnimation, dtype=WhatFor;
								kind=kind. Значения (stype):
									0-Stand
									1-Move
									2-Attack
									3-Death
									4-Spending
									5-Working
									...

	07 dd(address)		5		Безусловный переход по адресу в памяти
 */
	//Регистры анимации-для ускорения доступа к анимациям
	MoreAnimation ARegs[5];
	Weapon* Weap;
	void DefaultSettings(GeneralObject* GO);
	void LoadAnimation(int Reg,byte WhatFor,word Kind);
	void SendFlyTo(int x,int y,byte prio);
	void MakeProcess();
	void MakePreProcess();
	void MakeWPreProcess();
	void SendTo(int x,int y,int);
	void WSendTo(int x2,int y2,int Prio);
	void AttackObj(word OID,int Prio);
	void WAttackObj(word OID,int Prio);
	void FlyAttack(word OID,byte Prio);
	void AttackPoint(byte x,byte y,byte wep,int Prio);
	void ComplexAttack(word OID,byte wep,int Prio);
	void ProcessFly();
	void CreatePath(int x1,int y1);
	void CreateSimplePath(int x1,int y1);
	void CreatePrePath(int x1,int y1);
	void CreateWPath(int txt1,int tyt1);
	void CreatePreWPath(int txt1,int tyt1);
	void CreateSimpleWPath(int txt1,int tyt1);
	void MoveFrom(int dir);
	void WMoveFrom(int dir);
	void FreeAsmLink();
	void LoadCurAnm(int reg);
	void Die();
	void Eliminate();
	void MakeDamage(int Fundam,int Persist,OneObject* Sender);
	void SearchSupport(word OBJ);//Поиск помощников замочить OBJ
	void SearchVictim();
	void FreeOrdBlock(Order1* p );
	void ClearOrders();
	void ProcessMotion();
	void ProcessAttackMotion();
	void SendInGroup(byte tx,byte ty,byte x0,byte y0,byte x1,byte y1);
	void NextStage();
	bool BuildObj(word OID,int Prio);
	int CheckAbility(word ID);
	void Produce(word ID);
	bool BuildWall(word OID,int Prio);
	bool DamageWall(word OID,int Prio);
	word FindNearestBase();
	bool TakeResource(byte x,byte y,byte ResID,int Prio);
	void PerformUpgrade(word NewU);
	word FindNearestPort();
	bool BuildOnWater(word OID,int Prio);
	bool GetOil(word OID,int Prio);
	void SetDstPoint(byte x,byte y);
	void Patrol(int x1,int y1,int x2,int y2,byte prio);
	//void Repair(int x,int y,byte prio);
	void EnableDoubleForce();
	void DisableDoubleForce();
	void EnableTripleForce();
	void DiasableTripleForce();
	void EnableQuadraForce();
	void DisableQuadraForce();
	void ContinueAttackPoint(byte x,byte y,int Prio);
	void ContinueAttackWall(byte x,byte y,int Prio);
	void NuclearAttack(byte x,byte y);
	void MakeMeUFO();
	void WaitForRepair();
	inline int GetMinDam(){
		return Ref.Visual->info.Basic.MinDamage;
	};
	inline int GetMaxDam(){
		return Ref.Visual->info.Basic.MaxDamage;
	}
	bool GoToTransport(word MID,byte Prio);
	bool UnloadPassangers(byte x ,byte y);
	void MakeMeFly(word MagID);
	void MakeMeSit();
//#pragma warning( disable )
	inline byte DistTo(byte xx,byte yy){
		__asm{
			xor		eax,eax
			xor		ecx,ecx
			xor		edx,edx
			mov		al,xx
			mov		ebx,this
			mov		dl,[ebx]this.x
			sub		ax,dx
			jge		uui
			neg		ax
uui:		mov		cl,yy
			mov		dl,[ebx]this.y
			sub		cx,dx
			jge		uux
			neg		cx
uux:		cmp		cx,ax
			jl		uuz
			mov		ax,cx
uuz:
		};
	};
//#pragma warning( default )
	void CloseObject();
};
//Описание оружия
class Nation;
class Weapon{
public:
	Visuals* Vis;//оружие оформляется как отдельная нация
	word NatIndex;
	byte FShift;//нач. смещение источника поражения
	byte MinDamage;
	byte MaxDamage;
	byte Force;
	byte FDecrement;//насколько уменьшается сила оружия при попадании
	byte Probab1;//вероятность продить 1 оружие(0..255)
	byte Probab2;//вероятность породить 2 оружия(0..2555)
	short ExplPath;//путь, проходимый до деления
	byte DirectFactor;//степень корреляции направления перемещения
					  //порождаемого оружия и порождающего
					  //0-независимы, 255-совпадают
	//word		ar1;
	//word		ar2;
	word		atime;
	byte Transparent;
	Weapon* NextWeapon;//Оружие,порождаемое в точке поражения
	Octant* Fly;
	Octant* Action;
	byte NMask;
	void CreateWeapon(Nation* WNat,word Ind,byte ExplCount,byte Kind);
	void CreateNWeapon(int r1,int r2,int time);
	//Kind: 0-simple weapon
	//1-weak division weapon
	//2-middle division
	//3-strong division,NUCLEAR-like weapon
	//4-super strong weapon.Total elimination.
	short SoundID;
	
};
//Описание элемента анимации(оружие в действии)
class AnmObject{
public:
	byte xm;//Map coordinates
	byte ym;
	byte xm0;
	byte ym0;
	int x;
	int y;
	int vx;
	int vy;
	short v;
	word ASerial;
	byte PhDir;//(0..255);-физическое направление
	byte Direction;//направление из октанта
	byte NMask;
	OneObject* Sender;
	Weapon* Wpn;
	word Path;
	word MaxPath;
	bool Visible:1;
	bool OneTime:1;
	bool SATT:1;
	bool UATT:1;
	bool WATT:1;
	bool AATT:1;
	bool P100:1;
	bool T3X3:1;
	bool AnmLead:1;
	byte Transparent;
	byte Nanm;
	byte Canm;
	byte Force;
	byte Decrement;
	word MinDamage;
	word MaxDamage;
	OneSlide* Kino;
	byte destX;
	byte destY;
	word DestObj;
	word DSerial;
};
class City;
class Needness{
public:
	byte NeedType;//==0-monster,==1-Upgrade
	word MonID;
	byte GroupSize;
	byte Amount;
	word Probability;
	word MoneyPercent;
};
struct SWPAR{
	word Range;
	byte MinMagic;
	bool Enabled:1;
	bool Fly:1;
};
struct sAI_Req{
	byte Kind;//0-unit,1-upgrade,2-group
	word ObjID;
	word Amount;
};
struct sAI_Devlp{
	byte Kind;//0-unit,1-upgrade
	byte ConKind;//0-unit,2-group
	word ObjID;
	word ConID;
	word Amount;
	word GoldPercent;
	word AtnPercent;
};
//Описание нации в целом
class Nation{
public:
	char SCRIPT[16];
	int NMon;
	GeneralObject* Mon[2048];
	City* CITY;
	int NGidot;
	int NFarms;
	word* PAble[2048];
	word PACount[2048];
	int BranchPercent[NBRANCH];
	Upgrade UPG;
	int NOct;
	int NSlides;
	//AI Statements
	int CasheSize;
	int TAX_PERCENT;
	int CASH_PUSH_PROBABILITY;
	int NationalAI;//0..32768-determines speed of development
	int AI_Level_MIN;
	int AI_Level_MAX;
	int AI_Forward;
	short DangerSound;
	short VictorySound;
	short ConstructSound;
	short BuildDieSound;
	short UnitDieSound;
	word  LastAttackTime;
	byte MagicDelay;
	word NLmenus;
	word* Lmenus;
	word NAmenus;
	word* Amenus;
	word NWmenus;
	word* Wmenus;
	word NCmenus;
	word* Cmenus;
	word NNeed;
	Needness NEED[1024];
	//ENDAI
	byte NNUM;
	Octant* Animations;
	OneSlide* Slides;
	MoreAnimation* Finf;
	int  NFinf;
	byte palette[256];
	word CivilisationLevel;
	byte NMask;
	word NIcons;
	WIcon* wIcons[1024];
	word TOWN0ID;
	word TOWN1ID;
	word TOWN2ID;
	word SubmID;
	word PortID;
	word RefineryID;
	word FoundaryID;
	word TankerID;
	word OilBaseID;
	word TOWNHALL1;
	word TOWNHALL2;
	word LUMBERMILL;
	word WMagic[256];
	word NCOND;
	word CLSize[256];//Access controlling
	word* CLRef[256];
	//Strange weapon prameters
	word SWRange[256];
	SWPAR SWP[256];
	//-------------NEW AI--------------
	word NGrp;        //Groups of types definition
	word GRSize[32];
	word* GRRef[32];
	word  GAmount[32];//Result of calculation
	word N_AI_Levels;
	word N_AI_Req[256];
	word N_AI_Devlp[256];
	sAI_Req* AI_Req[256];
	sAI_Devlp* AI_Devlp[256];
	word AI_Level;
	//---------------------------------
	void CreateNation(LPCSTR NationFile,byte NMask,byte NIndex);
	bool CreateTerrainAt(int x,int y,int n);
	bool CreateOnWater(int xp,int yp,int n);
	bool CreateOnFly(int xf,int yf,int n);
	void AssignWeapon(Weapon* Wpn,int i);
	int CreateBuilding(word ID,byte x,byte y);
	bool CheckBuilding(word ID,byte x,byte y);
	void GetUpgradeCostString(char* st,word UI);
	void GetTax(byte RC_Type,int RC_Amount);
	void CloseNation();
};
//Отряд монстров
class Legion{
public:
	word* Member;//Указатель на список членов легиона
	word NMemb;//Кол-во членов
	word NNeed;//необходимо для комплектации
	byte NMask;//нация
	byte minx;
	byte miny;
	byte maxx;
	byte maxy;
	byte XL1;
	byte YL1;
	byte SizeX1;//какой должен быть размер по OX область защитников
	byte SizeY1;//------/------ по OY
	byte XL2;
	byte YL2;
	byte SizeX2;//------------/---------------- область стрелков
	byte SizeY2;
	byte maxDeplX;//размер отряда по х, при котором уже нужно построение
	byte maxDeplY;
	byte critDeplX;//посроение абсолютно необходимо,иначе разгром
	byte critDeplY;
	word MobilTimer;//время на группировку
	word Order3Time;
	word NoMmobilTimer;//время, в течение которого новый приказ о группировке
					   //игнорируется
	word EnemyID;
	
	byte destX;
	byte destY;
	byte destX1;
	byte destY1;
	byte FinX;
	byte FinY;
	byte Ordering;//степень упорядоченности 
	//флаги реакции отряда на события
	bool NoMotion:1;//не сдвигаться ни прт каких условиях,кроме приказа по армии
	bool NoFire:1;//не открывать огонь по врагу и не привлекать внимания
	bool NoInitiate:1;//не инициировать сражение
	//по умолчанию все флаги=false-самовольное избирание тактики сражения
	//флаги состояния отряда
	bool Mobil:1;//отряд мобилизован
	bool inWar;//отряд вовлечен в военные действия
	bool NotExists;//полный разгром  отряда
	bool CheckMobility:1;//проверять степень упорядоченности
	bool TempMobil;//промежуточная мобилизация
	bool StonesInside:1;//в середине расположения есть неустранимые преграды
	//флаги приказов
	bool needMobil;//приказ о мобилизации
	bool needCompose:1;//необходима группировка
	bool needMoveTo:1;
	bool needDestroyObj:1;
	bool needCompactness:1;//по пути отряд поджидает отставших
	void CreateLegionInSquare(byte x,byte y,byte x1,byte y1,byte Mask);
	void CreateSmartLegionInSquare(byte x,byte y,byte x1,byte y1,byte Mask);
	void CreateLegionFromSelection(byte Mask);
	void CreateSmartLegionFromSelection(byte Mask);
	void CreateLegionNearPoint(byte x,byte y,byte MaxRadius,word NMem,byte Mask);
	void CreateSmartLegionNearPoint(byte x,byte y,byte MaxRadius,word NMem,byte Mask);
	void CreateMobilZone(byte x,byte y);
	void FastCreateMobilZone(byte x,byte y);
	char GetOrderDegree();
	//0-никакого порядка нет, войско рассеяно
	//1-порядок недостаточен для нормального функционирования
	//2-порядок, достаточный для перемещения
	//3-минимальный беспорядок,твари только на одну клетку не влезли
	//4-идеальный порядок
	void Compose();
	void Mobilisate();
	void MoveTo(byte x,byte y);
	void AttackObj(word ObjID);
	void CalculateArea();//заполнение полей minx,miny,maxx,maxy
	void CalculateBestComposition();//по значениям 
	//min..,max..,NMemb->NNeed,SizeX1,SizeY1
	void CalculateSoldiers();//считает и быстро сортирует солдат,
	//определяет занимаемую отрядом область
	void CreateFinalZone(byte x,byte y);
	void PreProcess();
	void Process();//продолжить выполнение приказа
	bool MobilisationIsDone();
	bool CompositionIsDone();
	void SendSoldiers();
	void SendTo(byte x,byte y);

};
class SelGroup{
public:
	word* Member;
	word* SerialN;
	word NMemb;
	bool CanMove:1;
	bool CanSearchVictim:1;
	bool CanHelpToFriend:1;
	bool Egoizm:1;
	SelGroup();
	void CreateFromSelection(byte NI);
	void SelectMembers(byte NI);
	void DeleteMembers(byte NI);
};


//Массив все монстров на карте
#define MaxObj 8192
#define maximage 2048
extern OneObject* Group[MaxObj];
extern RLCTable MImage[maximage];
extern RLCTable miniMImage[maximage];
void LoadMonsters();
#define maxmap 256  //Don't change it!
typedef byte SimpMap[maxmap][maxmap];
extern MapCell map[maxmap][maxmap];
extern SimpMap TrMap;
void LoadLock();
#define MaxAsmCount 16384
#define OneAsmSize 256
#define OneAShift 8;
#define MaxOrdCount 32768
#define OneOrdSize 32;
#define OneOShift 5;
char* GetAsmBlock();
void FreeAsmBlock(char* p );
void InitAsmBuf();
Order1* GetOrdBlock();
//void FreeOrdBlock(Order1* p );
void InitOrdBuf();
extern Order1  OrdBuf[MaxOrdCount];
extern bool	AsmUsage[MaxAsmCount];
extern int	msx;
extern int msy;
extern void Except();
//Размер очереди на выполнение(2^n only !)
#define StSize 8192
#define StMask StSize-1;
extern word ComStc[StSize];
extern word StHead;
extern word StTile;
#define FreeTime 600;
void CarryOutOrder();
void InitStack();
extern RLCTable rlc;
extern RLCFont rlf;
extern RLCFont rlf1;
extern RLCTable mon;
extern RLCTable rlc_s;
extern RLCFont rlf_s;
extern RLCFont rlf1_s;
extern RLCTable mon_s;
void doooo();
extern byte LLock[maxmap][maxmap];
extern word Mops[maxmap][maxmap];
extern word Creator;
extern Nation NAT;
extern int	smapx;
extern int	smapy;
extern int	smaplx;
extern int	smaply;
extern int minix;
extern int	miniy;
extern HWND hwnd;
void MakePostProcess();
void MakeWPostProcess();
void PrepareProcessing();
extern int Flips;
extern word Died[maxmap][maxmap];
//extern void FreeOrdBlock(Order1* p );
extern int	mapx;
extern int	mapy;
//byte CreateExObj(Weapon* Wep,short x,short y,
//				 short dx,short dy,short v,byte Mask,OneObject* Send);
void CreateExObjD(Weapon* Wep,int x,int y,
				 short PDir,short v,byte Mask,OneObject* Send);
//byte CreateExObjDPoint(Weapon* Wep,short x,short y,
//				 short dx,short dy,short v,byte Mask,OneObject* Send,byte dsx,byte dsy);
byte AttackByWeapon(Weapon* Wep,short x,short y,OneObject* Send,word DOBJ);
void InitExplosions();
void ProcessExpl();
extern Weapon FlyFire1;
extern Weapon Vibux1;
void CloseExplosions();
short rando();
extern byte PlayerMask;
extern bool EgoFlag;
void AddAsk(word ReqID,byte x,byte y,char zdx,char zdy);
extern SelGroup SelSet[80];
extern Weapon* WPLIST[1024];
typedef void HandlePro(int);
void InitZones();
int CreateZone(int x,int y,int lx,int ly,HandlePro* HPro,int Index,char* Hint);
void ControlZones();
void DeleteZone(int i);
void ShowProp();
void InitPrpBar();
void ShowAbility();
extern word* Selm[8];
extern word* SerN[8];
extern word NSL[8];
void CmdCreateSelection(byte NI,byte x,byte y,byte x1,byte y1);
void CmdSendToXY(byte NI,byte x,byte y);
void CmdAttackObj(byte NI,word ObjID);
void CmdCreateTerrain(byte NI,byte x,byte y,word Type);
void CmdCreateBuilding(byte NI,byte x,byte y,word Type);
void CmdProduceObj(byte NI,word Type);
void CmdMemSelection(byte NI,byte Index);
void CmdRememSelection(byte NI,byte Index);
void CmdBuildObj(byte NI,word ObjID);
void CmdBuildWall(byte NI);
void CmdRepairWall(byte NI,word LIN);
void CmdDamageWall(byte NI,word LIN);
void CmdTakeRes(byte NI,byte x,byte y,byte ResID);
void CmdPerformUpgrade(byte NI,word UI);
void CmdCreateKindSelection(byte NI,byte x,byte y,byte x1,byte y1,byte Kind);
void CmdCreateTypeSelection(byte NI,byte x,byte y,byte x1,byte y1,word Type);
void CmdCreateGoodSelection(byte NI,word x,word y,word x1,word y1);
void CmdCreateGoodKindSelection(byte NI,word x,word y,word x1,word y1,byte Kind);
void CmdCreateGoodTypeSelection(byte NI,word x,word y,word x1,word y1,word Type);
void CmdSetDst(byte NI,byte x,byte y);
void CmdSendToPoint(byte NI,byte x,byte y);
void CmdAttackToXY(byte NI,byte x,byte y);
void CmdStop(byte NI);
void CmdStandGround(byte NI);
void CmdPatrol(byte NI,byte x,byte y);
void CmdRepair(byte NI,byte x,byte y);
void CmdGetResource(byte NI,byte x,byte y);
void CmdComplexAttackPoint(byte NI,byte x,byte y,byte kind);
void CmdComplexAttack(byte NI,word ID,byte kind);
void CmdSendToTransport(byte NI,word ID);
void CmdUnload(byte NI,byte x,byte y);
void CmdDie(byte NI);
void CmdContinueAttackPoint(byte NI,byte x,byte y);
void CmdContinueAttackWall(byte NI,byte x,byte y);
void CmdSitDown(byte NI);
void CmdNucAtt(byte NI,byte x,byte y);
void CmdChooseSelType(byte NI,word ID);
void CmdChooseUnSelType(byte NI,word ID);
extern Nation NATIONS[8];
extern byte MyNation;
void InitEBuf();
void ExecuteBuffer();
extern char Prompt[80];
extern int PromptTime;
extern byte tmap[maxmap][maxmap];
bool CheckCost(byte NI,byte ResID,word Amount);
void CreateWaterMap();
extern byte WLock[256][256];//Water locking map
extern byte WMap[256][256];//Static locking map
extern int SCRSizeX;
extern int SCRSizeY;
extern int RSCRSizeX;
extern int RSCRSizeY;
extern int COPYSizeX;
void CmdGetOil(byte NI,word UI);
//DEBUGGING definitions. Remove this definitions in retail version
//#define DEBLEVEL1
//#define DEBLEVEL2
//#define DEBLEVEL3
//#define DEBLEVEL4
extern byte NLocks[64][64];
inline void IncLock(byte x,byte y){
	NLocks[y>>2][x>>2]++;
};
inline void DecLock(byte x,byte y){
	NLocks[y>>2][x>>2]--;
};
extern bool FASTMODE;
extern word MAXOBJECT;
void SetupHint();
void AssignHint(char* s,int time);
void GetChar(GeneralObject* GO,char* s);
void ProcessHint();
extern OneObject OBJECTS[8192];
extern short TSin[257];
extern short TCos[257];
extern short TAtg[257];
void SetFlyMarkers();
void ClearFlyMarkers();
typedef void UniqMethood(int n,int x,int y);
void HandleSW();
void CreateStrangeObject(int i,byte NI,int x,int y,word ID);
void ShowRLCItemMutno(int x,int y,lpRLCTable lprt,int n);
void ShowRLCItemFired(int x,int y,lpRLCTable lprt,int n);
byte CreateUniExObj(Weapon* Wep,int x,int y,
				 short v,byte Mask,
				 OneObject* Send,
				 byte dsx,byte dsy,
				 word DOBJ);
byte CreateLeadingObject(Weapon* Wep,int x,int y,
				 short v,byte Mask,
				 OneObject* Send,
				 word DestAnm);
bool CheckAttAbility(OneObject* OB,word Patient);
void PrepareToEdit();
void PrepareToGame();
extern int MaxSizeX;
extern int MaxSizeY;
extern bool MiniMode;
void SetMiniMode();
void ClearMiniMode();
extern int Shifter;
extern int Multip;