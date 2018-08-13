#include "SpriteAnimation.h"
#include "GL\glew.h"
#include "Vertex.h"

SpriteAnimation::SpriteAnimation(const std::string &meshName, int row, int col)
	: Mesh(meshName)
	, m_row(row)
	, m_col(col)
	, m_currentTime(0)
	, m_currentFrame(0)
	, m_playCount(0)
	, direction(0,0,0)
	, pos(0,0,0)
	, rotationY(0)
{
	m_anim = NULL;
}

SpriteAnimation::~SpriteAnimation()
{
	if (m_anim)
		delete m_anim;
}

void SpriteAnimation::Update(double dt)
{
	if (m_anim->animActive == true) // If animation happens
	{
		// Get Animation current time, m_currentTime.
		//Animation current time increments with dt
		m_currentTime += (float)dt;

		// Find int numFrame.
		int numFrame = Math::Max(1, m_anim->endFrame - m_anim->startFrame + 1);

		// Get float frameTime.
		//FrameTime = animTime / numFrame
		float frameTime = m_anim->animTime / numFrame;
	
		// Get Animation current frame, m_currentFrame.
		m_currentFrame = Math::Min(m_anim->endFrame, m_anim->startFrame + (int)(m_currentTime / frameTime));

		// Check if m_currentTime is greater than or equal animTime
		if (m_currentTime >= m_anim->animTime)
		{
			//Check if repeatCount == 0
			if (m_anim->repeatCount == 0)
			{
				//Set animActive = false
				m_anim->animActive = false;
				//reset m_currentTime
				m_currentTime = 0;
				//set m_currentFrame = startFrame
				m_currentFrame = m_anim->startFrame;
				//Set animation end flag, ended to true
				m_anim->ended = true;
			}
		}
		
		//Check if repeatCount == 1
		if (m_anim->repeatCount == 1)
		{
			//reset m_currentTime
			m_currentTime = 0;
			//set m_currentFrame = startFrame
			m_currentFrame = m_anim->startFrame;
		}
	}
}

void SpriteAnimation::Render()
{
	// Insert codes. Hint: same as in Mesh.cpp.
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(Position));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Position) + sizeof(Color)));
	//if(textureID > 0)
	{
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Position) + sizeof(Color) + sizeof(Vector3)));
	}


	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	if (mode == DRAW_LINES)
		glDrawElements(GL_LINES, indexBuffer, GL_UNSIGNED_INT, (void*)(m_currentFrame * m_col * sizeof(GLuint)));
	else if (mode == DRAW_TRIANGLE_STRIP)
		glDrawElements(GL_TRIANGLE_STRIP, indexBuffer, GL_UNSIGNED_INT, (void*)(m_currentFrame * m_col * sizeof(GLuint)));
	else
		glDrawElements(GL_TRIANGLES, indexBuffer, GL_UNSIGNED_INT, (void*)(m_currentFrame * m_col * sizeof(GLuint)));

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	//if(textureID > 0)
	{
		glDisableVertexAttribArray(3);
	}
}