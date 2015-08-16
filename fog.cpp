#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mgraph.h"
#include "mouse.h"
#include "menu.h"
#include "MapDiscr.h"
#include "mode.h"
word fmap[256][256];
#define MaxShad 4528
byte fog[8192+1024];
byte wfog[8192];
byte yfog[8192];
byte rfog[8192];
byte darkfog[40960];
int FogMode;
void ShowSuperFog();
void ProcessFog(){
	byte z=0;//byte(rando()&1);
	__asm{
		mov		ecx,msx
		inc		ecx
lab1:	mov		ax,[fmap+ecx*2]
		mov		[fmap+512+ecx*2],ax;
		mov		ax,[fmap+256*255*2+ecx*2]
		mov		[fmap+256*254*2+ecx*2],ax
		loop	lab1
		mov		ecx,msy
		mov		ebx,offset fmap+512
lab2:	mov		ax,[ebx]
		mov		[ebx+2],ax
		mov		ax,[ebx+510]
		mov		[ebx+508],ax
		add		ebx,512
		loop	lab2
		cmp		z,0
		jne		Process1
		//Process0
		mov		bl,byte ptr msx
		mov		bh,byte ptr msy
		mov		ecx,offset fmap
		add		ecx,514
lab3:	mov		dx,[ecx-512]
		add		dx,[ecx+512]
		add		dx,[ecx-2]
		add		dx,[ecx+2]
		mov		ax,dx
		//shr		dx,8
		//sub		ax,dx
		shr		ax,2
		//cmp		ax,MaxShad
		//ja		iii
		or		ax,63
		sub		ax,63
iii:
		mov		[ecx],ax
		add		ecx,2
		dec		bl
		jnz		lab3
		add		ecx,512
		sub		ecx,msx
		sub		ecx,msx
		mov		bl,byte ptr msx
		dec		bh
		jnz		lab3
		jmp		ProcessFinished
Process1:
		xor		ebx,ebx
		mov		bl,byte ptr msx
		mov		bh,byte ptr msy
		mov		ecx,offset fmap
		add		ecx,ebx
		add		ecx,ebx
		//add		ecx,514
lab4:	mov		dx,[ecx-512]
		add		dx,[ecx+512]
		add		dx,[ecx-2]
		add		dx,[ecx+2]
		mov		ax,dx
		//shr		dx,8
		//sub		ax,dx
		shr		ax,2
		//cmp		ax,MaxShad
		//ja		ioo
		or		ax,63
		sub		ax,63
ioo:
		mov		[ecx],ax
		sub		ecx,2
		dec		bl
		jnz		lab4
		sub		ecx,512
		add		ecx,msx
		add		ecx,msx
		mov		bl,byte ptr msx
		dec		bh
		jnz		lab3
ProcessFinished:
	};
};
void LoadFog(){
	ResFile fx=RReset("darkb.grd");//004g.grd");
	RBlockRead(fx,&fog[1024],8192);
	memcpy(&fog[0],&fog[1024],256);
	memcpy(&fog[256],&fog[1024],256);
	memcpy(&fog[512],&fog[1024],256);
	memcpy(&fog[512+256],&fog[1024],256);
	RClose(fx);
	//fx=RReset("003xw.grd");
	fx=RReset("darkw.grd");//003cm.grd");
	RBlockRead(fx,&wfog,8192);
	RClose(fx);
	fx=RReset("003xr.grd");
	RBlockRead(fx,&yfog,8192);
	RClose(fx);
	fx=RReset("003xr1.grd");
	RBlockRead(fx,&rfog,8192);
	RClose(fx);
	fx=RReset("darkfog.grd");
	RBlockRead(fx,&darkfog,40960);
	RClose(fx);
};
#define Shifter 7

