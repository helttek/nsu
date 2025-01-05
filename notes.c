mythread_create(mythread_t *thread, void *(*start_routine)(void *), void *arg){
	mythread_t mythread=new_mythread();
	create_stack();
	clone(thread_startup, mythread);
	*thread=mythread;
}

typedef void *(*start_routine)(void*)
struct mythread_t{
	//volatile if needed
	start_routine start_routine;
	void *arg;
	int pid;
	int mythread_id;
	int bp;//base pointer
	int sp;//stack pointer
}

stack_restore(){
	
}

int thread_startup(void * arg){
	mythread_t methread=(mythread_t) arg;
	//init
	methread->start_routine(mythread->arg);
	//finalize
	wait_till_join();//thread finished, wait for join
}

//---------------------------------------------------------------------------------------

/*
look up getcontext, swapcontext, set and make context functions in libc source

schedule look up settimer()


*/


-- почему  нельзя в очереди использовать detached поток, чтобы небыло join?
-- что в htop значат зеленые и красные цвета


-- почитать, почему плохо юзать один конд вар - во-первых, просыпаются оба потока (не детрминировано, какой
 из них начнет работать)
-- можно ли поменять вэйт и лок в семафоре - no, there will be a deadlock: лок мьютекса всегда будет 
опережать разлок кондвара (сперва лочится мьютекс в одном из потоков происходит заход в сем_вэйт,
 слип, переход в другой поток, сем_вэйт, попытка блокировки мьютекса - дедлок)
-- чем отличается бинарная семафора от мутекса - в семафоре нет ownership - кто угодно может сделать лок или анлок,
в обычнос мьютексе же анлок может быть сделан только тредом, обладающим этим мьютексом
-- как работает конд вэйт - есть 2 группы (готовы принимать сигнал и скоро будут 
принимать сигнал), обе реализованы на фьютексе, лок и анлок мьютекса не обычный, а:
Instead of the normal mutex unlock / lock functions, we use
__pthread_mutex_unlock_usercnt(m, 0) / __pthread_mutex_cond_lock(m)
because those will not change the mutex-internal users count, so that it
can be detected when a condvar is still associated with a particular
mutex because there is a waiter blocked on this condvar using this mutex.
см. закладку в браузере

