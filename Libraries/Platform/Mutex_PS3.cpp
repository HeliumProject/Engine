#include "Mutex.h"
#include "Platform.h"

#include "Common/Assert.h"

using namespace Platform;

Mutex::Mutex()
{
  pthread_mutex_init(&m_Handle, NULL);
}

Mutex::~Mutex()
{
  pthread_mutex_destroy(&m_Handle);
}

void Mutex::Lock()
{
  int result = pthread_mutex_lock(&m_Handle);
  NOC_ASSERT( result == 0 );
}

void Mutex::Unlock()
{
  int result = pthread_mutex_unlock(&m_Handle);
  NOC_ASSERT( result == 0 );
}
