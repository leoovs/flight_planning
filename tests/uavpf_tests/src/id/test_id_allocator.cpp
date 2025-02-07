#include <catch2/catch_all.hpp>
#include <uavpf/id/id_allocator.h>

TEST_CASE("IDAllocator")
{
	uavpf::IDAllocator idAllocator;

	uavpf::IDAllocator usedAllocator;
	uavpf::BaseID id1 = usedAllocator.Allocate();
	uavpf::BaseID id2 = usedAllocator.Allocate();
	uavpf::BaseID id3 = usedAllocator.Allocate();
	
	SECTION("IDs ascend when allocated")
	{
		uavpf::BaseID first = idAllocator.Allocate();	
		uavpf::BaseID second = idAllocator.Allocate();	
		uavpf::BaseID third = idAllocator.Allocate();	

		REQUIRE(0 == first);
		REQUIRE(1 == second);
		REQUIRE(2 == third);

		REQUIRE(idAllocator.IsActive(first));
		REQUIRE(idAllocator.IsActive(second));
		REQUIRE(idAllocator.IsActive(third));
	}

	SECTION("IDs descend when recycled (Last in, First out order)")
	{
		usedAllocator.Free(id1);
		usedAllocator.Free(id2);
		usedAllocator.Free(id3);

		REQUIRE_FALSE(usedAllocator.IsActive(id1));
		REQUIRE_FALSE(usedAllocator.IsActive(id2));
		REQUIRE_FALSE(usedAllocator.IsActive(id3));

		uavpf::BaseID id4 = usedAllocator.Allocate();
		uavpf::BaseID id5 = usedAllocator.Allocate();
		uavpf::BaseID id6 = usedAllocator.Allocate();

		REQUIRE(id4 == id3);
		REQUIRE(id5 == id2);
		REQUIRE(id6 == id1);
	}
}

