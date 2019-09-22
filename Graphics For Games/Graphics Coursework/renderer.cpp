#include "renderer.h"
Renderer::Renderer(Window & parent) : OGLRenderer(parent) {

	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);
	FontShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	//Scene 1 Light Scene
	scenes.push_back(new Scene());
	scenes[0]->addCamera(new Camera(0.0f, 0.0f, Vector3(
		RAW_WIDTH * HEIGHTMAP_X / 2.0f, 500, RAW_HEIGHT * HEIGHTMAP_Z)));
	scenes[0]->setHeightMap( new HeightMap(TEXTUREDIR "terrain.raw"));
	scenes[0]->setSceneShader(new Shader(SHADERDIR "BumpVertex.glsl",
		SHADERDIR "BumpFragment.glsl"));
	scenes[0]->getHeightMap()->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR "Barren Reds.jpg", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	scenes[0]->getHeightMap()->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR "Barren RedsDOT3.jpg", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	scenes[0]->sethellData(new MD5FileData(MESHDIR"hellknight.md5mesh"));

	scenes[0]->sethellNode( new MD5Node(*scenes[0]->gethellData()));
	scenes[0]->addLight(new Light(Vector3(3000.0f, 1250.0f, 3800.0f),
		Vector4(3, 3, 3, 3), 5500.0f));
	scenes[0]->addlightsLookAts(Vector3(0, 0, 0));
	scenes[0]->gethellData()->AddAnim(MESHDIR"idle2.md5anim");
	scenes[0]->gethellNode()->PlayAnim(MESHDIR"idle2.md5anim");
	if (!scenes[0]->getSceneShader()->LinkProgram() || !FontShader->LinkProgram()||
		!scenes[0]->getHeightMap()->GetTexture() || !scenes[0]->getHeightMap()->GetBumpMap()) {
		return;

	}

	SetTextureRepeating(scenes[0]->getHeightMap()->GetTexture(), true);
	SetTextureRepeating(scenes[0]->getHeightMap()->GetBumpMap(), true);


	scenes[0]->gethellNode()->SetTransform(Matrix4::Translation(Vector3(2000, 300, 2000)));
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);
	scenes[0]->getCamera(0)->SetPosition(Vector3(0, 30, 175));

	//Scene 2 Shadow Scene
	scenes.push_back(new Scene());
	scenes[1]->addCamera(new Camera(0.0f, 0.0f, Vector3(
		RAW_WIDTH * HEIGHTMAP_X / 2.0f, 500, RAW_HEIGHT * HEIGHTMAP_Z)));
	scenes[1]->setHeightMap(new HeightMap(TEXTUREDIR "terrain.raw"));
	scenes[1]->setSceneShader(new Shader(SHADERDIR "shadowscenevert.glsl",
		SHADERDIR "shadowscenefrag.glsl"));
	scenes[1]->getHeightMap()->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR "Barren Reds.jpg", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	scenes[1]->getHeightMap()->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR "Barren RedsDOT3.jpg", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	scenes[1]->setshadowShader( new Shader(SHADERDIR "shadowVert.glsl",
		SHADERDIR "shadowFrag.glsl"));
	scenes[1]->sethellData(new MD5FileData(MESHDIR"hellknight.md5mesh"));

	scenes[1]->sethellNode(new MD5Node(*scenes[1]->gethellData()));
	scenes[1]->addLight(new Light(Vector3(3000.0f, 1250.0f, 3800.0f),
		Vector4(1, 1, 1, 1), 5500.0f));
	scenes[1]->addlightsLookAts(Vector3(0, 0, 0));
	scenes[1]->gethellData()->AddAnim(MESHDIR"idle2.md5anim");
	scenes[1]->gethellNode()->PlayAnim(MESHDIR"idle2.md5anim");
	if (!scenes[1]->getSceneShader()->LinkProgram() || !scenes[1]->getshadowShader()->LinkProgram() ||
		!scenes[1]->getHeightMap()->GetTexture() || !scenes[1]->getHeightMap()->GetBumpMap()) {
		return;

	}

	SetTextureRepeating(scenes[1]->getHeightMap()->GetTexture(), true);
	SetTextureRepeating(scenes[1]->getHeightMap()->GetBumpMap(), true);
	scenes[1]->addshadowTex(new GLuint());
	glGenTextures(1, scenes[1]->getshadowTex(0));
	glBindTexture(GL_TEXTURE_2D, *scenes[1]->getshadowTex(0));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
		GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);
	scenes[1]->addshadowFBO(new GLuint());
	glGenFramebuffers(1, scenes[1]->getshadowFBO(0));

	glBindFramebuffer(GL_FRAMEBUFFER, *scenes[1]->getshadowFBO(0));
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, *scenes[1]->getshadowTex(0), 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	scenes[1]->gethellNode()->SetTransform(Matrix4::Translation(Vector3(2000, 300, 2000)));
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);
	scenes[1]->getCamera(0)->SetPosition(Vector3(0, 30, 175));


	//Scene 3
	scenes.push_back(new Scene());
	scenes[2]->addCamera(new Camera(0.0f, 0.0f, Vector3(
		RAW_WIDTH * HEIGHTMAP_X / 2.0f, 500, RAW_HEIGHT * HEIGHTMAP_Z)));
	scenes[2]->setHeightMap(new HeightMap(TEXTUREDIR "terrain.raw"));
	scenes[2]->addquad(Mesh::GenerateQuad());
	scenes[2]->addLight(new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f,
		(RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)),
		Vector4(0.9f, 0.9f, 1.0f, 1),
		(RAW_WIDTH * HEIGHTMAP_X) / 2.0f));

	scenes[2]->setreflectShader(new Shader(SHADERDIR "PerPixelVertex.glsl",
		SHADERDIR "reflectFragment.glsl"));
	scenes[2]->setskyboxShader( new Shader(SHADERDIR "skyboxVertex.glsl",
		SHADERDIR "skyboxFragment.glsl"));
	scenes[2]->setLightShader(new Shader(SHADERDIR "PerPixelVertex.glsl",
		SHADERDIR "PerPixelFragment.glsl"));
	scenes[2]->addlightsLookAts(Vector3(0, 0, 0));
	if (!scenes[2]->getreflectShader()->LinkProgram() || !scenes[2]->getLightShader()->LinkProgram() ||
		!scenes[2]->getskyboxShader()->LinkProgram()) {
		return;

	}
	scenes[2]->getquad(0)->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "water.TGA",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	scenes[2]->getHeightMap()->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	scenes[2]->getHeightMap()->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	scenes[2]->setcubeMap( SOIL_load_OGL_cubemap(
		TEXTUREDIR "rusted_west.jpg", TEXTUREDIR "rusted_east.jpg",
		TEXTUREDIR "rusted_up.jpg", TEXTUREDIR "rusted_down.jpg",
		TEXTUREDIR "rusted_south.jpg", TEXTUREDIR "rusted_north.jpg",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	));

	
	if (!scenes[2]->getcubeMap() || !scenes[2]->getquad(0)->GetTexture() || !scenes[2]->getHeightMap()->GetTexture() ||
		!scenes[2]->getHeightMap()->GetBumpMap()) {
		return;

	}

	SetTextureRepeating(scenes[2]->getquad(0)->GetTexture(), true);
	SetTextureRepeating(scenes[2]->getHeightMap()->GetTexture(), true);
	SetTextureRepeating(scenes[2]->getHeightMap()->GetBumpMap(), true);
	scenes[2]->setwaterRotate(0.0f);

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);
	//scene 4
	scenes.push_back(new Scene());
	for (int i = 0;i < 2;i++) {
		scenes[3]->addCamera(new Camera(0.0f, 135.0f, Vector3(50 * i, 500, 0)));
		scenes[3]->addquad(Mesh::GenerateQuad());
	}

	scenes[3]->setHeightMap(new HeightMap(TEXTUREDIR "terrain.raw"));
	scenes[3]->getHeightMap()->SetTexture(
		SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.jpg",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	scenes[3]->getHeightMap()->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR "Barren RedsDOT3.jpg", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	scenes[3]->setSceneShader(new Shader(SHADERDIR "shadowscenevert.glsl",
		SHADERDIR "shadowscenefrag.glsl"));
	scenes[3]->setProcessShader(new Shader(SHADERDIR "processVertex.glsl",
		SHADERDIR "processfrag.glsl"));
	scenes[3]->setshadowShader(new Shader(SHADERDIR "shadowVert.glsl",
		SHADERDIR "shadowFrag.glsl"));

	scenes[3]->sethellData(new MD5FileData(MESHDIR"hellknight.md5mesh"));

	scenes[3]->sethellNode(new MD5Node(*scenes[3]->gethellData()));

	scenes[3]->gethellData()->AddAnim(MESHDIR"idle2.md5anim");
	scenes[3]->gethellNode()->PlayAnim(MESHDIR"idle2.md5anim");
	scenes[3]->addLight(new Light(Vector3(3000.0f, 1250.0f, 3800.0f),
		Vector4(1, 1, 1, 1), 5500.0f));
	scenes[3]->addLight(new Light(Vector3(0.0f, 1250.0f, 0.0f),
		Vector4(1, 1, 1, 1), 5500.0f));
	scenes[3]->addLight(new Light(Vector3(0.0f, 1250.0f, 3800.0f),
		Vector4(1, 1, 1, 1), 5500.0f));
	scenes[3]->addlightsLookAts(Vector3(0, 0, 0));
	scenes[3]->addlightsLookAts(Vector3(3000.0f, 0, 3800.0f));
	scenes[3]->addlightsLookAts(Vector3(3000.0f, 0, 0.0f));
	if (!scenes[3]->getSceneShader()->LinkProgram() || !scenes[3]->getProcessShader()->LinkProgram() || !FontShader->LinkProgram() ||
		!scenes[3]->getshadowShader()->LinkProgram() || !scenes[3]->getHeightMap()->GetTexture() || !scenes[3]->getHeightMap()->GetBumpMap()) {
		return;

	}


	SetTextureRepeating(scenes[3]->getHeightMap()->GetTexture(), true);
	SetTextureRepeating(scenes[3]->getHeightMap()->GetBumpMap(), true);
	for (int j = 0; j < 2;j++) {
		scenes[3]->addbufferDepthTex(new GLuint());
		scenes[3]->addbufferFBO(new GLuint());
		scenes[3]->addprocessFBO(new GLuint());
		// Generate our scene depth texture ...
		glGenTextures(1, scenes[3]->getbufferDepthTex(j));
		glBindTexture(GL_TEXTURE_2D, *scenes[3]->getbufferDepthTex(j));
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height,
			0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

		
		// And our colour texture ...
		for (int i = 0; i < 2; ++i) {
			glGenTextures(1, scenes[3]->getbufferColourTex(j, i));
			glBindTexture(GL_TEXTURE_2D, *scenes[3]->getbufferColourTex(j, i));
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		}

		glGenFramebuffers(1, scenes[3]->getbufferFBO(j)); // We ’ll render the scene into this
		glGenFramebuffers(1, scenes[3]->getprocessFBO(j)); // And do post processing in this

		glBindFramebuffer(GL_FRAMEBUFFER, *scenes[3]->getbufferFBO(j));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D, *scenes[3]->getbufferDepthTex(j), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
			GL_TEXTURE_2D, *scenes[3]->getbufferDepthTex(j), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, *scenes[3]->getbufferColourTex(j, 0), 0);
		scenes[3]->addshadowTex(new GLuint());
		glGenTextures(1, scenes[3]->getshadowTex(j));
		glBindTexture(GL_TEXTURE_2D, *scenes[3]->getshadowTex(j));
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
			GL_COMPARE_R_TO_TEXTURE);

		glBindTexture(GL_TEXTURE_2D, 0);
		scenes[3]->addshadowFBO(new GLuint());
		glGenFramebuffers(1, scenes[3]->getshadowFBO(j));

		glBindFramebuffer(GL_FRAMEBUFFER, *scenes[3]->getshadowFBO(j));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D, *scenes[3]->getshadowTex(j), 0);
		glDrawBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// We can check FBO attachment success using this command !
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
			GL_FRAMEBUFFER_COMPLETE || !scenes[3]->getbufferDepthTex(j) ||!scenes[3]->getshadowTex(j) || !scenes[3]->getbufferColourTex(j, 0)) {
			return;

		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	scenes[3]->gethellNode()->SetTransform(Matrix4::Translation(Vector3(2000, 500, 2000)));
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);

	//scene 5
	scenes.push_back(new Scene());
	for (int i = 0;i < 2;i++) {
		scenes[4]->addCamera(new Camera(0.0f, 135.0f, Vector3(50 * i, 500, 0)));
		scenes[4]->addquad(Mesh::GenerateQuad());
	}

	scenes[4]->setHeightMap(new HeightMap(TEXTUREDIR "terrain.raw"));
	scenes[4]->getHeightMap()->SetTexture(
		SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.jpg",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	scenes[4]->setSceneShader(new Shader(SHADERDIR "TexturedVertex.glsl",
		SHADERDIR "TexturedFragment.glsl"));
	scenes[4]->setProcessShader(new Shader(SHADERDIR "processVertex.glsl",
		SHADERDIR "processfrag.glsl"));

	if (!scenes[4]->getSceneShader()->LinkProgram() || !scenes[4]->getProcessShader()->LinkProgram() ||
		!scenes[4]->getHeightMap()->GetTexture()) {
		return;
	}

	SetTextureRepeating(scenes[4]->getHeightMap()->GetTexture(), true);
	for (int j = 0; j < 2;j++) {
		scenes[4]->addbufferDepthTex(new GLuint());
		scenes[4]->addbufferFBO(new GLuint());
		scenes[4]->addprocessFBO(new GLuint());
		// Generate our scene depth texture ...
		glGenTextures(1, scenes[4]->getbufferDepthTex(j));
		glBindTexture(GL_TEXTURE_2D, *scenes[4]->getbufferDepthTex(j));
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height,
			0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);


		// And our colour texture ...
		for (int i = 0; i < 2; ++i) {
			glGenTextures(1, scenes[4]->getbufferColourTex(j, i));
			glBindTexture(GL_TEXTURE_2D, *scenes[4]->getbufferColourTex(j, i));
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		}

		glGenFramebuffers(1, scenes[4]->getbufferFBO(j)); // We ’ll render the scene into this
		glGenFramebuffers(1, scenes[4]->getprocessFBO(j)); // And do post processing in this

		glBindFramebuffer(GL_FRAMEBUFFER, *scenes[4]->getbufferFBO(j));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D, *scenes[4]->getbufferDepthTex(j), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
			GL_TEXTURE_2D, *scenes[4]->getbufferDepthTex(j), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, *scenes[4]->getbufferColourTex(j, 0), 0);

		// We can check FBO attachment success using this command !
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
			GL_FRAMEBUFFER_COMPLETE || !scenes[4]->getbufferDepthTex(j) || !scenes[4]->getbufferColourTex(j, 0)) {
			return;
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);
	//fin

	glEnable(GL_DEPTH_TEST);
	currentScene = 0;
	if (currentScene != 2) {
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}
	else {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}
	if(currentScene!=2)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	if (currentScene == 3) {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
	}
	if (currentScene == 4) {
		glEnable(GL_DEPTH_TEST);
	}
	init = true;
}


