namespace UIO_Plugin
{
	class Initialized
	{
	public:
		bool init();
		void destroy();

	protected:
		virtual bool onInit() = 0;
		virtual void onDestroy() = 0;
	};
};