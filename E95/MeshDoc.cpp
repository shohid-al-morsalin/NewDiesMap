// MeshDoc.cpp: implementation of the CMeshDoc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MeshDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern GLubyte cr[26],cg[26],cb[26];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeshDoc::CMeshDoc()
{
	m_AddWireframe = 0;
	m_Smooth = 1;
	m_PolygonOffset = -1.0f;
	m_Light = TRUE;
	m_Mode = GL_FILL;
}

CMeshDoc::~CMeshDoc()
{

}

//***********************************************
// RenderScene
//***********************************************
void CMeshDoc::RenderScene(SXFrm &XFm)
{
	// Main drawing
	glPolygonMode(GL_FRONT_AND_BACK,m_Mode);
	if(m_Light) {
		::glEnable(GL_LIGHTING);
	}
	else {
		::glDisable(GL_LIGHTING);
	}

	DrawColorChart();

	// Position / translation / scale
	glTranslated(XFm.m_xTranslation,XFm.m_yTranslation,XFm.m_zTranslation);
	glRotatef(XFm.m_xRotation, 1.0, 0.0, 0.0);
	glRotatef(XFm.m_yRotation, 0.0, 1.0, 0.0);
	glRotatef(XFm.m_zRotation, 0.0, 0.0, 1.0);
	glScalef(XFm.m_xScaling,XFm.m_yScaling,XFm.m_zScaling);
	
	m_SceneGraph.glDraw();
	
	// Add wire frame (no light, and line mode)
	if(m_AddWireframe)
	{
		// Set state
		::glDisable(GL_LIGHTING);
		::glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		::glEnable(GL_POLYGON_OFFSET_LINE);
		::glPolygonOffset(m_PolygonOffset,-1.0f);
		
		// Draw again...
		m_SceneGraph.glDraw(TYPE_MESH3D);
		
		// Restore light and mode
		::glDisable(GL_POLYGON_OFFSET_LINE);
		::glEnable(GL_LIGHTING);
	}
	
}


BOOL CMeshDoc::OpenVRMLFile(LPCTSTR lpszPathName)
{
	// Extension-based checking
	CString file = lpszPathName;
	
	// Extension
	CString extension = lpszPathName;
	extension = extension.Right(4);
	extension.MakeLower();
	
	// Path "c:\path\file.wrl" -> c:\path
	CString path = lpszPathName;
	path = path.Left(path.ReverseFind('\\'));
	
	// CDocument
	SetCurrentDirectory(path);
	
//	TRACE("\nOpening document\n");
// 	TRACE("File      : %s\n",lpszPathName);
// 	TRACE("Path      : %s\n",path);
// 	TRACE("Extension : %s\n",extension);
	
	// Start reading VRML file
	if(extension == ".wrl")
	{
//		TRACE("wrl type\n");
		// Parser VRML 2.0
		CParserVrml parser;
		if(parser.Run((char *)lpszPathName,&m_SceneGraph))
		{
			m_SceneGraph.BuildAdjacency();
			m_SceneGraph.CalculateNormalPerFace();
			m_SceneGraph.CalculateNormalPerVertex();
			return TRUE;
		}
	}
	
	return TRUE;
}

void CMeshDoc::MeshLoop()
{
//	BeginWaitCursor();
	int NbObject = m_SceneGraph.NbObject();
	for(int i=0;i<NbObject;i++)
	{
		CObject3d *pObject3d = m_SceneGraph[i];
		if(pObject3d->GetType() == TYPE_MESH3D)
		{
			CMesh3d *pMesh  = (CMesh3d *)pObject3d;
			pMesh->SubdivisionLoop();
		}
	}
//	EndWaitCursor();
}

