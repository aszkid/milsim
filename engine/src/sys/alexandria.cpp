#include "sys/alexandria.hpp"
#include "util/io.hpp"

#include <deque>
#include <glm/ext.hpp>
#include <tiny_obj_loader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

using namespace MilSim;

const json MilSim::db_find(const json root, const std::string id, const std::string type)
{
	for(auto it = root.begin(); it != root.end(); it++) {
		if(it->at("type") == type && it->at("id") == id) {
			if(type == "folder") {
				return it.value().at("contents");
			} else {
				return it.value();
			}
		}
	}
	return json();
}
const json MilSim::db_get_asset(const json root, const apathy::Path p, const Asset::Type t)
{
	auto v = p.split();
	std::deque<apathy::Path::Segment> crumbs(v.begin(), v.end());
	crumbs.pop_front(); // empty 0 element
	crumbs.pop_front(); // pop db name

	auto asset_id = crumbs.back().segment;
	crumbs.pop_back();

	auto sub = root;
	do {
		auto seg = crumbs.front().segment;
		crumbs.pop_front();
		sub = db_find(sub, seg, "folder");
	} while(!crumbs.empty() && !sub.empty());

	return db_find(sub, asset_id, Asset::type_to_str(t));
}

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

////////////////////////////////////////
// ALEXANDRIA
////////////////////////////////////////
Alexandria::Alexandria(apathy::Path local_root, Render* render)
	: Sys::Sys("Alexandria"), m_local_root(local_root), m_sys_render(render)
{
	
}
Alexandria::~Alexandria()
{
}

void Alexandria::init()
{
	m_hermes->subscribe(Crypto::HASH("Alexandria"), {
		Crypto::HASH("RenderResource")
	});

	// Prepare common vertex layouts
	RenderResourceContext* rrc = m_sys_render->new_rrc();
	m_sys_render->alloc(&m_vl_mesh, RenderResource::VERTEX_LAYOUT);
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
	rrc->push_vertex_layout(layout, m_vl_mesh);
	m_sys_render->dispatch(rrc);
}
void Alexandria::kill()
{
	/*for(auto hash : m_loaded) {
		unload_asset(hash);
	}*/
	while(!m_loaded.empty()) {
		unload_asset(*m_loaded.begin());
	}
}

void Alexandria::update(std::chrono::milliseconds delta)
{
	for(auto& msg : m_hermes->pull_inbox(Crypto::HASH("Alexandria"))) {
		
	}
	//m_log->info("{} assets, {} hot.", m_assets.size(), m_loaded.size());
}

void Alexandria::parse_database(const std::string filename)
{
	apathy::Path dbpath(get_working_path(filename));
	m_log->info("Loading database `{}`...", filename);

	json _db = json::parse(IO::read_file(dbpath.string()));
	const std::string dbtype = _db["db_type"];
	const std::string dbname = _db["db_name"];
	const auto dbhash = Crypto::HASH(dbname);
	apathy::Path id_root("/" + dbname + "/");
	m_dbs[dbhash] = std::move(_db);

	json& db = m_dbs[dbhash];
	// dont forget to normalize paths...
	if(dbtype == "local") {
		const json& data = db["data"];
		parse_folder(data, id_root, dbname);
	} else {
		m_log->error("We don't support databases of type `{}` yet!", dbtype);
		abort();
	}

	m_log->info("Finished loading database!");
}
void Alexandria::parse_folder(const json& root, apathy::Path path, const std::string db_name)
{
	//m_log->info("Loading folder `{}`", path.string());

	for(auto& node : root) {
		const std::string new_id = node["id"];

		if(node["type"] == "folder") {
			parse_folder(node["contents"], apathy::Path(path).append(new_id).directory(), db_name);
		}
		else {
			parse_asset(apathy::Path(path).append(new_id), Asset::str_to_type(node["type"]), &node);
		}
	}
}

