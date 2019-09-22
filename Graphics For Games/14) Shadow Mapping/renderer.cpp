#include "renderer.h"
Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	camera = new Camera(0.0f, 0.0f, Vector3(
		RAW_WIDTH * HEIGHTMAP_X / 2.0f, 500, RAW_HEIGHT * HEIGHTMAP_Z));

	heightMap = new HeightMap(TEXTUREDIR "terrain.raw");
	SceneShader = new Shader(SHADERDIR "shadowscenevert.glsl",
		SHADERDIR "shadowscenefrag.glsl");
	shadowShader = new Shader(SHADERDIR "shadowVert.glsl",
		SHADERDIR "shadowFrag.glsl");
	pointlightShader = new Shader(SHADERDIR "pointlightvert.glsl",
		SHADERDIR "pointlightfrag.glsl");
	if (!pointlightShader->LinkProgram()) {
		return;

	}
	heightMap->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR "Barren Reds.jpg", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR "Barren RedsDOT3.jpg", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
#ifdef MD5_USE_HARDWARE_SKINNING
	currentShader = new Shader("skeletonvertex.glsl", SHADERDIR"TexturedFragment.glsl");
#else
	HellShader = new Shader(SHADERDIR"shadowscenevert.glsl", SHADERDIR"shadowscenefrag.glsl");
#endif
	hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");

	hellNode = new MD5Node(*hellData);

	light.push_back(new Light(Vector3(3000.0f, 1250.0f, 3800.0f),
		Vector4(1,1,1, 1), 5500.0f));
	light.push_back(new Light(Vector3(0.0f, 1250.0f, 0.0f),
		Vector4(1,1, 1, 1), 5500.0f));
	light.push_back(new Light(Vector3(0.0f, 1250.0f, 3800.0f),
		Vector4(1, 1, 1, 1), 5500.0f));
	lightLookAts.push_back(Vector3(0, 0, 0));
	lightLookAts.push_back(Vector3(3000.0f, 0, 3800.0f));
	lightLookAts.push_back(Vector3(3000.0f, 0, 0.0f));
	if (!HellShader->LinkProgram()) {
		return;
	}

	hellData->AddAnim(MESHDIR"idle2.md5anim");
	hellNode->PlayAnim(MESHDIR"idle2.md5anim");
	if (!SceneShader->LinkProgram() || !shadowShader->LinkProgram() ||
		!heightMap->GetTexture() || !heightMap->GetBumpMap()) {
		return;

	}

	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
		GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);
	glGenFramebuffers(1, &shadowFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	hellNode->SetTransform(Matrix4::Translation(Vector3(2000, 500, 2000)));
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);
	camera->SetPosition(Vector3(0, 30, 175));
/*
	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);

	GLenum buffers[2];
	buffers[0] = GL_COLOR_ATTACHMENT0;
	buffers[1] = GL_COLOR_ATTACHMENT1;

	// Generate our scene depth texture ...
	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightEmissiveTex);
	GenerateScreenTexture(lightSpecularTex);
	// And now attach them to our FBOs
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE) {
		return;

	}

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, lightEmissiveTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE) {
		return;



	}*/
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	init = true;
}

void Renderer::GenerateScreenTexture(GLuint & into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0,
		depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8,
		width, height, 0,
		depth ? GL_DEPTH_COMPONENT : GL_RGBA,
		GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

}


Renderer ::~Renderer(void) {
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
	delete camera;
	delete heightMap;
	for (Light * l : light)
		delete l;
	light.clear();
	delete hellData;
	delete hellNode;
	delete SceneShader;
	delete HellShader;
	delete shadowShader;
	currentShader = NULL;
}



void Renderer::FillBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(SceneShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"bumpTex"), 1);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);
	modelMatrix.ToIdentity();
	UpdateShaderMatrices();

	heightMap->Draw();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


