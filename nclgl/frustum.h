# pragma once

 #include "plane.h"
 #include "Matrix4.h"
 #include "scenenode.h"
 class Matrix4; // Compile the Mat4 class first , please !

 class Frustum {
	 public:
		 Frustum(void) {};
		 ~Frustum(void) {};
		
			 void FromMatrix(const Matrix4 & mvp);
		 bool InsideFrustum(SceneNode & n);
		 protected:
			 Plane planes[6];
			
};