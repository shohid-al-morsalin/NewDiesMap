#pragma once

#include "BaseException.h"

// PI value
const float PI = 3.14159f;

/** Exception class for CGrapher and its derivative classes
*
*	ex) You can throw CGrapherException object like this.
*
*	1. throw CGrapherException();	-> message would be "GrapherException"
*
*	2. throw CGrapherException(_T("Disaster")); -> message would be "Disaster"
*
*	3. throw CGrapherException(_T("Disaster"), LOCATION); -> message would be "Disaster SomeFile line no 11"
*/
class CGrapherException : public BaseException {
public:
	/** Default Constructor
	*	@param		None
	*	@exceptions	None
	*/
	CGrapherException() throw() : BaseException(_T("GrapherException")) {
	}
	/** Constructor with string description
	*	@param		message - This should be a useful message for error tracking.
	*	@exceptions	None
	*/
	CGrapherException(string message) throw() : BaseException(message) {
	}
	/** Constructor with string description and error location
	*	@param		message - This should be a useful message for error tracking.
	*	@param		location - Location where Exception has occrurred. You can use LOCATION macro here.
	*	@exceptions	None
	*/
	CGrapherException(string message, string location) throw() : BaseException(message, location) {
	}
};
