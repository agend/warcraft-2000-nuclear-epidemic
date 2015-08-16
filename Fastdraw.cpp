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
#include "ddini.h"
#include "ResFile.h"
#include "mode.h"
extern int SCRSizeX;
extern int SCRSizeY;
extern int RSCRSizeX;
extern int RSCRSizeY;
extern int COPYSizeX;
byte PAL1[256];
byte PAL2[256];
byte PAL3[256];
byte PAL4[256];
byte PAL5[256];
byte PAL6[256];
byte PAL7[256];
extern void* offScreenPtr;
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
int OfsTable[400];
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
//Window parameters
int WindX=0; 
int WindY=0;
int WindLx;
int WindLy;
int WindX1;
int WindY1; 
int ScrWidth;
int ScrHeight;
int BytesPerPixel=1;
void* ScreenPtr=NULL;
void* RealScreenPtr=NULL;
void InitRLCWindows(){
	WindLx=SCRSizeX;
	WindLy=SCRSizeY;
	WindX1=SCRSizeX-1;
	WindY1=SCRSizeY-1; 
	ScrWidth=SCRSizeX;
	ScrHeight=SCRSizeY;
};
/*  Creating window
 */
bool ProcessMessages();
static int cntr;
static bool InCycle;
extern int RealLx;
extern int RealLy;
void SetRLCWindow(int x,int y,int lx,int ly,int slx)
{
	WindX=x;
	WindY=y;
	WindX1=x+lx-1;
	WindY1=y+ly-1;
	ScrWidth=slx;
	if(WindX<0)WindX=0;
	if(WindY<0)WindY=0;
	if(WindX1>=RealLx)WindX1=RealLx-1;
	if(WindY1>=RealLy)WindY1=RealLy-1;
	WindLx=WindX1-WindX+1;
	WindLy=WindY1-WindY+1;
}
//  Setting proper value of the screen pointer
void SetScreenPtr(void)
{
#ifdef COPYSCR
	ScreenPtr=offScreenPtr;
	RealScreenPtr=ddsd.lpSurface;
#else
	ScreenPtr=ddsd.lpSurface;
#endif
}

