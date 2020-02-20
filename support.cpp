//
//  support.cpp
//  Project 4
//
//  Created by Allen Miyazawa on 3/14/17.
//  Copyright © 2017 Allen Miyazawa. All rights reserved.
//

#include "support.h"
#include "provided.h"

bool operator==(const GeoCoord &one, const GeoCoord &other) //compare two geo coords
{
    if (one.latitude == other.latitude && one.longitude == other.longitude)
        return true;
    return false;
}

bool operator<(const GeoCoord &one, const GeoCoord &other) //compare two geo coords
{
    if (one.latitude < other.latitude || one.longitude < other.longitude)
        return true;
    return false;
}