Renderer ::~Renderer(void) {
	for (Scene *sc : scenes)
		delete sc;
	scenes.clear();
	delete basicFont;
	delete FontShader;
}

void Renderer::UpdateScene(float msec) {
	//if (currentScene == 0)
		//scenes[currentScene]->getLight(0)->SetPosition(Vector3(scenes[currentScene]->getLight(0)->GetPosition().x-0.5f*msec, scenes[currentScene]->getLight(0)->GetPosition().y, scenes[currentScene]->getLight(0)->GetPosition().z));
	scenes[currentScene]->getCamera(scenes[currentScene]->getcurrentCamera())->UpdateCamera(msec);
	viewMatrix = scenes[currentScene]->getCamera(scenes[currentScene]->getcurrentCamera())->BuildViewMatrix();

	//HellKnight controller
	if (scenes[currentScene]->gethellNode() != NULL) {
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_H))
			scenes[currentScene]->gethellNode()->SetTransform(Matrix4::Translation(Vector3(scenes[currentScene]->gethellNode()->GetTransform().GetPositionVector().x + 1, scenes[currentScene]->gethellNode()->GetTransform().GetPositionVector().y, scenes[currentScene]->gethellNode()->GetTransform().GetPositionVector().z)));
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_G))
			scenes[currentScene]->gethellNode()->SetTransform(Matrix4::Translation(Vector3(scenes[currentScene]->gethellNode()->GetTransform().GetPositionVector().x, scenes[currentScene]->gethellNode()->GetTransform().GetPositionVector().y, scenes[currentScene]->gethellNode()->GetTransform().GetPositionVector().z + 1)));
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_N))
			scenes[currentScene]->gethellNode()->SetTransform(Matrix4::Translation(Vector3(scenes[currentScene]->gethellNode()->GetTransform().GetPositionVector().x - 1, scenes[currentScene]->gethellNode()->GetTransform().GetPositionVector().y, scenes[currentScene]->gethellNode()->GetTransform().GetPositionVector().z)));
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_M))
			scenes[currentScene]->gethellNode()->SetTransform(Matrix4::Translation(Vector3(scenes[currentScene]->gethellNode()->GetTransform().GetPositionVector().x, scenes[currentScene]->gethellNode()->GetTransform().GetPositionVector().y, scenes[currentScene]->gethellNode()->GetTransform().GetPositionVector().z - 1)));
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_I))
			scenes[currentScene]->gethellNode()->SetTransform(Matrix4::Translation(Vector3(scenes[currentScene]->gethellNode()->GetTransform().GetPositionVector().x, scenes[currentScene]->gethellNode()->GetTransform().GetPositionVector().y + 1, scenes[currentScene]->gethellNode()->GetTransform().GetPositionVector().z)));
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_O))
			scenes[currentScene]->gethellNode()->SetTransform(Matrix4::Translation(Vector3(scenes[currentScene]->gethellNode()->GetTransform().GetPositionVector().x, scenes[currentScene]->gethellNode()->GetTransform().GetPositionVector().y - 1, scenes[currentScene]->gethellNode()->GetTransform().GetPositionVector().z)));
		scenes[currentScene]->gethellNode()->Update(msec);
	}
	//updating enables
	if (currentScene != 2)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	if (currentScene != 2) {
		glDisable(GL_BLEND);
	}
	else {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}
	if(currentScene==0)
		glDisable(GL_CULL_FACE);
	if (currentScene == 3) {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
	}
	if (currentScene == 4) {
		glEnable(GL_DEPTH_TEST);
	}
	//rotation of water
	if(scenes[currentScene]->getwaterRotate()>=0)
		 scenes[currentScene]->setwaterRotate(scenes[currentScene]->getwaterRotate()+ msec / 1000.0f);
	if (paused) {
		currentpausedTime += msec;
		if (TotalPausedTime < currentpausedTime) {
			currentScene++;
			currentpausedTime = 0;
			if (currentScene >= NUMBERSCENES)
				currentScene = 0;
			restartMatrices();
		}
	}
	totalMsec += msec;
	currentFPS++;
	if (totalMsec > 1000)
	{
		
		FPS = currentFPS;
		totalMsec = 0;
		currentFPS = 0;
	}
}




