#include "sys/alexandria.hpp"

#include "util/io.hpp"

using namespace MilSim;

////////////////////////////////////////
// FONTASSET
////////////////////////////////////////
FontAsset::FontAsset()
	: Asset::Asset()
{}
FontAsset::~FontAsset()
{}

bool FontAsset::load()
{

}

////////////////////////////////////////
// SHADERPROGRAMASSET
////////////////////////////////////////
ShaderProgramAsset::ShaderProgramAsset(const std::string vert_source, const std::string frag_source)
	: m_vert_source(vert_source), m_frag_source(frag_source)
{

}
ShaderProgramAsset::~ShaderProgramAsset()
{
	glDeleteShader(m_vert_id);
	glDeleteShader(m_prog_id);
}

bool ShaderProgramAsset::load()
{
	const char *vert = m_vert_source.c_str();
	const char *frag = m_frag_source.c_str();
	
	m_vert_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(m_vert_id, 1, &vert, NULL);
	glCompileShader(m_vert_id);
	int ok;
	char info[512];
	glGetShaderiv(m_vert_id, GL_COMPILE_STATUS, &ok);
	if(!ok) {
		glGetShaderInfoLog(m_vert_id, 512, NULL, info);
		spdlog::get("core")->error("Vertex shader compilation failed: {}", info);
	}

	m_frag_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(m_frag_id, 1, &frag, NULL);
	glCompileShader(m_frag_id);

	// Link the shader program
	m_prog_id = glCreateProgram();
	glAttachShader(m_prog_id, m_vert_id);
	glAttachShader(m_prog_id, m_frag_id);
	glLinkProgram(m_prog_id);

	m_loaded = true;

	return true;
}

////////////////////////////////////////
// SCRIPTASSET
////////////////////////////////////////
ScriptAsset::ScriptAsset()
{
	
}
ScriptAsset::~ScriptAsset()
{

}

bool ScriptAsset::load()
{

}


////////////////////////////////////////
// ALEXANDRIA
////////////////////////////////////////
Alexandria::Alexandria()
	: Sys::Sys("alexandria")
{

}
Alexandria::~Alexandria()
{

}

void Alexandria::init()
{

}
void Alexandria::kill()
{

}

void Alexandria::update()
{
	
}

void Alexandria::load_database(const std::string filename)
{
	sel::State lua;
	lua.Load(filename);

	const std::string type = lua["db_type"];
	const std::string name = lua["db_name"];
	std::string id_root = "/" + name + "/";

	// dont forget to normalize paths...

	if(type == "local") {
		sel::Selector data = lua["data"];
		load_folder(data, id_root);
	} else {
		m_log->error("We don't support databases of type `{}` yet!", type);
	}
}
void Alexandria::load_folder(const sel::Selector& root, const std::string old_id)
{
	uint i = 1;
	sel::Selector node = root[i];

	while(node.exists()) {
		const std::string sub_id = node["id"];
		if(node["type"] == "folder") {
			load_folder(node["contents"], old_id + sub_id + "/");
		}
		else {
			add_asset(old_id + sub_id, node["type"], node);
		}
		node = root[++i];
	}
}

Asset* Alexandria::get_asset(const t_asset_id id)
{
	// Find asset
	auto asset = m_assets.find(id);
	if(asset != m_assets.end()) {
		if(asset->second->m_loaded == false)
			asset->second->load();
		return asset->second.get();
	}
	// Asset is not in our database!
	m_log->info("Asset `{}` is not in our database!", id);
	return nullptr;
}
Asset* Alexandria::get_asset(const std::string id)
{
	return get_asset(HASH(id));
}
void Alexandria::add_asset(const std::string id, const std::string type, sel::Selector& root)
{
	auto hash = HASH(id);

	if(m_assets.find(hash) != m_assets.end()) {
		m_log->info("Asset `{}` already in our database, skipping.", id);
		return;
	}

	if(type == "font") {
		m_log->info("Adding font `{}`...", id);
		m_assets[hash] = t_asset_ptr(new FontAsset());

	} else if(type == "shader") {
		m_log->info("Adding shader `{}`...", id);
		std::string vertpath = root["vertex"];
		std::string fragpath = root["fragment"];

		auto vert_source = IO::read_file("/media/data/dev/milsim/run/alexandria" + vertpath);
		auto frag_source = IO::read_file("/media/data/dev/milsim/run/alexandria" + fragpath);
		
		m_assets[hash] = t_asset_ptr(new ShaderProgramAsset(
			vert_source, frag_source
		));


		// this is not optimal; define some sort of loading policy
		if(!m_assets[hash]->load()) {
			m_log->error("Failed to compile shader `{}`!", id);
		}

	}
}