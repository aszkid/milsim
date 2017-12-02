#include "sys/alexandria.hpp"
#include "util/io.hpp"

#include <glm/ext.hpp>
#include <tiny_obj_loader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

using namespace MilSim;

////////////////////////////////////////
// FONTASSET
////////////////////////////////////////
FontAsset::FontAsset()
	: Asset::Asset("FontAsset")
{}
FontAsset::~FontAsset()
{}

bool FontAsset::inner_load()
{
	return true;
}

////////////////////////////////////////
// TEXTUREASSET
////////////////////////////////////////
TextureAsset::TextureAsset(const std::string name, unsigned char* data, int width, int height, int channels)
	: Asset::Asset("TextureAsset." + name), m_data(data), m_width(width), m_height(height), m_channels(channels)
{

}
TextureAsset::~TextureAsset()
{
	stbi_image_free(m_data);
}
bool TextureAsset::inner_load()
{
	glGenTextures(1, &m_tex_id);
	glBindTexture(GL_TEXTURE_2D, m_tex_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if(m_channels == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_data);
	} else if(m_channels == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_data);
	} else {
		m_logger->error("Texture is not RGB nor RGBA! Aborting...");
		abort();
	}
	glGenerateMipmap(GL_TEXTURE_2D);

	return true;
}


////////////////////////////////////////
// MODELASSET
////////////////////////////////////////
ModelAsset::ModelAsset(const std::string name, std::vector<Mesh> meshes)
	: Asset::Asset("ModelAsset." + name), m_meshes(meshes), m_texture(0)
{
}
ModelAsset::~ModelAsset()
{
	
}
bool ModelAsset::inner_load()
{
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
			sizeof(Vertex), (void*)offsetof(Vertex, m_position)
		);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3,
			GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (void*)offsetof(Vertex, m_normal)
		);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2,
			GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (void*)offsetof(Vertex, m_texture)
		);
	}

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
	glDeleteShader(m_prog_id);
}

bool ShaderProgramAsset::inner_load()
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
		m_logger->error("Vertex shader compilation failed: {}", info);
		abort();
	}

	m_frag_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(m_frag_id, 1, &frag, NULL);
	glCompileShader(m_frag_id);
	glGetShaderiv(m_frag_id, GL_COMPILE_STATUS, &ok);
	if(!ok) {
		glGetShaderInfoLog(m_frag_id, 512, NULL, info);
		m_logger->error("Fragment shader compilation failed: {}", info);
		abort();
	}

	// Link the shader program
	m_prog_id = glCreateProgram();
	glAttachShader(m_prog_id, m_vert_id);
	glAttachShader(m_prog_id, m_frag_id);
	glLinkProgram(m_prog_id);
	glGetProgramiv(m_prog_id, GL_LINK_STATUS, &ok);
	if(!ok) {
		glGetProgramInfoLog(m_prog_id, 512, NULL, info);
		m_logger->error("Shader program failed to link: {}", info);
		abort();
	}

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

bool ScriptAsset::inner_load()
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

	apathy::Path dbpath(m_local_root);
	dbpath.append(filename).sanitize();
	m_log->info("Loading database `{}`...", filename);

	sel::State lua;
	lua.Load(dbpath.string());

	const std::string dbtype = lua["db_type"];
	const std::string dbname = lua["db_name"];
	apathy::Path id_root("/" + dbname + "/");

	// dont forget to normalize paths...

	if(dbtype == "local") {
		sel::Selector data = lua["data"];
		load_folder(data, id_root, dbname);
	} else {
		m_log->error("We don't support databases of type `{}` yet!", dbtype);
		abort();
	}

	m_log->info("Finished loading database!");
}
void Alexandria::load_folder(const sel::Selector& root, apathy::Path path, const std::string db_name)
{
	m_log->info("Loading folder `{}`", path.string());

	uint i = 1;
	sel::Selector node = root[i];


	while(node.exists()) {
		const std::string new_id = node["id"];

		if(node["type"] == "folder") {
			load_folder(node["contents"], apathy::Path(path).append(new_id).directory(), db_name);
		}
		else {
			add_asset(apathy::Path(path).append(new_id), node["type"], &node, db_name);
		}
		node = root[++i];
	}
}

