///////////////////////////////////////////////////////////
// CDIRSND.H -- Header file for the CDirSound class.
///////////////////////////////////////////////////////////

#ifndef __CDIRSND_H
#define __CDIRSND_H

#include <afxwin.h>
#include "cwave.h"
#include <dsound.h>

#define MAXSND 400
#define MAXSND1 401
class CDirSound
{
protected:
    LPDIRECTSOUND m_pDirectSoundObj;
    HWND m_hWindow;
    UINT m_currentBufferNum;
    LPDIRECTSOUNDBUFFER m_bufferPointers[MAXSND1];
    DWORD m_bufferSizes[MAXSND1];

public:
	void CDirSound::CreateDirSound(HWND hWnd);
    ~CDirSound();
    UINT CreateSoundBuffer(CWave* pWave);
    BOOL DirectSoundOK();
    BOOL CopyWaveToBuffer(CWave* pWave, UINT bufferNum);
	void SetVolume(UINT bufferNum,int vol);
	void SetPan(UINT bufferNum,int pan);
    BOOL PlaySound(UINT bufferNum);
    BOOL StopSound(UINT bufferNum);
	int GetPos(UINT bufferNum);
protected:
    void ReleaseAll();
};

#endif

