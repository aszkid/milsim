#pragma once

#include <cstdint>
#include <apathy/path.hpp>
#include <glm/glm.hpp>

#include "sys/render.hpp"

namespace MilSim {

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

		uint32_t m_layer;
		RenderResource m_program;
		std::string m_vert_source;
		std::string m_frag_source;
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

}