void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	if (currentScene == 0) {

			drawMap();
			DrawHellKnight();
			DrawText("FPS: " + std::to_string(FPS), Vector3(0, 0, 0), 16.0f);
			glUseProgram(0);

	}
	else if (currentScene == 1) {

		DrawShadowScene(0); // First render pass ...
		DrawCombinedScene(0); // Second render pass ...
		DrawText("FPS: " + std::to_string(FPS), Vector3(0, 0, 0), 16.0f);
		
	}
	else if (currentScene == 2) {

		DrawSkybox();
		drawMap();
		DrawWater();
		DrawText("FPS: " + std::to_string(FPS), Vector3(0, 0, 0), 16.0f);
	}
	else if (currentScene == 3) {
		glDisable(GL_BLEND);
		glDepthFunc(GL_LESS);
		DrawShadowScene(0); // First render pass ...
		DrawCombinedScene(0); // Second render pass ...
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		DrawShadowScene(1); // First render pass ...
		DrawCombinedScene(1); // Second render pass ...
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		DrawShadowScene(2); // First render pass ...
		DrawCombinedScene(2); // Second render pass ...
		glDisable(GL_BLEND);
//		DrawText("FPS: " + std::to_string(FPS), Vector3(0, 0, 0), 16.0f);
	}
	else if (currentScene == 4) {
		DrawScene(scenes[currentScene]->getcurrentCamera());
		if(blurring)
		DrawPostProcess(scenes[currentScene]->getcurrentCamera());
		scenes[currentScene]->setCurrentCamera(scenes[currentScene]->getcurrentCamera()+1);
		if (scenes[currentScene]->getcurrentCamera() == 2) {
			PresentScene(scenes[currentScene]->getcurrentCamera());
			scenes[currentScene]->setCurrentCamera(0);
		//	DrawText("FPS: " + std::to_string(FPS), Vector3(0, 0, 0), 16.0f);
			SwapBuffers();
			
		}
	}
	if(currentScene!=4)
	SwapBuffers();
	AlreadyDrawedFPS = false;
}

