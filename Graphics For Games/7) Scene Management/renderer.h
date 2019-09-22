# pragma once

 #include "../nclgl/OGLRenderer.h"
 #include "../nclgl/camera.h"
 #include "../nclgl/scenenode.h"
 #include"../nclgl/frustum.h"
 #include "../6) Scene Graph/cuberobot.h"
 #include <algorithm> // For std :: sort ...

 class Renderer : public OGLRenderer {
	 public:
		 Renderer(Window & parent);
		 virtual ~Renderer(void);
		 virtual void UpdateScene(float msec);
		  virtual void RenderScene();
		 
			  protected:
				  void BuildNodeLists(SceneNode * from);
				  void SortNodeLists();
				  void ClearNodeLists();
				  void DrawNodes();
				  void DrawNode(SceneNode * n);
				 
					  SceneNode * root;
				  Camera * camera;
				  Mesh * quad;
				 
					  Frustum frameFrustum;
				 
					  vector < SceneNode * > transparentNodeList;
				  vector < SceneNode * > nodeList;
				 
 };