#pragma once
class CDosName {
public:
	char filename[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

public:
	CDosName(void);
	~CDosName(void);
};
