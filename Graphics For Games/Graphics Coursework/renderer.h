# pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/heightmap.h"
#include "../nclgl/scenenode.h"
#include "../../nclgl/MD5Mesh.h"
#include "../../nclgl/MD5Node.h"
#include "TextMesh.h"
#include "scene.h"
#include <cmath>
#include <vector>

const int SHADOWSIZE = 2048; // New !
const int NUMBERSCENES = 5;

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);
	void ChangeScene();
	int getCurrentScene() { return currentScene; }

	/*
	Draws the passed in line of text, at the passed in position. Can have an optional font size, and
	whether to draw it in orthographic or perspective mode.
	*/
	void	DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);

protected:
	void DrawNode(SceneNode * n);
	void DrawHellKnight();
	void drawMap();
	void DrawShadowScene(int i); // New !
	void DrawCombinedScene(int i); // New !
	void DrawWater();
	void DrawSkybox();
	void PresentScene(int i);
	void DrawPostProcess(int i);
	void DrawScene(int i);
	void restartMatrices();
	
	float distance(Vector3 x, Vector3 y);
	std::vector<Scene*> scenes;
	float lightAngle = 0;
	int currentScene =0;
	Font*	basicFont;	//A font! a basic one...
	Shader*	FontShader;	//A font! a basic one...
	float totalMsec = 0;
	float FPS= 0;
	float currentFPS= 0;
	bool blurring = false;
	bool AlreadyDrawedFPS = false;
	bool paused = false;
	float currentpausedTime = 0.0f;
	float TotalPausedTime = 10000.0f;
};
