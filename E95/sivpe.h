#ifndef __SIVPE_H__
#define __SIVPE_H__

#include <sidefs.h>

#include <dshow.h>
#include <initguid.h>

#ifdef DIRECT3D_VERSION
#undef DIRECT3D_VERSION
#define DIRECT3D_VERSION 0x0900
#endif

#include <d3d9.h>
//#include <vmr9.h>

#undef SIAPI
#if defined (SIVPE_EXPORTS)
	#define SIAPI __declspec(dllexport)
#elif defined (SIIPL_SLIB)
	#define SIAPI extern
#else
	#define SIAPI __declspec(dllimport)
#endif

#define WM_SI_DSGRAPHNOTIFY  (WM_USER + 0x100)

typedef SIResult (* SI_FUNC_D3D_DRAW)(IDirect3DDevice9 * pD3DDev);
typedef SIResult (* SI_FUNC_FETCH_SAMPLE) (BYTE * pBits);

typedef struct tagSIAVIMetaData
{
  SI32u DataValid;
  SI32f DefaultWhiteBalance[4];
	SI32f UserWhiteBalance[4];
	char  szLookFileName[40];
}SIAVIMedaData;

class SIAPI CSIGenericPlayer
{
public:
	typedef enum {Init, Stopped, Paused, Running} PLAY_STATE_t;
public:
	PLAY_STATE_t get_current_state(){return m_PlayState;}
	virtual SIResult initialize() = 0;
	virtual SIResult uninitialize() = 0;
protected:
	 PLAY_STATE_t m_PlayState;
};


class SIAPI CSIDSMediaFilePlayer  : public CSIGenericPlayer
{
public:
	CSIDSMediaFilePlayer();
	~CSIDSMediaFilePlayer();

	SIResult initialize();
	SIResult uninitialize();

	SIResult open_clip(const char * lpszFile, HWND hWnd, int iResolution);
	SIResult close_clip();
	SIResult set_video_rect(const RECT * rect);

	SIResult stop();
	SIResult pause();
	SIResult play();
	SIResult step(SI32s FrameNum);
	SIResult seek(SI32u Frame);
	SIResult fast_forward();
	SIResult fast_backward();

	SI32u get_current_frame();
  SISize get_video_size();

	SIResult handle_graph_event();
	SIResult set_d3d_draw_callback(SI_FUNC_D3D_DRAW fp);
  SIResult set_fetch_sample_callback(SI_FUNC_FETCH_SAMPLE fp); 

//	typedef enum {Brightness = 0x1, Contrast=0x2, Hue = 0x4, Saturation = 0x8} VIDEO_SCREEN_ADJUST_t;
	SIResult repaint(HDC hDC);

	LONGLONG m_iFirstFrame, m_iLastFrame;
	SI32s m_iFrameRate;
  SIAVIMedaData m_SIAVIMetaData;
protected:
	class _DSMediaFilePlayerBase * m_pImpl;

	SI32u m_iFastPlayMode;
	SI64s m_iCurrentFrame;
	IPin * get_pin(IBaseFilter *pFilter, PIN_DIRECTION PinDir);
	void disable_fast_play(void);
	//void trace_current_pos(CString str);
};

#endif