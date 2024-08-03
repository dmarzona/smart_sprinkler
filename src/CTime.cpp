#include "Arduino.h"
#include "CTime.h"

static const unsigned long DAY_IN_SECONDS = 86400L;
static const unsigned long HOUR_IN_SECONDS = 3600L;
static const unsigned long MINUTE_IN_SECONDS = 60L;

CTime::CTime():
      epoch(0)
    , seconds(0)
    , minutes(0)
    , hours(0)
    , day(0)
    , month(0)
    , year(0)
{

}

void CTime::UpdateEpoch(time_t new_epoch)
{
    epoch = new_epoch;
    CalculateSeconds();
    CalculateMinutes();
    CalculateHours();
    CalculateDate();
}

time_t CTime::GetEpoch()
{
    return epoch;
}

void CTime::Increase()
{
    epoch++;
    CalculateSeconds();
    CalculateMinutes();
    CalculateHours();
    CalculateDate();
}

int CTime::GetSeconds()
{
    return seconds;
}

int CTime::GetMinutes()
{
    return minutes;
}

int CTime::GetHours()
{
    return hours;
}

int CTime::GetDay()
{
    return day;
}

int CTime::GetMonth()
{
    return month;
}

int CTime::GetYear()
{
    return year;
}

char* CTime::getDateString()
{
    snprintf(this->date, 20, "%04d-%02d-%02d %02d:%02d:%02d",
             year, month, day, hours, minutes, seconds);
    return date;
}

char* CTime::getDateStringForFilename()
{
    snprintf(this->file_name_string, 11, "%04d_%02d_%02d", year, month, day);
    return file_name_string;
}

void CTime::CalculateSeconds()
{
    seconds = int(epoch % 60);
}

void CTime::CalculateMinutes()
{
    minutes = (int)((epoch % HOUR_IN_SECONDS) / MINUTE_IN_SECONDS);
}

void CTime::CalculateHours()
{
    hours = (int)((epoch % DAY_IN_SECONDS) / HOUR_IN_SECONDS);
}

int CTime::IsLeapYear(int year_to_check)
{
    return (year_to_check % 4 == 0 && (year_to_check % 100 != 0 || year_to_check % 400 == 0));
}

void CTime::CalculateDate()
{
    int daysPerMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    year = 1970; // Epoch time starts in 1970
 
    // Calculate days since epoch
    long days = epoch / DAY_IN_SECONDS;
 
    // Adjust for leap years
    while (days >= 365) {
        if (IsLeapYear(year)) {
            if (days > 365) {
                days -= 366;
                year += 1;
            }
        } else {
            days -= 365;
            year += 1;
        }
    }
 
    // Adjust for current year's leap status
    if (IsLeapYear(year)) {
        daysPerMonth[February] = 29;
    }
 
    // Calculate month and day
    month = 0;
    while (days >= daysPerMonth[month]) {
        days -= daysPerMonth[month];
        month += 1;
    }
    month += 1;     // Months starts at one 
    day = days + 1; // Day of month starts at 1
}