#include "sys/hermes.hpp"

#include <algorithm>
#include "spdlog/spdlog.h"

using namespace MilSim;

Hermes::Hermes()
	: Sys::Sys()
{}

Hermes::~Hermes()
{}

void Hermes::init()
{

}
void Hermes::kill()
{
	
}


void Hermes::update()
{
	for(auto& sub : m_subs) {
		for(auto& ev : m_event_buffer) {
			if(std::find(sub.m_channels.begin(), sub.m_channels.end(), ev.m_channel) != sub.m_channels.end()) {
				spdlog::get("main")->info("Relaying event to a guy...");
			}
		}
	}
}