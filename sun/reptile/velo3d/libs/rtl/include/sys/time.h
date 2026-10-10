

#ifndef __TIME_H
#define __TIME_H


#define DST_NONE	0	/* not on dst */
#define DST_USA		1	/* USA style dst */
#define DST_AUST	2	/* Australian style dst */
#define DST_WET		3	/* Western European dst */
#define DST_MET		4	/* Middle European dst */
#define DST_EET		5	/* Eastern European dst */
#define DST_CAN		6	/* Canada */


/*
 * Structure returned by gettimeofday(2) system call,
 * and used in other calls.
 */

struct timeval 
{
    long tv_sec;     /* seconds */
    long tv_usec;    /* and microseconds */

    //fred: i guess this is better.
    //unsigned long tv_sec;     /* seconds */
    //unsigned long tv_usec;    /* and microseconds */
};

struct timezone 
{
    int tz_minuteswest;    /* minutes west of Greenwich */
    int tz_dsttime;        /* type of dst correction */

    //fred: i guess this is better.
    //unsigned long tz_minuteswest;    /* minutes west of Greenwich */
    //unsigned long tz_dsttime;        /* type of dst correction */
};


int gettimeofday (struct timeval *tv, struct timezone *tz);
int settimeofday (const struct timeval *tv, const struct timezone *tz);


#endif    


