#include "provided.h"
#include "MyMap.h"
#include <string>
#include <vector>
using namespace std;

class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
private:
	// geo coords of start of segment, geo coords of end of segments, geo coords of attractions
	// and their respective segments
	MyMap<string, StreetSegment> m_segmentsStart; 
	MyMap<string, StreetSegment> m_segmentsEnd;
	MyMap<string, StreetSegment> m_segmentsRepeats1;
	MyMap<string, StreetSegment> m_segmentsRepeats2;
	MyMap<string, StreetSegment> m_segmentsRepeats3;
	MyMap<string, StreetSegment> m_attractions;
};

SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

void SegmentMapperImpl::init(const MapLoader& ml) // O((N + A)*log(N + A))
{
	
	StreetSegment s;
	for (int i = 0; i < ml.getNumSegments(); i++)
	{
		ml.getSegment(i, s);
		
		// associate geosegment with segment (which incudes start and end)
		if (m_segmentsStart.find(s.segment.start.latitudeText + ',' + s.segment.start.longitudeText) == nullptr)
			m_segmentsStart.associate(s.segment.start.latitudeText + ',' + s.segment.start.longitudeText, s);
		else if (m_segmentsRepeats1.find(s.segment.start.latitudeText + ',' + s.segment.start.longitudeText) == nullptr)
			m_segmentsRepeats1.associate(s.segment.start.latitudeText + ',' + s.segment.start.longitudeText, s);
		else if (m_segmentsRepeats2.find(s.segment.start.latitudeText + ',' + s.segment.start.longitudeText) == nullptr)
			m_segmentsRepeats2.associate(s.segment.start.latitudeText + ',' + s.segment.start.longitudeText, s);
		else
			m_segmentsRepeats3.associate(s.segment.start.latitudeText + ',' + s.segment.start.longitudeText, s);

		if (m_segmentsEnd.find(s.segment.end.latitudeText + ',' + s.segment.end.longitudeText) == nullptr)
			m_segmentsEnd.associate(s.segment.end.latitudeText + ',' + s.segment.end.longitudeText, s); 
		else if (m_segmentsRepeats1.find(s.segment.end.latitudeText + ',' + s.segment.end.longitudeText) == nullptr)
			m_segmentsRepeats1.associate(s.segment.end.latitudeText + ',' + s.segment.end.longitudeText, s);
		else if (m_segmentsRepeats2.find(s.segment.end.latitudeText + ',' + s.segment.end.longitudeText) == nullptr)
			m_segmentsRepeats2.associate(s.segment.end.latitudeText + ',' + s.segment.end.longitudeText, s);
		else
			m_segmentsRepeats3.associate(s.segment.end.latitudeText + ',' + s.segment.end.longitudeText, s);

		for (int j = 0; j < s.attractions.size(); j++)  // associate attractions in segment with segment
		{
			m_attractions.associate(s.attractions[j].geocoordinates.latitudeText 
				+ ',' + s.attractions[j].geocoordinates.longitudeText, s);
		}
	}
	
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const //O(log(N+A)) 
{
	
	vector<StreetSegment> segments;

	// NEED TO SOMEHOW GET THE ENDS AND STARTS IN THE VECTOR 

	if (m_attractions.find(gc.latitudeText + ',' + gc.longitudeText) != nullptr)
		segments.push_back(*(m_attractions.find(gc.latitudeText + ',' + gc.longitudeText)));
		
	if (m_segmentsStart.find(gc.latitudeText + ',' + gc.longitudeText) != nullptr)
		segments.push_back(*(m_segmentsStart.find(gc.latitudeText + ',' + gc.longitudeText)));
	

	if (m_segmentsEnd.find(gc.latitudeText + ',' + gc.longitudeText) != nullptr)
		segments.push_back(*(m_segmentsEnd.find(gc.latitudeText + ',' + gc.longitudeText)));

	if (m_segmentsRepeats1.find(gc.latitudeText + ',' + gc.longitudeText) != nullptr)
		segments.push_back(*(m_segmentsRepeats1.find(gc.latitudeText + ',' + gc.longitudeText)));

	if (m_segmentsRepeats2.find(gc.latitudeText + ',' + gc.longitudeText) != nullptr)
		segments.push_back(*(m_segmentsRepeats2.find(gc.latitudeText + ',' + gc.longitudeText)));

	if (m_segmentsRepeats3.find(gc.latitudeText + ',' + gc.longitudeText) != nullptr)
		segments.push_back(*(m_segmentsRepeats3.find(gc.latitudeText + ',' + gc.longitudeText)));

	return segments;  // This compiles, but may not be correct
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