Asset* Alexandria::get_asset(const t_asset_id hash)
{
	auto asset = m_assets.find(hash);
	if(asset == m_assets.end()) {
		m_log->info("Asset `{:x}` not registered!", hash);
		return nullptr;
	}
	return asset->second.get();
}
bool Alexandria::load_asset(const t_asset_id hash)
{
	if(m_loaded.find(hash) != m_loaded.end())
		return true;
	auto asset = get_asset(hash);
	if(asset == nullptr)
		return false;

	const auto db_id = Crypto::HASH(asset->database());
	const auto& db = m_dbs[db_id];
	const auto& data = db_get_asset(db["data"], asset->m_id, asset->m_type);

	if(data.empty()) {
		m_log->info("Asset `{}` ({:x}) has no database entry!", asset->m_id.string(), Crypto::HASH(asset->m_id.string()));
		return false;
	}

	m_log->info("Loading asset `{}` [{}] ({:x})", asset->m_id.string(), Asset::type_to_str(asset->m_type), hash);

	bool ok;
	switch(asset->m_type) {
	case Asset::MODEL:
		ok =  _load_model(hash, &data);
		break;
	case Asset::MATERIAL:
		ok =  _load_material(hash, &data);
		break;
	case Asset::TEXTURE:
		ok =  _load_texture(hash, &data);
		break;
	case Asset::MAP:
		ok =  _load_map(hash, &data);
		break;
	default:
		ok = false;
		break;
	}

	if(ok)
		m_loaded.emplace(hash);
	
	return ok;
}
void Alexandria::unload_asset(const t_asset_id hash)
{
	auto it = m_assets.find(hash);
	auto hot = m_loaded.find(hash);
	if(it == m_assets.end() || hot == m_loaded.end())
		return;
	auto asset = it->second.get();

	m_log->debug("Unloading asset `{}` [{}] ({:x})", asset->m_id.string(), Asset::type_to_str(asset->m_type), hash);
	
	switch(asset->m_type) {
	case Asset::MODEL:
		_unload_model(hash);
		break;
	case Asset::MATERIAL:
		_unload_material(hash);
		break;
	case Asset::TEXTURE:
		_unload_texture(hash);
		break;
	case Asset::MAP:
		_unload_map(hash);
		break;
	default:
		break;
	}
	
	m_loaded.erase(hash);
}

RenderResource Alexandria::get_vertex_layout(const size_t kind)
{
	if(kind == Crypto::HASH("VertexLayout/Mesh")) {
		return m_vl_mesh;
	} else {
		return {0};
	}
}
t_asset_id Alexandria::parse_asset(apathy::Path path, const Asset::Type type, const json* root)
{
	const std::string id = path.sanitize().string();
	auto hash = Crypto::HASH(id);

	if(m_assets.find(hash) != m_assets.end()) {
		m_log->info("Asset `{}` already in our database, skipping.", id);
		return 0;
	}

	switch(type) {
	case Asset::FONT:
		break;

	case Asset::SHADER:
		register_asset<ShaderProgramAsset>(id);
		break;

	case Asset::MODEL: {
		auto model = register_asset<ModelAsset>(id);

		// Register zero mesh
		apathy::Path mesh_id(model->m_id);
		mesh_id = mesh_id.append("0");
		auto mesh = register_asset<MeshAsset>(mesh_id);
		model->m_meshes.push_back(Crypto::HASH(mesh_id.string()));

		// Register mesh material, if any
		auto material_root = root->find("material");
		if(material_root != root->end()) {
			apathy::Path material_id(model->m_id);
			material_id = material_id.append("Material");
			parse_asset(material_id, Asset::MATERIAL, &(*material_root));
			mesh->m_material = Crypto::HASH(material_id.string());
		}

		break;

	} case Asset::TEXTURE: {
		auto texture = register_asset<TextureAsset>(id);
		break;

	} case Asset::MATERIAL: {
		auto material = register_asset<MaterialAsset>(id);

		auto Kd_tex = root->find("Kd_tex");
		if(Kd_tex != root->end()) {
			apathy::Path Kd_tex_id(material->m_id);
			Kd_tex_id = Kd_tex_id.append(Kd_tex->get<std::string>());
			register_asset<TextureAsset>(Kd_tex_id);
			material->m_diffuse_tex = Crypto::HASH(Kd_tex_id.sanitize().string());
		}
		auto Ks_tex = root->find("Ks_tex");
		if(Ks_tex != root->end()) {
			apathy::Path Ks_tex_id(material->m_id);
			Ks_tex_id = Ks_tex_id.append(Ks_tex->get<std::string>());
			register_asset<TextureAsset>(Ks_tex_id);
			material->m_specular_tex = Crypto::HASH(Ks_tex_id.sanitize().string());
		}
		
		break;

	} case Asset::MAP: {
		auto map = register_asset<MapAsset>(id);

		// Register heightmap
		apathy::Path tex_id(id);
		tex_id = tex_id.append(root->at("source").get<std::string>());
		register_asset<TextureAsset>(tex_id);
		map->m_tex = Crypto::HASH(tex_id.string());

		// Register mesh
		apathy::Path mesh_id(id);
		mesh_id = mesh_id.append("Mesh");
		register_asset<MeshAsset>(mesh_id);
		map->m_mesh = Crypto::HASH(mesh_id.string());

		break;

	} default:
		break;
	}

	return hash;
}


