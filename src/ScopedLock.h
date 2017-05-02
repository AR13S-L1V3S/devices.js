#ifndef _H_SCOPEDLOCK_
#define _H_SCOPEDLOCK_

class ScopedLock
{
public:
	ScopedLock(uv_mutex_t& mutex) :
		mutex(mutex)
	{
		uv_mutex_lock(&mutex);
	}
	~ScopedLock(void)
	{
		uv_mutex_unlock(&mutex);
	}

private:
	uv_mutex_t& mutex;
};

#endif
