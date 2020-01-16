#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#define PSIZE 1000000
static pthread_mutex_t q_mutex=PTHREAD_MUTEX_INITIALIZER;
static int q;
static int max_q_size;
static int q_underflows;
static void produce(){
    pthread_mutex_lock(&q_mutex);
    ++q;
    if(q>max_q_size){
        max_q_size=q;
    }
    pthread_mutex_unlock(&q_mutex);
}
static void consume(){
    pthread_mutex_lock(&q_mutex);
    --q;
    if(q<0){
        q_underflows++;
    }
    pthread_mutex_unlock(&q_mutex);
}

static int fildes[2];
static void *p1(void *arg){
    char dummy='x';
    for(int i=0; i<PSIZE; ++i){
        produce();
        if(write(fildes[1],&dummy,1)<0){
            printf("fildes write error: %m\n");
            break;
        }
    }
    return 0;
}
static void *c1(void *arg){
    char dummy;
    for(int i=0; i<PSIZE; ++i){
        if(read(fildes[0], &dummy, 1)<0){
            printf("fildes read error: %m\n");
            break;
        }
        consume();
    }
    return 0;
}

static void reset_q(){
    q=0;
    max_q_size=0;
    q_underflows=0;
}


struct timespec diff(struct timespec start, struct timespec end){
    struct timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

static struct timespec time_threads(void *(t1)(void *),
                             void *(t2)(void *)){
    pthread_t tid[2];
    struct timespec start;
    reset_q();
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    pthread_create(&tid[0],0,t1,0);
    pthread_create(&tid[1],0,t2,0);
    pthread_join(tid[0],0);
    pthread_join(tid[1],0);
    struct timespec end;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    return diff(start,end);
}


static pthread_mutex_t p2_mutex=PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t p2_cond;

static void *p2(void *arg){
    for(int i=0; i<PSIZE; ++i){
        pthread_mutex_lock(&p2_mutex);
        produce();
        pthread_cond_signal(&p2_cond);
        pthread_mutex_unlock(&p2_mutex);
    }
    return 0;
}
static void *c2(void *arg){
    for(int i=0; i<PSIZE; ++i){
        pthread_mutex_lock(&p2_mutex);
        while(q==0){
            pthread_cond_wait(&p2_cond, &p2_mutex);
        }
        consume();
        pthread_mutex_unlock(&p2_mutex);
    }
    return 0;
}

sem_t p3_sem;
static void *p3(void *arg){
    for(int i=0; i<PSIZE; ++i){
        produce();
        sem_post(&p3_sem);
    }
    return 0;
}
static void *c3(void *arg){
    for(int i=0; i<PSIZE; ++i){
        sem_wait(&p3_sem);
        consume();
    }
    return 0;
}


int main(){
    pipe(fildes);
    struct timespec t1 = time_threads(p1,c1);
    printf("Pipe. Q = %d, max_q_size %d, underflows %d\n",
           q,max_q_size, q_underflows);
    printf("Time: %ld S:%ld nS\n",t1.tv_sec, t1.tv_nsec);

    
    pthread_cond_init(&p2_cond,0);
    t1=time_threads(p2,c2);
    pthread_cond_destroy(&p2_cond);
    printf("Cond var. Q = %d, max_q_size %d, underflows %d\n",
           q,max_q_size, q_underflows);
    printf("Time: %ld S:%ld nS\n",t1.tv_sec, t1.tv_nsec);

    sem_init(&p3_sem,0,0);
    t1=time_threads(p3,c3);
    sem_destroy(&p3_sem);
    printf("Counting sem. Q = %d, max_q_size %d, underflows %d\n",
           q,max_q_size, q_underflows);
    printf("Time: %ld S:%ld nS\n",t1.tv_sec, t1.tv_nsec);
    
    return 0;
}
