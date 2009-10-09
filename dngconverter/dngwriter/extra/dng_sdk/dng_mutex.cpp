/*****************************************************************************/
// Copyright 2006-2008 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

/* $Id: //mondo/dng_sdk_1_2/dng_sdk/source/dng_mutex.cpp#1 $ */ 
/* $DateTime: 2008/03/09 14:29:54 $ */
/* $Change: 431850 $ */
/* $Author: tknoll $ */

#include "dng_mutex.h"
#include "dng_assertions.h"
#include "dng_exceptions.h"

#include <stdlib.h>

/*****************************************************************************/

#if qDNGThreadSafe

static pthread_once_t gInnermostMutexInited = PTHREAD_ONCE_INIT;

static pthread_key_t gInnermostMutexKey;

static void InitInnermostMutex ()
	{
	
	int result = pthread_key_create (&gInnermostMutexKey, NULL);

	DNG_ASSERT (result == 0, "pthread_key_create failed.");

	if (result != 0)
		ThrowProgramError ();

	}

static void SetInnermostMutex (dng_mutex *mutex)
	{

	int result;

	result = pthread_setspecific (gInnermostMutexKey, (void *)mutex);

	DNG_ASSERT (result == 0, "pthread_setspecific failed.");

	if (result != 0)
		ThrowProgramError ();

	}

static dng_mutex *GetInnermostMutex ()
	{

	void *result = pthread_getspecific (gInnermostMutexKey);

	return reinterpret_cast<dng_mutex *> (result);

	}

#endif

/*****************************************************************************/

dng_mutex::dng_mutex (const char *mutexName, uint32 mutexLevel)

	#if qDNGThreadSafe
	
	:	fPthreadMutex		()
	,	fMutexLevel			(mutexLevel)
	,	fRecursiveLockCount (0)
	,	fPrevHeldMutex		(NULL)
	,	fMutexName			(mutexName)
	
	#endif
	
	{
	
	#if qDNGThreadSafe
	
	pthread_once (&gInnermostMutexInited, InitInnermostMutex);

	if (pthread_mutex_init (&fPthreadMutex, NULL) != 0)
		{
		ThrowMemoryFull ();
		}
	
	#endif
	
	}

/*****************************************************************************/

dng_mutex::~dng_mutex ()
	{
	
	#if qDNGThreadSafe

	pthread_mutex_destroy (&fPthreadMutex);

	#endif
	
	}

/*****************************************************************************/

void dng_mutex::Lock ()
	{
	
	#if qDNGThreadSafe

	dng_mutex *innermostMutex = GetInnermostMutex ();

	if (innermostMutex != NULL)
		{

		if (innermostMutex == this)
			{

			fRecursiveLockCount++;

			return;

			}

		bool lockOrderPreserved = fMutexLevel > innermostMutex->fMutexLevel /* ||
								  (fMutexLevel == innermostMutex->fMutexLevel && innermostMutex < this) */;

		if (!lockOrderPreserved)
			{
			
			DNG_REPORT ("Lock ordering violation.");
			
			#if qDNGDebug
			
			dng_show_message_f ("This mutex: %s v Innermost mutex: %s",
								this->MutexName (),
								innermostMutex->MutexName ());
			
			#endif

			}

		}

	pthread_mutex_lock (&fPthreadMutex);

	fPrevHeldMutex = innermostMutex;

	SetInnermostMutex (this);

	#endif
	
	}

/*****************************************************************************/

void dng_mutex::Unlock ()
	{
	
	#if qDNGThreadSafe
	
	DNG_ASSERT (GetInnermostMutex () == this, "Mutexes unlocked out of order!!!");

	if (fRecursiveLockCount > 0)
		{

		fRecursiveLockCount--;

		return;

		}

	SetInnermostMutex (fPrevHeldMutex);

	fPrevHeldMutex = NULL;

	pthread_mutex_unlock (&fPthreadMutex);
	
	#endif
	
	}

/*****************************************************************************/

const char *dng_mutex::MutexName () const
	{
	
	#if qDNGThreadSafe
	
	if (fMutexName)
		return fMutexName;
	
	#endif
	
	return "< unknown >";
	
	}

/*****************************************************************************/

dng_lock_mutex::dng_lock_mutex (dng_mutex *mutex)

	:	fMutex (mutex)
	
	{
	
	if (fMutex)
		fMutex->Lock ();
		
	}

/*****************************************************************************/

dng_lock_mutex::~dng_lock_mutex ()
	{
	
	if (fMutex)
		fMutex->Unlock ();
		
	}

/*****************************************************************************/

#if qDNGThreadSafe

/*****************************************************************************/

dng_condition::dng_condition ()

	:	fPthreadCondition ()

	{

	int result;

	result = pthread_cond_init (&fPthreadCondition, NULL);

	DNG_ASSERT (result == 0, "pthread_cond_init failed.");

	if (result != 0)
		{
		ThrowProgramError ();
		}

	}

/*****************************************************************************/

dng_condition::~dng_condition ()
	{
	
	pthread_cond_destroy (&fPthreadCondition);
	
	}

/*****************************************************************************/

bool dng_condition::Wait (dng_mutex &mutex, double timeoutSecs)
	{

	bool timedOut = false;

	dng_mutex *innermostMutex = GetInnermostMutex ();

	DNG_ASSERT (innermostMutex == &mutex, "Attempt to wait on non-innermost mutex.");

	innermostMutex = mutex.fPrevHeldMutex;

	SetInnermostMutex (innermostMutex);

	mutex.fPrevHeldMutex = NULL;

	if (timeoutSecs < 0)
		{
		
		pthread_cond_wait (&fPthreadCondition, &mutex.fPthreadMutex);
		
		}
		
	else
		{
		
		struct timespec now;

		dng_pthread_now (&now);

		timeoutSecs += now.tv_sec;
		timeoutSecs += now.tv_nsec / 1000000000.0;

		now.tv_sec  = (long) timeoutSecs;
		now.tv_nsec = (long) ((timeoutSecs - now.tv_sec) * 1000000000);

		timedOut = pthread_cond_timedwait (&fPthreadCondition, &mutex.fPthreadMutex, &now) == ETIMEDOUT;
		
		}

	mutex.fPrevHeldMutex = innermostMutex;

	SetInnermostMutex (&mutex);

	return !timedOut;

	}

/*****************************************************************************/

void dng_condition::Signal ()
	{

	int result;

	result = pthread_cond_signal (&fPthreadCondition);

	DNG_ASSERT (result == 0, "pthread_cond_signal failed.");

	if (result != 0)
		ThrowProgramError ();

	}

/*****************************************************************************/

void dng_condition::Broadcast ()
	{

	int result;

	result = pthread_cond_broadcast (&fPthreadCondition);

	DNG_ASSERT (result == 0, "pthread_cond_broadcast failed.");

	if (result != 0)
		ThrowProgramError ();

	}

/*****************************************************************************/

#endif // qDNGThreadSafe

/*****************************************************************************/
