/////////////////////////////////////////////////////////////////////////////
// Tokenizer.h
//
// Date:        Monday, October 22, 2001
// Autor:       Eduardo Velasquez
// Description: Tokenizer class for CStrings. Works like strtok.
///////////////

#pragma once

#if !defined(_BITSET_)
#	include <bitset>
#endif // !defined(_BITSET_)

class CTokenizer {
public:
	CTokenizer(const CString& cs, const CString& csDelim);
	void SetDelimiters(const CString& csDelim);

	bool Next(CString& cs);
	CString	Tail() const;

private:
	CString m_cs;
	std::bitset<256> m_delim;
	int m_nCurPos;
};
