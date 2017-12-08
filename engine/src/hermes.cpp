#include "hermes.hpp"

#include <algorithm>
#include <spdlog/spdlog.h>

using namespace MilSim;

Hermes::Hermes()
{
	m_log = Logger::create("Hermes");
}

Hermes::~Hermes()
{}


void Hermes::subscribe(const uint32_t subid, std::vector<t_channel> channels)
{
	Subscription* sub;
	if(m_subs.find(subid) != m_subs.end()) {
		m_log->info("Subscription `{:x}` already exists; appending new channels...");
	} else {
		m_subs[subid] = Subscription();
	}

	sub = &m_subs[subid];
	sub->m_channels.insert(sub->m_channels.end(), channels.begin(), channels.end());
}

std::vector<Message*> Hermes::pull_inbox(const uint32_t subid)
{
	/**
	 * (old: no writing on m_inbox happens while we do this)
	 * we cannot really assume that. the render thread might
	 * push messages while the main thread is updating components,
	 * and then we're fucked.
	 * this is not the most efficient solution, but it is a solution.
	 */
	std::lock_guard<std::mutex> guard(m_mutex);
	auto& sub = m_subs[subid];
	std::vector<Message*> res;
	for(auto& m : m_inbox) {
		if(std::find(sub.m_channels.begin(), sub.m_channels.end(), m->m_chan) != sub.m_channels.end()) {
			res.push_back(m.get());
		}
	}
	return res;
}

void Hermes::send(Message* msg)
{
	/**
	 * Now `Hermes` owns the message memory,
	 * and will free it once we flush the message inbox.
	 * 
	 * Rudimentarily thread-safe, blocks on read-and-write.
	 * We could have a "write" queue, and a "read" queue to avoid
	 * this double locking.
	 */
	std::lock_guard<std::mutex> guard(m_mutex);
	m_inbox.push_back(std::unique_ptr<Message>(msg));
}

void Hermes::clean()
{
	m_inbox.clear();
}