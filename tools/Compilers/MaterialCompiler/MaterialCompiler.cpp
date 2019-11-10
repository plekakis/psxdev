// MaterialCompiler.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thirdparty/cxxopts/cxxopts.hpp>
#include <thirdparty/tinyxml2/tinyxml2.h>

using namespace tinyxml2;

static const std::string s_input = "input";
static const std::string s_output = "output";

struct MaterialCompileOptions
{
	std::string  m_inputFilename;
	std::string  m_outputFilename;
};

namespace MATHelpers
{
	///////////////////////////////////////////////////
	// This must match the engine's hashing function.
	// engine/base/utils/utils.h
	///////////////////////////////////////////////////
	uint16_t HashString(const char* i_string)
	{
		// djb2 implementation of string hashing
		// http://www.cse.yorku.ca/~oz/hash.html
		// Adopted for 16bit hashing

		uint32_t hash = 5381u;
		char c;
		while (c = *i_string++)
		{
			hash = ((hash << 5u) + hash) + c;
		}

		return (hash ^ (hash >> 16)) & 0xffff;
	}

	///////////////////////////////////////////////////
	void WriteMatInstance(FILE* f, uint16_t name, uint16_t texture, uint8_t type, uint8_t r, uint8_t g, uint8_t b, uint8_t flags)
	{
		uint8_t pad = 0u;
		fwrite(&name, sizeof(uint16_t), 1, f);
		fwrite(&texture, sizeof(uint16_t), 1, f);
		fwrite(&type, sizeof(uint8_t), 1, f);
		fwrite(&r, sizeof(uint8_t), 1, f);
		fwrite(&g, sizeof(uint8_t), 1, f);
		fwrite(&b, sizeof(uint8_t), 1, f);
		fwrite(&flags, sizeof(uint8_t), 1, f);
		fwrite(&pad, sizeof(uint8_t), 3, f);
	}

	///////////////////////////////////////////////////
	void WriteMeshRef(FILE* f, uint16_t filename, uint8_t submeshIndex, uint16_t matIndex)
	{
		uint8_t pad = 0u;
		fwrite(&matIndex, sizeof(uint16_t), 1, f);
		fwrite(&filename, sizeof(uint16_t), 1, f);
		fwrite(&submeshIndex, sizeof(uint8_t), 1, f);
		fwrite(&pad, sizeof(uint8_t), 3, f);
	}
}

///////////////////////////////////////////////////
// Parse commandline options.
///////////////////////////////////////////////////
bool parse(int argc, char* argv[], MaterialCompileOptions& compileOptions)
{
	cxxopts::Options options("MaterialCompiler", "Writes the MATLIB.MAT file out of a material.xml");
	options.add_options()
		("i,input", "Input filename (.xml)", cxxopts::value<std::string>())
		("o,output", "Output filename (.mat)", cxxopts::value<std::string>())
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
	return true;
}

///////////////////////////////////////////////////
// Represents a material.
struct Material
{
	std::string m_name;
	std::string m_texture;
	uint32_t	m_type;
	uint32_t	m_red;
	uint32_t	m_green;
	uint32_t	m_blue;
};

///////////////////////////////////////////////////
// Links a material to a specific submesh.
struct MaterialRef
{
	uint32_t		m_submeshIndex;
	std::string		m_materialName;
};

///////////////////////////////////////////////////
// Represents a material/model link. A link can have as many materials as submeshes (but can skip submeshes if no material is required).
struct MaterialLink
{
	std::string m_modelName;
	std::vector<MaterialRef> m_refs;
};

///////////////////////////////////////////////////
void Validate(std::vector<Material> const& i_materials, std::vector<MaterialLink> const& i_links)
{
	// Material validation.
	if (i_materials.size() > std::numeric_limits<uint16_t>().max())
	{
		throw std::exception("Too many materials!");
	}

	for (auto const& mat : i_materials)
	{
		// Material validation.
		if (mat.m_type > 3)
		{
			throw std::exception("Material specifying unsupported vertex type!");
		}

		if (mat.m_red > 255 || mat.m_green > 255 || mat.m_blue > 255)
		{
			throw std::exception("Material specifying invalid color!");
		}

		if (mat.m_name == "")
		{
			throw std::exception("Material specified no name!");
		}
	}

	// Links validation.
	for (auto const& link : i_links)
	{
		for (auto const& ref : link.m_refs)
		{
			if (ref.m_submeshIndex > std::numeric_limits<uint8_t>().max())
			{
				throw std::exception("Models can have up to 255 submeshes, material ref is referencing higher!");
			}

			if (ref.m_materialName == "")
			{
				throw std::exception("Material ref specified no material name!");
			}

			// See if the material with this name exists
			bool found = false;
			for (auto const& mat : i_materials)
			{
				if (ref.m_materialName == mat.m_name)
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				throw std::exception("Material ref specified unknown material name!");
			}
		}
	}
}

///////////////////////////////////////////////////
void PopulateMaterials(XMLDocument* const i_xml, std::vector<Material>& io_materials)
{
	XMLElement* const xmlRoot = i_xml->FirstChildElement("material_library");
	XMLElement* const materialsRoot = xmlRoot->FirstChildElement("materials");

	if (materialsRoot)
	{
		XMLElement* child = materialsRoot->FirstChildElement();
		if (child != nullptr)
		{
			do
			{
				Material mat;

				XMLElement* const name = child->FirstChildElement("name");
				XMLElement* const texture = child->FirstChildElement("texture");
				XMLElement* const type = child->FirstChildElement("type");
				XMLElement* const red = child->FirstChildElement("red");
				XMLElement* const green = child->FirstChildElement("green");
				XMLElement* const blue = child->FirstChildElement("blue");
				
				// Update material data.
				mat.m_name = name ? name->GetText() : "";
				mat.m_texture = texture ? texture->GetText() : "";
				mat.m_type = type ? type->IntText() : ~0u;
				mat.m_red = red ? red->IntText() : 255;
				mat.m_green = green ? green->IntText() : 255;
				mat.m_blue = blue ? blue->IntText() : 255;

				// All good, carry on.
				io_materials.push_back(mat);
			} while (child = child->NextSiblingElement());
		}
	}
}

