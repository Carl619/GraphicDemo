#include "renderer.h"
Mesh * Renderer::cubemesh = NULL;

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	camera = new Camera(0.0f, 0.0f, Vector3(
		 RAW_WIDTH * HEIGHTMAP_X / 2.0f, 500, RAW_HEIGHT * HEIGHTMAP_Z));
	quad = Mesh::GenerateQuad();
	m = new OBJMesh();
	m->LoadOBJMesh(MESHDIR "cube.obj");
	cubemesh = m;
		 heightMap = new HeightMap(TEXTUREDIR "terrain.raw");
		 mapshader = new Shader(SHADERDIR "BumpVertex.glsl",
		 SHADERDIR "BumpFragment.glsl");
		cubeshader = new Shader(SHADERDIR "BumpVertexRobot.glsl",
		 SHADERDIR "BumpFragmentRobot.glsl");
		HDRshader = new Shader(SHADERDIR "BumpBloomVertexRobot.glsl",
			SHADERDIR "BumpBloomFragmentRobot.glsl");
		processShader = new Shader(SHADERDIR "processVertex.glsl",
			SHADERDIR "processfrag.glsl");
		bloomfinalShader = new Shader(SHADERDIR "bloomfinalvertex.glsl",
			SHADERDIR "bloomfinalfrag.glsl");
	 heightMap->SetTexture(SOIL_load_OGL_texture(
		  TEXTUREDIR "Barren Reds.jpg", SOIL_LOAD_AUTO,
		  SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	 
		  heightMap->SetBumpMap(SOIL_load_OGL_texture(
			  TEXTUREDIR "Barren RedsDOT3.jpg", SOIL_LOAD_AUTO,
			  SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	  light = new Light(Vector3(2000,500,2000),
		   Vector4(1, 1, 1, 1), (RAW_WIDTH * HEIGHTMAP_X) / 2.0f);	

		  if (!processShader->LinkProgram() || !bloomfinalShader->LinkProgram() || !HDRshader->LinkProgram() || !mapshader->LinkProgram() || !cubeshader->LinkProgram()||
			  !heightMap->GetTexture() || !heightMap->GetBumpMap()) {
		  return;
		 
	 }
	  SetTextureRepeating(heightMap->GetTexture(), true);
	  SetTextureRepeating(heightMap->GetBumpMap(), true);

	  cube = new SceneNode(cubemesh, Vector4(5,5,5 , 1));
	  cube->SetModelScale(Vector3(50, 50, 50));
	  cube->SetTransform(Matrix4::Translation(Vector3(3000, 250, 2000)));
		   projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
			   (float)width / (float)height, 45.0f);
			   // Generate our scene depth texture ...
			   glGenTextures(1, &bufferDepthTex);
			   glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
			   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height,
				   0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

			   // And our colour texture ...
			   for (int i = 0; i < 2; ++i) {
				   glGenTextures(1, &bufferColourTex[i]);
				   glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
				   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

				   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
					   GL_RGBA, GL_UNSIGNED_BYTE, NULL);

			   }

/*			   glGenFramebuffers(1, &HDRFBO); // We ’ll render the scene into this
			   // Buffers for HDR
			   for (int i = 0; i < 2; ++i) {
				   glGenTextures(1, &HDRColourTex[i]);
				   glBindTexture(GL_TEXTURE_2D, HDRColourTex[i]);
				   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
					   GL_RGBA, GL_UNSIGNED_BYTE, NULL);

			   }*/

			   glGenFramebuffers(1, &bufferFBO); // We ’ll render the scene into this
			   glGenFramebuffers(1, &processFBO); // And do post processing in this

			   glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
			   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
				   GL_TEXTURE_2D, bufferDepthTex, 0);
			   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
				   GL_TEXTURE_2D, bufferDepthTex, 0);
				   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
					   GL_TEXTURE_2D, bufferColourTex[0], 0);
					  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
					   GL_TEXTURE_2D, bufferColourTex[1], 0);
			   unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
			   glDrawBuffers(2, attachments);
			   // We can check FBO attachment success using this command !
			   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
				   GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0] || !bufferColourTex[1]) {
				   return;

			   }
		   glBindFramebuffer(GL_FRAMEBUFFER, 0);

		   glGenFramebuffers(2, HDRFBO);
		   glGenTextures(2, bufferHDRTex);
		   for (unsigned int i = 0; i < 2; i++)
		   {
			  // glBindFramebuffer(GL_FRAMEBUFFER, HDRFBO[i]);

			   glBindTexture(GL_TEXTURE_2D, bufferHDRTex[i]);
			  
			   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
			   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_FLOAT, NULL);   //GL_RGB16F
			   //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferHDRTex[i], 0);
			   //// also check if framebuffers are complete (no need for depth buffer)
			   //if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				  // std::cout << "Framebuffer not complete!" << std::endl;
		   }
		   glBindFramebuffer(GL_FRAMEBUFFER, 0);
		   glEnable(GL_DEPTH_TEST);
		   glEnable(GL_CULL_FACE);
	   init = true;
}


