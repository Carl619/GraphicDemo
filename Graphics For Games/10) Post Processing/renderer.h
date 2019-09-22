# pragma once

 #include "../nclgl/OGLRenderer.h"
 #include "../nclgl/HeightMap.h"
 #include "../nclgl/camera.h"
#include <vector>
 #define POST_PASSES 10

 class Renderer : public OGLRenderer {
	 public:
		 Renderer(Window & parent);
		 virtual ~Renderer(void);
		
			 virtual void RenderScene();
		 virtual void UpdateScene(float msec);
		
			 protected:
				 void PresentScene(int i);
				 void DrawPostProcess(int i);
				 void DrawScene(int i);
				
					 Shader * sceneShader;
				 Shader * processShader;
				 std::vector<Camera *> camera;
				 std::vector<Mesh *> quad;
				 HeightMap * heightMap;
				
				 std::vector<GLuint> bufferFBO;
				 std::vector< GLuint> processFBO;
				 GLuint bufferColourTex[4][2];
				 std::vector<GLuint> bufferDepthTex;
				 int numberCameras = 4;
				 int currentCamera = 0;
				
};
