
#include "provided.h"
#include <vector>
#include "MyMap.h"

using namespace std;

class SegmentMapperImpl
{
public:
    SegmentMapperImpl();
    ~SegmentMapperImpl();
    void init(const MapLoader& ml);
    std::vector<StreetSegment> getSegments(const GeoCoord& gc) const;
    void addSegments(GeoCoord s, StreetSegment p);
private:
    MyMap<GeoCoord,std::vector<StreetSegment>> m;
};

SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

void SegmentMapperImpl::init(const MapLoader& ml)
{
    StreetSegment temp;
    for (int i = 0; i < ml.getNumSegments(); i++)
    {
        ml.getSegment(i,temp);
        addSegments(temp.segment.start, temp); //associate starting geo coord to streetsegment
        addSegments(temp.segment.end, temp); //--- ending geo coord to streetsegmetn
        for (int j = 0; j < temp.attractions.size();j++)
        {
            addSegments(temp.attractions[j].geocoordinates, temp); //--- all attractions to streetsegment
        }
    }
}
void SegmentMapperImpl::addSegments(GeoCoord s, StreetSegment p)
{ //add all streetsegment with the geo coord s to a vector
    vector<StreetSegment> li;
    if (m.find(s) != nullptr)
    {
        li = *(m.find(s));
    }
    li.push_back(p);
    m.associate(s,li);
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
    vector<StreetSegment> segments;
    if (m.find(gc) != nullptr)
        segments = *(m.find(gc));
    return segments;
}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
    m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
    delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
    m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
    return m_impl->getSegments(gc);
}
