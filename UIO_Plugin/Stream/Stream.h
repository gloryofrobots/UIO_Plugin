#pragma once 

namespace UIO_Plugin
{
	class Stream
	{
	public:
		virtual bool open() = 0;
		virtual bool isOpen() = 0;

		virtual void close() = 0;	
		virtual void destroy() = 0;
	};
}