#pragma once
#include "../nclgl/HeightMap.h"
#include "../nclgl/camera.h"
#include "../../nclgl/MD5Mesh.h"
#include "../../nclgl/MD5Node.h"
#include "../../nclgl/scenenode.h"

const int POST_PASSES = 10;
const int NUMBERCAMERAS = 2;
const int NUMBERBUFFERS = 2;
class Scene {
public:
	Scene() {
		hellNode = NULL;
		hellData = NULL;
		currentCamera = 0;
		 sceneShader = NULL;
		 processShader = NULL;
		shadowShader = NULL;
		lightShader = NULL;
		reflectShader = NULL;
		 skyboxShader = NULL;
		heightMap = NULL;
		root = NULL;
		waterRotate = -1.0f;
		for(int i=0;i< NUMBERCAMERAS;i++)
			for(int j=0;j< NUMBERBUFFERS;j++)
				bufferColourTex[i][j]= new GLuint();
	}
	~Scene() {
		if(hellNode)
			delete hellNode;
		for (Mesh* m : quad)
			delete m;
		for (Camera* c : cameras)
			delete c;
		for (Light* l : lights)
			delete l;
		hellData = NULL;
		sceneShader = NULL;
		processShader = NULL;
		shadowShader = NULL;
		lightShader = NULL;
		reflectShader = NULL;
		skyboxShader = NULL;
		lightsLookAts.clear();
		delete bufferColourTex[0][0];
		delete bufferColourTex[0][1];
		delete bufferColourTex[1][0];
		delete bufferColourTex[1][1];
		if (heightMap)
			delete heightMap;
	 }
	
	void setSceneShader(Shader * shader) { sceneShader = shader; };
	void setProcessShader(Shader * shader) { processShader = shader; };
	Shader * getSceneShader() { return sceneShader; };
	Shader * getProcessShader() {return processShader; };

	void addCamera(Camera * camera) {
		cameras.push_back(camera);
	}
	Camera * getCamera(int i) { return cameras[i]; };

	void addLight(Light * li) {
		lights.push_back(li);
	}
	Light * getLight(int i) { return lights[i]; };

	void addlightsLookAts(Vector3 li) {
		lightsLookAts.push_back(li);
	}
	Vector3 getlightsLookAts(int i) { return lightsLookAts[i]; };

	void addquad(Mesh * q) {
		quad.push_back(q);
	}
	
	Mesh * getquad(int i) { return quad[i]; };
	void setHeightMap(HeightMap * map) { heightMap = map; }
	HeightMap * getHeightMap() { return heightMap; }

	void addbufferFBO(GLuint* FBO) { bufferFBO.push_back(FBO); };
	void addprocessFBO(GLuint* FBO) { processFBO.push_back(FBO); };
	GLuint *getbufferFBO(int i) { return bufferFBO[i]; };
	GLuint *getprocessFBO(int i) { return processFBO[i]; };

	void setbufferColourTex(int i, int j, GLuint* g) { bufferColourTex[i][j] = g; }
	GLuint *getbufferColourTex(int i, int j) { return bufferColourTex[i][j]; }

	void addbufferDepthTex( GLuint* g) { bufferDepthTex.push_back(g); }
	GLuint *getbufferDepthTex(int i) { return bufferDepthTex[i]; }

	void setCurrentCamera(int i) { currentCamera = i; }
	int getcurrentCamera() { return currentCamera; }

	void sethellData(MD5FileData* i) { hellData = i; }
	MD5FileData* gethellData() { return hellData; }

	void sethellNode(MD5Node* i) { hellNode = i; }
	MD5Node* gethellNode() { return hellNode; }

	void setroot(SceneNode* i) { root = i; }
	SceneNode* getroot() { return root; }

	void addshadowTex( GLuint* g) { shadowTex.push_back(g); }
	GLuint *getshadowTex(int i) { return shadowTex[i]; }

	void addshadowFBO( GLuint* g) { shadowFBO.push_back(g); }
	GLuint *getshadowFBO(int i) { return shadowFBO[i]; }

	void setshadowShader(Shader * shader) { shadowShader = shader; };
	Shader * getshadowShader() { return shadowShader; };

	void setskyboxShader(Shader * shader) { skyboxShader = shader; };
	Shader * getskyboxShader() { return skyboxShader; };

	void setreflectShader(Shader * shader) { reflectShader = shader; };
	Shader * getreflectShader() { return reflectShader; };

	void setLightShader(Shader * shader) { lightShader = shader; };
	Shader * getLightShader() { return lightShader; };

	void setcubeMap(GLuint g) { cubeMap = g; }
	GLuint getcubeMap() { return cubeMap; }

	void setwaterRotate(float rot) { waterRotate = rot; };
	float getwaterRotate() { return waterRotate; };

protected:
	Shader * sceneShader;
	Shader * processShader;
	Shader * shadowShader;
	Shader * lightShader;
	Shader * reflectShader;
	Shader * skyboxShader;	
	HeightMap * heightMap;
	std::vector<Light *> lights;
	std::vector<Vector3> lightsLookAts;
	std::vector<Camera *> cameras;
	std::vector<Mesh *> quad;

	std::vector<GLuint*> bufferFBO;
	std::vector< GLuint*> processFBO;
	GLuint *bufferColourTex[NUMBERCAMERAS][NUMBERBUFFERS];
	std::vector<GLuint*> bufferDepthTex;
	int currentCamera = 0;
	MD5FileData*hellData;
	MD5Node*	hellNode;
	SceneNode * root;
	
	std::vector < GLuint*> shadowTex;
	std::vector < GLuint*> shadowFBO;



	GLuint cubeMap;
	float waterRotate;
};