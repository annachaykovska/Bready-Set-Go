#pragma once
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <vector>

#include <stbi/stb_image.h>

#include "Model.h"
#include <glad/glad.h>

Model::Model()
{
	this->visible = true;
}

Model::Model(const Model& rhs)
{
	this->visible = true;
	this->meshes = rhs.meshes;
	this->directory = rhs.directory;
	this->textures_loaded = rhs.textures_loaded;
}

Model::Model(Mesh mesh)
{
	this->visible = true;
	this->meshes.push_back(mesh);
}

// Renders every mesh in this model
void Model::draw(Shader &shader)
{
	if (visible)
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].draw(shader);
}

void Model::loadModel(std::string path)
{
	// Declare Assimp's (model loading library) importer to read a model file
	Assimp::Importer importer;

	// Dump all of the data in the model file into Assimp's scene object
	// A scene object holds a tree of multiple node objects which contain multiple meshses each
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_OptimizeMeshes);

	// Error check our new scene object
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	// Save a string of the directory containing the model for later
	directory = path.substr(0, path.find_last_of('/'));

	// Start processing each node in the scene tree starting at the root
	processNode(scene->mRootNode, scene);
}

// Extracts all the meshes in a node and then recursively calls itself
// on any further nodes in the scene tree
void Model::processNode(aiNode* node, const aiScene* scene)
{
	// Iterate through all the meshes in this node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// Create an Assimp mesh object for each mesh
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		// Add the new mesh to a vector of all the meshes in this model
		meshes.push_back(processMesh(mesh, scene));
	}

	// Recursively search every child node of this node
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

// Extracts all of the relevant mesh data from a model's node
// and save it to memory in an easy to use form
Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// Iterates through each vertex in the mesh
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		// Declare new vertex struct
		Vertex vertex;

		// Get vertex data
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		// Get normal data
		if (mesh->HasNormals())
		{
			glm::vec3 normal;
			normal.x = mesh->mNormals[i].x;
			normal.y = mesh->mNormals[i].y;
			normal.z = mesh->mNormals[i].z;
			vertex.normal = normal;
		}

		// Get texture mapping data
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 tex;
			tex.x = mesh->mTextureCoords[0][i].x;
			tex.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = tex;
		}
		else
			vertex.texCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	// Iterate through all the indices in the mesh
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	Mesh newMesh(vertices, indices, textures);

	// If the mesh has texture image files associated with it, get those too
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		aiString materialName;
		material->Get(AI_MATKEY_NAME, materialName);

		aiColor3D diffuseColor;
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);

		aiColor3D specularColor;
		material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);

		aiColor3D ambientColor;
		material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);

		aiColor3D emissiveColor;
		material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);

		float shininess;
		material->Get(AI_MATKEY_SHININESS, shininess);
		
		float opacity;
		material->Get(AI_MATKEY_OPACITY, opacity);

		float refractionIndex;
		material->Get(AI_MATKEY_REFRACTI, refractionIndex);

		newMesh.material.name = std::string(materialName.C_Str());
		newMesh.material.ambient = glm::vec3(ambientColor.r, ambientColor.g, ambientColor.b);
		newMesh.material.diffuse = glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);
		newMesh.material.specular = glm::vec3(specularColor.r, specularColor.g, specularColor.b);
		newMesh.material.emissive = glm::vec3(emissiveColor.r, emissiveColor.g, emissiveColor.b);
		newMesh.material.shininess = shininess;
		newMesh.material.opacity = opacity;
		newMesh.material.refractionIndex = refractionIndex;

		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		for (auto it = diffuseMaps.begin(); it < diffuseMaps.end(); it++)
			newMesh.textures.push_back(*it);

		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		for(auto it = diffuseMaps.begin(); it < diffuseMaps.end(); it++)
			newMesh.textures.push_back(*it);
	}
	
	return newMesh;
}

// Loads texture data for meshes
std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;

	// Iterate through each texture in the mesh
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		bool skip = false;

		// Check if this texture has already been loaded
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}

		// Read a new texture from a file
		if (!skip)
		{
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), directory, false);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}

	return textures;
}

unsigned int Model::TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	// Create a new texture in openGL
	unsigned int textureID;
	glGenTextures(1, &textureID);

	// Read the image data from the file
	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		else
			format = GL_RGB;

		// Bind a new texture
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Write the texture data to a buffer for openGL
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Settings
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Free the image data from RAM
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void Model::physicsVerts(std::vector<physx::PxVec3>* verts)
{
	for (int i = 0; i < meshes.size(); i++)
		for (int j = 0; j < meshes[i].vertices.size(); j++)
			verts->push_back(physx::PxVec3(meshes[i].vertices[j].position.x, meshes[i].vertices[j].position.y, meshes[i].vertices[j].position.z));
}

void Model::physicsIndices(std::vector<physx::PxU32>* indices)
{
	unsigned int offset = 0;

	for (int i = 0; i < meshes.size(); i++)
	{
		for (int j = 0; j < meshes[i].vertices.size(); j++)
			indices->push_back(physx::PxU32(meshes[i].indices[j] + offset));

		offset += indices->size();
	}
}

void Model::drawDepthMap(Shader& shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].drawDepthMap(shader);
}