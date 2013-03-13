#pragma once 
#include "Logger.h"
#include "../System.h"

namespace UIO_Plugin
{
	class LogSystem : public System
	{	
	public:
		virtual void log( const char * _msg, ... ) = 0;
		virtual void addLogger( Logger * _logger ) = 0;
	};
}