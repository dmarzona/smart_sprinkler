#ifndef __CTIME__
#define __CTIME__
#include <stdio.h>

#define DAY_IN_SECONDS 86400L
#define HOUR_IN_SECONDS 3600L
#define MINUTE_IN_SECONDS 60L

enum Months
{
    January = 0,
    February,
    March,
    April,
    May,
    June,
    July,
    August,
    September,
    October,
    November,
    December,
};

class CTime
{
    public:
        CTime();
        void UpdateEpoch(time_t new_epoch);
        void UpdateEpoch(const char* date_string);
        time_t GetEpoch();
        void Increase();
        int GetSeconds();
        int GetMinutes();
        int GetHours();
        int GetDay();
        int GetMonth();
        int GetYear();
        char* getDateString();
        char* getDateStringForFilename();
        int operator-(const CTime& other) const;
    
    private:
        void CalculateSeconds();
        void CalculateMinutes();
        void CalculateHours();
        void CalculateDate();
        void CalculateEpoch();
        int IsLeapYear(int year_to_check);

        int epoch;
        int seconds;
        int minutes;
        int hours;
        int day;
        int month;
        int year;
        char date[20];
        char file_name_string[11];
};
#endif
