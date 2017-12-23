#include "provided.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

class MapLoaderImpl
{
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
	int m_numSegs;
	vector<StreetSegment> m_segments;
};

MapLoaderImpl::MapLoaderImpl()
{
	m_numSegs = 0;
}

MapLoaderImpl::~MapLoaderImpl()
{
}

bool MapLoaderImpl::load(string mapFile) // O(N)
{
	
	ifstream f(mapFile);
	
	if (!f)
		return false;

	string line; //coord;// , attraction = "";
	int numAttractions = 0;
	//vector<string> geoCoords;
	//vector<string> attractioninfo;
	//for (int i = 0; i < 20000; i++)
	while (getline(f, line))
	{
		vector<string> geoCoords;
		StreetSegment s;
		string coord;
		// get name
		s.streetName = line;
		
		// get segment
		getline(f, line);
		for (int i = 0; i < line.size(); i++)
		{
			if (line[i] != ',' && line[i] != ' ')
			{
				coord += line[i];
				if (i == line.size() - 1)
					geoCoords.push_back(coord);
			}
			else if(coord != "")
			{
				geoCoords.push_back(coord);
				coord = "";
			}
		}
		
		GeoCoord g1(geoCoords[0], geoCoords[1]);
		GeoCoord g2(geoCoords[2], geoCoords[3]);
		GeoSegment sg(g1, g2);
		s.segment = GeoSegment(GeoCoord(geoCoords[0], geoCoords[1]), GeoCoord(geoCoords[2], geoCoords[3]));
		
		//get attractions
		
		numAttractions = 0;
		getline(f, line);
		int part = 0;
		for (int i = 0; i < line.size(); i++)
		{
			part = (line[i] - 48) * pow(10, line.size() - 1 - i);
			numAttractions += part;
		}

		
		for (int i = 0; i < numAttractions; i++)
		{
			vector<string> attractioninfo;
			string attraction;
			getline(f, line);
			int j;
			for (j = 0; j < line.size(); j++) // get name of attraction
			{
				if (line[j] != '|')
				{
					attraction += line[j];
					if (j == line.size() - 1)
						attractioninfo.push_back(attraction);
				}
				else if (attraction != "")
				{
					attractioninfo.push_back(attraction);
					attraction = "";
					break;
				}
			}

			for (int i = j + 1; i < line.size(); i++) // get coordinates of attraction
			{
				if (line[i] != ',' && line[i] != ' ')
				{
					attraction += line[i];
					if (i == line.size() - 1)
						attractioninfo.push_back(attraction);
				}
				else if (attraction != "")
				{
					attractioninfo.push_back(attraction);
					attraction = "";
				}
			}
			Attraction a;
			a.name = attractioninfo[0];
			
			a.geocoordinates = GeoCoord(attractioninfo[1], attractioninfo[2]);
			
			s.attractions.push_back(a);
		}
	
		 m_segments.push_back(s);
	}
	return true;  
}

size_t MapLoaderImpl::getNumSegments() const
{
	return m_segments.size();
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
	if (segNum < 0 || segNum > getNumSegments() - 1)
		return false;  

	seg = m_segments[segNum];
	return true;
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
