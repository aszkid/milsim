#include "sys/alexandria.hpp"
#include "util/io.hpp"

#include <glm/ext.hpp>

using namespace MilSim;

////////////////////////////////////////
// FONTASSET
////////////////////////////////////////
FontAsset::FontAsset()
	: Asset::Asset("FontAsset")
{}
FontAsset::~FontAsset()
{}

bool FontAsset::load()
{
	return true;
}

////////////////////////////////////////
// MODELASSET
////////////////////////////////////////
ModelAsset::ModelAsset(std::vector<Mesh> meshes)
	: Asset::Asset("ModelAsset"), m_meshes(meshes)
{

}
ModelAsset::~ModelAsset()
{
	
}
bool ModelAsset::load()
{
	if(m_loaded)
		return true;

	for(auto& m : m_meshes) {
		glGenVertexArrays(1, &m.m_vao);
		glGenBuffers(1, &m.m_vbo);
		
		glBindVertexArray(m.m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m.m_vbo);

		// upload vertex data
		glBufferData(
			GL_ARRAY_BUFFER,
			m.m_verts.size() * sizeof(Vertex),
			&m.m_verts[0],
			GL_STATIC_DRAW
		);
		
		// declare vertex structure
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3,
			GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (void*)0
		);
		glVertexAttribPointer(1, 3,
			GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (void*)offsetof(Vertex, m_normal)
		);
	}

	m_loaded = true;
	return true;
}

////////////////////////////////////////
// SHADERPROGRAMASSET
////////////////////////////////////////
ShaderProgramAsset::ShaderProgramAsset(const std::string vert_source, const std::string frag_source)
	: Asset::Asset("ShaderProgramAsset"), m_vert_source(vert_source), m_frag_source(frag_source)
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
	
	int ok;
	char info[512];

	m_vert_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(m_vert_id, 1, &vert, NULL);
	glCompileShader(m_vert_id);
	glGetShaderiv(m_vert_id, GL_COMPILE_STATUS, &ok);
	if(!ok) {
		glGetShaderInfoLog(m_vert_id, 512, NULL, info);
		m_log->error("Vertex shader compilation failed: {}", info);
	}

	m_frag_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(m_frag_id, 1, &frag, NULL);
	glCompileShader(m_frag_id);
	glGetShaderiv(m_frag_id, GL_COMPILE_STATUS, &ok);
	if(!ok) {
		glGetShaderInfoLog(m_frag_id, 512, NULL, info);
		m_log->error("Fragment shader compilation failed: {}", info);
	}

	// Link the shader program
	m_prog_id = glCreateProgram();
	glAttachShader(m_prog_id, m_vert_id);
	glAttachShader(m_prog_id, m_frag_id);
	glLinkProgram(m_prog_id);
	glGetProgramiv(m_prog_id, GL_LINK_STATUS, &ok);
	if(!ok) {
		glGetProgramInfoLog(m_prog_id, 512, NULL, info);
		m_log->error("Shader program failed to link: {}", info);
	}

	m_loaded = true;

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
	}

	return true;
}

////////////////////////////////////////
// SCRIPTASSET
////////////////////////////////////////
ScriptAsset::ScriptAsset()
	: Asset::Asset("ScriptAsset")
{
	
}
ScriptAsset::~ScriptAsset()
{

}

bool ScriptAsset::load()
{
	return true;
}


////////////////////////////////////////
// ALEXANDRIA
////////////////////////////////////////
Alexandria::Alexandria(const std::string local_root)
	: Sys::Sys("Alexandria"), m_local_root(local_root)
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
	// TODO: use JSON instead of Lua for database loading... makes more sense

	const std::string dbpath = m_local_root + "/" + filename;
	m_log->info("Loading database `{}`...", filename);

	sel::State lua;
	lua.Load(dbpath);

	const std::string type = lua["db_type"];
	const std::string name = lua["db_name"];
	std::string id_root = "/" + name + "/";

	// dont forget to normalize paths...

	if(type == "local") {
		sel::Selector data = lua["data"];
		load_folder(data, id_root, name);
	} else {
		m_log->error("We don't support databases of type `{}` yet!", type);
	}
}
void Alexandria::load_folder(const sel::Selector& root, const std::string old_id, const std::string db_name)
{
	uint i = 1;
	sel::Selector node = root[i];

	while(node.exists()) {
		const std::string sub_id = node["id"];
		if(node["type"] == "folder") {
			load_folder(node["contents"], old_id + sub_id + "/", db_name);
		}
		else {
			add_asset(old_id + sub_id, node["type"], node, db_name);
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
	return get_asset(Crypto::HASH(id));
}
void Alexandria::add_asset(const std::string id, const std::string type, sel::Selector& root, const std::string db_name)
{
	auto hash = Crypto::HASH(id);
	const std::string path = m_local_root + id;

	if(m_assets.find(hash) != m_assets.end()) {
		m_log->info("Asset `{}` already in our database, skipping.", id);
		return;
	}

	if(type == "font") {
		m_log->info("Adding font `{}`...", id);
		m_assets[hash] = t_asset_ptr(new FontAsset());

	} else if(type == "shader") {
		m_log->info("Adding shader `{}`...", id);

		// TODO: catch exceptions...
		auto vert_source = IO::read_file(path + ".vert");
		auto frag_source = IO::read_file(path + ".frag");

		m_assets[hash] = t_asset_ptr(new ShaderProgramAsset(
			vert_source, frag_source
		));
		m_assets[hash]->set_hash(hash);

		// this is not optimal; define some sort of loading policy
		/*if(!m_assets[hash]->load()) {
			m_log->error("Failed to compile shader `{}`!", id);
		}*/

	} else if(type == "model") {
		m_log->info("Adding model `{}`...", id);

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str());
		if(!err.empty()) {
			m_log->info("Warning: {}", err);
		}
		if(!ret) {
			m_log->error("Failed to log model {}!", id);
			return ;
		}

		Mesh mesh;
		// loop shapes
		for(size_t s = 0; s < shapes.size(); s++) {
			// loop faces
			size_t index_off = 0;
			for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				// loop vertices
				int fv = shapes[s].mesh.num_face_vertices[f];
				for(size_t v = 0; v < fv; v++) {
					tinyobj::index_t idx = shapes[s].mesh.indices[index_off + v];
					tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
					tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
					tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
					tinyobj::real_t nx = attrib.vertices[3*idx.normal_index+0];
					tinyobj::real_t ny = attrib.vertices[3*idx.normal_index+1];
					tinyobj::real_t nz = attrib.vertices[3*idx.normal_index+2];
					tinyobj::real_t tx = attrib.vertices[2*idx.texcoord_index+0];
					tinyobj::real_t ty = attrib.vertices[2*idx.texcoord_index+1];
					mesh.m_verts.emplace_back(glm::vec3(vx, vy, vz), glm::vec3(nx, ny, nz));
				}
				index_off += fv;
			}
		}
		m_assets[hash] = t_asset_ptr(new ModelAsset({mesh}));
		m_assets[hash]->set_hash(hash);
	}
}