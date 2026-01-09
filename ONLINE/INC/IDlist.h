#define MAX_SVIDLIST       600
#define MAX_CEIDLIST       600

// struct SVID_LIST
// {
//     UINT			m_svidno;
// 	char			m_svname[101];
// 	int				m_type;
// 	int				m_size;
// 	int				m_mode;
// 	char			m_unit[11];
//
// 	typedef enum    {N_EXPORT,SV_TYPE,DV_TYPE} VarTyp;
// 	VarTyp			m_bVarType;
// };

struct RPT_LIST {
	UINT				m_rptno;
	UINT* svid;
	UINT				svidcnt;
	struct RPT_LIST* m_rptnext;
};

// struct CEID_LIST
// {
// 	BOOL           m_reportflag;
// 	unsigned int   m_ceid;
// 	char           m_cename[101];
// 	UINT*          m_rptid;
// 	int            m_rptidcurrentcnt;
// };
//
// struct ALRM_LIST
// {
//
// };