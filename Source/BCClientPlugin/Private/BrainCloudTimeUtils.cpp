// Copyright 2018 bitHeads, Inc. All Rights Reserved.

#include "BrainCloudTimeUtils.h"
#include "BCClientPluginPrivatePCH.h"
#include "BrainCloudWrapper.h"
#include "BCWrapperProxy.h"
#include "Internationalization/Text.h"
#include "GenericPlatform/GenericPlatformMisc.h"

#if PLATFORM_WINDOWS
#include "timezoneapi.h"
#endif


BrainCloudTimeUtils::BrainCloudTimeUtils(BrainCloudClient *client) : _client(client){};

int64 BrainCloudTimeUtils::UTCDateTimeToUTCMillis(FDateTime dateTime)
{
    return (dateTime.GetTicks() - FDateTime(1970, 1, 1).GetTicks()) / ETimespan::TicksPerMillisecond;
}

FDateTime BrainCloudTimeUtils::UTCMillisToUTCDateTime(int64 utcDateTime)
{
    return FDateTime(1970, 1, 1) + FTimespan(utcDateTime * ETimespan::TicksPerMillisecond);
}

FDateTime BrainCloudTimeUtils::LocalTimeToUTCTime(FDateTime localDate)
{
    return localDate.UtcNow();
}
    
FDateTime BrainCloudTimeUtils::UTCTimeToLocalTime (FDateTime utcDate)
{
    return utcDate.Now();
}

/*
 utility function to find timezone offset
 uses tm and time_t structs
 FDateTime are adjusted for daylight savings, localtime_r is not
 tm.is_dst is positive if dst
 */
float BrainCloudTimeUtils::UTCTimeZoneOffset(){
    /*
    // query for calendar time
    struct timeval Time;
    gettimeofday(&Time, NULL);
    
    // convert it to local time
    struct tm loc;
    localtime_r(&Time.tv_sec, &loc);

    // convert it to UTC
    struct tm utc;
    gmtime_r(&Time.tv_sec, &utc);
    
    time_t localTime, utcTime;
    localTime = mktime(&loc);
    utcTime = mktime(&utc);

    float hours, minutes, seconds, full, offset;

    // find the time difference between local and utc
    seconds = difftime(localTime, utcTime);

    full = abs(seconds) / 3600.0;
    hours = floor(full);
    minutes = full - hours;
    
    minutes = round(minutes * 4.0) * 0.25; // round to nearest quarter hour just in case
    
    offset = hours + minutes;
    
    if(seconds < 0){
        offset *= -1.0;
    }
    */
    
    FDateTime UTCTime = FDateTime::UtcNow();
    FDateTime LocalTime = FDateTime::Now();    
    
    FTimespan span = LocalTime - UTCTime;

    float offset = span.GetHours() + span.GetMinutes() / 60.0;

    // todo: account for daylight savings
    UE_LOG(LogBrainCloud, Log, TEXT("GetInvariantTimeZone() is %s"), *FString(FText::GetInvariantTimeZone()));  // Etc/Unknown
    UE_LOG(LogBrainCloud, Log, TEXT("StrTimestamp() is %s"), *FString(FPlatformTime::StrTimestamp()));
    UE_LOG(LogBrainCloud, Log, TEXT("Local.ToIso8601() is %s"), *FString(LocalTime.ToIso8601()));
    UE_LOG(LogBrainCloud, Log, TEXT("UTC.ToIso8601() is %s"), *FString(UTCTime.ToIso8601()));
    UE_LOG(LogBrainCloud, Log, TEXT("GetTimeZoneId() is %s"), *FString(FGenericPlatformMisc::GetTimeZoneId()));
    UE_LOG(LogBrainCloud, Log, TEXT("ToUnixTimestamp() is %i"), LocalTime.ToUnixTimestamp());
    UE_LOG(LogBrainCloud, Log, TEXT("IsInitialized() is %i"), FInternationalization::Get().IsInitialized());
    //UE_LOG(LogBrainCloud, Log, TEXT("createDefault() is %s"), *FString(icu::TimeZone::createDefault()));


#if PLATFORM_WINDOWS

    DWORD dwRet;
    TIME_ZONE_INFORMATION tziOld;
    UE_LOG(LogBrainCloud, Log, TEXT("tzinfo is %f"), tziOld.Bias/60.0);

    dwRet = GetTimeZoneInformation(&tziOld);

    if (dwRet == 1 || dwRet == 0)
    {
        UE_LOG(LogBrainCloud, Log, TEXT("timezone is %s"), *FString(tziOld.StandardName));
    }
    else if (dwRet == 2)
    {
        UE_LOG(LogBrainCloud, Log, TEXT("timezone is %s"), *FString(tziOld.DaylightName));
    }
    else
    {
        UE_LOG(LogBrainCloud, Log, TEXT("GTZI failed (%d)"), GetLastError());
    }

#endif

    return offset;
}
