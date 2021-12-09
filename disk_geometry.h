#pragma once

#define MAX_ATTRIBUTE 30



typedef struct    //smart attribute
{
    BYTE    Id;
    BYTE    StatusFlags[2];
    BYTE    AttributeValue;
    BYTE    WorstValue;
    BYTE    RawValue[6];
    BYTE    Reserved;
} SMART_ATTRIBUTE;

typedef struct
{
    ULONG PredictFailure;
    BYTE revisionNumber[2];
    SMART_ATTRIBUTE Attribute[MAX_ATTRIBUTE];

    BYTE offlineDataCollectionStatus;
    BYTE selfTestExecutionStatus;
    BYTE Total_time_in_seconds_to_complete_off_line_data_collection_activity[2];
    BYTE Reserved;
    BYTE Offline_data_collection_capability;
    BYTE SMART_capability[2];
    BYTE Error_logging_capability;
    BYTE Self_test_Failure_Checkpoint;
    BYTE Short_self_test_routine_recommended_polling_time_minutes;
    BYTE Extended_self_test_routine_recommended_polling_time_minutes;
    //BYTE Reserved[];      // currently IGNORED
    BYTE Data_tructure_Checksum;

} SMART_DATA;






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
    
    SMART_DATA smartData;


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

        smartData = { 0 };
    }

    
    void startDevice()
    {
        hDevice = CreateFileW(drive_path,          // drive to open
            
            //0,                // no access to the drive
            GENERIC_READ | GENERIC_WRITE,                // no access to the drive


            FILE_SHARE_READ | FILE_SHARE_WRITE,// share mode
            //FILE_SHARE_READ ,// share mode
            
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
                    wprintf(L"\t\tPartition Type: %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X ",
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

                    if (IsEqualGUID(temp_GPT.PartitionType, PARTITION_BASIC_DATA_GUID) == 1)
                        wprintf(L" [PARTITION_BASIC_DATA]\n");
                    else if(IsEqualGUID(temp_GPT.PartitionType, PARTITION_ENTRY_UNUSED_GUID) == 1)
                        wprintf(L" [PARTITION_ENTRY_UNUSED]\n");
                    else if (IsEqualGUID(temp_GPT.PartitionType, PARTITION_SYSTEM_GUID) == 1)
                        wprintf(L" [PARTITION_SYSTEM]\n");
                    else if (IsEqualGUID(temp_GPT.PartitionType, PARTITION_MSFT_RESERVED_GUID) == 1)
                        wprintf(L" [PARTITION_MSFT_RESERVED]\n");
                    else if (IsEqualGUID(temp_GPT.PartitionType, PARTITION_LDM_METADATA_GUID) == 1)
                        wprintf(L" [PARTITION_LDM_METADATA]\n");
                    else if (IsEqualGUID(temp_GPT.PartitionType, PARTITION_LDM_DATA_GUID) == 1)
                        wprintf(L" [PARTITION_LDM_DATA]\n");
                    else if (IsEqualGUID(temp_GPT.PartitionType, PARTITION_MSFT_RECOVERY_GUID) == 1)
                        wprintf(L" [PARTITION_MSFT_RECOVERY]\n");


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

                    wprintf(L"\t\tAttributes: ");
                    if (temp_GPT.Attributes == GPT_ATTRIBUTE_PLATFORM_REQUIRED)
                        wprintf(L"GPT_ATTRIBUTE_PLATFORM_REQUIRED [%llu]\n", temp_GPT.Attributes);
                    else if (temp_GPT.Attributes == GPT_BASIC_DATA_ATTRIBUTE_NO_DRIVE_LETTER)
                        wprintf(L"GPT_BASIC_DATA_ATTRIBUTE_NO_DRIVE_LETTER [%llu]\n", temp_GPT.Attributes);
                    else if (temp_GPT.Attributes == GPT_BASIC_DATA_ATTRIBUTE_HIDDEN)
                        wprintf(L"GPT_BASIC_DATA_ATTRIBUTE_HIDDEN [%llu]\n", temp_GPT.Attributes);
                    else if (temp_GPT.Attributes == GPT_BASIC_DATA_ATTRIBUTE_SHADOW_COPY)
                        wprintf(L"GPT_BASIC_DATA_ATTRIBUTE_SHADOW_COPY [%llu]\n", temp_GPT.Attributes);
                    else if (temp_GPT.Attributes == GPT_BASIC_DATA_ATTRIBUTE_READ_ONLY)
                        wprintf(L"GPT_BASIC_DATA_ATTRIBUTE_READ_ONLY [%llu]\n", temp_GPT.Attributes);
                    else
                        wprintf(L"UNKNOWN [%llu]\n", temp_GPT.Attributes);



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

    void GetSTORAGE_PREDICT_FAILURE()
    {
        STORAGE_PREDICT_FAILURE spf = { 0 };
        BOOL bResult = FALSE;
        DWORD junk = 0;

        bResult = DeviceIoControl(hDevice, // device to be queried
            IOCTL_STORAGE_PREDICT_FAILURE, // operation to perform
            0, 0,                       // no input buffer
            &spf, sizeof(spf),             // output buffer
            &junk,                         // # bytes returned
            (LPOVERLAPPED)NULL);          // synchronous I/O

        if (bResult)
        {
            smartData.PredictFailure = spf.PredictFailure;
           
            for (int aa = 0, bb = 1; aa < 512; aa++, bb++)
            {
                switch (aa)
                {
                    case 0:
                        smartData.revisionNumber[0] = *(spf.VendorSpecific + aa);
                        smartData.revisionNumber[1] = *(spf.VendorSpecific + aa + 1);
                    break;

                    case 2:
                        parseSMARTAttributes(&spf);
                    break;

                    case 362:
                        smartData.offlineDataCollectionStatus = *(spf.VendorSpecific + aa);
                    break;

                    case 363:
                        smartData.selfTestExecutionStatus = *(spf.VendorSpecific + aa);
                    break;

                    case 364:
                        smartData.Total_time_in_seconds_to_complete_off_line_data_collection_activity[0] = *(spf.VendorSpecific + aa);
                        smartData.Total_time_in_seconds_to_complete_off_line_data_collection_activity[1] = *(spf.VendorSpecific + aa + 1);
                    break;

                    case 366:
                        smartData.Reserved = *(spf.VendorSpecific + aa);
                    break;

                    case 367:
                        smartData.Offline_data_collection_capability = *(spf.VendorSpecific + aa);
                    break;

                    case 368:
                        smartData.SMART_capability[0] = *(spf.VendorSpecific + aa);
                        smartData.SMART_capability[1] = *(spf.VendorSpecific + aa + 1);
                    break;

                    case 370:
                        smartData.Error_logging_capability = *(spf.VendorSpecific + aa);
                    break;

                    case 371:
                        smartData.Self_test_Failure_Checkpoint = *(spf.VendorSpecific + aa);
                    break;

                    case 372:
                        smartData.Short_self_test_routine_recommended_polling_time_minutes = *(spf.VendorSpecific + aa);
                    break;

                    case 373:
                        smartData.Extended_self_test_routine_recommended_polling_time_minutes = *(spf.VendorSpecific + aa);
                    break;

                    case 374:
                        // currently IGNORED
                    break;

                    case 511:
                        smartData.Data_tructure_Checksum = *(spf.VendorSpecific + aa);
                    break;
                }
            }

            printSMARTdata();
        }
        else
        {
            printError(GetLastError(), L"GetTEST");
        }
    }

    
    void GetATACommandResponse_DCO_data()
    {
        BOOL bResult = FALSE;
        DWORD junk = 0;

        const unsigned int IDENTIFY_buffer_size = 512;

        unsigned char Buffer[IDENTIFY_buffer_size + sizeof(ATA_PASS_THROUGH_EX)] = { 0 };
        ATA_PASS_THROUGH_EX& PTE = *(ATA_PASS_THROUGH_EX*)Buffer;
        PTE.Length = sizeof(PTE);
        PTE.TimeOutValue = 10;
        PTE.DataTransferLength = 512;
        PTE.DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX);

        IDEREGS* ir = (IDEREGS*)PTE.CurrentTaskFile;
        ir->bFeaturesReg = 0xC2;
        ir->bCommandReg = 0xB1;
        ir->bDriveHeadReg = 1;

        // IDENTIFY is neither 48-bit nor DMA, it reads from the device:
        PTE.AtaFlags = ATA_FLAGS_DATA_IN | ATA_FLAGS_DRDY_REQUIRED;

        DWORD BR = 0;
        bResult = DeviceIoControl(hDevice, IOCTL_ATA_PASS_THROUGH, &PTE, sizeof(Buffer), &PTE, sizeof(Buffer), &BR, 0);

        if (bResult)
        {
            wprintf(L"RETURNED STATUS: \n");
            wprintf(L"ERROR: 0x%02X\n", PTE.CurrentTaskFile[0]);    // TODO: prikazi kateri error je glede na bit-e
            wprintf(L"STATUS: 0x%02X\n", PTE.CurrentTaskFile[6]);
            wprintf(L"\tBSY: %s\n", (((PTE.CurrentTaskFile[6]) >> 3) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tDRDY: %s\n", (((PTE.CurrentTaskFile[6]) >> 6) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tDRQ: %s\n", (((PTE.CurrentTaskFile[6]) >> 7) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tERR: %s\n", (((PTE.CurrentTaskFile[6]) >> 0) & 1) == 1 ? L"TRUE [some error happened]" : L"FALSE [no error]");
            wprintf(L"\n");
            wprintf(L"\n");
            wprintf(L"\n");

            wprintf(L"*****  DATA  *****\n");
            
            WORD* data = (WORD*)(Buffer + sizeof(ATA_PASS_THROUGH_EX));

            // Word 0
            wprintf(L"Data structure revision: 0x%X\n", *(data + 0));
            wprintf(L"\n");

            // Word 1
            wprintf(L"Multiword DMA modes supported: 0x%X\n", *(data + 1));
            wprintf(L"\tReporting support for Multiword DMA mode 2 and below: %s\n", (((*(data + 1)) >> 2) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Multiword DMA mode 1 and below: %s\n", (((*(data + 1)) >> 1) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\t Reporting support for Multiword DMA mode 0: %s\n", (((*(data + 1)) >> 0) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\n");

            // Word 2
            wprintf(L"Ultra DMA modes supported: 0x%X\n", *(data + 2));
            wprintf(L"\tReporting support for Ultra DMA mode 6 and below: %s\n", (((*(data + 2)) >> 6) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Ultra DMA mode 5 and below: %s\n", (((*(data + 2)) >> 5) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Ultra DMA mode 4 and below: %s\n", (((*(data + 2)) >> 4) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Ultra DMA mode 3 and below: %s\n", (((*(data + 2)) >> 3) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Ultra DMA mode 2 and below: %s\n", (((*(data + 2)) >> 2) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Ultra DMA mode 1 and below: %s\n", (((*(data + 2)) >> 1) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Ultra DMA mode 0: %s\n", (((*(data + 2)) >> 0) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\n");


            // Word 3-6
            typedef struct
            {
                USHORT ll[4];
            }LBA;
            LBA* lba = (LBA*)(data + 3);
            wprintf(L"Maximum LBA: 0x%X%X%X%X\n", lba->ll[0], lba->ll[1], lba->ll[2], lba->ll[3]);
            wprintf(L"\n");

            // Word 7
            wprintf(L"Command set/feature set supported: 0x%X - 0x%X - 0x%X\n", *(data + 7), *(data + 8), *(data + 21));
            wprintf(L"\tPART 1:\n");
            wprintf(L"\tReporting support for Write-Read-Verify feature: %s\n", (((*(data + 7)) >> 14) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for SMART Conveyance self-test: %s\n", (((*(data + 7)) >> 13) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for SMART Selective self-test: %s\n", (((*(data + 7)) >> 12) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Forced Unit Access: %s\n", (((*(data + 7)) >> 11) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Streaming feature: %s\n", (((*(data + 7)) >> 9) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for 48-bit Addressing feature: %s\n", (((*(data + 7)) >> 8) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Host Protected Area feature: %s\n", (((*(data + 7)) >> 7) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Automatic acoustic management: %s\n", (((*(data + 7)) >> 6) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for READ/WRITE DMA QUEUED commands: %s\n", (((*(data + 7)) >> 5) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Power-up in Standby feature: %s\n", (((*(data + 7)) >> 4) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Security feature: %s\n", (((*(data + 7)) >> 3) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for SMART error log: %s\n", (((*(data + 7)) >> 2) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for SMART self-test: %s\n", (((*(data + 7)) >> 1) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for SMART feature: %s\n", (((*(data + 7)) >> 0) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tPART 2:\n");
            wprintf(L"\tReporting support for software settings preservation: %s\n", (((*(data + 8)) >> 4) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for asynchronous notification: %s\n", (((*(data + 8)) >> 3) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for interface power management: %s\n", (((*(data + 8)) >> 2) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for non-zero buffer offsets: %s\n", (((*(data + 8)) >> 1) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for the NCQ feature: %s\n", (((*(data + 8)) >> 0) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tPART 3:\n");
            wprintf(L"\tReporting support for NV Cache feature: %s\n", (((*(data + 21)) >> 15) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for NV Cache Power Management feature: %s\n", (((*(data + 21)) >> 14) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for WRITE UNCORRECTABLE: %s\n", (((*(data + 21)) >> 13) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting of support for the Trusted Computing feature: %s\n", (((*(data + 21)) >> 12) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for the Free-fall Control feature: %s\n", (((*(data + 21)) >> 11) & 1) == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\n");

            // Word 208-254
            typedef struct
            {
                USHORT ll[47];
                
            }VENDOR_SPECIFIC;
            VENDOR_SPECIFIC* vs = (VENDOR_SPECIFIC*)(data + 208);
            wprintf(L"Vendor Specific: \n");
            for(int aa = 0; aa < 47; aa++)
                wprintf(L"%X ",vs->ll[aa]);
            wprintf(L"\n");
            wprintf(L"\n");

            // Word 255
            wprintf(L"Integrity: 0x%X\n", *(data + 255));
            int temp_i = *(data + 255);
            for (int aa = 8; aa < 16; aa++)
                temp_i = ((temp_i) & (~(1 << aa)));
            wprintf(L"\tSignature: 0x%X\n", temp_i);
            int temp_ii = *(data + 255);
            for (int aa = 0; aa < 4; aa++)
                temp_ii = ((temp_ii) & (~(1 << aa)));
            temp_ii = temp_ii >> 8;
            wprintf(L"\tChecksum: 0x%X\n", temp_ii);
        }
        else
        {
            printError(GetLastError(), L"GetATACommandResponse_DCO_data");
        }

    }

    void GetATACommandResponse_IDENTIFY_DEVICE_data()
    {
        
    }

    







    

    void GetVolumeInf()
    {
        wchar_t lpVolumeNameBuffer[MAX_PATH + 1];
        wchar_t lpFileSystemNameBuffer[MAX_PATH + 1];
        DWORD lpVolumeSerialNumber = 0;
        DWORD lpMaximumComponentLength = 0;
        DWORD lpFileSystemFlags = 0;
        BOOL tttt;

        wchar_t temp_s[5];
        swprintf_s(temp_s, 5, L"%wc%wc", drive_path[4], drive_path[5]);

        if(drive_path[4] == L'C')
            tttt = GetVolumeInformationW(
                NULL,
                //(LPCWSTR)L"D:",
                lpVolumeNameBuffer,
                MAX_PATH + 1,
                &lpVolumeSerialNumber,
                &lpMaximumComponentLength,
                &lpFileSystemFlags,
                lpFileSystemNameBuffer,
                MAX_PATH + 1
            );
        else
        {
            tttt = GetVolumeInformationW(
                (LPCWSTR)temp_s,         // TODO: poberi ven iz drive_path zadnja dva znaka
                lpVolumeNameBuffer,
                MAX_PATH + 1,
                &lpVolumeSerialNumber,
                &lpMaximumComponentLength,
                &lpFileSystemFlags,
                lpFileSystemNameBuffer,
                MAX_PATH + 1
            );
        }

        if (tttt == 1)
        {
            wprintf(L"Volume Name: %s\n", lpVolumeNameBuffer);
            wprintf(L"File System Name: %s\n", lpFileSystemNameBuffer);

            // TODO: preveri kako pa kaj 
            //wprintf(L"Volume serial num (programatically): %llu\n", lpVolumeSerialNumber);

            // TODO: to get hard disk's serial number that the manufacturer assigns

            // TODO: preveri kako pa kaj 
            //wprintf(L"Max supported file name length: %llu\n", lpMaximumComponentLength);

            // TODO: izpisi se po imenu kateri flag-i so
            wprintf(L"File system flags: %08X ", lpFileSystemFlags);
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

    void parseSMARTAttributes(STORAGE_PREDICT_FAILURE *spf)
    {
        for (int aa = 2, bb = 0, j = 0; aa < 362; aa++, bb++)
        {
            if (bb == 12)
            {
                bb = 0;
                j++;
            }
            
            switch (bb)
            {
                case 0:
                    smartData.Attribute[j].Id = *(spf->VendorSpecific + aa);
                break;

                case 1:
                    smartData.Attribute[j].StatusFlags[0] = *(spf->VendorSpecific + aa);
                    smartData.Attribute[j].StatusFlags[1] = *(spf->VendorSpecific + aa + 1);
                break;

                case 3:
                    smartData.Attribute[j].AttributeValue = *(spf->VendorSpecific + aa);
                break;

                case 4:
                    smartData.Attribute[j].WorstValue = *(spf->VendorSpecific + aa);
                break;

                case 5:
                    smartData.Attribute[j].RawValue[0] = *(spf->VendorSpecific + aa);
                    smartData.Attribute[j].RawValue[1] = *(spf->VendorSpecific + aa + 1);
                    smartData.Attribute[j].RawValue[2] = *(spf->VendorSpecific + aa + 2);
                    smartData.Attribute[j].RawValue[3] = *(spf->VendorSpecific + aa + 3);
                    smartData.Attribute[j].RawValue[4] = *(spf->VendorSpecific + aa + 4);
                    smartData.Attribute[j].RawValue[5] = *(spf->VendorSpecific + aa + 5);
                break;

                case 11:
                    smartData.Attribute[j].Reserved = *(spf->VendorSpecific + aa);
                break;
            }
        }
    }

    void printSMARTdata()
    {
        if(smartData.PredictFailure != 0)
            wprintf(L"device is currently predicting an imminent FAILURE!!!!!!!!!\n");
        else
            wprintf(L"NO failure is predicting!!\n");

        wprintf(L"\n");

        wprintf(L"Revision number: %d\n",  
            smartData.revisionNumber[0]
        );
        wprintf(L"\n");

        wprintf(L"ATTRIBUTES: \n");
        for (int aa = 0; aa < MAX_ATTRIBUTE; aa++)
        {
            if (smartData.Attribute[aa].Id != 0x00)
            {
                switch (smartData.Attribute[aa].Id)
                {
                    case 1:
                        wprintf(L"\tRead Error Rate:\n");
                    break;

                    case 2:
                        wprintf(L"\tThroughput Performance:\n");
                    break;

                    case 3:
                        wprintf(L"\tSpin Up Time:\n");
                    break;

                    case 4:
                        wprintf(L"\tStart/Stop Count:\n");
                    break;

                    case 5:
                        wprintf(L"\tReallocated Sector Count:\n");
                    break;

                    case 7:
                        wprintf(L"\tSeek Error Rate:\n");
                    break;

                    case 8:
                        wprintf(L"\tSeek Time Performance:\n");
                    break;

                    case 9:
                        wprintf(L"\tPower-On Hours Count:\n");
                    break;

                    case 10:
                        wprintf(L"\tSpin Retry Count:\n");
                    break;

                    case 12:
                        wprintf(L"\tDevice Power Cycle Count:\n");
                    break;

                    case 160:
                        wprintf(L"\tFree-fall Sensor Self Test Result:\n");
                    break;

                    case 191:
                        wprintf(L"\tG Sense error rate:\n");
                    break;

                    case 192:
                        wprintf(L"\tPower off retract count:\n");
                    break;

                    case 193:
                        wprintf(L"\tLoad/Unload cycle count:\n");
                    break;

                    case 194:
                        wprintf(L"\tDevice Temperature:\n");
                    break;

                    case 196:
                        wprintf(L"\tReallocation Sector Event Count:\n");
                    break;

                    case 197:
                        wprintf(L"\tCurrent Pending Sector Count:\n");
                    break;

                    case 198:
                        wprintf(L"\tOff-Line Scan Uncorrectable Sector Count:\n");
                    break;

                    case 199:
                        wprintf(L"\tUltra DMA CRC Error Count:\n");
                    break;

                    case 220:
                        wprintf(L"\tDisk Shiftt:\n");
                    break;

                    case 222:
                        wprintf(L"\tLoaded Hours:\n");
                    break;

                    case 223:
                        wprintf(L"\tLoad Retry Count:\n");
                    break;

                    case 224:
                        wprintf(L"\tLoad Friction:\n");
                    break;

                    case 226:
                        wprintf(L"\tLoad in Time:\n");
                    break;

                    case 240:
                        wprintf(L"\tWrite Head:\n");
                    break;

                    case 254:
                        wprintf(L"\tFree-fall Sensor Work Count:\n");
                    break;
                }

                wprintf(L"\t\tStatus flags:\n");
                    wprintf(L"\t\t\tpre-failure/advisory: %s\n",    (((smartData.Attribute[aa].StatusFlags[0]) >> 0) & 1) == 1 ? L"The drive may have failure." : L"Product life period may expired." );
                    wprintf(L"\t\t\ton-line data collection: %s\n", (((smartData.Attribute[aa].StatusFlags[0]) >> 1) & 1) == 1 ? L"Attribute value will be changed during normal operation." : L"Attribute value will be changed during off-line data collection operation." );
                    wprintf(L"\t\t\tPerformance Attribute: %s\n",   (((smartData.Attribute[aa].StatusFlags[0]) >> 2) & 1) == 1 ? L"TRUE" : L"FALSE");
                    wprintf(L"\t\t\tError rate attribute:\n",       (((smartData.Attribute[aa].StatusFlags[0]) >> 3) & 1) == 1 ? L"TRUE" : L"FALSE");
                    wprintf(L"\t\t\tEvent Count Attribute:\n",      (((smartData.Attribute[aa].StatusFlags[0]) >> 4) & 1) == 1 ? L"TRUE" : L"FALSE");
                    wprintf(L"\t\t\tSelf-Preserving Attribute:\n",  (((smartData.Attribute[aa].StatusFlags[0]) >> 5) & 1) == 1 ? L"TRUE" : L"FALSE");

                wprintf(L"\t\tATT value: 0x%02X [%d]\n", smartData.Attribute[aa].AttributeValue, smartData.Attribute[aa].AttributeValue);
                wprintf(L"\t\tWorst value: 0x%02X [%d]\n", smartData.Attribute[aa].WorstValue, smartData.Attribute[aa].WorstValue);
                wprintf(L"\t\tRaw value (in hex): %02X %02X %02X %02X %02X %02X\n",
                    smartData.Attribute[aa].RawValue[0],
                    smartData.Attribute[aa].RawValue[1],
                    smartData.Attribute[aa].RawValue[2],
                    smartData.Attribute[aa].RawValue[3],
                    smartData.Attribute[aa].RawValue[4],
                    smartData.Attribute[aa].RawValue[5]
                );
                wprintf(L"\t\tReserved: 0x%02X [%d]\n", smartData.Attribute[aa].Reserved, smartData.Attribute[aa].Reserved);

                wprintf(L"\n");
            }
        }
        wprintf(L"\n");
        wprintf(L"\n");

        int aaaa = ((smartData.offlineDataCollectionStatus) & (~(1 << 7)));
        wprintf(L"Off-line data collection status:\n");
        wprintf(L"\tAutomatic Off-Line Data Collection Status: ");
        switch ((((smartData.offlineDataCollectionStatus) >> 7) & 1))
        {
            case 1:
                wprintf(L"enabled\n");
            break;

            case 0:
                wprintf(L"disabled\n");
            break;
        }
        wprintf(L"\tDefinition: ");     // TODO: add other number for the same options (0x82, 0x83, ...)
        switch (aaaa)
        {
            case 0:
                wprintf(L"Off-line data collection never started.\n");
            break;

            case 2:
                wprintf(L"All segments completed without errors. In this case, current segment pointer equals to total segments required.\n");
            break;

            case 3:
                wprintf(L"Off-line activity in progress.\n");
            break;

            case 4:
                wprintf(L"Off-line data collection suspended by interrupting command\n");
            break;

            case 5:
                wprintf(L"Off-line data collecting aborted by interrupting command\n");
            break;

            case 6:
                wprintf(L"Off-line data collection aborted with fatal error\n");
            break;
        }
        wprintf(L"\n");

        wprintf(L"Self-test execution status: \n");
        int percents = smartData.selfTestExecutionStatus;
        percents = ((percents) & (~(1 << 7)));
        percents = ((percents) & (~(1 << 6)));
        percents = ((percents) & (~(1 << 5)));
        percents = ((percents) & (~(1 << 4)));
        int stes_status = smartData.selfTestExecutionStatus;
        stes_status = ((stes_status) & (~(1 << 0)));
        stes_status = ((stes_status) & (~(1 << 1)));
        stes_status = ((stes_status) & (~(1 << 2)));
        stes_status = ((stes_status) & (~(1 << 3)));
        stes_status = stes_status >> 4;
        wprintf(L"\tPercent Self-test remaining: %d%\n",percents*10);
        wprintf(L"\tCurrent Self-test execution status: ");
        switch (stes_status)
        {
            case 0:
                wprintf(L"The self-test routine completed without error or has never been run\n");
            break;

            case 1:
                wprintf(L"The self-test routine aborted by the host\n");
            break;

            case 2:
                wprintf(L"The self-test routine interrupted by the host with a hard or soft reset\n");
            break;

            case 3:
                wprintf(L"The device was unable to complete the self-test routine due to a fatal error or unknown test error\n");
            break;

            case 4:
                wprintf(L"The self-test routine completed with unknown element failure\n");
            break;

            case 5:
                wprintf(L"The self-test routine completed with electrical element failure\n");
            break;

            case 6:
                wprintf(L"The self-test routine completed with servo element failure\n");
            break;

            case 7:
                wprintf(L"The self-test routine completed with read element failure\n");
            break;

            case 15:
                wprintf(L"The self-test routine in progress\n");
            break;
        }
        wprintf(L"\n");


        // TODO: ?? which order 1->0 or 0->1
        int total_seconds = (smartData.Total_time_in_seconds_to_complete_off_line_data_collection_activity[1] << 8) | (smartData.Total_time_in_seconds_to_complete_off_line_data_collection_activity[0]);
        wprintf(L"Total time in seconds to complete off-line data collection activity: %d\n", total_seconds);
        wprintf(L"\n");

        wprintf(L"Current Segment Pointer (in hex): 0x%02X\n", smartData.Reserved);
        wprintf(L"\n");

        wprintf(L"Off-line data collection capability: %02X\n", smartData.Offline_data_collection_capability);
        wprintf(L"\tExecute Off-line Immediate implemented bit: ");
        switch ((((smartData.Offline_data_collection_capability) >> 0) & 1))
        {
            case 0:
                wprintf(L"FALSE\n");
            break;

            case 1:
                wprintf(L"TRUE\n");
            break;
        }
        wprintf(L"\tEnable/disable Automatic Off-line implemented bit: ");
        switch ((((smartData.Offline_data_collection_capability) >> 1) & 1))
        {
            case 0:
                wprintf(L"FALSE\n");
            break;

            case 1:
                wprintf(L"TRUE\n");
            break;
        }
        wprintf(L"\tabort/restart off-line by host bit: ");
        switch ((((smartData.Offline_data_collection_capability) >> 2) & 1))
        {
            case 0:
                wprintf(L"The device will suspend off-line data collection activity after an interrupting command and resume it after some vendor specific event\n");
            break;

            case 1:
                wprintf(L"The device will abort off-line data collection activity upon receipt of a new command\n");
            break;
        }
        wprintf(L"\tOff-line Read Scanning implemented bit: ");
        switch ((((smartData.Offline_data_collection_capability) >> 3) & 1))
        {
            case 0:
                wprintf(L"FALSE\n");
            break;

            case 1:
                wprintf(L"TRUE\n");
            break;
        }
        wprintf(L"\tSelf-test implemented bit: ");
        switch ((((smartData.Offline_data_collection_capability) >> 4) & 1))
        {
            case 0:
                wprintf(L"FALSE\n");
            break;

            case 1:
                wprintf(L"TRUE\n");
            break;
        }
        wprintf(L"\tSelective self-test implemented bit: ");
        switch ((((smartData.Offline_data_collection_capability) >> 6) & 1))
        {
            case 0:
                wprintf(L"FALSE\n");
            break;

            case 1:
                wprintf(L"TRUE\n");
            break;
        }
        wprintf(L"\n");


        // TODO: ?? which order 1->0 or 0->1
        wprintf(L"SMART capability: \n");
        wprintf(L"\tPre-power mode attribute saving capability: ");
        switch ((((smartData.SMART_capability[0]) >> 0) & 1))
        {
            case 0:
                wprintf(L"FALSE\n");
            break;

            case 1:
                wprintf(L"TRUE\n");
            break;
        }
        wprintf(L"\tAttribute autosave capability: ");
        switch ((((smartData.SMART_capability[0]) >> 1) & 1))
        {
            case 0:
                wprintf(L"FALSE\n");
            break;

            case 1:
                wprintf(L"TRUE\n");
            break;
        }
        wprintf(L"\n");



        wprintf(L"Error logging capability: ", smartData.Error_logging_capability);
        switch ((((smartData.Error_logging_capability) >> 0) & 1))
        {
        case 0:
            wprintf(L"FALSE\n");
            break;

        case 1:
            wprintf(L"TRUE\n");
            break;
        }
        wprintf(L"\n");



        wprintf(L"Self-test Failure Checkpointy (in hex): 0x%02X\n", smartData.Self_test_Failure_Checkpoint);
        wprintf(L"\n");


        wprintf(L"Short self-test routine recommended polling time (in minutes): %d\n", smartData.Short_self_test_routine_recommended_polling_time_minutes);
        wprintf(L"\n");

        wprintf(L"Extended self-test routine recommended polling time (in minutes): %d\n", smartData.Extended_self_test_routine_recommended_polling_time_minutes);
        wprintf(L"\n");

        wprintf(L"Data structure Checksum (in hex): 0x%02X\n", smartData.Data_tructure_Checksum);
        wprintf(L"\n");
    }

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

            case 8:
                swprintf(temp_string, 100, L"%s failed. Error %ld [ERROR_NOT_ENOUGH_MEMORY].\n", fun, err_id);
            break;

            case 31:
                swprintf(temp_string, 100, L"%s failed. Error %ld [ERROR_GEN_FAILURE]. (A device attached to the system is not functioning.)\n", fun, err_id);
            break;

            case 50:
                swprintf(temp_string, 100, L"%s failed. Error %ld [ERROR_NOT_SUPPORTED].\n", fun, err_id);
            break;

            case 53:
                swprintf(temp_string, 100, L"%s failed. Error %ld [ERROR_BAD_NETPATH].\n", fun, err_id);
            break;

            case 87:
                swprintf(temp_string, 100, L"%s failed. Error %ld [ERROR_INVALID_PARAMETER].\n", fun, err_id);
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

    static void swapbytes(char* out, char* in, size_t n)
    {
        for (size_t iii = 0; iii < n; iii += 2) {
            out[iii] = in[iii + 1];
            out[iii + 1] = in[iii];
        }
    }

    void ata_format_id_string(char* out, const unsigned char* in, int n)
    {
        bool must_swap = true;
        char tmp[65];
        n = n > 64 ? 64 : n;

        swapbytes(out, (char*)in, n);
        out[n] = '\0';
    }




};




