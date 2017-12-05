#pragma once

#include "sys.hpp"

namespace MilSim {

	/**
	 * POD structure holding a render 'key' to perform sorting
	 * and a pointer to the actual render data.
	 */
	struct RenderCommand {
		uint64_t m_key;
		void* m_job;
	};

	/**
	 * Holds all the data required 
	struct RenderJob {

	};

	/**
	 * Provides an interface for submitting render commands.
	 * Try to design a sort of command API that lets higher-level
	 * systems care zero about OpenGL.
	 */
	class Render : public Sys {
	public:
		Render();
		~Render();
	};

};