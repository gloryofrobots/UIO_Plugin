#pragma once 
#include "DecoderSystem.h"

namespace UIO_Plugin
{
	class DecoderSystem;
	class FileSystem;
	class LogSystem;

	class Codec 
	{
	public:
		virtual bool initialise( FileSystem * _fileSystem, LogSystem* logSystem ) = 0;
		virtual void destroy() = 0;
		virtual DecoderSystem * getDecoderSystem() = 0;	
	};
}