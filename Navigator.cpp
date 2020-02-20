#include "provided.h"
#include <string>
#include <vector>
#include "MyMap.h"
#include <list>
#include "support.h"
#include <algorithm>
using namespace std;

class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
    string convertDir(const double angle) const;
    string leftOrRight(const double angle) const;
private:
    MapLoader ml;
    AttractionMapper am;
    SegmentMapper sm;
};


struct ScoreSegment
{
    ScoreSegment(GeoCoord s, GeoCoord end, ScoreSegment* p, double i, double prevCumG)
    {
        co = s;
        prev = p;
        hScore = distanceEarthMiles(co, end);
        gscore = i;
        cumGScore = i+prevCumG;
        totalScore = hScore + cumGScore;
        comptotalScore = -totalScore;
    }
    ScoreSegment()
    {
        co = GeoCoord();
        prev = nullptr;
        hScore = 0;
        gscore = 0;
        cumGScore = 0;
        totalScore = 0;
        comptotalScore = -totalScore;
    }
    GeoCoord co;
    ScoreSegment* prev; //pointer to previous node in tree
    double hScore; //the distance from point to end
    double gscore; //distance from point to previous point
    double cumGScore; //cumulative distance travelled
    double totalScore; // cumulative distance travelled + distance from point to end
    double comptotalScore; //negative of totalScore
};


bool operator<(const ScoreSegment &one, const ScoreSegment &other)
{
    return (one.comptotalScore < other.comptotalScore); //compare the negatives of total score,so heap is sorted by
                                                            //lowest totalscore
}


NavigatorImpl::NavigatorImpl()
{
}

NavigatorImpl::~NavigatorImpl()
{
}

bool NavigatorImpl::loadMapData(string mapFile)
{
    bool sf = ml.load(mapFile);
    if (!sf)
        return false;
    am.init(ml);
    sm.init(ml);
    return true;
}
string NavigatorImpl::convertDir(const double angle) const
{ //calculates the direction based on the angle
    if (0<=angle && angle <= 22.5)
        return "east";
    else if(22.5 <angle && angle <= 67.5)
        return "northeast";
    else if(67.5 < angle && angle <= 112.5)
        return "north";
    else if(112.5 <angle && angle <= 157.5)
        return "northwest";
    else if(157.5 <angle && angle <= 202.5)
        return "west";
    else if(202.5 <angle && angle <= 247.5)
        return "southwest";
    else if(247.5 <angle && angle <= 292.5)
        return "south";
    else if(292.5 <angle && angle <= 337.5)
        return "southeast";
    else if(337.5 <angle && angle < 360)
        return "east";
    else
        return "      ";
}

