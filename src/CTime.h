#include <stdio.h>

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
        time_t GetEpoch();
        void Increase();
        int GetSeconds();
        int GetMinutes();
        int GetHours();
        int GetDay();
        int GetMonth();
        int GetYear();
        char* getDateString();
    
    private:
        void CalculateSeconds();
        void CalculateMinutes();
        void CalculateHours();
        void CalculateDate();
        int IsLeapYear(int year_to_check);

        int epoch;
        int seconds;
        int minutes;
        int hours;
        int day;
        int month;
        int year;
        char date[20];
};