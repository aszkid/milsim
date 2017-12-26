#pragma once

#include <unordered_set>
#include <json.hpp>
#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>
#include <apathy/path.hpp>

#include "sys.hpp"
#include "sys/render.hpp"
#include "util/crypto.hpp"
#include "assets.hpp"

namespace MilSim {

	using namespace gl;
	using json = nlohmann::json;


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
		T* get_asset(const t_asset_id hash, const GetFlag flag = LOAD) {
			static_assert(
				std::is_base_of<Asset, T>::value,
				"Asset passed is not derived from `Asset` base class!"
			);
			return static_cast<T*>(get_asset(hash, flag));
		};
		template<class T>
		T* get_asset(const std::string name, const GetFlag flag = LOAD) {
			return get_asset<T>(Crypto::HASH(name), flag);
		};
		Asset* get_asset(const t_asset_id hash, const GetFlag flag);
		bool have_asset(const t_asset_id hash);
		bool is_loaded(const t_asset_id hash);

		bool load_asset(const std::string name);
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
		bool _load_texture(const t_asset_id hash, const json* root, bool gpu = true);
		bool _load_map(const t_asset_id hash, const json* root);
		bool _load_shader_program(const t_asset_id hash, const json* root);

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