void ClearScreen()
{
	if(DDError) return;
	int sz=ScrWidth*ScrHeight*BytesPerPixel/4;
	__asm
	{
		push	edi
		push	esi
		mov		edi,ScreenPtr
		xor		eax,eax
		cld
		mov		ecx,sz
		rep		stosd
		pop		esi
		pop		edi
	}
}
//Showing RLC image with clipping
void ShowRLC(int x,int y,void* PicPtr)
{
	byte precomp[256];
	//for(int i=0;i<256;i++) precomp[i]=i;
	int ScrOfst=int(ScreenPtr)+y*ScrWidth+x;
	int addofs=0;
	int subline=0;
	int PLY=(lpRLCHeader(PicPtr)->SizeY)&65535;
	int PLX=(lpRLCHeader(PicPtr)->SizeX)&65535;
	if ((y+PLY-1<WindY)|(y>WindY1)||
		((x+PLX<=WindX)||(x>WindX1)||!PLY)) return;
	if (y<WindY) 
	{
		__asm	
		{	
			mov		edx,PicPtr
			add		edx,4
			xor		eax,eax
			mov		ecx,WindY
			sub		ecx,y
			xor		eax,eax
			xor		ebx,ebx
Loop1xx1:	mov		al,[edx]
			inc		edx
			or		eax,eax
			jz		Loop1xx3
Loop1xx2:	mov		bl,[edx+1]
			add		edx,ebx
			add		edx,2
			dec		eax
			jnz		Loop1xx2
Loop1xx3:	dec		cx
			jnz		Loop1xx1
			sub		edx,PicPtr
			sub		edx,4
			mov		addofs,edx
		}
		subline=WindY-y;
		ScrOfst=int(ScreenPtr)+WindY*ScrWidth+x;
	}
	if (WindY1<y+PLY-1) subline+=y+PLY-1-WindY1;
	addofs+=4;
	PLY-=subline;
	if(PLY>0){
		if(x<WindX){
			int roff=WindX-x;
			__asm{
				push	esi
				push	edi
				mov		edi,ScrOfst
				mov		esi,PicPtr
				add		esi,addofs
				xor		ecx,ecx
				xor		eax,eax
				cld
ScanLineLoop1:
				cmp		PLY,0
				je		ScanLineLoopEnd1
				push	edi
				mov		bl,[esi]
				inc		esi
				or		bl,bl
				jz		NextLine1
				mov		bh,byte ptr roff
BeginLine1:		mov		cl,[esi]
				sub		bh,cl
				add		edi,ecx
				mov		cl,[esi+1]
				//sub		bh,cl
				add		esi,2
				//clipping left code
				cmp		bh,0
				jle		ok1
				cmp		bh,cl
				jl		hdraw1
				//nothing to draw
				sub		bh,cl
				add		esi,ecx
				add		edi,ecx
				dec		bl
				jnz		BeginLine1
				pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop1
hdraw1:			//draw only small part of line
				sub		cl,bh
				mov		al,bh
				xor		bh,bh
				add		esi,eax
				add		edi,eax
ok1:
				mov		eax,ecx
				shr		ecx,2
				//jcxz	Lx11
				rep		movsd
				mov		ecx,eax
				and		ecx,3
				//jcxz	Lx21
				rep		movsb
Lx21:			dec		bl
				jnz		BeginLine1
NextLine1:		pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop1
ScanLineLoopEnd1:
				pop		edi
				pop		esi
			};
		}else if(x+PLX>=WindX1){
			int roff=WindX1-x+1;
			int part;
			__asm{
				push	esi
				push	edi
				mov		edi,ScrOfst
				mov		esi,PicPtr
				add		esi,addofs
				xor		ecx,ecx
				xor		eax,eax
				cld
ScanLineLoop2:
				cmp		PLY,0
				je		ScanLineLoopEnd2
				push	edi
				mov		bl,[esi]
				inc		esi
				or		bl,bl
				jz		NextLine2
				mov		bh,byte ptr roff
BeginLine2:		mov		cl,[esi]
				sub		bh,cl
				add		edi,ecx
				mov		cl,[esi+1]
				add		esi,2
				//clipping right code
				cmp		bh,cl
				jge		ok2
				//clipping
				cmp		bh,0
				jle		ntd2
				//partial drawing
				sub		cl,bh
				mov		part,ecx
				mov		cl,bh
				mov		eax,ecx
				shr		ecx,2
				//jcxz	Lx11_1
				rep		movsd
				mov		ecx,eax
				and		ecx,3
				//jcxz	Lx2
				rep		movsb
				add		esi,part
				jmp		ntd4
ntd2:			//scanning to the next line
				add		esi,ecx
ntd4:			dec		bl
				jz		NextLine2
ntd22:			mov		cl,[esi+1]
				add		esi,2
				add		esi,ecx
				dec		bl
				jnz		ntd22
				jmp		NextLine2
ok2:			sub		bh,cl
				mov		eax,ecx
				shr		ecx,2
				//jcxz	Lx11
				rep		movsd
Lx11:			mov		ecx,eax
				and		ecx,3
				//jcxz	Lx22
				rep		movsb
Lx22:			dec		bl
				jnz		BeginLine2
NextLine2:		pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop2
ScanLineLoopEnd2:
				pop		edi
				pop		esi
			};
		}else
		__asm
		{
			push	esi
			push	edi
			mov		edi,ScrOfst
			mov		esi,PicPtr
			add		esi,addofs
			xor		ecx,ecx
			xor		ebx,ebx
			cld
ScanLineLoop:
			cmp		PLY,0
			je		ScanLineLoopEnd
			push	edi
			mov		bl,[esi]
			inc		esi
			or		bl,bl
			jz		NextLine
BeginLine:	mov		cl,[esi]
			add		edi,ecx
			mov		cl,[esi+1]
			add		esi,2
			mov		eax,ecx
			shr		ecx,2
			jcxz	Lx1
			rep		movsd
Lx1:		mov		ecx,eax
			and		ecx,3
			jcxz	Lx2
			rep		movsb
//			xor		eax,eax
//rrr:		lodsb
//			mov		al,[precomp+eax]
//	        stosb
//			loop	rrr
//			rep		movsb
Lx2:		dec		ebx
			jnz		BeginLine
NextLine:	pop		edi
			add		edi,ScrWidth
			dec     PLY
			jmp		ScanLineLoop
ScanLineLoopEnd:
			pop		edi
			pop		esi
		}
	}
}
//End of RLC with clipping
//Showing inverse RLC image with clipping
void ShowRLCi(int x,int y,void* PicPtr)
{
	byte precomp[256];
	//for(int i=0;i<256;i++) precomp[i]=i;
	int ScrOfst=int(ScreenPtr)+y*ScrWidth+x;
	int addofs=0;
	int subline=0;
	int PLY=(lpRLCHeader(PicPtr)->SizeY)&65535;
	int PLX=(lpRLCHeader(PicPtr)->SizeX)&65535;
	if ((y+PLY-1<WindY)|(y>WindY1)||
		((x<WindX)||(x-PLX+1>=WindX1)||!PLY)) return;
	if (y<WindY) 
	{
		__asm	
		{	
			mov		edx,PicPtr
			add		edx,4
			xor		eax,eax
			mov		ecx,WindY
			sub		ecx,y
			xor		eax,eax
			xor		ebx,ebx
Loop1xx1:	mov		al,[edx]
			inc		edx
			or		eax,eax
			jz		Loop1xx3
Loop1xx2:	mov		bl,[edx+1]
			add		edx,ebx
			add		edx,2
			dec		eax
			jnz		Loop1xx2
Loop1xx3:	dec		cx
			jnz		Loop1xx1
			sub		edx,PicPtr
			sub		edx,4
			mov		addofs,edx
		}
		subline=WindY-y;
		ScrOfst=int(ScreenPtr)+WindY*ScrWidth+x;
	}
	if (WindY1<y+PLY-1) subline+=y+PLY-1-WindY1;
	addofs+=4;
	PLY-=subline;
	if(PLY>0){
		if(x>WindX1){
			int roff=x-WindX1;
			__asm{
				push	esi
				push	edi
				mov		edi,ScrOfst
				mov		esi,PicPtr
				add		esi,addofs
				xor		ecx,ecx
				xor		eax,eax
				cld
ScanLineLoop1:
				cmp		PLY,0
				je		ScanLineLoopEnd1
				push	edi
				mov		bl,[esi]
				inc		esi
				or		bl,bl
				jz		NextLine1
				mov		bh,byte ptr roff
BeginLine1:		mov		cl,[esi]
				sub		bh,cl
				sub		edi,ecx
				mov		cl,[esi+1]
				//sub		bh,cl
				add		esi,2
				//clipping left code
				cmp		bh,0
				jle		ok1
				cmp		bh,cl
				jl		hdraw1
				//nothing to draw
				sub		bh,cl
				add		esi,ecx
				sub		edi,ecx
				dec		bl
				jnz		BeginLine1
				pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop1
hdraw1:			//draw only small part of line
				sub		cl,bh
				mov		al,bh
				xor		bh,bh
				add		esi,eax
				sub		edi,eax
ok1:
				movsb
				sub		edi,2
				dec		cl
				jnz		ok1
Lx21:			dec		bl
				jnz		BeginLine1
NextLine1:		pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop1
ScanLineLoopEnd1:
				pop		edi
				pop		esi
			};
		}else if(x-PLX+1<WindX){
			int roff=x-WindX+1;
			int part;
			__asm{
				push	esi
				push	edi
				mov		edi,ScrOfst
				mov		esi,PicPtr
				add		esi,addofs
				xor		ecx,ecx
				xor		eax,eax
				cld
ScanLineLoop2:
				cmp		PLY,0
				je		ScanLineLoopEnd2
				push	edi
				mov		bl,[esi]
				inc		esi
				or		bl,bl
				jz		NextLine2
				mov		bh,byte ptr roff
BeginLine2:		mov		cl,[esi]
				sub		bh,cl
				sub		edi,ecx
				mov		cl,[esi+1]
				add		esi,2
				//clipping right code
				cmp		bh,cl
				jge		ok2
				//clipping
				cmp		bh,0
				jle		ntd2
				//partial drawing
				sub		cl,bh
				mov		part,ecx
				mov		cl,bh
lxsd1:			movsb
				sub		edi,2
				dec		cl
				jnz		lxsd1
				add		esi,part
				jmp		ntd4
ntd2:			//scanning to the next line
				add		esi,ecx
ntd4:			dec		bl
				jz		NextLine2
ntd22:			mov		cl,[esi+1]
				add		esi,2
				add		esi,ecx
				dec		bl
				jnz		ntd22
				jmp		NextLine2
ok2:			sub		bh,cl
lkfr1:			movsb
				sub		edi,2
				dec		cl
				jnz		lkfr1
Lx22:			dec		bl
				jnz		BeginLine2
NextLine2:		pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop2
ScanLineLoopEnd2:
				pop		edi
				pop		esi
			};
		}else
		__asm
		{
			push	esi
			push	edi
			mov		edi,ScrOfst
			mov		esi,PicPtr
			add		esi,addofs
			xor		ecx,ecx
			xor		ebx,ebx
			cld
ScanLineLoop:
			cmp		PLY,0
			je		ScanLineLoopEnd
			push	edi
			mov		bl,[esi]
			inc		esi
			or		bl,bl
			jz		NextLine
BeginLine:	mov		cl,[esi]
			sub		edi,ecx
			mov		cl,[esi+1]
			add		esi,2
ghte:		movsb
			sub		edi,2
			dec		cl
			jnz		ghte
Lx2:		dec		ebx
			jnz		BeginLine
NextLine:	pop		edi
			add		edi,ScrWidth
			dec     PLY
			jmp		ScanLineLoop
ScanLineLoopEnd:
			pop		edi
			pop		esi
		}
	}
}
//End of RLC with clipping & with palette
void ShowRLCpal(int x,int y,void* PicPtr,byte* pal)
{
	byte precomp[256];
	//for(int i=0;i<256;i++) precomp[i]=i;
	int ScrOfst=int(ScreenPtr)+y*ScrWidth+x;
	int addofs=0;
	int subline=0;
	int PLY=(lpRLCHeader(PicPtr)->SizeY)&65535;
	int PLX=(lpRLCHeader(PicPtr)->SizeX)&65535;
	if ((y+PLY-1<WindY)|(y>WindY1)||
		((x+PLX<=WindX)||(x>WindX1)||!PLY)) return;
	if (y<WindY) 
	{
		__asm	
		{	
			mov		edx,PicPtr
			add		edx,4
			xor		eax,eax
			mov		ecx,WindY
			sub		ecx,y
			xor		eax,eax
			xor		ebx,ebx
Loop1xx1:	mov		al,[edx]
			inc		edx
			or		eax,eax
			jz		Loop1xx3
Loop1xx2:	mov		bl,[edx+1]
			add		edx,ebx
			add		edx,2
			dec		eax
			jnz		Loop1xx2
Loop1xx3:	dec		cx
			jnz		Loop1xx1
			sub		edx,PicPtr
			sub		edx,4
			mov		addofs,edx
		}
		subline=WindY-y;
		ScrOfst=int(ScreenPtr)+WindY*ScrWidth+x;
	}
	if (WindY1<y+PLY-1) subline+=y+PLY-1-WindY1;
	addofs+=4;
	PLY-=subline;
	if(PLY>0){
		if(x<WindX){
			int roff=WindX-x;
			__asm{
				push	esi
				push	edi
				mov		edi,ScrOfst
				mov		esi,PicPtr
				add		esi,addofs
				xor		ecx,ecx
				xor		eax,eax
				mov		ebx,pal
				cld
ScanLineLoop1:
				cmp		PLY,0
				je		ScanLineLoopEnd1
				push	edi
				mov		dl,[esi]
				inc		esi
				or		dl,dl
				jz		NextLine1
				mov		dh,byte ptr roff
BeginLine1:		mov		cl,[esi]
				sub		dh,cl
				add		edi,ecx
				mov		cl,[esi+1]
				//sub		bh,cl
				add		esi,2
				//clipping left code
				cmp		dh,0
				jle		ok1
				cmp		dh,cl
				jl		hdraw1
				//nothing to draw
				sub		dh,cl
				add		esi,ecx
				add		edi,ecx
				dec		dl
				jnz		BeginLine1
				pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop1
hdraw1:			//draw only small part of line
				sub		cl,dh
				mov		al,dh
				xor		dh,dh
				add		esi,eax
				add		edi,eax
ok1:
				lodsb
				mov		al,[ebx+eax]
				stosb
				dec		cl
				jnz		ok1
Lx21:			dec		dl
				jnz		BeginLine1
NextLine1:		pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop1
ScanLineLoopEnd1:
				pop		edi
				pop		esi
			};
		}else if(x+PLX>=WindX1){
			int roff=WindX1-x+1;
			int part;
			__asm{
				push	esi
				push	edi
				mov		edi,ScrOfst
				mov		esi,PicPtr
				add		esi,addofs
				xor		ecx,ecx
				xor		eax,eax
				mov		ebx,pal
				cld
ScanLineLoop2:
				cmp		PLY,0
				je		ScanLineLoopEnd2
				push	edi
				mov		dl,[esi]
				inc		esi
				or		dl,dl
				jz		NextLine2
				mov		dh,byte ptr roff
BeginLine2:		mov		cl,[esi]
				sub		dh,cl
				add		edi,ecx
				mov		cl,[esi+1]
				add		esi,2
				//clipping right code
				cmp		dh,cl
				jge		ok2
				//clipping
				cmp		dh,0
				jle		ntd2
				//partial drawing
				sub		cl,dh
				mov		part,ecx
				mov		cl,dh
kkj1:			lodsb	
				mov		al,[ebx+eax]
				stosb
				dec		cl
				jnz		kkj1
				add		esi,part
				jmp		ntd4
ntd2:			//scanning to the next line
				add		esi,ecx
ntd4:			dec		dl
				jz		NextLine2
ntd22:			mov		cl,[esi+1]
				add		esi,2
				add		esi,ecx
				dec		dl
				jnz		ntd22
				jmp		NextLine2
ok2:			sub		dh,cl
kkj2:			lodsb	
				mov		al,[ebx+eax]
				stosb
				dec		cl
				jnz		kkj2
Lx22:			dec		dl
				jnz		BeginLine2
NextLine2:		pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop2
ScanLineLoopEnd2:
				pop		edi
				pop		esi
			};
		}else
		__asm
		{
			push	esi
			push	edi
			mov		edi,ScrOfst
			mov		esi,PicPtr
			add		esi,addofs
			xor		ecx,ecx
			xor		ebx,ebx
			xor		eax,eax
			mov		ebx,pal
			cld
ScanLineLoop:
			cmp		PLY,0
			je		ScanLineLoopEnd
			push	edi
			mov		dl,[esi]
			inc		esi
			or		dl,dl
			jz		NextLine
BeginLine:	mov		cl,[esi]
			add		edi,ecx
			mov		cl,[esi+1]
			add		esi,2
hgaw:		lodsb
			mov		al,[ebx+eax]
			stosb
			dec		cl
			jnz		hgaw
Lx2:		dec		dl
			jnz		BeginLine
NextLine:	pop		edi
			add		edi,ScrWidth
			dec     PLY
			jmp		ScanLineLoop
ScanLineLoopEnd:
			pop		edi
			pop		esi
		}
	}
}
//End of RLC with clipping & encoding
//Showing inverse RLC image with clipping & encodint
void ShowRLCipal(int x,int y,void* PicPtr,byte* pal)
{
	byte precomp[256];
	//for(int i=0;i<256;i++) precomp[i]=i;
	int ScrOfst=int(ScreenPtr)+y*ScrWidth+x;
	int addofs=0;
	int subline=0;
	int PLY=(lpRLCHeader(PicPtr)->SizeY)&65535;
	int PLX=(lpRLCHeader(PicPtr)->SizeX)&65535;
	if ((y+PLY-1<WindY)|(y>WindY1)||
		((x<WindX)||(x-PLX+1>=WindX1)||!PLY)) return;
	if (y<WindY) 
	{
		__asm	
		{	
			mov		edx,PicPtr
			add		edx,4
			xor		eax,eax
			mov		ecx,WindY
			sub		ecx,y
			xor		eax,eax
			xor		ebx,ebx
Loop1xx1:	mov		al,[edx]
			inc		edx
			or		eax,eax
			jz		Loop1xx3
Loop1xx2:	mov		bl,[edx+1]
			add		edx,ebx
			add		edx,2
			dec		eax
			jnz		Loop1xx2
Loop1xx3:	dec		cx
			jnz		Loop1xx1
			sub		edx,PicPtr
			sub		edx,4
			mov		addofs,edx
		}
		subline=WindY-y;
		ScrOfst=int(ScreenPtr)+WindY*ScrWidth+x;
	}
	if (WindY1<y+PLY-1) subline+=y+PLY-1-WindY1;
	addofs+=4;
	PLY-=subline;
	if(PLY>0){
		if(x>WindX1){
			int roff=x-WindX1;
			__asm{
				push	esi
				push	edi
				mov		edi,ScrOfst
				mov		esi,PicPtr
				add		esi,addofs
				xor		ecx,ecx
				xor		eax,eax
				mov		ebx,pal
				cld
ScanLineLoop1:
				cmp		PLY,0
				je		ScanLineLoopEnd1
				push	edi
				mov		dl,[esi]
				inc		esi
				or		dl,dl
				jz		NextLine1
				mov		dh,byte ptr roff
BeginLine1:		mov		cl,[esi]
				sub		dh,cl
				sub		edi,ecx
				mov		cl,[esi+1]
				//sub		bh,cl
				add		esi,2
				//clipping left code
				cmp		dh,0
				jle		ok1
				cmp		dh,cl
				jl		hdraw1
				//nothing to draw
				sub		dh,cl
				add		esi,ecx
				sub		edi,ecx
				dec		dl
				jnz		BeginLine1
				pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop1
hdraw1:			//draw only small part of line
				sub		cl,dh
				mov		al,dh
				xor		dh,dh
				add		esi,eax
				sub		edi,eax
ok1:			lodsb
				mov		al,[ebx+eax]
				mov		[edi],al
				dec		edi
				dec		cl
				jnz		ok1
Lx21:			dec		dl
				jnz		BeginLine1
NextLine1:		pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop1
ScanLineLoopEnd1:
				pop		edi
				pop		esi
			};
		}else if(x-PLX+1<WindX){
			int roff=x-WindX+1;
			int part;
			__asm{
				push	esi
				push	edi
				mov		edi,ScrOfst
				mov		esi,PicPtr
				add		esi,addofs
				xor		ecx,ecx
				xor		eax,eax
				mov		ebx,pal
				cld
ScanLineLoop2:
				cmp		PLY,0
				je		ScanLineLoopEnd2
				push	edi
				mov		dl,[esi]
				inc		esi
				or		dl,dl
				jz		NextLine2
				mov		dh,byte ptr roff
BeginLine2:		mov		cl,[esi]
				sub		dh,cl
				sub		edi,ecx
				mov		cl,[esi+1]
				add		esi,2
				//clipping right code
				cmp		dh,cl
				jge		ok2
				//clipping
				cmp		dh,0
				jle		ntd2
				//partial drawing
				sub		cl,dh
				mov		part,ecx
				mov		cl,dh
lxsd1:			lodsb
				mov		al,[ebx+eax]
				mov		[edi],al
				dec		edi
				dec		cl
				jnz		lxsd1
				add		esi,part
				jmp		ntd4
ntd2:			//scanning to the next line
				add		esi,ecx
ntd4:			dec		dl
				jz		NextLine2
ntd22:			mov		cl,[esi+1]
				add		esi,2
				add		esi,ecx
				dec		dl
				jnz		ntd22
				jmp		NextLine2
ok2:			sub		dh,cl
lkfr1:			lodsb
				mov		al,[ebx+eax]
				mov		[edi],al
				dec		edi
				dec		cl
				jnz		lkfr1
Lx22:			dec		dl
				jnz		BeginLine2
NextLine2:		pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop2
ScanLineLoopEnd2:
				pop		edi
				pop		esi
			};
		}else
		__asm
		{
			push	esi
			push	edi
			mov		edi,ScrOfst
			mov		esi,PicPtr
			add		esi,addofs
			xor		ecx,ecx
			xor		ebx,ebx
			xor		eax,eax
			mov		ebx,pal
			cld
ScanLineLoop:
			cmp		PLY,0
			je		ScanLineLoopEnd
			push	edi
			mov		dl,[esi]
			inc		esi
			or		dl,dl
			jz		NextLine
BeginLine:	mov		cl,[esi]
			sub		edi,ecx
			mov		cl,[esi+1]
			add		esi,2
ghte:		lodsb
			mov		al,[eax+ebx]
			mov		[edi],al
			dec		edi
			dec		cl
			jnz		ghte
Lx2:		dec		dl
			jnz		BeginLine
NextLine:	pop		edi
			add		edi,ScrWidth
			dec     PLY
			jmp		ScanLineLoop
ScanLineLoopEnd:
			pop		edi
			pop		esi
		}
	}
}
//End of inverted RLC with clipping & encoding

