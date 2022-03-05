#pragma once

#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <PxPhysicsAPI.h>

#include "Mesh.h"

class Model : public Component
{
public:

	std::vector<Mesh> meshes;
	std::vector<Texture> textures_loaded;
	bool visible;

	Model();
	Model(const Model& rhs);
	Model(char* path) { loadModel(path); }
	Model(Mesh mesh);
	void draw(Shader& shader);
	bool isTextured() { return (textures_loaded.size() != 0); }
	std::vector<physx::PxVec3> physicsVerts();
	std::vector<physx::PxU32> physicsIndices();

	//Model(Model& rhs) = delete; // No copy constructor
	//void operator=(const Model&) = delete; // No assignment operator

private:

	std::string directory;
	
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma);
};