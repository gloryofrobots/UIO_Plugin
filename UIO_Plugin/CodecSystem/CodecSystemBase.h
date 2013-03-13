#pragma once 
#include "CodecSystem.h"
#include <map>
#include <vector>

namespace UIO_Plugin
{
	class CodecSystemBase : public CodecSystem
	{
	public:
		bool init() override;
		void destroy() override;

		bool registerCodec( Codec * _codec, const TCodecFormat&  _format, FileSystem * _fileSystem, LogSystem* _logSystem ) override;
		TCodecId findCodec( const TCodecFormat&  _format ) override;
		Codec * getCodec( TCodecId _codecId ) override;

	protected:
		typedef std::vector<Codec *> TVectorCodec;
		TVectorCodec m_codecs;

		typedef std::map<TCodecFormat, TCodecId> TMapCodecFormats;
		TMapCodecFormats m_codecFormats;
	};
}