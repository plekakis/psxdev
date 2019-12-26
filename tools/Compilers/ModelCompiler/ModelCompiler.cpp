
#include "types.h"
#include "tinyobjloader/tiny_obj_loader.h"
#include <thirdparty/cxxopts/cxxopts.hpp>

namespace PSMHelpers
{
	static const uint32_t s_psmWarningTriCount = 1024;
	static const uint32_t s_psmErrorTriCount = s_psmWarningTriCount * 4;
	static const uint32_t s_psmHeader = 0x1234beef;

	static const float s_psmDefaultVertexScale = 0.01f;
	static const float s_psmVertexScale = static_cast<float>((1u << 12u));

	///////////////////////////////////////////////////
	float Saturate(float v)
	{
		return std::max(0.0f, std::min(1.0f, v));
	}

	///////////////////////////////////////////////////
	void WritePadding(FILE* f, uint8_t c)
	{
		uint8_t pad = 0u;
		fwrite(&pad, sizeof(uint8_t), c, f);
	}

	///////////////////////////////////////////////////
	void WriteColor(FILE* f, ColorType r, ColorType g, ColorType b)
	{
		fwrite(&r, sizeof(ColorType), 1, f);
		fwrite(&g, sizeof(ColorType), 1, f);
		fwrite(&b, sizeof(ColorType), 1, f);
		WritePadding(f, 1);
	}

	///////////////////////////////////////////////////
	void WriteColor(FILE* f, Color const& v)
	{
		WriteColor(f, v.m_r, v.m_g, v.m_b);
	}

	///////////////////////////////////////////////////
	void WritePosition(FILE* f, PositionType x, PositionType y, PositionType z)
	{		
		fwrite(&x, sizeof(PositionType), 1, f);
		fwrite(&y, sizeof(PositionType), 1, f);
		fwrite(&z, sizeof(PositionType), 1, f);
		WritePadding(f, 2);
	}

	///////////////////////////////////////////////////
	void WritePosition(FILE* f, Position const& v)
	{
		WritePosition(f, v.m_x, v.m_y, v.m_z);
	}

	///////////////////////////////////////////////////
	void WriteNormal(FILE* f, NormalType nx, NormalType ny, NormalType nz)
	{
		fwrite(&nx, sizeof(NormalType), 1, f);
		fwrite(&ny, sizeof(NormalType), 1, f);
		fwrite(&nz, sizeof(NormalType), 1, f);
		WritePadding(f, 2);
	}

	///////////////////////////////////////////////////
	void WriteNormal(FILE* f, Normal const& v)
	{
		WriteNormal(f, v.m_x, v.m_y, v.m_z);
	}

	///////////////////////////////////////////////////
	void WriteTexcoord(FILE* f, TexcoordType u, TexcoordType v)
	{
		fwrite(&u, sizeof(TexcoordType), 1, f);
		fwrite(&v, sizeof(TexcoordType), 1, f);
	}

	///////////////////////////////////////////////////
	void WriteTexcoord(FILE* f, Texcoord const& v)
	{
		WriteTexcoord(f, v.m_u, v.m_v);
	}

	///////////////////////////////////////////////////
	void WriteHeader(FILE* f, uint8_t submeshCount)
	{
		fwrite(&s_psmHeader, sizeof(uint32_t), 1, f);
		fwrite(&submeshCount, sizeof(uint8_t), 1, f);
		WritePadding(f, 3);
	}

	///////////////////////////////////////////////////
	void WriteSubmeshHeader(FILE* f, Submesh const& submesh)
	{
		fwrite(&submesh.m_triangleCount, sizeof(uint16_t), 1, f);
		WritePadding(f, 2);
	}
}

///////////////////////////////////////////////////
// Commandline options:
// --------------------
// -i filename, --input=filename	[specify input filename]
// -o filename, --output=filename	[specify output filename]
// -s value, --scale=value			[specify vertex scale]

///////////////////////////////////////////////////
// Regarding vertex format:
// ------------------------
//
// All vertex data is written out, assuming a GT3 primitive will be used at runtime.
// The runtime checks against the material definition for the currently loaded PSM to find the actual vertex format to use for storing & rendering.

static const std::string s_input = "input";
static const std::string s_output = "output";
static const std::string s_scale = "scale";