void Renderer::DrawScene(int i) {
	glBindFramebuffer(GL_FRAMEBUFFER, *scenes[currentScene]->getbufferFBO(i));
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT |
		GL_STENCIL_BUFFER_BIT);

	SetCurrentShader(scenes[currentScene]->getSceneShader());
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);
	UpdateShaderMatrices();

	scenes[currentScene]->getHeightMap()->Draw();
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::DrawPostProcess(int i) {
	glBindFramebuffer(GL_FRAMEBUFFER, *scenes[currentScene]->getprocessFBO(i));
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,* scenes[currentScene]->getbufferColourTex(i,1), 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(scenes[currentScene]->getProcessShader());
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glUniform2f(glGetUniformLocation(currentShader->GetProgram(),
		"pixelSize"), 1.0f / width, 1.0f / height);

	for (int j = 0; j < POST_PASSES; ++j) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D,* scenes[currentScene]->getbufferColourTex(i, 1), 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			"isVertical"), 0);

		scenes[currentScene]->getquad(i)->SetTexture(*scenes[currentScene]->getbufferColourTex(i, 0));
		scenes[currentScene]->getquad(i)->Draw();
		// Now to swap the colour buffers , and do the second blur pass
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			"isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, *scenes[currentScene]->getbufferColourTex(i, 0), 0);

		scenes[currentScene]->getquad(i)->SetTexture(*scenes[currentScene]->getbufferColourTex(i, 1));
		scenes[currentScene]->getquad(i)->Draw();

	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);

}

