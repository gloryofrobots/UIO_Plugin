#include "Plugin.h"
#include "CodecSystem/DecoderSystemBase.h"
#include "CodecSystem/CodecSystemBase.h"
#include "CodecSystem/CodecBase.h"
#include "Platform/PlatformWin32.h"
#include "LogSystem/LoggerFile.h"
#include "LogSystem/LogSystemBase.h"
#include "../src/ImageDecoderWEBP.h"
#include "FileSystem/FileStreamCRT.h"
#include "FileSystem/FileSystem_T.h"

#define CHECK_INIT(system) if ( system->init() == false ){ return false; }
#define DESTROY_SYSTEM(system) if ( system != NULL ){ system->destroy();  system = NULL; }

namespace UIO_Plugin
{
	typedef DecoderSystemSpecific<ImageDecoderWEBP> TDecoderSystemWebP;
	typedef CodecWithDecoder<TDecoderSystemWebP> TCodecWebP;
	typedef FileSystem_T<InputFileStreamCRT, OutputFileStreamCRT> TFileSystemCRT;
	//////////////////////////////////////////////////////
	Platform * platform = NULL;
	LogSystem * logSystem = NULL;
	FileSystem * fileSystem = NULL;
	CodecSystem * codecSystem = NULL;
	Plugin * plugin = NULL;
	TPluginId pluginId = 0;
	//////////////////////////////////////////////////////
	static TPluginId  getPluginId() 
	{
		return pluginId;
	}
	//////////////////////////////////////////////////////
	static FileSystem * getFileSystem() 
	{
		return fileSystem;
	}
	//////////////////////////////////////////////////////
	static LogSystem * getLogSystem() 
	{
		return logSystem;
	}
	//////////////////////////////////////////////////////
	static CodecSystem * getCodecRegistry() 
	{
		return codecSystem;
	}
	//////////////////////////////////////////////////////
	static Platform * getPlatform() 
	{
		return platform;
	}
	//////////////////////////////////////////////////////
	static bool initSystems()
	{
		platform = new PlatformWin32();

		fileSystem = new TFileSystemCRT();
		CHECK_INIT(fileSystem);

		logSystem = new LogSystemBase();
		
		Logger * logger = new LoggerFile(fileSystem, "WebpLog.txt");
		
		CHECK_INIT( logger );

		logSystem->addLogger( logger );

		CHECK_INIT(logSystem);

		codecSystem = new CodecSystemBase();
		
		CHECK_INIT(codecSystem);

		if ( codecSystem->registerCodec( new TCodecWebP(), "web", fileSystem, logSystem ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////
	Plugin* getPlugin()
	{
		return plugin;
	}
	//////////////////////////////////////////////////////
	Plugin* createPlugin( TPluginId _id )
	{
		if( plugin != NULL )
		{
			if( _id == pluginId )
			{
				return plugin;
			}

			destroyPlugin();
		}

		try
		{
			if ( initSystems() == false )
			{
				return NULL;
			}

			plugin = new Plugin();

			(plugin)->getCodecRegistry = &getCodecRegistry;
			(plugin)->getPlatform = &getPlatform;
			(plugin)->getFileSystem = &getFileSystem;
			(plugin)->getLogSystem = &getLogSystem;
			(plugin)->getId = &getPluginId;

			pluginId = _id;
			return plugin;
		}
		catch( ... )
		{
			return NULL;
		}
	}
	///////////////////////////////////////////////////////////////////
	void destroyPlugin()
	{
		DESTROY_SYSTEM(codecSystem);
		DESTROY_SYSTEM(logSystem);
		DESTROY_SYSTEM(fileSystem);

		if( platform != NULL )
		{
			delete platform;
			platform = NULL;
		}
		if ( plugin != NULL )
		{
			delete plugin;
			plugin = NULL;
		}
	}
}
