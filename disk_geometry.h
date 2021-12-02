#pragma once


class LOCAL_DISK_GEOMETRY
{
public:
    
    wchar_t drive_path[30];
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



    LOCAL_DISK_GEOMETRY(const wchar_t drive_name[] = L"\\\\.\\PhysicalDrive0")
    {
        wcscpy_s(drive_path,30, drive_name);

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
            printError(GetLastError(), L"CreateFileW");
        }
    }

    void endDevice()
    {
        if(CloseHandle(hDevice) == 0)
            printError(GetLastError(), L"endDevice");
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
            printError(GetLastError(), L"GetDiskAttributes");
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

            printData();
        }
        else
        {
            printError(GetLastError(), L"GetDriveGeometry");
        }
    }

    void GetDriveLayoutEx()
    {
        BOOL bResult = FALSE;
        DWORD junk = 0;
        BYTE data[10000] { 0 };

        bResult = DeviceIoControl(hDevice, // device to be queried
            IOCTL_DISK_GET_DRIVE_LAYOUT_EX, // operation to perform
            NULL, 0,                       // no input buffer
            data, sizeof(data),             // output buffer
            &junk,                         // # bytes returned
            (LPOVERLAPPED)NULL);          // synchronous I/O

        if (bResult)
        {
            DRIVE_LAYOUT_INFORMATION_EX* test = (DRIVE_LAYOUT_INFORMATION_EX *)data;

            switch (test->PartitionStyle)
            {
                case PARTITION_STYLE_MBR:
                    wprintf(L"Drive Style: PARTITION_STYLE_MBR\n");
                break;

                case PARTITION_STYLE_GPT:
                    wprintf(L"Drive Style: PARTITION_STYLE_GPT\n");
                break;

                case PARTITION_STYLE_RAW:
                    wprintf(L"Drive Style: PARTITION_STYLE_RAW\n");
                break;
            }

            // TODO: rethink for adding DRIVE_LAYOUT_INFORMATION_MBR (only for master boot record type partitioning on the drive)

            wprintf(L"Drive Partitions: [%d]:\n", (test)->PartitionCount);

            for (int aa = 0; aa < test->PartitionCount; aa++)
            {
                wprintf(L"\tPartition [%d]:\n",aa);

                PARTITION_INFORMATION_EX bla = test->PartitionEntry[aa];

                wprintf(L"\t\tPartition ID: %d\n", bla.PartitionNumber);

                int style_flag = 0;
                switch (bla.PartitionStyle)
                {
                    case PARTITION_STYLE_MBR:
                        wprintf(L"\t\tStyle: PARTITION_STYLE_MBR\n");
                        style_flag = 0;
                    break;

                    case PARTITION_STYLE_GPT:
                        wprintf(L"\t\tStyle: PARTITION_STYLE_GPT\n");
                        style_flag = 1;
                    break;

                    case PARTITION_STYLE_RAW:
                        wprintf(L"\t\tStyle: PARTITION_STYLE_RAW\n");
                        style_flag = 2;
                    break;
                }

                wprintf(L"\t\tStarting offset: %I64d\n",bla.StartingOffset.QuadPart);

                wprintf(L"\t\tLength: %I64d B  (%.2f GB)\n",bla.PartitionLength.QuadPart, calculateToGB(bla.PartitionLength.QuadPart));
                wprintf(L"\t\tRewrite: %d\n",bla.RewritePartition);
                wprintf(L"\t\tIsServicePartition: %d\n",bla.IsServicePartition);
                
                if (style_flag == 0)
                {
                    PARTITION_INFORMATION_MBR temp_MBR = bla.Mbr;

                    GetDiskPartitionType(temp_MBR.PartitionType);

                    wprintf(L"\t\tBoot Indicator: %d\n", temp_MBR.BootIndicator);
                    wprintf(L"\t\tRecognized Partition: %d\n", temp_MBR.RecognizedPartition);
                    wprintf(L"\t\tHidden Sectors: %d\n", temp_MBR.HiddenSectors);
                    wprintf(L"\t\tPartition Id: %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n", 
                                temp_MBR.PartitionId.Data1,
                                temp_MBR.PartitionId.Data2,
                                temp_MBR.PartitionId.Data3,
                                temp_MBR.PartitionId.Data4[0],
                                temp_MBR.PartitionId.Data4[1],
                                temp_MBR.PartitionId.Data4[2],
                                temp_MBR.PartitionId.Data4[3],
                                temp_MBR.PartitionId.Data4[4],
                                temp_MBR.PartitionId.Data4[5],
                                temp_MBR.PartitionId.Data4[6],
                                temp_MBR.PartitionId.Data4[7]
                            );
                }
                else if(style_flag == 1)
                {
                    PARTITION_INFORMATION_GPT temp_GPT = bla.Gpt;

                    // TODO: please beside this code show also text
                    wprintf(L"\t\tPartition Type: %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
                        temp_GPT.PartitionType.Data1,
                        temp_GPT.PartitionType.Data2,
                        temp_GPT.PartitionType.Data3,
                        temp_GPT.PartitionType.Data4[0],
                        temp_GPT.PartitionType.Data4[1],
                        temp_GPT.PartitionType.Data4[2],
                        temp_GPT.PartitionType.Data4[3],
                        temp_GPT.PartitionType.Data4[4],
                        temp_GPT.PartitionType.Data4[5],
                        temp_GPT.PartitionType.Data4[6],
                        temp_GPT.PartitionType.Data4[7]
                    );

                    wprintf(L"\t\tPartition Id: %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
                        temp_GPT.PartitionId.Data1,
                        temp_GPT.PartitionId.Data2,
                        temp_GPT.PartitionId.Data3,
                        temp_GPT.PartitionId.Data4[0],
                        temp_GPT.PartitionId.Data4[1],
                        temp_GPT.PartitionId.Data4[2],
                        temp_GPT.PartitionId.Data4[3],
                        temp_GPT.PartitionId.Data4[4],
                        temp_GPT.PartitionId.Data4[5],
                        temp_GPT.PartitionId.Data4[6],
                        temp_GPT.PartitionId.Data4[7]
                    );

                    wprintf(L"\t\tAttributes: %llu\n", temp_GPT.Attributes);

                    wprintf(L"\t\tName: %s\n", temp_GPT.Name);
                }
                else
                {
                    // ??
                }

                ee;
            }

            // TODO: save all those data into object

        }
        else
        {
            printError(GetLastError(), L"GetDriveLayoutEx");
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
            wprintf(L"BytesRead: %I64d B  (%.2f GB)\n", pdg.BytesRead.QuadPart, calculateToGB(pdg.BytesRead.QuadPart));
            wprintf(L"BytesWritten: %I64d B  (%.2f GB)\n", pdg.BytesWritten.QuadPart, calculateToGB(pdg.BytesWritten.QuadPart));
            wprintf(L"ReadTime: %I64d\n", pdg.ReadTime.QuadPart);
            wprintf(L"WriteTime: %I64d\n", pdg.WriteTime.QuadPart);
            wprintf(L"IdleTime: %I64d\n", pdg.IdleTime.QuadPart);
        }
        else
        {
            printError(GetLastError(), L"GetDiskPerformance");
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
            
        }
        else
        {
            printError(GetLastError(), L"GetDiskPerformanceOFF");
        }
    }

    void GetVolumeRealDisk()
    {
        BOOL bResult = FALSE;
        DWORD junk = 0;
        VOLUME_DISK_EXTENTS data { 0 };

        bResult = DeviceIoControl(hDevice, // device to be queried
            IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, // operation to perform
            NULL, 0,                       // no input buffer
            &data, sizeof(data),             // output buffer
            &junk,                         // # bytes returned
            (LPOVERLAPPED)NULL);          // synchronous I/O

        if (bResult)
        {
            wprintf(L"NumberOfDiskExtents: %d\n", data.NumberOfDiskExtents);

            // TODO: do for more than one disk (in case of more than 1 disk currently is to small buffer)
            
            DISK_EXTENT temp_de = data.Extents[0];

            wprintf(L"\t DiskNumber: %d\n", temp_de.DiskNumber);
            wprintf(L"\t DiskName: \\\\.\\PhysicalDrive%d\n", temp_de.DiskNumber);

            wprintf(L"\t StartingOffset: %I64d\n", temp_de.StartingOffset.QuadPart);
            wprintf(L"\t ExtentLength: %I64d B  (%.2f GB)\n", temp_de.ExtentLength.QuadPart, calculateToGB(temp_de.ExtentLength.QuadPart));

        }
        else
        {
            printError(GetLastError(), L"GetVolumeRealDisk");
        }
    }

    

