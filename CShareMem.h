#pragma once

// 通过共享内存，传递数据，
// 目前用在log记录上

struct My_MEM
{
    double db1;
    double db2;
    double db3;
    double db4;
    char end[1024];
};

class CShareMem
{
public:
    CShareMem();
    ~CShareMem();

    int initShareMem(int key, bool isServer);
    int initData();     // 测试时，初始化调用一次
    int updateGetData(My_MEM* pMemData);

private:
    // --------------------------信号量相关方法
    // 初始化 信号量
    int init_sem(int sem_key, int* sem_id);

    // 从系统中删除信号量
    int del_sem(int sem_id);

    // 信号量设置初始值
    int init_sem_value(int sem_id, int init_value);

    /* P 操作函数 */
    int sem_p(int sem_id);

    /* V 操作函数*/
    int sem_v(int sem_id);

    // ---------------------------共享内存相关方法
    // 初始化共享内存
    int init_shm(int shm_key);

    // 删除共享内存
    int del_shm(int shm_id);

    // 挂载共享内存
    int load_shm(int shm_id, void** mem);

    // 分离共享内存
    int unload_shm(int shm_id, void** mem);

private:
    int m_readSemID;

    int m_shmID;
    My_MEM* m_shareMem;
};