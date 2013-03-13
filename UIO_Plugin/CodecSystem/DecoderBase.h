#pragma once 
#include "Decoder.h"

namespace UIO_Plugin
{
	class DecoderBase : public Decoder
	{
	public:
		DecoderBase();
		virtual ~DecoderBase();

		bool initialize( InputStream * _stream, LogSystem * _logSystem ) override;
		virtual void destroy() override;

		bool decode( unsigned char* _buffer, unsigned int _bufferSize ) override;
		InputStream * getStream() const override;
		void setOptions( FrameOptions * _options ) override;
	protected:
		virtual bool _initialize() = 0;
		virtual bool _decode( unsigned char* _buffer, unsigned int _bufferSize ) = 0;
		FrameOptions m_options;
		LogSystem * m_logSystem;
		InputStream * m_stream;
	};
}