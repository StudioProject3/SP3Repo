
#include "GameObject.h"

GameObject::GameObject(GAMEOBJECT_TYPE typeValue) 
	: type(typeValue),
	scale(1, 1, 1),
	pos(0, 0, 0),
	direction(0,0,0),
	active(false),
	animation(NULL),
	ParticleMesh(NULL)
{
}

GameObject::~GameObject()
{
}