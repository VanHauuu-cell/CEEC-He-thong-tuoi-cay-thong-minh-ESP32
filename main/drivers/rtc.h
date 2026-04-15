#ifndef RTC_H
#define RTC_H

void ds3231_init(void);
void rtc_get_time(int *hour, int *minute);

#endif