struct ModelCompileOptions
{
	std::string  m_inputFilename;
	std::string  m_outputFilename;
	float		 m_vertexScale;
};

///////////////////////////////////////////////////
// Parse commandline options.
///////////////////////////////////////////////////
bool parse(int argc, char* argv[], ModelCompileOptions& compileOptions)
{
	cxxopts::Options options("ModelCompiler", "Converts .obj 3D models to .psm");
	options.add_options()
		("i,input", "Input filename (obj)", cxxopts::value<std::string>())
		("o,output", "Output filename (psm)", cxxopts::value<std::string>())
		("s,scale", "Vertex scale", cxxopts::value<float>())
		;

	auto result = options.parse(argc, argv);
	
	// must specify input
	size_t const inputs = result.count(s_input);
	if (inputs == 0)
	{
		std::cout << "Input is not specified." << std::endl;
		return false;
	}
	
	// must specify output
	size_t const outputs = result.count(s_output);
	if (outputs == 0)
	{
		std::cout << "Output is not specified." << std::endl;
		return false;
	}
	
	// update the compilation structure
	compileOptions.m_inputFilename = result[s_input].as<std::string>();
	compileOptions.m_outputFilename = result[s_output].as<std::string>();
	if (result.count(s_scale) == 1)
	{
		compileOptions.m_vertexScale = result[s_scale].as<float>();
	}

	return true;
}