bool Alexandria::_load_model(const t_asset_id hash, const json* root)
{
	auto model = get_asset<ModelAsset>(hash);
	const std::string file = root->at("source");
	apathy::Path working_path(get_working_path(model->m_id));

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	bool ret = tinyobj::LoadObj(
		&attrib, &shapes, &materials, &err,
		apathy::Path(working_path).append(file).string().c_str(),
		apathy::Path(working_path).directory().string().c_str()
	);
	if(!err.empty()) {
		// Do not worry of a 'material error' is printed: we take care of it manually, we use a different format
		//m_log->info("Warning: {}", err);
	}
	if(!ret) {
		m_log->error("Failed to load model {}!", model->m_id.string());
		abort();
	}
	
	// Prepare asset -- TODO: pool this allocation? let `Alexandria` manage it
	// We assume the model is a single mesh... for now
	auto mesh = get_asset<MeshAsset>(model->m_meshes[0]);
	m_loaded.emplace(model->m_meshes[0]);

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
	RenderResourceContext* rrc = m_sys_render->new_rrc();
	m_sys_render->alloc(&mesh->m_vb_handle, RenderResource::VERTEX_BUFFER);
	RenderResourceContext::VertexBufferData buff = {
		.chunks = {
			{.data = &mesh->m_verts[0], .size = vertices * sizeof(Vertex)}
		},
		.usage = RenderResourceContext::VertexBufferData::STATIC
	};
	rrc->push_vertex_buffer(buff, mesh->m_vb_handle);
	m_sys_render->dispatch(rrc);
	
	// Load material
	if(!_load_material(mesh->m_material, &root->at("material")))
		return false;
	m_loaded.emplace(mesh->m_material);
	return true;
}

bool Alexandria::_load_material(const t_asset_id hash, const json* root)
{
	auto material = get_asset<MaterialAsset>(hash);
	apathy::Path working_path(get_working_path(material->m_id));
	
	std::vector<float> Ka = root->at("Ka");
	std::vector<float> Kd = root->at("Kd");
	std::vector<float> Ks = root->at("Ks");

	material->m_ambient = glm::vec3(Ka[0], Ka[1], Ka[2]);
	material->m_diffuse = glm::vec3(Kd[0], Kd[1], Kd[2]);
	material->m_specular = glm::vec3(Ks[0], Ks[1], Ks[2]);

	// Load specular and diffuse textures
	if(material->m_diffuse_tex != 0) {
		if(!_load_texture(material->m_diffuse_tex, nullptr))
			return false;
		m_loaded.emplace(material->m_diffuse_tex);
	}
	if(material->m_specular_tex != 0) {
		if(!_load_texture(material->m_specular_tex, nullptr))
			return false;
		m_loaded.emplace(material->m_specular_tex);
	}

	return true;
}

bool Alexandria::_load_shader(ShaderProgramAsset* shader, apathy::Path id, const json* root)
{
	const size_t hash = Crypto::HASH(id.string());
	const std::string file = root->at("source");
	apathy::Path working_path(get_working_path(id));
	
	// TODO: catch exceptions...
	auto vert_source = IO::read_file(apathy::Path(working_path).string() + ".vert");
	auto frag_source = IO::read_file(apathy::Path(working_path).string() + ".frag");

	return true;
}

