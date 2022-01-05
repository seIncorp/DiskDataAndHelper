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

    SMART_DATA smartData;

    LOCAL_DISK_GEOMETRY(const wchar_t drive_name[] = L"\\\\.\\PhysicalDrive0")
    {
        wcscpy_s(drive_path, 30, drive_name);

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
        if (CloseHandle(hDevice) == 0)
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
        BYTE data[10000]{ 0 };

        bResult = DeviceIoControl(hDevice, // device to be queried
            IOCTL_DISK_GET_DRIVE_LAYOUT_EX, // operation to perform
            NULL, 0,                       // no input buffer
            data, sizeof(data),             // output buffer
            &junk,                         // # bytes returned
            (LPOVERLAPPED)NULL);          // synchronous I/O

        if (bResult)
        {
            DRIVE_LAYOUT_INFORMATION_EX* test = (DRIVE_LAYOUT_INFORMATION_EX*)data;

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
                wprintf(L"\tPartition [%d]:\n", aa);

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

                wprintf(L"\t\tStarting offset: %I64d\n", bla.StartingOffset.QuadPart);

                wprintf(L"\t\tLength: %I64d B  (%.2f GB)\n", bla.PartitionLength.QuadPart, calculateToGB(bla.PartitionLength.QuadPart));
                wprintf(L"\t\tRewrite: %d\n", bla.RewritePartition);
                wprintf(L"\t\tIsServicePartition: %d\n", bla.IsServicePartition);

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
                else if (style_flag == 1)
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
                    else if (IsEqualGUID(temp_GPT.PartitionType, PARTITION_ENTRY_UNUSED_GUID) == 1)
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



    


    // TODO: change WORD array to BYTE array
    int ATACommandCall(WORD data[], BYTE* arr)
    {
        BOOL bResult = FALSE;
        DWORD junk = 0;
        const unsigned int IDENTIFY_buffer_size = 512;
        int flag_LBA = 0;

        switch (arr[6])
        {

        case 0x2f:
        case 0xc8:
        case 0xe9:
        case 0xe4:
        case 0xe1:
        case 0xe3:
        case 0xa1:
        case 0xe7:
        case 0x90:
        case 0xe5:
        case 0x78:
        case 0xEC:
        case 0xB0:
            //if (arr[0] == 0xDA)
              //  flag_LBA = 1;

        case 0xB1:  // DCO CALL
        {
            unsigned char Buffer[IDENTIFY_buffer_size + sizeof(ATA_PASS_THROUGH_EX)] = { 0 };
            ATA_PASS_THROUGH_EX& PTE = *(ATA_PASS_THROUGH_EX*)Buffer;
            PTE.Length = sizeof(PTE);
            PTE.TimeOutValue = 10;
            PTE.DataTransferLength = 512;
            PTE.DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX);

            PTE.CurrentTaskFile[0] = arr[0];
            PTE.CurrentTaskFile[1] = arr[1];
            PTE.CurrentTaskFile[2] = arr[2];
            PTE.CurrentTaskFile[3] = arr[3];
            PTE.CurrentTaskFile[4] = arr[4];
            PTE.CurrentTaskFile[5] = arr[5];
            PTE.CurrentTaskFile[6] = arr[6];
            PTE.CurrentTaskFile[7] = arr[7];

            PTE.AtaFlags = ATA_FLAGS_DATA_IN | ATA_FLAGS_DRDY_REQUIRED;
            //PTE.AtaFlags = ATA_FLAGS_DATA_IN | ATA_FLAGS_DRDY_REQUIRED | ATA_FLAGS_48BIT_COMMAND;

            DWORD BR = 0;
            bResult = DeviceIoControl(hDevice, IOCTL_ATA_PASS_THROUGH, &PTE, sizeof(Buffer), &PTE, sizeof(Buffer), &BR, 0);

            if (bResult)
            {
                printResponseData(PTE.CurrentTaskFile, 1, (wchar_t*)L"asd123");

                for (int aa = 0; aa < 256; aa++)
                    data[aa] = *((WORD*)(Buffer + sizeof(ATA_PASS_THROUGH_EX)) + aa);

                return 0;
            }
            else
            {
                printError(GetLastError(), L"GetATACommandResponse_DCO_data");

                return 1;
            }
        }
        break;

        case 0xF8:  // HPA - READ NATIVE MAX ADDRESS
        {
            const unsigned int IDENTIFY_buffer_size = 512;

            unsigned char Buffer[10 + sizeof(ATA_PASS_THROUGH_EX)] = { 0 };
            ATA_PASS_THROUGH_EX& PTE = *(ATA_PASS_THROUGH_EX*)Buffer;
            PTE.Length = sizeof(PTE);
            PTE.TimeOutValue = 10;
            PTE.DataTransferLength = 10;
            PTE.DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX);

            PTE.CurrentTaskFile[0] = arr[0];
            PTE.CurrentTaskFile[1] = arr[1];
            PTE.CurrentTaskFile[2] = arr[2];
            PTE.CurrentTaskFile[3] = arr[3];
            PTE.CurrentTaskFile[4] = arr[4];
            PTE.CurrentTaskFile[5] = arr[5];
            PTE.CurrentTaskFile[6] = arr[6];
            PTE.CurrentTaskFile[7] = arr[7];

            PTE.AtaFlags = ATA_FLAGS_DATA_IN | ATA_FLAGS_DRDY_REQUIRED;

            DWORD BR = 0;
            bResult = DeviceIoControl(hDevice, IOCTL_ATA_PASS_THROUGH, &PTE, sizeof(Buffer), &PTE, sizeof(Buffer), &BR, 0);

            if (bResult)
            {
                printResponseData(PTE.CurrentTaskFile, 1, (wchar_t*)L"Max address");

                return 0;
            }
            else
            {
                printError(GetLastError(), L"GetATACommandResponse_DCO_data");

                return 1;
            }
        }
        break;
        }
    }

    void GetATACommandResponse_DCO_data()
    {
        ATA_COMMAND dco_get{ 0x00 };
        dco_get.bFeaturesReg = 0xC2;
        dco_get.bDriveHeadReg = 0x01;
        dco_get.bCommandReg = 0xB1;

        WORD data[256];
        if (ATACommandCall(data, (BYTE*)&dco_get) == 0)
        {
            wprintf(L"*****  DATA  *****\n");

            DCO_DATA* dco_data = (DCO_DATA*)data;

            wprintf(L"Data structure revision: 0x%X\n", dco_data->revision);
            wprintf(L"\n");

            wprintf(L"Multiword DMA modes supported: 0x%X\n", dco_data->Multiword_DMA_modes_supported);
            wprintf(L"\tReporting support for Multiword DMA mode 2 and below: %s\n", dco_data->Multiword_DMA_modes_supported.a2 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Multiword DMA mode 1 and below: %s\n", dco_data->Multiword_DMA_modes_supported.a1 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Multiword DMA mode 0: %s\n", dco_data->Multiword_DMA_modes_supported.a0 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\n");

            wprintf(L"Ultra DMA modes supported: 0x%X\n", dco_data->Ultra_DMA_modes_supported);
            wprintf(L"\tReporting support for Ultra DMA mode 6 and below: %s\n", dco_data->Ultra_DMA_modes_supported.a6 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Ultra DMA mode 5 and below: %s\n", dco_data->Ultra_DMA_modes_supported.a5 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Ultra DMA mode 4 and below: %s\n", dco_data->Ultra_DMA_modes_supported.a4 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Ultra DMA mode 3 and below: %s\n", dco_data->Ultra_DMA_modes_supported.a3 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Ultra DMA mode 2 and below: %s\n", dco_data->Ultra_DMA_modes_supported.a2 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Ultra DMA mode 1 and below: %s\n", dco_data->Ultra_DMA_modes_supported.a1 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Ultra DMA mode 0: %s\n", dco_data->Ultra_DMA_modes_supported.a0 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\n");

            wprintf(L"Maximum LBA (highest address by the factory default): 0x%X%X%X%X\n", dco_data->Maximum_LBA[3], dco_data->Maximum_LBA[2], dco_data->Maximum_LBA[1], dco_data->Maximum_LBA[0]);
            wprintf(L"\n");

            wprintf(L"Command set/feature set supported: 0x%X - 0x%X - 0x%X\n", dco_data->Command_set_feature_set_supported_part1, dco_data->Serial_ATA_Comm_set_feature_set_supported_part2, dco_data->Command_set_feature_set_supported_part3);
            wprintf(L"\tPART 1:\n");
            wprintf(L"\tReporting support for Write-Read-Verify feature: %s\n", dco_data->Command_set_feature_set_supported_part1.a14 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for SMART Conveyance self-test: %s\n", dco_data->Command_set_feature_set_supported_part1.a13 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for SMART Selective self-test: %s\n", dco_data->Command_set_feature_set_supported_part1.a12 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Forced Unit Access: %s\n", dco_data->Command_set_feature_set_supported_part1.a11 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Streaming feature: %s\n", dco_data->Command_set_feature_set_supported_part1.a9 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for 48-bit Addressing feature: %s\n", dco_data->Command_set_feature_set_supported_part1.a8 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Host Protected Area feature: %s\n", dco_data->Command_set_feature_set_supported_part1.a7 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Automatic acoustic management: %s\n", dco_data->Command_set_feature_set_supported_part1.a6 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for READ/WRITE DMA QUEUED commands: %s\n", dco_data->Command_set_feature_set_supported_part1.a5 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Power-up in Standby feature: %s\n", dco_data->Command_set_feature_set_supported_part1.a4 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for Security feature: %s\n", dco_data->Command_set_feature_set_supported_part1.a3 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for SMART error log: %s\n", dco_data->Command_set_feature_set_supported_part1.a2 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for SMART self-test: %s\n", dco_data->Command_set_feature_set_supported_part1.a1 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for SMART feature: %s\n", dco_data->Command_set_feature_set_supported_part1.a0 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tPART 2:\n");
            wprintf(L"\tReporting support for software settings preservation: %s\n", dco_data->Serial_ATA_Comm_set_feature_set_supported_part2.a4 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for asynchronous notification: %s\n", dco_data->Serial_ATA_Comm_set_feature_set_supported_part2.a3 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for interface power management: %s\n", dco_data->Serial_ATA_Comm_set_feature_set_supported_part2.a2 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for non-zero buffer offsets: %s\n", dco_data->Serial_ATA_Comm_set_feature_set_supported_part2.a1 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for the NCQ feature: %s\n", dco_data->Serial_ATA_Comm_set_feature_set_supported_part2.a0 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tPART 3:\n");
            wprintf(L"\tReporting support for NV Cache feature: %s\n", dco_data->Command_set_feature_set_supported_part3.a15 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for NV Cache Power Management feature: %s\n", dco_data->Command_set_feature_set_supported_part3.a14 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for WRITE UNCORRECTABLE: %s\n", dco_data->Command_set_feature_set_supported_part3.a13 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting of support for the Trusted Computing feature: %s\n", dco_data->Command_set_feature_set_supported_part3.a12 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tReporting support for the Free-fall Control feature: %s\n", dco_data->Command_set_feature_set_supported_part3.a11 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\n");

            wprintf(L"Vendor Specific: \n");
            for (int aa = 0; aa < 47; aa++)
                wprintf(L"0x%X ", dco_data->Vendor_Specific[aa]);
            wprintf(L"\n");
            wprintf(L"\n");

            wprintf(L"Integrity: 0x%X\n", dco_data->Integrity_word);
            wprintf(L"\tSignature: 0x%X\n", dco_data->Integrity_word.signature);
            wprintf(L"\tChecksum: 0x%X\n", dco_data->Integrity_word.checksum);
        }
    }

    void GetATACommandResponse_HPA_data()
    {
        ATA_COMMAND hpa_get{ 0x00 };
        hpa_get.bDriveHeadReg = 0x01;
        hpa_get.bCommandReg = 0xF8;

        WORD data[256];
        if (ATACommandCall(data, (BYTE*)&hpa_get) == 0)
        {

        }
    }

    void GetATACommandResponse_IDENTIFY_DEVICE_data()
    {
        // TODO: add for PATA and SATA version
        
        ATA_COMMAND id_get{ 0x00 };
        id_get.bDriveHeadReg = 0x01;
        id_get.bCommandReg = 0xEC;

        WORD data[256];
        if (ATACommandCall(data, (BYTE*)&id_get) == 0)
        {
            // TODO: add if statments for bits: shall be cleared to zero and shall be set to one
            // TODO: add all validation for data

            wprintf(L"*****  DATA  *****\n");

            IDENTIFY_DEVICE_DATA* id_data = (IDENTIFY_DEVICE_DATA*)data;

            char SerialNumber[21];
            ata_format_id_string(SerialNumber, id_data->Serial_number, sizeof(SerialNumber) - 1);
            wprintf(L"Serial number: %S\n", SerialNumber);

            char FirmwareRevision[9];
            ata_format_id_string(FirmwareRevision, id_data->Firmware_revision, sizeof(FirmwareRevision) - 1);
            printf("Firmware Revision: %s\n", FirmwareRevision);

            char ModelNumber[41];
            ata_format_id_string(ModelNumber, id_data->Model_number, sizeof(ModelNumber) - 1);
            printf("Model number: %s\n", ModelNumber);
            wprintf(L"\n");

            wprintf(L"General configuration: 0x%X  --> 0x%X\n", *(data + 0), id_data->General_configuration);
            wprintf(L"\tResponse incomplete: %s\n", id_data->General_configuration.a2 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tATA device: %s\n", id_data->General_configuration.a15 == 0 ? L"TRUE" : L"FALSE");

            wprintf(L"Specific configuration: 0x%X  --> 0x%X\n", *(data + 2), id_data->Specific_configuration);
            switch (id_data->Specific_configuration)
            {
            case 0x37C8:
                wprintf(L"\tDevice requires SET FEATURES subcommand to spin-up after power-up and IDENTIFY DEVICE data is incomplet\n");
                break;

            case 0x738C:
                wprintf(L"\t Device requires SET FEATURES subcommand to spin-up after power-up and IDENTIFY DEVICE data is complete\n");
                break;

            case 0x8C73:
                wprintf(L"\tDevice does not require SET FEATURES subcommand to spin-up after power - up and IDENTIFY DEVICE data is incomplete\n");
                break;

            case 0xC837:
                wprintf(L"\tDevice does not require SET FEATURES subcommand to spin-up after power - up and IDENTIFY DEVICE data is complete\n");
                break;
            }
            wprintf(L"\n");


            wprintf(L"READ / WRITE MULTIPLE support: %X -->  %X [%lu]\n", *(data + 47), id_data->Mandatory_1, id_data->Mandatory_1.a1);
            wprintf(L"\n");
            wprintf(L"Trusted Computing feature: %X -->  %X [%s]\n", *(data + 48), id_data->Trusted_Computing_feature, id_data->Trusted_Computing_feature.a0 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\n");
            wprintf(L"Capabilities: %X - %X -->  %X - %X\n", *(data + 49), *(data + 50), id_data->Capabilities_1, id_data->Capabilities_2);
            wprintf(L"\tLong Physical Sector Alignment Error reporting: %d\n", id_data->Capabilities_1.a0);
            wprintf(L"\tDMA supported: %s\n", id_data->Capabilities_1.a8 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tLBA supported: %s\n", id_data->Capabilities_1.a9 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tIORDY: %s\n", id_data->Capabilities_1.a10 == 1 ? L"may be disabled" : L"may NOT be disabled");
            wprintf(L"\tIORDY: %s\n", id_data->Capabilities_1.a11 == 1 ? L"supported" : L"NOT supported");
            wprintf(L"\t%s\n", id_data->Capabilities_1.a13 == 1 ? L"Standby timer values as specified in this standard are supported" : L"Standby timer values shall be managed by the device");
            wprintf(L"\t%s\n", id_data->Capabilities_2.a0 == 1 ? L"There is a minimum Standby time value and it is vendor specific" : L"There is no minimum Standby timer value");

            wprintf(L"\n");
            wprintf(L"Free-fall Control Sensitivity: %X -->  %X\n", *(data + 53), id_data->Free_fall_Control);
            wprintf(L"\n");
            wprintf(L"Multiple_sector_setting: %X -->  %X\n", *(data + 59), id_data->Multiple_sector_setting);
            if (id_data->Multiple_sector_setting.a8 == 1)
            {
                wprintf(L"\tnumber of logical sectors that shall be transferred per DRQ data block: %lu\n", id_data->Multiple_sector_setting.a0);
            }
            wprintf(L"\tSanitize antifreeze lock ext command: %s\n", id_data->Multiple_sector_setting.a10 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\t%s\n", id_data->Multiple_sector_setting.a11 == 1 ? L"The commands allowed during a sanitize operation are as specified by this standard" : L"The commands allowed during a sanitize operation are as specified by ACS - 2");
            wprintf(L"\tSANITIZE SUPPORTED: %s\n", id_data->Multiple_sector_setting.a12 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tCRYPTO SCRAMBLE EXT command: %s\n", id_data->Multiple_sector_setting.a13 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tOVERWRITE EXT command: %s\n", id_data->Multiple_sector_setting.a14 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tBLOCK ERASE EXT command: %s\n", id_data->Multiple_sector_setting.a15 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\n");

            wprintf(L"Total number of user addressable logical sectors (28-bit commands): 0x%04X%04X\n", id_data->Total_n_user_addr_log_sectors[1], id_data->Total_n_user_addr_log_sectors[0]);
            wprintf(L"\n");

            wprintf(L"Multiword DMA transfer: %X -->  %X\n", *(data + 63), id_data->Multiword_DMA_transfer);
            wprintf(L"\tMultiword DMA mode 0: %s\n", id_data->Multiword_DMA_transfer.a0 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tMultiword DMA mode 1 and below: %s\n", id_data->Multiword_DMA_transfer.a1 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tMultiword DMA mode 2 and below: %s\n", id_data->Multiword_DMA_transfer.a2 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tMultiword DMA mode 0: %s\n", id_data->Multiword_DMA_transfer.a8 == 1 ? L"selected" : L"not selected");
            wprintf(L"\tMultiword DMA mode 1: %s\n", id_data->Multiword_DMA_transfer.a9 == 1 ? L"selected" : L"not selected");
            wprintf(L"\tMultiword DMA mode 2: %s\n", id_data->Multiword_DMA_transfer.a10 == 1 ? L"selected" : L"not selected");
            wprintf(L"\n");

            if (id_data->Free_fall_Control.a1 == 1)
            {
                wprintf(L"PIO transfer modes supported: %X -->  %X\n", *(data + 64), id_data->PIO_transfer_modes_supported);
                wprintf(L"\tPIO mode 3 supported: %s\n", id_data->PIO_transfer_modes_supported.a0 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tPIO mode 4 supported: %s\n", id_data->PIO_transfer_modes_supported.a1 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\n");

                wprintf(L"Minimum Multiword DMA transfer cycle time per word: %d -->  %d nanoseconds\n", *(data + 65), id_data->Min_Mult_DMA_trans_cycle_time_per_word);
                wprintf(L"\n");

                wprintf(L"Device recommended Multiword DMA cycle time: %d -->  %d nanoseconds\n", *(data + 66), id_data->Man_recomm_Multi_DMA_trans_cycle_time);
                wprintf(L"\n");

                wprintf(L"Minimum PIO transfer cycle time without IORDY flow control: %d -->  %d nanoseconds\n", *(data + 67), id_data->Min_PIO_trans_cycle_time_without_flow_con);
                wprintf(L"\n");

                wprintf(L"Minimum PIO transfer cycle time with IORDY flow control: %d -->  %d nanoseconds\n", *(data + 68), id_data->Min_PIO_trans_cycle_time_with_IORDY_flow_con);
                wprintf(L"\n");
            }

            wprintf(L"Additional Supported: %X -->  %X\n", *(data + 69), id_data->Additional_Supported);
            wprintf(L"\tAll write cache is non-volatile: %s\n", id_data->Additional_Supported.a2 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tExtended Number of User Addressable Sectors: %s\n", id_data->Additional_Supported.a3 == 1 ? L"supported" : L"NOT supported");
            wprintf(L"\tDevice Encrypts All User Data on the device: %s\n", id_data->Additional_Supported.a4 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\t Trimmed LBA range(s) returning zeroed data: %s\n", id_data->Additional_Supported.a5 == 1 ? L"supported" : L"NOT supported");
            wprintf(L"\tOptional ATA device 28-bit commands: %s\n", id_data->Additional_Supported.a6 == 1 ? L"NOT supported" : L"supported");
            wprintf(L"\tDOWNLOAD MICROCODE DMA: %s\n", id_data->Additional_Supported.a8 == 1 ? L"supported" : L"NOT supported");
            wprintf(L"\tWRITE BUFFER DMA: %s\n", id_data->Additional_Supported.a10 == 1 ? L"supported" : L"NOT supported");
            wprintf(L"\tREAD BUFFER DMA: %s\n", id_data->Additional_Supported.a11 == 1 ? L"supported" : L"NOT supported");
            wprintf(L"\tLong Physical Sector Alignment Error Reporting Control: %s\n", id_data->Additional_Supported.a13 == 1 ? L"supported" : L"NOT supported");
            wprintf(L"\tDeterministic data in trimmed LBA range(s): %s\n", id_data->Additional_Supported.a14 == 1 ? L"supported" : L"NOT supported");
            wprintf(L"\n");

            // TODO: add if statment about showing it 
            wprintf(L"Queue depth: %X -->  %X [%d]\n", *(data + 75), id_data->Queue_depth, id_data->Queue_depth);
            wprintf(L"\n");

            if (*(data + 76) != 0x0000 || *(data + 76) != 0xFFFF)
            {
                wprintf(L"Serial ATA Capabilities: %X -->  %X\n", *(data + 76), id_data->Serial_ATA_Capabilities);
                wprintf(L"\tSupports SATA Gen1 Signaling Speed (1.5Gb/s): %s\n", id_data->Serial_ATA_Capabilities.a1 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tSupports SATA Gen2 Signaling Speed (3.0Gb/s): %s\n", id_data->Serial_ATA_Capabilities.a2 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tSupports SATA Gen3 Signaling Speed (6.0Gb/s): %s\n", id_data->Serial_ATA_Capabilities.a3 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tSupports the NCQ feature set: %s\n", id_data->Serial_ATA_Capabilities.a8 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tSupports receipt of host initiated power management requests: %s\n", id_data->Serial_ATA_Capabilities.a9 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\t Supports the SATA Phy Event Counters log: %s\n", id_data->Serial_ATA_Capabilities.a10 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tSupports Unload while NCQ commands are outstanding: %s\n", id_data->Serial_ATA_Capabilities.a11 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tSupports NCQ priority information: %s\n", id_data->Serial_ATA_Capabilities.a12 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tSupports Host Automatic Partial to Slumber transitions: %s\n", id_data->Serial_ATA_Capabilities.a13 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tSupports Device Automatic Partial to Slumber transitions: %s\n", id_data->Serial_ATA_Capabilities.a14 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tSupports READ LOG DMA EXT: %s\n", id_data->Serial_ATA_Capabilities.a15 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\n");

                wprintf(L"Serial ATA Additional Capabilities: %X -->  %X\n", *(data + 77), id_data->Serial_ATA_Additional_Capabilities);
                wprintf(L"\tCURRENT SERIAL ATA SIGNAL SPEED: %d\n", id_data->Serial_ATA_Additional_Capabilities.a1);
                wprintf(L"\tSupports NCQ Streaming: %s\n", id_data->Serial_ATA_Additional_Capabilities.a4 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tSupports NCQ Queue Management Command: %s\n", id_data->Serial_ATA_Additional_Capabilities.a5 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tSupports RECEIVE FPDMA QUEUED and SEND FPDMA QUEUED commands: %s\n", id_data->Serial_ATA_Additional_Capabilities.a6 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\n");

                wprintf(L"Serial ATA features supported: %X -->  %X\n", *(data + 78), id_data->SATA_Features_Supported);
                wprintf(L"\tDevice supports non-zero buffer offsets: %s\n", id_data->SATA_Features_Supported.a1 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tDevice supports DMA Setup auto-activation: %s\n", id_data->SATA_Features_Supported.a2 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tDevice supports initiating power management: %s\n", id_data->SATA_Features_Supported.a3 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tDevice supports in-order data delivery: %s\n", id_data->SATA_Features_Supported.a4 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tDevice supports Hardware Feature Control: %s\n", id_data->SATA_Features_Supported.a5 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tDevice supports Software Settings Preservation: %s\n", id_data->SATA_Features_Supported.a6 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tDevice supports NCQ Autosense: %s\n", id_data->SATA_Features_Supported.a7 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\n");

                wprintf(L"Serial ATA features enabled: %X -->  %X\n", *(data + 79), id_data->SATA_Features_Enabled);
                wprintf(L"\tNon-zero buffer offsets: %s\n", id_data->SATA_Features_Enabled.a1 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tDMA Setup auto-activation: %s\n", id_data->SATA_Features_Enabled.a2 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tDevice initiated power management: %s\n", id_data->SATA_Features_Enabled.a3 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tIn-order data delivery: %s\n", id_data->SATA_Features_Enabled.a4 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tHardware Feature Control: %s\n", id_data->SATA_Features_Enabled.a5 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tSoftware Settings Preservation: %s\n", id_data->SATA_Features_Enabled.a6 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tAutomatic Partial to Slumber transitions: %s\n", id_data->SATA_Features_Enabled.a7 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\n");
            }

            if (*(data + 80) != 0x0000 || *(data + 80) != 0xFFFF)
            {
                wprintf(L"Major version number: %X -->  %X\n", *(data + 80), id_data->Major_revision_number);
                wprintf(L"\tsupports ATA/ATAPI-5: %s\n", id_data->Major_revision_number.a5 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tsupports ATA/ATAPI-6: %s\n", id_data->Major_revision_number.a6 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tsupports ATA/ATAPI-7: %s\n", id_data->Major_revision_number.a7 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tsupports ATA8-ACS: %s\n", id_data->Major_revision_number.a8 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tsupports ACS-2: %s\n", id_data->Major_revision_number.a9 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\tsupports ACS-3: %s\n", id_data->Major_revision_number.a10 == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\n");
            }

            wprintf(L"Minor version number: %X -->  %X\n", *(data + 81), id_data->Minor_revision_number);
            MinorVersionNumberPrint(id_data->Minor_revision_number);
            wprintf(L"\n");









            wprintf(L"Commands and feature sets supported: %X -->  %X\n", *(data + 84), id_data->Commandsand_feature_sets_supported_3);
            wprintf(L"\t0: %02X\n", id_data->Commandsand_feature_sets_supported_3.a0);
            wprintf(L"\t1: %02X\n", id_data->Commandsand_feature_sets_supported_3.a1);
            wprintf(L"\t4: %02X\n", id_data->Commandsand_feature_sets_supported_3.a4);
            wprintf(L"\t5: %02X\n", id_data->Commandsand_feature_sets_supported_3.a5);
            wprintf(L"\t6: %02X\n", id_data->Commandsand_feature_sets_supported_3.a6);
            wprintf(L"\t8: %02X\n", id_data->Commandsand_feature_sets_supported_3.a8);
            wprintf(L"\t13: %02X\n", id_data->Commandsand_feature_sets_supported_3.a13);
            wprintf(L"\t14: %02X\n", id_data->Commandsand_feature_sets_supported_3.a14);
            wprintf(L"\t15: %02X\n", id_data->Commandsand_feature_sets_supported_3.a15);

            wprintf(L"\t--5: %02X\n", id_data->Command_set_feature_enabled_supported_3.a5);


            // TODO: continue...


            wprintf(L"\n");
            wprintf(L"\n");
            wprintf(L"\n");
            wprintf(L"\n");













            wprintf(L"\n");














        }
    }

    /***********************************************************************************************/
    /* SMART functions (Self-Monitoring, Analysis, and Reporting Technology (SMART) feature set)  */
    void GetATACommandResponse_SMART_READ_data()    // DONE!! <-- dodaj se za app komando
    {
        ATA_COMMAND id_get{ 0x00 };

        id_get.bFeaturesReg = 0xD0;
        id_get.bSectorCountReg = 0x00;
        id_get.bSectorNumberReg = 0x00;
        id_get.bCylLowReg = 0x4F;
        id_get.bCylHighReg = 0xC2;
        id_get.bDriveHeadReg = 0x00;
        id_get.bCommandReg = 0xB0;
        

        
        

        BYTE data[512]{ 0 };
        if (ATACommandCall((WORD *)data, (BYTE*)&id_get) == 0)
        {
            /*for (int ii = 0; ii < 512; ii++)
                wprintf(L"%X ", *(data+ii));
            ee;
            ee;*/

            //data[363] = 0xc3;

            ATA_SMART_DATA* asd = (ATA_SMART_DATA*)data;


            

            wprintf(L"Vendor specific: \n");
            for (int ii = 0, aa = 0; ii < 362; ii++, aa++)
            {
                if (aa == 30)
                {
                    ee;
                    aa = 0;
                }
                wprintf(L"0x%02X ", asd->vendor_specific_1[ii]);
            }
            ee;


            // TODO: add option to show vendor specific
            wprintf(L"Off-line collection status: 0x%02X\n", asd->Offline_data_collection_status);
            switch (asd->Offline_data_collection_status)
            {
                case 0x00:

                case 0x80:
                    wprintf(L"\tOff-line data collection activity was never started.\n");
                break;

                case 0x01:
                    wprintf(L"\tReserved\n");
                break;

                case 0x02:
                case 0x82:
                    wprintf(L"\tOff-line data collection activity was completed without error.\n");
                break;

                case 0x03:
                    wprintf(L"\tOff-line activity in progress.\n");
                break;

                case 0x04:
                case 0x84:
                    wprintf(L"\tOff-line data collection activity was suspended by an interrupting command from host.\n");
                break;

                case 0x05:
                case 0x85:
                    wprintf(L"\tOff-line data collection activity was aborted by an interrupting command from host.\n");
                break;

                case 0x06:
                case 0x86:
                    wprintf(L"\tOff-line data collection activity was aborted by the device with a fatal error.\n");
                break;
            }
            wprintf(L"\n");

            wprintf(L"Self-test execution status: 0x%02X\n", asd->SelfTest_execution_status);
            wprintf(L"\tPercent Self-Test Remaining: [0x%02X] %d\n", asd->SelfTest_execution_status.a0, asd->SelfTest_execution_status.a0);
            wprintf(L"\tSelf-test Execution Status: [0x%02X]\n", asd->SelfTest_execution_status.a4);
            switch (asd->SelfTest_execution_status.a4)
            {
                case 0x00:
                    wprintf(L"\t\tIndicates a previous self-test routine completed without error or no self-test status is available\n");
                break;

                case 0x01:
                    wprintf(L"\t\tThe self-test routine was aborted by the host\n");
                break;

                case 0x02:
                    wprintf(L"\t\tThe self-test routine was interrupted by the host with a hardware or software reset\n");
                break;

                case 0x03:
                    wprintf(L"\t\tA fatal error or unknown test error occurred while the device was executing its self-test routine and the device was unable to complete the self - test routine\n");
                break;

                case 0x04:
                    wprintf(L"\t\tThe previous self-test completed having a test element that failed and the test element that failed is not known.\n");
                break;

                case 0x05:
                    wprintf(L"\t\t The previous self-test completed having the electrical element of the test failed.\n");
                break;

                case 0x06:
                    wprintf(L"\t\tThe previous self-test completed having the servo and/or seek test element of the test failed.\n");
                break;

                case 0x07:
                    wprintf(L"\t\tThe previous self-test completed having the read element of the test failed.\n");
                break;

                case 0x08:
                    wprintf(L"\t\tThe previous self-test completed having a test element that failed and the device is suspected of having handling damage.\n");
                break;

                case 0x0F:
                    wprintf(L"\t\tSelf-test routine in progress.\n");
                break;
            }
            wprintf(L"\n");
            
            wprintf(L"Off-line data collection capability: 0x%02X\n", asd->Offline_data_collection_capability);
            wprintf(L"\tSMART EXECUTE OFF-LINE IMMEDIATE: [%s]\n", asd->Offline_data_collection_capability.a0 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tABORT/RESTART OFF-LINE BY HOST: [%s]\n", asd->Offline_data_collection_capability.a2 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tOFF-LINE READ SCANNING IMPLEMENTED: [%s]\n", asd->Offline_data_collection_capability.a3 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tSELF-TEST IMPLEMENTED: [%s]\n", asd->Offline_data_collection_capability.a4 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tCONVEYANCE SELF-TEST IMPLEMENTED: [%s]\n", asd->Offline_data_collection_capability.a5 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tSELECTIVE SELF-TEST IMPLEMENTED: [%s]\n", asd->Offline_data_collection_capability.a6 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\n");

            wprintf(L"SMART capability: 0x%02X%02X\n", asd->SMART_capability_2, asd->SMART_capability_1);
            wprintf(L"\tdevice saves SMART data prior to going into a power saving mode (i.e., Idle, Standby, or Sleep) or upon return to Active mode or Idle mode from a Standby mode: [%s]\n", asd->SMART_capability_1.a0 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\tSMART ENABLE/DISABLE ATTRIBUTE AUTOSAVE: [%s]\n", asd->SMART_capability_1.a1 == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\n");

            wprintf(L"Error logging capability: [%s]\n", asd->Err_logging_capability == 1 ? L"TRUE" : L"FALSE");
            wprintf(L"\n");

            wprintf(L"Short self-test routine recommended polling time (in minutes): %d\n", asd->ShortSelfTest_routine_recommended_polling_time_minutes);
            wprintf(L"\n");

            wprintf(L"Extended self-test routine recommended polling time in minutes: %d\n", asd->ExtSelfRest_routine_recommended_polling_time_minutes);
            wprintf(L"\n");

            wprintf(L"Conveyance self-test routine recommended polling time in minutes: %d\n", asd->ConveyanceSelfTest_routine_recommended_polling_time_minutes);
            wprintf(L"\n");

            wprintf(L"Extended self-test routine recommended polling time in minutes: 0x%02X%02X\n", asd->ExtSelfTest_routine_recommended_polling_time_minutes[0], asd->ExtSelfTest_routine_recommended_polling_time_minutes[0]);
            wprintf(L"\n");

            wprintf(L"Vendor specific: \n");
            for (int ii = 0, aa = 0; ii < 125; ii++, aa++)
            {
                if (aa == 30)
                {
                    ee;
                    aa = 0;
                }
                wprintf(L"0x%02X ", asd->vendor_specific_5[ii]);
            }
            wprintf(L"\n");

            wprintf(L"Data structure checksum: 0x%02X\n", asd->Data_structure_checksum);
            wprintf(L"\n");
        }
    }

    void GetATACommandResponse_SMART_READ_LOG_data()
    {
        ATA_COMMAND id_get{ 0x00 };

        id_get.bFeaturesReg = 0xD5;         // 0
        id_get.bSectorCountReg = 0x01;      // 1    <-- number of log pages 
        id_get.bSectorNumberReg = 0x04;     // 2    <-- LOG ADDRESS field
        id_get.bCylLowReg = 0x4F;           // 3
        id_get.bCylHighReg = 0xC2;          // 4
        id_get.bDriveHeadReg = 0x01;        // 5
        id_get.bCommandReg = 0xB0;          // 6

        // TODO: naredi tabelo za  Log directory
        // TODO: razmisli kaj bi s tem naredil glede: SCT Command/Status, SCT Data Transfer, HOST specific read data
        // TODO: naredi se za ostale loge: Device Statistics, ...

        BYTE data[512];
        if (ATACommandCall((WORD*)data, (BYTE*)&id_get) == 0)
        {
            if (id_get.bSectorNumberReg == 0x00)
            {
                for (int ii = 2, a = 0, c = 1; ii < 512; ii++, a++)
                {
                    if (a == 10)
                    {
                        a = 0;
                        ee;
                    }

                    if (*(data + ii) > 0x00)
                    {
                        wprintf(L"[BYTE: %d - ADD: %d (0x%02X)] -> 0x%02X\n", ii, c, c, *(data + ii));
                    }

                    if (ii % 2 == 0)
                        c++;
                }
                ee;
                ee;
                
                SMART_LOG_DIRECTORY* sld = (SMART_LOG_DIRECTORY*)data;

                wprintf(L"Number of log pages at log address 1: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_1, data[2]);
                wprintf(L"Number of log pages at log address 2: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_2, data[4]);
                wprintf(L"Number of log pages at log address 3: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_3, data[6]);
                wprintf(L"Number of log pages at log address 4: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_4, data[8]);
                wprintf(L"Number of log pages at log address 5: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_5, data[10]);
                wprintf(L"Number of log pages at log address 6: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_6, data[12]);
                wprintf(L"Number of log pages at log address 7: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_6, data[14]);
                wprintf(L"Number of log pages at log address 8: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_6, data[16]);
                wprintf(L"Number of log pages at log address 9: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_6, data[18]);
                wprintf(L"Number of log pages at log address 10: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_6, data[20]);
                wprintf(L"Number of log pages at log address 11: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_6, data[22]);
                wprintf(L"Number of log pages at log address 12: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_6, data[24]);
                wprintf(L"Number of log pages at log address 13: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_6, data[26]);
                wprintf(L"Number of log pages at log address 14: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_6, data[28]);
                wprintf(L"Number of log pages at log address 15: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_6, data[30]);
                wprintf(L"Number of log pages at log address 16: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_6, data[32]);
                wprintf(L"Number of log pages at log address 17: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_6, data[34]);
                wprintf(L"Number of log pages at log address 18: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_6, data[36]);
                wprintf(L"Number of log pages at log address 19: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_6, data[38]);
                wprintf(L"Number of log pages at log address 20: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_6, data[40]);
                wprintf(L"Number of log pages at log address 21: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_6, data[42]);
                wprintf(L"Number of log pages at log address 22: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_6, data[44]);
                wprintf(L"Number of log pages at log address 23: 0x%02X  0x%02X\n", sld->num_log_pages_at_log_address_6, data[46]);
                wprintf(L"Number of log pages at log address 24:   0x%02X\n", data[48]);
                wprintf(L"Number of log pages at log address 25:   0x%02X\n", data[50]);
                wprintf(L"Number of log pages at log address 26:   0x%02X\n", data[52]);
                wprintf(L"Number of log pages at log address 27:   0x%02X\n", data[54]);
                wprintf(L"Number of log pages at log address 28:   0x%02X\n", data[56]);
                wprintf(L"Number of log pages at log address 29:   0x%02X\n", data[58]);
                wprintf(L"Number of log pages at log address 30:   0x%02X\n", data[60]);
                wprintf(L"Number of log pages at log address 31:   0x%02X\n", data[62]);
                wprintf(L"Number of log pages at log address 32:   0x%02X\n", data[64]);
                wprintf(L"Number of log pages at log address 33:   0x%02X\n", data[66]);
                wprintf(L"Number of log pages at log address 34:   0x%02X\n", data[68]);
                wprintf(L"Number of log pages at log address 35:   0x%02X\n", data[70]);
                wprintf(L"Number of log pages at log address 36:   0x%02X\n", data[72]);
                wprintf(L"Number of log pages at log address 37:   0x%02X\n", data[74]);
                wprintf(L"Number of log pages at log address 38:   0x%02X\n", data[76]);
                wprintf(L"Number of log pages at log address 39:   0x%02X\n", data[78]);
                wprintf(L"Number of log pages at log address 40:   0x%02X\n", data[80]);
                wprintf(L"Number of log pages at log address 41:   0x%02X\n", data[82]);
            }
            else
            {
                for (int ii = 0, a = 0; ii < 512; ii++, a++)
                {
                    if (a == 30)
                    {
                        a = 0;
                        ee;
                    }
                    wprintf(L"%02X ", *(data + ii));
                }
                ee;
                ee;


            }

        }
    }

    // TODO: SMART EXECUTE OFF-LINE IMMEDIATE (p. 280)

    void GetATACommandResponse_SMART_RETURN_STATUS_data()
    {
        ATA_COMMAND id_get{ 0x00 };

        id_get.bFeaturesReg = 0xDA;         // 0
        id_get.bSectorCountReg = 0x00;      // 1    <-- number of log pages 
        id_get.bSectorNumberReg = 0x00;     // 2    <-- LOG ADDRESS field
        id_get.bCylLowReg = 0x4F;           // 3
        id_get.bCylHighReg = 0xC2;          // 4
        id_get.bDriveHeadReg = 0x01;        // 5
        id_get.bCommandReg = 0xB0;          // 6

        // TODO: naredi tabelo za  Log directory
        // TODO: razmisli kaj bi s tem naredil glede: SCT Command/Status, SCT Data Transfer, HOST specific read data
        // TODO: naredi se za ostale loge: Device Statistics, ...

        BYTE data[512];
        if (ATACommandCall((WORD*)data, (BYTE*)&id_get) == 0)
        {
            /*for (int ii = 0, a = 0; ii < 512; ii++, a++)
            {
                if (a == 30)
                {
                    a = 0;
                    ee;
                }
                wprintf(L"%02X ", *(data + ii));
            }
            ee;
            ee;*/
        }

    }

    // TODO: SMART WRITE LOG (p. 291)


    /***********************************************************************************************/
    /* Accessible Max Address Configuration  */

    // TODO: GET NATIVE MAX ADDRESS EXT (p. 98)
    void GetATACommandResponse_GET_NATIVE_MAX_ADDRESS_EXT_data()
    {
        ATA_COMMAND id_get{ 0x00 };

        id_get.bFeaturesReg = 0x00;         // 0
        id_get.bSectorCountReg = 0x00;      // 1    <-- number of log pages 
        id_get.bSectorNumberReg = 0x00;     // 2    <-- LOG ADDRESS field
        id_get.bCylLowReg = 0x00;           // 3
        id_get.bCylHighReg = 0x00;          // 4
        id_get.bDriveHeadReg = 0x01;        // 5
        id_get.bCommandReg = 0x78;          // 6

        // TODO: naredi tabelo za  Log directory
        // TODO: razmisli kaj bi s tem naredil glede: SCT Command/Status, SCT Data Transfer, HOST specific read data
        // TODO: naredi se za ostale loge: Device Statistics, ...

        BYTE data[512];
        if (ATACommandCall((WORD*)data, (BYTE*)&id_get) == 0)
        {
            /*for (int ii = 0, a = 0; ii < 512; ii++, a++)
            {
                if (a == 30)
                {
                    a = 0;
                    ee;
                }
                wprintf(L"%02X ", *(data + ii));
            }
            ee;
            ee;*/
        }

    }
     
     
    // TODO: SET ACCESSIBLE MAX ADDRESS EXT (p. 99)
    // TODO: FREEZE ACCESSIBLE MAX ADDRESS EXT (p. 100)

    /***********************************************************************************************/
    /* OTHER FUNCTIONS  */

    void GetATACommandResponse_CHECK_POWER_MODE_data()
    {
        ATA_COMMAND id_get{ 0x00 };

        id_get.bFeaturesReg = 0x00;         // 0
        id_get.bSectorCountReg = 0x00;      // 1    <-- number of log pages 
        id_get.bSectorNumberReg = 0x00;     // 2    <-- LOG ADDRESS field
        id_get.bCylLowReg = 0x00;           // 3
        id_get.bCylHighReg = 0x00;          // 4
        id_get.bDriveHeadReg = 0x01;        // 5
        id_get.bCommandReg = 0xe5;          // 6

        

        BYTE data[512];
        if (ATACommandCall((WORD*)data, (BYTE*)&id_get) == 0)
        {
            /*for (int ii = 0, a = 0; ii < 512; ii++, a++)
            {
                if (a == 30)
                {
                    a = 0;
                    ee;
                }
                wprintf(L"%02X ", *(data + ii));
            }
            ee;
            ee;*/
        }

    }

    void GetATACommandResponse_EXECUTE_DEVICE_DIAGNOSTIC_data()
    {
        ATA_COMMAND id_get{ 0x00 };

        id_get.bFeaturesReg = 0x00;         // 0
        id_get.bSectorCountReg = 0x00;      // 1    <-- number of log pages 
        id_get.bSectorNumberReg = 0x00;     // 2    <-- LOG ADDRESS field
        id_get.bCylLowReg = 0x00;           // 3
        id_get.bCylHighReg = 0x00;          // 4
        id_get.bDriveHeadReg = 0x01;        // 5
        id_get.bCommandReg = 0x90;          // 6

       

        BYTE data[512];
        if (ATACommandCall((WORD*)data, (BYTE*)&id_get) == 0)
        {
            /*for (int ii = 0, a = 0; ii < 512; ii++, a++)
            {
                if (a == 30)
                {
                    a = 0;
                    ee;
                }
                wprintf(L"%02X ", *(data + ii));
            }
            ee;
            ee;*/
        }

    }

    void GetATACommandResponse_FLUSH_CACHE_data()
    {
        ATA_COMMAND id_get{ 0x00 };

        id_get.bFeaturesReg = 0x00;         // 0
        id_get.bSectorCountReg = 0x00;      // 1    <-- number of log pages 
        id_get.bSectorNumberReg = 0x00;     // 2    <-- LOG ADDRESS field
        id_get.bCylLowReg = 0x00;           // 3
        id_get.bCylHighReg = 0x00;          // 4
        id_get.bDriveHeadReg = 0x01;        // 5
        id_get.bCommandReg = 0xe7;          // 6

        

        BYTE data[512];
        if (ATACommandCall((WORD*)data, (BYTE*)&id_get) == 0)
        {
            /*for (int ii = 0, a = 0; ii < 512; ii++, a++)
            {
                if (a == 30)
                {
                    a = 0;
                    ee;
                }
                wprintf(L"%02X ", *(data + ii));
            }
            ee;
            ee;*/
        }

    }

    void GetATACommandResponse_IDLE_data()
    {
        ATA_COMMAND id_get{ 0x00 };

        id_get.bFeaturesReg = 0x00;         // 0
        id_get.bSectorCountReg = 0x00;      // 1    <-- number of log pages 
        id_get.bSectorNumberReg = 0x00;     // 2    <-- LOG ADDRESS field
        id_get.bCylLowReg = 0x00;           // 3
        id_get.bCylHighReg = 0x00;          // 4
        id_get.bDriveHeadReg = 0x01;        // 5
        id_get.bCommandReg = 0xe3;          // 6

        // TODO: dodaj se za timer

        BYTE data[512];
        if (ATACommandCall((WORD*)data, (BYTE*)&id_get) == 0)
        {
            /*for (int ii = 0, a = 0; ii < 512; ii++, a++)
            {
                if (a == 30)
                {
                    a = 0;
                    ee;
                }
                wprintf(L"%02X ", *(data + ii));
            }
            ee;
            ee;*/
        }

    }

    void GetATACommandResponse_IDLE_IMMEDIATE_data()
    {
        ATA_COMMAND id_get{ 0x00 };

        id_get.bFeaturesReg = 0x00;         // 0
        id_get.bSectorCountReg = 0x00;      // 1    <-- number of log pages 
        id_get.bSectorNumberReg = 0x00;     // 2    <-- LOG ADDRESS field
        id_get.bCylLowReg = 0x00;           // 3
        id_get.bCylHighReg = 0x00;          // 4
        id_get.bDriveHeadReg = 0x01;        // 5
        id_get.bCommandReg = 0xe1;          // 6

        // TODO: dodaj se za unload

        BYTE data[512];
        if (ATACommandCall((WORD*)data, (BYTE*)&id_get) == 0)
        {
            /*for (int ii = 0, a = 0; ii < 512; ii++, a++)
            {
                if (a == 30)
                {
                    a = 0;
                    ee;
                }
                wprintf(L"%02X ", *(data + ii));
            }
            ee;
            ee;*/
        }

    }
    
    /***********************************************************************************************/
    /* READ FUNCTIONS  */


    void GetATACommandResponse_READ_BUFFER_data()
    {
        ATA_COMMAND id_get{ 0x00 };

        id_get.bFeaturesReg = 0x00;         // 0
        id_get.bSectorCountReg = 0x00;      // 1    <-- number of log pages 
        id_get.bSectorNumberReg = 0x00;     // 2    <-- LOG ADDRESS field
        id_get.bCylLowReg = 0x00;           // 3
        id_get.bCylHighReg = 0x00;          // 4
        id_get.bDriveHeadReg = 0x01;        // 5
        id_get.bCommandReg = 0xe4;          // 6

        // TODO: dodaj se za unload

        BYTE data[512];
        if (ATACommandCall((WORD*)data, (BYTE*)&id_get) == 0)
        {
            for (int ii = 0, a = 0; ii < 512; ii++, a++)
            {
                if (a == 30)
                {
                    a = 0;
                    ee;
                }
                wprintf(L"%02X ", *(data + ii));
            }
            ee;
            ee;
        }

    }

    void GetATACommandResponse_READ_DMA_data()
    {
        ATA_COMMAND id_get{ 0x00 };

        id_get.bFeaturesReg = 0x00;         // 0
        id_get.bSectorCountReg = 0x01;      // 1    <-- number of log pages 
        id_get.bSectorNumberReg = 0x0a;     // 2    <-- LOG ADDRESS field
        id_get.bCylLowReg = 0x00;           // 3
        id_get.bCylHighReg = 0x00;          // 4
        id_get.bDriveHeadReg = 0x01;        // 5
        id_get.bCommandReg = 0xc8;          // 6

        // TODO: ugotovi prave velikosti logical sector-ja

        BYTE data[512];
        if (ATACommandCall((WORD*)data, (BYTE*)&id_get) == 0)
        {
            for (int ii = 0, a = 0; ii < 512; ii++, a++)
            {
                if (a == 30)
                {
                    a = 0;
                    ee;
                }
                wprintf(L"%02X ", *(data + ii));
            }
            ee;
            ee;
        }

    }

    // TODO: READ DMA EXT

    // TODO: READ FPDMA QUEUED


    int ATACommandCall_v2(BYTE data_out[], int data_out_size, ATA_COMMAND PTF, ATA_COMMAND CTF, int is48bit)
    {
        BOOL bResult = FALSE;
        DWORD junk = 0;
        const int size = 512;


        BYTE Buffer[size + sizeof(ATA_PASS_THROUGH_EX)] = { 0 };
        

        ATA_PASS_THROUGH_EX& APTE = *(ATA_PASS_THROUGH_EX*)Buffer;
        APTE.Length = sizeof(APTE);
        APTE.TimeOutValue = 10;
        APTE.DataTransferLength = data_out_size;
        APTE.DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX);

        APTE.CurrentTaskFile[0] = CTF.bFeaturesReg;
        APTE.CurrentTaskFile[1] = CTF.bSectorCountReg;
        APTE.CurrentTaskFile[2] = CTF.bSectorNumberReg;
        APTE.CurrentTaskFile[3] = CTF.bCylLowReg;
        APTE.CurrentTaskFile[4] = CTF.bCylHighReg;
        APTE.CurrentTaskFile[5] = CTF.bDriveHeadReg;
        APTE.CurrentTaskFile[6] = CTF.bCommandReg;
        APTE.CurrentTaskFile[7] = CTF.bReserved;

        if (is48bit == 1)
        {
            APTE.PreviousTaskFile[0] = PTF.bFeaturesReg;
            APTE.PreviousTaskFile[1] = PTF.bSectorCountReg;
            APTE.PreviousTaskFile[2] = PTF.bSectorNumberReg;  
            APTE.PreviousTaskFile[3] = PTF.bCylLowReg;  
            APTE.PreviousTaskFile[4] = PTF.bCylHighReg;  
            APTE.PreviousTaskFile[5] = PTF.bDriveHeadReg;
            APTE.PreviousTaskFile[6] = PTF.bCommandReg;
            APTE.PreviousTaskFile[7] = PTF.bReserved;
        }


        APTE.AtaFlags = ATA_FLAGS_DATA_IN | ATA_FLAGS_DRDY_REQUIRED;

        if (is48bit == 1)
            APTE.AtaFlags = APTE.AtaFlags | ATA_FLAGS_48BIT_COMMAND;

        //std::cout << sizeof(APTE) << "  " << buffer_size << std::endl;

        DWORD BR = 0;
        //bResult = DeviceIoControl(hDevice, IOCTL_ATA_PASS_THROUGH, &APTE, sizeof(APTE), &APTE, sizeof(Buffer), &BR, 0);
        bResult = DeviceIoControl(hDevice, IOCTL_ATA_PASS_THROUGH, &APTE, sizeof(Buffer), &APTE, sizeof(Buffer), &BR, 0);
        if (bResult)
        {
            //  TODO: raje vrni pa se tam printa
            printResponseData(APTE.CurrentTaskFile, 1, (wchar_t*)L"asd123");

            for (int aa = 0; aa < (data_out_size); aa++)
                data_out[aa] = *((Buffer + sizeof(ATA_PASS_THROUGH_EX)) + aa);
            

            //free(Buffer);
            return 1;
        }
        else
        {
            printError(GetLastError(), L"ATACommandCall_v2");
            
            //free(Buffer);
            return 0;
        }

    }

    void GetATACommandResponse_READ_LOG_EXT_data(ATA_COMMAND PTF, ATA_COMMAND CTF, int is48bit)
    {
        BYTE data[512];
 
        if (ATACommandCall_v2(data, 512, PTF, CTF, 1) == 1)
        {
            
           for (int ii = 0, a = 0; ii < 512; ii++, a++)
            {
                if (a == 8)
                {
                    a = 0;
                    ee;
                }
                wprintf(L"%02X ", *(data + ii));
            }
            ee;
            ee;
            
            // TODO: naredi tako da se kliče samo tiste log-e ki so podprti

            if (CTF.bCylLowReg == List_of_supported_log_pages)
            {
                LIST_OF_SUPPORTED_LOGS* gs = (LIST_OF_SUPPORTED_LOGS*)data;

                wprintf(L"Device Statistics Information Header:   0x%016llX\n", gs->Device_Statistics_Information_Header);
                wprintf(L"\tRevision number: %lu [0x%llX]\n", gs->Device_Statistics_Information_Header.q1, gs->Device_Statistics_Information_Header.q1);
                wprintf(L"\tLog page number: %d [0x%llX]\n", gs->Device_Statistics_Information_Header.q2, gs->Device_Statistics_Information_Header.q2);
               
                wprintf(L"Number of entries (n) in the following list:   0x%X\n", gs->Number_of_entries_n_in_the_following_list);
                
                for (int ii = 0; ii < gs->Number_of_entries_n_in_the_following_list; ii++)
                {
                    wprintf(L"\tLog page number of %d supported Device Statistics log page (0x%02X):   0x%X\n", ii, ii, gs->Log_page_number[ii]);
                }


                wprintf(L"\n");
            }

            if (CTF.bCylLowReg == General_Statistics)
            {
                GENERAL_STATISTICS* gs = (GENERAL_STATISTICS*)data;

                wprintf(L"Device Statistics Information Header:   0x%016llX\n", gs->Device_Statistics_Information_Header);
                wprintf(L"\tRevision number: %lu [0x%X]\n", gs->Device_Statistics_Information_Header.w1, gs->Device_Statistics_Information_Header.w1);
                wprintf(L"\tLog page number: %d [0x%X]\n", gs->Device_Statistics_Information_Header.b3, gs->Device_Statistics_Information_Header.b3);
                wprintf(L"\n");

                wprintf(L"Lifetime Power-On Resets:   0x%016llX\n", gs->Lifetime_Power_On_Resets);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Lifetime_Power_On_Resets.a7,
                    gs->Lifetime_Power_On_Resets.a6,
                    gs->Lifetime_Power_On_Resets.a5,
                    gs->Lifetime_Power_On_Resets.a4,
                    gs->Lifetime_Power_On_Resets.a3
                );
                wprintf(L"\tNumber of times that the device has processed a Power-On Reset event: %llu [0x%X]\n", gs->Lifetime_Power_On_Resets.d1, gs->Lifetime_Power_On_Resets.d1);
                wprintf(L"\n");

                wprintf(L"Power-on Hours:   0x%016llX\n", gs->Power_on_Hours);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Power_on_Hours.a7,
                    gs->Power_on_Hours.a6,
                    gs->Power_on_Hours.a5,
                    gs->Power_on_Hours.a4,
                    gs->Power_on_Hours.a3
                );
                wprintf(L"\tPower-on Hours: %llu [0x%X]\n", gs->Power_on_Hours.d1, gs->Power_on_Hours.d1);
                wprintf(L"\n");

                wprintf(L"Logical Sectors Written:   0x%016llX\n", gs->Logical_Sectors_Written);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Logical_Sectors_Written.a7,
                    gs->Logical_Sectors_Written.a6,
                    gs->Logical_Sectors_Written.a5,
                    gs->Logical_Sectors_Written.a4,
                    gs->Logical_Sectors_Written.a3
                );
                wprintf(L"\tLogical Sectors Written: %llu [0x%llX]\n", gs->Logical_Sectors_Written.dw1, gs->Logical_Sectors_Written.dw1);
                wprintf(L"\n");

                wprintf(L"Number of Write Commands:   0x%016llX\n", gs->Number_of_Write_Commands);
                wprintf(L"\tDEVICE STATISTICS FLAGS:\n");
                printfDeviceStatistics(
                    gs->Number_of_Write_Commands.a7,
                    gs->Number_of_Write_Commands.a6,
                    gs->Number_of_Write_Commands.a5,
                    gs->Number_of_Write_Commands.a4,
                    gs->Number_of_Write_Commands.a3
                );
                wprintf(L"\tNumber of Write Commands: %llu [0x%llX]\n", gs->Number_of_Write_Commands.dw1, gs->Number_of_Write_Commands.dw1);
                wprintf(L"\n");

                wprintf(L"Logical Sectors Read:   0x%016llX\n", gs->Logical_Sectors_Read);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Logical_Sectors_Read.a7,
                    gs->Logical_Sectors_Read.a6,
                    gs->Logical_Sectors_Read.a5,
                    gs->Logical_Sectors_Read.a4,
                    gs->Logical_Sectors_Read.a3
                );
                wprintf(L"\tLogical Sectors Read: %llu [0x%llX]\n", gs->Logical_Sectors_Read.dw1, gs->Logical_Sectors_Read.dw1);
                wprintf(L"\n");

                wprintf(L"Number of Read Commands:   0x%016llX\n", gs->Number_of_Read_Commands);
                wprintf(L"\tDEVICE STATISTICS FLAGS:\n");
                printfDeviceStatistics(
                    gs->Number_of_Read_Commands.a7,
                    gs->Number_of_Read_Commands.a6,
                    gs->Number_of_Read_Commands.a5,
                    gs->Number_of_Read_Commands.a4,
                    gs->Number_of_Read_Commands.a3
                );
                wprintf(L"\tNumber of Read Commands: %llu [0x%llX]\n", gs->Number_of_Read_Commands.dw1, gs->Number_of_Read_Commands.dw1);
                wprintf(L"\n");

                wprintf(L"Date and Time TimeStamp:   0x%016llX\n", gs->Date_and_Time_TimeStamp);
                wprintf(L"\tDEVICE STATISTICS FLAGS:\n");
                printfDeviceStatistics(
                    gs->Date_and_Time_TimeStamp.a7,
                    gs->Date_and_Time_TimeStamp.a6,
                    gs->Date_and_Time_TimeStamp.a5,
                    gs->Date_and_Time_TimeStamp.a4,
                    gs->Date_and_Time_TimeStamp.a3
                );
                wprintf(L"\tDate and Time TimeStamp: %llu [0x%llX]\n", gs->Date_and_Time_TimeStamp.dw1, gs->Date_and_Time_TimeStamp.dw1);
                wprintf(L"\n");
            }

            if (CTF.bCylLowReg == Free_Fall_Statistics)
            {
                FREE_FALL_STATISTICS* gs = (FREE_FALL_STATISTICS*)data;


            }

            if (CTF.bCylLowReg == Rotating_Media_Statistics)
            {
                ROTATING_MEDIA_STATISTICS* gs = (ROTATING_MEDIA_STATISTICS*)data;

                wprintf(L"Device Statistics Information Header:   0x%016llX\n", gs->Device_Statistics_Information_Header);
                wprintf(L"\tRevision number: %lu [0x%X]\n", gs->Device_Statistics_Information_Header.q1, gs->Device_Statistics_Information_Header.q1);
                wprintf(L"\tLog page number: %d [0x%X]\n", gs->Device_Statistics_Information_Header.q2, gs->Device_Statistics_Information_Header.q2);
                wprintf(L"\n");

                wprintf(L"Spindle Motor Power-on Hours:   0x%016llX\n", gs->Spindle_Motor_Power_on_Hours);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Spindle_Motor_Power_on_Hours.a7,
                    gs->Spindle_Motor_Power_on_Hours.a6,
                    gs->Spindle_Motor_Power_on_Hours.a5,
                    gs->Spindle_Motor_Power_on_Hours.a4,
                    gs->Spindle_Motor_Power_on_Hours.a3
                );
                wprintf(L"\tSpindle Motor Power-on Hours: %llu [0x%X]\n", gs->Spindle_Motor_Power_on_Hours.q1, gs->Spindle_Motor_Power_on_Hours.q1);
                wprintf(L"\n");

                wprintf(L"Head Flying Hours:   0x%016llX\n", gs->Head_Flying_Hours);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Head_Flying_Hours.a7,
                    gs->Head_Flying_Hours.a6,
                    gs->Head_Flying_Hours.a5,
                    gs->Head_Flying_Hours.a4,
                    gs->Head_Flying_Hours.a3
                );
                wprintf(L"\tHead Flying Hours: %llu [0x%X]\n", gs->Head_Flying_Hours.q1, gs->Head_Flying_Hours.q1);
                wprintf(L"\n");

                wprintf(L"Head Load Events:   0x%016llX\n", gs->Head_Load_Events);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Head_Load_Events.a7,
                    gs->Head_Load_Events.a6,
                    gs->Head_Load_Events.a5,
                    gs->Head_Load_Events.a4,
                    gs->Head_Load_Events.a3
                );
                wprintf(L"\tHead Load Events: %llu [0x%X]\n", gs->Head_Load_Events.q1, gs->Head_Load_Events.q1);
                wprintf(L"\n");

                wprintf(L"Number of Reallocated Logical Sectors:   0x%016llX\n", gs->Number_of_Reallocated_Logical_Sectors);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Number_of_Reallocated_Logical_Sectors.a7,
                    gs->Number_of_Reallocated_Logical_Sectors.a6,
                    gs->Number_of_Reallocated_Logical_Sectors.a5,
                    gs->Number_of_Reallocated_Logical_Sectors.a4,
                    gs->Number_of_Reallocated_Logical_Sectors.a3
                );
                wprintf(L"\tNumber of Reallocated Logical Sectors: %llu [0x%X]\n", gs->Number_of_Reallocated_Logical_Sectors.q1, gs->Number_of_Reallocated_Logical_Sectors.q1);
                wprintf(L"\n");

                wprintf(L"Read Recovery Attempts:   0x%016llX\n", gs->Read_Recovery_Attempts);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Read_Recovery_Attempts.a7,
                    gs->Read_Recovery_Attempts.a6,
                    gs->Read_Recovery_Attempts.a5,
                    gs->Read_Recovery_Attempts.a4,
                    gs->Read_Recovery_Attempts.a3
                );
                wprintf(L"\tRead Recovery Attempts: %llu [0x%X]\n", gs->Read_Recovery_Attempts.q1, gs->Read_Recovery_Attempts.q1);
                wprintf(L"\n");

                wprintf(L"Number of Mechanical Start Failures:   0x%016llX\n", gs->Number_of_Mechanical_Start_Failures);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Number_of_Mechanical_Start_Failures.a7,
                    gs->Number_of_Mechanical_Start_Failures.a6,
                    gs->Number_of_Mechanical_Start_Failures.a5,
                    gs->Number_of_Mechanical_Start_Failures.a4,
                    gs->Number_of_Mechanical_Start_Failures.a3
                );
                wprintf(L"\tNumber of Mechanical Start Failures: %llu [0x%X]\n", gs->Number_of_Mechanical_Start_Failures.q1, gs->Number_of_Mechanical_Start_Failures.q1);
                wprintf(L"\n");

                wprintf(L"Number of Reallocation Candidate Logical Sectors:   0x%016llX\n", gs->Number_of_Reallocation_Candidate_Logical_Sectors);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Number_of_Reallocation_Candidate_Logical_Sectors.a7,
                    gs->Number_of_Reallocation_Candidate_Logical_Sectors.a6,
                    gs->Number_of_Reallocation_Candidate_Logical_Sectors.a5,
                    gs->Number_of_Reallocation_Candidate_Logical_Sectors.a4,
                    gs->Number_of_Reallocation_Candidate_Logical_Sectors.a3
                );
                wprintf(L"\tNumber of Reallocation Candidate Logical Sectors: %llu [0x%X]\n", gs->Number_of_Reallocation_Candidate_Logical_Sectors.q1, gs->Number_of_Reallocation_Candidate_Logical_Sectors.q1);
                wprintf(L"\n");

                wprintf(L"Number of High Priority Unload Events:   0x%016llX\n", gs->Number_of_High_Priority_Unload_Events);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Number_of_High_Priority_Unload_Events.a7,
                    gs->Number_of_High_Priority_Unload_Events.a6,
                    gs->Number_of_High_Priority_Unload_Events.a5,
                    gs->Number_of_High_Priority_Unload_Events.a4,
                    gs->Number_of_High_Priority_Unload_Events.a3
                );
                wprintf(L"\tNumber of High Priority Unload Events: %llu [0x%X]\n", gs->Number_of_High_Priority_Unload_Events.q1, gs->Number_of_High_Priority_Unload_Events.q1);
                wprintf(L"\n");
            }

            if (CTF.bCylLowReg == General_Errors_Statistics)
            {
                GENERAL_ERROR_STATISTICS* gs = (GENERAL_ERROR_STATISTICS*)data;

                wprintf(L"Device Statistics Information Header:   0x%016llX\n", gs->Device_Statistics_Information_Header);
                wprintf(L"\tRevision number: %lu [0x%X]\n", gs->Device_Statistics_Information_Header.q1, gs->Device_Statistics_Information_Header.q1);
                wprintf(L"\tLog page number: %d [0x%X]\n", gs->Device_Statistics_Information_Header.q2, gs->Device_Statistics_Information_Header.q2);
                wprintf(L"\n");

                wprintf(L"Number of Reported Uncorrectable Errors:   0x%016llX\n", gs->Number_of_Reported_Uncorrectable_Errors);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Number_of_Reported_Uncorrectable_Errors.a7,
                    gs->Number_of_Reported_Uncorrectable_Errors.a6,
                    gs->Number_of_Reported_Uncorrectable_Errors.a5,
                    gs->Number_of_Reported_Uncorrectable_Errors.a4,
                    gs->Number_of_Reported_Uncorrectable_Errors.a3
                );
                wprintf(L"\tNumber of Reported Uncorrectable Errors: %llu [0x%X]\n", gs->Number_of_Reported_Uncorrectable_Errors.q1, gs->Number_of_Reported_Uncorrectable_Errors.q1);
                wprintf(L"\n");

                wprintf(L"Number of Resets Between Command Acceptance and Command Completion:   0x%016llX\n", gs->Number_of_Resets_Between_Command_Acceptanceand_Command_Completion);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Number_of_Resets_Between_Command_Acceptanceand_Command_Completion.a7,
                    gs->Number_of_Resets_Between_Command_Acceptanceand_Command_Completion.a6,
                    gs->Number_of_Resets_Between_Command_Acceptanceand_Command_Completion.a5,
                    gs->Number_of_Resets_Between_Command_Acceptanceand_Command_Completion.a4,
                    gs->Number_of_Resets_Between_Command_Acceptanceand_Command_Completion.a3
                );
                wprintf(L"\tNumber of Resets Between Command Acceptance and Command Completion: %llu [0x%X]\n", gs->Number_of_Resets_Between_Command_Acceptanceand_Command_Completion.q1, gs->Number_of_Resets_Between_Command_Acceptanceand_Command_Completion.q1);
                wprintf(L"\n");
            }

            if (CTF.bCylLowReg == Temperature_Statistics)
            {
                TEMPERATURE_STATISTICS* gs = (TEMPERATURE_STATISTICS*)data;
                WCHAR cc = 248;

                // TODO: preveri ce so prave enote za vsak prikaz

                wprintf(L"Device Statistics Information Header:   0x%016llX\n", gs->Device_Statistics_Information_Header);
                wprintf(L"\tRevision number: %lu [0x%X]\n", gs->Device_Statistics_Information_Header.q1, gs->Device_Statistics_Information_Header.q1);
                wprintf(L"\tLog page number: %d [0x%X]\n", gs->Device_Statistics_Information_Header.q2, gs->Device_Statistics_Information_Header.q2);
                wprintf(L"\n");

                wprintf(L"Current Temperature:   0x%016llX\n", gs->Current_Temperature);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Current_Temperature.a7,
                    gs->Current_Temperature.a6,
                    gs->Current_Temperature.a5,
                    gs->Current_Temperature.a4,
                    gs->Current_Temperature.a3
                );
                wprintf(L"\tCurrent Temperature: %d\370C [0x%llX]\n", gs->Current_Temperature.q0,gs->Current_Temperature.q0);
                wprintf(L"\n");

                wprintf(L"Average Short Term Temperature:   0x%016llX\n", gs->Average_Short_Term_Temperature);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Average_Short_Term_Temperature.a7,
                    gs->Average_Short_Term_Temperature.a6,
                    gs->Average_Short_Term_Temperature.a5,
                    gs->Average_Short_Term_Temperature.a4,
                    gs->Average_Short_Term_Temperature.a3
                );
                wprintf(L"\tAverage Short Term Temperature: %d\370C [0x%llX]\n", gs->Average_Short_Term_Temperature.q0, gs->Average_Short_Term_Temperature.q0);
                wprintf(L"\n");

                wprintf(L"Average Long Term Temperature:   0x%016llX\n", gs->Average_Long_Term_Temperature);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Average_Long_Term_Temperature.a7,
                    gs->Average_Long_Term_Temperature.a6,
                    gs->Average_Long_Term_Temperature.a5,
                    gs->Average_Long_Term_Temperature.a4,
                    gs->Average_Long_Term_Temperature.a3
                );
                wprintf(L"\tAverage Long Term Temperature: %d\370C [0x%llX]\n", gs->Average_Long_Term_Temperature.q0, gs->Average_Long_Term_Temperature.q0);
                wprintf(L"\n");

                wprintf(L"Highest Temperature:   0x%016llX\n", gs->Highest_Temperature);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Highest_Temperature.a7,
                    gs->Highest_Temperature.a6,
                    gs->Highest_Temperature.a5,
                    gs->Highest_Temperature.a4,
                    gs->Highest_Temperature.a3
                );
                wprintf(L"\tHighest Temperature: %d\370C [0x%llX]\n", gs->Highest_Temperature.q0, gs->Highest_Temperature.q0);
                wprintf(L"\n");

                wprintf(L"Lowest Temperature:   0x%016llX\n", gs->Lowest_Temperature);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Lowest_Temperature.a7,
                    gs->Lowest_Temperature.a6,
                    gs->Lowest_Temperature.a5,
                    gs->Lowest_Temperature.a4,
                    gs->Lowest_Temperature.a3
                );
                wprintf(L"\tLowest Temperature: %d\370C [0x%llX]\n", gs->Lowest_Temperature.q0, gs->Lowest_Temperature.q0);
                wprintf(L"\n");

                wprintf(L"Highest Average Short Term Temperature:   0x%016llX\n", gs->Highest_Average_Short_Term_Temperature);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Highest_Average_Short_Term_Temperature.a7,
                    gs->Highest_Average_Short_Term_Temperature.a6,
                    gs->Highest_Average_Short_Term_Temperature.a5,
                    gs->Highest_Average_Short_Term_Temperature.a4,
                    gs->Highest_Average_Short_Term_Temperature.a3
                );
                wprintf(L"\tHighest Average Short Term Temperature: %d\370C [0x%llX]\n", gs->Highest_Average_Short_Term_Temperature.q0, gs->Highest_Average_Short_Term_Temperature.q0);
                wprintf(L"\n");

                wprintf(L"Lowest Average Short Term Temperature:   0x%016llX\n", gs->Lowest_Average_Short_Term_Temperature);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Lowest_Average_Short_Term_Temperature.a7,
                    gs->Lowest_Average_Short_Term_Temperature.a6,
                    gs->Lowest_Average_Short_Term_Temperature.a5,
                    gs->Lowest_Average_Short_Term_Temperature.a4,
                    gs->Lowest_Average_Short_Term_Temperature.a3
                );
                wprintf(L"\tLowest Average Short Term Temperature: %d\370C [0x%llX]\n", gs->Lowest_Average_Short_Term_Temperature.q0, gs->Lowest_Average_Short_Term_Temperature.q0);
                wprintf(L"\n");

                wprintf(L"Highest Average Long Term Temperature:   0x%016llX\n", gs->Highest_Average_Long_Term_Temperature);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Highest_Average_Long_Term_Temperature.a7,
                    gs->Highest_Average_Long_Term_Temperature.a6,
                    gs->Highest_Average_Long_Term_Temperature.a5,
                    gs->Highest_Average_Long_Term_Temperature.a4,
                    gs->Highest_Average_Long_Term_Temperature.a3
                );
                wprintf(L"\tHighest Average Long Term Temperature: %d\370C [0x%llX]\n", gs->Highest_Average_Long_Term_Temperature.q0, gs->Highest_Average_Long_Term_Temperature.q0);
                wprintf(L"\n");

                wprintf(L"Lowest Average Long Term Temperature:   0x%016llX\n", gs->Lowest_Average_Long_Term_Temperature);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Lowest_Average_Long_Term_Temperature.a7,
                    gs->Lowest_Average_Long_Term_Temperature.a6,
                    gs->Lowest_Average_Long_Term_Temperature.a5,
                    gs->Lowest_Average_Long_Term_Temperature.a4,
                    gs->Lowest_Average_Long_Term_Temperature.a3
                );
                wprintf(L"\tLowest Average Long Term Temperature: %d\370C [0x%llX]\n", gs->Lowest_Average_Long_Term_Temperature.q0, gs->Lowest_Average_Long_Term_Temperature.q0);
                wprintf(L"\n");

                wprintf(L"Time in Over-Temperature:   0x%016llX\n", gs->Time_in_Over_Temperature);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Time_in_Over_Temperature.a7,
                    gs->Time_in_Over_Temperature.a6,
                    gs->Time_in_Over_Temperature.a5,
                    gs->Time_in_Over_Temperature.a4,
                    gs->Time_in_Over_Temperature.a3
                );
                wprintf(L"\tTime in Over-Temperature: %d\370C [0x%llX]\n", gs->Time_in_Over_Temperature.q1, gs->Time_in_Over_Temperature.q1);
                wprintf(L"\n");

                wprintf(L"Specified Maximum Operating Temperature:   0x%016llX\n", gs->Specified_Maximum_Operating_Temperature);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Specified_Maximum_Operating_Temperature.a7,
                    gs->Specified_Maximum_Operating_Temperature.a6,
                    gs->Specified_Maximum_Operating_Temperature.a5,
                    gs->Specified_Maximum_Operating_Temperature.a4,
                    gs->Specified_Maximum_Operating_Temperature.a3
                );
                wprintf(L"\tSpecified Maximum Operating Temperature: %d\370C [0x%llX]\n", gs->Specified_Maximum_Operating_Temperature.q0, gs->Specified_Maximum_Operating_Temperature.q0);
                wprintf(L"\n");

                wprintf(L"Time in Under-Temperature:   0x%016llX\n", gs->Time_in_Under_Temperature);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Time_in_Under_Temperature.a7,
                    gs->Time_in_Under_Temperature.a6,
                    gs->Time_in_Under_Temperature.a5,
                    gs->Time_in_Under_Temperature.a4,
                    gs->Time_in_Under_Temperature.a3
                );
                wprintf(L"\tTime in Under-Temperature: %d\370C [0x%llX]\n", gs->Time_in_Under_Temperature.q1, gs->Time_in_Under_Temperature.q1);
                wprintf(L"\n");

                wprintf(L"Specified Minimum Operating Temperature:   0x%016llX\n", gs->Specified_Minimum_Operating_Temperature);
                wprintf(L"\tDEVICE STATISTICS FLAGS: \n");
                printfDeviceStatistics(
                    gs->Specified_Minimum_Operating_Temperature.a7,
                    gs->Specified_Minimum_Operating_Temperature.a6,
                    gs->Specified_Minimum_Operating_Temperature.a5,
                    gs->Specified_Minimum_Operating_Temperature.a4,
                    gs->Specified_Minimum_Operating_Temperature.a3
                );
                wprintf(L"\tSpecified Minimum Operating Temperature: %d\370C [0x%llX]\n", gs->Specified_Minimum_Operating_Temperature.q0, gs->Specified_Minimum_Operating_Temperature.q0);
                wprintf(L"\n");






            }

            if (CTF.bCylLowReg == Transport_Statistics)
            {
                TRANSPORT_STATISTICS* gs = (TRANSPORT_STATISTICS*)data;



            }

            if (CTF.bCylLowReg == Solid_State_Device_Statistics)
            {
                SOLID_STATE_DEVICE_STATISTICS* gs = (SOLID_STATE_DEVICE_STATISTICS*)data;









            }
        }
        else
        {
            printError(GetLastError(), L"GetATACommandResponse_READ_LOG_EXT_data");

            
        }


    }

    void printfDeviceStatistics(BYTE f1,BYTE f2,BYTE f3,BYTE f4,BYTE f5)
    {
        wprintf(L"\t\tDEVICE STATISTIC SUPPORTED :   0x%X\n", f1);
        wprintf(L"\t\tVALID VALUE:   0x%X\n", f2);
        wprintf(L"\t\tNORMALIZED STATISTIC:   0x%X\n", f3);
        wprintf(L"\t\tSTATISTIC SUPPORTS DSN :   0x%X\n", f4);
        wprintf(L"\t\tMONITORED CONDITION MET :   0x%X\n", f5);
    }


    /***********************************************************************************************/



















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

        if (drive_path[4] == L'C')
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
        VOLUME_DISK_EXTENTS data{ 0 };

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


    void parseSMARTAttributes(STORAGE_PREDICT_FAILURE* spf)
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
        if (smartData.PredictFailure != 0)
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
                wprintf(L"\t\t\tpre-failure/advisory: %s\n", (((smartData.Attribute[aa].StatusFlags[0]) >> 0) & 1) == 1 ? L"The drive may have failure." : L"Product life period may expired.");
                wprintf(L"\t\t\ton-line data collection: %s\n", (((smartData.Attribute[aa].StatusFlags[0]) >> 1) & 1) == 1 ? L"Attribute value will be changed during normal operation." : L"Attribute value will be changed during off-line data collection operation.");
                wprintf(L"\t\t\tPerformance Attribute: %s\n", (((smartData.Attribute[aa].StatusFlags[0]) >> 2) & 1) == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\t\t\tError rate attribute:\n", (((smartData.Attribute[aa].StatusFlags[0]) >> 3) & 1) == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\t\t\tEvent Count Attribute:\n", (((smartData.Attribute[aa].StatusFlags[0]) >> 4) & 1) == 1 ? L"TRUE" : L"FALSE");
                wprintf(L"\t\t\tSelf-Preserving Attribute:\n", (((smartData.Attribute[aa].StatusFlags[0]) >> 5) & 1) == 1 ? L"TRUE" : L"FALSE");

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
        wprintf(L"\tPercent Self-test remaining: %d%\n", percents * 10);
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

    void printResponseData(UCHAR response[], int LBA_flag, wchar_t text[])
    {



        wprintf(L"RETURNED STATUS: \n");


        // TODO: uredi, ker ene komande rabijo razlago vrednosti in ne errorje npr. EXECUTE DEVICE DIAGNOSTIC
        wprintf(L"ERROR: 0x%02X\n", response[0]);
        wprintf(L"\tIllegal length indicator: %s\n", (((response[0]) >> 0) & 1) == 1 ? L"TRUE" : L"FALSE");
        wprintf(L"\tEnd of media: %s\n", (((response[0]) >> 1) & 1) == 1 ? L"TRUE" : L"FALSE");
        wprintf(L"\tAbort: %s\n", (((response[0]) >> 2) & 1) == 1 ? L"TRUE" : L"FALSE");
        wprintf(L"\tID not found: %s\n", (((response[0]) >> 4) & 1) == 1 ? L"TRUE" : L"FALSE");
        wprintf(L"\tUncorrectable: %s\n", (((response[0]) >> 6) & 1) == 1 ? L"TRUE" : L"FALSE");
        wprintf(L"\tInterface CRC: %s\n", (((response[0]) >> 7) & 1) == 1 ? L"TRUE" : L"FALSE");


        
        // TODO: uredi ker ene ATA komande rabijo to:
        wprintf(L"COUNT: 0x%02X\n", response[1]);



        wprintf(L"DEVICE: 0x%02X\n", response[5]);

        wprintf(L"STATUS: 0x%02X\n", response[6]);
        wprintf(L"\tError: %s\n", (((response[6]) >> 0) & 1) == 1 ? L"TRUE" : L"FALSE");
        wprintf(L"\tSense data available: %s\n", (((response[6]) >> 1) & 1) == 1 ? L"TRUE" : L"FALSE");
        wprintf(L"\tAlignment error: %s\n", (((response[6]) >> 2) & 1) == 1 ? L"TRUE" : L"FALSE");
        wprintf(L"\tDevice fault: %s\n", (((response[6]) >> 5) & 1) == 1 ? L"TRUE" : L"FALSE");
        wprintf(L"\tTransport:\n");
        wprintf(L"\t\tBSY: %s\n", (((response[6]) >> 3) & 1) == 1 ? L"TRUE" : L"FALSE");
        wprintf(L"\t\tDRDY: %s\n", (((response[6]) >> 6) & 1) == 1 ? L"TRUE" : L"FALSE");
        wprintf(L"\t\tDRQ: %s\n", (((response[6]) >> 7) & 1) == 1 ? L"TRUE" : L"FALSE");
        wprintf(L"\n");
        wprintf(L"\n");

        if (LBA_flag == 1)
        {
            wprintf(L"LBA [%s]: 0x%X%X%X\n", text, response[2], response[3], response[4]);
            // TODO: uredi primerno, ker eni rabijo od 2 - 4, eni pa od 4 - 2
            // TODO: dodaj se za print kaj pomeni vrednost

        }

        wprintf(L"\n");
        wprintf(L"\n"); wprintf(L"\n");
    }

    void MinorVersionNumberPrint(WORD a)
    {
        switch (a)
        {
            case 0x0000:
                wprintf(L"\tMinor version is not reported\n");
            break;

            case 0x0013:
                wprintf(L"\tATA/ATAPI-5 T13 1321D version 3\n");
            break;

            case 0x0015:
                wprintf(L"\tATA/ATAPI-5 T13 1321D version 1\n");
            break;

            case 0x0016:
                wprintf(L"\tATA/ATAPI-5 published, ANSI INCITS 340-2000\n");
            break;

            case 0x0018:
                wprintf(L"\t ATA/ATAPI-6 T13 1410D version 0\n");
            break;

            case 0x0019:
                wprintf(L"\tATA/ATAPI-6 T13 1410D version 3a\n");
            break;

            case 0x001a:
                wprintf(L"\tATA/ATAPI-7 T13 1532D version 1\n");
            break;

            case 0x001b:
                wprintf(L"\tATA/ATAPI-6 T13 1410D version 2\n");
            break;

            case 0x001c:
                wprintf(L"\tATA/ATAPI-6 T13 1410D version 1\n");
            break;

            case 0x001d:
                wprintf(L"\tATA/ATAPI-7 published ANSI INCITS 397-2005\n");
            break;

            case 0x001e:
                wprintf(L"\tATA/ATAPI-7 T13 1532D version 0\n");
            break;

            case 0x001f:
                wprintf(L"\tACS-3 Revision 3b\n");
            break;

            case 0x0021:
                wprintf(L"\tATA/ATAPI-7 T13 1532D version 4a\n");
            break;

            case 0x0022:
                wprintf(L"\tATA/ATAPI-6 published, ANSI INCITS 361-2002\n");
            break;

            case 0x0027:
                wprintf(L"\tATA8-ACS version 3c\n");
            break;

            case 0x0028:
                wprintf(L"\tATA8-ACS version 6\n");
            break;

            case 0x0029:
                wprintf(L"\tATA8-ACS version 4\n");
            break;

            case 0x0031:
                wprintf(L"\tACS-2 Revision 2\n");
            break;

            case 0x0033:
                wprintf(L"\tATA8-ACS version 3e\n");
            break; 

            case 0x0039:
                wprintf(L"\tATA8-ACS version 4c\n");
            break;

            case 0x0042:
                wprintf(L"\tATA8-ACS version 3f\n");
            break;

            case 0x0052:
                wprintf(L"\tATA8-ACS version 3b\n");
            break;

            case 0x006d:
                wprintf(L"\tACS-3 Revision 5\n");
            break;

            case 0x0082:
                wprintf(L"\tACS-2 published, ANSI INCITS 482-2012\n");
            break;

            case 0x0107:
                wprintf(L"\tATA8-ACS version 2d\n");
            break;

            case 0x0110:
                wprintf(L"\tACS-2 Revision 3\n");
            break;

            case 0x011b:
                wprintf(L"\t ACS-3 Revision 4\n");
            break;

            case 0xffff:
                wprintf(L"\t Minor version is not reported\n");
            break;
        }

    }
};