//End of RLC with clipping & with palette->fon
void ShowRLCfonpal(int x,int y,void* PicPtr,byte* pal)
{
	byte precomp[256];
	//for(int i=0;i<256;i++) precomp[i]=i;
	int ScrOfst=int(ScreenPtr)+y*ScrWidth+x;
	int addofs=0;
	int subline=0;
	int PLY=(lpRLCHeader(PicPtr)->SizeY)&65535;
	int PLX=(lpRLCHeader(PicPtr)->SizeX)&65535;
	if ((y+PLY-1<WindY)|(y>WindY1)||
		((x+PLX<=WindX)||(x>WindX1)||!PLY)) return;
	if (y<WindY) 
	{
		__asm	
		{	
			mov		edx,PicPtr
			add		edx,4
			xor		eax,eax
			mov		ecx,WindY
			sub		ecx,y
			xor		eax,eax
			xor		ebx,ebx
Loop1xx1:	mov		al,[edx]
			inc		edx
			or		eax,eax
			jz		Loop1xx3
Loop1xx2:	mov		bl,[edx+1]
			add		edx,ebx
			add		edx,2
			dec		eax
			jnz		Loop1xx2
Loop1xx3:	dec		cx
			jnz		Loop1xx1
			sub		edx,PicPtr
			sub		edx,4
			mov		addofs,edx
		}
		subline=WindY-y;
		ScrOfst=int(ScreenPtr)+WindY*ScrWidth+x;
	}
	if (WindY1<y+PLY-1) subline+=y+PLY-1-WindY1;
	addofs+=4;
	PLY-=subline;
	if(PLY>0){
		if(x<WindX){
			int roff=WindX-x;
			__asm{
				push	esi
				push	edi
				mov		edi,ScrOfst
				mov		esi,PicPtr
				add		esi,addofs
				xor		ecx,ecx
				xor		eax,eax
				mov		ebx,pal
				cld
ScanLineLoop1:
				cmp		PLY,0
				je		ScanLineLoopEnd1
				push	edi
				mov		dl,[esi]
				inc		esi
				or		dl,dl
				jz		NextLine1
				mov		dh,byte ptr roff
BeginLine1:		mov		cl,[esi]
				sub		dh,cl
				add		edi,ecx
				mov		cl,[esi+1]
				//sub		bh,cl
				add		esi,2
				//clipping left code
				cmp		dh,0
				jle		okk1
				cmp		dh,cl
				jl		hdraw1
				//nothing to draw
				sub		dh,cl
				add		esi,ecx
				add		edi,ecx
				dec		dl
				jnz		BeginLine1
				pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop1
hdraw1:			//draw only small part of line
				sub		cl,dh
				mov		al,dh
				xor		dh,dh
				add		esi,eax
				add		edi,eax
okk1:			add		esi,ecx
ok1:			mov		al,[edi]
				mov		al,[ebx+eax]
				stosb
				dec		cl
				jnz		ok1
Lx21:			dec		dl
				jnz		BeginLine1
NextLine1:		pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop1
ScanLineLoopEnd1:
				pop		edi
				pop		esi
			};
		}else if(x+PLX>=WindX1){
			int roff=WindX1-x+1;
			int part;
			__asm{
				push	esi
				push	edi
				mov		edi,ScrOfst
				mov		esi,PicPtr
				add		esi,addofs
				xor		ecx,ecx
				xor		eax,eax
				mov		ebx,pal
				cld
ScanLineLoop2:
				cmp		PLY,0
				je		ScanLineLoopEnd2
				push	edi
				mov		dl,[esi]
				inc		esi
				or		dl,dl
				jz		NextLine2
				mov		dh,byte ptr roff
BeginLine2:		mov		cl,[esi]
				sub		dh,cl
				add		edi,ecx
				mov		cl,[esi+1]
				add		esi,2
				//clipping right code
				cmp		dh,cl
				jge		ok2
				//clipping
				cmp		dh,0
				jle		ntd2
				//partial drawing
				sub		cl,dh
				mov		part,ecx
				mov		cl,dh
				add		esi,ecx
kkj1:			mov		al,[edi]
				mov		al,[ebx+eax]
				stosb
				dec		cl
				jnz		kkj1
				add		esi,part
				jmp		ntd4
ntd2:			//scanning to the next line
				add		esi,ecx
ntd4:			dec		dl
				jz		NextLine2
ntd22:			mov		cl,[esi+1]
				add		esi,2
				add		esi,ecx
				dec		dl
				jnz		ntd22
				jmp		NextLine2
ok2:			sub		dh,cl
				add		esi,ecx
kkj3:			mov		al,[edi]
				mov		al,[ebx+eax]
				stosb
				dec		cl
				jnz		kkj3
Lx22:			dec		dl
				jnz		BeginLine2
NextLine2:		pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop2
ScanLineLoopEnd2:
				pop		edi
				pop		esi
			};
		}else
		__asm
		{
			push	esi
			push	edi
			mov		edi,ScrOfst
			mov		esi,PicPtr
			add		esi,addofs
			xor		ecx,ecx
			xor		ebx,ebx
			xor		eax,eax
			mov		ebx,pal
			cld
ScanLineLoop:
			cmp		PLY,0
			je		ScanLineLoopEnd
			push	edi
			mov		dl,[esi]
			inc		esi
			or		dl,dl
			jz		NextLine
BeginLine:	mov		cl,[esi]
			add		edi,ecx
			mov		cl,[esi+1]
			add		esi,2
			add		esi,ecx
hgaw:		mov		al,[edi]
			mov		al,[ebx+eax]
			stosb
			dec		cl
			jnz		hgaw
Lx2:		dec		dl
			jnz		BeginLine
NextLine:	pop		edi
			add		edi,ScrWidth
			dec     PLY
			jmp		ScanLineLoop
ScanLineLoopEnd:
			pop		edi
			pop		esi
		}
	}
}
//End of RLC with clipping & encoding
//Showing inverse RLC image with clipping & encodint
void ShowRLCifonpal(int x,int y,void* PicPtr,byte* pal)
{
	byte precomp[256];
	//for(int i=0;i<256;i++) precomp[i]=i;
	int ScrOfst=int(ScreenPtr)+y*ScrWidth+x;
	int addofs=0;
	int subline=0;
	int PLY=(lpRLCHeader(PicPtr)->SizeY)&65535;
	int PLX=(lpRLCHeader(PicPtr)->SizeX)&65535;
	if ((y+PLY-1<WindY)|(y>WindY1)||
		((x<WindX)||(x-PLX+1>=WindX1)||!PLY)) return;
	if (y<WindY) 
	{
		__asm	
		{	
			mov		edx,PicPtr
			add		edx,4
			xor		eax,eax
			mov		ecx,WindY
			sub		ecx,y
			xor		eax,eax
			xor		ebx,ebx
Loop1xx1:	mov		al,[edx]
			inc		edx
			or		eax,eax
			jz		Loop1xx3
Loop1xx2:	mov		bl,[edx+1]
			add		edx,ebx
			add		edx,2
			dec		eax
			jnz		Loop1xx2
Loop1xx3:	dec		cx
			jnz		Loop1xx1
			sub		edx,PicPtr
			sub		edx,4
			mov		addofs,edx
		}
		subline=WindY-y;
		ScrOfst=int(ScreenPtr)+WindY*ScrWidth+x;
	}
	if (WindY1<y+PLY-1) subline+=y+PLY-1-WindY1;
	addofs+=4;
	PLY-=subline;
	if(PLY>0){
		if(x>WindX1){
			int roff=x-WindX1;
			__asm{
				push	esi
				push	edi
				mov		edi,ScrOfst
				mov		esi,PicPtr
				add		esi,addofs
				xor		ecx,ecx
				xor		eax,eax
				mov		ebx,pal
				cld
ScanLineLoop1:
				cmp		PLY,0
				je		ScanLineLoopEnd1
				push	edi
				mov		dl,[esi]
				inc		esi
				or		dl,dl
				jz		NextLine1
				mov		dh,byte ptr roff
BeginLine1:		mov		cl,[esi]
				sub		dh,cl
				sub		edi,ecx
				mov		cl,[esi+1]
				//sub		bh,cl
				add		esi,2
				//clipping left code
				cmp		dh,0
				jle		okk1
				cmp		dh,cl
				jl		hdraw1
				//nothing to draw
				sub		dh,cl
				add		esi,ecx
				sub		edi,ecx
				dec		dl
				jnz		BeginLine1
				pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop1
hdraw1:			//draw only small part of line
				sub		cl,dh
				mov		al,dh
				xor		dh,dh
				add		esi,eax
				sub		edi,eax
okk1:			add		esi,ecx
				//or		cl,cl
				//jz		Lx21
ok1:			mov		al,[edi]
				mov		al,[ebx+eax]
				mov		[edi],al
				dec		edi
				dec		cl
				jnz		ok1
Lx21:			dec		dl
				jnz		BeginLine1
NextLine1:		pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop1
ScanLineLoopEnd1:
				pop		edi
				pop		esi
			};
		}else if(x-PLX+1<WindX){
			int roff=x-WindX+1;
			int part;
			__asm{
				push	esi
				push	edi
				mov		edi,ScrOfst
				mov		esi,PicPtr
				add		esi,addofs
				xor		ecx,ecx
				xor		eax,eax
				mov		ebx,pal
				cld
ScanLineLoop2:
				cmp		PLY,0
				je		ScanLineLoopEnd2
				push	edi
				mov		dl,[esi]
				inc		esi
				or		dl,dl
				jz		NextLine2
				mov		dh,byte ptr roff
BeginLine2:		mov		cl,[esi]
				sub		dh,cl
				sub		edi,ecx
				mov		cl,[esi+1]
				add		esi,2
				//clipping right code
				cmp		dh,cl
				jge		ok2
				//clipping
				cmp		dh,0
				jle		ntd2
				//partial drawing
				sub		cl,dh
				mov		part,ecx
				mov		cl,dh
				add		esi,ecx
lxsd1:			mov		al,[edi]
				mov		al,[ebx+eax]
				mov		[edi],al
				dec		edi
				dec		cl
				jnz		lxsd1
				add		esi,part
				jmp		ntd4
ntd2:			//scanning to the next line
				add		esi,ecx
ntd4:			dec		dl
				jz		NextLine2
ntd22:			mov		cl,[esi+1]
				add		esi,2
				add		esi,ecx
				dec		dl
				jnz		ntd22
				jmp		NextLine2
ok2:			sub		dh,cl
				add		esi,ecx
lkfr1:			mov		al,[edi]
				mov		al,[ebx+eax]
				mov		[edi],al
				dec		edi
				dec		cl
				jnz		lkfr1
Lx22:			dec		dl
				jnz		BeginLine2
NextLine2:		pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop2
ScanLineLoopEnd2:
				pop		edi
				pop		esi
			};
		}else
		__asm
		{
			push	esi
			push	edi
			mov		edi,ScrOfst
			mov		esi,PicPtr
			add		esi,addofs
			xor		ecx,ecx
			xor		ebx,ebx
			xor		eax,eax
			mov		ebx,pal
			cld
ScanLineLoop:
			cmp		PLY,0
			je		ScanLineLoopEnd
			push	edi
			mov		dl,[esi]
			inc		esi
			or		dl,dl
			jz		NextLine
BeginLine:	mov		cl,[esi]
			sub		edi,ecx
			mov		cl,[esi+1]
			add		esi,2
			add		esi,ecx
ghte:		mov		al,[edi]
			mov		al,[eax+ebx]
			mov		[edi],al
			dec		edi
			dec		cl
			jnz		ghte
Lx2:		dec		dl
			jnz		BeginLine
NextLine:	pop		edi
			add		edi,ScrWidth
			dec     PLY
			jmp		ScanLineLoop
ScanLineLoopEnd:
			pop		edi
			pop		esi
		}
	}
}
//End of inverted RLC with clipping & encoding->fon