///////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	ModelCompileOptions options;
	try
	{
		if (parse(argc, argv, options))
		{
			// Load OBJ file.
			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;

			std::cout << "Loading " << options.m_inputFilename.c_str() << std::endl;

			std::string warn, err;
			bool const modelLoadSuccess = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, options.m_inputFilename.c_str());
			if (!modelLoadSuccess)
			{
				throw std::exception(err.c_str());
			}

			if (shapes.size() > std::numeric_limits<uint8_t>().max())
			{
				throw std::exception("Too many submeshes, must be less than 255!");
			}

			// Convert to PSM.
			Model psmModel;
			for (size_t s = 0; s < shapes.size(); s++)
			{
				Submesh psmSubmesh;
				memset(&psmSubmesh, 0, sizeof(psmSubmesh));

				psmSubmesh.m_triangleCount = static_cast<uint32_t>(shapes[s].mesh.indices.size() / 3u);
				
				if (psmSubmesh.m_triangleCount >= PSMHelpers::s_psmWarningTriCount)
				{
					std::cout << "Warning: Submesh " << s << " has over " << PSMHelpers::s_psmWarningTriCount << " triangles (" << psmSubmesh.m_triangleCount << "). This can impact performance a lot." << std::endl;
				}

				if (psmSubmesh.m_triangleCount >= PSMHelpers::s_psmErrorTriCount)
				{
					std::stringstream errStream;
					errStream << "Maximum triangle count reached! Submesh " << s << " has " << psmSubmesh.m_triangleCount << " triangles.";
					throw std::exception(errStream.str().c_str());
				}

				// Loop over faces(polygon)
				size_t index_offset = 0;
				for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
				{
					size_t const fv = shapes[s].mesh.num_face_vertices[f];
					if (fv != 3)
					{
						throw std::exception("PSM supports only faces made out of triangles (3 vertices)");
					}
					
					TriangleData psmTri;

					// Loop over vertices in the face.
					for (size_t v = 0; v < fv; v++)
					{
						// access to vertex
						tinyobj::index_t const idx = shapes[s].mesh.indices[index_offset + v];
						
						psmTri.m_vertexData[v].m_position.m_x = static_cast<PositionType>(options.m_vertexScale * attrib.vertices[3 * idx.vertex_index + 0] * PSMHelpers::s_psmVertexScale);
						psmTri.m_vertexData[v].m_position.m_y = static_cast<PositionType>(options.m_vertexScale * attrib.vertices[3 * idx.vertex_index + 1] * PSMHelpers::s_psmVertexScale);
						psmTri.m_vertexData[v].m_position.m_z = static_cast<PositionType>(options.m_vertexScale * attrib.vertices[3 * idx.vertex_index + 2] * PSMHelpers::s_psmVertexScale);
						
						psmTri.m_vertexData[v].m_normal.m_x = idx.normal_index >= 0 ? static_cast<NormalType>(attrib.normals[3 * idx.normal_index + 0] * 4096.0f) : 0;
						psmTri.m_vertexData[v].m_normal.m_y = idx.normal_index >= 0 ? static_cast<NormalType>(attrib.normals[3 * idx.normal_index + 1] * 4096.0f) : 0;
						psmTri.m_vertexData[v].m_normal.m_z = idx.normal_index >= 0 ? static_cast<NormalType>(attrib.normals[3 * idx.normal_index + 2] * 4096.0f) : 0;
						
						psmTri.m_vertexData[v].m_texcoord.m_u = static_cast<TexcoordType>(PSMHelpers::Saturate(attrib.texcoords[2 * idx.texcoord_index + 0]) * 255.0f);
						psmTri.m_vertexData[v].m_texcoord.m_v = static_cast<TexcoordType>((1.0f - PSMHelpers::Saturate(attrib.texcoords[2 * idx.texcoord_index + 1])) * 255.0f);
						
						psmTri.m_vertexData[v].m_color.m_r = static_cast<ColorType>(PSMHelpers::Saturate(attrib.colors[3 * idx.vertex_index + 0]) * 255.0f);
						psmTri.m_vertexData[v].m_color.m_g = static_cast<ColorType>(PSMHelpers::Saturate(attrib.colors[3 * idx.vertex_index + 1]) * 255.0f);
						psmTri.m_vertexData[v].m_color.m_b = static_cast<ColorType>(PSMHelpers::Saturate(attrib.colors[3 * idx.vertex_index + 2]) * 255.0f);
					}
					
					psmSubmesh.m_triangles.push_back(psmTri);

					index_offset += fv;

					// per-face material
					shapes[s].mesh.material_ids[f];
				}

				if (psmSubmesh.m_triangles.size() != psmSubmesh.m_triangleCount)
				{
					throw std::exception("Invalid triangle count!");
				}

				psmModel.m_submeshes.push_back(psmSubmesh);
			}

			// Write PSM.
			FILE* f = nullptr;
			fopen_s(&f, options.m_outputFilename.c_str(), "wb");

			if (!f)
			{
				throw std::exception("Cannot open output filename for writing!");
			}

			std::cout << "Writing to " << options.m_outputFilename.c_str() << std::endl;

			PSMHelpers::WriteHeader(f, static_cast<uint8_t>(psmModel.m_submeshes.size()));

			uint32_t submeshIndex = 0;
			for (auto const& submesh : psmModel.m_submeshes)
			{
				std::cout << "Writing submesh " << submeshIndex << ". Triangle count: " << submesh.m_triangleCount << std::endl;

				PSMHelpers::WriteSubmeshHeader(f, submesh);

				for (auto const& triangle : submesh.m_triangles)
				{
					VertexData const* vtx = triangle.m_vertexData;
					
					// vertex color
					PSMHelpers::WriteColor(f, vtx[0].m_color);
					PSMHelpers::WriteColor(f, vtx[1].m_color);
					PSMHelpers::WriteColor(f, vtx[2].m_color);
					// texcoord
					PSMHelpers::WriteTexcoord(f, vtx[0].m_texcoord);
					PSMHelpers::WriteTexcoord(f, vtx[1].m_texcoord);
					PSMHelpers::WriteTexcoord(f, vtx[2].m_texcoord);
					PSMHelpers::WriteTexcoord(f, 0u, 0u); // padding
					// vertex position
					PSMHelpers::WritePosition(f, vtx[0].m_position);
					PSMHelpers::WritePosition(f, vtx[1].m_position);
					PSMHelpers::WritePosition(f, vtx[2].m_position);
					PSMHelpers::WritePosition(f, 0u, 0u, 0u);
					// vertex normal
					PSMHelpers::WriteNormal(f, vtx[0].m_normal);
					PSMHelpers::WriteNormal(f, vtx[1].m_normal);
					PSMHelpers::WriteNormal(f, vtx[2].m_normal);
					PSMHelpers::WriteNormal(f, 0u, 0u, 0u); // padding
				}

				++submeshIndex;
			}

			fclose(f);
			f = nullptr;
		}
	}
	catch (std::exception& exc)
	{
		std::cout << exc.what() << std::endl;
	}

	return 0;
}