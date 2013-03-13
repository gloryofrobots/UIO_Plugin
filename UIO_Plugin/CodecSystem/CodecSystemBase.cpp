#include "CodecSystemBase.h"
#include "../FileSystem/FileSystem.h"
#include "../LogSystem/LogSystem.h"

namespace UIO_Plugin
{
	///////////////////////////////////////////////////////////////////////////
	bool CodecSystemBase::registerCodec( Codec * _codec, const TCodecFormat& _format, FileSystem * _fileSystem, LogSystem* _logSystem )
	{
		TMapCodecFormats::iterator it_find = m_codecFormats.find( _format );

		if( it_find != m_codecFormats.end() )
		{
			return false;
		}

		if( _codec->initialise( _fileSystem, _logSystem ) == false )
		{
			return false;
		}

		m_codecs.push_back( _codec );
		m_codecFormats[_format] = m_codecs.size() - 1;
		return true;
	}
	///////////////////////////////////////////////////////////////////////////
	UIO_Plugin::TCodecId CodecSystemBase::findCodec( const TCodecFormat& _format )
	{
		TMapCodecFormats::iterator it_find = m_codecFormats.find( _format );

		if( it_find == m_codecFormats.end() )
		{
			return AEIO_USER_PLUGIN_INVALID_ID;
		}

		return it_find->second;
	}
	///////////////////////////////////////////////////////////////////////////
	Codec * CodecSystemBase::getCodec( TCodecId _codecId )
	{
		if( _codecId < 0 || _codecId >= m_codecs.size() )
		{
			return NULL;
		}

		return m_codecs[_codecId];
	}
	///////////////////////////////////////////////////////////////////////////
	bool CodecSystemBase::init()
	{
		return true;
	}
	///////////////////////////////////////////////////////////////////////////
	void CodecSystemBase::destroy()
	{
		for(TVectorCodec::iterator it = m_codecs.begin();
			it != m_codecs.end();
			++it )
		{
			(*it)->destroy();
		}

		m_codecs.clear();
		m_codecFormats.clear();

		delete this;
	}
	///////////////////////////////////////////////////////////////////////////
}