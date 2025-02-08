#include <catch2/catch_all.hpp>
#include <uavpf/format_str.h>

TEST_CASE("String formatting")
{
	SECTION("sprintf-like formatting")
	{
		std::string result = uavpf::FormatStr("%s - %d _ %f", "Whatever", 107, 10.5f);
		REQUIRE(result == "Whatever - 107 _ 10.500000");
	}

	SECTION("Scientific notation")
	{
		std::string scientific = uavpf::FormatStr("%.1e", 1e-7f);
		REQUIRE(scientific == "1.0e-07");
	}
}