bool Alexandria::_load_texture(const t_asset_id hash, const json* root, bool gpu)
{
	auto texture = get_asset<TextureAsset>(hash);
	apathy::Path working_path(get_working_path(texture->m_id));

	// Upload to memory
	stbi_set_flip_vertically_on_load(true);
	texture->m_data = stbi_load(working_path.string().c_str(), &texture->m_width, &texture->m_height, &texture->m_channels, 0);
	if(texture->m_data == NULL) {
		m_log->info("Failed to load texture: {}", stbi_failure_reason());
		return false;
	}

	GLenum pixf;
	switch(texture->m_channels) {
	case 1:
		pixf = GL_RED;
		break;
	case 2:
		pixf = GL_RG;
		break;
	case 3:
		pixf = GL_RGB;
		break;
	case 4:
		pixf = GL_RGBA;
		break;
	default:
		abort();
	}

	// Upload to the GPU
	if(gpu) {
		RenderResourceContext* rrc = m_sys_render->new_rrc();
		m_sys_render->alloc(&texture->m_handle, RenderResource::TEXTURE);
		RenderResourceContext::TextureData tex = {
			.width = texture->m_width,
			.height = texture->m_height,
			.source = texture->m_data,
			.internal_format = pixf,
			.pixel_format = pixf,
			/*.mipmap = true,
			.filter = RenderResourceContext::TextureData::Filter::LINEAR*/
		};
		rrc->push_texture(tex, texture->m_handle);
		m_sys_render->dispatch(rrc);
	}

	return true;
}

bool Alexandria::_load_map(const t_asset_id hash, const json* root)
{
	auto map = get_asset<MapAsset>(hash);

	auto texture = get_asset<TextureAsset>(map->m_tex);
	if(!_load_texture(map->m_tex, nullptr, false))
		return false;
	m_loaded.emplace(map->m_tex);

	// Populate vertex and index buffers
	const size_t w = texture->m_width;
	const size_t h = texture->m_height;
	const size_t vlen = w * h;
	const size_t tris = (w-1)*(h-1)*2;
	const size_t ilen = tris*3;

	auto mesh = get_asset<MeshAsset>(map->m_mesh);
	auto& vbuffer = mesh->m_verts;
	auto& ibuffer = mesh->m_indices;
	vbuffer.resize(vlen);
	ibuffer.resize(ilen);
	// Add mesh to `loaded` list
	m_loaded.emplace(map->m_mesh);

	for(size_t i = 0; i < vlen; i++) {
		glm::vec3 pos;
		pos.x = i % w;
		pos.z = (size_t)(i / h);
		pos.y = (float)texture->m_data[i] / 4.f;
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

	// Upload to GPU memory
	RenderResourceContext* rrc = m_sys_render->new_rrc();

	m_sys_render->alloc(&mesh->m_ib_handle, RenderResource::INDEX_BUFFER);
	RenderResourceContext::IndexBufferData ib = {
		.data = &ibuffer[0],
		.size = ilen * sizeof(GLuint)
	};
	rrc->push_index_buffer(ib, mesh->m_ib_handle);

	m_sys_render->alloc(&mesh->m_vb_handle, RenderResource::VERTEX_BUFFER);
	RenderResourceContext::VertexBufferData vb = {
		.chunks = {
			{.data = &vbuffer[0], .size = vlen * sizeof(glm::vec3)}
		},
		.usage = RenderResourceContext::VertexBufferData::Usage::STATIC
	};
	rrc->push_vertex_buffer(vb, mesh->m_vb_handle);

	m_sys_render->dispatch(rrc);

	return true;
}

apathy::Path Alexandria::get_working_path(const apathy::Path id) const
{
	apathy::Path working_path(m_local_root);
	working_path.append(id).sanitize();
	return working_path;
}


void Alexandria::_unload_model(const t_asset_id hash)
{
	auto model = get_asset<ModelAsset>(hash);
	for(auto mesh : model->m_meshes) {
		unload_asset(mesh);
	}
}
void Alexandria::_unload_texture(const t_asset_id hash)
{
	auto texture = get_asset<TextureAsset>(hash);
	stbi_image_free(texture->m_data);
	/**
	 * if(texture->m_handle != 0) {
	 * 	dispatch `free` message to render system
	 * }
	 */
}
void Alexandria::_unload_material(const t_asset_id hash)
{
	auto material = get_asset<MaterialAsset>(hash);
	unload_asset(material->m_diffuse_tex);
	unload_asset(material->m_specular_tex);
}
void Alexandria::_unload_map(const t_asset_id hash)
{
	auto map = get_asset<MapAsset>(hash);
	unload_asset(map->m_mesh);
	unload_asset(map->m_tex);
}