//End of RLC with clipping & with palette(half-transparent fog)
void ShowRLChtpal(int x,int y,void* PicPtr,byte* pal)
{
	byte precomp[256];
	//for(int i=0;i<256;i++) precomp[i]=i;
	int ScrOfst=int(ScreenPtr)+y*ScrWidth+x;
	int addofs=0;
	int subline=0;
	int PLY=(lpRLCHeader(PicPtr)->SizeY)&65535;
	int PLX=(lpRLCHeader(PicPtr)->SizeX)&65535;
	if ((y+PLY-1<WindY)|(y>WindY1)||
		((x+PLX<=WindX)||(x>WindX1)||!PLY)) return;
	if (y<WindY) 
	{
		__asm	
		{	
			mov		edx,PicPtr
			add		edx,4
			xor		eax,eax
			mov		ecx,WindY
			sub		ecx,y
			xor		eax,eax
			xor		ebx,ebx
Loop1xx1:	mov		al,[edx]
			inc		edx
			or		eax,eax
			jz		Loop1xx3
Loop1xx2:	mov		bl,[edx+1]
			add		edx,ebx
			add		edx,2
			dec		eax
			jnz		Loop1xx2
Loop1xx3:	dec		cx
			jnz		Loop1xx1
			sub		edx,PicPtr
			sub		edx,4
			mov		addofs,edx
		}
		subline=WindY-y;
		ScrOfst=int(ScreenPtr)+WindY*ScrWidth+x;
	}
	if (WindY1<y+PLY-1) subline+=y+PLY-1-WindY1;
	addofs+=4;
	PLY-=subline;
	if(PLY>0){
		if(x<WindX){
			int roff=WindX-x;
			__asm{
				push	esi
				push	edi
				mov		edi,ScrOfst
				mov		esi,PicPtr
				add		esi,addofs
				xor		ecx,ecx
				xor		eax,eax
				mov		ebx,pal
				cld
ScanLineLoop1:
				cmp		PLY,0
				je		ScanLineLoopEnd1
				push	edi
				mov		dl,[esi]
				inc		esi
				or		dl,dl
				jz		NextLine1
				mov		dh,byte ptr roff
BeginLine1:		mov		cl,[esi]
				sub		dh,cl
				add		edi,ecx
				mov		cl,[esi+1]
				//sub		bh,cl
				add		esi,2
				//clipping left code
				cmp		dh,0
				jle		ok1
				cmp		dh,cl
				jl		hdraw1
				//nothing to draw
				sub		dh,cl
				add		esi,ecx
				add		edi,ecx
				dec		dl
				jnz		BeginLine1
				pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop1
hdraw1:			//draw only small part of line
				sub		cl,dh
				xor		eax,eax
				mov		al,dh
				xor		dh,dh
				add		esi,eax
				add		edi,eax
ok1:
				mov		ah,[esi]
				inc		esi
				mov		al,[edi]
				mov		al,[ebx+eax]
				stosb
				dec		cl
				jnz		ok1
Lx21:			dec		dl
				jnz		BeginLine1
NextLine1:		pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop1
ScanLineLoopEnd1:
				pop		edi
				pop		esi
			};
		}else if(x+PLX>=WindX1){
			int roff=WindX1-x+1;
			int part;
			__asm{
				push	esi
				push	edi
				mov		edi,ScrOfst
				mov		esi,PicPtr
				add		esi,addofs
				xor		ecx,ecx
				xor		eax,eax
				mov		ebx,pal
				cld
ScanLineLoop2:
				cmp		PLY,0
				je		ScanLineLoopEnd2
				push	edi
				mov		dl,[esi]
				inc		esi
				or		dl,dl
				jz		NextLine2
				mov		dh,byte ptr roff
BeginLine2:		mov		cl,[esi]
				sub		dh,cl
				add		edi,ecx
				mov		cl,[esi+1]
				add		esi,2
				//clipping right code
				cmp		dh,cl
				jge		ok2
				//clipping
				cmp		dh,0
				jle		ntd2
				//partial drawing
				sub		cl,dh
				mov		part,ecx
				mov		cl,dh
kkj1:			mov		ah,[esi]
				inc		esi
				mov		al,[edi]	
				mov		al,[ebx+eax]
				stosb
				dec		cl
				jnz		kkj1
				add		esi,part
				jmp		ntd4
ntd2:			//scanning to the next line
				add		esi,ecx
ntd4:			dec		dl
				jz		NextLine2
ntd22:			mov		cl,[esi+1]
				add		esi,2
				add		esi,ecx
				dec		dl
				jnz		ntd22
				jmp		NextLine2
ok2:			sub		dh,cl
kkj4:			mov		ah,[esi]
				inc		esi
				mov		al,[edi]	
				mov		al,[ebx+eax]
				stosb
				dec		cl
				jnz		kkj4
Lx22:			dec		dl
				jnz		BeginLine2
NextLine2:		pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop2
ScanLineLoopEnd2:
				pop		edi
				pop		esi
			};
		}else
		__asm
		{
			push	esi
			push	edi
			mov		edi,ScrOfst
			mov		esi,PicPtr
			add		esi,addofs
			xor		ecx,ecx
			xor		ebx,ebx
			xor		eax,eax
			mov		ebx,pal
			cld
ScanLineLoop:
			cmp		PLY,0
			je		ScanLineLoopEnd
			push	edi
			mov		dl,[esi]
			inc		esi
			or		dl,dl
			jz		NextLine
BeginLine:	mov		cl,[esi]
			add		edi,ecx
			mov		cl,[esi+1]
			add		esi,2
hgaw:		mov		ah,[esi]
			inc		esi
			mov		al,[edi]
			mov		al,[ebx+eax]
			stosb
			dec		cl
			jnz		hgaw
Lx2:		dec		dl
			jnz		BeginLine
NextLine:	pop		edi
			add		edi,ScrWidth
			dec     PLY
			jmp		ScanLineLoop
ScanLineLoopEnd:
			pop		edi
			pop		esi
		}
	}
}
//End of RLC with clipping & encoding
//Showing inverse RLC image with clipping & encodint(half-transparent fog)
void ShowRLCihtpal(int x,int y,void* PicPtr,byte* pal)
{
	byte precomp[256];
	//for(int i=0;i<256;i++) precomp[i]=i;
	int ScrOfst=int(ScreenPtr)+y*ScrWidth+x;
	int addofs=0;
	int subline=0;
	int PLY=(lpRLCHeader(PicPtr)->SizeY)&65535;
	int PLX=(lpRLCHeader(PicPtr)->SizeX)&65535;
	if ((y+PLY-1<WindY)|(y>WindY1)||
		((x<WindX)||(x-PLX+1>=WindX1)||!PLY)) return;
	if (y<WindY) 
	{
		__asm	
		{	
			mov		edx,PicPtr
			add		edx,4
			xor		eax,eax
			mov		ecx,WindY
			sub		ecx,y
			xor		eax,eax
			xor		ebx,ebx
Loop1xx1:	mov		al,[edx]
			inc		edx
			or		eax,eax
			jz		Loop1xx3
Loop1xx2:	mov		bl,[edx+1]
			add		edx,ebx
			add		edx,2
			dec		eax
			jnz		Loop1xx2
Loop1xx3:	dec		cx
			jnz		Loop1xx1
			sub		edx,PicPtr
			sub		edx,4
			mov		addofs,edx
		}
		subline=WindY-y;
		ScrOfst=int(ScreenPtr)+WindY*ScrWidth+x;
	}
	if (WindY1<y+PLY-1) subline+=y+PLY-1-WindY1;
	addofs+=4;
	PLY-=subline;
	if(PLY>0){
		if(x>WindX1){
			int roff=x-WindX1;
			__asm{
				push	esi
				push	edi
				mov		edi,ScrOfst
				mov		esi,PicPtr
				add		esi,addofs
				xor		ecx,ecx
				xor		eax,eax
				mov		ebx,pal
				cld
ScanLineLoop1:
				cmp		PLY,0
				je		ScanLineLoopEnd1
				push	edi
				mov		dl,[esi]
				inc		esi
				or		dl,dl
				jz		NextLine1
				mov		dh,byte ptr roff
BeginLine1:		mov		cl,[esi]
				sub		dh,cl
				sub		edi,ecx
				mov		cl,[esi+1]
				//sub		bh,cl
				add		esi,2
				//clipping left code
				cmp		dh,0
				jle		ok1
				cmp		dh,cl
				jl		hdraw1
				//nothing to draw
				sub		dh,cl
				add		esi,ecx
				sub		edi,ecx
				dec		dl
				jnz		BeginLine1
				pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop1
hdraw1:			//draw only small part of line
				xor		eax,eax
				sub		cl,dh
				mov		al,dh
				xor		dh,dh
				add		esi,eax
				sub		edi,eax
ok1:			mov		ah,[esi]
				inc		esi
				mov		al,[edi]
				mov		al,[ebx+eax]
				mov		[edi],al
				dec		edi
				dec		cl
				jnz		ok1
Lx21:			dec		dl
				jnz		BeginLine1
NextLine1:		pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop1
ScanLineLoopEnd1:
				pop		edi
				pop		esi
			};
		}else if(x-PLX+1<WindX){
			int roff=x-WindX+1;
			int part;
			__asm{
				push	esi
				push	edi
				mov		edi,ScrOfst
				mov		esi,PicPtr
				add		esi,addofs
				xor		ecx,ecx
				xor		eax,eax
				mov		ebx,pal
				cld
ScanLineLoop2:
				cmp		PLY,0
				je		ScanLineLoopEnd2
				push	edi
				mov		dl,[esi]
				inc		esi
				or		dl,dl
				jz		NextLine2
				mov		dh,byte ptr roff
BeginLine2:		mov		cl,[esi]
				sub		dh,cl
				sub		edi,ecx
				mov		cl,[esi+1]
				add		esi,2
				//clipping right code
				cmp		dh,cl
				jge		ok2
				//clipping
				cmp		dh,0
				jle		ntd2
				//partial drawing
				sub		cl,dh
				mov		part,ecx
				mov		cl,dh
lxsd1:			mov		ah,[esi]
				inc		esi
				mov		al,[edi]
				mov		al,[ebx+eax]
				mov		[edi],al
				dec		edi
				dec		cl
				jnz		lxsd1
				add		esi,part
				jmp		ntd4
ntd2:			//scanning to the next line
				add		esi,ecx
ntd4:			dec		dl
				jz		NextLine2
ntd22:			mov		cl,[esi+1]
				add		esi,2
				add		esi,ecx
				dec		dl
				jnz		ntd22
				jmp		NextLine2
ok2:			sub		dh,cl
lkfr1:			mov		ah,[esi]
				inc		esi
				mov		al,[edi]
				mov		al,[ebx+eax]
				mov		[edi],al
				dec		edi
				dec		cl
				jnz		lkfr1
Lx22:			dec		dl
				jnz		BeginLine2
NextLine2:		pop		edi
				add		edi,ScrWidth
				dec     PLY
				jmp		ScanLineLoop2
ScanLineLoopEnd2:
				pop		edi
				pop		esi
			};
		}else
		__asm
		{
			push	esi
			push	edi
			mov		edi,ScrOfst
			mov		esi,PicPtr
			add		esi,addofs
			xor		ecx,ecx
			xor		ebx,ebx
			xor		eax,eax
			mov		ebx,pal
			cld
ScanLineLoop:
			cmp		PLY,0
			je		ScanLineLoopEnd
			push	edi
			mov		dl,[esi]
			inc		esi
			or		dl,dl
			jz		NextLine
BeginLine:	mov		cl,[esi]
			sub		edi,ecx
			mov		cl,[esi+1]
			add		esi,2
ghte:		mov		ah,[esi]
			inc		esi
			mov		al,[edi]
			mov		al,[eax+ebx]
			mov		[edi],al
			dec		edi
			dec		cl
			jnz		ghte
Lx2:		dec		dl
			jnz		BeginLine
NextLine:	pop		edi
			add		edi,ScrWidth
			dec     PLY
			jmp		ScanLineLoop
ScanLineLoopEnd:
			pop		edi
			pop		esi
		}
	}
}
//End of inverted RLC with clipping & encoding(half-transparent fog)




