void Renderer::PresentScene(int i) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(scenes[currentScene]->getSceneShader());
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	//quad[i]->SetTexture(bufferColourTex[i][0]);
	//quad[i]->Draw();

	glViewport(0, 0, width * 0.5, height);
	scenes[currentScene]->getquad(0)->SetTexture(*scenes[currentScene]->getbufferColourTex(0, 0));
	scenes[currentScene]->getquad(0)->Draw();
	glViewport(width * 0.5, 0, width * 0.5, height);
	scenes[currentScene]->getquad(1)->SetTexture(*scenes[currentScene]->getbufferColourTex(1, 0));
	scenes[currentScene]->getquad(1)->Draw();
	glViewport(0, 0, width, height);
	glUseProgram(0);

}

void Renderer::DrawShadowScene(int i) {
	glBindFramebuffer(GL_FRAMEBUFFER, *scenes[currentScene]->getshadowFBO(scenes[currentScene]->getcurrentCamera()));

	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	SetCurrentShader(scenes[currentScene]->getshadowShader());
	viewMatrix = Matrix4::BuildViewMatrix(
		scenes[currentScene]->getLight(i)->GetPosition(), scenes[currentScene]->getlightsLookAts(i));
	textureMatrix = biasMatrix * (projMatrix * viewMatrix);

	UpdateShaderMatrices();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	drawMap();
	DrawHellKnight();
	glCullFace(GL_BACK);
	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::DrawNode(SceneNode * n) {
	if (n->GetMesh()) {
		Matrix4 transform = n->GetWorldTransform() *
			Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(
			glGetUniformLocation(currentShader->GetProgram(),
				"modelMatrix"), 1, false, (float *)& transform);

		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(),
			"nodeColour"), 1, (float *)& n->GetColour());

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			"useTexture"), (int)n->GetMesh()->GetTexture());
		n->Draw(*this);

	}

	for (vector < SceneNode * >::const_iterator
		i = n->GetChildIteratorStart();
		i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);

	}

}

