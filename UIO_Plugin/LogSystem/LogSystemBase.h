#pragma once 
#include "LogSystem.h"
#include <vector>

namespace UIO_Plugin
{
	class LogSystemBase : public LogSystem
	{	
	public:
		LogSystemBase();
		virtual ~LogSystemBase();
		void log( const char * _msg, ... ) override;

	public:
		bool init() override;
		void destroy() override;
		void addLogger( Logger * _logger ) override;
	protected:
		void _logMessage( const char * _msg );

		typedef std::vector<Logger *> TVectorLogger;
		TVectorLogger m_loggers;
	};
}