Asset* Alexandria::get_asset(const t_asset_id id, const GetFlag flag)
{
	// Find asset
	auto asset = m_assets.find(id);
	if(asset != m_assets.end()) {
		if(asset->second->m_loaded == false && flag == LOAD)
			asset->second->load();
		return asset->second.get();
	}
	// Asset is not in our database!
	m_log->info("Asset `{}` is not in our database!", id);
	return nullptr;
}
Asset* Alexandria::get_asset(const std::string id, const GetFlag flag)
{
	return get_asset(Crypto::HASH(id), flag);
}
void Alexandria::add_asset(apathy::Path path, const std::string type, sel::Selector* root, const std::string db_name)
{
	const std::string id = path.sanitize().string();
	const std::string stem = path.stem().string();
	auto hash = Crypto::HASH(id);
	const std::string short_id = (root == nullptr) ? path.filename() : (*root)["id"];

	m_log->info("Adding asset `{}` of type `{}` -- `{:x}`...", id, type, hash);

	if(m_assets.find(hash) != m_assets.end()) {
		m_log->info("Asset `{}` already in our database, skipping.", id);
		return;
	}

	apathy::Path working_path(m_local_root);
	working_path.append(path).sanitize();

	if(type == "font") {
		m_assets[hash] = t_asset_ptr(new FontAsset());

	} else if(type == "shader") {
		// TODO: catch exceptions...
		auto vert_source = IO::read_file(apathy::Path(working_path).string() + ".vert");
		auto frag_source = IO::read_file(apathy::Path(working_path).string() + ".frag");

		place_asset(hash, new ShaderProgramAsset(
			vert_source, frag_source
		));

	} else if(type == "model") {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;

		bool ret = tinyobj::LoadObj(
			&attrib, &shapes, &materials, &err,
			working_path.string().c_str(),
			apathy::Path(working_path).parent().string().c_str()
		);
		if(!err.empty()) {
			m_log->info("Warning: {}", err);
		}
		if(!ret) {
			m_log->error("Failed to load model {}!", id);
			abort();
		}

		Mesh mesh;
		/*m_log->info("{} vertices", attrib.vertices.size() / 3);
		m_log->info("{} normals", attrib.normals.size() / 3);
		m_log->info("{} materials", materials.size());
		m_log->info("{} shapes", shapes.size());*/

		// last texture used
		t_asset_id tex = 0;

		// loop materials
		for(size_t m = 0; m < materials.size(); m++) {
			// gotcha! create a `Texture` asset; we could even do `Material` assets
			// on top of that.
			apathy::Path tex_id(path);
			tex_id = tex_id.parent().append(materials[m].diffuse_texname);
			add_asset(tex_id, "texture", nullptr, db_name);
			tex = Crypto::HASH(tex_id.sanitize().string());
			// force texture load for now
			get_asset(tex);
		}

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
					tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
					tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
					tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];
					tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
					tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];
					mesh.m_verts.emplace_back(
						glm::vec3(vx, vy, vz),
						glm::vec3(nx, ny, nz),
						glm::vec2(tx, ty)
					);
				}
				index_off += fv;
			}
		}
		//m_assets[hash] = t_asset_ptr(new ModelAsset(short_id, {mesh}));
		place_asset(hash, new ModelAsset(short_id, {mesh}));
		((ModelAsset*)get_asset(hash, NO_LOAD))->m_texture = tex;

	} else if(type == "texture") {
		int width, height, channels;
		unsigned char* data = stbi_load(working_path.string().c_str(), &width, &height, &channels, 0);
		/*m_assets[hash] = t_asset_ptr(new TextureAsset(
			short_id, data, width, height, channels
		));*/
		m_log->info("Texture has {} channels", channels);

		place_asset(hash, new TextureAsset(
			short_id, data, width, height, channels
		));
	}
	
	//m_assets[hash]->set_path(path); ??
}