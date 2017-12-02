#pragma once

#include "sys.hpp"
#include "util/crypto.hpp"

#include <selene.h>
#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>
#include <apathy/path.hpp>

using namespace gl;


namespace MilSim {

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

	private:
		bool inner_load();
		Format m_format;
	};

	/**
	 * Texture asset.
	 */
	class TextureAsset : public Asset {
	public:
		TextureAsset(const std::string name,
			unsigned char* data,
			int width, int height, int channels);
		~TextureAsset();

		GLuint m_tex_id;
		unsigned char* m_data;
		int m_width, m_height, m_channels;
	
	private:
		bool inner_load();
		void inner_free();
	};

	/**
	 * Vertex base data structure, minimal.
	 */
	struct Vertex {
		Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 tex) {
			m_position = pos;
			m_normal = norm;
			m_texture = tex;
		};

		glm::vec3 m_position;
		glm::vec3 m_normal;
		glm::vec2 m_texture;
	};
	/**
	 * Mesh: main structure for 3D geometry.
	 */
	struct Mesh {
		std::vector<Vertex> m_verts;
		GLuint m_vao, m_vbo, m_ebo;
	};
	/**
	 * ModelAsset: lowest level of 3D organization,
	 * since Meshes are *not* accessible by ID
	 * (should they?...)
	 */
	class ModelAsset : public Asset {
	public:
		ModelAsset(const std::string name, std::vector<Mesh> meshes);
		~ModelAsset();

		std::vector<Mesh> m_meshes;
		t_asset_id m_texture;

	private:
		bool inner_load();
		void inner_free();
	};

	/**
	 * Compiles shaders.
	 */
	class ShaderProgramAsset : public Asset {
	public:
		ShaderProgramAsset(const std::string name, const std::string vert_source, const std::string frag_source);
		~ShaderProgramAsset();

	// private:?
		GLuint m_vert_id;
		GLuint m_frag_id;
		GLuint m_prog_id;
		std::string m_vert_source;
		std::string m_frag_source;
		std::map<std::string, GLuint> m_uniforms;
	
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

	private:
		sel::State m_state;
		bool inner_load();
	};

	/**
	 * Alexandria (the library of). Asset manager.
	 */
	class Alexandria : public Sys {
	public:
		Alexandria(const std::string local_root);
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

	private:
		std::map<uint32_t, t_asset_ptr> m_assets;

		void load_asset();

		// Database loading
		void load_folder(const sel::Selector& root, apathy::Path path, const std::string db_name);
		void add_asset(apathy::Path path, const std::string type, sel::Selector* root, const std::string db_name);

		void place_asset(const t_asset_id hash, Asset* asset) {
			if(m_assets.find(hash) != m_assets.end()) {
				m_log->warn("Asset `{:x}` already exists, skipping...");
				return;
			}
			m_assets[hash] = t_asset_ptr(asset);
			m_assets[hash]->set_hash(hash);
		}

		apathy::Path m_local_root;
	};

}