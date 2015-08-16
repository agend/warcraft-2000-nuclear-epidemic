/***********************************************************************
 *Direct Draw initialisation module                    
 *
 * This module creates the Direct Draw object with the primary surface
 * and a backbuffer and sets 800x600x8 display mode.
 *
 ***********************************************************************/
#ifndef __DDINI_H_
#define __DDINI_H_
#include "afx.h"
//#include <windows.h>
//#include <windowsx.h>
#include <ddraw.h>
#include <stdlib.h>
#include <stdarg.h>
#include "resource.h"
#include "lines.h"
#ifndef __ddini_cpp_
extern LPDIRECTDRAW            lpDD;           // DirectDraw object
extern LPDIRECTDRAWSURFACE     lpDDSPrimary;   // DirectDraw primary surface
extern LPDIRECTDRAWSURFACE     lpDDSBack;      // DirectDraw back surface
extern BOOL                    bActive;        // is application active?
extern BOOL                    CurrentSurface; //=FALSE if backbuffer
											   // is active (Primary surface is visible)
											   //=TRUE if  primary surface is active
											   // (but backbuffer is visible)
extern BOOL                    DDError;        //=FALSE if Direct Draw works normally 
extern DDSURFACEDESC   ddsd;
extern HWND hwnd;
extern BOOL DDDebug;
/*  Create Direct Draw object
 * 
 * This procedure creadtes Direct Draw object with primary surface and 
 * one backbuffer and sets 800x600x8 display mode.
 * This procedure enables usage of all other procedure in thes module.
 */
BOOL CreateDDObjects(HWND hwnd);
/*     Closing all Direct Draw objects
 *
 * This procedure must be called before the program terminates,
 * otherwise Windows can occur some problems.
 */
void FreeDDObjects( void );
/*
 * Flipping Pages
 */
void FlipPages(void);
/*
 * Getting Screen Pointer
 *
 * You will ge the pointer to the invisible area of the screen
 * i.e, if primary surface is visible, then you will obtain the
 * pointer to the backbuffer.
 * You must call UnlockSurface() to allow Windows draw on the screen
 */
void LockSurface(void);
/*
 *  Unlocking the surface 
 *
 *  You must unlock the Video memory for Windows to work properly
 */
void UnlockSurface(void);
/*
 * Getting then DC of the active (invisible) area of the screen
 */
HDC GetSDC(void);
//Sttting the palette
void LoadPalette(LPCSTR lpFileName);
void SetDebugMode();
void NoDebugMode();
#endif
#endif //__DDINI_H_