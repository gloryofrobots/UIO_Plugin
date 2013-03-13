#pragma once 
#include "../Config.h"
#include "../Stream/InputStream.h"
#include "../UIO_Plugin_Header.h"
#include "../LogSystem/LogSystem.h"

namespace UIO_Plugin
{	
	class Decoder
	{
	public:
		virtual bool initialize( InputStream * _stream, LogSystem * _logSystem ) = 0;
		virtual void destroy() = 0;
		virtual bool decode( unsigned char* _buffer, unsigned int _bufferSize ) = 0;
		virtual InputStream * getStream() const = 0;
		virtual void writeHeader( UIO_Plugin_Header * _header ) = 0;
		//virtual virtual const FormatInfo& getFormatInfo() const = 0;
		virtual void setOptions( FrameOptions * _options ) = 0;
	};
}