void Renderer::DrawHellKnight()
{
	if (currentScene == 0) {
		SetCurrentShader(scenes[currentScene]->getSceneShader());
		glUseProgram(currentShader->GetProgram());
		glUniformMatrix4fv(
			glGetUniformLocation(currentShader->GetProgram(),
				"modelMatrix"), 1, false, (float *)& scenes[currentScene]->gethellNode()->GetTransform());
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			"bumpTex"), 1);
		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
			"cameraPos"), 1, (float *)&scenes[currentScene]->getCamera(scenes[currentScene]->getcurrentCamera())->GetPosition());
	}
	else{
		modelMatrix.ToIdentity();
		Matrix4 tempMatrix = textureMatrix * modelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
			, "textureMatrix"), 1, false, *& tempMatrix.values);
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
			, "modelMatrix"), 1, false, *& scenes[currentScene]->gethellNode()->GetTransform().values);
		modelMatrix = Matrix4::Translation(Vector3(0, 0, 0));
	}
	scenes[currentScene]->gethellNode()->Draw(*this);
}

void Renderer::drawMap()
{
	if (currentScene == 0) {
		SetCurrentShader(scenes[currentScene]->getSceneShader());
		glUseProgram(currentShader->GetProgram());
		UpdateShaderMatrices();
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			"diffuseTex"), 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			"bumpTex"), 1);

		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
			"cameraPos"), 1, (float *)&scenes[currentScene]->getCamera(0)->GetPosition());
		modelMatrix.ToIdentity();
		UpdateShaderMatrices();
		SetShaderLight(*scenes[currentScene]->getLight(0));
	}
	else if (currentScene == 1) {
	Matrix4 tempMatrix = textureMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "textureMatrix"), 1, false, *& tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "modelMatrix"), 1, false, *& modelMatrix.values);

}
	else if (currentScene == 2) {
	SetCurrentShader(scenes[currentScene]->getLightShader());
	glUseProgram(currentShader->GetProgram());
	SetShaderLight(*scenes[currentScene]->getLight(0));
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"bumpTex"), 1);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float *)&scenes[currentScene]->getCamera(0)->GetPosition());


	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();
}
	else if (currentScene == 3) {
		Matrix4 tempMatrix = textureMatrix * modelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
			, "textureMatrix"), 1, false, *& tempMatrix.values);
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
			, "modelMatrix"), 1, false, *& modelMatrix.values);
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
			(float)width / (float)height, 45.0f);
		UpdateShaderMatrices();
	}
	scenes[currentScene]->getHeightMap()->Draw();
	if(currentScene==2)
		glUseProgram(0);
}

