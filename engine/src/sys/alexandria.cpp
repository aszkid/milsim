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
TextureAsset::TextureAsset(const std::string name)
	: Asset::Asset("TextureAsset." + name)
{

}
TextureAsset::~TextureAsset()
{
	if(m_loaded) {
		stbi_image_free(m_data);
	}
}
bool TextureAsset::inner_load()
{
	/*glGenTextures(1, &m_tex_id);
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
	glGenerateMipmap(GL_TEXTURE_2D);*/

	return true;
}
void TextureAsset::inner_free()
{
	//glDeleteTextures(1, &m_tex_id);
}


////////////////////////////////////////
// MODELASSET
////////////////////////////////////////
ModelAsset::ModelAsset(const std::string name)
	: Asset::Asset("ModelAsset." + name), m_texture(0)
{
}
ModelAsset::~ModelAsset()
{
	
}
bool ModelAsset::inner_load()
{
	/*for(auto& m : m_meshes) {
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
	}*/

	return true;
}
void ModelAsset::inner_free()
{
	/*for(auto& m : m_meshes) {
		glDeleteBuffers(1, &m.m_vbo);
		glDeleteVertexArrays(1, &m.m_vao);
	}*/
	m_meshes.clear();
}

////////////////////////////////////////
// SHADERPROGRAMASSET
////////////////////////////////////////
ShaderProgramAsset::ShaderProgramAsset(const std::string name, const std::string vert_source, const std::string frag_source)
	: Asset::Asset("ShaderProgramAsset." + name), m_vert_source(vert_source), m_frag_source(frag_source)
{

}
ShaderProgramAsset::~ShaderProgramAsset()
{

}

