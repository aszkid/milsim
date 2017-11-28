#include "sys/alexandria.hpp"

using namespace MilSim;

////////////////////////////////////////
// FONTASSET
////////////////////////////////////////
FontAsset::FontAsset()
	: Asset::Asset()
{}
FontAsset::~FontAsset()
{}

void FontAsset::load()
{

}


////////////////////////////////////////
// ALEXANDRIA
////////////////////////////////////////
Alexandria::Alexandria()
	: Sys::Sys(spdlog::stdout_color_mt("alexandria"))
{

}
Alexandria::~Alexandria()
{

}

void Alexandria::init()
{

}
void Alexandria::kill()
{

}

void Alexandria::update()
{
	
}

void Alexandria::load_database(const std::string filename)
{
	sel::State lua;
	lua.Load(filename);

	const std::string type = lua["db_type"];
	const std::string name = lua["db_name"];
	std::string id_root = "/" + name + "/";

	// dont forget to normalize paths...

	if(type == "local") {
		sel::Selector data = lua["data"];
		load_folder(data, id_root);
	} else {
		m_log->error("We don't support databases of type `{}` yet!", type);
	}
}
void Alexandria::load_folder(const sel::Selector& root, const std::string old_id)
{
	uint i = 1;
	sel::Selector node = root[i];

	while(node.exists()) {
		const std::string sub_id = node["id"];
		if(node["type"] == "folder") {
			load_folder(node["contents"], old_id + sub_id + "/");
		}
		else {
			add_asset(old_id + sub_id, node["type"], node);
		}
		node = root[++i];
	}
}

Asset* Alexandria::get_asset(const std::string id)
{
	auto hash = HASH(id);

	// Find asset
	auto asset = m_assets.find(hash);
	if(asset != m_assets.end()) {
		if(asset->second->loaded == false)
			asset->second->load();
		return asset->second.get();
	}
	// Asset is not in our database!
	m_log->info("Asset `{}` is not in our database!", id);
	return nullptr;
}
void Alexandria::add_asset(const std::string id, const std::string type, const sel::Selector& root)
{
	auto hash = HASH(id);
	if(m_assets.find(hash) != m_assets.end()) {
		m_log->info("Asset `{}` already in our database, skipping.", id);
		return;
	}

	if(type == "font") {
		m_log->info("Adding font `{}`...", id);
		m_assets[hash] = t_asset_ptr(new FontAsset());
	}
}