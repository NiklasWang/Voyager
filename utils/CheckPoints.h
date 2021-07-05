#ifndef _CHECK_POINTS_H_
#define _CHECK_POINTS_H_

#include "CheckPointsImpl.h"

namespace voyager {

#ifdef ENABLE_CHECK_POINTS_TRACKER

extern CheckPointsImpl gDefaultCheckPointsImpl;

#define CheckPointRequest(name)                 gDefaultCheckPointsImpl.request(CHECK_POINT_TYPE_RANDOM_ORDER, name)
#define CheckPointRequestBlur(name)             gDefaultCheckPointsImpl.requestBlur(CHECK_POINT_TYPE_RANDOM_ORDER, name)
#define CheckPointRequestNone(name)             gDefaultCheckPointsImpl.requestNone(CHECK_POINT_TYPE_RANDOM_ORDER, name)
#define CheckPointRequestNoneBlur(name)         gDefaultCheckPointsImpl.requestNoneBlur(CHECK_POINT_TYPE_RANDOM_ORDER, name)
#define CheckPointRequestFmt(fmt, args...)      gDefaultCheckPointsImpl.request(CHECK_POINT_TYPE_RANDOM_ORDER, true, fmt, ##args)
#define CheckPointReach(name)                   gDefaultCheckPointsImpl.reach(CHECK_POINT_TYPE_RANDOM_ORDER, name)
#define CheckPointReachFmt(fmt, args...)        gDefaultCheckPointsImpl.reach(CHECK_POINT_TYPE_RANDOM_ORDER, fmt, ##args)
#define CheckPointTrack(func)                   gDefaultCheckPointsImpl.track(CHECK_POINT_TYPE_RANDOM_ORDER, func)
#define CheckPointTrackIgnoreReturn(func)       gDefaultCheckPointsImpl.track(CHECK_POINT_TYPE_RANDOM_ORDER, func, true)
#define CheckPointDump()                        gDefaultCheckPointsImpl.dump(CHECK_POINT_TYPE_RANDOM_ORDER)
#define CheckPointClear()                       gDefaultCheckPointsImpl.clear(CHECK_POINT_TYPE_RANDOM_ORDER)

#define CheckPointFixedRequest(name)            gDefaultCheckPointsImpl.request(CHECK_POINT_TYPE_FIXED_ORDER, name)
#define CheckPointFixedRequestBlur(name)        gDefaultCheckPointsImpl.requestBlur(CHECK_POINT_TYPE_FIXED_ORDER, name)
#define CheckPointFixedRequestNone(name)        gDefaultCheckPointsImpl.requestNone(CHECK_POINT_TYPE_FIXED_ORDER, name)
#define CheckPointFixedRequestNoneBlur(name)    gDefaultCheckPointsImpl.requestNoneBlur(CHECK_POINT_TYPE_FIXED_ORDER, name)
#define CheckPointFixedRequestFmt(fmt, args...) gDefaultCheckPointsImpl.request(CHECK_POINT_TYPE_FIXED_ORDER, true, fmt, ##args)
#define CheckPointFixedReach(name)              gDefaultCheckPointsImpl.reach(CHECK_POINT_TYPE_FIXED_ORDER, name)
#define CheckPointFixedReachFmt(fmt, args...)   gDefaultCheckPointsImpl.reach(CHECK_POINT_TYPE_FIXED_ORDER, fmt, ##args)
#define CheckPointFixedTrack(func)              gDefaultCheckPointsImpl.track(CHECK_POINT_TYPE_FIXED_ORDER, func)
#define CheckPointFixedTrackIgnoreReturn(func)  gDefaultCheckPointsImpl.track(CHECK_POINT_TYPE_FIXED_ORDER, func, true)
#define CheckPointFixedDump()                   gDefaultCheckPointsImpl.dump(CHECK_POINT_TYPE_FIXED_ORDER)
#define CheckPointFixedClear()                  gDefaultCheckPointsImpl.clear(CHECK_POINT_TYPE_FIXED_ORDER)

#else

#define CheckPointRequest(name)
#define CheckPointRequestBlur(name)
#define CheckPointRequestNone(name)
#define CheckPointRequestNoneBlur(name)
#define CheckPointRequestFmt(fmt, args...)
#define CheckPointReach(name)
#define CheckPointReachFmt(fmt, args...)
#define CheckPointTrack(func)
#define CheckPointTrackIgnoreReturn(func)
#define CheckPointDump()
#define CheckPointClear()

#define CheckPointFixedRequest(name)
#define CheckPointFixedRequestBlur(name)
#define CheckPointFixedRequestNone(name)
#define CheckPointFixedRequestFmt(fmt, args...)
#define CheckPointFixedReach(name)
#define CheckPointFixedReachFmt(fmt, args...)
#define CheckPointFixedTrack()
#define CheckPointFixedDump()
#define CheckPointFixedClear()

#endif

};

#endif
