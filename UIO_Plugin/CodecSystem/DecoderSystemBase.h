#pragma once 
#include "../Config.h"
#include "DecoderSystem.h"
#include <vector>

namespace UIO_Plugin
{
	class FileSystem;
	class LogSystem;

	class DecoderSystemBase : public DecoderSystem
	{
	public:
		DecoderSystemBase( FileSystem * _fileSystem, LogSystem * _logSystem );
		virtual ~DecoderSystemBase();

		TDecoderId createDecoder( const String & _filePath ) override;

		Decoder * getDecoder( TDecoderId _id ) const override;
		
		bool init() override;
		void destroy() override;
	protected:
		virtual Decoder * _createDecoder( InputStream * _stream ) = 0;
		
	private:
		TDecoderId saveDecoder_( Decoder * _decoder );

	protected:
		typedef std::vector<Decoder *> TVectorDecoder;
		FileSystem * m_fileSystem;
		LogSystem * m_logSystem;
		TVectorDecoder m_decoders;
	};

	template <class T>
	class DecoderSystemSpecific: public DecoderSystemBase
	{
	public:
		DecoderSystemSpecific( FileSystem * _fileSystem, LogSystem * _logSystem)
			: DecoderSystemBase( _fileSystem, _logSystem )
		{
	
		}

	protected:
		Decoder * _createDecoder( InputStream * _stream ) override
		{
			T * decoder =  new T();
			if( decoder->initialize( _stream, m_logSystem ) == false )
			{
				delete decoder;
				return NULL;
			}

			return decoder;
		}
	};
}