string NavigatorImpl::leftOrRight(const double angle) const
{ //calculates which way to turn based on angle
    if (angle < 180)
        return "left";
    else if(angle >= 180)
        return "right";
    return "      ";
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
    bool isFound = false;
    bool ifReplaced = false;
    MyMap<GeoCoord, ScoreSegment> l; //closed list
    vector<ScoreSegment> geoVec; //open list
    GeoCoord startGC; //GeoCoord of start
    GeoCoord endGC;   //GeoCoord of end
    if (!(am.getGeoCoord(start,startGC))) //check if start is valid
        return NAV_BAD_SOURCE;
    if (!(am.getGeoCoord(end,endGC))) //check if end is valid
        return NAV_BAD_DESTINATION;
    
    vector<StreetSegment> startSeg = sm.getSegments(startGC); //the streetsegment with start
    GeoCoord tempSS(startSeg[0].segment.start.latitudeText, startSeg[0].segment.start.longitudeText); //one end of start streetseg
    GeoCoord tempSE(startSeg[0].segment.end.latitudeText, startSeg[0].segment.end.longitudeText); //another end of start
    ScoreSegment start1(tempSS, endGC, nullptr, distanceEarthMiles(startGC, tempSS), 0);
    ScoreSegment start2(tempSE, endGC, nullptr, distanceEarthMiles(startGC, tempSE), 0);
    geoVec.push_back(start1); //adding the starting positions to open list
    push_heap(geoVec.begin(), geoVec.end());
    geoVec.push_back(start2);
    push_heap(geoVec.begin(), geoVec.end());
    vector<StreetSegment> endSeg = sm.getSegments(endGC); //the streetsegment with end
    GeoCoord tempES(endSeg[0].segment.start.latitudeText, endSeg[0].segment.start.longitudeText); //one end of end streetseg
    GeoCoord tempEE(endSeg[0].segment.end.latitudeText, endSeg[0].segment.end.longitudeText); //another end
    
    if (startSeg[0].segment.start == endSeg[0].segment.start && startSeg[0].segment.end == endSeg[0].segment.end)
    { //if the start and the end are on the same street segment
        directions.clear();
        GeoSegment oneSeg(startGC, endGC);
        NavSegment onlyOne(convertDir(angleOfLine(oneSeg)),startSeg[0].streetName,
                           distanceEarthMiles(startGC,endGC),oneSeg);
        directions.push_back(onlyOne); //only one NavSegment in vector
        return NAV_SUCCESS;
    }
    ScoreSegment* returnPtr = nullptr; //pointer for tracking when the end street segment is reached
    while (!(geoVec.empty()))
    {
        ScoreSegment returnSeg;
        make_heap(geoVec.begin(), geoVec.end()); //sort vector according to how small the totalScore is
        
        ScoreSegment low = geoVec.front(); //Get the square with the lowest total score
        pop_heap(geoVec.begin(), geoVec.end());
        geoVec.pop_back();
        l.associate(low.co, low); //add the current to the closed list
        vector<StreetSegment> p = sm.getSegments(low.co); // Retrieve all its walkable adjacent squares
        for (int ert = 0; ert < p.size(); ert++)
        {
            GeoCoord sideCo1 = p[ert].segment.start;
            GeoCoord sideCo2 = p[ert].segment.end;
            

            if (sideCo1 == low.co) //to make sure there's no repetition
            {
                returnSeg = ScoreSegment(sideCo2, endGC, l.find(low.co), distanceEarthMiles(sideCo2, low.co),low.cumGScore);
                if(sideCo2 == tempES || sideCo2 == tempEE)
                { //if we add one of two end points of end street segment, we found the path!
                    isFound = true;
                    l.associate(returnSeg.co, returnSeg);
                    returnPtr = l.find(returnSeg.co);
                    break;
                }
            }
            else if(sideCo2 == low.co)
            {
                returnSeg = ScoreSegment(sideCo1, endGC, l.find(low.co), distanceEarthMiles(sideCo1, low.co),low.cumGScore);
                if(sideCo1 == tempES || sideCo1 == tempEE)
                {
                    isFound = true;
                    l.associate(returnSeg.co, returnSeg);
                    returnPtr = l.find(returnSeg.co);
                    break;
                }
            }
            
            ScoreSegment* rty = l.find(returnSeg.co);
            if (rty != nullptr) //// if this adjacent is already in the closed list ignore it
            {
                continue;
            }

            for (int vv = 0; vv < geoVec.size(); vv++)
            {
                if(returnSeg.co == geoVec[vv].co) //if the adjacent is in the open list
                {
                    ifReplaced = true;
                    if(returnSeg.totalScore < geoVec[vv].totalScore) //if the current total score is greater than the new totalscore
                    {//update everything
                        geoVec[vv].prev = returnSeg.prev;
                        geoVec[vv].gscore = returnSeg.gscore;
                        geoVec[vv].cumGScore = returnSeg.cumGScore;
                        geoVec[vv].totalScore = returnSeg.totalScore;
                        geoVec[vv].comptotalScore = returnSeg.comptotalScore;
                        make_heap(geoVec.begin(), geoVec.end());
                        break;
                    }
                    
                }
            }
            
            if(!ifReplaced) //if the adjacent havent replaced something in open list
            {
                geoVec.push_back(returnSeg); //push to open list
                push_heap(geoVec.begin(), geoVec.end());
            }
            ifReplaced = false;
        }
        if(isFound) //break from loop when destination is found
            break;
    }
    if (geoVec.empty())
    {
        return NAV_NO_ROUTE; //no route if open list is empty
    }
    else
    {
        list<ScoreSegment> li;
        while (returnPtr != nullptr)
        {
            li.push_front(*returnPtr); //push the ScoreSegments (goes from the botton of tree) to the top of a list
            returnPtr = returnPtr->prev; //track to parent pointer
        }
        vector<NavSegment> tempNav;
        GeoSegment beg(startGC,li.front().co); //get the segment from start to one endpoint of street segment
        double firstDistance = distanceEarthMiles(startGC,li.front().co); //get distance
        NavSegment navSeg(convertDir(angleOfLine(beg)),startSeg[0].streetName,
                          firstDistance,beg);
        tempNav.push_back(navSeg); //push the first NavSegment to vector
        list<ScoreSegment>::iterator it = li.begin();
        for (it++; it != li.end(); it++) // iterate through list
        {
            string sName;
            GeoCoord forName1 = it->prev->co; //previous segment
            GeoCoord forName2 = it->co;     //current segment
            vector<StreetSegment> vForName1 = sm.getSegments(forName1);
            vector<StreetSegment> vForName2 = sm.getSegments(forName2);
            for (int qq = 0; qq < vForName1.size(); qq++)
            {
                for (int pp = 0; pp < vForName2.size();pp++)
                {
                    if (vForName1[qq].streetName == vForName2[pp].streetName) //if prev and curr segment are the same street
                    {
                        sName = vForName1[qq].streetName; //set common name
                        break;
                    }
                }
            }
            GeoSegment forAngle(forName1,forName2);
            if (tempNav.back().m_streetName == sName)  //Proceed
            {
                NavSegment addNav1(convertDir(angleOfLine(forAngle)), sName,it->gscore, forAngle);
                tempNav.push_back(addNav1);
            }
            else //Turn
            {
                GeoSegment secondPt(tempNav.back().m_geoSegment.start,forName2);
                NavSegment addNav2(leftOrRight(angleBetween2Lines(secondPt,forAngle)), sName);
                tempNav.push_back(addNav2);
                NavSegment addNav1(convertDir(angleOfLine(forAngle)), sName,it->gscore, forAngle);
                tempNav.push_back(addNav1);
            }
        }
        directions.clear();
        GeoSegment ending(li.back().co,endGC); //get last segment
        NavSegment lastSeg(convertDir(angleOfLine(ending)), endSeg[0].streetName, distanceEarthMiles(endGC, li.back().co), ending);
        tempNav.push_back(lastSeg);
        directions = tempNav;
    }
    return NAV_SUCCESS;
}

//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}
