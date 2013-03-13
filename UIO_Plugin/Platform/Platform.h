#pragma once 
#include "../Config.h"

namespace UIO_Plugin
{
	class Platform
	{
	public:
		virtual bool openFileDialog( char * _dest ) = 0;
		virtual String getFileExtension( String & _path ) = 0;
	};
}