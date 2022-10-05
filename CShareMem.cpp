#include "CShareMem.h"
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <iostream>
using namespace std;

union semun
{
    int value;
    struct semid_ds *buf;
    unsigned short int *array;
    struct seminfo *__buf;
};

CShareMem::CShareMem()
{
    m_shmID = 0;
    m_readSemID = 0;
    m_shareMem = NULL;
}

CShareMem::~CShareMem()
{
    //
}

int CShareMem::initShareMem(int key, bool isServer)
{
    if (-1 == init_shm(key))
    {
        return -1;
    }

    if (-1 == load_shm(m_shmID, (void **)&m_shareMem))
    {
        return -1;
    }

    // 读信号量
    if (-1 == init_sem(key + 1, &m_readSemID))
    {
        return -1;
    }

    if (isServer)
    {
        if (-1 == init_sem_value(m_readSemID, 1))
        {
            return -1;
        }
    }

    return 0;
}

int CShareMem::initData()
{
    memset(m_shareMem, 0, sizeof(My_MEM));
    return 0;
}

int CShareMem::updateGetData(My_MEM *pMemData)
{
    if (pMemData == NULL)
    {
        return -1;
    }

    if (-1 == sem_p(m_readSemID))
    {
        return -1;
    }

    m_shareMem->db3++;
    memcpy(pMemData, m_shareMem, sizeof(My_MEM));

    if (-1 == sem_v(m_readSemID))
    {
        return -1;
    }

    return 0;
}

int CShareMem::init_sem(int sem_key, int *sem_id)
{
    *sem_id = semget((key_t)sem_key, 1, 0666);
    if (*sem_id == -1)
    {
        *sem_id = semget((key_t)sem_key, 1, 0666 | IPC_CREAT);
        if (*sem_id == -1)
        {
            cout << "信号量semget 获取失败" << endl;
            return -1;
        }
    }

    cout << "信号量 semget success." << endl;

    return 0;
}

// 从系统中删除信号量
int CShareMem::del_sem(int sem_id)
{
    union semun sem_union;
    if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
    {
        cout << "信号量 semctl 删除失败" << endl;
        return -1;
    }

    cout << "信号量 semctl 删除成功" << endl;
    return 0;
}

// 信号量设置初始值
int CShareMem::init_sem_value(int sem_id, int init_value)
{
    union semun sem_union;
    sem_union.value = init_value; // 设置初始值

    if (semctl(sem_id, 0, SETVAL, sem_union) == -1)
    {
        cout << "信号量 initialize sem value fail." << endl;
        return -1;
    }
    else
    {
        cout << "信号量 initialize sem value success." << endl;
    }
    return 0;
}

/* P 操作函数 */
int CShareMem::sem_p(int sem_id)
{
    struct sembuf sops;
    sops.sem_num = 0;        /* 单个信号量的编号应该为 0 */
    sops.sem_op = -1;        /* 表示 P 操作 */
    sops.sem_flg = SEM_UNDO; /* 若进程退出，系统将还原信号量*/

    if (semop(sem_id, &sops, 1) == -1)
    {
        perror("P operation");
        return -1;
    }
    return 0;
}

/* V 操作函数*/
int CShareMem::sem_v(int sem_id)
{
    struct sembuf sops;
    sops.sem_num = 0;        /* 单个信号量的编号应该为 0 */
    sops.sem_op = 1;         /* 表示 V 操作 */
    sops.sem_flg = SEM_UNDO; /* 若进程退出，系统将还原信号量*/

    if (semop(sem_id, &sops, 1) == -1)
    {
        perror("V operation");
        return -1;
    }
    return 0;
}

// 初始化共享内存
int CShareMem::init_shm(int shm_key)
{
    m_shmID = shmget((key_t)shm_key, sizeof(My_MEM), 0666);
    if (m_shmID == -1)
    {
        m_shmID = shmget((key_t)shm_key, sizeof(My_MEM), IPC_CREAT | 0666);
        if (m_shmID == -1)
        {
            cout << "shmget error." << endl;
            return -1;
        }
    }
    cout << "shmget success." << endl;
    return 0;
}

// 删除共享内存
int CShareMem::del_shm(int shm_id)
{
    int ret = shmctl(shm_id, IPC_RMID, NULL);
    if (ret == -1)
    {
        cout << "shmctl 删除失败" << endl;
        return -1;
    }
    cout << "shmctl 删除成功" << endl;
    return 0;
}

// 挂载共享内存
int CShareMem::load_shm(int shm_id, void **mem)
{
    *mem = shmat(shm_id, NULL, 0);
    if (*mem == (void *)-1)
    {
        cerr << "shmat 挂载失败" << endl;
        return -1;
    }
    cout << "shmat 挂载成功" << endl;
    return 0;
}

// 分离共享内存
int CShareMem::unload_shm(int shm_id, void **mem)
{
    int ret = shmdt(*mem);
    if (ret == -1)
    {
        cout << "shmdt 分离失败" << endl;
        return -1;
    }
    cout << "shmdt 分离成功" << endl;
    return 0;
}