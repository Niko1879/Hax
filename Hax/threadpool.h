/*
 * Class to manage a pool of threads (WORK IN PROGRESS).
 * 
 * A global mutex is used for synchronisation, and
 * a condition variable to simulate queueing up new threads.
 */


#pragma once
#include <thread>
#include <vector>
#include <mutex>


namespace Hax
{
	class Threadpool
	{
	public:
		Threadpool(size_t nthread) : nthread(nthread), threadpool(nthread), isActive(nthread, false), numActive(0) {}
		
		//Execute the given function with the provided arguments in it's own thread
		//and add it to the threadpool.
		template<class Function, class... Args>
		void Submit(Function&& f, Args&&... args)
		{
			std::unique_lock<std::mutex> lock(mtx);
			if (Full())
			{
				//Notified by: MarkFinished
				cv.wait(lock);
			}
			size_t threadInd = GetFirstInactive();
			if (threadpool[threadInd].joinable())
			{
				//Mark the old thread as finished
				threadpool[threadInd].join();
			}

			threadpool[threadInd] = std::thread([threadInd, this, f, args...]()
				{
					f(args...);
					MarkFinished(threadInd);
				});

			++numActive;
			isActive[threadInd] = true;
		}

		//Returns true if all threads are busy
		bool Full() const
		{
			return numActive == nthread;
		}

		size_t NumActive() const
		{
			return numActive;
		}

		//Joins all active threads
		void WaitAll()
		{
			for (std::thread& t : threadpool)
			{
				if (t.joinable()) t.join();
			}
		}

		~Threadpool()
		{
			WaitAll();
		}


	private:
		//Register thread as finished so it can be replaced by a new one
		void MarkFinished(int threadInd)
		{
			std::lock_guard<std::mutex> lk(mtx);
			isActive[threadInd] = false;
			--numActive;
			cv.notify_one();
		}

		//Returns the index of the first inactive thread
		size_t GetFirstInactive()
		{
			for (size_t i = 0; i < nthread; ++i)
			{
				if (!isActive[i])
				{
					return i;
				}
			}
			
			//error
			return nthread + 1;
		}

		std::vector<std::thread> threadpool;
		std::vector<bool> isActive;
		size_t numActive;
		size_t nthread;
		std::mutex mtx;
		std::condition_variable cv;
	};
}

