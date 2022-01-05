#pragma once
#include "main.h"

#define QWORD   __int64


/* ATA COMMANDS */
#define READ_LOG_EXT_48 0x2f


/* LOG ADDRESSES OF READ_LOG_EXT_48*/
#define LOG_DIRECTORY_48                0x00
#define EXT_COMP_SMART_ERROR_LOG_48     0x03
#define DEVICE_STATISTICS_48            0x04


/* DEVICE STATISTICS OF DEVICE_STATISTICS_48*/
#define List_of_supported_log_pages         0x00
#define General_Statistics                  0x01
#define Free_Fall_Statistics                0x02
#define Rotating_Media_Statistics           0x03
#define General_Errors_Statistics           0x04
#define Temperature_Statistics              0x05
#define Transport_Statistics                0x06
#define Solid_State_Device_Statistics       0x07




typedef struct
{
    WORD revision;                                          // Word 0

    struct Multiword_DMA_modes_supported_t                     // Word 1
    {
        USHORT a0 : 1;
        USHORT a1 : 1;
        USHORT a2 : 1;
        USHORT : 13;
    } Multiword_DMA_modes_supported;

    struct Ultra_DMA_modes_supported_t                         // Word 2
    {
        USHORT a0 : 1;
        USHORT a1 : 1;
        USHORT a2 : 1;
        USHORT a3 : 1;
        USHORT a4 : 1;
        USHORT a5 : 1;
        USHORT a6 : 1;
        USHORT : 9;

    } Ultra_DMA_modes_supported;

    WORD Maximum_LBA[4];                                    // Word 3-6

    struct Command_set_feature_set_supported_part1_t           // Word 7
    {
        USHORT a0 : 1;
        USHORT a1 : 1;
        USHORT a2 : 1;
        USHORT a3 : 1;
        USHORT a4 : 1;
        USHORT a5 : 1;
        USHORT a6 : 1;
        USHORT a7 : 1;
        USHORT a8 : 1;
        USHORT a9 : 1;
        USHORT a10 : 1;
        USHORT a11 : 1;
        USHORT a12 : 1;
        USHORT a13 : 1;
        USHORT a14 : 1;

    } Command_set_feature_set_supported_part1;

    struct Serial_ATA_Comm_set_feature_set_supported_part2_t   // Word 8
    {
        USHORT a0 : 1;
        USHORT a1 : 1;
        USHORT a2 : 1;
        USHORT a3 : 1;
        USHORT a4 : 1;
        USHORT a5 : 1;
    } Serial_ATA_Comm_set_feature_set_supported_part2;

    WORD Reserved_1;                                        // Word 9
    WORD Reserved_2[11];                                    // Word 10 - 20

    struct Command_set_feature_set_supported_part3_t           // Word 21
    {
        USHORT : 11;
        USHORT a11 : 1;
        USHORT a12 : 1;
        USHORT a13 : 1;
        USHORT a14 : 1;
        USHORT a15 : 1;
    } Command_set_feature_set_supported_part3;

    WORD Reserved_3;                                        // Word 22
    WORD Reserved_4[185];                                   // Word 23- 207
    WORD Vendor_Specific[47];                               // Word 208 - 254

    struct Integrity_word_t                                    // Word 255
    {
        USHORT signature : 8;
        USHORT checksum : 8;

    } Integrity_word;

} DCO_DATA;

typedef struct
{
    BYTE bFeaturesReg;          // 0
    BYTE bSectorCountReg;       // 1

    BYTE bSectorNumberReg;      // 2
    BYTE bCylLowReg;            // 3
    BYTE bCylHighReg;           // 4

    BYTE bDriveHeadReg;         // 5
    BYTE bCommandReg;           // 6
    BYTE bReserved;             // 7
} ATA_COMMAND;

