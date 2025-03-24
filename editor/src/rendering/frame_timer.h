#pragma once

#include <chrono>

namespace editor
{
	class FrameTimer
	{
	public:
		void Tick();
		float GetDeltaTimeSeconds() const;

	private:
		std::chrono::high_resolution_clock::time_point mPrev;
		float mDeltaTimeSeconds = 0.0f;
	};
}

