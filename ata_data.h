#pragma once
#include "main.h"

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
    BYTE bFeaturesReg;
    BYTE bSectorCountReg;
    BYTE bSectorNumberReg;
    BYTE bCylLowReg;
    BYTE bCylHighReg;
    BYTE bDriveHeadReg;
    BYTE bCommandReg;
    BYTE bReserved;
} ATA_COMMAND;

typedef struct
{
    struct General_configuration_t                                                             // Word 0
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

    struct Mandatory_1_t                                                                    // Word 47
    {
        USHORT a1 : 8;
        USHORT a2 : 8;
    } Mandatory_1;

    struct Trusted_Computing_feature_t                                                         // Word 48
    {
        USHORT a0 : 1;
        USHORT : 13;
        USHORT a14 : 1;
        USHORT a15 : 1;
    } Trusted_Computing_feature;

    struct Capabilities_1_t                                                                    // Word 49
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

    struct Capabilities_2_t                                                                    // Word 50
    {
        USHORT a0 : 1;
        USHORT : 15;

    } Capabilities_2;

    WORD Obsolete_5[2];                                                                     // Word 51 - 52

    struct Free_fall_Control_t                                                                 // Word 53
    {
        USHORT : 1;
        USHORT a1 : 1;
        USHORT a2 : 1;
        USHORT : 5;
        USHORT a8 : 8;
    } Free_fall_Control;

    WORD Obsolete_6[5];                                                                     // Word 54 - 58

    struct Multiple_sector_setting_t                                                           // Word 59
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

    struct Multiword_DMA_transfer_t                                                         // Word 63
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

    struct PIO_transfer_modes_supported_T                                                  // Word 64
    {
        USHORT a0 : 1;
        USHORT a1 : 1;
        USHORT : 14;
    } PIO_transfer_modes_supported;

    WORD Min_Mult_DMA_trans_cycle_time_per_word;                                            // Word 65
    WORD Man_recomm_Multi_DMA_trans_cycle_time;                                             // Word 66
    WORD Min_PIO_trans_cycle_time_without_flow_con;                                         // Word 67
    WORD Min_PIO_trans_cycle_time_with_IORDY_flow_con;                                      // Word 68

    struct Additional_Supported_t                                                              // Word 69
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

    struct Queue_depth_t                                                                    // Word 75
    {
        USHORT a0 : 5;
        USHORT : 11;
    } Queue_depth;

    struct Serial_ATA_Capabilities_t                                                        // Word 76
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

    struct Serial_ATA_Additional_Capabilities_t                                                // Word 77
    {
        USHORT a0 : 1;
        USHORT a1 : 3;
        USHORT a4 : 1;
        USHORT a5 : 1;
        USHORT a6 : 1;
        USHORT : 9;
    } Serial_ATA_Additional_Capabilities;

    struct SATA_Features_Supported_t                                                           // Word 78
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

    WORD SATA_Features_Enabled;                                                             // Word 79
    WORD Major_revision_number;                                                             // Word 80
    WORD Minor_revision_number;                                                             // Word 81
    WORD Command_set_supported;                                                             // Word 82
    WORD Command_sets_supported;                                                            // Word 83
    WORD Command_set_feature_supported;                                                     // Word 84
    WORD Command_set_feature_enabled_supported_1;                                           // Word 85
    WORD Command_set_feature_enabled_supported_2;                                           // Word 86
    WORD Command_set_feature_enabled_supported_3;                                           // Word 87
    WORD Ultra_DMA_modes;                                                                   // Word 88
    WORD Time_required_for_security_erase_unit_completion;                                  // Word 89
    WORD Time_required_for_Enhanced_security_erase_completion;                              // Word 90
    WORD Current_advanced_power_management_value;                                           // Word 91
    WORD Master_Password_Identifier;                                                        // Word 92
    WORD Hardware_reset_result;                                                             // Word 93
    WORD Vendor_recommended_acoustic;                                                       // Word 94
    WORD Stream_Minimum_Request_Size;                                                       // Word 95
    WORD Streaming_Transfer_Time_DMA;                                                       // Word 96
    WORD Streaming_Access_Latency_DMA_and_PIO;                                              // Word 97
    WORD Streaming_Performance_Granularity[2];                                              // Word 98 - 99
    WORD Total_Number_of_User_Addressable_Sectors_for_the_48_bit_Address_feature_set[4];    // Word 100 - 103
    WORD Streaming_Transfer_Time_PIO;                                                       // Word 104
    WORD Reserved_8;                                                                        // Word 105
    WORD Physical_sector_size_Logical_Sector_Size;                                          // Word 106
    WORD Inter_seek_delay_for_ISO_7779_acoustic_testing_in_microseconds;                    // Word 107
    WORD xyz_1;                                                                             // Word 108
    WORD xyz_2;                                                                             // Word 109
    WORD xyz_3;                                                                             // Word 110
    WORD xyz_4;                                                                             // Word 111
    WORD Reserved_for_world_wide_name_extension_to_128_bits;                                // Word 112 - 115
    WORD Reserved_for_INCITS_TR37_2004;                                                     // Word 116
    WORD Words_per_Logical_Sector;                                                          // Word 117 - 118
    WORD Supported_Settings;                                                                // Word 119
    WORD Command_set_feature_enabled_supported_4;                                           // Word 120
    WORD Reserved_for_expanded_supportedand_enabled_settings;                               // Word 121 - 126
    WORD Obsolete_8;                                                                        // Word 127
    WORD Security_status;                                                                   // Word 128
    WORD Vendor_specific[31];                                                               // Word 129 - 159
    WORD CFA_power_mode_1;                                                                  // Word 160
    WORD Reserved_9[15];                                                                    // Word 161 - 175
    WORD Current_media_serial_number[30];                                                   // Word 176 - 205
    WORD SCT_Command_Transport;                                                             // Word 206
    WORD Reserved_10[2];                                                                    // Word 207 - 208
    WORD Alignment_of_logical_blocks_within_a_larger_physical_block;                        // Word 209
    WORD Write_Read_Verify_Sector_Count_Mode_3_Only[2];                                     // Word 210 - 211
    WORD Verify_Sector_Count_Mode_2_Only[2];                                                // Word 212 - 213
    WORD NV_Cache_Capabilities;                                                             // Word 214
    WORD NV_Cache_Size_in_Logical_Blocks_1;                                                 // Word 215
    WORD NV_Cache_Size_in_Logical_Blocks_2;                                                 // Word 216
    WORD Nominal_media_rotation_rate;                                                       // Word 217
    WORD Reserved_11;                                                                       // Word 218
    WORD NV_Cache_Options;                                                                  // Word 219
    WORD Reserved_12;                                                                       // Word 220
    WORD Reserved_13;                                                                       // Word 221
    WORD Transport_Major_revision_number;                                                   // Word 222
    WORD Transport_Minor_revision_number;                                                   // Word 223
    WORD Reserved_14[10];                                                                   // Word 224 - 233
    WORD Minimum_number_of_512_byte_units_per_DOWNLOAD_MICROCODE_command_for_mode_03h;      // Word 234
    WORD Maximum_number_of_512_byte_units_per_DOWNLOAD_MICROCODE_command_for_mode_03h;      // Word 235
    WORD Reserved_15[19];                                                                   // Word 236 - 254
    WORD Integrity_word;                                                                    // Word 255

} IDENTIFY_DEVICE_DATA;




