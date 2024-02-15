#ifndef TIMER_H
#define TIMER_H

#include<iostream>
#include<set>
#include<functional>
#include<thread>
#include<atomic>
#include<chrono>
#include<mutex>
#include<condition_variable>

/*Debug*/
//#define _DEBUG_

/*Timer Task Bass Class*/
struct TimerNodeBass
{
	uint64_t id;
	time_t expire;
};

/*Timer Task Bass Class*/
struct TimerNode : public TimerNodeBass
{
	using Callback = std::function<void(const TimerNode& node)>;
	Callback func;
	TimerNode(uint64_t id, time_t expire, TimerNode::Callback func);
};

/*Overload less than*/
bool operator<(const TimerNodeBass& ln, const TimerNodeBass rn);

/*Timer Class*/
class Timer
{
private:
	std::thread m_worker;
	std::set<TimerNode, std::less<>> m_task;
	std::atomic<bool> m_stop;
	std::mutex m_task_mutex;
	std::condition_variable m_condition;
	static uint64_t gid;
	static inline uint64_t GenID();
	void run();
public:
	Timer();
	~Timer();
	/*Return Tick*/
	static inline time_t GetTick();
	TimerNodeBass AddTimer(int time,TimerNode::Callback func);
	bool DelTimer(TimerNodeBass& node);
	/*Return Recent Task Time*/
	time_t TimeToSellp();
};

#endif



