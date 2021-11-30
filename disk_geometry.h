#pragma once


class LOCAL_DISK_GEOMETRY
{
public:
    
    LPWSTR drive_path;
    ULONGLONG cylinders;
    ULONGLONG tracksPerCylinder;
    ULONGLONG all_tracks;
    ULONGLONG sectorsPerTrack;
    ULONGLONG all_sectors;
    ULONGLONG bytesPerSector;
    ULONGLONG diskSize_bytes;
    double diskSize_giga;
    MEDIA_TYPE media_type;
    HANDLE hDevice;

    LOCAL_DISK_GEOMETRY(LPWSTR drive_name = (LPWSTR)L"\\\\.\\PhysicalDrive0")
    {
        drive_path = drive_name;
        cylinders = 0;
        tracksPerCylinder = 0;
        all_tracks = 0;
        sectorsPerTrack = 0;
        all_sectors = 0;
        bytesPerSector = 0;
        diskSize_bytes = 0;
        diskSize_giga = 0;
        media_type = Unknown;

        hDevice = INVALID_HANDLE_VALUE;
        //startDevice();
    }

    void calculateDiskSize(DISK_GEOMETRY *pdg)
    {
        diskSize_bytes = pdg->Cylinders.QuadPart * (ULONG)pdg->TracksPerCylinder * (ULONG)pdg->SectorsPerTrack * (ULONG)pdg->BytesPerSector;
        diskSize_giga = (double)diskSize_bytes / (1024 * 1024 * 1024);
    }

    void startDevice()
    {
        hDevice = CreateFileW(drive_path,          // drive to open
            0,                // no access to the drive
            FILE_SHARE_READ | // share mode
            FILE_SHARE_WRITE,
            NULL,             // default security attributes
            OPEN_EXISTING,    // disposition
            0,                // file attributes
            NULL);            // do not copy file attributes

        if (hDevice == INVALID_HANDLE_VALUE)    // cannot open the drive
        {
            wprintf(L"CreateFileW failed. Error %ld.\n", GetLastError());
        }
    }

    void endDevice()
    {
        CloseHandle(hDevice);
    }

    void GetDiskAttributes()
    {
        GET_DISK_ATTRIBUTES dci = { 0 };
        BOOL bResult = FALSE;
        DWORD junk = 0;

        bResult = DeviceIoControl(hDevice, // device to be queried
            IOCTL_DISK_GET_DISK_ATTRIBUTES, // operation to perform
            NULL, 0,                       // no input buffer
            &dci, sizeof(dci),             // output buffer
            &junk,                         // # bytes returned
            (LPOVERLAPPED)NULL);          // synchronous I/O

        if (bResult)
        {
            wprintf(L"Version: %d\n", dci.Version);
            switch (dci.Attributes)
            {
                case DISK_ATTRIBUTE_OFFLINE:
                    wprintf(L"Atrribute: DISK_ATTRIBUTE_OFFLINE\n");
                break;

                case DISK_ATTRIBUTE_READ_ONLY:
                    wprintf(L"Atrribute: DISK_ATTRIBUTE_READ_ONLY\n");
                break;

                case 0x0000000000000000:
                    wprintf(L"Atrribute: READ and WRITE\n");
                break;
            }
        }
        else
        {
            wprintf(L"GetDiskCacheInformation failed. Error %ld.\n", GetLastError());
        }
    }

    void GetDriveGeometry()
    {
        // Retrieves information about the physical disk's geometry: 
        // type, number of cylinders, tracks per cylinder, sectors per track, and bytes per sector.
        DISK_GEOMETRY pdg = { 0 };
        BOOL bResult = FALSE;
        DWORD junk = 0;

        bResult = DeviceIoControl(hDevice, // device to be queried
            IOCTL_DISK_GET_DRIVE_GEOMETRY, // operation to perform
            NULL, 0,                       // no input buffer
            &pdg, sizeof(pdg),             // output buffer
            &junk,                         // # bytes returned
            (LPOVERLAPPED)NULL);          // synchronous I/O

        if (bResult)
        {
            calculateDiskSize(&pdg);
            cylinders = (ULONG)pdg.Cylinders.QuadPart;
            tracksPerCylinder = (ULONGLONG)pdg.TracksPerCylinder;

            all_tracks = pdg.Cylinders.QuadPart * (ULONGLONG)pdg.TracksPerCylinder;

            sectorsPerTrack = (ULONGLONG)pdg.SectorsPerTrack;
            all_sectors = all_tracks * (ULONGLONG)pdg.SectorsPerTrack;

            bytesPerSector = (ULONGLONG)pdg.BytesPerSector;


            media_type = pdg.MediaType;
        }
        else
        {
            wprintf(L"GetDriveGeometry failed. Error %ld.\n", GetLastError());
        }
    }

    void printData()
    {
        wprintf(L"Drive path      = %ws\n", drive_path);
        switch (media_type)
        {
            case Unknown:
                wprintf(L"Media type      = Unknown\n");
            break;

            case FixedMedia:
                wprintf(L"Media type      = FixedMedia\n");
            break;

            case RemovableMedia:
                wprintf(L"Media type      = RemovableMedia\n");
            break;
        }
        

        wprintf(L"Cylinders       = %I64d\n", cylinders);
        wprintf(L"Tracks/cylinder = %I64d\n", tracksPerCylinder);
        wprintf(L"ALL Tracks      = %I64d\n", all_tracks);


        wprintf(L"Sectors/track   = %I64d\n", sectorsPerTrack);
        wprintf(L"ALL Sectors     = %I64d\n", all_sectors);

        wprintf(L"Bytes/sector    = %I64d\n", bytesPerSector);

        wprintf(L"Disk size       = %I64d (Bytes)\n"
            L"                = %.2f (Gb)\n", diskSize_bytes, diskSize_giga);
    }

