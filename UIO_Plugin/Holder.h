# pragma once

namespace UIO_Plugin
{
	template<class T>
	class Holder
	{
	public:
		Holder()
		{
		}

		static void keep(T * _hostage)
		{
			if(m_hostage != 0 )
			{
				throw "MP_Holder::already hosted";
			}
			m_hostage = _hostage;
		}

		static T * get()
		{
			return m_hostage;
		}

		static bool empty()
		{
			return m_hostage == 0;
		}

		static void clear()
		{
			m_hostage = 0;
		}

	protected:
		static T * m_hostage;
	};

	template<class T>
	T * Holder<T>::m_hostage = 0;
}
