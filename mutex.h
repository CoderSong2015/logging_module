#ifndef TRAF_MUTEX_H_
#define TRAF_MUTEX_H_


//Here we only consider the environment of linux
#include <pthread.h>
#include <stdlib.h>
#include <iostream>
//can modify different mutex type here
//like
//    CRITICAL_SECTION
//    pthread_rwlock_t
//    or int when no thread will use
typedef pthread_mutex_t Mutex_Type;


namespace {

class Mutex{
 public:
    inline Mutex();
    inline ~Mutex();
    inline void Lock();
    inline void Unlock();

 private:
    Mutex_Type mutex_;

};

#define FUNC_PTHREAD(fncall)  do { \
if(fncall(&mutex_) != 0) abort(); \
}while(0)

Mutex::Mutex() {
    if(pthread_mutex_init(&mutex_, NULL) != 0) {
        abort();
    }
    std::cout << "init";
}
Mutex::~Mutex() { FUNC_PTHREAD(pthread_mutex_destroy); }
void Mutex::Lock() { FUNC_PTHREAD(pthread_mutex_lock); }
void Mutex::Unlock() { FUNC_PTHREAD(pthread_mutex_unlock); }

//RAII alloc the mutex on the stack
class MutexLock{
 public:
    explicit MutexLock(Mutex* mu):mutex_(mu){ mutex_->Lock(); }
    ~MutexLock(){ mutex_->Unlock(); }
 private:
    Mutex * const mutex_;
};

// Catch bug where variable name is omitted, e.g. MutexLock (&mu);
#define MutexLock(x) COMPILE_ASSERT(0, mutex_lock_decl_missing_var_name)

}
#endif TRAF_MUTEX_H_
