#include "LogSystemBase.h"
#include "../Config.h"
#include <cstdarg>
namespace UIO_Plugin
{
	///////////////////////////////////////////////////////////
	LogSystemBase::LogSystemBase()
	{

	}
	///////////////////////////////////////////////////////////
	LogSystemBase::~LogSystemBase()
	{
	}
	///////////////////////////////////////////////////////////
	void LogSystemBase::log( const char * _msg, ... )
	{
		va_list argList;

		va_start(argList, _msg);

		char str[1024];

		vsprintf( str, _msg, argList );

		va_end(argList);

		String message( str );
		message += '\n';

		this->_logMessage( message.c_str() );
	}
	///////////////////////////////////////////////////////////
	void LogSystemBase::destroy()
	{
		for( TVectorLogger::iterator it = m_loggers.begin();
			it != m_loggers.end();
			++it)
		{
			(*it)->destroy();
		}

		m_loggers.clear();

		delete this;
	}
	///////////////////////////////////////////////////////////
	bool LogSystemBase::init()
	{
		return true;
	}
	///////////////////////////////////////////////////////////
	void LogSystemBase::_logMessage( const char * _msg )
	{
		for( TVectorLogger::iterator it = m_loggers.begin();
			it != m_loggers.end();
			++it)
		{
			(*it)->log( _msg );
		}
	}
	///////////////////////////////////////////////////////////
	void LogSystemBase::addLogger( Logger * _logger )
	{
		m_loggers.push_back( _logger );
	}
	///////////////////////////////////////////////////////////
}