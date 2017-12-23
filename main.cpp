// The main.cpp you can use for testing will replace this file soon.

#include "provided.h"
#include "MyMap.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
using namespace std;

int main()
{
	/*
	string line;
	ifstream f("mapdata.txt");

	while (getline(f, line))
	{
		cout << line << endl;
	}
	*/

	/*
	MapLoader m;
	SegmentMapper a;
	StreetSegment s;
	GeoCoord g("34.0455760", "-118.4432295");
	vector<StreetSegment> v;
	if (m.load("mapdata.txt"))
	{
		for (int i = 0; i < m.getNumSegments(); i++)
		{
			m.getSegment(i, s);
			for (int j = 0; j < s.attractions.size(); j++)
			{
				if (s.attractions[j].name == "Equinox Fitness")
					cout << "yessum" << endl;
			}
			
		}
			a.init(m);
			v = a.getSegments(g);
			for (int i = 0; i < v.size(); i++)
			{
				cout << v[i].streetName << endl;
			}
				

	}
	else
		cout << "failed";
		
	cout << m.getNumSegments() << endl;
	*/

	/*
	string s = "poop";
	string* t = &s;
	MyMap<int, string> m;
	m.associate(1, "bob");
	m.associate(2, "bob");
	m.associate(1, "bob");
	m.associate(1, "bob");
	m.associate(1, "bob");
	m.associate(1, "bob");
	cout << m.size() << endl;
	t = m.find(2);
	cout << *t << endl;
	*/
	/*
	GeoCoord g("34.0500553", "-118.4830936"); // stay on track
	GeoCoord f("34.0502047", "-118.4831622");// turn
	GeoCoord end("34.0580186", "-118.4135996");

	cout << distanceEarthMiles(g, end) << endl;
	cout << distanceEarthMiles(f, end) << endl;
	*/

	/*
	Navigator n;
	vector<NavSegment> s;
	
	n.loadMapData("testmap.txt");
	//n.navigate("Harvard-Westlake Middle School", "Greystone Mansion", s);
	// 34.0908428,-118.4038080
	//n.navigate("Brentwood Country Mart", "The Annenberg Space for Photography", s);
	n.navigate("Eros Statue", "Hamleys Toy Store", s);
	
	for (int i = 0; i < s.size(); i++)
	{
		cout << s[i].m_streetName << endl;
	}
	*/
	// 34.0500505,-118.4831179
	/*
	MapLoader m;
	SegmentMapper a;
	vector<StreetSegment> v;
	GeoCoord g("34.0908428", "-118.4038080");
	m.load("mapdata.txt");
	a.init(m);
    v = a.getSegments(g);
	for (int i = 0; i < v.size(); i++)
	{
		cout << v[i].streetName << endl;
	}
	*/

	/*
	MapLoader m;
	AttractionMapper a;
	GeoCoord g;
	m.load("mapdata.txt");
	a.init(m);
	a.getGeoCoord( "Greystone Mansion", g);
	
	cout << g.latitudeText << " " << g.longitudeText << endl;
	*/
//GeoSegment start(GeoCoord("34.0490597", "-118.4880803"), GeoCoord("34.0492076","-118.4873561"));
//GeoSegment end(GeoCoord("34.0492076", "-118.4873561"), GeoCoord("34.0493581","-118.4865797"));
//GeoSegment start(GeoCoord("34.0496358","-118.4852136"), GeoCoord("34.0500505", "-118.4831179"));
//GeoSegment end(GeoCoord("34.0502047", "-118.4831622"), GeoCoord("34.0500505", "-118.4831179"));
// "34.0502047", "-118.4831622"
//cout << angleBetween2Lines(end, start) << endl;
cout << "About to test MyMap" << endl;
{
	MyMap<int, string> mm;
	mm.associate(20, "Ethel");
	mm.associate(10, "Fred");
	const string* p = mm.find(10);
	assert(p != nullptr  &&  *p == "Fred");
	assert(mm.find(30) == nullptr);
	assert(mm.size() == 2);
}
cout << "MyMap PASSED" << endl;

cout << "About to test MapLoader" << endl;
{
	MapLoader ml;
	assert(ml.load("testmap.txt"));
	size_t numSegments = ml.getNumSegments();
	assert(numSegments == 7);
	bool foundAttraction = false;
	for (size_t i = 0; i < numSegments; i++)
	{
		StreetSegment seg;
		assert(ml.getSegment(i, seg));
		if (seg.streetName == "Picadilly")
		{
			assert(seg.attractions.size() == 1);
			assert(seg.attractions[0].name == "Eros Statue");
			foundAttraction = true;
			break;
		}
	}
	assert(foundAttraction);
}
cout << "MapLoader PASSED" << endl;

cout << "About to test AttractionMapper" << endl;
{
	MapLoader ml;
	assert(ml.load("testmap.txt"));
	AttractionMapper am;
	am.init(ml);
	GeoCoord gc;
	assert(am.getGeoCoord("Hamleys Toy Store", gc));
	assert(gc.latitudeText == "51.512812");
	assert(gc.longitudeText == "-0.140114");
}
cout << "AttractionMapper PASSED" << endl;

cout << "About to test SegmentMapper" << endl;
{
	MapLoader ml;
	assert(ml.load("testmap.txt"));
	SegmentMapper sm;
	sm.init(ml);
	GeoCoord gc("51.510087", "-0.134563");
	vector<StreetSegment> vss = sm.getSegments(gc);
	assert(vss.size() == 4);
	string names[4];
	for (size_t i = 0; i < 4; i++)
		names[i] = vss[i].streetName;
	sort(names, names + 4);
	const string expected[4] = {
		"Coventry Street", "Picadilly", "Regent Street", "Shaftesbury Avenue"
	};
	assert(equal(names, names + 4, expected));
}
cout << "SegmentMapper PASSED" << endl;

cout << "About to test Navigator" << endl;
{
	Navigator nav;
	assert(nav.loadMapData("testmap.txt"));
	vector<NavSegment> directions;
	assert(nav.navigate("Eros Statue", "Hamleys Toy Store", directions) == NAV_SUCCESS);
	assert(directions.size() == 6);
	struct ExpectedItem
	{
		NavSegment::NavCommand command;
		string direction;
		double distance;
		string streetName;
	};
	const ExpectedItem expected[6] = {
		{ NavSegment::PROCEED, "northwest", 0.0138, "Picadilly" },
		{ NavSegment::TURN, "left", 0, "" },
		{ NavSegment::PROCEED, "west", 0.0119, "Regent Street" },
		{ NavSegment::PROCEED, "west", 0.0845, "Regent Street" },
		{ NavSegment::PROCEED, "west", 0.0696, "Regent Street" },
		{ NavSegment::PROCEED, "northwest", 0.1871, "Regent Street" },
	};
	for (size_t i = 0; i < 6; i++)
	{
		const NavSegment& ns = directions[i];
		const ExpectedItem& exp = expected[i];
		if (ns.m_command == NavSegment::PROCEED)
			cout << "PROCEED" << endl;
		if (ns.m_command == NavSegment::TURN)
			cout << "TURN" << endl;
		cout << ns.m_direction << endl;
		cout << ns.m_streetName << endl;
		assert(ns.m_command == exp.command); // FAILED
		assert(ns.m_direction == exp.direction);  // FAILED
		if (ns.m_command == NavSegment::PROCEED)
		{
			assert(abs(ns.m_distance - exp.distance) < 0.00051);
			assert(ns.m_streetName == exp.streetName);
		}
	}
}
cout << "Navigator PASSED" << endl;
}


