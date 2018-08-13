#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Vector3.h"
#include "SpriteAnimation.h"

struct GameObject
{
	enum GAMEOBJECT_TYPE
	{
		GO_NONE = 0,
		GO_TREE,
		GO_GRASS,
		GO_LEAF,
		GO_SMOKE,
		GO_F_LIGHTS,
		GO_RAIN,
		GO_RAINANIM,
		GO_TOTAL, //must be last
	};
	GAMEOBJECT_TYPE type;
	Vector3 pos;
	Vector3 scale;
	Vector3 vel;
	Vector3 direction;
	SpriteAnimation* animation;
	Mesh* ParticleMesh;

	bool active;
	float rotationY;
	float rotationX;
	float rotationZ;
	float camDist;
	float duration;

	GameObject(GAMEOBJECT_TYPE typeValue = GO_TREE);
	~GameObject();
};

#endif