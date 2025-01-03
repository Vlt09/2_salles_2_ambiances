#pragma once
#include <vector>

namespace Utils
{
    inline float linearMapping(float value, float minDomain, float maxDomain, float minCodomain, float maxCodomain)
    {
        return (value - minDomain) * (maxCodomain - minCodomain) / (maxDomain - minDomain) + minCodomain;
    }
}
