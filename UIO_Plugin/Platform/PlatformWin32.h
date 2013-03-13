#pragma once
#include "Platform.h"

namespace UIO_Plugin
{
	class PlatformWin32 : public Platform
	{
	public:
		bool openFileDialog( char * _dest ) override;
		String getFileExtension( String & _path ) override;
	};
}