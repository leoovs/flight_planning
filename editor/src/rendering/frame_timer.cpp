#include "rendering/frame_timer.h"

namespace editor
{
	void FrameTimer::Tick()
	{
		auto now = std::chrono::high_resolution_clock::now();

		constexpr float cNano(std::nano().den);
		auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(now - mPrev).count();

		mDeltaTimeSeconds = duration / cNano;

		mPrev = std::chrono::high_resolution_clock::now();
	}

	float FrameTimer::GetDeltaTimeSeconds() const
	{
		return mDeltaTimeSeconds;
	}
}

