#pragma once

#include "sys.hpp"
#include "util/crypto.hpp"

#include "spdlog/spdlog.h"
#include "selene.h"
#include <glbinding/gl/gl.h>

using namespace gl;


namespace MilSim {

	/**
	 * Asset: abstract base class for assets.
	 */
	class Asset {
	public:
		Asset(const std::string logname)
			: m_log(spdlog::stdout_color_mt("Alexandria." + logname))
		{};
		virtual ~Asset() {};

		bool m_loaded;
		virtual bool load() = 0;
	protected:
		std::shared_ptr<spdlog::logger> m_log;
	};

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
	 * MeshAsset: main structure for 3D geometry.
	 */
	class MeshAsset : public Asset {
	public:
		MeshAsset();
		~MeshAsset();

		bool load();
	private:
		// 
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

	typedef std::unique_ptr<Asset> t_asset_ptr;
	typedef uint32_t t_asset_id;

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