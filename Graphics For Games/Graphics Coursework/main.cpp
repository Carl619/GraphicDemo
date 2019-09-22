#include "../../nclgl/window.h"
#include "renderer.h"


#pragma comment(lib, "nclgl.lib")

int main() {
	Window w("Coursework!", 800, 600, true);
	if (!w.HasInitialised()) {
		return -1;
	}


	Renderer renderer(w); //This handles all the boring OGL 3.2 initialisation stuff, and sets up our tutorial!
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		float fps = w.GetTimer()->GetTimedMS();
			renderer.UpdateScene(fps);
			renderer.RenderScene();
			if(renderer.getCurrentScene()==4)
		for (int i = 0;i < 2;i++) {
			renderer.UpdateScene(fps);
			renderer.RenderScene();
		}
		renderer.ChangeScene();
	}
	return 0;
}