#pragma once
#include "FileSystem.h"

namespace UIO_Plugin
{
	template <class TypeInputStream, class TypeOutputStream>
	class FileSystem_T : public FileSystem
	{
	public: 
		bool init() override
		{
			return true;
		}
		
		void destroy() override
		{
			delete this;
		}

		InputStream * openInputStream( const String& _fileName ) override
		{
			return this->openStream_<TypeInputStream>(_fileName);
		}

		OutputStream * openOutputStream( const String& _fileName ) override
		{
			return this->openStream_<TypeOutputStream>(_fileName);
		}

		bool destroyStream( InputStream * _stream ) override
		{	
			_stream->destroy();
			return true;
			//return this->destroyStream_<TypeInputStream>(_stream);
		}

		bool destroyStream( OutputStream * _stream ) override
		{
			_stream->destroy();
			return true;
			//return this->destroyStream_<TypeOutputStream>(_stream);
		}

	private:
		template <class T>
		T* openStream_( const String& _fileName )
		{
			T * stream = new T(_fileName);
			if( stream->open() == false )
			{
				stream->destroy();
				return NULL;
			}

			return stream;
		}

		template <class T>
		bool destroyStream_( T* _stream )
		{
			_stream->close();
			_stream->destroy(); 
			return true;
		}
	};
}