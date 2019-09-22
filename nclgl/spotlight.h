#pragma once
#include "light.h"

class SpotLight : public Light {
public:
	SpotLight(Vector3 position, Vector4 colour, float radius,float cutoff,Vector3 dir):Light(position,colour,radius) {
		this->CutOff = cutoff;
		Direction = dir;
	}
	SpotLight() {
	}
	~SpotLight(void) {};

	float GetCutOff() const { return CutOff; }
	void SetCutOff(float val) { CutOff = val; }
	Vector3 GetDirection() const { return Direction; }
	void SetDirection(Vector3 val) { Direction = val; }
protected:
	float CutOff;
	Vector3 Direction;
};