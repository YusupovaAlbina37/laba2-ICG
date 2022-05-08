#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
using namespace glm;

class Pipeline
{
public:
	Pipeline()
	{
		m_scale      = vec3(1.0f, 1.0f, 1.0f);
		m_worldPos   = vec3(0.0f, 0.0f, 0.0f);
		m_rotateInfo = vec3(0.0f, 0.0f, 0.0f);
	}

	void Scale(float ScaleX, float ScaleY, float ScaleZ)
	{
		m_scale.x = ScaleX;
		m_scale.y = ScaleY;
		m_scale.z = ScaleZ;
	}

	void WorldPos(float x, float y, float z)
	{
		m_worldPos.x = x;
		m_worldPos.y = y;
		m_worldPos.z = z;
	}

	void Rotate(float RotateX, float RotateY, float RotateZ)
	{
		m_rotateInfo.x = RotateX;
		m_rotateInfo.y = RotateY;
		m_rotateInfo.z = RotateZ;
	}

	const mat4* GetTrans();

private:
	void InitScaleTransform(mat4& m) const;
	void InitRotateTransform(mat4& m) const;
	void InitTranslationTransform(mat4& m) const;

	vec3 m_scale;
	vec3 m_worldPos;
	vec3 m_rotateInfo;

	mat4 m_transformation;
};