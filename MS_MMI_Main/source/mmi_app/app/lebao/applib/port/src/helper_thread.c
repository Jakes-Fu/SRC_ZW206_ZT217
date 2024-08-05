#include "port_cfg.h"

#ifdef REAL_WATCH_RTOS

#include "helper_thread.h"

typedef struct thread_proc_port_t {
	int(*callback)(void * param);
	void* data;
} thread_proc_port_t;

thread_mutex_t* g_appMutex = INVALID_MUTEX_ID;

thread_id_t p_thread_current_thread_id(void)
{
	return SCI_IdentifyThread();
}

void p_thread_yield(void)
{
	// do nothing	
}

void p_thread_exit(int return_code)
{
	thread_id_t id = SCI_IdentifyThread();
	SCI_TerminateThread(id);
}

static void thread_proc_port(uint32 param, void* data)
{
	thread_proc_port_t* proc = (thread_proc_port_t *)data;
	proc->callback(proc->data);
	os_free(proc);
}

thread_ptr_t p_thread_create(int(*thread_proc)(void*), void* user_data, char const* name, int stack_size)
{
	thread_id_t id = SCI_INVALID_BLOCK_ID;
	thread_proc_port_t* proc = os_malloc(sizeof(thread_proc_port_t));

	proc->callback = thread_proc;
	proc->data = user_data;

	id = SCI_CreateThread(name,
		name,
		thread_proc_port,
		0,
		proc,
		stack_size,
		20, // queue_num
		TX_SYSTEM_NORMALE,
		SCI_PREEMPT,
		SCI_AUTO_START);

	helper_debug_printf("thread <- 0x%x\n", id);

	// SCI_ERROR == SCI_IsThreadExist(id)
	if (id == NULL || id == SCI_INVALID_BLOCK_ID) {
		os_free(proc);
		return 0;
	}

	return id;
}

void p_thread_destroy(thread_ptr_t thread)
{
	helper_debug_printf("thread -> 0x%x\n", thread);
	SCI_DeleteThread(thread);
}

int p_thread_join(thread_ptr_t thread)
{
	// do nothing
	return 0;
}

void p_thread_set_high_priority(void)
{
  int p = 0;
	thread_id_t id = SCI_IdentifyThread();
	SCI_ChangeThreadPriority(id, TX_SYSTEM_HIGH, &p);
}

void thread_mutex_init(thread_mutex_t** mutex)
{
	*mutex = SCI_CreateMutex("helper-mutex", SCI_INHERIT);
	helper_debug_printf("*mutex <- %p\n", *mutex);
}

void thread_mutex_term(thread_mutex_t* mutex)
{
	helper_debug_printf("*mutex -> %p\n", mutex);

	if (mutex == NULL) return;
	SCI_DeleteMutex(mutex);
}

void thread_mutex_lock(thread_mutex_t* mutex)
{
	if (mutex == NULL) return;
	SCI_GetMutex(mutex, SCI_WAIT_FOREVER); //SCI_NO_WAIT
}

void thread_mutex_unlock(thread_mutex_t* mutex)
{
	if (mutex == NULL) return;
	SCI_PutMutex(mutex);
}

//int volatile* var
int thread_atomic_int_load(thread_atomic_int_t* atomic)
{
	int ret = 0;

	thread_mutex_lock(g_appMutex);
	ret = (*atomic);
	thread_mutex_unlock(g_appMutex);

	return ret;
}

void thread_atomic_int_store(thread_atomic_int_t* atomic, int desired)
{
	thread_mutex_lock(g_appMutex);
	(*atomic) = desired;
	thread_mutex_unlock(g_appMutex);
}

int thread_atomic_int_inc(thread_atomic_int_t* atomic)
{
	int ret = 0;

	thread_mutex_lock(g_appMutex);
	ret = ++(*atomic);
	thread_mutex_unlock(g_appMutex);

	return ret;
}

int thread_atomic_int_dec(thread_atomic_int_t* atomic)
{
	int ret = 0;

	thread_mutex_lock(g_appMutex);
	ret = --(*atomic);
	thread_mutex_unlock(g_appMutex);

	return ret;
}

int thread_atomic_int_add(thread_atomic_int_t* atomic, int value)
{
	int ret = 0;

	thread_mutex_lock(g_appMutex);
	*atomic += value;
	ret = (*atomic);
	thread_mutex_unlock(g_appMutex);

	return ret;
}

int thread_atomic_int_sub(thread_atomic_int_t* atomic, int value)
{
	int ret = 0;

	thread_mutex_lock(g_appMutex);
	*atomic -= value;
	ret = (*atomic);
	thread_mutex_unlock(g_appMutex);

	return ret;
}

#endif