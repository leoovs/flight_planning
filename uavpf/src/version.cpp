#include "uavpf/version.h"

#include "uavpf/config.h"

uavpf::Version uavpf::GetCurrentVersion()
{
	return
	{
		UAVPF_VERSION_MAJOR,
		UAVPF_VERSION_MINOR,
		UAVPF_VERSION_PATCH,
	};
}

