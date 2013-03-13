#pragma once 

#include <stdio.h>
#include "../Stream/InputStream.h"
#include "../Stream/OutputStream.h"
#include "../Config.h"

namespace UIO_Plugin
{
	class FileStreamCRT
	{
	public:
		FileStreamCRT( const String& _fileName );
		virtual ~FileStreamCRT();

	protected:
		String m_fileName;
		FILE * m_file;
	};
	
	class InputFileStreamCRT: public InputStream, public FileStreamCRT
	{
	public:
		InputFileStreamCRT( const String& _fileName );
		~InputFileStreamCRT();

	public:
		int read( char* _buf, int _count ) override;
		void seek( int _pos ) override;
		int tell() override;


		int size() override;
		//bool time( time_t & _time ) override;	
		
		bool open() override;
		bool isOpen() override;
		//FIX ME!!!!!!!!!!!!!!!!!!!!!!!
		void close() override;
		void destroy() override;
	};

	class OutputFileStreamCRT: public FileStreamCRT, public OutputStream
	{
	public:
		OutputFileStreamCRT( const String& _fileName );
		~OutputFileStreamCRT();
	public:
		virtual void write( const char* _data, int _count ) override;
		virtual void flush() override;

		bool isOpen() override;
		bool open() override;

		void close() override;
		void destroy() override;
	};
}