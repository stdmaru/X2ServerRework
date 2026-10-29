#ifndef SERDEQUE_H
#define SERDEQUE_H "$Id: SerDeque.h,v 1.3 2003/01/14 05:44:39 cvs Exp $"

#include "SerDefs.h"
#include "SerSTL.h"

#include <deque>

/**
 * SerializeHelper functions for deque<>
 **/
namespace SerializeHelper
{
    template <typename T>
    inline SerializeTag ComputeTag(const std::deque<T>&, Adl = Adl())
    {
        return eTAG_DEQUE;
    }

    template <typename T>
    inline bool PutInto(KSerializer& ser, const std::deque<T>& obj, Adl = Adl())
    {
        return PutRange(ser, obj.begin(), obj.end(), obj.size());
    }

    template <typename T>
    inline bool GetFrom(KSerializer& ser, std::deque<T>& obj, Adl = Adl())
    {
        obj.clear();
        return GetRange(ser, std::back_inserter(obj), (T*)0);
    }
}

#endif
