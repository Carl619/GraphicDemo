#include "../../nclgl/window.h"
#include "Renderer.h"

#pragma comment(lib, "nclgl.lib")

int main() {	
	Window w("Post Processing!", 800,600,false);
	if(!w.HasInitialised()) {
		return -1;
	}
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		float fps = w.GetTimer()->GetTimedMS();
		for (int i = 0;i < 4;i++) {
			renderer.UpdateScene(fps);
			renderer.RenderScene();
		}
	}

	return 0;
}