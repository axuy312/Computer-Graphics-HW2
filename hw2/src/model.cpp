#include "model.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <glm/vec3.hpp>

Model* Model::fromObjectFile(const char* obj_file) {
  Model* m = new Model();

  std::ifstream ObjFile(obj_file);

  if (!ObjFile.is_open()) {
    std::cout << "Can't open File !" << std::endl;
    return NULL;
  }

  /* TODO#1: Load model data from OBJ file
   *         You only need to handle v, vt, vn, f
   *         Other fields you can directly ignore
   *         Fill data into m->positions, m->texcoords m->normals and m->numVertex
   *         Data format:
   *           For positions and normals
   *         | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    | 8    | 9    | 10   | 11   | ...
   *         | face 1                                                       | face 2               ...
   *         | v1x  | v1y  | v1z  | v2x  | v2y  | v2z  | v3x  | v3y  | v3z  | v1x  | v1y  | v1z  | ...
   *         | vn1x | vn1y | vn1z | vn2x | vn2y | vn2z | vn3x | vn3y | vn3z | vn1x | vn1y | vn1z | ...
   *           For texcoords
   *         | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    | ...
   *         | face 1                                  | face 2        ...
   *         | v1x  | v1y  | v2x  | v2y  | v3x  | v3y  | v1x  | v1y  | ...
   * Note:
   *        OBJ File Format (https://en.wikipedia.org/wiki/Wavefront_.obj_file)
   *        For simplification, all faces from OBJ file are triangle (vertex per face = 3)
   */

  std::string line = "";
  char *token;
  std::string prefix = "";
  std::stringstream ss, ss2;
  
  //¦spositions, texcoords, normals
  std::vector<float*> positions;
  std::vector<float*> texcoords;
  std::vector<float*> normals;

  positions.push_back(nullptr);
  texcoords.push_back(nullptr);
  normals.push_back(nullptr);
  m->numVertex = 0;
  m->drawMode = GL_TRIANGLES;
  while (getline(ObjFile, line)) {
    ss.clear();
    ss.str(line);
    ss >> prefix;
    if (prefix == "v") {
      float* pos = new float[3];
      ss >> pos[0] >> pos[1] >> pos[2];
      positions.push_back(pos);
    } else if (prefix == "vt") {
      //std::cout << line << "\n";
      float* pos = new float[2];
      ss >> pos[0] >> pos[1];
      texcoords.push_back(pos);
    } else if (prefix == "vn") {
      float* pos = new float[3];
      ss >> pos[0] >> pos[1] >> pos[2];
      normals.push_back(pos);
    } else if (prefix == "f") {
      //std::cout << line << "\n";
      while (ss >> line) {
        char* dup = _strdup(line.c_str());
        char* pTmp = NULL;
        //push the positions
        token = strtok_s(dup, "/", &pTmp);
        int idx = std::stoi(token);
        m->positions.push_back(positions[idx][0]);
        m->positions.push_back(positions[idx][1]);
        m->positions.push_back(positions[idx][2]);

        // push the texcoords
        token = strtok_s(NULL, "/", &pTmp);
        idx = std::stoi(token);
        m->texcoords.push_back(texcoords[idx][0]);
        m->texcoords.push_back(texcoords[idx][1]);

        // push the normals
        token = strtok_s(NULL, "/", &pTmp);
        idx = std::stoi(token);
        m->normals.push_back(normals[idx][0]);
        m->normals.push_back(normals[idx][1]);
        m->normals.push_back(normals[idx][2]);
        
        m->numVertex++;

        free(dup);
      }
    }
  }
  if (ObjFile.is_open()) ObjFile.close();

  return m;
}
