# pragma once

 #include "../nclgl/OGLRenderer.h"
 #include "../nclgl/Camera.h"
 #include "../nclgl/heightmap.h"
#include "..\nclgl\scenenode.h"
#include "..\nclgl\OBJMesh.h"
#include <cmath>
#define POST_PASSES 10

 class Renderer : public OGLRenderer {
	 public:
		Renderer(Window & parent);
		 virtual ~Renderer(void);
		
			 virtual void RenderScene();
		 virtual void UpdateScene(float msec);
			 protected:
				 void Renderer::drawMap();
				 void DrawNode(SceneNode * n);
				 void PresentScene(int i);
				 void DrawPostProcess(int i);
				 void DrawHDR();
				 void DrawBloom();
				 void DrawCubeBright(SceneNode * n);
				 static Mesh * cubemesh;
				 Camera * camera;
				 Light * light;
				 OBJMesh * m;
				 SceneNode * cube;
				 Shader * cubeshader;
				 Shader * mapshader;
				 Shader * bloomshader;
				 Shader * HDRshader;
				 Shader * processShader;
				 Shader * bloomfinalShader;
				 Mesh * quad;
				 HeightMap * heightMap;
				 GLuint bufferFBO;
				 GLuint processFBO;
				 GLuint bloomFBO;
				 GLuint HDRFBO[2];
				 GLuint HDRColourTex[2];
				 GLuint bufferHDRTex[2];
				 GLuint bufferColourTex[2];
				 GLuint bufferDepthTex;
};
