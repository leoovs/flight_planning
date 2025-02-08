#include "uavpf/format_str.h"

#include <cstdio>

#include <array>
#include <vector>

std::string uavpf::FormatStr(std::string_view fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	std::string result = FormatStrV(fmt, args);
	va_end(args);

	return result;
}

std::string uavpf::FormatStrV(std::string_view fmt, va_list args)
{
	constexpr size_t cOptimalBufferSize = 512;
	std::array<char, cOptimalBufferSize> optimalBuffer;

	va_list firstTryArgs;
	va_copy(firstTryArgs, args);
	size_t requiredSize = std::vsnprintf(
		optimalBuffer.data(),
		optimalBuffer.size(),
		fmt.data(),
		firstTryArgs);
	va_end(firstTryArgs);
	requiredSize++; // Include null-terminator.

	if (requiredSize <= optimalBuffer.size())
	{
		return std::string(optimalBuffer.data());	
	}

	std::vector<char> worstCaseBuffer(requiredSize);
	std::vsnprintf(
		worstCaseBuffer.data(),
		worstCaseBuffer.size(),
		fmt.data(),
		args);

	return std::string(worstCaseBuffer.data());
}

