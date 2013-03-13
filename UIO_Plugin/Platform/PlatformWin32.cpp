#include "PlatformWin32.h"
#include <windows.h>

namespace UIO_Plugin
{
	template<class T>
	int inline findAndReplace(T& source, const T& find, const T& replace)
	{
		int num=0;
		std::size_t fLen = find.size();
		std::size_t rLen = replace.size();
		for (std::size_t pos=0; (pos=source.find(find, pos))!=T::npos; pos+=rLen)
		{
			num++;
			source.replace(pos, fLen, replace);
		}
		return num;
	}
	
	bool PlatformWin32::openFileDialog(char * _buffer)
	{
		HWND hostWindow = GetForegroundWindow();
		OPENFILENAME ofn;       // common dialog box structure
		char szFile[260];       // buffer for file name
		// Initialize OPENFILENAME
		ZeroMemory( &ofn, sizeof(ofn) );
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hostWindow;
		ofn.lpstrFile = szFile;
		// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
		// use the contents of szFile to initialize itself.
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "webp files(*.webp)\0*.webp\0all\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		// Display the Open dialog box. 
		if(GetOpenFileName(&ofn)==TRUE){ 
			strcpy(_buffer,ofn.lpstrFile);
		}

		//PF_GET_PLATFORM_DATA(PF_PlatData_MAIN_WND, (void **)&hwndOwner);
		return true;
	}
	////////////////////////////////////////////////////////////////////////////
	UIO_Plugin::String PlatformWin32::getFileExtension( String & _path )
	{
		String::size_type idx;

		idx = _path.rfind('.');

		if( idx == String::npos )
		{
			return String("");
		}

		return _path.substr(idx+1);
	}
	////////////////////////////////////////////////////////////////////////////
}