#pragma once

#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

class Model
{
public:

	std::vector<Mesh> meshes;

	Model(char* path) { loadModel(path); }
	void draw(Shader& shader);
	bool isTextured() { return (textures_loaded.size() != 0); }

private:

	std::string directory;
	std::vector<Texture> textures_loaded;
	
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma);
};