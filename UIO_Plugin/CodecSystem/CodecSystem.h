#pragma once 
#include "../Config.h"
#include "../System.h"
#include "Codec.h"

namespace UIO_Plugin
{
	class FileSystem;
	class LogSystem;

	class CodecSystem : public System
	{
	public:
		virtual bool registerCodec( Codec * _codec, const TCodecFormat&  _format, FileSystem * _fileSystem, LogSystem* _logSystem ) = 0;
		virtual TCodecId findCodec( const TCodecFormat&  _format ) = 0;
		virtual Codec * getCodec( TCodecId _codecId ) = 0;
	};
}