void ShowFoggedBattle2(){
	byte	sx=smaplx;
	byte	sy=4;
	int		StOff=smapy*SCRSizeX+smapx;
	int		Addv1=(SCRSizeX<<1)-64;
	int		Addv2=(SCRSizeX<<6);
	int		Addv3=-(smaplx<<5)+(SCRSizeX<<6)-32;
	word	MinShad=(MaxShad-(32<<Shifter));
	word	tmp;
	int		adds=int(ScreenPtr);
	int		addr=int(ScreenPtr);
	int		fad=int(fog)+1024;
	int		ADDX=1;
	int		ADDY=SCRSizeX;
	__asm{
		xor	ebx,ebx
		cld
		mov		al,byte ptr mapx
		mov		ah,byte ptr mapy
		mov		esi,StOff
		mov		edi,esi
		add		esi,adds
		add		edi,addr
lab0:	add		esi,ADDX
		add		edi,ADDX
		neg     ADDX
		mov		tmp,ax
		mov		bx,ax
		shl		ebx,1
		mov		bx,[fmap+ebx]
		and     ebx,0xFFFF
		cmp		bx,MaxShad
		jae		FullColor
		cmp		bx,MinShad
		jbe		AbsBlack
		sub		bx,MaxShad
		neg		bx
		shr		bx,Shifter
		mov		bh,bl
		xor		bl,bl
		//Shadowed image
		add		ebx,fad
		mov		cl,32
		mov		ch,32
		xor		eax,eax
SI_1:	xor		eax,eax
		lodsb
		inc     esi
		add		eax,ebx
		mov		al,[eax]
		stosb
		inc     edi
		dec		cl
		jnz		SI_1
		add		esi,Addv1
		add		edi,Addv1
		mov		cl,32
		dec		ch
		jnz		SI_1
		jmp		Next_slide
FullColor:
		add		esi,32
		add		edi,32
		sub		esi,Addv2
		sub		edi,Addv2
		xor		ecx,ecx
/*		mov		cx,32
FC_1:	movsd
		movsd
		movsd
		movsd
		movsd
		movsd
		movsd
		movsd
		add		esi,Addv1
		add		edi,Addv1
		loop	FC_1
*/
		jmp	Next_slide
AbsBlack:
		xor		ecx,ecx
		mov		cx,32
		xor		eax,eax
AB_1:	stosd
		stosd
		stosd
		stosd
		stosd
		stosd
		stosd
		stosd
		add		edi,Addv1
		add		esi,SCRSizeX
		loop	AB_1
Next_slide:
		add		esi,Addv2
		add		edi,Addv2
		mov		ax,tmp
		inc		al
		dec		sx
		jnz		lab0
		add		edi,Addv3
		add		esi,Addv3
		add		esi,ADDY
		add		edi,ADDY
		neg		ADDY
		mov		al,byte ptr smaplx
		mov		sx,al
		mov		ax,tmp
		mov		al,byte ptr mapx
		inc		ah
		dec		sy
		jnz		lab0
	};
};
static byte BFog[64][64];
void SetScreenFog16x16(){
	smaplx++;
	smaply++;
	int fofs=int(fmap)+((((mapy-1)<<8)+(mapx-1))<<1);
	int Saddy=(256-smaplx)<<1;
	int Daddy=(64-smaplx)<<1;
	word MinShad=(MaxShad-(32<<Shifter));
	//filling
	__asm{
		push	esi
		push	edi
		mov		cl,byte ptr smaplx
		mov		ch,byte ptr smaply
		mov		esi,fofs
		mov		edi,offset BFog+64+2
		cld
LP1:	lodsw	
		cmp		ax,MaxShad
		jae		FullColor
		cmp		ax,MinShad
		jbe		AbsBlack
		sub		ax,MaxShad
		neg		ax
		shr		ax,Shifter
		jmp		asgn1
AbsBlack:
		mov		ax,32
		jmp		asgn1
FullColor:
		xor		ax,ax
asgn1:	mov		ah,al
		mov		[edi],ax
		mov		[edi+64],ax
		add		edi,2
		dec		cl
		jnz		LP1
		mov		cl,byte ptr smaplx
		add		esi,Saddy
		add		edi,Daddy
		dec		ch
		jnz		LP1
		//set colors at the edges
		/*mov		esi,offset BFog+64+2
		mov		edi,offset BFog+2
		mov		ecx,smaplx
		rep		movsw
		mov		esi,smaply
		shl		esi,6
		add		esi,2+offset BFog
		mov		edi,esi
		add		edi,64
		mov		ecx,smaplx
		rep		movsw
		mov		esi,offset BFog+64
		mov		edi,offset BFog+65
		mov		ecx,smaply
		shl		ecx,1
cyc1:	movsb
		add		esi,63
		add		edi,63
		loop	cyc1
		mov		esi,offset BFog+64+2
		add		esi,smaplx
		add		esi,smaplx
		mov		edi,esi
		inc		edi
		mov		ecx,smaply
		shl		ecx,1
cyc2:	movsb
		add		esi,63
		add		edi,63
		loop	cyc2*/
		pop		edi
		pop		esi
	};
	smaplx--;
	smaply--;
};
void ProcessScreenFog16x16(){
	smaplx++;
	smaply++;
	int	ads=64-(smaplx<<1);
	__asm{
		push	esi
		mov		esi,offset BFog+64+2
		mov		ch,byte ptr smaply
		shl		ch,1
gt0:	mov		cl,byte ptr smaplx
gt1:	mov		ax,[esi-1]
		add		ax,[esi+1]
		add		ax,[esi-64]
		add		ax,[esi+64]
		shr		ax,2
		and		ax,0x1F1F
		mov		[esi],ax
		add		esi,2
		dec		cl
		jnz		gt1
		add		esi,ads
		dec		ch
		jnz		gt0
		pop		esi
	};
	smaplx--;
	smaply--;
};
void ShowScreenFog16x16(){
	SetScreenFog16x16();
	ProcessScreenFog16x16();
	smaplx++;
	smaply++;
	byte    ssx=(smaplx)<<1;
	byte	sx=ssx;
	byte	sy=(smaply)<<1;
	int		StOff=smapy*SCRSizeX+smapx;
	int		Addv1=SCRSizeX-16;
	int		Addv2=16-SCRSizeX*16;
	int		Addv3=-ssx*16+SCRSizeX*16;
	int     Addv4=(SCRSizeX<<4);
	word	MinShad=(MaxShad-(32<<Shifter));
	word	tmp;
	int		adds=int(ScreenPtr)+(smapx-16)+(smapy-16)*SCRSizeX;
	int		addf=64-((smaplx)<<1);
	int		fad=int(BFog)+64+2;
	int		uuu;
	__asm{
		push	esi
		push	edi
		mov		esi,fad
		mov		edi,adds
		cld

sfg1:	xor		ebx,ebx
		mov		bh,[esi]
		inc		esi
		or		bh,bh
		jz		EmptyBar
		cmp		bh,28
		jae		BlackBar
		add		ebx,offset fog+1024
		mov		ecx,16
		xor		eax,eax
		mov		edx,16
		mov		uuu,0
sfg2:	mov		al,[edi]
		mov		al,[ebx+eax]
		mov		[edi],al
		inc		edi
		loop	sfg2
		add		edi,Addv1
		xor		uuu,1
		jz		uuu_L1
		add		edi,6
		jmp		uuu_L2
uuu_L1: sub		edi,6
uuu_L2:	mov		ecx,16
		dec		edx
		jnz		sfg2
		//sub		edi,4
		jmp		NextStep
BlackBar:
		xor		eax,eax
		mov		ecx,16
		mov		edx,12
sfg3:	stosd
		stosd
		stosd
		stosd
		add		edi,Addv1
		add		edi,edx
		sub		edi,6
		xor		edx,12
		loop	sfg3
		jmp		NextStep
EmptyBar:
		add		edi,16
		jmp		sfg4
NextStep:
		add		edi,Addv2
sfg4:	dec		sx
		jnz		sfg1
		mov		al,ssx
		mov		sx,al
		add		edi,Addv3
		add		esi,addf
		dec		sy
		jnz		sfg1
		pop		edi
		pop		esi
	};
	smaplx--;
	smaply--;
};
void ShowScreenFog16x16_Best(){
	SetScreenFog16x16();
	ProcessScreenFog16x16();
	byte    ssx=smaplx<<1;
	byte	sx=ssx;
	byte	sy=smaply<<1;
	int		StOff=(smapy-16)*SCRSizeX+smapx-16;
	int		Addv1=SCRSizeX-16;
	int		Addv2=16-SCRSizeX*16;
	int		Addv3=-ssx*16+SCRSizeX*16;
	int     Addv4=(SCRSizeX<<4);
	word	MinShad=(MaxShad-(32<<Shifter));
	word	tmp;
	int		adds=int(ScreenPtr)+(smapx-16)+(smapy-16)*SCRSizeX;
	int		addf=64-(smaplx<<1);
	int		fad=int(BFog)+64+2;
	int		uuu,zzz;
	__asm{
		push	esi
		push	edi
		mov		esi,fad
		mov		edi,adds
		cld

sfg1:	xor		ebx,ebx
		mov		bh,[esi]
		inc		esi
		or		bh,bh
		jz		EmptyBar
		cmp		bh,23
		jae		BlackBar
		add		ebx,offset fog
		mov		ecx,16
		xor		eax,eax
		mov		edx,16
		mov		uuu,0
		//mov		zzz,ebp
		mov		zzz,1024*10
sfg2:	mov		al,[edi]
		mov		al,[ebx+eax]
		mov		[edi],al
		inc		edi
		add		edi,zzz
		sub		edi,1024*5
		xor		zzz,1024*10
		loop	sfg2
		add		edi,Addv1
		xor		uuu,1
		jz		uuu_L1
		add		edi,5
		jmp		uuu_L2
uuu_L1: sub		edi,5
uuu_L2:	mov		ecx,16
		dec		edx
		jnz		sfg2
		//mov		ebp,zzz
		//sub		edi,4
		jmp		NextStep
BlackBar:
		xor		eax,eax
		mov		ecx,16
		mov		edx,12
sfg3:	stosd
		stosd
		stosd
		stosd
		add		edi,Addv1
		add		edi,edx
		sub		edi,6
		xor		edx,12
		loop	sfg3
		cmp		byte ptr [esi-65],23
		jae		NextStep
		xor		eax,eax
		mov		ecx,6
		mov		edx,12
sfg3_1:	stosd
		stosd
		stosd
		stosd
		add		edi,Addv1
		add		edi,edx
		sub		edi,6
		xor		edx,12
		loop	sfg3_1
		sub		edi,6*1024
		add		edi,16
		jmp		NextStep
EmptyBar:
		add		edi,16
		jmp		sfg4
NextStep:
		add		edi,Addv2
sfg4:	dec		sx
		jnz		sfg1
		mov		al,ssx
		mov		sx,al
		add		edi,Addv3
		add		esi,addf
		dec		sy
		jnz		sfg1
		pop		edi
		pop		esi
	};
};
void ShowFoggedBattle(){
	ShowSuperFog();
	return;
	ShowScreenFog16x16();
	return;
	byte	sx=smaplx;
	byte	sy=smaply;
	int		StOff=smapy*SCRSizeX+smapx;
	int		Addv1=SCRSizeX-32;
	int		Addv2=32-SCRSizeX*32;
	int		Addv3=-smaplx*32+SCRSizeX*32;
	word	MinShad=(MaxShad-(32<<Shifter));
	word	tmp;
	int		adds=int(ScreenPtr);
	int		addr=int(ScreenPtr);
	int		fad=int(fog);
	__asm{
		cld
		mov		al,byte ptr mapx
		mov		ah,byte ptr mapy
		mov		esi,StOff
		mov		edi,esi
		add		esi,adds
		add		edi,addr
lab0:	xor		ebx,ebx
		mov		tmp,ax
		mov		bx,ax
		shl		ebx,1
		mov		bx,[fmap+ebx]
		and     ebx,0xFFFF
		cmp		bx,MaxShad
		jae		FullColor
		cmp		bx,MinShad
		jbe		AbsBlack
		sub		bx,MaxShad
		neg		bx
		shr		bx,Shifter
		mov		bh,bl
		xor		bl,bl
		//Shadowed image
		add		ebx,fad
		mov		cl,32
		mov		ch,32
		xor		eax,eax
SI_1:	xor		eax,eax
		lodsb
		add		eax,ebx
		mov		al,[eax]
		stosb
		dec		cl
		jnz		SI_1
		add		esi,Addv1
		add		edi,Addv1
		mov		cl,32
		dec		ch
		jnz		SI_1
		jmp		Next_slide
FullColor:
		add		esi,32
		add		edi,32
		sub		esi,Addv2
		sub		edi,Addv2
		xor		ecx,ecx
/*		mov		cx,32
FC_1:	movsd
		movsd
		movsd
		movsd
		movsd
		movsd
		movsd
		movsd
		add		esi,Addv1
		add		edi,Addv1
		loop	FC_1
*/
		jmp	Next_slide
AbsBlack:
		xor		ecx,ecx
		mov		cx,32
		xor		eax,eax
AB_1:	stosd
		stosd
		stosd
		stosd
		stosd
		stosd
		stosd
		stosd
		add		edi,Addv1
		add		esi,SCRSizeX
		loop	AB_1
Next_slide:
		add		esi,Addv2
		add		edi,Addv2
		mov		ax,tmp
		inc		al
		dec		sx
		jnz		lab0
		add		edi,Addv3
		add		esi,Addv3
		mov		al,byte ptr smaplx
		mov		sx,al
		mov		ax,tmp
		mov		al,byte ptr mapx
		inc		ah
		dec		sy
		jnz		lab0
	};
};
void ShowFoggedBattle1(){
	byte	sx=smaplx;
	byte	sy=smaply;
	int		StOff=smapy*SCRSizeX+smapx;
	int		Addv1=SCRSizeX-32;
	int		Addv2=32-SCRSizeX*32;
	int		Addv3=-smaplx*32+SCRSizeX*32;
	word	MinShad=(MaxShad-(32<<Shifter));
	word	tmp;
	int		adds=int(ScreenPtr);
	int		addr=int(ScreenPtr);
	int		fad=int(fog);
	__asm{
		cld
		mov		al,byte ptr mapx
		mov		ah,byte ptr mapy
		mov		esi,StOff
		mov		edi,esi
		add		esi,adds
		add		edi,addr
lab0:	xor		ebx,ebx
		mov		tmp,ax
		mov		bx,ax
		shl		ebx,1
		mov		bx,[fmap+ebx]
		cmp		bx,MaxShad
		jae		FullColor
		cmp		bx,MinShad
		jbe		AbsBlack
		sub		bx,MaxShad
		neg		bx
		shr		bx,Shifter
		mov		bh,bl
		xor		bl,bl
		//Shadowed image
		add		ebx,fad
		mov		cl,32
		mov		ch,32
		xor		eax,eax
SI_1:	xor		eax,eax
		lodsb
		add		eax,ebx
		mov		al,[eax]
		stosb
		dec		cl
		jnz		SI_1
		add		esi,Addv1
		add		edi,Addv1
		mov		cl,32
		dec		ch
		jnz		SI_1
		jmp		Next_slide
FullColor:
		add		esi,32
		add		edi,32
		sub		esi,Addv2
		sub		edi,Addv2
		xor		ecx,ecx
/*		mov		cx,32
FC_1:	movsd
		movsd
		movsd
		movsd
		movsd
		movsd
		movsd
		movsd
		add		esi,Addv1
		add		edi,Addv1
		loop	FC_1
*/
		jmp	Next_slide
AbsBlack:
		xor		ecx,ecx
		mov		cx,32
		xor		eax,eax
AB_1:	stosd
		stosd
		stosd
		stosd
		stosd
		stosd
		stosd
		stosd
		add		edi,Addv1
		add		esi,SCRSizeX
		loop	AB_1
Next_slide:
		add		esi,Addv2
		add		edi,Addv2
		mov		ax,tmp
		inc		al
		dec		sx
		jnz		lab0
		add		edi,Addv3
		add		esi,Addv3
		mov		al,byte ptr smaplx
		mov		sx,al
		mov		ax,tmp
		mov		al,byte ptr mapx
		inc		ah
		dec		sy
		jnz		lab0
	};
};
void AcceptMiniMap1(){
	word	MinShad=(MaxShad-(32<<Shifter));
	int ofs=minix+miniy*SCRSizeX;
	int lx=msx;
	int ly=msy;
	int addOf=SCRSizeX-lx;
	__asm{
		push	esi
		push	edi
		mov		edi,ScreenPtr
		add		edi,ofs
		mov		ecx,0101h
		cld
		mov		eax,lx
yyy:	mov		edx,ecx
		shl		edx,1
		cmp		word ptr[edx+fmap],512
		ja		uuu
		mov		byte ptr [edi],0
uuu:	inc		edi
		inc		cl
		dec		eax
		jnz		yyy
		mov		eax,lx
		add		edi,addOf
		inc		ch
		mov		cl,1
		dec		ly
		jnz		yyy
	};
};
void AcceptMiniMap(){
	word	MinShad=(MaxShad-(32<<Shifter));
	int ofs=minix+miniy*SCRSizeX;
	int lx=msx>>1;
	int ly=msy>>1;
	int addOf=SCRSizeX-lx;
	__asm{
		push	esi
		push	edi
		mov		edi,ScreenPtr
		add		edi,ofs
		mov		ecx,0101h
		cld
		mov		eax,lx
yyy:	mov		edx,ecx
		shl		edx,1
		cmp		word ptr[edx+fmap],512
		ja		uuu
		mov		byte ptr [edi],0
uuu:	inc		edi
		add		cl,2
		dec		eax
		jnz		yyy
		mov		eax,lx
		add		edi,addOf
		add     ch,2
		mov		cl,1
		dec		ly
		jnz		yyy
	};
};
#define zmin 0
#define zmax 32
void ShowFluentFog(int x,int y,int z1,int z2,int z3,int z4){
	//return;
	int scrof=int(ScreenPtr)+x+y*SCRSizeX;
	int adds=SCRSizeX-32;
	if(z1<=zmin&&z2<=zmin&&z3<=zmin&&z4<=zmin)return;
	if(z1>=zmax&&z2>=zmax&&z3>=zmax&&z4>=zmax){
		__asm{
			push	edi
			mov		ebx,adds
			mov		edi,scrof
			mov		dl,32
			xor		eax,eax
			cld
iug:		mov		ecx,8
			rep		stosd
			add		edi,ebx
			dec		dl
			jnz		iug
			pop		edi
		};
		return;
	}else{
		int a,b,c,d,p,q,r;
		__asm{
			push	edi
			push	esi
			mov		eax,z1
			mov		ebx,eax
			//shl		ebx,8
			mov		a,ebx
			mov		ecx,z2
			sub		ecx,eax
			shr		ecx,5
			mov		b,ecx
			mov		ecx,z3
			sub		ecx,eax
			shr		ecx,5
			mov		c,ecx
			mov		ecx,z4
			add		ecx,z1
			sub		ecx,z2
			sub		ecx,z3
			shr		ecx,10
			mov		d,ecx
			//coefficients are now calculated
			mov		ecx,0x2020
			mov		bx,word ptr a		//bh=fogging value
			add		bx,256*4
			mov		dx,word ptr b		//dx=fog incrementor
			xor		eax,eax
			mov		esi,scrof
			mov		word ptr p,bx
qqw1:		mov		ax,bx
			mov		al,[esi]
			mov		al,[fog+eax]
			mov		[esi],al
			inc		esi
			add		bx,dx
			dec		cl
			jnz		qqw1
			add		esi,adds
			add		dx,word ptr d
			mov		bx,word ptr p
			add		bx,word ptr c
			mov		cl,32
			mov		word ptr p,bx
			dec		ch
			jnz		qqw1
			pop		esi
			pop		edi
		};
	};
};
void ShowSuperFluentFog(int x,int y,int z1x,int z2x,int z3x,int z4x){
	//return;
	int z1=z1x<<16;
	int	z2=z2x<<16;
	int z3=z3x<<16;
	int	z4=z4x<<16;
	int scrof=int(ScreenPtr)+x+y*SCRSizeX;
	int adds=SCRSizeX-32;
	if(z1x<=zmin&&z2x<=zmin&&z3x<=zmin&&z4x<=zmin)return;
	if(z1x>=zmax&&z2x>=zmax&&z3x>=zmax&&z4x>=zmax){
		__asm{
			push	edi
			mov		ebx,adds
			mov		edi,scrof
			mov		dl,32
			xor		eax,eax
			cld
iug:		mov		ecx,8
			rep		stosd
			add		edi,ebx
			dec		dl
			jnz		iug
			pop		edi
		};
		return;
	}else{
		int a,b,c,d,p,q,r;
		__asm{
			push	edi
			push	esi
			mov		eax,z1
			mov		ebx,eax
			//shl		ebx,8
			mov		a,ebx
			mov		ecx,z2
			sub		ecx,eax
			sar		ecx,5
			mov		b,ecx
			mov		ecx,z3
			sub		ecx,eax
			sar		ecx,5
			mov		c,ecx
			mov		ecx,z4
			add		ecx,z1
			sub		ecx,z2
			sub		ecx,z3
			sar		ecx,10
			mov		d,ecx
			//coefficients are now calculated
			mov		ecx,0x2020
			mov		ebx,a		//bh=fogging value
			add		ebx,256*256*3
			mov		edx,b		//dx=fog incrementor
			xor		eax,eax
			mov		esi,scrof
			mov		p,ebx
qqw1:		mov		eax,ebx
			shr		eax,8
			mov		al,[esi]
			mov		al,[fog+eax]
			mov		[esi],al
			inc		esi
			add		ebx,edx
			dec		cl
			jnz		qqw1
			add		esi,adds
			add		edx,d
			mov		ebx,p
			add		ebx,c
			mov		cl,32
			mov		p,ebx
			dec		ch
			jnz		qqw1
			pop		esi
			pop		edi
		};
		//Xbar(x+12,y+12,8,8,clrGreen);
	};
};
void ShowSuperFluentFog2(int x,int y,int z1x,int z2x,int z3x,int z4x){
	//return;
	int z1=z1x<<16;
	int	z2=z2x<<16;
	int z3=z3x<<16;
	int	z4=z4x<<16;
	int scrof=int(ScreenPtr)+x+y*SCRSizeX;
	int adds=SCRSizeX-32;
	if(z1x<=zmin&&z2x<=zmin&&z3x<=zmin&&z4x<=zmin)return;
	if(z1x>=zmax&&z2x>=zmax&&z3x>=zmax&&z4x>=zmax){
		__asm{
			push	edi
			mov		ebx,adds
			mov		edi,scrof
			mov		dl,32
			xor		eax,eax
			cld
iug:		mov		ecx,8
			rep		stosd
			add		edi,ebx
			dec		dl
			jnz		iug
			pop		edi
		};
		return;
	}else{
		int a,b,c,d,p,q,r;
		__asm{
			push	edi
			push	esi
			mov		eax,z1
			mov		ebx,eax
			//shl		ebx,8
			mov		a,ebx
			mov		ecx,z2
			sub		ecx,eax
			sar		ecx,5
			mov		b,ecx
			mov		ecx,z3
			sub		ecx,eax
			sar		ecx,5
			mov		c,ecx
			mov		ecx,z4
			add		ecx,z1
			sub		ecx,z2
			sub		ecx,z3
			sar		ecx,10
			mov		d,ecx
			//coefficients are now calculated
			mov		ecx,0x2020
			mov		ebx,a		//bh=fogging value
			add		ebx,256*256*3
			mov		edx,b		//dx=fog incrementor
			xor		eax,eax
			mov		esi,scrof
			mov		p,ebx
qqw1:		mov		eax,ebx
			shr		eax,8
			mov		al,[esi]
			mov		al,[fog+eax]
			mov		[esi],al
			inc		esi
			add		ebx,edx
			dec		cl
			jnz		qqw1
			add		esi,adds
			add		edx,d
			mov		ebx,p
			add		ebx,c
			mov		cl,32
			mov		p,ebx
			dec		ch
			jnz		qqw1
			pop		esi
			pop		edi
		};
		//Xbar(x+12,y+12,8,8,clrGreen);
	};
};
void ShowSuperFluentFog8(int x,int y,int z1x,int z2x,int z3x,int z4x){
	//return;
	int z1=z1x<<16;
	int	z2=z2x<<16;
	int z3=z3x<<16;
	int	z4=z4x<<16;
	int scrof=int(ScreenPtr)+x+y*SCRSizeX;
	int adds=SCRSizeX-32;
	if(z1x<=zmin&&z2x<=zmin&&z3x<=zmin&&z4x<=zmin)return;
	if(z1x>=zmax&&z2x>=zmax&&z3x>=zmax&&z4x>=zmax){
		__asm{
			push	edi
			mov		ebx,adds
			mov		edi,scrof
			mov		dl,32
			xor		eax,eax
			cld
iug:		mov		ecx,8
			rep		stosd
			add		edi,ebx
			dec		dl
			jnz		iug
			pop		edi
		};
		return;
	}else{
		int a,b,c,d,p,q,r;
		__asm{
			push	edi
			push	esi
			mov		eax,z1
			mov		ebx,eax
			//shl		ebx,8
			mov		a,ebx
			mov		ecx,z2
			sub		ecx,eax
			sar		ecx,5
			mov		b,ecx
			mov		ecx,z3
			sub		ecx,eax
			sar		ecx,5
			mov		c,ecx
			mov		ecx,z4
			add		ecx,z1
			sub		ecx,z2
			sub		ecx,z3
			sar		ecx,10
			mov		d,ecx
			//coefficients are now calculated
			mov		ecx,0x2004
			mov		ebx,a		//bh=fogging value
			add		ebx,256*256*3
			mov		edx,b		//dx=fog incrementor
			xor		eax,eax
			mov		esi,scrof
			mov		p,ebx
qqw1:		mov		eax,ebx
			shr		eax,8
			mov		al,[esi]
			mov		al,[fog+eax]
			mov		[esi],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+1]
			mov		al,[fog+eax]
			mov		[esi+1],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+2]
			mov		al,[fog+eax]
			mov		[esi+2],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+3]
			mov		al,[fog+eax]
			mov		[esi+3],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+4]
			mov		al,[fog+eax]
			mov		[esi+4],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+5]
			mov		al,[fog+eax]
			mov		[esi+5],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+6]
			mov		al,[fog+eax]
			mov		[esi+6],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+7]
			mov		al,[fog+eax]
			mov		[esi+7],al
			add		esi,8
			add		ebx,edx
			dec		cl
			jnz		qqw1
			add		esi,adds
			add		edx,d
			mov		ebx,p
			add		ebx,c
			mov		cl,4
			mov		p,ebx
			dec		ch
			jnz		qqw1
			pop		esi
			pop		edi
		};
		//Xbar(x+12,y+12,8,8,clrGreen);
	};
};
void ShowSuperFluentFog32w(int x,int y,int z1x,int z2x,int z3x,int z4x){
	//return;
	int z1=z1x<<16;
	int	z2=z2x<<16;
	int z3=z3x<<16;
	int	z4=z4x<<16;
	int scrof=int(ScreenPtr)+x+y*SCRSizeX;
	int adds=SCRSizeX-32;
	if(z1x<=zmin&&z2x<=zmin&&z3x<=zmin&&z4x<=zmin)return;
	if(z1x>=zmax&&z2x>=zmax&&z3x>=zmax&&z4x>=zmax){
		__asm{
			push	edi
			mov		ebx,adds
			mov		edi,scrof
			mov		dl,32
			xor		eax,eax
			cld
iug:		mov		ecx,8
			rep		stosd
			add		edi,ebx
			dec		dl
			jnz		iug
			pop		edi
		};
		return;
	}else{
		int a,b,c,d,p,q,r;
		int cc=32;
		__asm{
			push	edi
			push	esi
			mov		eax,z1
			mov		ebx,eax
			//shl		ebx,8
			mov		a,ebx
			mov		ecx,z2
			sub		ecx,eax
			sar		ecx,5
			mov		b,ecx
			mov		ecx,z3
			sub		ecx,eax
			sar		ecx,5
			mov		c,ecx
			mov		ecx,z4
			add		ecx,z1
			sub		ecx,z2
			sub		ecx,z3
			sar		ecx,10
			mov		d,ecx
			//coefficients are now calculated
			mov		ecx,0x2004
			mov		ebx,a		//bh=fogging value
			add		ebx,256*256*3
			mov		edx,b		//dx=fog incrementor
			xor		eax,eax
			mov		esi,scrof
			mov		p,ebx
qqw1:		mov		eax,ebx
			shr		eax,8
			mov		al,[esi]
			mov		cl,[fog+eax]
			//mov		cl,al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+1]
			mov		ch,[fog+eax]
			mov		[esi],cx
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+2]
			mov		cl,[fog+eax]
			//mov		[esi+2],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+3]
			mov		ch,[fog+eax]
			mov		[esi+2],cx
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+4]
			mov		cl,[fog+eax]
			//mov		[esi+4],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+5]
			mov		ch,[fog+eax]
			mov		[esi+4],cx
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+6]
			mov		cl,[fog+eax]
			//mov		[esi+6],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+7]
			mov		ch,[fog+eax]
			mov		[esi+6],al
			//add		esi,8
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+8]
			mov		cl,[fog+eax]
			//mov		[esi],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+9]
			mov		ch,[fog+eax]
			mov		[esi+8],cx
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+10]
			mov		cl,[fog+eax]
			//mov		[esi+2],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+11]
			mov		ch,[fog+eax]
			mov		[esi+10],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+12]
			mov		cl,[fog+eax]
			//mov		[esi+4],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+13]
			mov		ch,[fog+eax]
			mov		[esi+12],cx
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+14]
			mov		cl,[fog+eax]
			//mov		[esi+6],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+15]
			mov		ch,[fog+eax]
			mov		[esi+14],al
			add		esi,8
			add		ebx,edx
			mov		eax,ebx
			//shr		eax,8
			mov		al,[esi+16]
			mov		cl,[fog+eax]
			//mov		[esi],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+17]
			mov		ch,[fog+eax]
			mov		[esi+16],cx
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+18]
			mov		cl,[fog+eax]
			//mov		[esi+2],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+19]
			mov		ch,[fog+eax]
			mov		[esi+18],cx
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+20]
			mov		cl,[fog+eax]
			//mov		[esi+4],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+21]
			mov		ch,[fog+eax]
			mov		[esi+20],cx
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+22]
			mov		cl,[fog+eax]
			//mov		[esi+6],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+23]
			mov		ch,[fog+eax]
			mov		[esi+22],cx
			add		esi,8
			add		ebx,edx
			mov		eax,ebx
			//shr		eax,8
			mov		al,[esi+24]
			mov		cl,[fog+eax]
			//mov		[esi],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+25]
			mov		ch,[fog+eax]
			mov		[esi+24],cx
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+26]
			mov		cl,[fog+eax]
			//mov		[esi+2],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+27]
			mov		ch,[fog+eax]
			mov		[esi+26],cx
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+28]
			mov		cl,[fog+eax]
			//mov		[esi+4],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+29]
			mov		ch,[fog+eax]
			mov		[esi+28],cx
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+30]
			mov		cl,[fog+eax]
			//mov		[esi+36],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+31]
			mov		ch,[fog+eax]
			mov		[esi+30],cx
			add		esi,32
			add		ebx,edx
			//jnz		qqw1
			add		esi,adds
			add		edx,d
			mov		ebx,p
			add		ebx,c
			//mov		cl,4
			mov		p,ebx
			dec		cc
			jnz		qqw1
			pop		esi
			pop		edi
		};
		//Xbar(x+12,y+12,8,8,clrGreen);
	};
};
void ShowSuperFluentFog32(int x,int y,int z1x,int z2x,int z3x,int z4x){
	//return;
	int z1=z1x<<16;
	int	z2=z2x<<16;
	int z3=z3x<<16;
	int	z4=z4x<<16;
	int scrof=int(ScreenPtr)+x+y*SCRSizeX;
	int adds=SCRSizeX-32;
	if(z1x<=zmin&&z2x<=zmin&&z3x<=zmin&&z4x<=zmin)return;
	if(z1x>=zmax&&z2x>=zmax&&z3x>=zmax&&z4x>=zmax){
		__asm{
			push	edi
			mov		ebx,adds
			mov		edi,scrof
			mov		dl,32
			xor		eax,eax
			cld
iug:		mov		ecx,8
			rep		stosd
			add		edi,ebx
			dec		dl
			jnz		iug
			pop		edi
		};
		return;
	}else{
		int a,b,c,d,p,q,r;
		__asm{
			push	edi
			push	esi
			mov		eax,z1
			mov		ebx,eax
			//shl		ebx,8
			mov		a,ebx
			mov		ecx,z2
			sub		ecx,eax
			sar		ecx,5
			mov		b,ecx
			mov		ecx,z3
			sub		ecx,eax
			sar		ecx,5
			mov		c,ecx
			mov		ecx,z4
			add		ecx,z1
			sub		ecx,z2
			sub		ecx,z3
			sar		ecx,10
			mov		d,ecx
			//coefficients are now calculated
			mov		ecx,0x2004
			mov		ebx,a		//bh=fogging value
			add		ebx,256*256*3
			mov		edx,b		//dx=fog incrementor
			xor		eax,eax
			mov		esi,scrof
			mov		p,ebx
qqw1:		mov		eax,ebx
			shr		eax,8
			mov		al,[esi]
			mov		al,[fog+eax]
			mov		[esi],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+1]
			mov		al,[fog+eax]
			mov		[esi+1],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+2]
			mov		al,[fog+eax]
			mov		[esi+2],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+3]
			mov		al,[fog+eax]
			mov		[esi+3],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+4]
			mov		al,[fog+eax]
			mov		[esi+4],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+5]
			mov		al,[fog+eax]
			mov		[esi+5],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+6]
			mov		al,[fog+eax]
			mov		[esi+6],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+7]
			mov		al,[fog+eax]
			mov		[esi+7],al
			add		esi,8
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi]
			mov		al,[fog+eax]
			mov		[esi],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+1]
			mov		al,[fog+eax]
			mov		[esi+1],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+2]
			mov		al,[fog+eax]
			mov		[esi+2],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+3]
			mov		al,[fog+eax]
			mov		[esi+3],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+4]
			mov		al,[fog+eax]
			mov		[esi+4],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+5]
			mov		al,[fog+eax]
			mov		[esi+5],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+6]
			mov		al,[fog+eax]
			mov		[esi+6],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+7]
			mov		al,[fog+eax]
			mov		[esi+7],al
			add		esi,8
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi]
			mov		al,[fog+eax]
			mov		[esi],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+1]
			mov		al,[fog+eax]
			mov		[esi+1],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+2]
			mov		al,[fog+eax]
			mov		[esi+2],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+3]
			mov		al,[fog+eax]
			mov		[esi+3],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+4]
			mov		al,[fog+eax]
			mov		[esi+4],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+5]
			mov		al,[fog+eax]
			mov		[esi+5],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+6]
			mov		al,[fog+eax]
			mov		[esi+6],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+7]
			mov		al,[fog+eax]
			mov		[esi+7],al
			add		esi,8
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi]
			mov		al,[fog+eax]
			mov		[esi],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+1]
			mov		al,[fog+eax]
			mov		[esi+1],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+2]
			mov		al,[fog+eax]
			mov		[esi+2],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+3]
			mov		al,[fog+eax]
			mov		[esi+3],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+4]
			mov		al,[fog+eax]
			mov		[esi+4],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+5]
			mov		al,[fog+eax]
			mov		[esi+5],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+6]
			mov		al,[fog+eax]
			mov		[esi+6],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+7]
			mov		al,[fog+eax]
			mov		[esi+7],al
			add		esi,8
			add		ebx,edx
			//jnz		qqw1
			add		esi,adds
			add		edx,d
			mov		ebx,p
			add		ebx,c
			//mov		cl,4
			mov		p,ebx
			dec		ch
			jnz		qqw1
			pop		esi
			pop		edi
		};
		//Xbar(x+12,y+12,8,8,clrGreen);
	};
};
void ShowSuperFluentFog2_2old(int x,int y,int z1x,int z2x,int z3x,int z4x){
	//return;
	int z1=z1x<<16;
	int	z2=z2x<<16;
	int z3=z3x<<16;
	int	z4=z4x<<16;
	int scrof=int(ScreenPtr)+x+y*SCRSizeX;
	int adds=SCRSizeX-32;
	if(z1x<=zmin&&z2x<=zmin&&z3x<=zmin&&z4x<=zmin)return;
	if(z1x>=zmax&&z2x>=zmax&&z3x>=zmax&&z4x>=zmax){
		__asm{
			push	edi
			mov		ebx,adds
			mov		edi,scrof
			mov		dl,32
			xor		eax,eax
			cld
iug:		mov		ecx,8
			rep		stosd
			add		edi,ebx
			dec		dl
			jnz		iug
			pop		edi
		};
		return;
	}else{
		int a,b,c,d,p,q,r;
		static int xebp;;
		__asm{
			push	edi
			push	esi
			mov		eax,z1
			mov		ebx,eax
			//shl		ebx,8
			mov		a,ebx
			mov		ecx,z2
			sub		ecx,eax
			sar		ecx,5
			mov		b,ecx
			mov		ecx,z3
			sub		ecx,eax
			sar		ecx,5
			mov		c,ecx
			mov		ecx,z4
			add		ecx,z1
			sub		ecx,z2
			sub		ecx,z3
			sar		ecx,10
			mov		d,ecx
			//coefficients are now calculated
			mov		ecx,0x2010
			mov		ebx,a		//bh=fogging value
			add		ebx,256*256*3
			//mov		edx,b		
			xor		eax,eax
			mov		esi,scrof
			mov		p,ebx
			mov		xebp,ebp
			mov		ebp,b
qqw1:		mov		eax,ebx
			shr		eax,8
			mov		al,[esi]
			mov		dl,[fog+eax]
			//inc		esi
			add		ebx,ebp
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+1]
			mov		dh,[fog+eax]
			mov		[esi],dx
			add		esi,2
			add		ebx,ebp
			dec		cl
			jnz		qqw1
			mov		edx,ebp
			mov		ebp,xebp
			add		esi,adds
			add		edx,d
			mov		ebx,p
			add		ebx,c
			mov		cl,16
			mov		p,ebx
			mov		ebp,edx
			dec		ch
			jnz		qqw1
			mov		ebp,xebp
			pop		esi
			pop		edi
		};
		//Xbar(x+12,y+12,8,8,clrGreen);
	};
};
void ShowSuperFluentFog2_2(int x,int y,int z1x,int z2x,int z3x,int z4x){
	//return;
	int z1=z1x<<16;
	int	z2=z2x<<16;
	int z3=z3x<<16;
	int	z4=z4x<<16;
	int scrof=int(ScreenPtr)+x+y*SCRSizeX;
	int adds=SCRSizeX-32;
	if(z1x<=zmin&&z2x<=zmin&&z3x<=zmin&&z4x<=zmin)return;
	if(z1x>=zmax&&z2x>=zmax&&z3x>=zmax&&z4x>=zmax){
		__asm{
			push	edi
			mov		ebx,adds
			mov		edi,scrof
			mov		dl,32
			xor		eax,eax
			cld
iug:		mov		ecx,8
			rep		stosd
			add		edi,ebx
			dec		dl
			jnz		iug
			pop		edi
		};
		return;
	}else{
		int a,b,c,d,p,q,r;
		static int xebp;;
		__asm{
			push	edi
			push	esi
			mov		eax,z1
			mov		ebx,eax
			//shl		ebx,8
			mov		a,ebx
			mov		ecx,z2
			sub		ecx,eax
			sar		ecx,5
			mov		b,ecx
			mov		ecx,z3
			sub		ecx,eax
			sar		ecx,5
			mov		c,ecx
			mov		ecx,z4
			add		ecx,z1
			sub		ecx,z2
			sub		ecx,z3
			sar		ecx,10
			mov		d,ecx
			//coefficients are now calculated
			mov		ecx,0x1010
			mov		ebx,a		//bh=fogging value
			//add		ebx,256*256*3
			//mov		edx,b		
			xor		eax,eax
			mov		esi,scrof
			mov		p,ebx
			mov		xebp,ebp
			mov		ebp,b
qqw1:		mov		eax,ebx
			//add		eax,1632768
			shr		eax,8
			mov		al,[esi]
			mov		dl,[fog+eax]
			//inc		esi
			add		ebx,ebp
			mov		eax,ebx
			add		eax,16384
			shr		eax,8
			mov		al,[esi+1]
			mov		dh,[fog+eax]
			mov		[esi],dx
			add		esi,2
			add		ebx,ebp
			dec		cl
			jnz		qqw1
			mov		edx,ebp
			mov		ebp,xebp
			add		esi,adds
			add		edx,d
			mov		ebx,p
			add		ebx,c
			mov		cl,16
			mov		p,ebx
			mov		ebp,edx
qqw2:
			mov		eax,ebx
			add		eax,32768
			shr		eax,8
			mov		al,[esi]
			mov		dl,[fog+eax]
			//inc		esi
			add		ebx,ebp
			mov		eax,ebx
			add		eax,48796
			shr		eax,8
			mov		al,[esi+1]
			mov		dh,[fog+eax]
			mov		[esi],dx
			add		esi,2
			add		ebx,ebp
			dec		cl
			jnz		qqw2
			mov		edx,ebp
			mov		ebp,xebp
			add		esi,adds
			add		edx,d
			mov		ebx,p
			add		ebx,c
			mov		cl,16
			mov		p,ebx
			mov		ebp,edx


			dec		ch
			jnz		qqw1
			mov		ebp,xebp
			pop		esi
			pop		edi
		};
		//Xbar(x+12,y+12,8,8,clrGreen);
	};
};
void ShowSuperFluentFog2_2_16(int x,int y,int z1x,int z2x,int z3x,int z4x){
	//return;
	int z1=z1x<<16;
	int	z2=z2x<<16;
	int z3=z3x<<16;
	int	z4=z4x<<16;
	int scrof=int(ScreenPtr)+x+y*SCRSizeX;
	int adds=SCRSizeX-16;
	if(z1x<=zmin&&z2x<=zmin&&z3x<=zmin&&z4x<=zmin)return;
	if(z1x>=zmax&&z2x>=zmax&&z3x>=zmax&&z4x>=zmax){
		__asm{
			push	edi
			mov		ebx,adds
			mov		edi,scrof
			mov		dl,16
			xor		eax,eax
			cld
iug:		mov		ecx,4
			rep		stosd
			add		edi,ebx
			dec		dl
			jnz		iug
			pop		edi
		};
		return;
	}else{
		int a,b,c,d,p,q,r;
		static int xebp;;
		__asm{
			push	edi
			push	esi
			mov		eax,z1
			mov		ebx,eax
			//shl		ebx,8
			mov		a,ebx
			mov		ecx,z2
			sub		ecx,eax
			sar		ecx,4
			mov		b,ecx
			mov		ecx,z3
			sub		ecx,eax
			sar		ecx,4
			mov		c,ecx
			mov		ecx,z4
			add		ecx,z1
			sub		ecx,z2
			sub		ecx,z3
			sar		ecx,8
			mov		d,ecx
			//coefficients are now calculated
			mov		ecx,0x0808
			mov		ebx,a		//bh=fogging value
			//add		ebx,256*256*3
			//mov		edx,b		
			xor		eax,eax
			mov		esi,scrof
			mov		p,ebx
			mov		xebp,ebp
			mov		ebp,b
qqw1:		mov		eax,ebx
			//add		eax,1632768
			sar		eax,8
			mov		al,[esi]
			mov		dl,[fog+eax]
			//inc		esi
			add		ebx,ebp
			mov		eax,ebx
			add		eax,16384
			shr		eax,8
			mov		al,[esi+1]
			mov		dh,[fog+eax]
			mov		[esi],dx
			add		esi,2
			add		ebx,ebp
			dec		cl
			jnz		qqw1
			mov		edx,ebp
			mov		ebp,xebp
			add		esi,adds
			add		edx,d
			mov		ebx,p
			add		ebx,c
			mov		cl,8
			mov		p,ebx
			mov		ebp,edx
qqw2:
			mov		eax,ebx
			add		eax,32768
			shr		eax,8
			mov		al,[esi]
			mov		dl,[fog+eax]
			//inc		esi
			add		ebx,ebp
			mov		eax,ebx
			add		eax,48796
			shr		eax,8
			mov		al,[esi+1]
			mov		dh,[fog+eax]
			mov		[esi],dx
			add		esi,2
			add		ebx,ebp
			dec		cl
			jnz		qqw2
			mov		edx,ebp
			mov		ebp,xebp
			add		esi,adds
			add		edx,d
			mov		ebx,p
			add		ebx,c
			mov		cl,8
			mov		p,ebx
			mov		ebp,edx


			dec		ch
			jnz		qqw1
			mov		ebp,xebp
			pop		esi
			pop		edi
		};
		//Xbar(x+12,y+12,8,8,clrGreen);
	};
};

