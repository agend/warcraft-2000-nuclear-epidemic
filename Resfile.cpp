/*      Work with the resource files
 *   
 *  You must use this module for accesss to files this 
 * routine allows you to read files from disk or from 
 * the resource file, you even will not recognise where
 * the given file is.
 */
#include <afx.h>
//#include <windows.h>
#include <stdlib.h>
typedef HANDLE ResFile;
//Opening the resource file
ResFile RReset(LPCSTR lpFileName)
{
	SetLastError(0);
	return CreateFile(lpFileName,GENERIC_READ,FILE_SHARE_READ,NULL,
		                         OPEN_EXISTING,0/*FILE_ATTRIBUTE_NORMAL*/,NULL);
}
//Rewriting file
ResFile RRewrite(LPCSTR lpFileName)
{
	return CreateFile(lpFileName,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,
		                         CREATE_ALWAYS,0,NULL);
}
//Getting size of the resource file
DWORD RFileSize(HANDLE hFile)
{
	return GetFileSize(hFile,NULL);
}
// Setting file position 
DWORD RSeek(ResFile hFile,int pos)
{
	return SetFilePointer(hFile,pos,NULL,FILE_BEGIN);
}
//Reading the file
DWORD RBlockRead(ResFile hFile,LPVOID lpBuffer,DWORD BytesToRead)
{
	DWORD readBytes;
	ReadFile(hFile,lpBuffer,BytesToRead,&readBytes,NULL);
	return readBytes;
}
//Writing the file
DWORD RBlockWrite(ResFile hFile,LPVOID lpBuffer,DWORD BytesToWrite)
{
	DWORD writeBytes;
	WriteFile(hFile,lpBuffer,BytesToWrite,&writeBytes,NULL);
	return writeBytes;
}
DWORD IOresult(void)
{
	return GetLastError();
}
void RClose(ResFile hFile)
{
	CloseHandle(hFile);
}