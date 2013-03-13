#pragma once 
#include "Config.h"
#include "CodecSystem/CodecSystem.h"
#include "Platform/Platform.h"
#include "FileSystem/FileSystem.h"
#include "LogSystem/LogSystem.h"

namespace UIO_Plugin
{	
	struct PluginInfo
	{
		bool isVideo;
		bool isAudio;
		bool isSteel;

	};

	typedef long TPluginId;

	struct Plugin
	{
		TPluginId (*getId)();
		CodecSystem * (*getCodecRegistry)();
		FileSystem * (*getFileSystem)();
		LogSystem * (*getLogSystem)();
		Platform * (*getPlatform)();
	};

	bool pushNewHeader( UIO_Plugin_Header * _header ) override;
	bool fillFrame( Frame * _frame , UIO_Plugin_Header * _header ) override;

	Plugin* createPlugin( TPluginId _id );
	Plugin* getPlugin();
	void destroyPlugin();
}
