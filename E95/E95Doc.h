// E95Doc.h : interface of the CE95Doc class
//

#pragma once

class CE95Doc : public CDocument {
protected: // create from serialization only
	CE95Doc();
	DECLARE_DYNCREATE(CE95Doc)

	// Attributes
public:

	// Operations
public:

	// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

	// Implementation
public:
	virtual ~CE95Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