void Renderer::UpdateScene(float msec) {
	/*	lightAngle = lightAngle + 0.0005f*msec;
	float distanceLight = distance(light->GetPosition(), Vector3(HEIGHTMAP_X * 100, HEIGHTMAP_Y, HEIGHTMAP_Z * 100))
	float lightZpos = distanceLight * cos(lightAngle);
	float lightYpos = distanceLight * sin(lightAngle);
	light->SetPosition(Vector3(light->GetPosition().x, lightYpos, lightZpos));*/
	camera->UpdateCamera(msec);

	viewMatrix = camera->BuildViewMatrix();
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_H))
		hellNode->SetTransform(Matrix4::Translation(Vector3(hellNode->GetTransform().GetPositionVector().x + 1, hellNode->GetTransform().GetPositionVector().y, hellNode->GetTransform().GetPositionVector().z)));
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_G))
		hellNode->SetTransform(Matrix4::Translation(Vector3(hellNode->GetTransform().GetPositionVector().x, hellNode->GetTransform().GetPositionVector().y, hellNode->GetTransform().GetPositionVector().z + 1)));
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_N))
		hellNode->SetTransform(Matrix4::Translation(Vector3(hellNode->GetTransform().GetPositionVector().x - 1, hellNode->GetTransform().GetPositionVector().y, hellNode->GetTransform().GetPositionVector().z)));
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_M))
		hellNode->SetTransform(Matrix4::Translation(Vector3(hellNode->GetTransform().GetPositionVector().x, hellNode->GetTransform().GetPositionVector().y, hellNode->GetTransform().GetPositionVector().z - 1)));
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_I))
		hellNode->SetTransform(Matrix4::Translation(Vector3(hellNode->GetTransform().GetPositionVector().x, hellNode->GetTransform().GetPositionVector().y + 1, hellNode->GetTransform().GetPositionVector().z)));
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_O))
		hellNode->SetTransform(Matrix4::Translation(Vector3(hellNode->GetTransform().GetPositionVector().x, hellNode->GetTransform().GetPositionVector().y - 1, hellNode->GetTransform().GetPositionVector().z)));

	hellNode->Update(msec);

}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
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
	SwapBuffers();
}

void Renderer::DrawShadowScene(int i) {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	SetCurrentShader(shadowShader);
	viewMatrix = Matrix4::BuildViewMatrix(
		light[i]->GetPosition(), lightLookAts[i]);
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

void Renderer::DrawHellKnight()
{

	modelMatrix.ToIdentity();
	Matrix4 tempMatrix = textureMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "textureMatrix"), 1, false, *& tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "modelMatrix"), 1, false, *& hellNode->GetTransform().values);
	modelMatrix = Matrix4::Translation(Vector3(0, 0, 0));
	hellNode->Draw(*this);
}

void Renderer::drawMap()
{

	Matrix4 tempMatrix = textureMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "textureMatrix"), 1, false, *& tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "modelMatrix"), 1, false, *& modelMatrix.values);
	heightMap->Draw();
}

float Renderer::distance(Vector3 x, Vector3 y)
{
	return sqrtf((x.x - y.x)*(x.x - y.x) + (x.y - y.y)*(x.y - y.y) + (x.z - y.z)*(x.z - y.z));
}

void Renderer::DrawCombinedScene(int i) {
	SetCurrentShader(SceneShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"shadowTex"), 2);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& camera->GetPosition());

	SetShaderLight(*light[i]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	if(i==0)
		glDepthFunc(GL_LEQUAL);
	else if(i ==1)
		glDepthFunc(GL_LEQUAL);
	else if(i==2)
		glDepthFunc(GL_EQUAL);
	viewMatrix = camera->BuildViewMatrix();
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_J))
		viewMatrix = Matrix4::BuildViewMatrix(light[i]->GetPosition(), Vector3(0, 0, 0));
	UpdateShaderMatrices();

	drawMap();
	DrawHellKnight();

	glUseProgram(0);

}