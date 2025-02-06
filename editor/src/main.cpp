#include <iostream>

#include <uavpf/uavpf.h>

int main()
{
	uavpf::Version v = uavpf::GetCurrentVersion();
	std::cout << "flight_planning current version is "
		<< v.Major << '.'
		<< v.Minor << '.'
		<< v.Patch << '\n';
}

