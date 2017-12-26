#include "assets.hpp"

using namespace MilSim;

////////////////////////////////////////
// ASSET BASE CLASS
////////////////////////////////////////
Asset::Type Asset::str_to_type(const std::string t)
{
	if(t == "font") {
		return FONT;
	} else if(t == "texture") {
		return TEXTURE;
	} else if(t == "material") {
		return MATERIAL;
	} else if(t == "mesh") {
		return MESH;
	} else if(t == "model") {
		return MODEL;
	} else if(t == "shader") {
		return SHADER;
	} else if(t == "script") {
		return SCRIPT;
	} else if(t == "map") {
		return MAP;
	} else {
		return NONE;
	}
}
std::string Asset::type_to_str(const Type t)
{
	switch(t) {
	case FONT:
		return "font";
	case TEXTURE:
		return "texture";
	case MATERIAL:
		return "material";
	case MESH:
		return "mesh";
	case MODEL:
		return "model";
	case SHADER:
		return "shader";
	case SCRIPT:
		return "script";
	case MAP:
		return "map";
	default:
		return "unknown";
	}
}
std::string Asset::database() const {
	return m_id.split()[1].segment;
}

////////////////////////////////////////
// FONTASSET
////////////////////////////////////////
FontAsset::FontAsset()
	: Asset::Asset(apathy::Path(), "FontAsset", Type::FONT)
{}
FontAsset::~FontAsset()
{}


////////////////////////////////////////
// TEXTUREASSET
////////////////////////////////////////
TextureAsset::TextureAsset(const apathy::Path id)
	: Asset::Asset(id, "Texture", Type::TEXTURE)
{
	m_handle = {0};

	m_data = nullptr;
	m_width = 0;
	m_height = 0;
	m_channels = 0;
}
TextureAsset::~TextureAsset()
{

}

////////////////////////////////////////
// MATERIALASSET
////////////////////////////////////////
MaterialAsset::MaterialAsset(const apathy::Path id)
	: Asset::Asset(id, "Material", Type::MATERIAL)
{
	m_diffuse_tex = 0;
	m_specular_tex = 0;
}
MaterialAsset::~MaterialAsset()
{

}

////////////////////////////////////////
// MODELASSET
////////////////////////////////////////
MeshAsset::MeshAsset(const apathy::Path id)
	: Asset::Asset(id, "Mesh", Type::MESH)
{
	
}
MeshAsset::~MeshAsset()
{

}

////////////////////////////////////////
// MODELASSET
////////////////////////////////////////
ModelAsset::ModelAsset(const apathy::Path id)
	: Asset::Asset(id, "Model", Type::MODEL)
{
}
ModelAsset::~ModelAsset()
{
	
}

////////////////////////////////////////
// SHADERPROGRAMASSET
////////////////////////////////////////
ShaderProgramAsset::ShaderProgramAsset(const apathy::Path id)
	: Asset::Asset(id, "ShaderProgram", Type::SHADER)
{

}
ShaderProgramAsset::~ShaderProgramAsset()
{

}

//bool ShaderProgramAsset::inner_load()
//{
	/*const char *vert = m_vert_source.c_str();
	const char *frag = m_frag_source.c_str();

	// ...

	// Lookup uniforms
	GLint num_uniforms = 0;
	glGetProgramInterfaceiv(m_prog_id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &num_uniforms);
	static const GLenum props[4] = {
		GL_BLOCK_INDEX,
		GL_TYPE,
		GL_NAME_LENGTH,
		GL_LOCATION
	};
	for(int i = 0; i < num_uniforms; i++) {
		GLint values[4];
		glGetProgramResourceiv(m_prog_id, GL_UNIFORM, i, 4, props, 4, NULL, values);
		// skip block uniforms
		if(values[0] != -1)
			continue;
		std::string name(values[2], ' ');
		glGetProgramResourceName(m_prog_id, GL_UNIFORM, i, name.size(), NULL, &name[0]);
		name.pop_back(); // pop null character
		m_uniforms[name] = values[3];
	}*/

//	return true;
//}

////////////////////////////////////////
// SCRIPTASSET
////////////////////////////////////////
ScriptAsset::ScriptAsset()
	: Asset::Asset(apathy::Path(), "ScriptAsset", Type::SCRIPT)
{
	
}
ScriptAsset::~ScriptAsset()
{

}

////////////////////////////////////////
// MAPASSET
////////////////////////////////////////
MapAsset::MapAsset(const apathy::Path id)
	: Asset::Asset(id, "MapAsset", Type::MAP)
{}
MapAsset::~MapAsset()
{}