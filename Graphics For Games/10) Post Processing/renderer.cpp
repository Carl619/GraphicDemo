# include "renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	for (int i = 0;i < 4;i++) {
		camera.push_back(new Camera(0.0f, 135.0f, Vector3(50 * i, 500, 0)));
		quad.push_back(Mesh::GenerateQuad());
	}

	heightMap = new HeightMap(TEXTUREDIR "terrain.raw");
	heightMap->SetTexture(
		SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.jpg",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	sceneShader = new Shader(SHADERDIR "TexturedVertex.glsl",
		SHADERDIR "TexturedFragment.glsl");
	processShader = new Shader(SHADERDIR "processVertex.glsl",
		SHADERDIR "processfrag.glsl");

	if (!processShader->LinkProgram() || !sceneShader->LinkProgram() ||
		!heightMap->GetTexture()) {
		return;

	}

	SetTextureRepeating(heightMap->GetTexture(), true);
	for (int j = 0; j < 4;j++) {
		bufferDepthTex.push_back(GLint());
		bufferFBO.push_back(GLint());
		processFBO.push_back(GLint());
		// Generate our scene depth texture ...
		glGenTextures(1, &bufferDepthTex[j]);
		glBindTexture(GL_TEXTURE_2D, bufferDepthTex[j]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height,
			0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

		// And our colour texture ...
		for (int i = 0; i < 2; ++i) {
			glGenTextures(1, &bufferColourTex[j][i]);
			glBindTexture(GL_TEXTURE_2D, bufferColourTex[j][i]);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		}

		glGenFramebuffers(1, &bufferFBO[j]); // We ’ll render the scene into this
		glGenFramebuffers(1, &processFBO[j]); // And do post processing in this

		glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO[j]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D, bufferDepthTex[j], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
			GL_TEXTURE_2D, bufferDepthTex[j], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, bufferColourTex[j][0], 0);
		// We can check FBO attachment success using this command !
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
			GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex[j] || !bufferColourTex[j][0]) {
			return;

		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glEnable(GL_DEPTH_TEST);
	init = true;
}
			Renderer ::~Renderer(void) {
				 delete sceneShader;
				 delete processShader;
				 currentShader = NULL;
				
					 delete heightMap;
				quad.clear();
				  camera.clear();
				 for (int i = 0; i < 4;i++) {
					 glDeleteTextures(2, bufferColourTex[i]);
					 glDeleteTextures(1, &bufferDepthTex[i]);
					 glDeleteFramebuffers(1, &bufferFBO[i]);
					 glDeleteFramebuffers(1, &processFBO[i]);
				 }
			}

			void Renderer::UpdateScene(float msec) {
					camera[currentCamera]->UpdateCamera(msec);
					viewMatrix = camera[currentCamera]->BuildViewMatrix();
			}
			
				 void Renderer::RenderScene() {
				 DrawScene(currentCamera);
				DrawPostProcess(currentCamera);

				 currentCamera++;
				 if (currentCamera == 4) {
					 PresentScene(currentCamera);
					 SwapBuffers();
					 currentCamera = 0;
				 }
			}

				 void Renderer::DrawScene(int i) {
					 glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO[i]);
					  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT |
						  GL_STENCIL_BUFFER_BIT);
					 
						  SetCurrentShader(sceneShader);
					  projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
						  (float)width / (float)height, 45.0f);
					  UpdateShaderMatrices();
					 
						  heightMap->Draw();
					 
						  glUseProgram(0);
					  glBindFramebuffer(GL_FRAMEBUFFER, 0);
					 
				 }

				 void Renderer::DrawPostProcess(int i) {
					  glBindFramebuffer(GL_FRAMEBUFFER, processFBO[i]);
					  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
						  GL_TEXTURE_2D, bufferColourTex[i][1], 0);
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
								   GL_TEXTURE_2D, bufferColourTex[i][1], 0);
							   glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
								   "isVertical"), 0);
							  
								   quad[i]->SetTexture(bufferColourTex[i][0]);
							   quad[i]->Draw();
							   // Now to swap the colour buffers , and do the second blur pass
								   glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
									   "isVertical"), 1);
							   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
								   GL_TEXTURE_2D, bufferColourTex[i][0], 0);
							  
								   quad[i]->SetTexture(bufferColourTex[i][1]);
							   quad[i]->Draw();
							  
						  }

						  glBindFramebuffer(GL_FRAMEBUFFER, 0);
						   glUseProgram(0);
						  
							   glEnable(GL_DEPTH_TEST);
						  
				 }

				 void Renderer::PresentScene(int i) {
					  glBindFramebuffer(GL_FRAMEBUFFER, 0);
					  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
					  SetCurrentShader(sceneShader);
					  projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
					  viewMatrix.ToIdentity();
					  UpdateShaderMatrices();

					  //quad[i]->SetTexture(bufferColourTex[i][0]);
					  //quad[i]->Draw();

					  glViewport(0, 0, width * 0.5, height);
					  quad[0]->SetTexture(bufferColourTex[0][0]);
					  quad[0]->Draw();
					  glViewport(width * 0.5, 0, width * 0.5, height);
					  quad[1]->SetTexture(bufferColourTex[1][0]);
					  quad[1]->Draw();
					  glViewport(0, 0, width, height);
					  glUseProgram(0);
					 
				 }