typedef struct
{
    struct                                                                                  // Word 0
    {
        USHORT a1 : 2;
        USHORT a2 : 1;
        USHORT a3 : 12;
        USHORT a15 : 1;
    } General_configuration;

    WORD Obsolete_1;                                                                        // Word 1
    WORD Specific_configuration;                                                            // Word 2
    WORD Obsolete_2;                                                                        // Word 3
    WORD Retired_1[2];                                                                      // Word 4 - 5
    WORD Obsolete_3;                                                                        // Word 6
    WORD Reserved_1[2];                                                                     // Word 7 - 8
    WORD Retired_2;                                                                         // Word 9
    UCHAR Serial_number[20];                                                                // Word 10 - 19
    WORD Retired_3[2];                                                                      // Word 20 - 21
    WORD Obsolete_4;                                                                        // Word 22
    UCHAR Firmware_revision[8];                                                             // Word 23 - 26
    UCHAR Model_number[40];                                                                 // Word 27 - 46

    struct                                                                                  // Word 47
    {
        USHORT a1 : 8;
        USHORT a2 : 8;
    } Mandatory_1;

    struct                                                                                  // Word 48
    {
        USHORT a0 : 1;
        USHORT : 13;
        USHORT a14 : 1;
        USHORT a15 : 1; 
    } Trusted_Computing_feature;

    struct                                                                                  // Word 49
    {
        USHORT a0 : 2;
        USHORT : 6;
        USHORT a8 : 1;
        USHORT a9 : 1;
        USHORT a10 : 1;
        USHORT a11 : 1;
        USHORT a12 : 1;
        USHORT a13 : 1;
        USHORT : 2;

    }Capabilities_1;

    struct                                                                                  // Word 50
    {
        USHORT a0 : 1;
        USHORT : 15;

    } Capabilities_2;

    WORD Obsolete_5[2];                                                                     // Word 51 - 52

    struct                                                                                  // Word 53
    {
        USHORT : 1;
        USHORT a1 : 1;
        USHORT a2 : 1;
        USHORT : 5;
        USHORT a8 : 8;
    } Free_fall_Control;

    WORD Obsolete_6[5];                                                                     // Word 54 - 58

    struct                                                                                  // Word 59
    {
        USHORT a0 : 8;
        USHORT a8 : 1;
        USHORT : 1;
        USHORT a10 : 1;
        USHORT a11 : 1;
        USHORT a12 : 1;
        USHORT a13 : 1;
        USHORT a14 : 1;
        USHORT a15 : 1;
    } Multiple_sector_setting;

    WORD Total_n_user_addr_log_sectors[2];                                                  // Word 60 - 61
    WORD Obsolete_7;                                                                        // Word 62

    struct                                                                                  // Word 63
    {
        USHORT a0 : 1;
        USHORT a1 : 1;
        USHORT a2 : 1;
        USHORT : 5;
        USHORT a8 : 1;
        USHORT a9 : 1;
        USHORT a10 : 1;
        USHORT : 5;
    } Multiword_DMA_transfer;

    struct                                                                                  // Word 64
    {
        USHORT a0 : 1;
        USHORT a1 : 1;
        USHORT : 14;
    } PIO_transfer_modes_supported;

    WORD Min_Mult_DMA_trans_cycle_time_per_word;                                            // Word 65
    WORD Man_recomm_Multi_DMA_trans_cycle_time;                                             // Word 66
    WORD Min_PIO_trans_cycle_time_without_flow_con;                                         // Word 67
    WORD Min_PIO_trans_cycle_time_with_IORDY_flow_con;                                      // Word 68

    struct                                                                                  // Word 69
    {
        USHORT : 2;
        USHORT a2 : 1;
        USHORT a3 : 1;
        USHORT a4 : 1;
        USHORT a5 : 1;
        USHORT a6 : 1;
        USHORT a7 : 1;
        USHORT a8 : 1;
        USHORT a9 : 1;
        USHORT a10 : 1;
        USHORT a11 : 1;
        USHORT a12 : 1;
        USHORT a13 : 1;
        USHORT a14 : 1;
        USHORT a15 : 1;
    } Additional_Supported;

    WORD Reserved_5;                                                                        // Word 70
    WORD Reserved_6[4];                                                                     // Word 71 - 74

    struct                                                                                  // Word 75
    {
        USHORT a0 : 5;
        USHORT : 11;
    } Queue_depth;

    struct                                                                                  // Word 76
    {
        USHORT a0 : 1;
        USHORT a1 : 1;
        USHORT a2 : 1;
        USHORT a3 : 1;
        USHORT : 4;
        USHORT a8 : 1;
        USHORT a9 : 1;
        USHORT a10 : 1;
        USHORT a11 : 1;
        USHORT a12 : 1;
        USHORT a13 : 1;
        USHORT a14 : 1;
        USHORT a15 : 1;
    } Serial_ATA_Capabilities;

    struct                                                                                  // Word 77
    {
        USHORT a0 : 1;
        USHORT a1 : 3;
        USHORT a4 : 1;
        USHORT a5 : 1;
        USHORT a6 : 1;
        USHORT : 9;
    } Serial_ATA_Additional_Capabilities;

    struct                                                                                  // Word 78
    {
        USHORT a0 : 1;
        USHORT a1 : 1;
        USHORT a2 : 1;
        USHORT a3 : 1;
        USHORT a4 : 1;
        USHORT a5 : 1;
        USHORT a6 : 1;
        USHORT a7 : 1;
        USHORT : 8;
    } SATA_Features_Supported;

    struct                                                                                  // Word 79
    {
        USHORT a0 : 1;
        USHORT a1 : 1;
        USHORT a2 : 1;
        USHORT a3 : 1;
        USHORT a4 : 1;
        USHORT a5 : 1;
        USHORT a6 : 1;
        USHORT a7 : 1;
        USHORT : 8;
    } SATA_Features_Enabled;

    struct                                                                                  // Word 80
    {
        USHORT : 5;
        USHORT a5 : 1;
        USHORT a6 : 1;
        USHORT a7 : 1;
        USHORT a8 : 1;
        USHORT a9 : 1;
        USHORT a10 : 1;
        USHORT : 5;
    } Major_revision_number;

    WORD Minor_revision_number;                                                             // Word 81
    
    struct                                                                                  // Word 82
    {
        USHORT a0 : 1;
        USHORT a1 : 1;
        USHORT  : 1;
        USHORT a3 : 1;
        USHORT a4 : 1;
        USHORT a5 : 1;
        USHORT a6 : 1;
        USHORT  : 2;
        USHORT a9 : 1;
        USHORT : 2;
        USHORT a12 : 1;
        USHORT a13 : 1;
        USHORT a14 : 1;
        USHORT : 1;
    } Commandsand_feature_sets_supported_1;

    struct                                                       // Word 83
    {
        USHORT a0 : 1;
        USHORT : 1;
        USHORT a2 : 1;
        USHORT a3 : 1;
        USHORT : 1;
        USHORT a5 : 1;
        USHORT a6 : 1;
        USHORT : 3;
        USHORT a10 : 1;
        USHORT : 1;
        USHORT a12 : 1;
        USHORT a13 : 1;
        USHORT a14 : 1;
        USHORT a15 : 1;
    } Commandsand_feature_sets_supported_2;

    struct                                                      // Word 84
    {
        USHORT a0 : 1;
        USHORT a1 : 1;
        USHORT : 2;
        USHORT a4 : 1;
        USHORT a5 : 1;
        USHORT a6 : 1;
        USHORT : 1;
        USHORT a8 : 1;
        USHORT : 4;
        USHORT a13 : 1;
        USHORT a14 : 1;
        USHORT a15 : 1;
    } Commandsand_feature_sets_supported_3;

    struct                                            // Word 85
    {
        USHORT a0 : 1;
        USHORT a1 : 1;
        USHORT : 1;
        USHORT a3 : 1;
        USHORT a4 : 1;
        USHORT a5 : 1;
        USHORT a6 : 1;
        USHORT : 2;
        USHORT a9 : 1;
        USHORT : 2;
        USHORT a12 : 1;
        USHORT a13 : 1;
        USHORT a14 : 1;
        USHORT : 1;
    } Command_set_feature_enabled_supported_1;

    struct                                            // Word 86
    {
        USHORT a0 : 1;
        USHORT : 2;
        USHORT a3 : 1;
        USHORT : 1;
        USHORT a5 : 1;
        USHORT a6 : 1;
        USHORT : 3;
        USHORT a10 : 1;
        USHORT : 1;
        USHORT a12 : 1;
        USHORT a13 : 1;
        USHORT : 1;
        USHORT a15 : 1;
    } Command_set_feature_enabled_supported_2;

    struct                                            // Word 87
    {
        USHORT a0 : 1;
        USHORT a1 : 1;
        USHORT a2 : 1;
        USHORT : 2;
        USHORT a5 : 1;
        USHORT a6 : 1;
        USHORT : 1;
        USHORT a8 : 1;
        USHORT : 4;
        USHORT a13 : 1;
        USHORT a14 : 1;
        USHORT a15 : 1;
    } Command_set_feature_enabled_supported_3;

    struct                                                                    // Word 88
    {
        USHORT a0 : 1;
        USHORT a1 : 1;
        USHORT a2 : 1;
        USHORT a3 : 1;
        USHORT a4 : 1;
        USHORT a5 : 1;
        USHORT a6 : 1;
        USHORT : 1;
        USHORT a8 : 1;
        USHORT a9 : 1;
        USHORT a10 : 1;
        USHORT a11 : 1;
        USHORT a12 : 1;
        USHORT a13 : 1;
        USHORT a14 : 1;
        USHORT : 1;
    } Ultra_DMA_modes;

    WORD Time_required_for_security_erase_unit_completion;                                  // Word 89
    WORD Time_required_for_Enhanced_security_erase_completion;                              // Word 90
    WORD Current_advanced_power_management_value;                                           // Word 91
    WORD Master_Password_Identifier;                                                        // Word 92

    struct                                                              // Word 93
    {
        USHORT a0 : 1;
        USHORT a1 : 2;
        USHORT a3 : 1;
        USHORT a4 : 1;
        USHORT a5 : 1;
        USHORT a6 : 1;
        USHORT  : 1;
        USHORT a8 : 1;
        USHORT a9 : 2;
        USHORT a11 : 1;
        USHORT  : 1;
        USHORT a13 : 1;
        USHORT a14 : 1;
        USHORT a15 : 1;
    } Hardware_reset_result;

    WORD Obsolete_10;                                                                       // Word 94
    WORD Stream_Minimum_Request_Size;                                                       // Word 95
    WORD Streaming_Transfer_Time_DMA;                                                       // Word 96
    WORD Streaming_Access_Latency_DMA_and_PIO;                                              // Word 97
    WORD Streaming_Performance_Granularity[2];                                              // Word 98 - 99
    WORD Total_Number_of_User_Addressable_Sectors_for_the_48_bit_Address_feature_set[4];    // Word 100 - 103
    WORD Streaming_Transfer_Time_PIO;                                                       // Word 104
    WORD Max_num_of_512_byte_blocks_of_LBA_Range_Entries_per_DATA_SET_MANAGEMENT_command;   // Word 105

    struct                                           // Word 106
    {
        USHORT a0 : 4;
        USHORT : 8;
        USHORT a12 : 1;
        USHORT a13 : 1;
        USHORT a14 : 1;
        USHORT a15 : 1;
    } Physical_sector_size_Logical_Sector_Size;

    WORD Inter_seek_delay_for_ISO_7779_acoustic_testing_in_microseconds;                    // Word 107

    WORD World_wide_name[4];                                                                // Word 108 - 111
    WORD Reserved_for_world_wide_name_extension_to_128_bits[4];                             // Word 112 - 115
    WORD Obsolete_12;                                                                      // Word 116
    WORD Logical_sector_size[2];                                                      // Word 117 - 118
    WORD Supported_Settings;                                                                // Word 119
    WORD Command_set_feature_enabled_supported_4;                                           // Word 120
    WORD Reserved_for_expanded_supportedand_enabled_settings[6];                               // Word 121 - 126
    WORD Obsolete_8;                                                                        // Word 127

    struct                                                                                   // Word 128
    {
        USHORT a0 : 1;
        USHORT a1 : 2;
        USHORT a3 : 1;
        USHORT a4 : 1;
        USHORT a5 : 1;
        USHORT : 2;
        USHORT a8 : 1;
        USHORT : 7;
    } Security_status;

    WORD Vendor_specific[31];                                                               // Word 129 - 159
    WORD Reserved_9[8];                                                                    // Word 160 - 167
    
    struct                                                                                  // Word 168
    {
        USHORT a0 : 4;
        USHORT : 12;
    } Device_Nominal_Form_Factor;

    struct                                                                                      // Word 169
    {
        USHORT a0 : 1;
        USHORT : 15;
    } DATA_SET_MANAGEMENT_support;

    WORD Additional_Product_Identifier[4];                                                   // Word 170 - 173
    WORD Reserved_10[2];                                                                    // Word 174 - 175
    WORD Current_media_serial_number[30];                                                   // Word 176 - 205
    
    struct                                                             // Word 206
    {
        USHORT a0 : 1;
        USHORT : 1;
        USHORT a2 : 1;
        USHORT a3 : 1;
        USHORT a4 : 1;
        USHORT a5 : 1;
        USHORT : 6;
        USHORT a12 : 4;
    } SCT_Command_Transport;

    WORD Reserved_11[2];                                                                    // Word 207 - 208

    struct                                                                             // Word 209
    {
        USHORT a0 : 14;
        USHORT a14 : 1;
        USHORT a15 : 1;
    } Alignment_of_logical_blocks_within_a_larger_physical_block;

    WORD Write_Read_Verify_Sector_Count_Mode_3_Only[2];                                     // Word 210 - 211
    WORD Verify_Sector_Count_Mode_2_Only[2];                                                // Word 212 - 213
    WORD Obsolete_9[3];                                                                     // Word 214 - 216
    WORD Nominal_media_rotation_rate;                                                       // Word 217
    WORD Reserved_12;                                                                       // Word 218
    WORD Obsolete_11;                                                                  // Word 219
    WORD Write_Read_Verify_Mode;                                                             // Word 220
    WORD Reserved_13;                                                                       // Word 221
    
    struct                                                    // Word 222
    {
        USHORT a0 : 1;
        USHORT a1 : 1;
        USHORT a2 : 1;
        USHORT a3 : 1;
        USHORT a4 : 1;
        USHORT a5 : 1;
        USHORT a6 : 1;
        USHORT  : 5;
        USHORT a12 : 4;

    } Transport_Major_revision_number;

    WORD Transport_Minor_revision_number;                                                   // Word 223
    WORD Reserved_14[6];                                                                   // Word 224 - 229
    WORD Extended_Number_of_User_Addressable_Sectors[4];                                    // Word 230 - 233
    WORD Minimum_number_of_512_byte_data_blocks_per_Download_Microcode_mode_03h_operation;  // Word 234
    WORD Maximum_number_of_512_byte_data_blocks_per_Download_Microcode_mode_03h_operation;   // Word 235
    WORD Reserved_15[19];                                                                   // Word 236 - 254

    struct                                                                    // Word 255
    {
        USHORT Checksum_Validity_Indicator : 8;
        USHORT Checksum : 8;
    } Integrity_word;

} IDENTIFY_DEVICE_DATA;

