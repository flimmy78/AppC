#include "threadpool.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


ThreadPoolTask* TP_CreateThreadPoolTask()
{
    ThreadPoolTask *task;

    task = TP_MALLOC(ThreadPoolTask);
    if(0 != task){
        memset(task,0,sizeof(ThreadPoolTask));
        task->m_needFree = 0;
        task->m_run = 0;
        task->m_arge = 0;
        task->m_next = 0;
    }
    return task;
}

void TP_DeleteThreadPoolTask(ThreadPoolTask* task)
{
    if(0 == task) return;
    if(task->m_needFree)
    TP_FREE(task->m_arge);
    TP_FREE(task);
}

void TP_SetTaskArg(ThreadPoolTask* task, void* arge, int size)
{
    if(0==task) return;
    if(1==task->m_needFree){TP_FREE(task->m_arge);}

    if(0!=arge){
        if(size>0){
            task->m_arge = (void*)malloc(size);
            memcpy(task->m_arge,arge,size);
            task->m_needFree = 1;
        }else{
            task->m_arge = arge;
            task->m_needFree = 0;
        }
    }
}


/*pool*/
// 创建线程池
ThreadPool* TP_CreateThreadPool(int maxThrNum)
{
    ThreadPool* pool;
    pool = TP_MALLOC(ThreadPool);
    if(0!=pool){
        memset(pool,0,sizeof(ThreadPool));
        pthread_mutex_init(&pool->m_taskLock, 0);
        pthread_cond_init (&pool->m_taskReady, 0);

        pool->m_waitForDestroy = 0;
        pool->m_taskHeader = 0;
        /*thr*/
        pool->m_maxThrNum = (maxThrNum>0)?maxThrNum:10;// 最多10个线程
        pool->m_thrGroup = (pthread_t*)malloc(sizeof(pthread_t)*pool->m_maxThrNum);// 开辟一个线程池，里面包含了m_maxThrNum个线程，并把当前线程指针指向该线程池空间。
        if(0==pool->m_thrGroup){
            TP_FREE(pool);
        }else{
            /*create thread*/
            int i;
            for(i=0;i<pool->m_maxThrNum;i++){
                pthread_create(pool->m_thrGroup+i,0,(TP_FUN)(TP_Process),(void*)pool);// 创建m_maxThrNum个线程。
            }
        }
    }
    return pool;
}

void TP_DeleteThreadPool(ThreadPool *pool)
{
    int i;
    if(0==pool) return;
    /*鎶婄粨鏉熸爣蹇楃疆1*/
    pool->m_waitForDestroy = 1;
    /*鍞ら啋鎵�鏈夌瓑寰呯嚎绋嬶紝绾跨▼姹犺閿�姣佷簡*/
    pthread_cond_broadcast (&pool->m_taskReady);
    /*闃诲绛夊緟绾跨▼閫�鍑猴紝涓哄瓙绾跨▼鏀跺案*/
    for(i=0;i<pool->m_maxThrNum;i++){
        pthread_join (pool->m_thrGroup[i], 0);
    }
    TP_FREE(pool->m_thrGroup);

    /*娓呴櫎浠诲姟闃熷垪*/
    TP_CleanTask(pool);

    /*閿�姣佹潯浠跺彉閲忓拰浜掓枼閲�*/
    pthread_mutex_destroy(&pool->m_taskLock);
    pthread_cond_destroy(&pool->m_taskReady);
}

int TP_PushTask(ThreadPool*pool, ThreadPoolTask *task)
{
    if(0==task || 0==task->m_run) return -1;
    if(0==pool) return -1;

    /*鍔犻攣*/
    pthread_mutex_lock(&pool->m_taskLock);
    /*鎶婁换鍔℃坊鍔犲埌闃熷熬*/

    ThreadPoolTask *p = pool->m_taskHeader;
    if(0==p){
        pool->m_taskHeader = task;
    }else{
        while(p->m_next){
            p = p->m_next;
        }
        p->m_next = task;
    }
    /*瑙ｉ攣*/
    pthread_mutex_unlock(&pool->m_taskLock);
    /*鍞ら啋涓�涓鍦ㄧ瓑寰呯殑绾跨▼,
    濡傛灉鎵�鏈夌嚎绋嬮兘鍦ㄦ縺娲荤姸鎬佽繖鍙ュ垯娌℃湁浣滅敤*/
    pthread_cond_signal(&pool->m_taskReady);
    return 0;
}

int TP_PushTaskEx(ThreadPool* pool,TP_FUN fun,void* arge,int size)
{
    if(0==pool || 0==fun) return -1;
    ThreadPoolTask *task = TP_CreateThreadPoolTask();
    if(0==task) return -1;
    task->m_run = fun;
    TP_SetTaskArg(task,arge,size);
    return TP_PushTask(pool,task);
}

ThreadPoolTask* TP_PopTask(ThreadPool* pool)
{
    ThreadPoolTask *p;
    if(0== pool) return 0;
    /*鍔犻攣*/
    pthread_mutex_lock(&pool->m_taskLock);
    /*濡傛灉鐜板湪浠诲姟閾捐〃涓病鏈変换鍔″苟涓旀病鏈夋墦绠楅攢姣佺嚎绋嬫睜鍒欑瓑寰�*/
    while(0==pool->m_taskHeader && 0==pool->m_waitForDestroy){
        //printf("wait----- 0x%x\n",(unsigned int) pthread_self());
        pthread_cond_wait(&pool->m_taskReady,&pool->m_taskLock);
    }
    /*鍙栧緱闃熷ご浠诲姟*/
    p = pool->m_taskHeader;
    if(0!=p){
        pool->m_taskHeader = pool->m_taskHeader->m_next;
    }
    /*瑙ｉ攣*/
    pthread_mutex_unlock(&pool->m_taskLock);
    return p;
}

void TP_CleanTask(ThreadPool *pool)
{
    if(0==pool) return;
    /*娓呴櫎浠诲姟閾句腑鎵�鏈夌殑浠诲姟*/
    /*鍔犻攣*/
    pthread_mutex_lock(&pool->m_taskLock);
    ThreadPoolTask *p = pool->m_taskHeader;
    ThreadPoolTask *j;
    while(NULL!=p){
        j = p;
        p = p->m_next;
        TP_DeleteThreadPoolTask(j);
    }
    pool->m_taskHeader = 0;
    /*瑙ｉ攣*/
    pthread_mutex_unlock(&pool->m_taskLock);
}


void TP_Process(ThreadPool* pool)
{
    ThreadPoolTask *task;
    if(0==pool) return;

    while(0 == pool->m_waitForDestroy){
        task = TP_PopTask(pool);
        if(0==task || 0==task->m_run){
            if(1==pool->m_waitForDestroy) break;
        }else{
            /*濡傛灉姝ｇ‘鍦拌幏鍙栧埌浜唗ask鍒欐墽琛岃繖涓猼ask*/
            TP_FUN fun = task->m_run;
            void *arg = task->m_arge;
            (*fun)(arg);
            /*鎵ц瀹屾瘯鍚庨攢姣佽繖涓猼ask*/
            TP_DeleteThreadPoolTask(task);
        }
    }
}


