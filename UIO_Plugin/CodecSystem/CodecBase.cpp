#	include "CodecBase.h"

namespace UIO_Plugin
{
	CodecBase::CodecBase( )
		: Codec()
		, m_decoderSystem(NULL)
	{

	}
	/////////////////////////////////////////////////////
	CodecBase::~CodecBase()
	{
	}
	/////////////////////////////////////////////////////
	bool CodecBase::initialise( FileSystem * _fileSystem, LogSystem* _logSystem )
	{
		return this->_initialise( _fileSystem, _logSystem );
	}
	/////////////////////////////////////////////////////
	DecoderSystem * CodecBase::getDecoderSystem()
	{
		return m_decoderSystem;
	}
	/////////////////////////////////////////////////////
	void CodecBase::destroy()
	{
		if( m_decoderSystem != NULL )
		{
			m_decoderSystem->destroy();
			m_decoderSystem = NULL;
		}

		delete this;
	}
	/////////////////////////////////////////////////////
}