typedef struct
{
    BYTE vendor_specific_1[362];                                        // BYTE 0 - 361
    BYTE Offline_data_collection_status;                                // BYTE 362

    struct   // TODO: preveri ce dela pravilno                         // BYTE 363
    {
        BYTE a0 : 4;    // 0 - 3
        BYTE a4 : 4;    // 4 - 7
    } SelfTest_execution_status;

    BYTE vendor_specific_2[2];                                          // BYTE 364 - 365
    BYTE vendor_specific_3;                                             // BYTE 366

    struct                             // BYTE 367
    {
        BYTE a0 : 1;
        BYTE a1 : 1;
        BYTE a2 : 1;
        BYTE a3 : 1;
        BYTE a4 : 1;
        BYTE a5 : 1;
        BYTE a6 : 1;
        BYTE a7 : 1;
    } Offline_data_collection_capability;

    struct                                            // BYTE 368 - 369
    {
        BYTE a0 : 1;
        BYTE a1 : 1;
        BYTE : 6;
    } SMART_capability_1;
    BYTE SMART_capability_2;

    BYTE Err_logging_capability;                                        // BYTE 370
    BYTE vendor_specific_4;                                             // BYTE 371
    BYTE ShortSelfTest_routine_recommended_polling_time_minutes;        // BYTE 372
    BYTE ExtSelfRest_routine_recommended_polling_time_minutes;          // BYTE 373
    BYTE ConveyanceSelfTest_routine_recommended_polling_time_minutes;   // BYTE 374
    BYTE ExtSelfTest_routine_recommended_polling_time_minutes[2];       // BYTE 375 - 376
    BYTE Reserved[9];                                                   // BYTE 377 - 385
    BYTE vendor_specific_5[125];                                        // BYTE 386 - 510
    BYTE Data_structure_checksum;                                       // BYTE 511
} ATA_SMART_DATA;


