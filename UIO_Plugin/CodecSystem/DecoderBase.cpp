#include "DecoderBase.h"

namespace UIO_Plugin
{
	//////////////////////////////////////////////////////////////////////////
	DecoderBase::DecoderBase()
	{	
		memset( &m_options, 0, sizeof(m_options) );
	}
	//////////////////////////////////////////////////////////////////////////
	DecoderBase::~DecoderBase()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool DecoderBase::initialize( InputStream * _stream, LogSystem * _logSystem )
	{
		m_logSystem = _logSystem;
		m_stream = _stream;
		return this->_initialize();
	}
	//////////////////////////////////////////////////////////////////////////
	bool DecoderBase::decode( unsigned char* _buffer, unsigned int _bufferSize )
	{
		return this->_decode( _buffer, _bufferSize );
	}
	//////////////////////////////////////////////////////////////////////////
	InputStream * DecoderBase::getStream() const
	{
		return m_stream;
	}
	////////////////////////////////////////////////////////////////////////////
	//const FormatInfo& DecoderBase::getFormatInfo() const
	//{
	//	return m_formatInfo;
	//}
	//////////////////////////////////////////////////////////////////////////
	void DecoderBase::setOptions( FrameOptions * _opt )
	{
		m_options = *(_opt);
	}
	//////////////////////////////////////////////////////////////////////////
	void DecoderBase::destroy()
	{
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////

}
