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
	m_subs[subid] = Subscription();
	m_subs[subid].m_channels = channels;
}

std::vector<Message> Hermes::pull_inbox(const uint32_t subid)
{
	auto& sub = m_subs[subid];
	std::vector<Message> res;
	for(auto& m : m_inbox) {
		if(std::find(sub.m_channels.begin(), sub.m_channels.end(), m.m_chan) != sub.m_channels.end()) {
			res.push_back(m);
		}
	}
	return res;
}