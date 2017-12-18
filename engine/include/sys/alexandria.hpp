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
	struct Asset {
		Asset(const apathy::Path id, const std::string prefix) {
			m_loaded = false;
			m_id = id;
			m_logger = Logger::create("Sys.Alexandria." + prefix + "::" + m_id.string());
		};
		virtual ~Asset() {};

		t_logger m_logger;
		apathy::Path m_id;
		bool m_loaded;
	};

	// helper types
	typedef std::unique_ptr<Asset> t_asset_ptr;

	/**
	 * FontAsset: loads font files and prepares texturemaps.
	 */
	struct FontAsset : public Asset {
		FontAsset();
		~FontAsset();

		enum Format {
			TTF, OTF
		};

		Format m_format;
	};

	/**
	 * Texture asset.
	 */
	struct TextureAsset : public Asset {
		TextureAsset(const apathy::Path id);
		~TextureAsset();

		unsigned char* m_data;
		int m_width, m_height, m_channels;

		RenderResource m_handle;
	};

	/**
	 * Material asset.
	 */
	struct MaterialAsset : public Asset {
		MaterialAsset(const apathy::Path id);
		~MaterialAsset();

		glm::vec3 m_ambient;
		glm::vec3 m_diffuse;
		glm::vec3 m_specular;

		t_asset_id m_diffuse_tex;
		t_asset_id m_specular_tex;
	};

	struct Vertex {
		glm::vec3 m_position;
		glm::vec3 m_normal;
		glm::vec2 m_texcoord;
	};
	/**
	 * Mesh: main structure for 3D geometry.
	 */
	struct MeshAsset : public Asset {
		MeshAsset(const apathy::Path id);
		~MeshAsset();
		/**
		 * Back to the basics.
		 */
		std::vector<Vertex> m_verts;
		std::vector<GLuint> m_indices;
		t_asset_id m_material;
		RenderResource m_vb_handle;
		RenderResource m_ib_handle;
	};
	/**
	 * ModelAsset: 
	 */
	struct ModelAsset : public Asset {
		ModelAsset(const apathy::Path id);
		~ModelAsset();

		std::vector<t_asset_id> m_meshes;
	};

	/**
	 * Compiles shaders.
	 */
	struct ShaderProgramAsset : public Asset {
		ShaderProgramAsset(const apathy::Path id);
		~ShaderProgramAsset();

		GLuint m_vert_id;
		GLuint m_frag_id;
		GLuint m_prog_id;
		std::string m_vert_source;
		std::string m_frag_source;
		std::map<std::string, GLuint> m_uniforms;
	};

	/**
	 * For now, a container for sel::State.
	 */
	struct ScriptAsset : public Asset {
		ScriptAsset();
		~ScriptAsset();
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

		RenderResource get_vertex_layout(const size_t kind);

	private:
		std::map<uint32_t, t_asset_ptr> m_assets;

		void load_asset();

		// Database loading
		void load_folder(const json& root, apathy::Path path, const std::string db_name);
		Asset* add_asset(apathy::Path path, const std::string type, const json* root);

		/**
		 * Asset database.
		 */
		std::vector<std::unique_ptr<FontAsset>> m_fonts;
		std::vector<std::unique_ptr<TextureAsset>> m_textures;

		std::vector<size_t> m_loaded_fonts;
		std::vector<size_t> m_loaded_textures;

		// Asset-specific loading methods
		bool _load_model(ModelAsset* model, apathy::Path id, const json* root);
		bool _load_material(MaterialAsset* material, apathy::Path id, const json* root);
		bool _load_shader(ShaderProgramAsset* shader, apathy::Path id, const json* root);
		bool _load_texture(TextureAsset* texture, apathy::Path id, const json* root);
		bool _load_map(apathy::Path id, const json* root);
		
		apathy::Path get_working_path(const apathy::Path id) const;

		Asset* place_asset(const t_asset_id hash, Asset* asset) {
			if(m_assets.find(hash) != m_assets.end()) {
				m_log->warn("Asset `{:x}` already exists, skipping...");
				return nullptr;
			}
			m_assets[hash] = t_asset_ptr(asset);
			return asset;
		}

		apathy::Path m_local_root;

		// Handle for `Sys.Render` -- needed to upload assets to the GPU
		Render* m_sys_render;

		/**
		 * Common vertex layout handles.
		 * Every vertex buffer we populate has one of these
		 * as their vertex layout. We create them at startup.
		 */
		RenderResource m_vl_mesh;

	};

}