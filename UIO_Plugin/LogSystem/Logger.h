#pragma once 

namespace UIO_Plugin
{
	class Logger
	{
	public:
		virtual bool init() = 0;
		virtual void destroy() = 0;
		virtual void log( const char * _msg ) = 0;
	};
}