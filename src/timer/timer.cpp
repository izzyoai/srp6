#include "timer.h"

TimerNode::TimerNode(uint64_t id, time_t expire, TimerNode::Callback func) :func(std::move(func))
{
	this->expire = expire;
	this->id = id;
}

bool operator<(const TimerNodeBass& ln, const TimerNodeBass rn)
{
    if (ln.expire < rn.expire)
    {
        return true;
    }
    else if (ln.expire > rn.expire)
    {
        return false;
    }
    else
    {
        return ln.id < rn.id;
    }
}

Timer::Timer():m_stop(false)
{
    m_worker = std::thread(&Timer::run, this);
}

Timer::~Timer()
{
    m_stop.store(true);
    m_condition.notify_all();
    m_worker.join();
}

uint64_t Timer::gid = 0;

inline uint64_t Timer::GenID()
{
    return gid++;
}

void Timer::run()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(m_task_mutex);
        /*the task not empty || stop thread . continue*/
        m_condition.wait(lock, [this]()->bool {return !m_task.empty() || m_stop;});
        if (m_stop)
        {
            break;
        }
        time_t cur_time = GetTick();
        auto iter = m_task.begin();
        if (iter != m_task.end() && iter->expire <= GetTick())
        {
            lock.unlock();
            /*Debug*/
#ifdef _DEBUG_
            std::cout << "id:" << iter->id << " func executing" << std::endl;
#endif
            iter->func(*iter);
#ifdef _DEBUG_
            std::cout << "id:" << iter->id << " func complete" << std::endl;
#endif
            lock.lock();
            m_task.erase(iter);
        }
        else
        {
            m_condition.wait_for(lock, std::chrono::milliseconds(iter->expire - cur_time));
        }
    }
}


inline time_t Timer::GetTick()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    
}

TimerNodeBass Timer::AddTimer(int time, TimerNode::Callback func)
{
    time_t expire = GetTick() + time;
    auto pairs = m_task.emplace( GenID(), expire, std::move(func));
    return static_cast<TimerNodeBass>(*pairs.first);
}

bool Timer::DelTimer(TimerNodeBass& node)
{
    auto iter = m_task.find(node);
    if (iter != m_task.end())
    {
        m_task.erase(iter);
        return true;
    }
    return false;

}

time_t Timer::TimeToSellp()
{
    auto iter = m_task.cbegin();
    if (iter == m_task.cend())
    {
        return -1;
    }
    time_t diss = iter->expire - GetTick();
    return diss > 0 ? diss : 0;
}

