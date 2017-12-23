#include "provided.h"
#include "MyMap.h"
#include <iostream>
#include <string>
using namespace std;

class AttractionMapperImpl
{
public:
	AttractionMapperImpl();
	~AttractionMapperImpl();
	void init(const MapLoader& ml);
	bool getGeoCoord(string attraction, GeoCoord& gc) const;
private:
	MyMap<string, GeoCoord> m_attractions;
};

AttractionMapperImpl::AttractionMapperImpl()
{
}

AttractionMapperImpl::~AttractionMapperImpl()
{
}

void AttractionMapperImpl::init(const MapLoader& ml) // needs to run in O(N+A*log(A))
{
	StreetSegment s;
	char c;
	for (int i = 0; i < ml.getNumSegments(); i++)
	{
		ml.getSegment(i, s);
		for (int j = 0; j < s.attractions.size(); j++)
		{
			string upper;
			for (int i = 0; i < s.attractions[j].name.size(); i++)
			{
				c = toupper(s.attractions[j].name[i]);
				upper += c;
			}
			m_attractions.associate(upper, s.attractions[j].geocoordinates);
		}
	}
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const // O(log(A)) 
{
	string upper;
	char c;

	for (int i = 0; i < attraction.size(); i++)
	{
		c = toupper(attraction[i]);
		upper += c;
	}

	if (m_attractions.find(upper) == nullptr)
		return false;

	gc = *(m_attractions.find(upper));
	
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
