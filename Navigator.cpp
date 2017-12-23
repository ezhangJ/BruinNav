#include "provided.h"
#include "MyMap.h"
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <list>
#include <cmath>
#include <iostream>
using namespace std;

class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
private:
	struct Node
	{
		Node* parent;
		StreetSegment street;
		GeoCoord coord;
		string ID;
		double f, h, g;
	};
	AttractionMapper m_attractionMapper;
	SegmentMapper m_segmentMapper;
	Node* m_head;
};

NavigatorImpl::NavigatorImpl()
{
}

NavigatorImpl::~NavigatorImpl()
{
}

bool NavigatorImpl::loadMapData(string mapFile)
{
	MapLoader m;
	if (!m.load(mapFile))
		return false;  
	m_attractionMapper.init(m);
	m_segmentMapper.init(m);
	return true;
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
	// Starting and Ending Coordinates
	GeoCoord startCoord; 
	GeoCoord endCoord;
	if (!m_attractionMapper.getGeoCoord(start, startCoord))
		return NAV_BAD_SOURCE;
	if (!m_attractionMapper.getGeoCoord(end, endCoord))
		return NAV_BAD_DESTINATION;
	
	// Starting and Ending Segments
	StreetSegment startSegment;
	StreetSegment endSegment;
	vector<StreetSegment> v;
	
 	v = m_segmentMapper.getSegments(startCoord);
	startSegment = v.at(0);
	v = m_segmentMapper.getSegments(endCoord);
	endSegment = v.at(0);
	
	string startID = startSegment.streetName + startSegment.segment.start.latitudeText
		+ ',' + startSegment.segment.start.longitudeText;
	string endID = endSegment.streetName + endSegment.segment.start.latitudeText
		+ ',' + endSegment.segment.start.longitudeText;
	
	vector<Node*> path;
	MyMap<string, bool> alreadyBeen; // string = name of street + starting coordinate
	list<Node*> streetSegmentQueue; // queue to find target attraction
	//list<Node*> stayOnStreet;
	
	Node* startNode = new Node; // ADDED THIS
	startNode->parent = nullptr;
	startNode->ID = startID;
	startNode->street = startSegment;
	startNode->coord = startSegment.segment.start;
	startNode->g = abs(distanceEarthMiles(startNode->coord, endCoord));  // CHANGE THIS NEEDS TO BE DISYANCE FROM ATTRACTION DO SAME FOR SECOND THING BELOW
	startNode->h = 0;
	startNode->f = 0;// startNode->g + startNode->h;

	Node* startNode2 = new Node; // ADDED THIS
	startNode2->parent = nullptr;
	startNode2->ID = startID;
	startNode2->street = startSegment;
	startNode2->coord = startSegment.segment.end;
	startNode2->g = abs(distanceEarthMiles(startNode2->coord, endCoord));
	startNode2->h = 0;
	startNode2->f = 0;// startNode2->g + startNode2->h;

	streetSegmentQueue.push_back(startNode);
	streetSegmentQueue.push_back(startNode2);

	path.push_back(startNode);
	path.push_back(startNode2);

	alreadyBeen.associate(startID, true);

	while (!streetSegmentQueue.empty())
	{
		Node* current = nullptr;
		current = streetSegmentQueue.front();
		
		if (current->ID == endID) // check if in endSegment with attraction
		{
			break;
		}

		streetSegmentQueue.pop_front();

		//Start   get segments related to starting coordinate
		vector<StreetSegment> segmentsStart = m_segmentMapper.getSegments(current->coord);
	
		for (int i = 0; i < segmentsStart.size(); i++)
		{
			
			string s = segmentsStart[i].streetName + segmentsStart[i].segment.start.latitudeText
				+ ',' + segmentsStart[i].segment.start.longitudeText;
			

			if (alreadyBeen.find(s) == nullptr)
			{
				
				Node* n = new Node;
				n->parent = current;
				n->ID = s;
				n->street = segmentsStart[i];
				if (segmentsStart[i].segment.start.latitudeText == current->coord.latitudeText
					&& segmentsStart[i].segment.start.longitudeText == current->coord.longitudeText)
				{
					n->coord = segmentsStart[i].segment.end;
					n->g = abs(distanceEarthMiles(current->coord, segmentsStart[i].segment.end) + n->parent->g);
				}
				else
				{
					n->coord = segmentsStart[i].segment.start;
					n->g = abs(distanceEarthMiles(current->coord, segmentsStart[i].segment.start) + n->parent->g);
				}

				n->h = abs(distanceEarthMiles(n->coord, endCoord));
				n->f = n->h + n->g;
			//	if (n->street.streetName == current->street.streetName && (!streetSegmentQueue.empty() && n->f <= streetSegmentQueue.front()->f))
				//	stayOnStreet.push_front(n);
				if (!streetSegmentQueue.empty() && n->f <= streetSegmentQueue.front()->f)
				{
					streetSegmentQueue.push_front(n);
				}
				else
				{
					list<Node*>::iterator p;
					for (p = streetSegmentQueue.begin(); p != streetSegmentQueue.end(); p++)
					{
						if (n->f < (*p)->f)
						{
							streetSegmentQueue.insert(p, n);
							break;
						}
					}
					if (p == streetSegmentQueue.end())
						streetSegmentQueue.push_back(n);
				}
				
				alreadyBeen.associate(s, true); // MAKE SURE YOU DONT GO INTO SAME PLACE

				path.push_back(n); // KEEP TRACK OF ALL NODES

			}
		}
	}

	Node* finish = nullptr;

	for (int i = 0; i < path.size(); i++)
	{
		if (path[i]->ID == endID)
		{
			finish = path[i];
			break;
		}

		if (i == path.size() - 1)
			return NAV_NO_ROUTE;
	}

	list<NavSegment> directionList;
	Node* pathback = finish->parent;

	string endDirection;


	double endAngleLine = angleOfLine(GeoSegment(pathback->coord, endCoord));
	
	if (endAngleLine >= 0 && endAngleLine <= 22.5)
		endDirection = "east";
	else if (endAngleLine > 22.5 && endAngleLine <= 67.5)
		endDirection = "northeast";
	else if (endAngleLine > 67.5 && endAngleLine <= 112.5)
		endDirection = "north";
	else if (endAngleLine > 112.5 && endAngleLine <= 157.5)
		endDirection = "northwest";
	else if (endAngleLine > 157.5 && endAngleLine <= 202.5)
		endDirection = "west";
	else if (endAngleLine > 202.5 && endAngleLine <= 247.5)
		endDirection = "southwest";
	else if (endAngleLine > 247.5 && endAngleLine <= 292.5)
		endDirection = "south";
	else if (endAngleLine > 292.5 && endAngleLine <= 337.5)
		endDirection = "southeast";
	else if (endAngleLine > 337.5 && endAngleLine < 360)
		endDirection = "east";

	NavSegment endNav(endDirection, endSegment.streetName, distanceEarthMiles(endCoord, pathback->coord), endSegment.segment);
	directionList.push_front(endNav);

	
	while (pathback->parent != nullptr)
	{
		if (pathback->street.streetName != directionList.front().m_streetName)
		{
			GeoSegment g1, g2;
			if (pathback->coord.latitudeText == pathback->street.segment.start.latitudeText)
			{
				g1.start = pathback->street.segment.end;
				g1.end = pathback->coord;
			}
			else
			{
				g1.start = pathback->street.segment.start;
				g1.end = pathback->coord;
			}

			if (g1.end.latitudeText == directionList.front().m_geoSegment.start.latitudeText)
			{
				g2.start = directionList.front().m_geoSegment.start;
				g2.end = directionList.front().m_geoSegment.end;
			}
			else
			{
				g2.start = directionList.front().m_geoSegment.end;
				g2.end = directionList.front().m_geoSegment.start;
			}

			double angleTurn = angleBetween2Lines(g1, g2);
			string direction;
			if (angleTurn > 180)
				direction = "right";
			else if (angleTurn < 180)
				direction = "left";
			NavSegment nav(direction, directionList.front().m_streetName);
			directionList.push_front(nav);


			
		}

		GeoSegment g1;
		if (pathback->coord.latitudeText == pathback->street.segment.start.latitudeText)
		{
			g1.start = pathback->street.segment.end;
			g1.end = pathback->coord;
		}
		else
		{
			g1.start = pathback->street.segment.start;
			g1.end = pathback->coord;
		}
			string direction;
			double angleLine = angleOfLine(g1);
			
			if (angleLine >= 0 && angleLine <= 22.5)
				direction = "east";
			else if (angleLine > 22.5 && angleLine <= 67.5)
				direction = "northeast";
			else if (angleLine > 67.5 && angleLine <= 112.5)
				direction = "north";
			else if (angleLine > 112.5 && angleLine <= 157.5)
				direction = "northwest";
			else if (angleLine > 157.5 && angleLine <= 202.5)
				direction = "west";
			else if (angleLine > 202.5 && angleLine <= 247.5)
				direction = "southwest";
			else if (angleLine > 247.5 && angleLine <= 292.5)
				direction = "south";
			else if (angleLine > 292.5 && angleLine <= 337.5)
				direction = "southeast";
			else if (angleLine > 337.5 && angleLine < 360)
				direction = "east";

			NavSegment nav(direction, pathback->street.streetName, pathback->g - pathback->parent->g, pathback->street.segment);
			directionList.push_front(nav);
		
			pathback = pathback->parent;
			
	}

	if (pathback->street.streetName != directionList.front().m_streetName)
	{
		GeoSegment g1, g2;
		if (pathback->coord.latitudeText == pathback->street.segment.start.latitudeText)
		{
			g1.start = pathback->street.segment.end;
			g1.end = pathback->coord;
		}
		else
		{
			g1.start = pathback->street.segment.start;
			g1.end = pathback->coord;
		}

		if (g1.end.latitudeText == directionList.front().m_geoSegment.start.latitudeText)
		{
			g2.start = directionList.front().m_geoSegment.start;
			g2.end = directionList.front().m_geoSegment.end;
		}
		else
		{
			g2.start = directionList.front().m_geoSegment.end;
			g2.end = directionList.front().m_geoSegment.start;
		}

		double angleTurn = angleBetween2Lines(g1, g2);
		string direction;
		if (angleTurn > 180)
			direction = "right";
		else if (angleTurn < 180)
			direction = "left";
		NavSegment nav(direction, directionList.front().m_streetName);
		directionList.push_front(nav);
	}

	string startDirection;
	double startAngleLine = angleOfLine(GeoSegment(startCoord, pathback->coord));

	if (startAngleLine >= 0 && startAngleLine <= 22.5)
		startDirection = "east";
	else if (startAngleLine > 22.5 && startAngleLine <= 67.5)
		startDirection = "northeast";
	else if (startAngleLine > 67.5 && startAngleLine <= 112.5)
		startDirection = "north";
	else if (startAngleLine> 112.5 && startAngleLine <= 157.5)
		startDirection = "northwest";
	else if (startAngleLine> 157.5 && startAngleLine <= 202.5)
		startDirection = "west";
	else if (startAngleLine > 202.5 && startAngleLine <= 247.5)
		startDirection = "southwest";
	else if (startAngleLine > 247.5 && startAngleLine <= 292.5)
		startDirection = "south";
	else if (startAngleLine > 292.5 && startAngleLine <= 337.5)
		startDirection = "southeast";
	else if (startAngleLine > 337.5 && startAngleLine < 360)
		startDirection = "east";

	NavSegment startNav(startDirection, startSegment.streetName, distanceEarthMiles(startCoord, pathback->coord), startSegment.segment);
	directionList.push_front(startNav);
	
	while (!directionList.empty())
	{
		directions.push_back(directionList.front());
		directionList.pop_front();
	}
	//DELETE EVERYTHING
	
	for (int i = 0; i < path.size(); i++)
	{
		delete path[i];
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
