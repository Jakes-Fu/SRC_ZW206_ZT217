#include "port_cfg.h"

#ifdef REAL_WATCH_RTOS

#include "sci_types.h"
#include "os_api.h"

// Very low efficiency
void* os_realloc_impl(void* ptr, size_t newSize)
{
	char* newPtr = NULL;
	size_t oldSize = 0;

	if (newSize == 0) {
		return NULL;
	}

	if (NULL == ptr) {
		newPtr = SCI_ALLOC_APP(newSize);
	}
	else {
		oldSize = SCI_MEMSIZE(ptr);

		SCI_ASSERT(oldSize > 0);
		SCI_ASSERT(newSize >= oldSize);
		newPtr = (char*)SCI_ALLOC_APP(newSize);
		SCI_ASSERT(NULL != newPtr);
		SCI_MEMSET(newPtr, 0, newSize);
		SCI_MEMCPY(newPtr, ptr, oldSize);
		SCI_FREE(ptr);
	}
	return (void *)newPtr;
}

void* os_calloc_impl(size_t num, size_t size)
{
	if(num == 0 || size == 0)
		return NULL;
	else {
		int total = num * size;
		void *ptr = os_malloc(total);
	//	if (ptr)
	//		os_memset(ptr, 0, total);
		return ptr;		
	}
}
#endif