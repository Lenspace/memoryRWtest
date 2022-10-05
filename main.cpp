#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <string.h>
#include "CShareMem.h"

using namespace std;

std::mutex thread_task1_mutex;
My_MEM* s_mem = new My_MEM;
void thread_task1(int n)
{
    cout << "thread " << std::this_thread::get_id() << " begin!" << endl;
    My_MEM new_mem;
    for (int i = 0; i < n; i++)
    {
        std::lock_guard<std::mutex> lck(thread_task1_mutex);
        s_mem->db3++;
        memcpy(&new_mem, s_mem, sizeof(My_MEM));
    }
    cout << "thread " << std::this_thread::get_id() << " end! db3 is " << s_mem->db3 << endl;
}

const int s_sharemem_id = 0x090909;
void thread_task2(int n)
{
    cout << "thread " << std::this_thread::get_id() << " begin!" << endl;
    CShareMem *pShareMem = new CShareMem();
    if (-1 == pShareMem->initShareMem(s_sharemem_id, false))
    {
        cout << "thread " << std::this_thread::get_id() << " initShareMem fail!" << endl;
        return;
    }

    My_MEM new_mem;
    for (int i = 0; i < n; i++)
    {
        if (-1 == pShareMem->updateGetData(&new_mem))
        {
            cout << "thread " << std::this_thread::get_id() << " readData fail." << endl;
            break;
        }
    }
    cout << "thread " << std::this_thread::get_id() << " end! db3 is " << new_mem.db3 << endl;
}

void RecordTime(const char *szOut)
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    std::time_t timestamp = tp.time_since_epoch().count();

    if (szOut != NULL)
        std::cout << "time " << szOut << " : " << timestamp << std::endl;
    else
        std::cout << "time : " << timestamp << std::endl;
}

// 测试分几种：程序内部内存的读写
//           使用共享内存的读写
int main(int argc, char *argv[])
{
    RecordTime("开始测试");

    const int max_thread_num = 2;
    const int thread_while_time = 100000;

    memset(s_mem, 0, sizeof(My_MEM));

    if (true)
    {
        CShareMem *pShareMem = new CShareMem();
        if (-1 == pShareMem->initShareMem(s_sharemem_id, true))
        {
            cout << "thread " << std::this_thread::get_id() << " initShareMem fail!" << endl;
            return 1;
        }
        pShareMem->initData();

        std::thread threads[max_thread_num];
        for (int i = 0; i < max_thread_num; i++)
        {
            threads[i] = std::thread(thread_task2, thread_while_time);
        }

        for (auto &t : threads)
        {
            t.join();
        }
    }
    else
    {
        std::thread threads[max_thread_num];
        for (int i = 0; i < max_thread_num; i++)
        {
            threads[i] = std::thread(thread_task1, thread_while_time);
        }

        for (auto &t : threads)
        {
            t.join();
        }
    }

    RecordTime("all thread finished.");

    return 0;
}