///////////////////////////////////////////////////
void PopulateMaterialLinks(XMLDocument* const i_xml, std::vector<MaterialLink>& io_links)
{
	XMLElement* const xmlRoot = i_xml->FirstChildElement("material_library");
	XMLElement* const linksRoot = xmlRoot->FirstChildElement("links");
	if (linksRoot)
	{
		XMLElement* child = linksRoot->FirstChildElement();
		if (child != nullptr)
		{
			do
			{
				MaterialLink link;

				XMLAttribute const* model = child->FindAttribute("model");
				link.m_modelName = model ? model->Value() : ""; // model name

				XMLElement* ref = child->FirstChildElement("ref");
				if (!ref)
				{
					throw std::exception("Material link provided without any references!");
				}

				// Go through all references for submeshes.
				do
				{
					// Extract & validate.
					XMLAttribute const* submeshIndex = ref->FindAttribute("index");					
					XMLAttribute const* materialName = ref->FindAttribute("material");
					
					MaterialRef mr;
					mr.m_materialName = materialName ? materialName->Value() : "";
					mr.m_submeshIndex = submeshIndex ? submeshIndex->IntValue() : ~0u;

					// Proceed.
					link.m_refs.push_back(mr);
					
				} while (ref = ref->NextSiblingElement());

				io_links.push_back(link);
			} while (child = child->NextSiblingElement());
		}
	}
}

///////////////////////////////////////////////////
void WriteMAT(std::string const& i_filename, std::vector<Material>& i_materials, std::vector<MaterialLink>& i_links)
{
	FILE* f = nullptr;
	fopen_s(&f, i_filename.c_str(), "wb");
	
	if (!f)
	{
		throw std::exception("Cannot open output filename for writing!");
	}

	// count refs
	uint16_t matRefCount = 0u;
	for (auto const& link : i_links)
	{
		for (auto const& ref : link.m_refs)
		{
			++matRefCount;
		}
	}

	uint32_t const header = 0x150385BA;
	uint16_t const matCount = static_cast<uint16_t>(i_materials.size());
	uint16_t const meshCount = matRefCount;

	fwrite(&header, sizeof(uint32_t), 1, f);
	fwrite(&matCount, sizeof(uint16_t), 1, f);
	fwrite(&meshCount, sizeof(uint16_t), 1, f);

	for (auto const& mat : i_materials)
	{
		std::cout << "Writing material " << mat.m_name.c_str() << std::endl;

		MATHelpers::WriteMatInstance
		(
			f,
			MATHelpers::HashString(mat.m_name.c_str()),
			MATHelpers::HashString(mat.m_texture.c_str()),
			static_cast<uint8_t>(mat.m_type),
			static_cast<uint8_t>(mat.m_red),
			static_cast<uint8_t>(mat.m_green),
			static_cast<uint8_t>(mat.m_blue),
			0u
		);
	}

	for (auto const& link : i_links)
	{
		for (auto const& ref : link.m_refs)
		{
			// find material index
			uint16_t materialIndex = 0u;
			bool found = false;
			for (size_t i=0; i<i_materials.size(); ++i)
			{
				if (i_materials[i].m_name == ref.m_materialName)
				{
					materialIndex = static_cast<uint16_t>(i);
					found = true;
					break;
				}
			}

			if (!found) 
			{
				throw std::exception("Material name in link ref not found!");
			}

			std::cout << "Writing material link for model name " << link.m_modelName.c_str() << ", submesh index " << ref.m_submeshIndex << " and material name " << ref.m_materialName.c_str() << std::endl;

			MATHelpers::WriteMeshRef
			(
				f,
				MATHelpers::HashString(link.m_modelName.c_str()),
				static_cast<uint8_t>(ref.m_submeshIndex),
				materialIndex
			);
		}
	}
}

///////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	MaterialCompileOptions options;
	try
	{
		if (parse(argc, argv, options))
		{
			XMLDocument file;
			XMLError const error = file.LoadFile(options.m_inputFilename.c_str());
			switch (error)
			{
			case XML_ERROR_FILE_COULD_NOT_BE_OPENED:
			case XML_ERROR_FILE_NOT_FOUND:
			case XML_ERROR_FILE_READ_ERROR:
				throw std::exception("XML: File I/O error.");
				break;
			case XML_ERROR_PARSING_ELEMENT:
			case XML_ERROR_PARSING_ATTRIBUTE:
			case XML_ERROR_PARSING_TEXT:
			case XML_ERROR_PARSING_CDATA:
			case XML_ERROR_PARSING_COMMENT:
			case XML_ERROR_PARSING_DECLARATION:
			case XML_ERROR_PARSING_UNKNOWN:
				throw std::exception("XML: Structural error.");
				break;
			default:
				break;
			}

			// Starting parsing materials.
			std::vector<Material> materials;
			PopulateMaterials(&file, materials);

			// Start parsing material links.
			std::vector<MaterialLink> links;
			PopulateMaterialLinks(&file, links);

			// Validate links against materials.
			Validate(materials, links);

			// Convert to .MAT and write to destination
			WriteMAT(options.m_outputFilename, materials, links);
		}
	}
	catch (std::exception& exc)
	{
		std::cout << exc.what() << std::endl;
	}

	return 0;
}
