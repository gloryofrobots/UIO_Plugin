#pragma  once 

namespace UIO_Plugin
{
	class System
	{
	public:
		virtual bool init() = 0;
		virtual void destroy() = 0;
	};
};