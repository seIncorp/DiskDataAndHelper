#include "main.h"


/*
	TODO:
	- DODAJ: funkcijo za prikaz pravega errorja

*/



int wmain(int argc, wchar_t* argv[])
{
	// TODO: preveri in popravi zakaj ni prava pot ko se isto poda preko argumenta
	for (int ii = 0; ii < argc; ii++)
	{
		// -D ali -d
		if (argv[ii][0] == 0x2D && (argv[ii][1] == 0x64 || argv[ii][1] == 0x44) )
		{
	//		wprintf(L"%ws\n", argv[ii+1]);

	//		// TODO: za vec case-ov npr.: -D a,c,v ali -d a,c,v

	//		LOCAL_DISK_GEOMETRY* ldg = new LOCAL_DISK_GEOMETRY((LPWSTR)argv[ii + 1]);
	//		ldg->GetDriveGeometry();
	//		ldg->printData();
		}
	}

	// TODO: ugotovi kako se lahko klice tudi z imeni diska npr. C, D, ...
	//LOCAL_DISK_GEOMETRY *ldg = new LOCAL_DISK_GEOMETRY((LPWSTR)L"\\\\.\\PhysicalDrive0");
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

	/*LOCAL_DISK_GEOMETRY* ldg1 = new LOCAL_DISK_GEOMETRY((LPWSTR)L"\\\\.\\PhysicalDrive1");
	ldg1->startDevice();
	ldg1->GetDriveGeometry();
	ldg1->printData();
	ldg1->endDevice();*/

	return 0;
}