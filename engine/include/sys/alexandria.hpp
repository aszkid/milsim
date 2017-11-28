#pragma once

#include "sys.hpp"

#include "spdlog/spdlog.h"
#include "CRC.h"
#include "selene.h"


#define HASH(str)  CRC::Calculate(str.c_str(), id.length(), CRC::CRC_32())


namespace MilSim {

	/**
	 * Asset: abstract base class for assets.
	 */
	class Asset {
	public:
		Asset() {};
		virtual ~Asset() {};

		bool loaded;
		virtual void load() = 0;
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

		void load();
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

		void load();
	private:
		// 
	};

	typedef std::unique_ptr<Asset> t_asset_ptr;

	/**
	 * Alexandria (the library of). Asset manager.
	 */
	class Alexandria : public Sys {
	public:
		Alexandria();
		~Alexandria();

		void init();
		void kill();
		void update();

		void load_database(const std::string filename);

		Asset* get_asset(const std::string id);

	private:
		std::map<uint32_t, t_asset_ptr> m_assets;

		void load_asset();

		// Database loading
		void load_folder(const sel::Selector& root, const std::string old_id);
		void add_asset(const std::string id, const std::string type, const sel::Selector& root);
	};

}