private:

    void GetDiskPartitionType(BYTE b)
    {
        // TODO: Add other flags

        switch (b)
        {
        case PARTITION_ENTRY_UNUSED:
            wprintf(L"\t\tDisk Partition Type: PARTITION_ENTRY_UNUSED\n");
            break;

        case PARTITION_EXTENDED:
            wprintf(L"\t\tDisk Partition Type: PARTITION_EXTENDED\n");
            break;

        case PARTITION_FAT_12:
            wprintf(L"\t\tDisk Partition Type: PARTITION_FAT_12\n");
            break;

        case PARTITION_FAT_16:
            wprintf(L"\t\tDisk Partition Type: PARTITION_FAT_16\n");
            break;

        case PARTITION_FAT32:
            wprintf(L"\t\tDisk Partition Type: PARTITION_FAT32\n");
            break;

        case PARTITION_IFS:
            wprintf(L"\t\tDisk Partition Type: PARTITION_IFS\n");
            break;

        case PARTITION_LDM:
            wprintf(L"\t\tDisk Partition Type: PARTITION_LDM\n");
            break;

        case PARTITION_NTFT:
            wprintf(L"\t\tDisk Partition Type: PARTITION_NTFT\n");
            break;

        case VALID_NTFT:
            wprintf(L"\t\tDisk Partition Type: VALID_NTFT\n");
            break;

        case 0x27:
            wprintf(L"\t\tDisk Partition Type: reserved (manufacturers use this type for their rescue partition. Can be FAT32 or NTFS.)\n");
            break;

        case 0x82:
            wprintf(L"\t\tDisk Partition Type: Linux Swap partition, Prime or Solaris (Unix)\n");
            break;

        case 0x83:
            wprintf(L"\t\tDisk Partition Type: Linux native file systems (ext2/3/4, JFS, Reiser, xiafs, and others)\n");
            break;

        default:
            wprintf(L"\t\tDisk Partition Type: UNKNOWN [%02X]\n", b);
            wprintf(L"\t\tFor more details visit:\n\t\thttps://thestarman.pcministry.com/asm/mbr/PartTypes.htm\n\t\tOR contact developer to add this type.\n");
            break;
        }

    }

    double calculateToGB(ULONGLONG num)
    {
        return (double)num / (1024 * 1024 * 1024);
    }

    void calculateDiskSize(DISK_GEOMETRY* pdg)
    {
        diskSize_bytes = pdg->Cylinders.QuadPart * (ULONG)pdg->TracksPerCylinder * (ULONG)pdg->SectorsPerTrack * (ULONG)pdg->BytesPerSector;
        diskSize_giga = (double)diskSize_bytes / (1024 * 1024 * 1024);
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

    void printError(DWORD err_id, const wchar_t fun[])
    {
        wchar_t temp_string[100];
        
        switch (err_id)
        {
            case 1:
                swprintf(temp_string, 100, L"%s failed. Error %ld [ERROR_INVALID_FUNCTION].\n", fun, err_id);
            break;

            case 2:
                swprintf(temp_string, 100, L"%s failed. Error %ld [ERROR_FILE_NOT_FOUND].\n", fun, err_id);
            break;

            case 5:
                swprintf(temp_string, 100, L"%s failed. Error %ld [ERROR_ACCESS_DENIED].\n", fun, err_id);
            break;

            case 6:
                swprintf(temp_string, 100, L"%s failed. Error %ld [ERROR_INVALID_HANDLE].\n", fun, err_id);
            break;

            case 53:
                swprintf(temp_string, 100, L"%s failed. Error %ld [ERROR_BAD_NETPATH].\n", fun, err_id);
            break;

            case 122:
                swprintf(temp_string, 100, L"%s failed. Error %ld [ERROR_INSUFFICIENT_BUFFER].\n", fun, err_id);
            break;

            default:
                swprintf(temp_string, 100, L"%s failed. Error %ld [].\n", fun, err_id);
                break;
        }

        wprintf(L"%s\n", temp_string);
    }






};




