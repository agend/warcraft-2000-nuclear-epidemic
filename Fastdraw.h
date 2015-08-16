/*               Fast raster graphics routine
 *  
 *  This module presents several useful procedures for fast output,
 * using RLC graphics format (see the discription below). Before the
 * wor you must call SetRlcWindow to set properly the screen 
 * capabilities.After this you mast call SetScreenPtr to set the
 * output distanation. Uou can use LoadRLC to load the set of RLC
 * pictures into the RLCTable structure. Then you can draw the 
 * picture on the screen wit the procedure ShowRLCItem 
 */
extern int WindX; 
extern int WindY;
extern int WindLx;
extern int WindLy;
extern int WindX1;
extern int WindY1; 
extern int ScrWidth;
extern int ScrHeight;
extern int BytesPerPixel;
extern void* ScreenPtr;
extern void* RealScreenPtr;

struct RLCHeader
{
short	SizeX;
short	SizeY;
};
typedef RLCHeader* lpRLCHeader;
struct xRLCTable
{
int sign;
int	SCount;
int OfsTable[32768000];
};
typedef xRLCTable* RLCTable; 
typedef RLCTable* lpRLCTable;
struct RLCFont
{
int	FirstSymbol;
int LastSymbol;
RLCTable RLC;
};
typedef RLCFont* lpRLCFont;
void SetRLCWindow(int x,int y,int lx,int ly,int slx);
void SetScreenPtr(void);
int GetRLCWidth(RLCTable lpr,byte n);
int GetRLCHeight(RLCTable lpr,byte n);
/*      Showing the RLC picture
 * 
 *  This procedure needs window parameters to be initializated.
 */
void ShowRLC(int x,int y,void* PicPtr);
//          Loading RLC file
bool LoadRLC(LPCSTR lpFileName,RLCTable *RLCtbl);
//       Showing RLC item
void ShowRLCItem(int x,int y,lpRLCTable lprt,int n,byte nt);
void ShowRLCItemShadow(int x,int y,lpRLCTable lprt,int n);
void ClearScreen();
void ShowString(int x,int y,LPCSTR lps,lpRLCFont lpf);
void RegisterRLCFont(lpRLCFont lrf,RLCTable lpr,int fir);
void LoadPalettes();
/*
 *                   Graphics routine
 *                 RLC-format interface     
 *
 *RLC - description
 *
 *  unsigned int    Horisontal size of the picture
 *  unsigned short  Vertical size of the sub-picture
 *
 *    byte          Number of RLE substrings
 *    
 *      byte        Length of the substring
 *  
 *  byte[...]       Colors table
 */