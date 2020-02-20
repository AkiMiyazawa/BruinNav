//
//  AttractionMapper.cpp
//  Project 4
//
//  Created by Allen Miyazawa on 3/14/17.
//  Copyright © 2017 Allen Miyazawa. All rights reserved.
//

#include "provided.h"
#include <string>
#include "MyMap.h"

using namespace std;

class AttractionMapperImpl
{
public:
    AttractionMapperImpl();
    ~AttractionMapperImpl();
    void init(const MapLoader& ml);
    bool getGeoCoord(std::string attraction, GeoCoord& gc) const;
private:
    MyMap<std::string,GeoCoord> k;
};

AttractionMapperImpl::AttractionMapperImpl()
{
}

AttractionMapperImpl::~AttractionMapperImpl()
{
    
    
}
void makeSmall(string& a) //make string lowercased
{
    for (int i = 0; i < a.length();i++)
    {
        a[i] = tolower(a[i]);
    }
}

void AttractionMapperImpl::init(const MapLoader& ml)
{
    StreetSegment temp;
    for (int i = 0; i < ml.getNumSegments(); i++)
    {
        ml.getSegment(i,temp); //get street segment
        for (int j = 0; j < temp.attractions.size();j++)
        {
            string tempName =temp.attractions[j].name;
            makeSmall(tempName);
            k.associate(tempName, temp.attractions[j].geocoordinates); //associate attraction names to their geo coord in tree
        }
    }
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
    string lowerAtt = attraction;
    makeSmall(lowerAtt);
    const GeoCoord* ptr = k.find(lowerAtt); //get pointer to geo coord
    if(ptr == nullptr)
        return false;
    else
        gc = GeoCoord(ptr->latitudeText, ptr->longitudeText);
    return true;
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
    m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
    delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
    m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
    return m_impl->getGeoCoord(attraction, gc);
}

