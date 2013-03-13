#include "Logger.h"
#include "../Config.h"
#include "../FileSystem/FileSystem.h"

namespace UIO_Plugin
{
	class LoggerFile : public Logger
	{
	public:
		LoggerFile( FileSystem * _fileSystem, const String & _fileName );
		~LoggerFile();

		bool init() override;
		void destroy() override;
		void log( const char * _msg ) override;
	protected:
		FileSystem * m_fileSystem;
		String m_fileName;
		OutputStream * m_stream;
	};
}