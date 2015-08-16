/***********************************************************************
 *Direct Draw initialisation module                    
 *
 * This module creates the Direct Draw object with the primary surface
 * and a backbuffer and sets 800x600x8 display mode.
 *
 ***********************************************************************/
#define __ddini_cpp_
#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mode.h"
#include "MapDiscr.h"
#include "fog.h"
#include "GSound.h"
void SERROR();
void SERROR1();
void SERROR2();
void PropCopy();
void InitRLCWindows();
//#define COPYSCR
const int InitLx=1024;
const int InitLy=768;
int RealLx;
int RealLy;
int SCRSizeX;
int SCRSizeY;
int RSCRSizeX;
int RSCRSizeY;
int COPYSizeX;
int Pitch;
LPDIRECTDRAW            lpDD;           // DirectDraw object
LPDIRECTDRAWSURFACE     lpDDSPrimary;   // DirectDraw primary surface
LPDIRECTDRAWSURFACE     lpDDSBack;      // DirectDraw back surface
BOOL                    bActive;        // is application active?
BOOL                    CurrentSurface; //=FALSE if backbuffer
                                        // is active (Primary surface is visible)
										//=TRUE if  primary surface is active
										// (but backbuffer is visible)
BOOL                    DDError;        //=FALSE if Direct Draw works normally 
DDSURFACEDESC           ddsd;
PALETTEENTRY            GPal[256];
LPDIRECTDRAWPALETTE     lpDDPal;
BOOL					DDDebug;
extern bool PalDone;
extern word PlayerMenuMode;
typedef struct zzz{			
	BITMAPINFO bmp;
	PALETTEENTRY XPal[255];
};
zzz xxt;
//typedef byte barr[ScreenSizeX*ScreenSizeY];
void* offScreenPtr;
/*
 * Flipping Pages
 */
