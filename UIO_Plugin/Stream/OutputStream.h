#pragma once 
#include "Stream.h"

namespace UIO_Plugin
{
	class OutputStream : public Stream
	{
	public:
		virtual void write( const char* _data, int _count ) = 0;
		virtual void flush() = 0;
	};
}