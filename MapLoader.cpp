#include "provided.h"
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

class MapLoaderImpl
{
public:
    MapLoaderImpl();
    ~MapLoaderImpl();
    bool load(std::string mapFile);
    size_t getNumSegments() const;
    bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
    std::vector<StreetSegment*> s;
};


MapLoaderImpl::MapLoaderImpl()
{
}

MapLoaderImpl::~MapLoaderImpl()
{
    while(!s.empty())
    {
        delete s.back();
        s.pop_back();
    }
}

bool MapLoaderImpl::load(string mapFile)
{
    ifstream infile(mapFile); //read the file
    if (!infile)
        return false;
    string line;
    int count = 1; //count tells the function what each line contains
    int numAttraction = 0; //number of attractions
    bool isAttraction = false;
    StreetSegment temp;
    while(getline(infile,line))
    {
        if (count == 1) //street name
        {
            temp.streetName = line;
            count++;
        }
        else if(count == 2) //the geo coordinates
        {
            
            size_t commaPos = line.find(","); //find the first comma
            string startLat = line.substr(0,commaPos); //starting latitude is everything until the comma
            size_t secondSpace;
            string startLon;
            string endLat;
            string endLon;
            if (line[commaPos+1] == ' ') //if there's a space after comma
            {
                secondSpace = commaPos+2+line.substr(commaPos+2).find(" "); //find the space between two geo coordinates
                startLon = line.substr(commaPos+2,secondSpace-commaPos-2);
                commaPos = secondSpace+1+line.substr(secondSpace+1).find(","); //find the comma between ending geo coords
                endLat = line.substr(secondSpace+1,commaPos-secondSpace-1);
                if(line[commaPos+1] == ' ')
                {
                    endLon = line.substr(commaPos+2);
                }
                else
                {
                    endLon = line.substr(commaPos+1);
                }

            }
            else //if no space after comma
            {
                secondSpace = commaPos+1+line.substr(commaPos+2).find(" ");
                startLon = line.substr(commaPos+1,secondSpace-commaPos);
                commaPos = secondSpace+1+line.substr(secondSpace+1).find(",");
                endLat = line.substr(secondSpace+2,commaPos-secondSpace-2);
                if(line[commaPos+1] == ' ')
                {
                    endLon = line.substr(commaPos+2);
                }
                else
                {
                    endLon = line.substr(commaPos+1);
                }

            }
            GeoCoord s(startLat,startLon);
            GeoCoord e(endLat,endLon);
            GeoSegment coo(s,e);
            temp.segment = coo; //put geosegment into streetsegment
            count++;
            
        }
        else if(count == 3) //attractions
        {
            if (!isAttraction)
            {
                numAttraction = stoi(line.substr()); //get the number of attractions
                isAttraction = true;
            }
            else
            {
                if(numAttraction > 0)
                {
                    Attraction att;
                    size_t sepPoint = line.find("|"); //the point that separates the name and geo coord
                    att.name = line.substr(0,sepPoint);
                    size_t commaPos = sepPoint+1+line.substr(sepPoint+1).find(","); //position of comma in geo coord
                    string Lat = line.substr(sepPoint+1,commaPos-sepPoint-1);
                    string Lon;
                    if (line[commaPos+1] == ' ')
                    {
                        Lon = line.substr(commaPos+2);
                    }
                    else
                    {
                        Lon = line.substr(commaPos+1);
                    }
                    GeoCoord cAt(Lat,Lon);
                    att.geocoordinates = cAt;
                    temp.attractions.push_back(att); //push attraction into attraction vector
                    numAttraction--; //decrement to keep track of attractions left
                }
            }
            if (numAttraction == 0) //streetsegment ends
            {
                isAttraction = false;
                StreetSegment* n = new StreetSegment;
                *n = temp;
                s.push_back(n); //push streetsegment to vector
                count = 1;
                temp.attractions.clear();
            }
        }
        
    }
    return true;
}


size_t MapLoaderImpl::getNumSegments() const
{
    return s.size();
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
    if(segNum >= getNumSegments()) //false if number is over the possible limit
        return false;
    else
    {
        seg = *s[segNum]; //get streetsegment at segNum
        return true;
    }
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
    m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
    delete m_impl;
}

bool MapLoader::load(string mapFile)
{
    return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
    return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
    return m_impl->getSegment(segNum, seg);
}