void FlipPages(void)
{
	if (DDDebug){
		if(PlayerMenuMode!=1){
		//	ProcessFog();
		//	ShowFoggedBattle();
		};
		HDC WH=GetWindowDC(hwnd);
		//memcpy(xxt.XPal,&GPal[1],sizeof xxt.XPal);
		for(int i=0;i<256;i++){
			xxt.bmp.bmiColors[i].rgbRed=GPal[i].peRed;
			xxt.bmp.bmiColors[i].rgbBlue=GPal[i].peBlue;
			xxt.bmp.bmiColors[i].rgbGreen=GPal[i].peGreen;
		};
		xxt.bmp.bmiHeader.biSize=sizeof BITMAPINFOHEADER;
		xxt.bmp.bmiHeader.biWidth=SCRSizeX;
		xxt.bmp.bmiHeader.biHeight=-SCRSizeY;
		xxt.bmp.bmiHeader.biPlanes=1;
		xxt.bmp.bmiHeader.biBitCount=8;
		xxt.bmp.bmiHeader.biCompression=BI_RGB;
		xxt.bmp.bmiHeader.biSizeImage=0;
		int z=StretchDIBits(WH,0,0,COPYSizeX,RSCRSizeY,
			0,MaxSizeY-RSCRSizeY,COPYSizeX,RSCRSizeY,RealScreenPtr,&xxt.bmp,
			DIB_RGB_COLORS,SRCCOPY);
		//StretchDIBits(WH,smapx,smapy,smaplx*32,smaply*32,
		//	smapx,smapy,smaplx*32,smaply*32,RealScreenPtr,&xxt.bmp,
		//	DIB_RGB_COLORS,SRCCOPY);
		return;
	};	
#ifdef COPYSCR
	/*__asm{
		push	esi
		push	edi
		mov		esi,ScreenPtr
		mov		edi,RealScreenPtr
		mov		ecx,120000
		cld
		rep		movsd
		pop		edi
		pop		esi
	}*/
	if(PlayerMenuMode==1){
		int ofs=0;
		int	lx=COPYSizeX>>2;
		int	ly=SCRSizeY;
		int	addOf=SCRSizeX-(lx<<2);
		int RaddOf=RSCRSizeX-(lx<<2);
		__asm{
			push	esi
			push	edi	
			mov		esi,ScreenPtr
			mov		edi,RealScreenPtr
			add		esi,ofs
			add		edi,ofs
			cld
			mov		eax,ly
xxx:
			mov		ecx,lx
			rep		movsd
			add		esi,addOf
			add		edi,RaddOf
			dec		eax
			jnz		xxx
		};
		return;
	};
	/*__asm{
		push	esi
		push	edi
	};
	ProcessFog();
	ShowFoggedBattle();
	__asm{
		pop		edi
		pop		esi
	};*/
	int ofs=smapx+smapy*SCRSizeX;
	int ofs1=smapx+smapy*RSCRSizeX;
	int	lx=smaplx<<3;
	int	ly=smaply<<5;
	int	addOf=SCRSizeX-(lx<<2);
	int RaddOf=RSCRSizeX-(lx<<2);;
	__asm{
		push	esi
		push	edi
		mov		esi,ScreenPtr
		mov		edi,RealScreenPtr
		add		esi,ofs
		add		edi,ofs1
		cld
		mov		eax,ly
xxxx:
		mov		ecx,lx
		rep		movsd
		add		esi,addOf
		add		edi,RaddOf
		dec		eax
		jnz		xxxx
	};
	ofs=minix+miniy*SCRSizeX;
	ofs1=minix+miniy*RSCRSizeX;
	lx=msx>>3;
	ly=msy>>1;
	addOf=SCRSizeX-(lx<<2);
	RaddOf=RSCRSizeX-(lx<<2);
	__asm{
		push	esi
		push	edi
		mov		esi,ScreenPtr
		mov		edi,RealScreenPtr
		add		esi,ofs
		add		edi,ofs1
		cld
		mov		eax,ly
yyy:
		mov		ecx,lx
		rep		movsd
		add		esi,addOf
		add		edi,RaddOf
		dec		eax
		jnz		yyy
	};
	//PropCopy();
	return;
#else

	if(DDDebug) return;
	CurrentSurface=!CurrentSurface;
	while( 1 )
    {
        HRESULT ddrval;
        ddrval = lpDDSPrimary->Flip( NULL, 0 );
        if( ddrval == DD_OK )
        {
            break;
        }
        if( ddrval == DDERR_SURFACELOST )
        {
            ddrval = lpDDSPrimary->Restore();
            if( ddrval != DD_OK )
            {
                break;
            }
        }
        if( ddrval != DDERR_WASSTILLDRAWING )
        {
                break;
        }
    }
#endif
}
/*
 * Getting Screen Pointer
 *
 * You will ge the pointer to the invisible area of the screen
 * i.e, if primary surface is visible, then you will obtain the
 * pointer to the backbuffer.
 * You must call UnlockSurface() to allow Windows draw on the screen
 */
void LockSurface(void)
{
	if (DDDebug)
	{
		ScreenPtr=(void*)(int(offScreenPtr)+MaxSizeX*32);
		ddsd.lpSurface=ScreenPtr;
		RealScreenPtr=ScreenPtr;
		return;
	}
	if (DDError)  return;
#ifdef COPYSCR
	if (lpDDSPrimary->Lock(NULL,&ddsd,
							    DDLOCK_SURFACEMEMORYPTR|
								DDLOCK_WAIT,NULL)!=DD_OK) DDError=true ;
	RSCRSizeX=ddsd.lPitch;
#else
	if (lpDDSBack->Lock(NULL,&ddsd,
							    DDLOCK_SURFACEMEMORYPTR|
								DDLOCK_WAIT,NULL)!=DD_OK) DDError=true ;
	RSCRSizeX=ddsd.lPitch;
#endif
#ifdef COPYSCR
	ScreenPtr=(void*)(int(offScreenPtr)+MaxSizeX*32);
	//ddsd.lpSurface=ScreenPtr;
	RealScreenPtr=ScreenPtr;
	RealScreenPtr=ddsd.lpSurface;
#else
	ScreenPtr=ddsd.lpSurface;
#endif
}
/*
 *  Unlocking the surface 
 *
 *  You must unlock the Video memory for Windows to work properly
 */