bool ShaderProgramAsset::inner_load()
{
	/*const char *vert = m_vert_source.c_str();
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
	}*/

	return true;
}
void ShaderProgramAsset::inner_free()
{
	//glDeleteShader(m_prog_id);
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
Alexandria::Alexandria(const std::string local_root, Render* render)
	: Sys::Sys("Alexandria"), m_local_root(local_root), m_sys_render(render)
{

}
Alexandria::~Alexandria()
{
	for(auto& a : m_assets) {
		a.second->free();
	}
	m_assets.clear();
}

void Alexandria::init()
{
	m_hermes->subscribe(Crypto::HASH("Alexandria"), {
		Crypto::HASH("RenderResource")
	});
}
void Alexandria::kill()
{

}

void Alexandria::update()
{
	for(auto& msg : m_hermes->pull_inbox(Crypto::HASH("Alexandria"))) {
		if(msg->m_chan == Crypto::HASH("RenderResource")) {
			auto rrm = static_cast<RenderResourceMessage*>(msg);
			// use base class `RenderableAsset` for all assets that need render representation...
			if(rrm->m_instance.m_type == RenderResource::TEXTURE) {
				auto tex = (TextureAsset*)get_asset(rrm->m_creator, NO_LOAD);
				tex->m_handle = rrm->m_instance;
				m_log->info("Populated texture with render resource handle {:x}!", tex->m_handle);
			}
		}
	}

	for(auto it = m_rrc_pool.begin(); it != m_rrc_pool.end(); ) {
		if((*it)->m_delete.load()) {
			// slow, use a `free_rrc` vector (or similar)
			m_rrc_pool.erase(it);
		} else {
			++it;
		}
	}
}

void Alexandria::load_database(const std::string filename)
{
	apathy::Path dbpath(m_local_root);
	dbpath.append(filename).sanitize();
	m_log->info("Loading database `{}`...", filename);

	json db = json::parse(IO::read_file(dbpath.string()));

	const std::string dbtype = db["db_type"];
	const std::string dbname = db["db_name"];
	apathy::Path id_root("/" + dbname + "/");

	// dont forget to normalize paths...

	if(dbtype == "local") {
		const json& data = db["data"];
		load_folder(data, id_root, dbname);
	} else {
		m_log->error("We don't support databases of type `{}` yet!", dbtype);
		abort();
	}

	m_log->info("Finished loading database!");
}
void Alexandria::load_folder(const json& root, apathy::Path path, const std::string db_name)
{
	//m_log->info("Loading folder `{}`", path.string());

	for(auto& node : root) {
		const std::string new_id = node["id"];

		if(node["type"] == "folder") {
			load_folder(node["contents"], apathy::Path(path).append(new_id).directory(), db_name);
		}
		else {
			add_asset(apathy::Path(path).append(new_id), node["type"], &node, db_name);
		}
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
void Alexandria::add_asset(apathy::Path path, const std::string type, const json* root, const std::string db_name)
{
	const std::string id = path.sanitize().string();
	const std::string stem = path.stem().string();
	auto hash = Crypto::HASH(id);
	const std::string short_id = (root == nullptr) ? path.filename() : root->at("id").get<std::string>();

	m_log->info("Adding asset `{}` of type `{}` -- `{:x}`...", id, type, hash);

	if(m_assets.find(hash) != m_assets.end()) {
		m_log->info("Asset `{}` already in our database, skipping.", id);
		return;
	}

	apathy::Path working_path(m_local_root);
	working_path.append(path).sanitize();

	if(type == "font") {
		//m_assets[hash] = t_asset_ptr(new FontAsset());

	} else if(type == "shader") {
		// TODO: catch exceptions...
		auto vert_source = IO::read_file(apathy::Path(working_path).string() + ".vert");
		auto frag_source = IO::read_file(apathy::Path(working_path).string() + ".frag");

		place_asset(hash, new ShaderProgramAsset(
			short_id, vert_source, frag_source
		));

	} else if(type == "model") {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;

		const std::string file = (*root)["source"];

		bool ret = tinyobj::LoadObj(
			&attrib, &shapes, &materials, &err,
			apathy::Path(working_path).append(file).string().c_str(),
			apathy::Path(working_path).directory().string().c_str()
		);
		if(!err.empty()) {
			m_log->info("Warning: {}", err);
		}
		if(!ret) {
			m_log->error("Failed to load model {}!", id);
			abort();
		}

		// last texture used
		t_asset_id tex = 0;

		// loop materials
		for(size_t m = 0; m < materials.size(); m++) {
			// gotcha! create a `Texture` asset; we could even do `Material` assets
			// on top of that.
			apathy::Path tex_id(path);
			tex_id = tex_id.append(materials[m].diffuse_texname);
			add_asset(tex_id, "texture", nullptr, db_name);
			tex = Crypto::HASH(tex_id.sanitize().string());
			// force texture load for now
			get_asset(tex);
		}
		
		// Prepare asset -- TODO: pool this allocation? let `Alexandria` manage it
		ModelAsset* model = static_cast<ModelAsset*>(place_asset(hash, new ModelAsset(short_id)));
		model->m_meshes.push_back({});
		Mesh& mesh = model->m_meshes.back();

		// Memory copying -- pretty fast
		/*const size_t realsize = sizeof(tinyobj::real_t);

		const size_t len_v = attrib.vertices.size() / 3;
		const size_t bytes_v = attrib.vertices.size() * realsize;
		mesh.m_position.resize(len_v);
		memcpy(&mesh.m_position[0], &attrib.vertices[0], bytes_v);

		const size_t len_vn = attrib.normals.size() / 3;
		const size_t bytes_vn = attrib.normals.size() * realsize;
		mesh.m_normal.resize(len_vn);
		memcpy(&mesh.m_normal[0], &attrib.normals[0], bytes_vn);

		const size_t len_tc = attrib.texcoords.size() / 2;
		const size_t bytes_tc = attrib.texcoords.size() * realsize;
		if(len_tc > 0) {
			// assume that we have texture coordinates
			mesh.m_texture.resize(len_tc);
			memcpy(&mesh.m_texture[0], &attrib.texcoords[0], bytes_tc);
		}*/

		// Get size info
		const bool has_tex = attrib.texcoords.size() > 0;
		const size_t type_size = sizeof(tinyobj::real_t);
		const size_t vertices = attrib.vertices.size();
		const size_t vertices_byte = vertices * type_size;
		const size_t normals = attrib.normals.size();
		const size_t normals_byte = normals * type_size;
		const size_t texcoords = attrib.texcoords.size();
		const size_t texcoords_byte = texcoords * type_size;
		const size_t total_size = vertices_byte + normals_byte + texcoords_byte;

		// Allocate big chunk
		mesh.m_data = std::unique_ptr<unsigned char[]>(new unsigned char[total_size]);
		const unsigned char* vertex_ptr = &mesh.m_data.get()[0];
		const unsigned char* normal_ptr = vertex_ptr + vertices_byte;
		const unsigned char* texcoord_ptr = normal_ptr + normals_byte;

		// Copy vertex data
		memcpy((void*)vertex_ptr, &attrib.vertices[0], vertices_byte);
		memcpy((void*)normal_ptr, &attrib.normals[0], normals_byte);
		memcpy((void*)texcoord_ptr, &attrib.texcoords[0], texcoords_byte);

		// Pass in the texture loaded (only one for now)
		model->m_texture = tex;

		// Upload to the GPU
		RenderResourceContext* rrc = alloc_rrc();
		RenderResourceContext::VertexBufferData buff = {
			total_size, &mesh.m_data.get()[0], RenderResourceContext::VertexBufferData::STATIC
		};
		rrc->push_vertex_buffer(buff, hash);
		// push attributes
		/**
		 * RRC::VertexLayoutData layout;
		 * layout->push_attribute();
		 * 
		 * 
		 */

		m_log->info("Dispatching render-buffer message...");
		m_sys_render->dispatch(rrc);

	} else if(type == "texture") {
		TextureAsset* texture = static_cast<TextureAsset*>(place_asset(hash, new TextureAsset(short_id)));
		stbi_set_flip_vertically_on_load(true);
		texture->m_data = stbi_load(working_path.string().c_str(), &texture->m_width, &texture->m_height, &texture->m_channels, 0);

		// Upload to the GPU
		RenderResourceContext* rrc = alloc_rrc();
		RenderResourceContext::TextureData tex = {
			texture->m_width,
			texture->m_height,
			texture->m_channels,
			texture->m_data
		};
		rrc->push_texture(tex, hash);

		m_log->info("Dispatching render-resource message...");
		m_sys_render->dispatch(rrc);
	}
	
	//m_assets[hash]->set_path(path); ??
}

RenderResourceContext* Alexandria::alloc_rrc()
{
	m_rrc_pool.emplace_back(new RenderResourceContext());
	auto* rrc = m_rrc_pool.back().get();
	rrc->m_delete.store(false);
	return m_rrc_pool.back().get();
}