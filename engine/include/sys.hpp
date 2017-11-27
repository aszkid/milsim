#pragma once

namespace MilSim {

	class Sys {
	public:
		Sys() {};
		virtual ~Sys() {};

		virtual void init() = 0;
		virtual void kill() = 0;

		virtual void update() = 0;
	};

}
