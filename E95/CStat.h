#pragma once

class CStat {
public:
	CStat();
	virtual ~CStat();
	CStat(const CStat& p) {
		this->m_fTTV = p.m_fTTV;
		this->m_fSTD = p.m_fSTD;
		this->m_fAverage = p.m_fAverage;
		this->m_fMin = p.m_fMin;
		this->m_fMax = p.m_fMax;
		this->m_fWarp = p.m_fWarp;
		this->m_fBow = p.m_fBow;
		this->m_fMaxDieTTV = p.m_fMaxDieTTV;
		this->m_fStress = p.m_fStress;
	}

	void Divide(float factor) {
		this->m_fTTV /= factor;
		this->m_fSTD /= factor;
		this->m_fAverage /= factor;
		this->m_fMin /= factor;
		this->m_fMax /= factor;
		this->m_fWarp /= factor;
		this->m_fBow /= factor;
		this->m_fMaxDieTTV /= factor;
		this->m_fStress /= factor;
	}

public:
	void Conv(char** msg);
	long Binary(char** msg);
	long GetLength();

	float m_fTTV;
	float m_fSTD;
	float m_fAverage;
	float m_fMin;
	float m_fMax;

	float m_fWarp;
	float m_fBow;
	float m_fMaxDieTTV;
	float m_fStress;
public:
	void Serialize(CArchive& ar);
	void Clear();
};
