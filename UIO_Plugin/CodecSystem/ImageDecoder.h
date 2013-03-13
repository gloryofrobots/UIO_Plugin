#pragma once 
#include "DecoderBase.h"

namespace UIO_Plugin
{
	struct ImageFormatInfo
	{	
		ImageFormatInfo()
			: height(0)
			, width(0)
			, channels(0)
		{
		}

		size_t height;
		size_t width;
		int channels;
	};

	class ImageDecoder : public DecoderBase
	{
	public:
		ImageDecoder();
		virtual ~ImageDecoder();

	protected:
		ImageFormatInfo m_formatInfo;
	};
}