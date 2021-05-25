/*  You may distribute under the terms of either the GNU General Public License
 *  or the Artistic License (the same terms as Perl itself)
 *
 *  (C) Paul Evans, 2012 -- leonerd@leonerd.org.uk
 */

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

MODULE = Time::timegm    PACKAGE = Time::timegm

int
timegm(sec,min,hour,mday,mon,year,wday=0,yday=0,isdst=-1)
    int sec
    int min
    int hour
    int mday
    int mon
    int year
    int wday
    int yday
    int isdst
  PREINIT:
    struct tm tm;
  CODE:
    tm.tm_sec  = sec;
    tm.tm_min  = min;
    tm.tm_hour = hour;
    tm.tm_mday = mday;
    tm.tm_mon  = mon;
    tm.tm_year = year;
    tm.tm_wday = wday;
    tm.tm_yday = yday;
    tm.tm_isdst = isdst;
#if defined(HAVE_TIMEGM)
    RETVAL = timegm(&tm);
#elif defined(HAVE_MKGMTIME)
    RETVAL = _mkgmtime(&tm);
#else
    croak("timegm() not available");
#endif
  OUTPUT:
    RETVAL
