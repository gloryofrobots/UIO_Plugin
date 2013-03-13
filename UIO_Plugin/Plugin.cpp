#include "Plugin.h"

namespace UIO_Plugin
{	
	///////////////////////////////////////////////////////////////////////////
	bool pushNewHeader( UIO_Plugin_Header * _header )
	{
		String filePath( _header->name );
		String extension = getPlugin()->getPlatform()->getFileExtension( filePath );
		CodecSystem * registry = getPlugin()->getCodecRegistry();
		TCodecId codecId = registry->findCodec( extension );

		if( codecId == AEIO_USER_PLUGIN_INVALID_ID )
		{
			return false;
		}

		Codec * codec = registry->getCodec( codecId );
		DecoderSystem * decoderSystem = codec->getDecoderSystem();

		TDecoderId decoderId = decoderSystem->createDecoder( filePath );

		if ( decoderId == AEIO_USER_PLUGIN_INVALID_ID )
		{
			return false;
		}

		Decoder* decoder = decoderSystem->getDecoder( decoderId );

		_header->decoderId = decoderId;
		_header->codecId =	codecId;

		decoder->writeHeader(_header);
		return true;
	}
	///////////////////////////////////////////////////////////////////////////
	bool fillFrame( Frame * _frame , UIO_Plugin_Header * _header )
	{
		UIO_Plugin::CodecSystem * registry = getPlugin()->getCodecRegistry();
		UIO_Plugin::Codec * codec = registry->getCodec( _header->codecId );
		if ( codec == NULL )
		{
			return false;
		}

		UIO_Plugin::DecoderSystem * decoderSystem = codec->getDecoderSystem();
		UIO_Plugin::Decoder * decoder = decoderSystem->getDecoder( _header->decoderId );

		if( decoder == NULL )
		{
			return false;
		}

		decoder->setOptions( &(_frame->options) );
		decoder->decode( _frame->data, _frame->size );

		return true;
	}
	///////////////////////////////////////////////////////////////////////////
}