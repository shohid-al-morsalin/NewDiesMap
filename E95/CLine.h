#pragma once

class CRecipe;

class CLine : public CObject {
public:

	CLine& operator=(CLine& co);

	void Serialize(CArchive& ar);

	CLine();

	~CLine();

	short npoint;
	float angle;
private:
	short magic;
};
