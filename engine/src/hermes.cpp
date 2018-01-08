#include "hermes.hpp"

#include <algorithm>
#include <spdlog/spdlog.h>

using namespace MilSim;

Hermes::Hermes()
{
	m_log = Logger::create("Hermes");
}

Hermes::~Hermes()
{
	swap_queues();
	free_back_queues();
}

Hermes::t_queue const& Hermes::get_channel(const hash_t chan)
{
	return m_back_qs[chan];
}

void Hermes::send(Message* msg, const hash_t channel)
{
	HERMES_LOCK;
	m_front_qs[channel].push_back(msg);
}

void Hermes::swap_queues()
{
	HERMES_LOCK;
	free_back_queues();
	m_back_qs = std::move(m_front_qs);
	m_front_qs.clear();
}
void Hermes::free_back_queues()
{
	for(auto& q : m_back_qs) {
		for(auto p : q.second) {
			delete p;
		}
	}
}