#include "sys/render.hpp"

using namespace MilSim;

Render::Render()
	: Sys::Sys("Render"), m_should_stop(false)
{

}
Render::~Render()
{

}

void Render::init()
{
	m_thread = std::thread(
		&Render::thread_entry,
		this
	);
}
void Render::kill()
{
	m_should_stop.store(true);
	m_thread.join();
}
void Render::update()
{

}

void Render::thread_entry()
{
	while(!m_should_stop.load()) {
		//m_log->info("Logging from render thread...");
	}
}