float Renderer::distance(Vector3 x, Vector3 y)
{
	return sqrtf((x.x - y.x)*(x.x - y.x) + (x.y - y.y)*(x.y - y.y) + (x.z - y.z)*(x.z - y.z));
}


void Renderer::DrawCombinedScene(int i) {
	SetCurrentShader(scenes[currentScene]->getSceneShader());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"shadowTex"), 2);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& scenes[currentScene]->getCamera(scenes[currentScene]->getcurrentCamera())->GetPosition());

	SetShaderLight(*scenes[currentScene]->getLight(i));

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, *scenes[currentScene]->getshadowTex(scenes[currentScene]->getcurrentCamera()));
	if (currentScene == 3) {
		if (i == 0)
			glDepthFunc(GL_LEQUAL);
		else if (i == 1)
			glDepthFunc(GL_LEQUAL);
		else if (i == 2)
			glDepthFunc(GL_EQUAL);
	}
	viewMatrix = scenes[currentScene]->getCamera(scenes[currentScene]->getcurrentCamera())->BuildViewMatrix();
	UpdateShaderMatrices();

	drawMap();
	DrawHellKnight();
/*	if (!AlreadyDrawedFPS) {
		DrawText("FPS: " + std::to_string(FPS), Vector3(0, 0, 0), 16.0f);
		*/
	glUseProgram(0);

}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	SetCurrentShader(scenes[currentScene]->getskyboxShader());

	UpdateShaderMatrices();
	scenes[currentScene]->getquad(0)->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);

}

