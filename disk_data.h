#pragma once
#include "main.h"

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