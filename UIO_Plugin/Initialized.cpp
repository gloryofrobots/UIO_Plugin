#include "Initialized.h"

namespace UIO_Plugin
{
	///////////////////////////////////////
	bool Initialized::init()
	{
		return this->onInit();
	}
	///////////////////////////////////////
	void Initialized::destroy()
	{
		this->onDestroy();
		delete this;
	}
	///////////////////////////////////////
}