//typedef struct
//{
//    /*struct {
//        USHORT a0 : 1;
//        USHORT a1 : 1;
//        USHORT a2 : 1;
//        USHORT a3 : 1;
//        USHORT a4 : 1;
//        USHORT a5 : 1;
//        USHORT a6 : 1;
//        USHORT a7 : 1;
//    } Error;
//
//    UCHAR Count;
//    UCHAR LBA_1;
//    UCHAR LBA_2;
//    UCHAR LBA_3;
//    UCHAR Device;
//    UCHAR Status;*/
//
//    /*WORD Error;
//
//    WORD Sector_count;
//    WORD LBA_1;
//    WORD LBA_2;
//    WORD LBA_3;
//
//    struct{
//        USHORT a0 : 1;
//        USHORT a1 : 1;
//        USHORT a2 : 1;
//        USHORT a3 : 1;
//        USHORT a4 : 1;
//        USHORT a5 : 1;
//        USHORT a6 : 1;
//        USHORT a7 : 1;
//    } Device;
//
//    struct {
//        USHORT a0 : 1;
//        USHORT a1 : 1;
//        USHORT a2 : 1;
//        USHORT a3 : 1;
//        USHORT a4 : 1;
//        USHORT a5 : 1;
//        USHORT a6 : 1;
//        USHORT a7 : 1;
//    } Status;*/
//
//    UCHAR Reserved;
//} RESPONSE_DATA;