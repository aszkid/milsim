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

void FontAsset::handle_render_message(RenderResourceMessage* msg)
{

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

}
bool TextureAsset::inner_load()
{
	return true;
}
void TextureAsset::inner_free()
{
	m_logger->info("Freeing texture...");
	stbi_image_free(&m_data);
}

void TextureAsset::handle_render_message(RenderResourceMessage* msg)
{
	
}

////////////////////////////////////////
// MATERIALASSET
////////////////////////////////////////
MaterialAsset::MaterialAsset(const std::string name)
	: Asset::Asset("MaterialAsset." + name)
{

}
MaterialAsset::~MaterialAsset()
{

}

bool MaterialAsset::inner_load()
{
	return true;
}
void MaterialAsset::inner_free()
{

}

void MaterialAsset::handle_render_message(RenderResourceMessage* msg)
{
	
}

////////////////////////////////////////
// MODELASSET
////////////////////////////////////////
MeshAsset::MeshAsset(const std::string name)
	: Asset::Asset("Mesh." + name)
{

}
MeshAsset::~MeshAsset()
{

}

bool MeshAsset::inner_load()
{
	return true;
}
void MeshAsset::inner_free()
{
	m_verts.clear();
}

void MeshAsset::handle_render_message(RenderResourceMessage* msg)
{
	switch(msg->m_instance.m_type) {
	case RenderResource::Type::VERTEX_BUFFER:
		m_vb_handle = msg->m_instance;
		break;
	case RenderResource::Type::INDEX_BUFFER:
		m_ib_handle = msg->m_instance;
		break;
	default:
		break;
	}
}

////////////////////////////////////////
// MODELASSET
////////////////////////////////////////
ModelAsset::ModelAsset(const std::string name)
	: Asset::Asset("ModelAsset." + name)
{
}
ModelAsset::~ModelAsset()
{
	
}
bool ModelAsset::inner_load()
{
	return true;
}
void ModelAsset::inner_free()
{
	m_meshes.clear();
}

