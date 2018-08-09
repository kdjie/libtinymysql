//============================================================================
// Name        : Specific.h
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2012
// Description : 14166097@qq.com
//============================================================================

#pragma once

#include <pthread.h>

namespace tinymysql
{
	template <typename T>
	class TTSD
	{
	public:
		TTSD()
		{
			__createKey();
		}
		virtual ~TTSD()
		{
			__deleteKey();
		}

		pthread_key_t& Handle()
		{
			return m_key;
		}

		T* get()
		{
			return __getKey();
		}

		T* release()
		{ 
			T* pCur = __getKey();

			if (pCur)
				__setKey(NULL);

			return pCur;
		}

		void reset(T* p = NULL)
		{
			T* pCur = __getKey();

			if (pCur == p)
				return;

			__setKey(p);

			if (pCur)
				delete pCur;
		}

	protected:
		TTSD( const TTSD& );
		TTSD& operator=( const TTSD& );

	private:
		void __createKey()
		{
			pthread_key_create(&m_key, __cleanup);
		}
		void __deleteKey()
		{
			pthread_key_delete(m_key);
		}

		static void __cleanup(void* p)
		{
			if (p)
			{
			    delete (T*)p;
			}
		}

		T* __getKey() const
		{
			return (T*)pthread_getspecific(m_key);
		}
		void __setKey(T* p)
		{
			pthread_setspecific(m_key, p);
		}

	private:
		pthread_key_t m_key;
	};
}
