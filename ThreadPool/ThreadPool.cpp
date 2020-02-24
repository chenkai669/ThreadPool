#include "ThreadPool.h"
#include <assert.h>
#include<iostream>



ThreadPool::ThreadPool(int size):threads_size(size),m_mutex(),m_cond(),m_isStarted(false)
{
	start();
}


ThreadPool::~ThreadPool()
{
	if (m_isStarted)
	{
		stop();
	}
}


void ThreadPool::start()
{
	assert(m_threads.empty());
	m_isStarted = true;
	m_threads.reserve(threads_size);
	for (int i = 0; i < threads_size; i++)//添加线程入队列
	{
		m_threads.push_back(new std::thread(std::bind(&ThreadPool::threadLoop, this)));
	}
}


void ThreadPool::stop()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_isStarted = false;
	m_cond.notify_all();
	for (Threads::iterator it = m_threads.begin(); it != m_threads.end(); it++)
	{
		(*it)->join();
		delete *it;
	}
	m_threads.clear();
}

void ThreadPool::threadLoop()
{
	std::cout << std::this_thread::get_id() << std::endl;
	while (m_isStarted)
	{
		Task task = take();
		if (task)
		{
			task();
		}
	}

	std::unique_lock<std::mutex> lock(m_mutex);
}


void ThreadPool::addTask(const Task& task)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_tasks.push_back(task);
	m_cond.notify_one();
}


ThreadPool::Task ThreadPool::take()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	while (m_tasks.empty() && m_isStarted)
	{
		m_cond.wait(lock);
	}

	Task m_task;
	tasks_t::size_type size = m_tasks.size();
	if (!m_tasks.empty() && m_isStarted)
	{
		m_task = m_tasks.front();
		m_tasks.pop_front();
		//assert(size - 1 == m_isStarted);
		if (size == 0)
		{
			return NULL;
		}
	}
	return m_task;
}


void add()
{

	std::cout << "test" << std::endl;
}


int  main()
{
	ThreadPool threadpool(5);
	threadpool.addTask(add);
	getchar();

	return 0;
}