void ModelAsset::handle_render_message(RenderResourceMessage* msg)
{
	
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

void ShaderProgramAsset::handle_render_message(RenderResourceMessage* msg)
{
	
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

void ScriptAsset::handle_render_message(RenderResourceMessage* msg)
{
	
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

	// Prepare common vertex layouts
	RenderResourceContext* rrc = alloc_rrc();
	RenderResourceContext::VertexLayoutData layout = {
		.attribs = {
			{
				// Vertex position
				.size =  3,
				.type = RenderResourceContext::VertexLayoutData::Attribute::FLOAT,
				.offset = 0,
			},
			{
				// Normal vector
				.size = 3,
				.type = RenderResourceContext::VertexLayoutData::Attribute::FLOAT,
				.offset = offsetof(Vertex, m_normal)
			},
			{
				// Texture coordinates
				.size = 2,
				.type = RenderResourceContext::VertexLayoutData::Attribute::FLOAT,
				.offset = offsetof(Vertex, m_texcoord)
			}
		}
	};
	rrc->push_vertex_layout(layout, Crypto::HASH("VertexLayout/Mesh"));
	m_sys_render->dispatch(rrc);
}
void Alexandria::kill()
{

}

void Alexandria::update()
{
	for(auto& msg : m_hermes->pull_inbox(Crypto::HASH("Alexandria"))) {
		if(msg->m_chan == Crypto::HASH("RenderResource")) {
			auto rrm = static_cast<RenderResourceMessage*>(msg);
			// use base class `RenderableAsset` for all assets that need render representation?
			// we are assuming the message is of type `CREATED`, for now...

			auto* asset = get_asset(rrm->m_creator);
			if(asset != nullptr) {
				// delegate
				asset->handle_render_message(rrm);
			} else {
				m_log->info("Cannot delegate render message!");
			}

			/*switch(rrm->m_instance.m_type) {
			case RenderResource::TEXTURE: {
				auto tex = (TextureAsset*)get_asset(rrm->m_creator, NO_LOAD);
				tex->m_handle = rrm->m_instance;
				break;
			}
			case RenderResource::VERTEX_BUFFER: {
				auto mesh = (MeshAsset*)get_asset(rrm->m_creator, NO_LOAD);
				mesh->m_handle = rrm->m_instance;
				break;
			}
			case RenderResource::VERTEX_LAYOUT: {
				if(rrm->m_creator == Crypto::HASH("VertexLayout/Mesh")) {
					m_vl_mesh = rrm->m_instance;
				}
				break;
			}
			default:
				break;
			}*/
		}
	}

	for(auto it = m_rrc_pool.begin(); it != m_rrc_pool.end(); ) {
		// slow, use a `free_rrc` vector (or similar)
		auto* rrc = (*it).get();
		if(rrc->m_delete.load()) {
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
	m_log->info("Asset `{:x}` is not in our database!", id);
	return nullptr;
}
Asset* Alexandria::get_asset(const std::string id, const GetFlag flag)
{
	return get_asset(Crypto::HASH(id), flag);
}

RenderResourceInstance Alexandria::get_vertex_layout(const size_t kind)
{
	if(kind == Crypto::HASH("VertexLayout/Mesh")) {
		return m_vl_mesh;
	} else {
		return {0, RenderResource::Type::NONE};
	}
}

void Alexandria::add_asset(apathy::Path path, const std::string type, const json* root, const std::string db_name, const std::string short_id_override)
{
	const std::string id = path.sanitize().string();
	const std::string stem = path.stem().string();
	auto hash = Crypto::HASH(id);
	std::string short_id;
	if(short_id_override.empty()) {
		short_id = (root == nullptr) ? path.filename() : root->at("id").get<std::string>();
	} else {
		short_id = short_id_override;
	}

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

		auto shader = (ShaderProgramAsset*)place_asset(hash, new ShaderProgramAsset(
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
			// Do not worry of a 'material error' is printed: we take care of it manually, we use a different format
			m_log->info("Warning: {}", err);
		}
		if(!ret) {
			m_log->error("Failed to load model {}!", id);
			abort();
		}
		
		// Prepare asset -- TODO: pool this allocation? let `Alexandria` manage it
		ModelAsset* model = static_cast<ModelAsset*>(place_asset(hash, new ModelAsset(short_id)));
		apathy::Path mesh_id(path);
		mesh_id = mesh_id.append("Mesh0");
		auto mesh_hash = Crypto::HASH(mesh_id.string());
		MeshAsset* mesh = static_cast<MeshAsset*>(place_asset(
			mesh_hash,
			new MeshAsset(short_id + ".0")
		));
		model->m_meshes.push_back(mesh_hash);

		// Copy vertex data
		const bool has_tex = attrib.texcoords.size() > 0;
		const size_t vertices = attrib.vertices.size() / 3;
		mesh->m_verts.resize(vertices);

		for(size_t i = 0; i < vertices; i++) {
			mesh->m_verts[i] = {
				glm::vec3(attrib.vertices[3*i], attrib.vertices[3*i+1], attrib.vertices[3*i+2]),
				glm::vec3(attrib.normals[3*i], attrib.normals[3*i+1], attrib.normals[3*i+2]),
				(has_tex) ? glm::vec2(attrib.texcoords[2*i], attrib.texcoords[2*i+1]) : glm::vec2(0)
			};
		}

		// Prepare data buffer(s)
		RenderResourceContext* rrc = alloc_rrc();
		RenderResourceContext::VertexBufferData buff = {
			.chunks = {
				{.data = &mesh->m_verts[0], .size = vertices * sizeof(Vertex)}
			},
			.usage = RenderResourceContext::VertexBufferData::STATIC
		};
		rrc->push_vertex_buffer(buff, hash);
		m_sys_render->dispatch(rrc);
		
		// Attach material, if any
		auto material_root = root->find("material");
		if(material_root != root->end()) {
			apathy::Path material_id(path);
			material_id = material_id.append("Material");
			add_asset(material_id, "material", &(*material_root), db_name, short_id + ".Material");
			mesh->m_material = Crypto::HASH(material_id.string());
		}

	} else if(type == "texture") {
		TextureAsset* texture = static_cast<TextureAsset*>(place_asset(hash, new TextureAsset(short_id)));
		stbi_set_flip_vertically_on_load(true);
		texture->m_data = stbi_load(working_path.string().c_str(), &texture->m_width, &texture->m_height, &texture->m_channels, 0);

		m_log->info("Loading texture `{}`...", working_path.string());

		// Upload to the GPU
		RenderResourceContext* rrc = alloc_rrc();
		RenderResourceContext::TextureData tex = {
			texture->m_width,
			texture->m_height,
			texture->m_channels,
			texture->m_data
		};
		rrc->push_texture(tex, hash);
		m_sys_render->dispatch(rrc);

		texture->load();

	} else if(type == "material") {
		auto material = (MaterialAsset*)place_asset(hash, new MaterialAsset(short_id));
		
		std::vector<float> Ka = root->at("Ka");
		std::vector<float> Kd = root->at("Kd");
		std::vector<float> Ks = root->at("Ks");

		material->m_ambient = glm::vec3(Ka[0], Ka[1], Ka[2]);
		material->m_diffuse = glm::vec3(Kd[0], Kd[1], Kd[2]);
		material->m_specular = glm::vec3(Ks[0], Ks[1], Ks[2]);

		auto Kd_tex = root->find("Kd_tex");
		if(Kd_tex != root->end()) {
			apathy::Path Kd_tex_id(path);
			Kd_tex_id = Kd_tex_id.append(Kd_tex->get<std::string>());
			add_asset(Kd_tex_id, "texture", nullptr, db_name);
			material->m_diffuse_tex = Crypto::HASH(Kd_tex_id.sanitize().string());
		}

		auto Ks_tex = root->find("Ks_tex");
		if(Ks_tex != root->end()) {
			apathy::Path Ks_tex_id(path);
			Ks_tex_id = Ks_tex_id.append(Ks_tex->get<std::string>());
			add_asset(Ks_tex_id, "texture", nullptr, db_name);
			material->m_specular_tex = Crypto::HASH(Ks_tex_id.sanitize().string());
		}
	} else if(type == "map_noise") {
		apathy::Path noise_id(working_path);
		noise_id = noise_id.append(root->at("source").get<std::string>());
		TextureAsset texture = TextureAsset(short_id + ".Noise");
		stbi_set_flip_vertically_on_load(true);
		texture.m_data = stbi_load(noise_id.string().c_str(), &texture.m_width, &texture.m_height, &texture.m_channels, 0);

		// Populate vertex and index buffers
		const size_t w = texture.m_width;\
		const size_t h = texture.m_height;
		const size_t vlen = w * h;
		const size_t tris = (w-1)*(h-1)*2;
		const size_t ilen = tris*3;

		// Create mesh asset
		apathy::Path mesh_id(path);
		mesh_id = mesh_id.append("Mesh");
		auto mesh_hash = Crypto::HASH(mesh_id.string());
		MeshAsset* mesh = static_cast<MeshAsset*>(place_asset(
			mesh_hash,
			new MeshAsset(short_id + ".Mesh")
		));
		
		auto& vbuffer = mesh->m_verts;
		auto& ibuffer = mesh->m_indices;
		vbuffer.resize(vlen);
		ibuffer.resize(ilen);

		for(size_t i = 0; i < vlen; i++) {
			glm::vec3 pos;
			pos.x = i % w;
			pos.z = (size_t)(i / h);
			pos.y = (float)texture.m_data[i] / 4.f;
			//m_log->info("{}", glm::to_string(pos));
			vbuffer[i].m_position = pos;
		}
		for(size_t i = 0; i < tris; i++) {
			if(i % 2 == 0) {
				ibuffer[3*i]   = i/2;
				ibuffer[3*i+1] = (i/2)+w;
				ibuffer[3*i+2] = (i/2)+1; 
			} else {
				// https://oeis.org/A026741
				ibuffer[3*i]   = (i-1)/2;
				ibuffer[3*i+1] = ((i-1)/2)+w;
				ibuffer[3*i+2] = ((i-1)/2)+w+1;
			}
		}

		RenderResourceContext* rrc = alloc_rrc();
		RenderResourceContext::IndexBufferData ib = {
			.data = &ibuffer[0],
			.size = ilen * sizeof(GLuint)
		};
		rrc->push_index_buffer(ib, mesh_hash);
		RenderResourceContext::VertexBufferData vb = {
			.chunks = {
				{.data = &vbuffer[0], .size = vlen * sizeof(glm::vec3)}
			},
			.usage = RenderResourceContext::VertexBufferData::Usage::STATIC
		};
		rrc->push_vertex_buffer(vb, mesh_hash);

		m_sys_render->dispatch(rrc);
	}
	
	// TODO: 
	// m_assets[hash]->set_path(path); ??
}

RenderResourceContext* Alexandria::alloc_rrc()
{
	m_rrc_pool.emplace_back(new RenderResourceContext());
	auto* rrc = m_rrc_pool.back().get();
	rrc->m_delete.store(false);
	return m_rrc_pool.back().get();
}