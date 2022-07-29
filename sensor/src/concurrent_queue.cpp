#pragma once
#include <stdlib.h>
#include <iostream>

#include <utility>
#include <atomic>
#include <queue>
#include <condition_variable>
#include <cassert>

template<class T>
class ThreadSafeDeque {
    public:
    	inline ThreadSafeDeque(size_t max_size = -1UL) : maxsize(max_size), end(false) {};
        
        void push(const T& t)
        {
            std::unique_lock<std::mutex> lck(mtx);
            while (que.size() == maxsize && !end)
                cv_full.wait(lck);
            assert(!end);
            que.push(std::move(t));
            cv_empty.notify_one();
        }


        T pop()
        {
            std::unique_lock<std::mutex> lck(mtx);
            while (que.empty() && !end)
                cv_empty.wait(lck);
            if (que.empty()) return {};
            T t = std::move(que.front());
            que.pop();
            cv_full.notify_one();
            return t;
            
        }

        T remove (const T& t)
        {
            std::unique_lock<std::mutex> lck(mtx);
            while (que.empty() && !end)
                cv_empty.wait(lck);
            if (que.empty()) return {};
            T popped;
            for (size_t i = 0; i < que.size(); i++)
            {      
                T curr = std::move(que.front());
                que.pop();
                if(curr == t){
                    break;
                }
                else{
                    que.push(curr);
                }
            }
            cv_full.notify_one();
            return popped;
        }

        T peek(){
            return que.front();
        }

        void close(){
            end = true;
            std::lock_guard<std::mutex> lck(mtx);
            cv_empty.notify_one();
            cv_full.notify_one();
        }

        bool empty(){
            return que.empty();
        }

        long size(){
            return que.size();
        }


    private:
        std::queue<T> que;
        std::mutex mtx;
        std::condition_variable cv_empty, cv_full;
        const size_t maxsize;
        std::atomic<bool> end;

};

// int main(int argc, char const *argv[])
// {
//     ThreadSafeDeque<int> q;
    
//     q.push(1);
//     q.push(2);
//     q.push(3);
//     q.push(4);
//     q.push(5);
    
//     q.remove(3);
//     while (!q.empty())
//     {
//         int x = q.pop().value();
//         std::cout << x << std::endl;
//         /* code */
//     }

//     q.close();
    
//     return 0;
// }
