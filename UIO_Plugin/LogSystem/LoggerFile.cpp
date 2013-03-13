#include "LoggerFile.h"
#include <string.h>

namespace UIO_Plugin
{
	LoggerFile::LoggerFile( FileSystem * _fileSystem, const String & _fileName )
		: m_fileSystem(_fileSystem)
		, m_fileName(_fileName)
		, m_stream(NULL)
	{
	}
	///////////////////////////////////////////
	LoggerFile::~LoggerFile()
	{
	}
	///////////////////////////////////////////
	void LoggerFile::log( const char * _msg )
	{
		m_stream->write( _msg, strlen(_msg) );
		m_stream->flush();
	}
	///////////////////////////////////////////
	bool LoggerFile::init()
	{
		m_stream = m_fileSystem->openOutputStream( m_fileName );
		if( m_stream == NULL )
		{
			return false;
		}

		return true;
	}
	///////////////////////////////////////////
	void LoggerFile::destroy()
	{
		if ( m_stream != NULL )
		{
			m_fileSystem->destroyStream(m_stream);
			m_stream = NULL;
		}

		delete this;
	}
	///////////////////////////////////////////

}