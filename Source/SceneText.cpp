#include "SceneText.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include "LoadTGA.h"
#include "LoadHmap.h"
#include <sstream>

SceneText::SceneText()
{
}

SceneText::~SceneText()
{
}

void SceneText::Init()
{
	// Black background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 
	
	glDisable(GL_CULL_FACE);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	//m_programID = LoadShaders( "Shader//Texture.vertexshader", "Shader//Text.fragmentshader" );
	m_programID = LoadShaders("Shader//Fog.vertexshader", "Shader//Fog.fragmentshader");
	
	// Get a handle for our uniform
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");
	m_parameters[U_LIGHT1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
	m_parameters[U_LIGHT1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
	m_parameters[U_LIGHT1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
	m_parameters[U_LIGHT1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
	m_parameters[U_LIGHT1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
	m_parameters[U_LIGHT1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
	m_parameters[U_LIGHT1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
	m_parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[1].spotDirection");
	m_parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutoff");
	m_parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
	m_parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponent");

	// Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled[0]");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture[0]");

	// Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED1] = glGetUniformLocation(m_programID, "colorTextureEnabled[1]");
	m_parameters[U_COLOR_TEXTURE1] = glGetUniformLocation(m_programID, "colorTexture[1]");

	// Get a handle for our "textColor" uniform
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");
	
	// Get a handle for Fog uniform
	m_parameters[U_FOG_COLOR] = glGetUniformLocation(m_programID, "fogParam.color");
	m_parameters[U_FOG_START] = glGetUniformLocation(m_programID, "fogParam.start");
	m_parameters[U_FOG_END] = glGetUniformLocation(m_programID, "fogParam.end");
	m_parameters[U_FOG_DENSITY] = glGetUniformLocation(m_programID, "fogParam.density");
	m_parameters[U_FOG_TYPE] = glGetUniformLocation(m_programID, "fogParam.type");
	m_parameters[U_FOG_ENABLED] = glGetUniformLocation(m_programID, "fogParam.enabled");

	// Use our shader
	glUseProgram(m_programID);

	lights[0].type = Light::LIGHT_DIRECTIONAL;
	lights[0].position.Set(0, 4000, 0);
	lights[0].color.Set(1, 1, 1, 1);
	lights[0].power = 1;
	lights[0].kC = 10.f;
	lights[0].kL = 0.01f;
	lights[0].kQ = 0.001f;
	lights[0].cosCutoff = cos(Math::DegreeToRadian(45));
	lights[0].cosInner = cos(Math::DegreeToRadian(30));
	lights[0].exponent = 3.f;
	lights[0].spotDirection.Set(0.f, 1.f, 0.f);

	lights[1].type = Light::LIGHT_DIRECTIONAL;
	lights[1].position.Set(1, 1, 0);
	lights[1].color.Set(1, 1, 0.5f, 1);
	lights[1].power = 0.4f;
	//lights[1].kC = 1.f;
	//lights[1].kL = 0.01f;
	//lights[1].kQ = 0.001f;
	//lights[1].cosCutoff = cos(Math::DegreeToRadian(45));
	//lights[1].cosInner = cos(Math::DegreeToRadian(30));
	//lights[1].exponent = 3.f;
	//lights[1].spotDirection.Set(0.f, 1.f, 0.f);
	
	glUniform1i(m_parameters[U_NUMLIGHTS], 1);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);

	glUniform1i(m_parameters[U_LIGHT0_TYPE], lights[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &lights[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], lights[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], lights[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], lights[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], lights[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], lights[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], lights[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], lights[0].exponent);
	
	glUniform1i(m_parameters[U_LIGHT1_TYPE], lights[1].type);
	glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &lights[1].color.r);
	glUniform1f(m_parameters[U_LIGHT1_POWER], lights[1].power);
	glUniform1f(m_parameters[U_LIGHT1_KC], lights[1].kC);
	glUniform1f(m_parameters[U_LIGHT1_KL], lights[1].kL);
	glUniform1f(m_parameters[U_LIGHT1_KQ], lights[1].kQ);
	glUniform1f(m_parameters[U_LIGHT1_COSCUTOFF], lights[1].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT1_COSINNER], lights[1].cosInner);
	glUniform1f(m_parameters[U_LIGHT1_EXPONENT], lights[1].exponent);

	//Fog params
	Color fogColor(0.79f, 0.84f, 0.85f);
	glUniform3fv(m_parameters[U_FOG_COLOR], 1, &fogColor.r);
	glUniform1f(m_parameters[U_FOG_START], 10.f);
	glUniform1f(m_parameters[U_FOG_END], 1000.f);
	glUniform1f(m_parameters[U_FOG_DENSITY], 0.0007f);
	glUniform1i(m_parameters[U_FOG_TYPE], 1);
	glUniform1i(m_parameters[U_FOG_ENABLED], 1);

	camera.Init(Vector3(-50, 0, 260), Vector3(0, 0, 0), Vector3(0, 1, 0));

	for(int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = NULL;
	}
	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("reference");
	meshList[GEO_CROSSHAIR] = MeshBuilder::GenerateCrossHair("crosshair");
	meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("quad", Color(1, 1, 1), 1.f);
	meshList[GEO_QUAD]->textureArray[0] = LoadTGA("Image//calibri.tga");
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureArray[0] = LoadTGA("Image//calibri.tga");
	meshList[GEO_TEXT]->material.kAmbient.Set(1, 0, 0);

	meshList[GEO_RING] = MeshBuilder::GenerateRing("ring", Color(1, 0, 1), 36, 1, 0.5f);
	meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere("lightball", Color(1, 1, 1), 18, 36, 1.f);
	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("sphere", Color(1, 0, 0), 18, 36, 10.f);

	//SKYPLANE
	meshList[GEO_SKYPLANE] = MeshBuilder::GenerateSkyPlane("skyplane", Color(1.0f, 0, 0), 128, 200.f, 2100.f, 1, 1);
	meshList[GEO_SKYPLANE]->textureArray[0] = LoadTGA("Image//newSky.tga");

	//TERRAIN
	meshList[GEO_TERRAIN] = MeshBuilder::GenerateTerrain("terrain", "Image//heightmap2.raw", m_heightMap);
	meshList[GEO_TERRAIN]->textureArray[0] = LoadTGA("Image//ForestFloor.tga");

	//TREE
	meshList[GEO_TREE] = MeshBuilder::GenerateOBJ("treeObj", "OBJ//Tree.obj");
	meshList[GEO_TREE]->textureArray[0] = LoadTGA("Image//Tree.tga");
	meshList[GEO_TREE]->textureArray[1] = LoadTGA("Image//TreeMoss.tga");

	//GRASS
	meshList[GEO_GRASS] = MeshBuilder::GenerateOBJ("grassObj", "OBJ//Grass.obj");
	meshList[GEO_GRASS]->textureArray[0] = LoadTGA("Image//Grass.tga");

	//WATER
	meshList[GEO_WATER] = MeshBuilder::GenerateQuad("waterplane", Color(0,0,0), 150.f);
	meshList[GEO_WATER]->textureArray[0] = LoadTGA("Image//water.tga");

	//CAMPFIRE
	meshList[GEO_CAMPFIRE] = MeshBuilder::GenerateOBJ("CampfireObj", "OBJ//Campfire.obj");
	meshList[GEO_CAMPFIRE]->textureArray[0] = LoadTGA("Image//Campfire.tga");

	//Particles
	meshList[GEO_LEAF] = MeshBuilder::GenerateQuad("leafplane", Color(0, 0, 0), 10.f);
	meshList[GEO_LEAF]->textureArray[0] = LoadTGA("Image//Leaf.tga");
	meshList[GEO_SMOKE] = MeshBuilder::GenerateQuad("smokeplane", Color(0, 0, 0), 10.f);
	meshList[GEO_SMOKE]->textureArray[0] = LoadTGA("Image//Smoke.tga");
	meshList[GEO_F_LIGHTS] = MeshBuilder::GenerateQuad("fLightsPlane", Color(0, 0, 0), 10.f);
	meshList[GEO_F_LIGHTS]->textureArray[0] = LoadTGA("Image//Forest_Lights.tga");

	//HUMAN MODEL
	meshList[GEO_MODEL_HEAD] = MeshBuilder::GenerateOBJ("modelHead", "OBJ//Model_Head.obj");
	meshList[GEO_MODEL_HEAD]->textureArray[0] = LoadTGA("Image//Model_Head.tga");
	meshList[GEO_MODEL_FACE] = MeshBuilder::GenerateOBJ("modelHead", "OBJ//Model_Face.obj");
	meshList[GEO_MODEL_FACE]->textureArray[0] = LoadTGA("Image//Model_Face.tga");
	meshList[GEO_MODEL_UPPER] = MeshBuilder::GenerateOBJ("modelHead", "OBJ//Model_Upper.obj");
	meshList[GEO_MODEL_UPPER]->textureArray[0] = LoadTGA("Image//Model_Upper.tga");
	meshList[GEO_MODEL_BELT] = MeshBuilder::GenerateOBJ("modelHead", "OBJ//Model_Belt.obj");
	meshList[GEO_MODEL_BELT]->textureArray[0] = LoadTGA("Image//Model_Gloves.tga");
	meshList[GEO_MODEL_GLOVES] = MeshBuilder::GenerateOBJ("modelHead", "OBJ//Model_Gloves.obj");
	meshList[GEO_MODEL_GLOVES]->textureArray[0] = LoadTGA("Image//Model_Gloves.tga");
	meshList[GEO_MODEL_LOWER] = MeshBuilder::GenerateOBJ("modelHead", "OBJ//Model_Lower.obj");
	meshList[GEO_MODEL_LOWER]->textureArray[0] = LoadTGA("Image//Model_Lower.tga");
	meshList[GEO_MODEL_SHOES] = MeshBuilder::GenerateOBJ("modelHead", "OBJ//Model_Shoes.obj");
	meshList[GEO_MODEL_SHOES]->textureArray[0] = LoadTGA("Image//Model_Shoes.tga");

	//SPRITE ANIMATION
	meshList[GEO_FIRE_ANIMATION] = MeshBuilder::GenerateSpriteAnimation("fireAnim", 1, 6);
	meshList[GEO_FIRE_ANIMATION]->textureArray[0] = LoadTGA("Image//Fire.tga");
	FireAnim = dynamic_cast<SpriteAnimation*>(meshList[GEO_FIRE_ANIMATION]);
	if (FireAnim)
	{
		FireAnim->m_anim = new Animation();
		FireAnim->m_anim->Set(0, 5, 0, 1.f, true);
		FireAnim->pos.Set(-160.f, 350.f * ReadHeightMap(m_heightMap, -160.f / 4000, -120.f / 4000) + 28.f, -120.f);
	}

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 1000 units
	Mtx44 perspective;
	perspective.SetToPerspective(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
	//perspective.SetToOrtho(-80, 80, -60, 60, -1000, 1000);
	projectionStack.LoadMatrix(perspective);
	
	rotateAngle = 0;

	//Add Trees
	for (int i = 0; i < 200; ++i)
	{
		ObjectList.push_back(new GameObject(GameObject::GO_TREE));	
	}

	//Add Grass
	for (int i = 0; i < 500; ++i)
	{
		ObjectList.push_back(new GameObject(GameObject::GO_GRASS));
	}

	for (unsigned int i = 0; i < ObjectList.size(); ++i)
	{
		if (ObjectList[i]->type == GameObject::GO_TREE)
		{
			while (ObjectList[i]->pos.x < 500 && ObjectList[i]->pos.x > -350 && ObjectList[i]->pos.z < 450 && ObjectList[i]->pos.z > -800)
			{
				int rotationamount = Math::RandIntMinMax(0, 359);
				float randXPos = Math::RandFloatMinMax(-1800, 1800);
				float randZPos = Math::RandFloatMinMax(-1500, 1500);
				float randScale = Math::RandFloatMinMax(30, 50);
				ObjectList[i]->pos.Set(randXPos, 0, randZPos);
				ObjectList[i]->rotationX = RotateX(ObjectList[i]);
				ObjectList[i]->rotationY = (float)rotationamount;
				ObjectList[i]->rotationZ = RotateZ(ObjectList[i]);
				ObjectList[i]->type = GameObject::GO_TREE;
				ObjectList[i]->scale.Set(randScale, randScale, randScale);
				ObjectList[i]->active = true;
				ObjectList[i]->camDist = (camera.position - ObjectList[i]->pos).Length();
			}
		}
		else if (ObjectList[i]->type == GameObject::GO_GRASS)
		{
			while (ObjectList[i]->pos.x < 310 && ObjectList[i]->pos.x > -250 && ObjectList[i]->pos.z < 210 && ObjectList[i]->pos.z > -550)
			{
				int rotationamount = Math::RandIntMinMax(0, 359);
				float randXPos = Math::RandFloatMinMax(-1800, 1800);
				float randZPos = Math::RandFloatMinMax(-1500, 1500);
				float randScale = Math::RandFloatMinMax(10, 20);
				ObjectList[i]->pos.Set(randXPos, 0, randZPos);
				ObjectList[i]->rotationX = RotateX(ObjectList[i]);
				ObjectList[i]->rotationY = (float)rotationamount;
				ObjectList[i]->rotationZ = RotateZ(ObjectList[i]);
				ObjectList[i]->type = GameObject::GO_GRASS;
				ObjectList[i]->scale.Set(randScale, randScale, randScale);
				ObjectList[i]->active = true;
				ObjectList[i]->camDist = (camera.position - ObjectList[i]->pos).Length();
			}
		}
	}

	skyplanerotation = 0;
	watertranslateZ1 = 0;
	watertranslateZ2 = -749.8f;
	bLightEnabled = true;
	timer = 0;
}

float SceneText::RotateX(GameObject* Object)
{
	Vector3 frontZ;
	Vector3 backZ;
	Vector3 centerZ;

	frontZ.Set(Object->pos.x, 350.f * ReadHeightMap(m_heightMap, Object->pos.x / 4000, (Object->pos.z + 15) / 4000), Object->pos.z + 15);
	backZ.Set(Object->pos.x, 350.f * ReadHeightMap(m_heightMap, Object->pos.x / 4000, (Object->pos.z - 15) / 4000), Object->pos.z - 15);

	centerZ = frontZ - backZ;

	if (frontZ.y > backZ.y)
	{
		return -Math::RadianToDegree(acosf(centerZ.Dot(Vector3(0, 0, 1)) / centerZ.Length()));
	}
	else
	{
		return Math::RadianToDegree(acosf(centerZ.Dot(Vector3(0, 0, 1)) / centerZ.Length()));
	}
}

float SceneText::RotateZ(GameObject* Object)
{
	Vector3 frontX;
	Vector3 backX;
	Vector3 centerX;

	frontX.Set(Object->pos.x + 15, 350.f * ReadHeightMap(m_heightMap, (Object->pos.x + 15) / 4000, Object->pos.z / 4000), Object->pos.z);
	backX.Set(Object->pos.x - 15, 350.f * ReadHeightMap(m_heightMap, (Object->pos.x - 15) / 4000, Object->pos.z / 4000), Object->pos.z);

	centerX = frontX - backX;

	if (frontX.y > backX.y)
	{
		return -Math::RadianToDegree(acosf(centerX.Dot(Vector3(1, 0, 0)) / centerX.Length()));
	}
	else
	{
		return Math::RadianToDegree(acosf(centerX.Dot(Vector3(1, 0, 0)) / centerX.Length()));
	}
}

GameObject* SceneText::FetchGO()
{
	//Exercise 3a: Fetch a game object from m_goList and return it
	for (unsigned int i = 0; i < ObjectList.size(); ++i)
	{
		if (ObjectList[i]->active == false)
		{
			ObjectList[i]->active = true;
			return ObjectList[i];
		}
	}

	//Check if m_golist ran out of objects
	for (int i = 0; i < 10; ++i)
	{
		ObjectList.push_back(new GameObject(GameObject::GO_LEAF));
	}
	return ObjectList[ObjectList.size() - 10];
}

void SceneText::QuickSortAlpha(int first, int last)
{
	if (first < last)
	{
		int middle = (first + last) / 2;

		GameObject* temp = ObjectList[first];
		ObjectList[first] = ObjectList[middle];
		ObjectList[middle] = temp;

		GameObject* pivot = ObjectList[first];
		int smallIndex = first;

		for (int i = first + 1; i <= last; i++) //start from 2nd element
		{
			if (ObjectList[i]->camDist > pivot->camDist)
			{
				smallIndex++;
				temp = ObjectList[smallIndex];
				ObjectList[smallIndex] = ObjectList[i];
				ObjectList[i] = temp;
			}
		}

		temp = ObjectList[first];
		ObjectList[first] = ObjectList[smallIndex];
		ObjectList[smallIndex] = temp;

		int pivotLocation = smallIndex;
		QuickSortAlpha(first, pivotLocation - 1);
		QuickSortAlpha(pivotLocation + 1, last);
	}
}

void SceneText::Update(double dt)
{
	if(Application::IsKeyPressed('1'))
		glEnable(GL_CULL_FACE);
	if(Application::IsKeyPressed('2'))
		glDisable(GL_CULL_FACE);
	if(Application::IsKeyPressed('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(Application::IsKeyPressed('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	if(Application::IsKeyPressed('5'))
	{
		lights[0].type = Light::LIGHT_POINT;
		glUniform1i(m_parameters[U_LIGHT0_TYPE], lights[0].type);
	}
	else if(Application::IsKeyPressed('6'))
	{
		lights[0].type = Light::LIGHT_DIRECTIONAL;
		glUniform1i(m_parameters[U_LIGHT0_TYPE], lights[0].type);
	}
	else if(Application::IsKeyPressed('7'))
	{
		lights[0].type = Light::LIGHT_SPOT;
		glUniform1i(m_parameters[U_LIGHT0_TYPE], lights[0].type);
	}
	else if(Application::IsKeyPressed('8'))
	{
		bLightEnabled = true;
	}
	else if(Application::IsKeyPressed('9'))
	{
		bLightEnabled = false;
	}

	if (Application::IsKeyPressed('Z'))
	{
		glUniform1i(m_parameters[U_FOG_ENABLED], 0);
	}
	else if (Application::IsKeyPressed('X'))
	{
		glUniform1i(m_parameters[U_FOG_ENABLED], 1);
	}

	if(Application::IsKeyPressed('I'))
		lights[0].position.z -= (float)(10.f * dt);
	if(Application::IsKeyPressed('K'))
		lights[0].position.z += (float)(10.f * dt);
	if(Application::IsKeyPressed('J'))
		lights[0].position.x -= (float)(10.f * dt);
	if(Application::IsKeyPressed('L'))
		lights[0].position.x += (float)(10.f * dt);
	if(Application::IsKeyPressed('O'))
		lights[0].position.y -= (float)(10.f * dt);
	if(Application::IsKeyPressed('P'))
		lights[0].position.y += (float)(10.f * dt);

	timer += dt;
	sortTimer += (float)dt;
	rotateAngle += (float)(10 * dt);
	skyplanerotation += (float)(5 * dt);
	watertranslateZ1 += (float)(50 * dt);
	if (watertranslateZ1 > 749)
	{
		watertranslateZ1 = -749.f;
	}
	watertranslateZ2 += (float)(50 * dt);
	if(watertranslateZ2 > 749)
	{
		watertranslateZ2 = -749.f;
	}

	if (FireAnim)
	{
		FireAnim->Update(dt);
		FireAnim->m_anim->animActive = true;
		FireAnim->direction = (camera.position - FireAnim->pos).Normalized();
		float diffinAngle = Math::RadianToDegree(atan2(FireAnim->direction.x, FireAnim->direction.z));
		FireAnim->rotationY = (float)diffinAngle;
	}

	//Sort every 0.3 second
	if (sortTimer > 0.3)
	{
		//QuickSort for alpha 
		QuickSortAlpha(0, ObjectList.size() - 1);
		sortTimer = 0.0;
	}

	camera.SetCameraY(30 + 350.f * ReadHeightMap(m_heightMap, camera.position.x / 4000, camera.position.z / 4000), dt);
	camera.Update(dt);
	UpdateParticles(dt);
	fps = (float)(1.f / dt);
}

void SceneText::UpdateParticles(double dt)
{
	//Leaves Spawning
	for (int i = 0; i < 2; ++i)
	{
		float randXPos = Math::RandFloatMinMax(-1800, 1800);
		float randZPos = Math::RandFloatMinMax(-1500, 1500);
		float randXVel = Math::RandFloatMinMax(-20, 20);
		float randYVel = Math::RandFloatMinMax(-50, -10);
		float randZVel = Math::RandFloatMinMax(-20, 20);
		GameObject* newleaf = FetchGO();
		newleaf->pos.Set(randXPos, 450, randZPos);
		newleaf->vel.Set(randXVel, randYVel, randZVel);
		newleaf->direction = (camera.position - newleaf->pos).Normalized();
		newleaf->rotationX = 0;
		newleaf->rotationY = 0;
		newleaf->rotationZ = 50;
		newleaf->type = GameObject::GO_LEAF;
		newleaf->scale.Set(1, 0.5f, 1);
		newleaf->camDist = (camera.position - newleaf->pos).Length();
	}

	//Smoke Spawning
	float randXPos = Math::RandFloatMinMax(-180, -140);
	float randZPos = Math::RandFloatMinMax(-140, -100);
	float randYPos = Math::RandFloatMinMax(40, 50);
	float randYVel = Math::RandFloatMinMax(20, 30);
	float randScale = Math::RandFloatMinMax(0.5f, 0.8f);
	GameObject* newsmoke = FetchGO();
	newsmoke->pos.Set(randXPos, 350.f * ReadHeightMap(m_heightMap, -160.f / 4000, -120.f / 4000) + randYPos, randZPos);
	newsmoke->vel.Set(0, randYVel, 0);
	newsmoke->direction = (camera.position - newsmoke->pos).Normalized();
	newsmoke->rotationX = 0;
	newsmoke->rotationY = 0;
	newsmoke->rotationZ = 0;
	newsmoke->type = GameObject::GO_SMOKE;
	newsmoke->scale.Set(randScale, randScale, randScale);
	newsmoke->camDist = (camera.position - newsmoke->pos).Length();

	//Forest Lights Spawning
	randXPos = Math::RandFloatMinMax(-1800, 1800);
	randZPos = Math::RandFloatMinMax(-1500, 1500);
	randYVel = Math::RandFloatMinMax(15, 25);
	randScale = Math::RandFloatMinMax(0.5, 1.5f);
	float randDuration = Math::RandFloatMinMax(4, 6);
	GameObject* newfLight = FetchGO();
	newfLight->pos.Set(randXPos, 350.f * ReadHeightMap(m_heightMap, -160.f / 4000, -120.f / 4000), randZPos);
	newfLight->vel.Set(0, randYVel, 0);
	newfLight->direction = (camera.position - newfLight->pos).Normalized();
	newfLight->rotationX = 0;
	newfLight->rotationY = 0;
	newfLight->rotationZ = 0;
	newfLight->type = GameObject::GO_F_LIGHTS;
	newfLight->scale.Set(randScale, randScale, randScale);
	newfLight->camDist = (camera.position - newfLight->pos).Length();
	newfLight->duration = (float)timer + randDuration;

	//Particles Billboard + movement
	for (std::vector<GameObject *>::iterator it = ObjectList.begin(); it != ObjectList.end(); ++it)
	{
		GameObject *particle = (GameObject *)*it;

		//Leaves
		if (particle->type == GameObject::GO_LEAF && particle->active == true)
		{
			particle->direction = (camera.position - particle->pos).Normalized();
			float diffinAngle = Math::RadianToDegree(atan2(particle->direction.x, particle->direction.z));
			particle->rotationY = (float)diffinAngle;

			particle->pos += particle->vel * (float)dt;

			//Set active to false if out of map
			if (particle->pos.y < (350.f * ReadHeightMap(m_heightMap, particle->pos.x / 4000, particle->pos.z / 4000)))
			{
				particle->active = false;
			}
		}

		//Smoke
		else if (particle->type == GameObject::GO_SMOKE && particle->active == true)
		{
			if (particle->scale.x <= 0)
			{
				particle->active = false;
			}
			else
			{
				particle->direction = (camera.position - particle->pos).Normalized();
				float diffinAngle = Math::RadianToDegree(atan2(particle->direction.x, particle->direction.z));
				particle->rotationY = (float)diffinAngle;

				particle->scale -= Vector3(0.2f, 0.2f, 0.2f) * (float)dt;
				particle->pos += particle->vel * (float)dt;
			}
		}

		//Forest Lights
		else if (particle->type == GameObject::GO_F_LIGHTS && particle->active == true)
		{
			if (particle->duration < timer)
			{
				particle->active = false;
			}
			else
			{
				particle->direction = (camera.position - particle->pos).Normalized();
				float diffinAngle = Math::RadianToDegree(atan2(particle->direction.x, particle->direction.z));
				particle->rotationY = (float)diffinAngle;

				particle->pos += particle->vel * (float)dt;
			}
		}
	}
}

void SceneText::RenderText(Mesh* mesh, std::string text, Color color)
{
	if(!mesh || mesh->textureArray[0] <= 0)
		return;
	
	glDisable(GL_DEPTH_TEST);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for(unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);
}

void SceneText::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	if(!mesh || mesh->textureID <= 0)
		return;
	
	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10);
	projectionStack.PushMatrix();
		projectionStack.LoadMatrix(ortho);
		viewStack.PushMatrix();
			viewStack.LoadIdentity();
			modelStack.PushMatrix();
				modelStack.LoadIdentity();
				modelStack.Translate(x, y, 0);
				modelStack.Scale(size, size, size);
				glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
				glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
				glUniform1i(m_parameters[U_LIGHTENABLED], 0);
				glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mesh->textureArray[0]);
				glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
				for(unsigned i = 0; i < text.length(); ++i)
				{
					Mtx44 characterSpacing;
					characterSpacing.SetToTranslation(i * 1.0f + 0.5f, 0.5f, 0); //1.0f is the spacing of each character, you may change this value
					Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
					glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	
					mesh->Render((unsigned)text[i] * 6, 6);
				}
				glBindTexture(GL_TEXTURE_2D, 0);
				glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
			modelStack.PopMatrix();
		viewStack.PopMatrix();
	projectionStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void SceneText::RenderMeshIn2D(Mesh *mesh, bool enableLight, float size, float x, float y)
{
	Mtx44 ortho;
	ortho.SetToOrtho(-80, 80, -60, 60, -10, 10);
	projectionStack.PushMatrix();
		projectionStack.LoadMatrix(ortho);
		viewStack.PushMatrix();
			viewStack.LoadIdentity();
			modelStack.PushMatrix();
				modelStack.LoadIdentity();
				modelStack.Scale(size, size, size);
				modelStack.Translate(x, y, 0);
       
				Mtx44 MVP, modelView, modelView_inverse_transpose;
	
				MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
				glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
				if(mesh->textureArray[0] > 0)
				{
					glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, mesh->textureArray[0]);
					glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
				}
				else
				{
					glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
				}
				mesh->Render();
				if(mesh->textureArray[0] > 0)
				{
					glBindTexture(GL_TEXTURE_2D, 0);
				}
       
			modelStack.PopMatrix();
		viewStack.PopMatrix();
	projectionStack.PopMatrix();

}

void SceneText::RenderMesh(Mesh *mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;
	
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);

	if(enableLight && bLightEnabled)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView.a[0]);
		
		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{	
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}
	/*if(mesh->textureID > 0)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}*/

	for (int i = 0; i < MAX_TEXTURES; ++i)
	{
		if (mesh->textureArray[i] > 0)
		{
			glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED + i], 1);
		}
		else
		{
			glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED + i], 0);
		}

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, mesh->textureArray[i]);
		glUniform1i(m_parameters[U_COLOR_TEXTURE + i], i);
	}

	mesh->Render();
	//if(mesh->textureID > 0)
	//{
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//}
}

