#	pragma once

#	include "CodecSystem/ImageDecoder.h"
#	include "webp/decode.h"
//#	include "webp/mux.h"

namespace UIO_Plugin
{
	class ImageDecoderWEBP
		: public ImageDecoder
	{
	public:
		ImageDecoderWEBP();
		virtual ~ImageDecoderWEBP();
		void writeHeader( UIO_Plugin_Header * _header ) override;
		
	protected:
		bool _initialize() override;
		bool _decode( unsigned char* _buffer, unsigned int _bufferSize ) override;
	
	private:
		bool _decodeScale( unsigned char* _buffer, unsigned int _bufferSize );
		bool _decodeFull( unsigned char* _buffer, unsigned int _bufferSize );
	protected:
		uint8_t* m_dataBuffer;
		size_t m_dataSize;
		WebPDecoderConfig m_decoderConfig;
	};
}	
