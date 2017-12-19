#pragma once

#include <unordered_set>
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
	 * Assets have two states: REGISTERED or LOADED.
	 */
	struct Asset {
		enum Type {
			FONT,
			TEXTURE,
			MATERIAL,
			MESH,
			MODEL,
			SHADER,
			SCRIPT,
			MAP,
			NONE
		};
		static Type str_to_type(const std::string t);
		static std::string type_to_str(const Type t);
		std::string database() const;

		Asset(const apathy::Path id, const std::string prefix, Type t) {
			m_id = id;
			m_logger = Logger::create("Sys.Alexandria." + prefix + "::" + m_id.string());
			m_type = t;
		};
		virtual ~Asset() {};

		t_logger m_logger;
		apathy::Path m_id;
		Type m_type;
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
	 * Map asset. For now, noise.
	 */
	struct MapAsset : public Asset {
		MapAsset(const apathy::Path id);
		~MapAsset();

		t_asset_id m_mesh;
		t_asset_id m_tex;
	};


	/**
	 * Shitty database "filesystem".
	 */
	const json db_find(const json root, const std::string id, const std::string type);
	const json db_get_asset(const json root, const apathy::Path p, const Asset::Type type);


	/**
	 * Alexandria (the library of). Asset manager.
	 */
	class Alexandria : public Sys {
	public:
		Alexandria(apathy::Path local_root, Render* render);
		~Alexandria();

		void init();
		void kill();
		void update(std::chrono::milliseconds delta);

		void parse_database(const std::string filename);

		enum GetFlag {
			NO_LOAD,
			LOAD
		};
	
		template<class T>
		T* get_asset(const t_asset_id hash) {
			static_assert(
				std::is_base_of<Asset, T>::value,
				"Asset passed is not derived from `Asset` base class!"
			);
			return static_cast<T*>(get_asset(hash));
		};
		Asset* get_asset(const t_asset_id hash);

		bool load_asset(const t_asset_id hash);
		void unload_asset(const t_asset_id hash);

		RenderResource get_vertex_layout(const size_t kind);

	private:
		/**
		 * Asset library. Registered assets are in `m_assets`,
		 * while loaded assets are in `m_loaded`.
		 */
		std::map<uint32_t, t_asset_ptr> m_assets;
		std::unordered_set<uint32_t> m_loaded;

		/**
		 * Database book-keeping. Top-most directory, i.e.
		 * `/Base/Models/Greek` model is from `Base` database.
		 * Allows us to defer loading.
		 */
		std::map<uint32_t, json> m_dbs;

		/**
		 * Iterates JSON database and calls `parse_asset` on each found asset.
		 */
		void parse_folder(const json& root, apathy::Path path, const std::string db_name);
		/**
		 * Registers an asset by parsing a JSON snippet.
		 */
		t_asset_id parse_asset(apathy::Path path, const Asset::Type type, const json* root);

		template<class T>
		T* register_asset(const apathy::Path id) {
			static_assert(
				std::is_base_of<Asset, T>::value,
				"Asset passed is not derived from `Asset` base class!"
			);
			auto hash = Crypto::HASH(id.string());
			if(m_assets.find(hash) != m_assets.end()) {
				m_log->warn("Asset `{:x}` already registered, skipping...", hash);
				return nullptr;
			}
			auto asset = new T(id);
			m_assets[hash] = t_asset_ptr(asset);
			m_log->debug("Registering asset `{}` [{}] ({:x})", id.string(), Asset::type_to_str(asset->m_type), hash);
			return asset;
		}

		// Asset-specific loading methods
		bool _load_model(const t_asset_id hash, const json* root);
		bool _load_material(const t_asset_id hash, const json* root);
		bool _load_shader(ShaderProgramAsset* shader, apathy::Path id, const json* root);
		bool _load_texture(const t_asset_id hash, const json* root, bool gpu = true);
		bool _load_map(const t_asset_id hash, const json* root);

		void _unload_model(const t_asset_id hash);
		void _unload_material(const t_asset_id hash);
		void _unload_texture(const t_asset_id hash);
		void _unload_map(const t_asset_id hash);
		
		/**
		 * To deal with working paths vs. virtual database paths.
		 */
		apathy::Path get_working_path(const apathy::Path id) const;
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