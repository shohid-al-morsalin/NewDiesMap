#ifndef __PROCJOBINFO_H__
#define __PROCJOBINFO_H__

/**
 * Contains intermediate information of user input. Process job related parameters
 * This will be the content of a process job
 */
#include "..\128\CAS\NCas.h"

class CProcJobInfo {
public:
	CNCas Cas;
	CString CJobName;

	void Clear();

	CProcJobInfo(void);
	~CProcJobInfo(void);

	// Following are for convenient only
	//char SlotMap[32];  // extra bytes for security
};
#endif
