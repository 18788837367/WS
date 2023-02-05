#pragma once

#include <iostream>
#include <vector>
#include "shader.h"
#include "Mesh.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <stb_image.h>

class Model
{
public:
	Model(const std::string& path,bool gamma = false);
	void Draw(Shader shader);

	std::vector<Texture> textures_loaded;
	std::vector<Mesh>  meshes;
	std::string directory;
	bool gammaCorrection;

private:
	void loadModel(const std::string& path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char* path, const std::string &difectory, bool gamma = false);
};

