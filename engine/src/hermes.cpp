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
	auto& sub = m_subs[subid];
	std::vector<Message*> res;
	for(auto& m : m_inbox) {
		for(auto c : sub.m_channels) {
		}
		if(std::find(sub.m_channels.begin(), sub.m_channels.end(), m->m_chan) != sub.m_channels.end()) {
			res.push_back(m.get());
		}
	}
	return res;
}

void Hermes::send(Message* msg)
{
	// now `Hermes` owns the message memory
	m_inbox.push_back(std::unique_ptr<Message>(msg));
}

void Hermes::clean()
{
	m_inbox.clear();
}