void SceneText::RenderGround()
{
	modelStack.PushMatrix();
	modelStack.Rotate(-90, 1, 0, 0);
	modelStack.Translate(0, 0, -10);
	modelStack.Rotate(-90, 0, 0, 1);
	modelStack.Scale(400.0f, 400.0f, 400.0f);

	for (int x=0; x<10; x++)
	{
		for (int z=0; z<10; z++)
		{
			modelStack.PushMatrix();
			modelStack.Translate(x-5.0f, z-5.0f, 0.0f);
			if ( ((x*9+z) % 2) == 0)
				RenderMesh(meshList[GEO_GRASS_DARKGREEN], false);
			else
				RenderMesh(meshList[GEO_GRASS_LIGHTGREEN], false);
			modelStack.PopMatrix();
		}
	}
	modelStack.PopMatrix();
}

void SceneText::RenderTree()
{
	//Trees in pond area
	modelStack.PushMatrix();
	modelStack.Translate(-116.f, 350.f * ReadHeightMap(m_heightMap, -116.f / 4000, -201.f / 4000) - 40.f, -201.f);
	modelStack.Rotate(-20, 0, 0, 1);
	modelStack.Scale(30.0f, 30.f, 30.f);
	RenderMesh(meshList[GEO_TREE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-235.f, 350.f * ReadHeightMap(m_heightMap, -235.f / 4000, 25.f / 4000) - 20.f, 25.f);
	modelStack.Rotate(45, 0, 1, 0);
	modelStack.Scale(40.0f, 40.f, 40.f);
	RenderMesh(meshList[GEO_TREE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-259.f, 350.f * ReadHeightMap(m_heightMap, -259.f / 4000, 332.f / 4000), 332.f);
	modelStack.Rotate(150, 0, 1, 0);
	modelStack.Scale(30.0f, 30.f, 30.f);
	RenderMesh(meshList[GEO_TREE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(168.f, 350.f * ReadHeightMap(m_heightMap, 168.f / 4000, 294.f / 4000), 294.f);
	modelStack.Rotate(270, 0, 1, 0);
	modelStack.Rotate(20, 1, 0, 1);
	modelStack.Scale(30.0f, 30.f, 30.f);
	RenderMesh(meshList[GEO_TREE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(435.f, 350.f * ReadHeightMap(m_heightMap, 435.f / 4000, -106.f / 4000), -106.f);
	modelStack.Scale(30.0f, 30.f, 30.f);
	RenderMesh(meshList[GEO_TREE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(274.f, 350.f * ReadHeightMap(m_heightMap, 274.f / 4000, -435.f / 4000), -435.f);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Rotate(-10, 0, 0, 1);
	modelStack.Scale(30.0f, 30.f, 30.f);
	RenderMesh(meshList[GEO_TREE], true);
	modelStack.PopMatrix();

	//Trees outside of pond area
	for (unsigned int i = 0;i < ObjectList.size(); ++i)
	{
		if (ObjectList[i]->type == GameObject::GO_TREE)
		{
			modelStack.PushMatrix();
			modelStack.Translate(ObjectList[i]->pos.x, 350.f * ReadHeightMap(m_heightMap, ObjectList[i]->pos.x / 4000, ObjectList[i]->pos.z / 4000) - 30.f, ObjectList[i]->pos.z);
			modelStack.Rotate(ObjectList[i]->rotationX, 1, 0, 0);
			modelStack.Rotate(ObjectList[i]->rotationY, 0, 1, 0);
			modelStack.Rotate(ObjectList[i]->rotationZ, 0, 0, 1);
			modelStack.Scale(ObjectList[i]->scale.x, ObjectList[i]->scale.y, ObjectList[i]->scale.z);
			RenderMesh(meshList[GEO_TREE], true);
			modelStack.PopMatrix();
		}	
	}
}

void SceneText::RenderGrass()
{
	for (unsigned int i = 0;i < ObjectList.size(); ++i)
	{
		if (ObjectList[i]->type == GameObject::GO_GRASS)
		{
			modelStack.PushMatrix();
			modelStack.Translate(ObjectList[i]->pos.x, 350.f * ReadHeightMap(m_heightMap, ObjectList[i]->pos.x / 4000, ObjectList[i]->pos.z / 4000), ObjectList[i]->pos.z);
			modelStack.Rotate(ObjectList[i]->rotationX, 1, 0, 0);
			modelStack.Rotate(ObjectList[i]->rotationY, 0, 1, 0);
			modelStack.Rotate(ObjectList[i]->rotationZ, 0, 0, 1);
			modelStack.Scale(ObjectList[i]->scale.x * 2, ObjectList[i]->scale.y, ObjectList[i]->scale.z * 2);
			RenderMesh(meshList[GEO_GRASS], false);
			modelStack.PopMatrix();
		}
	}
}

void SceneText::RenderParticles()
{
	for (unsigned int i = 0;i < ObjectList.size(); ++i)
	{
		if (ObjectList[i]->type == GameObject::GO_LEAF && ObjectList[i]->active == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(ObjectList[i]->pos.x, ObjectList[i]->pos.y, ObjectList[i]->pos.z);
			modelStack.Rotate(ObjectList[i]->rotationX, 1, 0, 0);
			modelStack.Rotate(ObjectList[i]->rotationY, 0, 1, 0);
			modelStack.Rotate(ObjectList[i]->rotationZ, 0, 0, 1);
			modelStack.Scale(ObjectList[i]->scale.x, ObjectList[i]->scale.y, ObjectList[i]->scale.z);
			RenderMesh(meshList[GEO_LEAF], false);
			modelStack.PopMatrix();
		}
		else if (ObjectList[i]->type == GameObject::GO_SMOKE && ObjectList[i]->active == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(ObjectList[i]->pos.x, ObjectList[i]->pos.y, ObjectList[i]->pos.z);
			modelStack.Rotate(ObjectList[i]->rotationX, 1, 0, 0);
			modelStack.Rotate(ObjectList[i]->rotationY, 0, 1, 0);
			modelStack.Rotate(ObjectList[i]->rotationZ, 0, 0, 1);
			modelStack.Scale(ObjectList[i]->scale.x, ObjectList[i]->scale.y, ObjectList[i]->scale.z);
			RenderMesh(meshList[GEO_SMOKE], false);
			modelStack.PopMatrix();
		}
		else if (ObjectList[i]->type == GameObject::GO_F_LIGHTS && ObjectList[i]->active == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(ObjectList[i]->pos.x, ObjectList[i]->pos.y, ObjectList[i]->pos.z);
			modelStack.Rotate(ObjectList[i]->rotationX, 1, 0, 0);
			modelStack.Rotate(ObjectList[i]->rotationY, 0, 1, 0);
			modelStack.Rotate(ObjectList[i]->rotationZ, 0, 0, 1);
			modelStack.Scale(ObjectList[i]->scale.x, ObjectList[i]->scale.y, ObjectList[i]->scale.z);
			RenderMesh(meshList[GEO_F_LIGHTS], false);
			modelStack.PopMatrix();
		}
	}
}

void SceneText::RenderTerrain()
{
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0);
	modelStack.Scale(4000.0f, 350.f, 4000.0f);
	RenderMesh(meshList[GEO_TERRAIN], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(90, 80, -186 + watertranslateZ1);
	modelStack.Rotate(-90, 1, 0, 0);
	modelStack.Scale(3.f, 5.f, 1.f);
	RenderMesh(meshList[GEO_WATER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(90, 80, -186 + watertranslateZ2);
	modelStack.Rotate(-90, 1, 0, 0);
	modelStack.Scale(3.f, 5.f, 1.f);
	RenderMesh(meshList[GEO_WATER], true);
	modelStack.PopMatrix();
}

void SceneText::RenderSkyPlane()
{
	modelStack.PushMatrix();
	modelStack.Translate(0, 2100, 0);
	modelStack.Rotate(skyplanerotation, 0, 1, 0);
	RenderMesh(meshList[GEO_SKYPLANE], false);
	modelStack.PopMatrix();
}

void SceneText::RenderHuman()
{
	//HEAD
	modelStack.PushMatrix();
	modelStack.Translate(-213.f, 350.f * ReadHeightMap(m_heightMap, -213.f / 4000, -168.f / 4000), -168.f);
	modelStack.Rotate(45, 0, 1, 0);
	modelStack.Scale(60, 60, 60);
	RenderMesh(meshList[GEO_MODEL_HEAD], false);
	modelStack.PopMatrix();

	//FACE
	modelStack.PushMatrix();
	modelStack.Translate(-213.f, 350.f * ReadHeightMap(m_heightMap, -213.f / 4000, -168.f / 4000), -168.f);
	modelStack.Rotate(45, 0, 1, 0);
	modelStack.Scale(60, 60, 60);
	RenderMesh(meshList[GEO_MODEL_FACE], false);
	modelStack.PopMatrix();

	//UPPER
	modelStack.PushMatrix();
	modelStack.Translate(-213.f, 350.f * ReadHeightMap(m_heightMap, -213.f / 4000, -168.f / 4000), -168.f);
	modelStack.Rotate(45, 0, 1, 0);
	modelStack.Scale(60, 60, 60);
	RenderMesh(meshList[GEO_MODEL_UPPER], false);
	modelStack.PopMatrix();

	//BELT
	modelStack.PushMatrix();
	modelStack.Translate(-213.f, 350.f * ReadHeightMap(m_heightMap, -213.f / 4000, -168.f / 4000), -168.f);
	modelStack.Rotate(45, 0, 1, 0);
	modelStack.Scale(60, 60, 60);
	RenderMesh(meshList[GEO_MODEL_BELT], false);
	modelStack.PopMatrix();

	//GLOVES
	modelStack.PushMatrix();
	modelStack.Translate(-213.f, 350.f * ReadHeightMap(m_heightMap, -213.f / 4000, -168.f / 4000), -168.f);
	modelStack.Rotate(45, 0, 1, 0);
	modelStack.Scale(60, 60, 60);
	RenderMesh(meshList[GEO_MODEL_GLOVES], false);
	modelStack.PopMatrix();

	//LOWER
	modelStack.PushMatrix();
	modelStack.Translate(-213.f, 350.f * ReadHeightMap(m_heightMap, -213.f / 4000, -168.f / 4000), -168.f);
	modelStack.Rotate(45, 0, 1, 0);
	modelStack.Scale(60, 60, 60);
	RenderMesh(meshList[GEO_MODEL_LOWER], false);
	modelStack.PopMatrix();

	//SHOES
	modelStack.PushMatrix();
	modelStack.Translate(-213.f, 350.f * ReadHeightMap(m_heightMap, -213.f / 4000, -168.f / 4000), -168.f);
	modelStack.Rotate(45, 0, 1, 0);
	modelStack.Scale(60, 60, 60);
	RenderMesh(meshList[GEO_MODEL_SHOES], false);
	modelStack.PopMatrix();
}

void SceneText::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Mtx44 perspective;
	perspective.SetToPerspective(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
	//perspective.SetToOrtho(-80, 80, -60, 60, -1000, 1000);
	projectionStack.LoadMatrix(perspective);
	
	// Camera matrix
	viewStack.LoadIdentity();
	viewStack.LookAt(
						camera.position.x, camera.position.y, camera.position.z,
						camera.target.x, camera.target.y, camera.target.z,
						camera.up.x, camera.up.y, camera.up.z
					);
	// Model matrix : an identity matrix (model will be at the origin)
	modelStack.LoadIdentity();

	if(lights[0].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(lights[0].position.x, lights[0].position.y, lights[0].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if(lights[0].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * lights[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * lights[0].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * lights[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
	}
	if(lights[1].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(lights[1].position.x, lights[1].position.y, lights[1].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if(lights[1].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * lights[1].position;
		glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * lights[1].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT1_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * lights[1].position;
		glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightPosition_cameraspace.x);
	}
	
	//RenderMesh(meshList[GEO_AXES], false);

	// Render LightBall
	modelStack.PushMatrix();
	modelStack.Translate(lights[0].position.x, lights[0].position.y, lights[0].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], false);
	modelStack.PopMatrix();
	
	RenderHuman();	
	RenderSkyPlane();
	RenderTerrain();
	RenderTree();
	RenderGrass();
	RenderParticles();

	modelStack.PushMatrix();
	modelStack.Translate(FireAnim->pos.x, FireAnim->pos.y, FireAnim->pos.z);
	modelStack.Rotate(FireAnim->rotationY, 0, 1, 0);
	modelStack.Scale(50, 50, 50);
	RenderMesh(meshList[GEO_FIRE_ANIMATION], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-160.f, 350.f * ReadHeightMap(m_heightMap, -160.f / 4000, -120.f / 4000), -120.f);
	modelStack.Scale(5, 5, 5);
	RenderMesh(meshList[GEO_CAMPFIRE], true);
	modelStack.PopMatrix();

	// Render the crosshair
	RenderMeshIn2D(meshList[GEO_CROSSHAIR], false, 10.0f);

	//On screen text
	std::ostringstream ss;
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 6);
	
	std::ostringstream ss1;
	ss1.precision(4);
	ss1 << "Light(" << lights[0].position.x << ", " << lights[0].position.y << ", " << lights[0].position.z << ")";
	RenderTextOnScreen(meshList[GEO_TEXT], ss1.str(), Color(0, 1, 0), 3, 0, 3);
}

void SceneText::Exit()
{
	// Cleanup VBO
	for(int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if(meshList[i])
			delete meshList[i];
	}
	while (ObjectList.size() > 0)
	{
		GameObject *object = ObjectList.back();
		delete object;
		ObjectList.pop_back();
	}
	glDeleteProgram(m_programID);
	glDeleteVertexArrays(1, &m_vertexArrayID);
}
