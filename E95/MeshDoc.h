// MeshDoc.h: interface for the CMeshDoc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHDOC_H__268197B6_7143_462C_9822_42E7EF6C7E7E__INCLUDED_)
#define AFX_MESHDOC_H__268197B6_7143_462C_9822_42E7EF6C7E7E__INCLUDED_

#include "..\Lib3D\Base3d.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SXFrm
{
public:
	float m_xTranslation,m_yTranslation,m_zTranslation;
	float m_xRotation,m_yRotation,m_zRotation;
	float m_xScaling,m_yScaling,m_zScaling;
	SXFrm() {
		m_xTranslation=m_yTranslation=m_zTranslation=0;
		m_xRotation=m_yRotation=m_zRotation=0;
		m_xScaling=m_yScaling=m_zScaling=0;
	}
	void Set(float xT, float yT, float zT,
		     float xR, float yR, float zR,
			 float xS, float yS, float zS)
			 {
		m_xTranslation=xT;
		m_yTranslation=yT;
		m_zTranslation=zT;
		m_xRotation=xR;
		m_yRotation=yR;
		m_zRotation=zR;
		m_xScaling=xS;
		m_yScaling=yS;
		m_zScaling=zS;
			 }
};

class CMeshDoc  
{
public:
	// The scene
	CSceneGraph3d m_SceneGraph;
	
	// Options
	BOOL m_AddWireframe;
	BOOL m_Smooth;
	float m_PolygonOffset;
	BOOL m_Light;
	GLenum m_Mode;
	
	void RenderScene(SXFrm &XFm);

public:
	void DrawColorChart();
	void GenerateScene();
	std::vector<CPoint3D> *  GetDataPtr();
	void GenData();
	void MeshColorHeight();
	void GlSmooth();
	void MeshLoop();
	BOOL OpenVRMLFile(LPCTSTR lpszPathName);
	CMeshDoc();
	virtual ~CMeshDoc();

};

#endif // !defined(AFX_MESHDOC_H__268197B6_7143_462C_9822_42E7EF6C7E7E__INCLUDED_)
