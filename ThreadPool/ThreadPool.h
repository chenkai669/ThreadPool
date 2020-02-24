#pragma once
#include <functional>
#include<vector>
#include<thread>
#include<queue>
#include<mutex>
#include<condition_variable>



class ThreadPool
{
public:
	static const int kInitThreadsSize = 3;
	enum taskpriorityE
	{
		level0,
		level1,
		level2,
	};

	typedef std::function<void()> Task;
	typedef std::pair<taskpriorityE, Task> TaskPair;

public:
	ThreadPool(int size);
	~ThreadPool();

	void start();
	void stop();
	void addTask(const Task&);

private:
	ThreadPool(const ThreadPool&);//禁止拷贝构造
	const ThreadPool& operator=(const ThreadPool&);

	struct TaskPriorityCmp
	{
		bool operator()(const ThreadPool::TaskPair p1, const ThreadPool::TaskPair p2)
		{
			return p1.first > p2.first;//优先级
		}
	};
	void threadLoop();
	Task take();

private:
	typedef std::vector<std::thread*> Threads;
	typedef std::deque<Task> tasks_t;
	Threads m_threads;
	tasks_t m_tasks;
	std::mutex m_mutex;
	std::condition_variable m_cond;
	bool m_isStarted;
	int threads_size;
};