void ShowRLCp1(int x,int y,void* PicPtr)
{
	ShowRLCpal(x,y,PicPtr,PAL1);
}
void ShowRLCp2(int x,int y,void* PicPtr)
{
	ShowRLCpal(x,y,PicPtr,PAL2);
}
void ShowRLCp3(int x,int y,void* PicPtr)
{
	ShowRLCpal(x,y,PicPtr,PAL3);
}
void ShowRLCp4(int x,int y,void* PicPtr)
{
	ShowRLCpal(x,y,PicPtr,PAL4);
}
void ShowRLCp5(int x,int y,void* PicPtr)
{
	ShowRLCpal(x,y,PicPtr,PAL5);
}
void ShowRLCp6(int x,int y,void* PicPtr)
{
	ShowRLCpal(x,y,PicPtr,PAL6);
}
void ShowRLCp7(int x,int y,void* PicPtr)
{
	ShowRLCpal(x,y,PicPtr,PAL7);
}
void ShowRLCip1(int x,int y,void* PicPtr)
{
	ShowRLCipal(x,y,PicPtr,PAL1);
}
void ShowRLCip2(int x,int y,void* PicPtr)
{
	ShowRLCipal(x,y,PicPtr,PAL2);
}
void ShowRLCip3(int x,int y,void* PicPtr)
{
	ShowRLCipal(x,y,PicPtr,PAL3);
}
void ShowRLCip4(int x,int y,void* PicPtr)
{
	ShowRLCipal(x,y,PicPtr,PAL4);
}
void ShowRLCip5(int x,int y,void* PicPtr)
{
	ShowRLCipal(x,y,PicPtr,PAL5);
}
void ShowRLCip6(int x,int y,void* PicPtr)
{
	ShowRLCipal(x,y,PicPtr,PAL6);
}
void ShowRLCip7(int x,int y,void* PicPtr)
{
	ShowRLCipal(x,y,PicPtr,PAL7);
}
extern byte fog[8192+1024];
extern byte wfog[8192];
extern byte yfog[8192];
extern byte rfog[8192];
void ShowRLCShadow(int x,int y,void* PicPtr)
{
	ShowRLCfonpal(x,y,PicPtr,fog+1024+2048);
}
void ShowRLCiShadow(int x,int y,void* PicPtr)
{
	ShowRLCifonpal(x,y,PicPtr,fog+1024+2048);
}
void ShowRLCWhite(int x,int y,void* PicPtr)
{
	ShowRLCfonpal(x,y,PicPtr,wfog+1024);
}
void ShowRLCWFog(int x,int y,void* PicPtr)
{
	ShowRLChtpal(x,y,PicPtr,wfog);
}
void ShowRLCiWhite(int x,int y,void* PicPtr)
{
	ShowRLCifonpal(x,y,PicPtr,wfog+1024);
}
void ShowRLCiWFog(int x,int y,void* PicPtr)
{
	ShowRLCihtpal(x,y,PicPtr,wfog);
}
void ShowRLCFire(int x,int y,void* PicPtr)
{
	ShowRLCfonpal(x,y,PicPtr,yfog+4096);
}
void ShowRLCiFire(int x,int y,void* PicPtr)
{
	ShowRLCifonpal(x,y,PicPtr,yfog+4096);
}

