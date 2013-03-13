#pragma once 
#include "../Config.h"
#include "../System.h"
#include "Decoder.h"
#include <vector>

namespace UIO_Plugin
{
	class FileSystem;
	class LogSystem;

	class DecoderSystem : public System
	{
	public:
		virtual TDecoderId createDecoder( const String & _filePath ) = 0;
		virtual Decoder * getDecoder( TDecoderId _id ) const = 0;
	};
}