void UnlockSurface(void)
{
	if(DDDebug) return;
	if (DDError)  return;
	//Back Buffer is active
#ifdef COPYSCR
	if (lpDDSPrimary->Unlock(NULL)!=DD_OK) DDError=true ;
#else
	if (lpDDSBack->Unlock(NULL)!=DD_OK) DDError=true ;
#endif
}
/*
 * Getting then DC of the active (invisible) area of the screen
 */
HDC GetSDC(void)
{
	if(DDDebug) return 0;
	HDC hdc;
	if (DDError) return 0;
	if (CurrentSurface)
	{
		//Back Buffer is active
		if (lpDDSPrimary->GetDC(&hdc)!=DD_OK) DDError=true ;
	}else
	{
		//Primary Surface is active
		if (lpDDSBack->GetDC(&hdc)!=DD_OK) DDError=true;
	}
	return hdc;
}
/*
 * Timer Callback 
 */
BOOL CreateDDObjects(HWND hwnd)
{
	HRESULT ddrval;
	DDSCAPS ddscaps;
	char    buf[256];
	DDError=false;
	CurrentSurface=true;
	if (DDDebug)
	{
		
		DDError=false;
		SCRSizeX=MaxSizeX;
		SCRSizeY=MaxSizeY;
		COPYSizeX=RealLx;
		RSCRSizeX=RealLx;
		RSCRSizeY=RealLy;
		ScrHeight=SCRSizeY;
		ScrWidth=SCRSizeX;
		InitRLCWindows();
		WindX=0;
		WindY=0;
		WindLx=SCRSizeX;
		WindLy=SCRSizeY;
		WindX1=WindLx-1;
		WindY1=WindLy-1;
		BytesPerPixel=1;
		offScreenPtr=(malloc(SCRSizeX*(SCRSizeY+32*4)));
		return true;
	}
#ifdef COPYSCR
	offScreenPtr=offScreenPtr=(malloc(MaxSizeX*(MaxSizeY+32*4)));
#endif
	ddrval = DirectDrawCreate( NULL, &lpDD, NULL );
    if( ddrval == DD_OK )
    {
        // Get exclusive mode
        ddrval = lpDD->SetCooperativeLevel( hwnd,
                                DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
        if(ddrval == DD_OK )
        {
            ddrval = lpDD->SetDisplayMode(RealLx,RealLy,8); //COPYSizeX,RSCRSizeY, 8 );
            if( ddrval == DD_OK )
            {
                // Create the primary surface with 1 back buffer
                ddsd.dwSize = sizeof( ddsd );
                ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
                ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
                                      DDSCAPS_FLIP | 
                                      DDSCAPS_COMPLEX;
                ddsd.dwBackBufferCount = 1;
                ddrval = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL );
                if( ddrval == DD_OK )
                {
                    // Get a pointer to the back buffer
                    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
                    ddrval = lpDDSPrimary->GetAttachedSurface(&ddscaps, 
                                                          &lpDDSBack);
					if (ddrval==DD_OK)
					{
						DDError=false;
						SCRSizeX=MaxSizeX;
						SCRSizeY=MaxSizeY;
						RSCRSizeX=RealLx;//ddsd.lPitch;
						Pitch=ddsd.lPitch;
						COPYSizeX=RealLx;
						RSCRSizeY=RealLy;
						ScrHeight=SCRSizeY;
						ScrWidth=SCRSizeX;
						WindX=0;
						WindY=0;
						WindLx=SCRSizeX;
						WindLy=SCRSizeY;
						WindX1=WindLx-1;
						WindY1=WindLy-1;
						BytesPerPixel=1;
						return true;
					}
                        // Create a timer to flip the pages
                      /*  if( SetTimer( hwnd, TIMER_ID, 50, NULL ) )
                        {
                             return TRUE;
                        }*/
                }
            }
        }
    }
    wsprintf(buf, "Direct Draw Init Failed (%08lx)\n", ddrval );
    MessageBox( hwnd, buf, "ERROR", MB_OK );
	return false;
}
BOOL CreateRGBDDObjects(HWND hwnd)
{
	HRESULT ddrval;
	DDSCAPS ddscaps;
	char    buf[256];
	DDError=false;
	CurrentSurface=true;
	if (DDDebug)
	{
		
		DDError=false;
		SCRSizeX=MaxSizeX;
		SCRSizeY=MaxSizeY;
		COPYSizeX=RealLx;
		RSCRSizeX=RealLx;
		RSCRSizeY=RealLy;
		ScrHeight=SCRSizeY;
		ScrWidth=SCRSizeX;
		InitRLCWindows();
		WindX=0;
		WindY=0;
		WindLx=SCRSizeX;
		WindLy=SCRSizeY;
		WindX1=WindLx-1;
		WindY1=WindLy-1;
		BytesPerPixel=1;
		offScreenPtr=(malloc(SCRSizeX*(SCRSizeY+32*4)));
		return true;
	}
#ifdef COPYSCR
	offScreenPtr=offScreenPtr=(malloc(MaxSizeX*(MaxSizeY+32*4)));
#endif
	ddrval = DirectDrawCreate( NULL, &lpDD, NULL );
    if( ddrval == DD_OK )
    {
        // Get exclusive mode
        ddrval = lpDD->SetCooperativeLevel( hwnd,
                                DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
        if(ddrval == DD_OK )
        {
            ddrval = lpDD->SetDisplayMode(800,600,32); //COPYSizeX,RSCRSizeY, 8 );
            if( ddrval == DD_OK )
            {
                // Create the primary surface with 1 back buffer
                ddsd.dwSize = sizeof( ddsd );
                ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
                ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
                                      DDSCAPS_FLIP | 
                                      DDSCAPS_COMPLEX;
                ddsd.dwBackBufferCount = 1;
                ddrval = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL );
                if( ddrval == DD_OK )
                {
                    // Get a pointer to the back buffer
                    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
                    ddrval = lpDDSPrimary->GetAttachedSurface(&ddscaps, 
                                                          &lpDDSBack);
					if (ddrval==DD_OK)
					{
						DDError=false;
						SCRSizeX=MaxSizeX;
						SCRSizeY=MaxSizeY;
						RSCRSizeX=RealLx;//ddsd.lPitch;
						Pitch=ddsd.lPitch;
						COPYSizeX=RealLx;
						RSCRSizeY=RealLy;
						ScrHeight=SCRSizeY;
						ScrWidth=SCRSizeX;
						WindX=0;
						WindY=0;
						WindLx=SCRSizeX;
						WindLy=SCRSizeY;
						WindX1=WindLx-1;
						WindY1=WindLy-1;
						BytesPerPixel=1;
						return true;
					}
                        // Create a timer to flip the pages
                      /*  if( SetTimer( hwnd, TIMER_ID, 50, NULL ) )
                        {
                             return TRUE;
                        }*/
                }
            }
        }
    }
    wsprintf(buf, "Direct Draw Init Failed (%08lx)\n", ddrval );
    MessageBox( hwnd, buf, "ERROR", MB_OK );
	return false;
}
BOOL CreateRGB640DDObjects(HWND hwnd)
{
	HRESULT ddrval;
	DDSCAPS ddscaps;
	char    buf[256];
	DDError=false;
	CurrentSurface=true;
	if (DDDebug)
	{
		
		DDError=false;
		SCRSizeX=MaxSizeX;
		SCRSizeY=MaxSizeY;
		COPYSizeX=RealLx;
		RSCRSizeX=RealLx;
		RSCRSizeY=RealLy;
		ScrHeight=SCRSizeY;
		ScrWidth=SCRSizeX;
		InitRLCWindows();
		WindX=0;
		WindY=0;
		WindLx=SCRSizeX;
		WindLy=SCRSizeY;
		WindX1=WindLx-1;
		WindY1=WindLy-1;
		BytesPerPixel=1;
		offScreenPtr=(malloc(SCRSizeX*(SCRSizeY+32*4)));
		return true;
	}
#ifdef COPYSCR
	offScreenPtr=offScreenPtr=(malloc(MaxSizeX*(MaxSizeY+32*4)));
#endif
	ddrval = DirectDrawCreate( NULL, &lpDD, NULL );
    if( ddrval == DD_OK )
    {
        // Get exclusive mode
        ddrval = lpDD->SetCooperativeLevel( hwnd,
                                DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
        if(ddrval == DD_OK )
        {
            ddrval = lpDD->SetDisplayMode(640,480,24); //COPYSizeX,RSCRSizeY, 8 );
            if( ddrval == DD_OK )
            {
                // Create the primary surface with 1 back buffer
                ddsd.dwSize = sizeof( ddsd );
                ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
                ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
                                      DDSCAPS_FLIP | 
                                      DDSCAPS_COMPLEX;
                ddsd.dwBackBufferCount = 1;
                ddrval = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL );
                if( ddrval == DD_OK )
                {
                    // Get a pointer to the back buffer
                    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
                    ddrval = lpDDSPrimary->GetAttachedSurface(&ddscaps, 
                                                          &lpDDSBack);
					if (ddrval==DD_OK)
					{
						DDError=false;
						SCRSizeX=MaxSizeX;
						SCRSizeY=MaxSizeY;
						RSCRSizeX=RealLx;//ddsd.lPitch;
						Pitch=ddsd.lPitch;
						COPYSizeX=RealLx;
						RSCRSizeY=RealLy;
						ScrHeight=SCRSizeY;
						ScrWidth=SCRSizeX;
						WindX=0;
						WindY=0;
						WindLx=SCRSizeX;
						WindLy=SCRSizeY;
						WindX1=WindLx-1;
						WindY1=WindLy-1;
						BytesPerPixel=1;
						return true;
					}
                        // Create a timer to flip the pages
                      /*  if( SetTimer( hwnd, TIMER_ID, 50, NULL ) )
                        {
                             return TRUE;
                        }*/
                }
            }
        }
    }
    wsprintf(buf, "Direct Draw Init Failed (%08lx)\n", ddrval );
    MessageBox( hwnd, buf, "ERROR", MB_OK );
	return false;
}
/*   Direct Draw palette loading*/
void LoadPalette(LPCSTR lpFileName)
{
	//if (DDDebug) return;
	if (DDError) return;
	ResFile pf=RReset(lpFileName);
	memset(&GPal,0,1024);
	if (pf!=INVALID_HANDLE_VALUE)
	{
		for(int i=0;i<256;i++){
			RBlockRead(pf,&GPal[i],3);
			//RBlockRead(pf,&xx.bmp.bmiColors[i],3);
		};
		RClose(pf);
		if(!DDDebug){
			if(!PalDone){
				lpDD->CreatePalette(DDPCAPS_8BIT,&GPal[0],&lpDDPal,NULL);
				PalDone=true;
				lpDDSPrimary->SetPalette(lpDDPal);
				//lpDDSBack->SetPalette(lpDDPal);
			}else{
				lpDDPal->SetEntries(0,0,256,GPal);
			};
			//lpDDSPrimary->SetPalette(lpDDPal);
			//lpDDSBack->SetPalette(lpDDPal);
		};
	}

}
	
/*     Closing all Direct Draw objects
 *
 * This procedure must be called before the program terminates,
 * otherwise Windows can occur some problems.
 */

void FreeDDObjects( void )
{
	if (DDDebug)
	{
		free(offScreenPtr);
		return;
	}
    if( lpDD != NULL )
    {
		/*if( lpDDSBack != NULL )
        {
            lpDDSBack->Release();
            lpDDSBack = NULL;
        };*/

        if( lpDDSPrimary != NULL )
        {
            lpDDSPrimary->Release();
            lpDDSPrimary = NULL;
        };
		        lpDD->Release();
        lpDD = NULL;
    }
}
void SetDebugMode()
{
	DDDebug=true;
}
void NoDebugMode()
{
	DDDebug=false;
}
