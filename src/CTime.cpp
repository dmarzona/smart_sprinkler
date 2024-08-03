#include "Arduino.h"
#include "CTime.h"

CTime::CTime():
      epoch(0)
    , seconds(0)
    , minutes(0)
    , hours(0)
    , day(1)
    , month(1)
    , year(1970)
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

void CTime::UpdateEpoch(const char* date_string)
{
    sscanf(date_string, "%d-%d-%d %d:%d:%d",
            &year, &month, &day,
            &hours, &minutes, &seconds);
    if(year >= 1970 && month !=0 && day != 0)
    {
        CalculateEpoch();
    }
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

void CTime::CalculateEpoch()
{
    int daysPerMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int start_year = 1970;
    
    // Add seconds for full years since 1970
    for (int y = start_year; y < year; y++)
    {
        epoch += IsLeapYear(y) ? 366 * DAY_IN_SECONDS : 365 * DAY_IN_SECONDS;
    }
    
    // Add seconds for full months of the current year
    for (int m = 0; m < month-1; m++)
    {
        epoch += daysPerMonth[m] * DAY_IN_SECONDS;
        if (m == February && IsLeapYear(year))
        {
            epoch += DAY_IN_SECONDS;
        }
    }
    
    // Add seconds for full days of the current month
    epoch += (day - 1) * DAY_IN_SECONDS;
    
    // Add seconds for the current day
    epoch += hours * HOUR_IN_SECONDS;
    epoch += minutes * MINUTE_IN_SECONDS;
    epoch += seconds;
}

int CTime::operator-(const CTime& other) const
{
    return abs(this->epoch - other.epoch);
}
