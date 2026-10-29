#ifndef SERMAP_H
#define SERMAP_H "$Id: SerMap.h,v 1.3 2003/01/14 05:44:39 cvs Exp $"

#include "SerDefs.h"
#include "SerPair.h"
#include "SerSTL.h"

#include <map>
#include <iterator>

/**
 * SerializeHelper functions for map<> and multimap<>
 **/
namespace SerializeHelper
{
    template <typename KEY, typename VALUE, typename COMPARE>
    inline SerializeTag ComputeTag(const std::map<KEY,VALUE,COMPARE>&, Adl = Adl())
    {
        return eTAG_MAP;
    }
    template <typename KEY, typename VALUE, typename COMPARE>
    inline bool PutInto(KSerializer& ser, const std::map<KEY,VALUE,COMPARE>& obj, Adl = Adl())
    {
        return PutRange(ser, obj.begin(), obj.end(), obj.size());
    }

    template <typename KEY, typename VALUE, typename COMPARE>
    inline bool GetFrom(KSerializer& ser, std::map<KEY,VALUE,COMPARE>& obj, Adl = Adl())
    {
        obj.clear();
        return GetRange(ser, std::inserter(obj, obj.end()), (std::pair<KEY,VALUE>*)0);
    }

    template <typename KEY, typename VALUE, typename COMPARE>
    inline SerializeTag ComputeTag(const std::multimap<KEY,VALUE,COMPARE>&, Adl = Adl())
    {
        return eTAG_MULTIMAP;
    }

    template <typename KEY, typename VALUE, typename COMPARE>
    inline bool PutInto(KSerializer& ser, const std::multimap<KEY,VALUE,COMPARE>& obj, Adl = Adl())
    {
        return PutRange(ser, obj.begin(), obj.end(), obj.size());
    }

    template <typename KEY, typename VALUE, typename COMPARE>
    inline bool GetFrom(KSerializer& ser, std::multimap<KEY,VALUE,COMPARE>& obj, Adl = Adl())
    {
    obj.clear();
        return GetRange(ser, std::inserter(obj, obj.end()), (std::pair<KEY,VALUE>*)0);
    }
}

#endif
