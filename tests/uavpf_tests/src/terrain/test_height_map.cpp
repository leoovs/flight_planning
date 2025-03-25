#include <catch2/catch_all.hpp>
#include <uavpf/terrain/height_map.h>

TEST_CASE("HeightMap")
{
	SECTION("Resizing")
	{
		uavpf::HeightMap emptyHeightMap;
		REQUIRE(emptyHeightMap.GetWidth() == 0);
		REQUIRE(emptyHeightMap.GetHeight() == 0);
		REQUIRE(emptyHeightMap.GetSize() == 0);

		uavpf::HeightMap sizedHeightMap(2, 5);
		emptyHeightMap.Resize(2, 5);

		REQUIRE(emptyHeightMap.GetWidth() == sizedHeightMap.GetWidth());
		REQUIRE(sizedHeightMap.GetWidth() == 2);

		REQUIRE(emptyHeightMap.GetHeight() == sizedHeightMap.GetHeight());
		REQUIRE(sizedHeightMap.GetHeight() == 5);
	}

	SECTION("Row-major storage")
	{
		uavpf::HeightMap storage(2, 2);

		storage[0][0] = 1.0f;
		storage[0][1] = 2.0f;
		storage[1][0] = 3.0f;
		storage[1][1] = 4.0f;

		float* flat = &storage[0][0];

		REQUIRE(flat[0] == 1.0f);
		REQUIRE(flat[1] == 2.0f);
		REQUIRE(flat[2] == 3.0f);
		REQUIRE(flat[3] == 4.0f);
	}
}
