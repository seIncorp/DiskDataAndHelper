#include "main.h"

int wmain(int argc, wchar_t* argv[])
{
	LOCAL_DISK_GEOMETRY *ldg = new LOCAL_DISK_GEOMETRY((LPWSTR)L"\\\\.\\D:");
	ldg->startDevice();
	ldg->GetDriveGeometry();
	ldg->printData();

	ldg->GetDiskAttributes();
	ee;
	ldg->GetDriveLayoutEx();


	ldg->GetDiskPerformance();
	ee;
	ldg->GetDiskPerformanceOFF();



	ldg->endDevice();
	ee;

	return 0;
}