    void GetDriveLayoutEx()
    {
        DRIVE_LAYOUT_INFORMATION_EX pdg = { 0 };
        BOOL bResult = FALSE;
        DWORD junk = 0;


        bResult = DeviceIoControl(hDevice, // device to be queried
            IOCTL_DISK_GET_DRIVE_LAYOUT_EX, // operation to perform
            NULL, 0,                       // no input buffer
            &pdg, sizeof(pdg),             // output buffer
            &junk,                         // # bytes returned
            (LPOVERLAPPED)NULL);          // synchronous I/O

        if (bResult)
        {
            switch (pdg.PartitionStyle)
            {
                case PARTITION_STYLE_MBR:
                    wprintf(L"PartitionStyle: PARTITION_STYLE_MBR\n");

                break;

                case PARTITION_STYLE_GPT:
                {
                    wprintf(L"PartitionStyle: PARTITION_STYLE_GPT\n");
                    wprintf(L"PartitionCount: %d\n", pdg.PartitionCount);

                    DRIVE_LAYOUT_INFORMATION_GPT dlig = pdg.Gpt;
                    wprintf(L"DiskId (GUID): %8X-%4X-%4X-%2x%2x-%2x%2x%2x%2x%2x%2x\n",
                        dlig.DiskId.Data1,
                        dlig.DiskId.Data2,
                        dlig.DiskId.Data3,
                        dlig.DiskId.Data4[0],
                        dlig.DiskId.Data4[1],
                        dlig.DiskId.Data4[2],
                        dlig.DiskId.Data4[3],
                        dlig.DiskId.Data4[4],
                        dlig.DiskId.Data4[5],
                        dlig.DiskId.Data4[6],
                        dlig.DiskId.Data4[7]
                    );
                    wprintf(L"StartingUsableOffset (in Bytes): %ull\n", dlig.StartingUsableOffset);

                    wprintf(L"UsableLength (in Bytes): %I64d\n", dlig.UsableLength.QuadPart);
                    wprintf(L"MaxPartitionCount: %ull\n", dlig.MaxPartitionCount);

                    /******************************************************/
                    
                    PARTITION_INFORMATION_EX pix = pdg.PartitionEntry[0];
                    wprintf(L"PARTITON: \n");
                    switch (pix.PartitionStyle)
                    {
                        case PARTITION_STYLE_MBR:
                            wprintf(L"\tPartitionStyle: PARTITION_STYLE_MBR\n");
                        break;

                        case PARTITION_STYLE_GPT:
                            wprintf(L"\tPartitionStyle: PARTITION_STYLE_GPT\n");
                        break;

                        case PARTITION_STYLE_RAW:
                            wprintf(L"\tPartitionStyle: PARTITION_STYLE_RAW\n");
                        break;
                    }

                    wprintf(L"\tStartingOffset: %I64d\n", pix.StartingOffset.QuadPart);
                    wprintf(L"\tPartitionLength: %I64d\n", pix.PartitionLength.QuadPart);
                    wprintf(L"\tPartitionNumber: %d\n", pix.PartitionNumber);
                    wprintf(L"\tRewritePartition: %d\n", pix.RewritePartition);
                    wprintf(L"\tIsServicePartition: %d\n", pix.IsServicePartition);
                    /******************************************************/


                }
                break;

                case PARTITION_STYLE_RAW:
                    wprintf(L"PartitionStyle: PARTITION_STYLE_RAW\n");

                    
                break;
            }
            




        }
        else
        {
            wprintf(L"GetDriveLayoutEx failed. Error %ld.\n", GetLastError());
        }

    }

    void GetDiskPerformance()
    {
        DISK_PERFORMANCE pdg = { 0 };
        BOOL bResult = FALSE;
        DWORD junk = 0;

        // IMPORTANT: for this you must use C:, D:, ... instead PhysicalDrive0,...


        bResult = DeviceIoControl(hDevice, // device to be queried
            IOCTL_DISK_PERFORMANCE, // operation to perform
            NULL, 0,                       // no input buffer
            &pdg, sizeof(pdg),             // output buffer
            &junk,                         // # bytes returned
            (LPOVERLAPPED)NULL);          // synchronous I/O

        if (bResult)
        {
            wprintf(L"PERFORMANCE: \n");
            wprintf(L"BytesRead: %I64d\n", pdg.BytesRead.QuadPart);
            wprintf(L"BytesWritten: %I64d\n", pdg.BytesWritten.QuadPart);
            wprintf(L"ReadTime: %I64d\n", pdg.ReadTime.QuadPart);
            wprintf(L"WriteTime: %I64d\n", pdg.WriteTime.QuadPart);
            wprintf(L"IdleTime: %I64d\n", pdg.IdleTime.QuadPart);
        }
        else
        {
            wprintf(L"GetDiskPerformance failed. Error %ld.\n", GetLastError());
        }
    }

    void GetDiskPerformanceOFF()
    {
        DISK_PERFORMANCE pdg = { 0 };
        BOOL bResult = FALSE;
        DWORD junk = 0;


        bResult = DeviceIoControl(hDevice, // device to be queried
            IOCTL_DISK_PERFORMANCE_OFF, // operation to perform
            NULL, 0,                       // no input buffer
            NULL, 0,             // output buffer
            &junk,                         // # bytes returned
            (LPOVERLAPPED)NULL);          // synchronous I/O

        if (bResult)
        {
            wprintf(L"GetDiskPerformanceOFF OK\n");
        }
        else
        {
            wprintf(L"GetDiskPerformanceOFF failed. Error %ld.\n", GetLastError());
        }
    }








};




