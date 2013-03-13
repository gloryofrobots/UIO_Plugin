#pragma once 
#include <string>

#define AEIO_USER_PLUGIN_INVALID_ID -1

namespace UIO_Plugin
{	
	typedef std::string String;
	typedef String TCodecFormat; 
	typedef int TCodecId;
	typedef int TDecoderId;


	struct FrameOptions
	{
		size_t width;
		size_t height;
		size_t pitch;
		bool scale;
		size_t flags;
		double time;
	};

	struct Frame
	{
		FrameOptions options;
		unsigned char * data;
		size_t size;
	};
}