typedef struct
{
    BYTE version[2];                                // BYTE 0 - 1
    BYTE num_log_pages_at_log_address_1;            // BYTE 2
    BYTE reserved_1;                                // BYTE 3
    BYTE num_log_pages_at_log_address_2;            // BYTE 4
    BYTE reserved_2;                                // BYTE 5
    BYTE num_log_pages_at_log_address_3;            // BYTE 6
    BYTE reserved_3;                                // BYTE 7
    BYTE num_log_pages_at_log_address_4;            // BYTE 8
    BYTE reserved_4;                                // BYTE 9
    BYTE num_log_pages_at_log_address_5;            // BYTE 10
    BYTE reserved_5;                                // BYTE 2
    BYTE num_log_pages_at_log_address_6;            // BYTE 2
    BYTE reserved_6;                                // BYTE 2
    BYTE num_log_pages_at_log_address_7;            // BYTE 2
    BYTE reserved_61;                                // BYTE 2
    BYTE num_log_pages_at_log_address_8;            // BYTE 2
    BYTE reserved_7;                                // BYTE 2
    BYTE num_log_pages_at_log_address_9;            // BYTE 2
    BYTE reserved_8;                                // BYTE 2
    BYTE num_log_pages_at_log_address_10;           // BYTE 2
    BYTE reserved_9;                                // BYTE 2
    BYTE num_log_pages_at_log_address_11;           // BYTE 2
    BYTE reserved_10;                               // BYTE 2
    BYTE num_log_pages_at_log_address_12;           // BYTE 2
    BYTE reserved_11;                               // BYTE 2
    BYTE num_log_pages_at_log_address_13;           // BYTE 2
    BYTE reserved_12;                               // BYTE 2
    BYTE num_log_pages_at_log_address_14;           // BYTE 2
    BYTE reserved_13;                               // BYTE 2
    BYTE num_log_pages_at_log_address_16;           // BYTE 2
    BYTE reserved_14;                               // BYTE 2
    BYTE num_log_pages_at_log_address_17;// BYTE 2
    BYTE reserved_15;// BYTE 2
    BYTE num_log_pages_at_log_address_18;// BYTE 2
    BYTE reserved_16;// BYTE 2
    BYTE num_log_pages_at_log_address_19;// BYTE 2
    BYTE reserved_17;// BYTE 2
    BYTE num_log_pages_at_log_address_20;// BYTE 2
    BYTE reserved_18;// BYTE 2
    BYTE num_log_pages_at_log_address_21;// BYTE 2
    BYTE reserved_19;// BYTE 2
    BYTE num_log_pages_at_log_address_22;// BYTE 2
    BYTE reserved_20;// BYTE 2
    BYTE num_log_pages_at_log_address_23;// BYTE 2
    BYTE reserved_21;// BYTE 2
    BYTE num_log_pages_at_log_address_24;// BYTE 2
    BYTE reserved_22;// BYTE 2
    BYTE num_log_pages_at_log_address_25;// BYTE 2
    






} SMART_LOG_DIRECTORY;



