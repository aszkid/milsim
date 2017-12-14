#pragma once

#include <json.hpp>
#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>
#include <apathy/path.hpp>

#include "sys.hpp"
#include "sys/render.hpp"
#include "util/crypto.hpp"

namespace MilSim {

	using namespace gl;
	using json = nlohmann::json;

	typedef uint32_t t_asset_id;

	/**
	 * Asset: abstract base class for assets.
	 */
	class Asset {
	public:
		Asset(const std::string logname) {
			m_loaded = false;
			m_name = "Alexandria." + logname;
			m_logger = Logger::create(m_name);
		};
		virtual ~Asset() {};

		void set_hash(t_asset_id hash) {
			m_hash = hash;
		};
		void set_path(apathy::Path path) {
			m_path = path;
		};

		bool m_loaded;
		bool load() {
			m_loaded = inner_load();
			return m_loaded;
		};
		void free() {
			if(m_loaded) {
				inner_free();
			}
		};

		virtual void handle_render_message(RenderResourceMessage* msg) = 0;

	protected:
		t_logger m_logger;
		apathy::Path m_path;
		std::string m_name;
		t_asset_id m_hash;

		virtual bool inner_load() = 0;
		virtual void inner_free() = 0;

	};

	// helper types
	typedef std::unique_ptr<Asset> t_asset_ptr;

	/**
	 * FontAsset: loads font files and prepares texturemaps.
	 */
	class FontAsset : public Asset {
	public:
		FontAsset();
		~FontAsset();

		enum Format {
			TTF, OTF
		};

		void handle_render_message(RenderResourceMessage* msg);

	private:
		bool inner_load();
		Format m_format;
	};

	/**
	 * Texture asset.
	 */
	class TextureAsset : public Asset {
	public:
		TextureAsset(const std::string name);
		~TextureAsset();

		unsigned char* m_data;
		int m_width, m_height, m_channels;

		RenderResourceInstance m_handle;

		void handle_render_message(RenderResourceMessage* msg);
	
	private:
		bool inner_load();
		void inner_free();
	};

	/**
	 * Material asset.
	 */
	class MaterialAsset : public Asset {
	public:
		MaterialAsset(const std::string name);
		~MaterialAsset();

		glm::vec3 m_ambient;
		glm::vec3 m_diffuse;
		glm::vec3 m_specular;

		t_asset_id m_diffuse_tex;
		t_asset_id m_specular_tex;

		void handle_render_message(RenderResourceMessage* msg);

	private:
		bool inner_load();
		void inner_free();
	};

	struct Vertex {
		glm::vec3 m_position;
		glm::vec3 m_normal;
		glm::vec2 m_texcoord;
	};
	/**
	 * Mesh: main structure for 3D geometry.
	 */
	class MeshAsset : public Asset {
	public:
		MeshAsset(const std::string name);
		~MeshAsset();
		/**
		 * Back to the basics.
		 */
		std::vector<Vertex> m_verts;
		std::vector<GLuint> m_indices;
		t_asset_id m_material;
		RenderResourceInstance m_vb_handle;
		RenderResourceInstance m_ib_handle;

		void handle_render_message(RenderResourceMessage* msg);

	private:
		bool inner_load();
		void inner_free();
	};
	/**
	 * ModelAsset: 
	 */
	class ModelAsset : public Asset {
	public:
		ModelAsset(const std::string name);
		~ModelAsset();

		std::vector<t_asset_id> m_meshes;

		void handle_render_message(RenderResourceMessage* msg);

	private:
		bool inner_load();
		void inner_free();
	};

	/**
	 * Compiles shaders.
	 */
	class ShaderProgramAsset : public Asset {
	public:
		ShaderProgramAsset(const std::string name);
		~ShaderProgramAsset();

	// private:?
		GLuint m_vert_id;
		GLuint m_frag_id;
		GLuint m_prog_id;
		std::string m_vert_source;
		std::string m_frag_source;
		std::map<std::string, GLuint> m_uniforms;

		void handle_render_message(RenderResourceMessage* msg);
	
	private:
		bool inner_load();
		void inner_free();
	};

	/**
	 * For now, a container for sel::State.
	 */
	class ScriptAsset : public Asset {
	public:
		ScriptAsset();
		~ScriptAsset();

		void handle_render_message(RenderResourceMessage* msg);

	private:
		bool inner_load();
	};

	/**
	 * Alexandria (the library of). Asset manager.
	 */
	class Alexandria : public Sys {
	public:
		Alexandria(apathy::Path local_root, Render* render);
		~Alexandria();

		void init();
		void kill();
		void update();

		void load_database(const std::string filename);

		enum GetFlag {
			NO_LOAD,
			LOAD
		};
	
		Asset* get_asset(const std::string id, const GetFlag flag = LOAD);
		Asset* get_asset(const t_asset_id id, const GetFlag flag = LOAD);

		RenderResourceInstance get_vertex_layout(const size_t kind);

	private:
		std::map<uint32_t, t_asset_ptr> m_assets;

		void load_asset();

		// Database loading
		void load_folder(const json& root, apathy::Path path, const std::string db_name);
		Asset* add_asset(apathy::Path path, const std::string type, const json* root);

		// Asset-specific loading methods
		bool _load_model_asset(ModelAsset* model, apathy::Path id, const json* root);
		bool _load_material_asset(MaterialAsset* material, apathy::Path id, const json* root);
		bool _load_shader_asset(ShaderProgramAsset* shader, apathy::Path id, const json* root);
		bool _load_texture_asset(TextureAsset* texture, apathy::Path id, const json* root);
		bool _load_map_asset(apathy::Path id, const json* root);

		Asset* place_asset(const t_asset_id hash, Asset* asset) {
			if(m_assets.find(hash) != m_assets.end()) {
				m_log->warn("Asset `{:x}` already exists, skipping...");
				return nullptr;
			}
			m_assets[hash] = t_asset_ptr(asset);
			m_assets[hash]->set_hash(hash);
			return asset;
		}

		apathy::Path m_local_root;

		// Handle for `Sys.Render` -- needed to upload assets to the GPU
		Render* m_sys_render;
		// Keeping track of RRCs -- we should have a thread-wide allocator
		std::vector<std::unique_ptr<RenderResourceContext>> m_rrc_pool;
		RenderResourceContext* alloc_rrc();

		/**
		 * Common vertex layout handles.
		 * Every vertex buffer we populate has one of these
		 * as their vertex layout. We create them at startup.
		 */
		RenderResourceInstance m_vl_mesh;

	};

}