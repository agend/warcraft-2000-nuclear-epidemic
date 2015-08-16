
class SQPicture{
public:
	word* PicPtr;
	SQPicture();
	void LoadPicture(char* Name);
	void Draw(int x,int y);
	void DrawTransparent(int x,int y);
	~SQPicture();
};
class SimpleDialog;
typedef bool VCall(SimpleDialog* SD);
class SimpleDialog{
public:
	bool Enabled:1;
	bool Active:1;
	bool MouseOver:1;
	bool NeedToDraw:1;
	bool MouseOverActive:1;
	int x,y,x1,y1;
	int UserParam;
	SimpleDialog* Parent;
	VCall* OnClick;
	VCall* OnDraw;
	VCall* OnActivate;
	VCall* OnLeave;
	VCall* OnKeyDown;
	VCall* OnMouseOver;
	VCall* Refresh;
	VCall* Destroy;
	VCall* OnUserClick;
	SimpleDialog();
};
class Picture:public SimpleDialog{
public:
	SQPicture* PassivePicture;
	SQPicture* ActivePicture;
	SQPicture* DisabledPicture;
	bool Transparent:1;
	Picture(){SimpleDialog;};
};
class RLCPicture:public SimpleDialog{
public:
	RLCTable* PassivePicture;
	byte ppic;
	RLCTable* ActivePicture;
	byte apic;
	RLCTable* DisabledPicture;
	byte dpic;
};
class TextMessage{
	char* Message;
	RLCFont* Font;
	byte Align;
};
class TextButton:public SimpleDialog{
public:
	char* Message;
	RLCFont* ActiveFont;
	RLCFont* PassiveFont;
	RLCFont* DisabledFont;
	int	xc;
	int yc;
	int ActiveDX;
	int ActiveDY;
	byte Align;
};
class VScrollBar:public SimpleDialog{
public:
	int SMaxPos,SPos;
	int sbx,sby,sblx,sbly;
	int LastTime;
	byte mkly,btnly;
	bool Zaxvat;
	SQPicture* btn_up0;
	SQPicture* btn_up1;
	SQPicture* btn_up1p;
	SQPicture* btn_dn0;
	SQPicture* btn_dn1;
	SQPicture* btn_dn1p;
	SQPicture* sbar0;
	SQPicture* sbar1;
	SQPicture* marker;
	void SetMaxPos(int i){SMaxPos=i;};
	int GetMaxPos(){return SMaxPos;};
	int GetPos(){return SPos;};
};
class BpxTextButton:public TextButton{
public:
	SQPicture* PassivePicture;
	SQPicture* ActivePicture;
	SQPicture* DisabledPicture;
};
struct ListBoxItem{
	char* Message;
	int Param1;
	ListBoxItem* NextItem; 
};
class ListBox:public SimpleDialog{
public:
	ListBoxItem* FirstItem;
	ListBoxItem* LastItem;
	SQPicture* ItemPic;
	RLCFont* AFont;
	RLCFont* PFont;
	byte ny;
	byte oneLy;
	int	 oneLx;
	int NItems;
	int CurItem;
	int FLItem;
	VScrollBar* VS;
	ListBoxItem* GetItem(int i);
	void AddItem(char* str,int info);
	void ClearItems();
	void SetFirstPos(int i);
	void SetCurrentItem(int i);
};
class InputBox:public SimpleDialog{
public:
	char* Str;
	int StrMaxLen;
	RLCFont* Font;
	RLCFont* AFont;
	SQPicture* Pict;
};
class DialogsSystem;
class CheckBox:public SimpleDialog{
public:
	DialogsSystem* DS;
	SQPicture* OnPic;
	SQPicture* OffPic;
	char* Message;
	bool State;
	int GroupIndex;
	RLCFont* Font;
	RLCFont* AFont;
};
class ColoredBar:public SimpleDialog{
public:
	byte color;
};
struct LineInfo{
	bool NeedFormat;
	word LineSize;
	word NSpaces;
	int  Offset;
	int  NextOffset;
	word LineWidth;
};
class TextViewer:public SimpleDialog{
public:
	char* TextPtr;
	int TextSize;
	int NLines;
	int Line;
	int PageSize;
	int Lx;
	word SymSize;
	RLCFont* Font;
	void GetNextLine(LineInfo*);
	void CreateLinesList();
	char** LinePtrs;
	word*  LineSize;
	word*  LineWidth;
	bool*  NeedFormat;
	word*  NSpaces;
};
class DialogsSystem{
public:
	int	BaseX,BaseY;
	SimpleDialog* DSS[512];
	int NDial;
	DialogsSystem(int x,int y);
	~DialogsSystem();
	RLCFont* Active;
	RLCFont* Passive;
	RLCFont* Disabled;
	RLCFont* Message;
	void ProcessDialogs();
	void MarkToDraw();
	void RefreshView();
	void CloseDialogs();
	void SetFonts(RLCFont* Active,
				  RLCFont* Passive,
				  RLCFont* Disabled,
				  RLCFont* Message);
	Picture* addPicture(SimpleDialog* Parent,int x,int y,
						SQPicture* Active,
						SQPicture* Passive,
						SQPicture* Disabled);
	RLCPicture* addRLCPicture(SimpleDialog* Parent,int x,int y,
						RLCTable* Active,byte apic,
						RLCTable* Passive,byte ppic,
						RLCTable* Disabled,byte dpic);
	TextMessage* addTextMessage(SimpleDialog* Parent,int x,int y,char* str,RLCFont* Font,byte Align);
	TextMessage* addsTextMessage(SimpleDialog* Parent,int x,int y,char* str);
	TextButton* addTextButton(SimpleDialog* Parent,int x,int y,char* str,
						RLCFont* Active,
						RLCFont* Passive,
						RLCFont* Disabled,
						byte Align);//==0-left, ==1-center,  ==2-right
	TextButton* addsTextButton(SimpleDialog* Parent,int x,int y,char* str);
	BpxTextButton* addBpxTextButton(SimpleDialog* Parent,int x,int y,char* str,
						RLCFont* Active,
						RLCFont* Passive,
						RLCFont* Disabled,
						SQPicture* pActive,
						SQPicture* pPassive,
						SQPicture* pDisabled);
	VScrollBar* addVScrollBar(SimpleDialog* Parent,int x,int y,int MaxPos,int Pos,
						SQPicture* btn_up0,
						SQPicture* btn_up1,
						SQPicture* btn_up1p,
						SQPicture* btn_dn0,
						SQPicture* btn_dn1,
						SQPicture* btn_dn1p,
						SQPicture* sbar0,
						SQPicture* sbar1,
						SQPicture* marker);
	VScrollBar* addHScrollBar(SimpleDialog* Parent,int x,int y,int MaxPos,int Pos,
						SQPicture* btn_up0,
						SQPicture* btn_up1,
						SQPicture* btn_up1p,
						SQPicture* btn_dn0,
						SQPicture* btn_dn1,
						SQPicture* btn_dn1p,
						SQPicture* sbar0,
						SQPicture* sbar1,
						SQPicture* marker);
	ListBox* addListBox(SimpleDialog* Parent,int x,int y,int Ny,
						SQPicture* ItemPic,
						RLCFont* AFont,
						RLCFont* PFont,
						VScrollBar* VS);
	InputBox* addInputBox(SimpleDialog* Parent,int x,int y,char* str,int Len,SQPicture* Panel,RLCFont* RFont,RLCFont* AFont);
	CheckBox* addCheckBox(SimpleDialog* Parent,int x,int y,char* Message,
						int group,bool State,
						SQPicture* OnPict,
						SQPicture* OffPict,
						RLCFont* Font,
						RLCFont* AFont);
	SimpleDialog* addViewPort(int x,int y,int Nx,int Ny);
	ColoredBar* addColoredBar(int x,int y,int Nx,int Ny,byte c);
	TextViewer* addTextViewer(SimpleDialog* Parent,int x,int y,int Lx,int Ly,char* TextFile,RLCFont* TFont); 
};
