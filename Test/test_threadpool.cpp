#include "pch.h"
#include "threadpool.h"


TEST(TestThreadpool, TestConstruct)
{
	EXPECT_NO_THROW(Hax::Threadpool threadpool(4));
}


TEST(TestThreadpool, TestSubmit)
{
	bool t1Running = false;
	bool t2Running = false;
	Hax::Threadpool threadpool(2);
	threadpool.Submit([&](bool& t)
		{
			std::this_thread::sleep_for(std::chrono::seconds(2));
			t = true;
		}, std::ref(t1Running));

	threadpool.Submit([&](bool& t)
		{
			std::this_thread::sleep_for(std::chrono::seconds(2));
			t = true;
		}, std::ref(t2Running));

	EXPECT_FALSE(t1Running);
	EXPECT_FALSE(t2Running);
	threadpool.WaitAll();
	EXPECT_TRUE(t1Running);
	EXPECT_TRUE(t2Running);
}


TEST(TestThreadpool, TestSubmitExcessWaits)
{
	std::vector<int> running(50, 0);
	std::mutex mtx;
	Hax::Threadpool threadpool(4);
	for (int i = 0; i < 50; ++i)
	{
		EXPECT_LE(threadpool.NumActive(), 4);
		threadpool.Submit([i, &mtx, &running]()
			{
				std::unique_lock<std::mutex>(mtx);
				running[i] = 1;
			});
	}

	threadpool.WaitAll();

	for (int i = 0; i < 50; ++i)
	{
		EXPECT_TRUE(running[i]);
	}
}