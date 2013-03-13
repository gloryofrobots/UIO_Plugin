#include "ImageDecoderWEBP.h"
#include <stdio.h>

#define HEADER_BYTES_TO_CHECK 160
#define LOG(msg) printf(msg)

namespace UIO_Plugin
{
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderWEBP::ImageDecoderWEBP( )
		: ImageDecoder()
		, m_dataBuffer(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderWEBP::~ImageDecoderWEBP()
	{
		if( m_dataBuffer != NULL )
		{
			delete [] m_dataBuffer;
			m_dataBuffer = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderWEBP::_initialize()
	{
		if( WebPInitDecoderConfig(&m_decoderConfig) == 0 )
		{
			return false;
		}

		//We must write all data into buffer in initialise because WebP API does not support custom read callbacks
		m_stream->seek( 0 );
		m_dataSize = m_stream->size();
		m_dataBuffer = new uint8_t [m_dataSize];
		m_stream->read( (char *)m_dataBuffer, m_dataSize );
		
		VP8StatusCode status = VP8_STATUS_OK;
		
		unsigned char* checkHeaderBuffer = m_dataBuffer;

		if ( WebPGetFeatures(checkHeaderBuffer, HEADER_BYTES_TO_CHECK, &m_decoderConfig.input) != VP8_STATUS_OK )
		{
			m_logSystem->log ("ImageDecoderWEBP : Bad or not WEBP file. can`t init features ");
			return false;
		}

		m_formatInfo.width = m_decoderConfig.input.width;
		m_formatInfo.height = m_decoderConfig.input.height;

		if( m_decoderConfig.input.has_alpha == false )
		{	
			m_formatInfo.channels = 3;
		}
		else
		{
			m_formatInfo.channels = 4;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderWEBP::_decode( unsigned char* _buffer, unsigned int _bufferSize )
	{	
		if( m_options.scale == true )
		{
			return this->_decodeScale( _buffer, _bufferSize );
		}
		else
		{
			return this->_decodeFull( _buffer, _bufferSize );
		}
	}
	/////////////////////////////////////////////////////////////////////////
	bool ImageDecoderWEBP::_decodeScale( unsigned char* _buffer, unsigned int _bufferSize )
	{
		m_decoderConfig.options.no_fancy_upsampling = 1;
		m_decoderConfig.options.use_scaling = 1;
		m_decoderConfig.options.scaled_width = m_options.width;
		m_decoderConfig.options.scaled_height = m_options.height;
		
		// Specify the desired output colorspace:
		m_decoderConfig.output.colorspace = MODE_ARGB;
		// Have config.output point to an external buffer:
		m_decoderConfig.output.u.RGBA.rgba = (uint8_t*)_buffer;
		m_decoderConfig.output.u.RGBA.stride = m_options.pitch;
		m_decoderConfig.output.u.RGBA.size = _bufferSize;
		m_decoderConfig.output.is_external_memory = 1;

		if(  WebPDecode( m_dataBuffer, m_dataSize, &m_decoderConfig) != VP8_STATUS_OK )
		{
			return false;
		}

		WebPFreeDecBuffer(&m_decoderConfig.output);

		return true;		
	}
	/////////////////////////////////////////////////////////////////////////
	bool ImageDecoderWEBP::_decodeFull( unsigned char* _buffer, unsigned int _bufferSize )
	{
		if( WebPDecodeARGBInto( m_dataBuffer, m_dataSize, _buffer, _bufferSize, m_options.pitch ) == NULL )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoderWEBP::writeHeader( UIO_Plugin_Header *  _header )
	{
		_header->hasVideo			=	TRUE;
		_header->heightLu			=	m_formatInfo.height;
		_header->widthLu			=	m_formatInfo.width;
		_header->bitdepthS			=	m_formatInfo.channels * 8;
		_header->hasAudio			=	FALSE;
		_header->fpsT.value = 0;
		_header->fpsT.scale = 0;
		_header->durationT.value	=	0;
		_header->durationT.scale	=	1;
	}
	//////////////////////////////////////////////////////////////////////////
}	