void Renderer::ChangeScene()
{
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_6)) {
		blurring = !blurring;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P)) {
		paused = !paused;
		currentpausedTime = 0.0f;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_LEFT)) {
		currentScene--;
		
		if (currentScene < 0)
			currentScene = NUMBERSCENES-1;
		restartMatrices();
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_RIGHT)) {
		currentScene++;
		
		if (currentScene >= NUMBERSCENES)
			currentScene = 0;
		restartMatrices();
	}
}


void Renderer::DrawWater() {
	SetCurrentShader(scenes[currentScene]->getreflectShader());
	SetShaderLight(*scenes[currentScene]->getLight(0));
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& scenes[currentScene]->getCamera(0)->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, scenes[currentScene]->getcubeMap());

	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f);

	float heightY = 256 * HEIGHTMAP_Y / 3.0f;

	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);

	modelMatrix =
		Matrix4::Translation(Vector3(heightX, heightY, heightZ)) *
		Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
		Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
		Matrix4::Rotation(scenes[currentScene]->getwaterRotate(), Vector3(0.0f, 0.0f, 1.0f));
	UpdateShaderMatrices();

	scenes[currentScene]->getquad(0)->Draw();

	glUseProgram(0);

}


/*
Draw a line of text on screen. If we were to have a 'static' line of text, we'd
probably want to keep the TextMesh around to save processing it every frame,
but for a simple demonstration, this is fine...
*/
void Renderer::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective) {
	//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text, *basicFont);

	SetCurrentShader(FontShader);
	
	glUseProgram(currentShader->GetProgram());	//Enable the shader...
												//And turn on texture unit 0
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	textureMatrix.ToIdentity();
	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	if (perspective) {
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix = scenes[currentScene]->getCamera(scenes[currentScene]->getcurrentCamera())->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	}
	else {
		//In ortho mode, we subtract the y from the height, so that a height of 0
		//is at the top left of the screen, which is more intuitive
		//(for me anyway...)
		modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	}
	//Either way, we update the matrices, and draw the mesh
	UpdateShaderMatrices();
	mesh->Draw();
//	glDisable(GL_BLEND);
	textureMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	viewMatrix = scenes[currentScene]->getCamera(scenes[currentScene]->getcurrentCamera())->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);
	delete mesh; //Once it's drawn, we don't need it anymore!
}

void Renderer::restartMatrices()
{
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);
	textureMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	viewMatrix = scenes[currentScene]->getCamera(scenes[currentScene]->getcurrentCamera())->BuildViewMatrix();
	glDepthFunc(GL_LESS);
}