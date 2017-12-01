#pragma once

#include "sys.hpp"
#include "util/crypto.hpp"

#include <selene.h>
#include <glbinding/gl/gl.h>
#include <tiny_obj_loader.h>
#include <glm/glm.hpp>

using namespace gl;


namespace MilSim {

	typedef uint32_t t_asset_id;

	/**
	 * Asset: abstract base class for assets.
	 */
	class Asset {
	public:
		Asset(const std::string logname)
			: m_name("Alexandria." + logname)
		{
			m_log = Logger::create(m_name);
		};
		virtual ~Asset() {};

		void set_hash(t_asset_id hash) {
			m_hash = hash;
		};

		bool m_loaded;
		virtual bool load() = 0;
	protected:
		t_logger m_log;
		std::string m_name;
		t_asset_id m_hash;
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

		bool load();
	private:
		Format m_format;
	};

	/**
	 * Vertex base data structure, minimal.
	 */
	struct Vertex {
		Vertex(glm::vec3 pos, glm::vec3 norm) {
			m_position = pos;
			m_normal = norm;
		};

		glm::vec3 m_position;
		glm::vec3 m_normal;
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
		bool load();

		std::vector<Mesh> m_meshes;
	};

	/**
	 * Compiles shaders.
	 */
	class ShaderProgramAsset : public Asset {
	public:
		ShaderProgramAsset(const std::string vert_source, const std::string frag_source);
		~ShaderProgramAsset();

		bool load();

	// private:?
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
	class ScriptAsset : public Asset {
	public:
		ScriptAsset();
		~ScriptAsset();

		bool load();
	private:
		sel::State m_state;
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

		Asset* get_asset(const std::string id);
		Asset* get_asset(const t_asset_id id);

	private:
		std::map<uint32_t, t_asset_ptr> m_assets;

		void load_asset();

		// Database loading
		void load_folder(const sel::Selector& root, const std::string old_id, const std::string db_name);
		void add_asset(const std::string id, const std::string type, sel::Selector& root, const std::string db_name);

		std::string m_local_root;
	};

}