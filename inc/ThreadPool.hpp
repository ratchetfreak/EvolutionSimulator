#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

class ThreadPool
{
	public:
		std::vector<std::unique_ptr<std::thread>>	workers;
		std::mutex							  jobMutex;
		std::queue<std::function<void(void)>> jobList;
		//std::function<void()>				 *job = nullptr;
		std::condition_variable				  isJobAvailable;
		int									  size;
		bool								  terminateSignal = false;
		std::atomic<int>					  count			  = 0;

		void threadLoop(int id)
		{
			while (true)
			{
				std::function<void()> job;
				{
					std::unique_lock<std::mutex> lock(jobMutex);
					isJobAvailable.wait(lock, [this]() { return !jobList.empty() || terminateSignal; });

					if (terminateSignal)
					{
						return;
					}

					count++;
					job = jobList.front();
					jobList.pop();
				}
				job();
				count--;
			}
		}

		ThreadPool(int size) : size(size)
		{
			workers.resize(size);

			for (int i = 0; i < size; i++)
			{
				workers[i] = std::make_unique<std::thread>(&ThreadPool::threadLoop, this, i);
			}

			isJobAvailable.notify_one();
		}

		void queue(std::function<void()> func)
		{
			{
				std::unique_lock<std::mutex> lock(jobMutex);
				jobList.push(func);
			}
			isJobAvailable.notify_one();
		}

		bool active()
		{
      std::unique_lock<std::mutex> lock(jobMutex);
			if (count.load() || !jobList.empty())
			{
				return true;
			}

			return false;
		}

		~ThreadPool()
		{
			{
				std::unique_lock<std::mutex> lock(jobMutex);
				terminateSignal = true;
			}

			isJobAvailable.notify_all();

			for (int i = 0; i < size; i++)
			{
				workers[i]->join();
				
			}

			workers.clear();

			
		}
};

// class ThreadDistribute
// {
// 	public:
// 		struct Worker
// 		{
// 				std::thread							 *thread = nullptr;
// 				std::queue<std::function<void(void)>> jobList;
// 				int									  cursor = 0;
// 		} *worker;
//
// 		std::function<void()> *job = nullptr;
// 		int					   size;
// 		bool				   terminateSignal = false;
//
// 		static void threadLoop(Worker &worker, bool &terminateSignal)
// 		{
// 			while (true)
// 			{
// 				while (worker.cursor < worker.jobList.size() && terminateSignal == false)
// 				{
// 				}
//
// 				if (terminateSignal)
// 				{
// 					return;
// 				}
//
// 				worker.jobList.front()();
//
// 				worker.cursor++;
// 			}
// 		}
//
// 		ThreadDistribute(int size) : size(size)
// 		{
// 			worker = new Worker[size];
//
// 			for (int i = 0; i < size; i++)
// 			{
// 				worker[i].thread = new std::thread(threadLoop, worker[i], terminateSignal);
// 			}
// 		}
//
// 		void queue(std::function<void()> func, int id)
// 		{
// 			worker[id].jobList.push(func);
// 		}
//
// 		bool active()
// 		{
// 			for (int i = 0; i < size; i++)
// 			{
// 				if (worker[i].cursor >= worker[i].jobList.size())
// 				{
// 					return true;
// 				}
// 			}
//
// 			return false;
// 		}
//
// 		void clear()
// 		{
// 			for(int i = 0; i < size; i++)
// 			{
// 				worker[i].jobList = std::queue<std::function<void()>>();
// 				worker[i].cursor = 0;
// 			}
// 		}
//
// 		~ThreadDistribute()
// 		{
// 			terminateSignal = true;
//
// 			for (int i = 0; i < size; i++)
// 			{
// 				worker[i].thread->join();
//
// 				delete worker[i].thread;
// 			}
//
// 			delete[] worker;
// 		}
// };