Renderer ::~Renderer(void) {
	 delete camera;
	 delete heightMap;
	 delete light;
	
}

 void Renderer::UpdateScene(float msec) {
	 camera->UpdateCamera(msec);
	 viewMatrix = camera->BuildViewMatrix();
	 cube->Update(msec);
}

 void Renderer::RenderScene() {
	 glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	 glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	 glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	 projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		 (float)width / (float)height, 45.0f);
	 drawMap();
	 UpdateShaderMatrices();
	 SetShaderLight(*light);
	 DrawCubeBright(cube);
	 UpdateShaderMatrices();
	 SetShaderLight(*light);
	 DrawHDR();
	 PresentScene(0);
	 glUseProgram(0);
	 SwapBuffers();
 }

 void Renderer::drawMap()
 {
	 SetCurrentShader(mapshader);
	 glUseProgram(currentShader->GetProgram());
	 UpdateShaderMatrices();
	 glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		 "diffuseTex"), 0);
	 glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		 "bumpTex"), 1);

	 glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		 "cameraPos"), 1, (float *)&camera->GetPosition());

	 UpdateShaderMatrices();
	 SetShaderLight(*light);

	 heightMap->Draw();
 }

 void Renderer::DrawNode(SceneNode * n) {
	 if (n->GetMesh()) {
		 SetCurrentShader(cubeshader);
		 glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			 "diffuseTex"), 0);
		 glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			 "bumpTex"), 1);

		 glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
			 "cameraPos"), 1, (float *)&camera->GetPosition());
		 Matrix4 transform = n->GetWorldTransform() *
			 Matrix4::Scale(n->GetModelScale());


		 glUniform4fv(glGetUniformLocation(currentShader->GetProgram(),
			 "nodeColour"), 1, (float *)& n->GetColour());

		 glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			 "useTexture"), (int)n->GetMesh()->GetTexture());
		 UpdateShaderMatrices();
		 SetShaderLight(*light);		
		 
		 glUniformMatrix4fv(
			 glGetUniformLocation(currentShader->GetProgram(),
				 "modelMatrix"), 1, false, (float *)& transform);

		 n->Draw(*this);

	 }

	 for (vector < SceneNode * >::const_iterator
		 i = n->GetChildIteratorStart();
		 i != n->GetChildIteratorEnd(); ++i) {
		 DrawNode(*i);

	 }

 }

 void Renderer::DrawHDR() {
	 glClearColor(1.0f, 0, 0, 1.0f);
	 glBindFramebuffer(GL_FRAMEBUFFER, HDRFBO[0]);
	 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		 GL_TEXTURE_2D, bufferHDRTex[1], 0);
	 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		 GL_TEXTURE_2D, bufferHDRTex[0], 0);
	 if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		 std::cout << "Framebuffer not complete!" << std::endl;
	 glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	 glBindFramebuffer(GL_FRAMEBUFFER, 0);

	 return;




	 glDisable(GL_DEPTH_TEST);
	 glDisable(GL_CULL_FACE);
	 glUniform2f(glGetUniformLocation(currentShader->GetProgram(),
		 "pixelSize"), 1.0f / width, 1.0f / height);
	 bool first_iteration = true;
	 for (int j = 0; j < POST_PASSES; ++j) {
		 glBindFramebuffer(GL_FRAMEBUFFER, HDRFBO[0]);
		 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			 GL_TEXTURE_2D, bufferHDRTex[1], 0);
		 SetCurrentShader(processShader);
		 projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
		 viewMatrix.ToIdentity();
		 UpdateShaderMatrices();
		 glUniform2f(glGetUniformLocation(currentShader->GetProgram(),
			 "pixelSize"), 1.0f / width, 1.0f / height);
		 if (first_iteration) {
			 glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);
			 first_iteration = false;
			 glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		 }
		 else
			 glBindTexture(GL_TEXTURE_2D, bufferHDRTex[0]);
		 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			 GL_TEXTURE_2D, bufferHDRTex[1], 0);
		 glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			 "isVertical"), 0);

		 quad->SetTexture(bufferHDRTex[0]);
		 quad->Draw();
		 glBindFramebuffer(GL_FRAMEBUFFER, HDRFBO[1]);
		 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			 GL_TEXTURE_2D, bufferHDRTex[1], 0);
		 SetCurrentShader(processShader);
		 projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
		 viewMatrix.ToIdentity();
		 UpdateShaderMatrices();
		 glBindTexture(GL_TEXTURE_2D, bufferHDRTex[1]);
		 glUniform2f(glGetUniformLocation(currentShader->GetProgram(),
			 "pixelSize"), 1.0f / width, 1.0f / height);
		 // Now to swap the colour buffers , and do the second blur pass
		 glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			 "isVertical"), 1);
		 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			 GL_TEXTURE_2D, bufferHDRTex[0], 0);

		 quad->SetTexture(bufferHDRTex[1]);
		 quad->Draw();

	 }

	 glBindFramebuffer(GL_FRAMEBUFFER, 0);
	 glUseProgram(0);

	 glEnable(GL_DEPTH_TEST);
	 glEnable(GL_CULL_FACE);
	 }
 void Renderer::DrawBloom()
 {
 }
 void Renderer::DrawCubeBright(SceneNode * n)
 {
	 if (n->GetMesh()) {
		 SetCurrentShader(HDRshader);
		 glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			 "diffuseTex"), 0);
		 glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			 "bumpTex"), 1);

		 glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
			 "cameraPos"), 1, (float *)&camera->GetPosition());
		 Matrix4 transform = n->GetWorldTransform() *
			 Matrix4::Scale(n->GetModelScale());


		 glUniform4fv(glGetUniformLocation(currentShader->GetProgram(),
			 "nodeColour"), 1, (float *)& n->GetColour());

		 glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			 "useTexture"), (int)n->GetMesh()->GetTexture());
		 UpdateShaderMatrices();
		 SetShaderLight(*light);

		 glUniformMatrix4fv(
			 glGetUniformLocation(currentShader->GetProgram(),
				 "modelMatrix"), 1, false, (float *)& transform);

		 n->Draw(*this);

	 }

	 for (vector < SceneNode * >::const_iterator
		 i = n->GetChildIteratorStart();
		 i != n->GetChildIteratorEnd(); ++i) {
		 DrawCubeBright(*i);

	 }
 }
 void Renderer::DrawPostProcess(int i) {
	 glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		 GL_TEXTURE_2D, bufferColourTex[1], 0);
	 glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	 SetCurrentShader(processShader);
	 projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	 viewMatrix.ToIdentity();
	 UpdateShaderMatrices();

	 glDisable(GL_DEPTH_TEST);

	 glUniform2f(glGetUniformLocation(currentShader->GetProgram(),
		 "pixelSize"), 1.0f / width, 1.0f / height);

	 for (int j = 0; j < POST_PASSES; ++j) {
		 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			 GL_TEXTURE_2D, bufferColourTex[1], 0);
		 glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			 "isVertical"), 0);

		 quad->SetTexture(bufferColourTex[0]);
		 quad->Draw();
		 // Now to swap the colour buffers , and do the second blur pass
		 glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			 "isVertical"), 1);
		 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			 GL_TEXTURE_2D, bufferColourTex[0], 0);

		 quad->SetTexture(bufferColourTex[1]);
		 quad->Draw();

	 }

	 glBindFramebuffer(GL_FRAMEBUFFER, 0);
	 glUseProgram(0);

	 glEnable(GL_DEPTH_TEST);

 }

 void Renderer::PresentScene(int i) {
	/* glBindFramebuffer(GL_FRAMEBUFFER, 0);
	 glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	 SetCurrentShader(mapshader);
	 projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	 viewMatrix.ToIdentity();
	 UpdateShaderMatrices();
	 quad->SetTexture(bufferColourTex[0]);
	 quad->Draw();
	 */
	 glBindFramebuffer(GL_FRAMEBUFFER, 0);
	 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 SetCurrentShader(bloomfinalShader);
	 glActiveTexture(GL_TEXTURE5);
	 glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);

	// glBindTexture(GL_TEXTURE_2D, heightMap->GetTexture());
	 glActiveTexture(GL_TEXTURE6);
	 glBindTexture(GL_TEXTURE_2D, bufferHDRTex[0]);
	 glActiveTexture(GL_TEXTURE0);

	 glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		 "scene"), 5);
	 glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		 "bloomBlur"), 6);
	 glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		 "bloom"), true);
	 glUniform1f(glGetUniformLocation(currentShader->GetProgram(),
		 "exposure"), 1.0f);
	 projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	 viewMatrix.ToIdentity();
	 modelMatrix.ToIdentity();
	 UpdateShaderMatrices();
	 quad->SetTexture(bufferColourTex[0]);

	 glDisable(GL_DEPTH_TEST);
	 glDisable(GL_CULL_FACE);
	 quad->Draw();
	 glEnable(GL_DEPTH_TEST);
	 glEnable(GL_CULL_FACE);
	 glUseProgram(0);

 }