void ShowSuperFluentFog32_160(int x,int y,int z1x,int z2x,int z3x,int z4x){
	if(MiniMode)return;
	int z1=z1x<<16;
	int	z2=z2x<<16;
	int z3=z3x<<16;
	int	z4=z4x<<16;
	int scrof=int(ScreenPtr)+x+y*SCRSizeX;
	int adds=SCRSizeX-32;
	if(z1x<=63&&z2x<=63&&z3x<=63&&z4x<=63)return;
	if(z1x>=96&&z2x>=96&&z3x>=96&&z4x>=96){
		__asm{
			push	edi
			mov		ebx,adds
			mov		edi,scrof
			mov		dl,32
			xor		eax,eax
			cld
iug:		mov		ecx,8
			rep		stosd
			add		edi,ebx
			dec		dl
			jnz		iug
			pop		edi
		};
		return;
	}else{
		int a,b,c,d,p,q,r;
		__asm{
			push	edi
			push	esi
			pushf
			mov		eax,z1
			mov		ebx,eax
			//shl		ebx,8
			mov		a,ebx
			mov		ecx,z2
			sub		ecx,eax
			sar		ecx,5
			mov		b,ecx
			mov		ecx,z3
			sub		ecx,eax
			sar		ecx,5
			mov		c,ecx
			mov		ecx,z4
			add		ecx,z1
			sub		ecx,z2
			sub		ecx,z3
			sar		ecx,10
			mov		d,ecx
			//coefficients are now calculated
			mov		ecx,0x2004
			mov		ebx,a		//bh=fogging value
			//add		ebx,256*256*3
			mov		edx,b		//dx=fog incrementor
			xor		eax,eax
			mov		esi,scrof
			mov		p,ebx
qqw1:		mov		eax,ebx
			sar		eax,8
			mov		al,[esi]
			mov		al,[darkfog+eax]
			mov		[esi],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			sar		eax,8
			mov		al,[esi+1]
			mov		al,[darkfog+eax]
			mov		[esi+1],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			sar		eax,8
			mov		al,[esi+2]
			mov		al,[darkfog+eax]
			mov		[esi+2],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			sar		eax,8
			mov		al,[esi+3]
			mov		al,[darkfog+eax]
			mov		[esi+3],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+4]
			mov		al,[darkfog+eax]
			mov		[esi+4],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+5]
			mov		al,[darkfog+eax]
			mov		[esi+5],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+6]
			mov		al,[darkfog+eax]
			mov		[esi+6],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+7]
			mov		al,[darkfog+eax]
			mov		[esi+7],al
			add		esi,8
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi]
			mov		al,[darkfog+eax]
			mov		[esi],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+1]
			mov		al,[darkfog+eax]
			mov		[esi+1],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+2]
			mov		al,[darkfog+eax]
			mov		[esi+2],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+3]
			mov		al,[darkfog+eax]
			mov		[esi+3],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+4]
			mov		al,[darkfog+eax]
			mov		[esi+4],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+5]
			mov		al,[darkfog+eax]
			mov		[esi+5],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+6]
			mov		al,[darkfog+eax]
			mov		[esi+6],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+7]
			mov		al,[darkfog+eax]
			mov		[esi+7],al
			add		esi,8
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi]
			mov		al,[darkfog+eax]
			mov		[esi],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+1]
			mov		al,[darkfog+eax]
			mov		[esi+1],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+2]
			mov		al,[darkfog+eax]
			mov		[esi+2],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+3]
			mov		al,[darkfog+eax]
			mov		[esi+3],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+4]
			mov		al,[darkfog+eax]
			mov		[esi+4],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+5]
			mov		al,[darkfog+eax]
			mov		[esi+5],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+6]
			mov		al,[darkfog+eax]
			mov		[esi+6],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+7]
			mov		al,[darkfog+eax]
			mov		[esi+7],al
			add		esi,8
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi]
			mov		al,[darkfog+eax]
			mov		[esi],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+1]
			mov		al,[darkfog+eax]
			mov		[esi+1],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+2]
			mov		al,[darkfog+eax]
			mov		[esi+2],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+3]
			mov		al,[darkfog+eax]
			mov		[esi+3],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+4]
			mov		al,[darkfog+eax]
			mov		[esi+4],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+5]
			mov		al,[darkfog+eax]
			mov		[esi+5],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+6]
			mov		al,[darkfog+eax]
			mov		[esi+6],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+7]
			mov		al,[darkfog+eax]
			mov		[esi+7],al
			add		esi,8
			add		ebx,edx
			//jnz		qqw1
			add		esi,adds
			add		edx,d
			mov		ebx,p
			add		ebx,c
			//mov		cl,4
			mov		p,ebx
			dec		ch
			jnz		qqw1
			popf
			pop		esi
			pop		edi
		};
		//Xbar(x+12,y+12,8,8,clrGreen);
		//Xbar(x,y,32,32,clrGreen);
	};
};
void ShowSuperFluentFog16_160(int x,int y,int z1x,int z2x,int z3x,int z4x){
//	if(MiniMode)return;
	int z1=z1x<<16;
	int	z2=z2x<<16;
	int z3=z3x<<16;
	int	z4=z4x<<16;
	int scrof=int(ScreenPtr)+x+y*SCRSizeX;
	int adds=SCRSizeX-16;
	if(z1x<=63&&z2x<=63&&z3x<=63&&z4x<=63)return;
	if(z1x>=96&&z2x>=96&&z3x>=96&&z4x>=96){
		__asm{
			push	edi
			mov		ebx,adds
			mov		edi,scrof
			mov		dl,16
			xor		eax,eax
			cld
iug:		mov		ecx,4
			rep		stosd
			add		edi,ebx
			dec		dl
			jnz		iug
			pop		edi
		};
		return;
	}else{
		int a,b,c,d,p,q,r;
		__asm{
			push	edi
			push	esi
			pushf
			mov		eax,z1
			mov		ebx,eax
			//shl		ebx,8
			mov		a,ebx
			mov		ecx,z2
			sub		ecx,eax
			sar		ecx,4
			mov		b,ecx
			mov		ecx,z3
			sub		ecx,eax
			sar		ecx,4
			mov		c,ecx
			mov		ecx,z4
			add		ecx,z1
			sub		ecx,z2
			sub		ecx,z3
			sar		ecx,8
			mov		d,ecx
			//coefficients are now calculated
			mov		ecx,0x1002
			mov		ebx,a		//bh=fogging value
			//add		ebx,256*256*3
			mov		edx,b		//dx=fog incrementor
			xor		eax,eax
			mov		esi,scrof
			mov		p,ebx
qqw1:		mov		eax,ebx
			shr		eax,8
			mov		al,[esi]
			mov		al,[darkfog+eax]
			mov		[esi],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+1]
			mov		al,[darkfog+eax]
			mov		[esi+1],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+2]
			mov		al,[darkfog+eax]
			mov		[esi+2],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+3]
			mov		al,[darkfog+eax]
			mov		[esi+3],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+4]
			mov		al,[darkfog+eax]
			mov		[esi+4],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+5]
			mov		al,[darkfog+eax]
			mov		[esi+5],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+6]
			mov		al,[darkfog+eax]
			mov		[esi+6],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+7]
			mov		al,[darkfog+eax]
			mov		[esi+7],al
			add		esi,8
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi]
			mov		al,[darkfog+eax]
			mov		[esi],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+1]
			mov		al,[darkfog+eax]
			mov		[esi+1],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+2]
			mov		al,[darkfog+eax]
			mov		[esi+2],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+3]
			mov		al,[darkfog+eax]
			mov		[esi+3],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+4]
			mov		al,[darkfog+eax]
			mov		[esi+4],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+5]
			mov		al,[darkfog+eax]
			mov		[esi+5],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+6]
			mov		al,[darkfog+eax]
			mov		[esi+6],al
			//inc		esi
			add		ebx,edx
			mov		eax,ebx
			shr		eax,8
			mov		al,[esi+7]
			mov		al,[darkfog+eax]
			mov		[esi+7],al
			add		esi,8
			add		ebx,edx
			//jnz		qqw1
			add		esi,adds
			add		edx,d
			mov		ebx,p
			add		ebx,c
			//mov		cl,4
			mov		p,ebx
			dec		ch
			jnz		qqw1
			popf
			pop		esi
			pop		edi
		};
		//Xbar(x+12,y+12,8,8,clrGreen);
		//Xbar(x,y,32,32,clrGreen);
	};
};
#define shf 2500
#define fmin 3096
byte fden[8192];
void makeFden(){
	for(int i=0;i<8192;i++){
		if(i>shf)fden[i]=158;
		else fden[i]=5+div(i*153,shf).quot;
	};
};
int GetF(word k){
	if(k>=fmin)return 5;
	return fden[fmin-k];
};
void ShowSuperFog(){
	//return;
	for(int i=0;i<smaplx;i++)
		for(int j=0;j<smaply;j++){
			word* fm=&fmap[mapy+j-1][mapx+i-1];
			ShowSuperFluentFog32_160(smapx+(i<<5),smapy+(j<<5),
			GetF(fm[0]),GetF(fm[1]),GetF(fm[256]),GetF(fm[257]));
		};
};
void miniShowSuperFog(){
	//return;
	for(int i=0;i<smaplx;i++)
		for(int j=0;j<smaply;j++){
			word* fm=&fmap[mapy+j-1][mapx+i-1];
			ShowSuperFluentFog16_160(smapx+(i<<4),smapy+(j<<4),
			GetF(fm[0]),GetF(fm[1]),GetF(fm[256]),GetF(fm[257]));
		};
};
void TurnFogOn(){
	FogMode=1;
};
void TurnFogOff(){
	FogMode=0;
};