//          Loading RLC file
bool LoadRLC(LPCSTR lpFileName,RLCTable *RLCtbl)
{
	ResFile f1=RReset(lpFileName);
	int xxx=IOresult();
	if (!xxx) 
	{
		DWORD fsz=RFileSize(f1);
		*RLCtbl=(RLCTable)malloc(fsz);
		RBlockRead(f1,*RLCtbl,fsz);
		//if (IOresult)
		{
			int shft=int(*RLCtbl);
			int cnt=((*RLCtbl)->SCount&65535);
			for(int i=0;i<cnt;i++) (*RLCtbl)->OfsTable[i]+=shft;
			return true;
		}
		free(*RLCtbl);
		return false;
	}
	return false;
} 
//       Showing RLC item
void ShowRLCItem(int x,int y,lpRLCTable lprt,int n,byte nt)
{
	cntr++;
	if(cntr>64&&!InCycle){
		InCycle=true;
		ProcessMessages();
		InCycle=false;
		cntr=0;
	};
	if(n<4096){
		switch(nt){
		case 1:
			ShowRLCp1(x,y,(void*)((*lprt)->OfsTable[n]));
			break;
		case 2:
			ShowRLCp2(x,y,(void*)((*lprt)->OfsTable[n]));
			break;
		case 3:
			ShowRLCp3(x,y,(void*)((*lprt)->OfsTable[n]));
			break;
		case 4:
			ShowRLCp4(x,y,(void*)((*lprt)->OfsTable[n]));
			break;
		case 5:
			ShowRLCp5(x,y,(void*)((*lprt)->OfsTable[n]));
			break;
		case 6:
			ShowRLCp6(x,y,(void*)((*lprt)->OfsTable[n]));
			break;
		case 7:
			ShowRLCp7(x,y,(void*)((*lprt)->OfsTable[n]));
			break;
		default:
			ShowRLC(x,y,(void*)((*lprt)->OfsTable[n]));
		};
	}else{
		switch(nt){
		case 1:
			ShowRLCip1(x,y,(void*)((*lprt)->OfsTable[n-4096]));
			break;
		case 2:
			ShowRLCip2(x,y,(void*)((*lprt)->OfsTable[n-4096]));
			break;
		case 3:
			ShowRLCip3(x,y,(void*)((*lprt)->OfsTable[n-4096]));
			break;
		case 4:
			ShowRLCip4(x,y,(void*)((*lprt)->OfsTable[n-4096]));
			break;
		case 5:
			ShowRLCip5(x,y,(void*)((*lprt)->OfsTable[n-4096]));
			break;
		case 6:
			ShowRLCip6(x,y,(void*)((*lprt)->OfsTable[n-4096]));
			break;
		case 7:
			ShowRLCip7(x,y,(void*)((*lprt)->OfsTable[n-4096]));
			break;
		default:
			ShowRLCi(x,y,(void*)((*lprt)->OfsTable[n-4096]));
		};
	};
};
void ShowRLCItemShadow(int x,int y,lpRLCTable lprt,int n)
{
	if(n<4096){
		ShowRLCShadow(x,y,(void*)((*lprt)->OfsTable[n]));
	}else{
		ShowRLCiShadow(x,y,(void*)((*lprt)->OfsTable[n-4096]));
	};
};
void ShowRLCItemMutno(int x,int y,lpRLCTable lprt,int n)
{
	if(n<4096){
		ShowRLCWFog(x,y,(void*)((*lprt)->OfsTable[n]));
	}else{
		ShowRLCiWFog(x,y,(void*)((*lprt)->OfsTable[n-4096]));
	};
};
void ShowRLCItemFired(int x,int y,lpRLCTable lprt,int n)
{
	if(n<4096){
		ShowRLCFire(x,y,(void*)((*lprt)->OfsTable[n]));
	}else{
		ShowRLCiFire(x,y,(void*)((*lprt)->OfsTable[n-4096]));
	};
};
int GetRLCWidth(RLCTable lpr,byte n)
{
	if (n<lpr->SCount) return (*((lpRLCHeader)((void*)(lpr->OfsTable[n])))).SizeX;
	else return 0;
}
int GetRLCHeight(RLCTable lpr,byte n)
{
	if (n<lpr->SCount) return (*((lpRLCHeader)((void*)(lpr->OfsTable[n])))).SizeY;
	else return 0;
}
void RegisterRLCFont(lpRLCFont lrf,RLCTable lpr,int fir)
{
	lrf->FirstSymbol=fir;
	lrf->LastSymbol=lpr->SCount+fir-1;
	lrf->RLC=lpr;
}
void ShowString(int x,int y,LPCSTR lps,lpRLCFont lpf)
{
	if (lps==NULL) return;
	byte	ch;
	int		i=0;
	do 
	{
		ch=lps[i];
		if(ch!=0) 
		{
			ShowRLCItem(x,y,&(lpf->RLC),lpf->FirstSymbol+ch,0);
			x+=GetRLCWidth(lpf->RLC,lpf->FirstSymbol+ch);
		}
		i++;
	}
    while(ch-0);
}
void ShowShadString(int x,int y,LPCSTR lps,lpRLCFont lpf)
{
	if (lps==NULL) return;
	byte	ch;
	int		i=0;
	do 
	{
		ch=lps[i];
		if(ch!=0) 
		{
			ShowRLCItemShadow(x+2,y+2,&(lpf->RLC),lpf->FirstSymbol+ch);
			ShowRLCItem(x,y,&(lpf->RLC),lpf->FirstSymbol+ch,0);
			x+=GetRLCWidth(lpf->RLC,lpf->FirstSymbol+ch);
		}
		i++;
	}
    while(ch-0);
}
void LoadPalettes(){
	ResFile f1=RReset("pal1.dat");
	RBlockRead(f1,PAL1,256);
	RClose(f1);
	f1=RReset("pal2.dat");
	RBlockRead(f1,PAL2,256);
	RClose(f1);
	f1=RReset("pal3.dat");
	RBlockRead(f1,PAL3,256);
	RClose(f1);
	f1=RReset("pal4.dat");
	RBlockRead(f1,PAL4,256);
	RClose(f1);
	f1=RReset("pal5.dat");
	RBlockRead(f1,PAL5,256);
	RClose(f1);
	f1=RReset("pal6.dat");
	RBlockRead(f1,PAL6,256);
	RClose(f1);
	f1=RReset("pal7.dat");
	RBlockRead(f1,PAL7,256);
	RClose(f1);
};
/* End of Graphics routine */