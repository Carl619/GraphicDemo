# pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/heightmap.h"
#include "../nclgl/scenenode.h"
#include "../../nclgl/MD5Mesh.h"
#include "../../nclgl/MD5Node.h"
#include "../../nclgl/light.h"
#include <cmath>
#define SHADOWSIZE 2048 // New !
#define LIGHTNUM 2 // We 'll generate LIGHTNUM squared lights ...

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);


protected:

	void DrawHellKnight();
	void drawMap();
	void FillBuffers(); //G- Buffer Fill Render Pass
	void DrawShadowScene(int i); // New !
	void DrawCombinedScene(int i); // New !
	void GenerateScreenTexture(GLuint &into, bool depth = false);

	float distance(Vector3 x, Vector3 y);
	Mesh * heightMap;
	Camera * camera;
	std::vector<Light *> light;
	std::vector<Vector3> lightLookAts;
	Shader * HellShader;
	Shader * ShaderRobot;
	Shader * pointlightShader;
	Shader * SceneShader;
	MD5FileData*hellData;
	MD5Node*	hellNode;
	Shader * shadowShader;
	GLuint shadowTex;
	GLuint shadowFBO;

	GLuint bufferFBO; // FBO for our G- Buffer pass
	GLuint bufferColourTex; // Albedo goes here
	GLuint bufferNormalTex; // Normals go here
	GLuint bufferDepthTex; // Depth goes here



	GLuint pointLightFBO; // FBO for our lighting pass
	GLuint lightEmissiveTex; // Store emissive lighting
	GLuint lightSpecularTex; // Store specular lighting

	float lightAngle = 0;
};
