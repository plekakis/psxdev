#pragma once

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>

using PositionType = int16_t;
using NormalType = int16_t;
using TexcoordType = uint8_t;
using ColorType = uint8_t;

struct Position
{
	PositionType m_x;
	PositionType m_y;
	PositionType m_z;
};

struct Normal
{
	NormalType m_x;
	NormalType m_y;
	NormalType m_z;
};

struct Color
{
	ColorType m_r;
	ColorType m_g;
	ColorType m_b;
};

struct Texcoord
{
	TexcoordType m_u;
	TexcoordType m_v;
};

struct VertexData
{
	Position m_position;
	Normal   m_normal;
	Color    m_color;
	Texcoord m_texcoord;
};

struct TriangleData
{
	VertexData m_vertexData[3];
};

struct Submesh
{
	uint16_t m_triangleCount;
	std::vector<TriangleData> m_triangles;
};

struct Model
{
	std::vector<Submesh> m_submeshes;
};