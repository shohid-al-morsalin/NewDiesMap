#pragma once
class CTaikoRingMeasProp
{
public:
	enum SearchType { LINEAR, CIRCULAR};

	double radius;
	double angle;
	double ZPos;
	double fromAngle;
	double toAngle;
	double distanceFromCenter;
	double offsetAngle;
	double waferRadius;
	// taiko tolerence
	float fMinWidth;
	float fMaxWidth;
	BOOL bSearchAround;	//remeasure
	int nRepeat;
	float fStepDistance;
	SearchType eSearchType;
	short npoints;
	int magic;
	float RI;
	float Exposure;

	double bevelWidth;
	double ringWidth;
	double RingSTD;
	double RingTTV;
	double RingThk;
	double RingStepHeight;
	double RingStepWidth;
	double RingStepWidthSTD;
	double RingStepWidthTTV;

	CTaikoRingMeasProp();
	~CTaikoRingMeasProp();
	void Serialize(CArchive& ar);
	void Clear();
	CTaikoRingMeasProp& operator=(CTaikoRingMeasProp& co);
};

