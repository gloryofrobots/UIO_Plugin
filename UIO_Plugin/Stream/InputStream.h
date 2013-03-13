#pragma once 
#include "Stream.h"

namespace UIO_Plugin
{
	class InputStream : public Stream
	{
	public:
		virtual int read( char* _buf, int _count ) = 0;
		virtual void seek( int _pos ) = 0;
		virtual int tell() = 0;
		virtual int size() = 0;
		//virtual bool time( time_t & _time ) = 0;		
	};
}