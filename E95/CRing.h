#pragma once

class CRing : public CObject {
public:

	CRing& operator=(CRing& co);

	void Serialize(CArchive& ar);

	CRing();

	~CRing();

	short npoint;
	float angle;
	float radius;
	float radius2;  // 0 indicate circular pattern and not segment [7/24/2010 C2C]

private:
	short magic;
};
