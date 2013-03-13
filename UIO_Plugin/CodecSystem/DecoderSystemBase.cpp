#include "DecoderSystemBase.h"
#include "../FileSystem/FileSystem.h"
#include "../LogSystem/LogSystem.h"

namespace UIO_Plugin
{
	//////////////////////////////////////////////
	DecoderSystemBase::DecoderSystemBase( FileSystem * _fileSystem, LogSystem * _logSystem )
		: m_fileSystem(_fileSystem)
		, m_logSystem(_logSystem)
	{
	}
	//////////////////////////////////////////////
	DecoderSystemBase::~DecoderSystemBase()
	{
	}
	//////////////////////////////////////////////
	Decoder * DecoderSystemBase::getDecoder( TDecoderId _id ) const
	{
		if( _id < 0 || (size_t) _id >= m_decoders.size() )
		{
			return NULL;
		}

		return m_decoders[_id];
	}
	//////////////////////////////////////////////
	TDecoderId DecoderSystemBase::createDecoder( const String & _filePath )
	{
		InputStream * stream = m_fileSystem->openInputStream( _filePath );
		if ( stream == NULL )
		{
			return AEIO_USER_PLUGIN_INVALID_ID;
		}

		Decoder * decoder = this->_createDecoder( stream );
		if( decoder == NULL )
		{
			m_fileSystem->destroyStream( stream );
			return AEIO_USER_PLUGIN_INVALID_ID;
		}

		return this->saveDecoder_(decoder); 
	}
	//////////////////////////////////////////////
	TDecoderId DecoderSystemBase::saveDecoder_( Decoder * _decoder )
	{
		m_decoders.push_back(_decoder);
		return m_decoders.size() - 1;
	}
	//////////////////////////////////////////////
	bool DecoderSystemBase::init()
	{
		return true;
	}
	//////////////////////////////////////////////
	void DecoderSystemBase::destroy()
	{
		for(TVectorDecoder::iterator it = m_decoders.begin();
			it != m_decoders.end();
			++it )
		{
			Decoder * decoder = (*it);
			InputStream * stream = decoder->getStream();
			m_fileSystem->destroyStream(stream);
			decoder->destroy();
		}

		m_decoders.clear();

		delete this;
	}
	//////////////////////////////////////////////
}