#pragma once

#include <cstdarg>

#include <string>
#include <string_view>

namespace uavpf
{
	std::string FormatStr(std::string_view fmt, ...);
	std::string FormatStrV(std::string_view fmt, va_list args);
}

