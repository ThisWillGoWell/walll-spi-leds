/*
 * alarm.h
 *
 *  Created on: Aug 23, 2019
 *      Author: will
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_


#define AlarmId uint32_t

void initScheduler();
void EnableAuto();
void DisableAuto();

#define OnHour 22
#define OnMin 56


#define OffHour 22
#define OffMin 57

#define POWER_CHECK_RATE (1 * 10 * 1000)
#define PATTERN_CHANGE_RATE (1 * 60 * 1000)
#endif /* SCHEDULER_H_ */
