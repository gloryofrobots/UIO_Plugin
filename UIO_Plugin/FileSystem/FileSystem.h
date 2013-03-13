#pragma once
#include "../Stream/InputStream.h"
#include "../Stream/OutputStream.h"
#include "../Config.h"
#include "../System.h"

namespace UIO_Plugin
{
	class FileSystem : public System
	{
	public: 
		virtual InputStream * openInputStream( const String& _fileName ) = 0;
		virtual OutputStream * openOutputStream( const String& _fileName ) = 0;
		virtual bool destroyStream( InputStream * ) = 0;
		virtual bool destroyStream( OutputStream * ) = 0;
	};
}