void CMeshDoc::GlSmooth()
{
	m_Smooth = !m_Smooth;
	if(m_Smooth)
	{
		m_SceneGraph.SetNormalBinding(NORMAL_PER_VERTEX);
		::glShadeModel(GL_SMOOTH);
		m_SceneGraph.BuildAdjacency();
	}
	else
	{
		m_SceneGraph.SetNormalBinding(NORMAL_PER_FACE);
		::glShadeModel(GL_FLAT);
		m_SceneGraph.BuildAdjacency();
	}
}

void CMeshDoc::MeshColorHeight()
{
	// Rainbow height ramp
	CColorRamp ramp;
	ramp.BuildRainbow();
	((CMesh3d *)m_SceneGraph.GetAt(0))->ColorHeight(&ramp);
}

void CMeshDoc::GenData()
{
	m_SceneGraph.FakeData();
	m_SceneGraph.Interpolate();
//	m_SceneGraph.BuildAdjacency();
	m_SceneGraph.CalculateNormalPerFace();
	m_SceneGraph.CalculateNormalPerVertex();
}

std::vector<CPoint3D> * CMeshDoc::GetDataPtr()
{
	return m_SceneGraph.GetDataPtr();
}

void CMeshDoc::GenerateScene()
{
	m_SceneGraph.Interpolate();
	m_SceneGraph.CalculateNormalPerFace();
	m_SceneGraph.CalculateNormalPerVertex();

//	MeshLoop ();
	MeshColorHeight();

	m_Light = FALSE;
	m_Mode = GL_FILL;

}

#define BMPHEIGHT	768
#define BMPWIDTH	1024

void CMeshDoc::DrawColorChart()
{
	float dh = float(2*BMPHEIGHT / 5.0);
	
	glBegin (GL_QUADS);
	glColor3ub(255,0,0);
	glVertex3f(BMPWIDTH-64, BMPHEIGHT, 0);
	glVertex3f(BMPWIDTH-120, BMPHEIGHT, 0);
	glColor3ub(255,255,0);
	glVertex3f(BMPWIDTH-120, BMPHEIGHT-dh, 0);
	glVertex3f(BMPWIDTH-64, BMPHEIGHT-dh, 0);
	
	glColor3ub(255,255,0);
	glVertex3f(BMPWIDTH-64, BMPHEIGHT-dh, 0);
	glVertex3f(BMPWIDTH-120, BMPHEIGHT-dh, 0);
	glColor3ub(0,255,0);
	glVertex3f(BMPWIDTH-120, BMPHEIGHT-2*dh, 0);
	glVertex3f(BMPWIDTH-64, BMPHEIGHT-2*dh, 0);
	
	glColor3ub(0,255,0);
	glVertex3f(BMPWIDTH-64, BMPHEIGHT-2*dh, 0);
	glVertex3f(BMPWIDTH-120, BMPHEIGHT-2*dh, 0);
	glColor3ub(0,255,255);
	glVertex3f(BMPWIDTH-120, BMPHEIGHT-3*dh, 0);
	glVertex3f(BMPWIDTH-64, BMPHEIGHT-3*dh, 0);
	
	glColor3ub(0,255,255);
	glVertex3f(BMPWIDTH-64, BMPHEIGHT-3*dh, 0);
	glVertex3f(BMPWIDTH-120, BMPHEIGHT-3*dh, 0);
	glColor3ub(0,0,255);
	glVertex3f(BMPWIDTH-120, BMPHEIGHT-4*dh, 0);
	glVertex3f(BMPWIDTH-64, BMPHEIGHT-4*dh, 0);
	
	glColor3ub(0,0,255);
	glVertex3f(BMPWIDTH-64, BMPHEIGHT-4*dh, 0);
	glVertex3f(BMPWIDTH-120, BMPHEIGHT-4*dh, 0);
	glColor3ub(255,0,255);
	glVertex3f(BMPWIDTH-120, BMPHEIGHT-5*dh, 0);
	glVertex3f(BMPWIDTH-64, BMPHEIGHT-5*dh, 0);
	
	glEnd();
}
