#pragma once 
#include "Codec.h"

namespace UIO_Plugin
{
	class CodecBase : public Codec
	{
	public:
		CodecBase( );
		~CodecBase();

		bool initialise( FileSystem * _fileSystem, LogSystem* logSystem ) override;

		void destroy() override;
		DecoderSystem * getDecoderSystem();		
	protected:
		virtual bool _initialise( FileSystem * _fileSystem, LogSystem* logSystem ) = 0;
		DecoderSystem * m_decoderSystem;
	};

	template <class TypeDecoderSystem>
	class CodecWithDecoder : public CodecBase
	{		
	protected: 
		bool _initialise( FileSystem * _fileSystem, LogSystem* logSystem )
		{
			m_decoderSystem = new  TypeDecoderSystem( _fileSystem, logSystem );
			return true;
		}
	};

}