/* LOGS and STATISTICS*/

// General Purpose Log Directory (GPL Log Address 00h)

typedef struct
{
    struct
    {
        unsigned __int64 q1 : 16;
        unsigned __int64 q2 : 8;
        unsigned __int64 q3 : 40;
    } Device_Statistics_Information_Header;

    BYTE Number_of_entries_n_in_the_following_list;
    
    BYTE Log_page_number[503];

} LIST_OF_SUPPORTED_LOGS;

typedef struct
{
    struct                                      // BYTE 0 - 7
    {
        WORD w1;        
        BYTE b3;
        BYTE r4;
        BYTE r5;
        BYTE r6;
        BYTE r7;
        BYTE r8;
    } Device_Statistics_Information_Header;

    struct
    {
        unsigned __int64 d1 : 32;
        unsigned __int64 r1 : 8;
        unsigned __int64 r2 : 8;
        unsigned __int64 r3 : 8;
        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
        
    } Lifetime_Power_On_Resets;

    struct
    {
        unsigned __int64 d1 : 32;
        unsigned __int64 r1 : 8;
        unsigned __int64 r2 : 8;
        unsigned __int64 r3 : 8;
        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;

    } Power_on_Hours;

    struct
    {
        unsigned __int64 dw1 : 48;
        unsigned __int64 : 8;
        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Logical_Sectors_Written;

    struct
    {
        unsigned __int64 dw1 : 48;
        unsigned __int64 : 8;
        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Number_of_Write_Commands;

    struct
    {
        unsigned __int64 dw1 : 48;
        unsigned __int64 : 8;
        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Logical_Sectors_Read;

    struct
    {
        unsigned __int64 dw1 : 48;
        unsigned __int64 : 8;
        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Number_of_Read_Commands;

    struct
    {
        unsigned __int64 dw1 : 48;
        unsigned __int64 : 8;
        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Date_and_Time_TimeStamp;

    unsigned __int64 reserved[56];

} GENERAL_STATISTICS;

typedef struct
{
    struct
    {
        unsigned __int64 q1 : 16;
        unsigned __int64 q2 : 8;
        unsigned __int64 q3 : 40;
    } Device_Statistics_Information_Header;

    struct
    {
        unsigned __int64 q1 : 32;

        unsigned __int64 r1 : 24;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Number_of_Free_Fall_Events_Detected;

    struct
    {
        unsigned __int64 q1 : 32;

        unsigned __int64 r1 : 24;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Overlimit_Shock_Events;

    unsigned __int64 reserved[61];

} FREE_FALL_STATISTICS;

typedef struct
{
    struct
    {
        unsigned __int64 q1 : 16;
        unsigned __int64 q2 : 8;
        unsigned __int64 q3 : 40;
    } Device_Statistics_Information_Header;

    struct
    {
        unsigned __int64 q1 : 32;

        unsigned __int64 r1 : 24;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Spindle_Motor_Power_on_Hours;

    struct 
    {
        unsigned __int64 q1 : 32;

        unsigned __int64 r1 : 24;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Head_Flying_Hours;

    struct 
    {
        unsigned __int64 q1 : 32;

        unsigned __int64 r1 : 24;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Head_Load_Events;

    struct 
    {
        unsigned __int64 q1 : 32;

        unsigned __int64 r1 : 24;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Number_of_Reallocated_Logical_Sectors;

    struct 
    {
        unsigned __int64 q1 : 32;

        unsigned __int64 r1 : 24;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Read_Recovery_Attempts;

    struct 
    {
        unsigned __int64 q1 : 32;

        unsigned __int64 r1 : 24;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Number_of_Mechanical_Start_Failures;

    struct 
    {
        unsigned __int64 q1 : 32;

        unsigned __int64 r1 : 24;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Number_of_Reallocation_Candidate_Logical_Sectors;

    struct 
    {
        unsigned __int64 q1 : 32;

        unsigned __int64 r1 : 24;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Number_of_High_Priority_Unload_Events;

    unsigned __int64 reserved[55];
} ROTATING_MEDIA_STATISTICS;

typedef struct
{
    struct
    {
        unsigned __int64 q1 : 16;
        unsigned __int64 q2 : 8;
        unsigned __int64 q3 : 40;
    } Device_Statistics_Information_Header;

    struct
    {
        unsigned __int64 q1 : 32;

        unsigned __int64 r1 : 24;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Number_of_Reported_Uncorrectable_Errors;

    struct
    {
        unsigned __int64 q1 : 32;

        unsigned __int64 r1 : 24;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Number_of_Resets_Between_Command_Acceptanceand_Command_Completion;

    unsigned __int64 reserved[61];

} GENERAL_ERROR_STATISTICS;

typedef struct
{
    struct
    {
        unsigned __int64 q1 : 16;
        unsigned __int64 q2 : 8;
        unsigned __int64 q3 : 40;
    } Device_Statistics_Information_Header;

    struct
    {
        unsigned __int64 q0 : 8;

        unsigned __int64 r0 : 48;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Current_Temperature;

    struct
    {
        unsigned __int64 q0 : 8;

        unsigned __int64 r0 : 48;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Average_Short_Term_Temperature;

    struct
    {
        unsigned __int64 q0 : 8;

        unsigned __int64 r0 : 48;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Average_Long_Term_Temperature;

    struct
    {
        unsigned __int64 q0 : 8;

        unsigned __int64 r0 : 48;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Highest_Temperature;

    struct
    {
        unsigned __int64 q0 : 8;

        unsigned __int64 r0 : 48;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Lowest_Temperature;

    struct
    {
        unsigned __int64 q0 : 8;

        unsigned __int64 r0 : 48;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Highest_Average_Short_Term_Temperature;

    struct
    {
        unsigned __int64 q0 : 8;

        unsigned __int64 r0 : 48;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Lowest_Average_Short_Term_Temperature;

    struct
    {
        unsigned __int64 q0 : 8;

        unsigned __int64 r0 : 48;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Highest_Average_Long_Term_Temperature;

    struct
    {
        unsigned __int64 q0 : 8;

        unsigned __int64 r0 : 48;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Lowest_Average_Long_Term_Temperature;

    struct
    {
        unsigned __int64 q1 : 32;

        unsigned __int64 r1 : 24;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Time_in_Over_Temperature;

    struct
    {
        unsigned __int64 q0 : 8;

        unsigned __int64 r0 : 48;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Specified_Maximum_Operating_Temperature;

    struct
    {
        unsigned __int64 q1 : 32;

        unsigned __int64 r1 : 24;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Time_in_Under_Temperature;

    struct
    {
        unsigned __int64 q0 : 8;

        unsigned __int64 r0 : 48;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Specified_Minimum_Operating_Temperature;

    unsigned __int64 reserved[50];

} TEMPERATURE_STATISTICS;

typedef struct
{
    struct
    {
        unsigned __int64 q1 : 16;
        unsigned __int64 q2 : 8;
        unsigned __int64 q3 : 40;
    } Device_Statistics_Information_Header;

    struct
    {
        unsigned __int64 q1 : 32;

        unsigned __int64 r1 : 24;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Number_of_hardware_resets;

    struct
    {
        unsigned __int64 q1 : 32;

        unsigned __int64 r1 : 24;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Number_of_ASR_Events;

    struct
    {
        unsigned __int64 q1 : 32;

        unsigned __int64 r1 : 24;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    }  Number_of_Interface_CRC_Errors;

    unsigned __int64 reserved[60];

} TRANSPORT_STATISTICS;

typedef struct
{
    struct
    {
        unsigned __int64 q1 : 16;
        unsigned __int64 q2 : 8;
        unsigned __int64 q3 : 40;
    } Device_Statistics_Information_Header;

    struct
    {
        unsigned __int64 q0 : 8;

        unsigned __int64 r0 : 48;

        unsigned __int64 a0 : 1;
        unsigned __int64 a1 : 1;
        unsigned __int64 a2 : 1;
        unsigned __int64 a3 : 1;
        unsigned __int64 a4 : 1;
        unsigned __int64 a5 : 1;
        unsigned __int64 a6 : 1;
        unsigned __int64 a7 : 1;
    } Current_Temperature;

    unsigned __int64 reserved[62];

} SOLID_STATE_DEVICE_STATISTICS;
















