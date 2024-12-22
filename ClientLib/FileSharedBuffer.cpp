#include "FileSharedBuffer.h"
#include "MeshComponent.h"

static void LoadMTL(const char* filePath, char*& textureName) {
    FILE* file = fopen(filePath, "r");
    if (!file) {
        //DXUT_ERR_MSGBOX("Failed to open .mtl file: %s", 0, filePath);
        return;
    }

    char line[256];

    while (fgets(line, (unsigned)_countof(line), file)) {
        if (strncmp(line, "map_Kd", 6) == 0) {
            char texturePath[MAX_PATH];
            sscanf_s(line, "map_Kd %s", texturePath);
            std::size_t textureNameSize = strlen(texturePath) + 1;
            textureName = (char*)malloc(textureNameSize * sizeof(char));
            if (!textureName)
                return;
                //DXUT_ERR_MSGBOX("Failed to allocate memory for texture name.", 0);
            strcpy_s(textureName, textureNameSize, texturePath);
            break;
        }
    }

    fclose(file);
}

inline void MeshLoader::LoadFile(const char* filePath, std::vector<Vertex>& vertices, std::vector<DWORD>& indices) {
	FILE* file = fopen(filePath, "rb");
    if (!file)
        return;// DXUT_ERR_MSGBOX("File not found or don't access %s.", 0, filePath);

    struct VertexHash {
        std::size_t operator()(const Vertex& vertex) const {
            return ((std::hash<float>()(vertex.position.x) ^
                (std::hash<float>()(vertex.position.y) << 1)) >> 1) ^
                (std::hash<float>()(vertex.position.z) << 1) ^
                ((std::hash<float>()(vertex.texCoord.x) ^
                    (std::hash<float>()(vertex.texCoord.y) << 1)) >> 1) ^
                ((std::hash<float>()(vertex.normal.x) ^
                    (std::hash<float>()(vertex.normal.y) << 1)) >> 1) ^
                (std::hash<float>()(vertex.normal.z) << 1);
        }
    };

    std::vector<XMFLOAT3> positions;
    std::vector<XMFLOAT2> texCoords;
    std::vector<XMFLOAT3> normals;
    std::unordered_map<Vertex, DWORD, VertexHash> vertexMap;

    char line[256];
    while (fgets(line, (unsigned)_countof(line), file)) {
        if (strncmp(line, "mtllib", 6) == 0) {
            char mtlFilePath[256];
            sscanf_s(line, "mtllib %s", mtlFilePath);
            LoadMTL(mtlFilePath, material);
        }
        else if (line[0] == 'v' && line[1] == ' ') {
            XMFLOAT3 pos{};
            sscanf_s(line, "v %f %f %f", &pos.x, &pos.y, &pos.z);
            positions.push_back(pos);
        }
        else if (line[0] == 'v' && line[1] == 'n') {
            XMFLOAT3 normal{};
            sscanf_s(line, "vn %f %f %f", &normal.x, &normal.y, &normal.z);
            normals.push_back(normal);
        }
        else if (line[0] == 'v' && line[1] == 't') {
            XMFLOAT2 tex{};
            sscanf_s(line, "vt %f %f", &tex.x, &tex.y);
            texCoords.push_back(tex);
        }
        else if (line[0] == 'f') {
            uint32_t posIndex[3]{}, texIndex[3]{}, normIndex[3]{};
            sscanf_s(line, "f %u/%u/%u %u/%u/%u %u/%u/%u",
                &posIndex[0], &texIndex[0], &normIndex[0],
                &posIndex[1], &texIndex[1], &normIndex[1],
                &posIndex[2], &texIndex[2], &normIndex[2]);

            for (int i = 0; i < 3; ++i) {
                Vertex vertex{};
                vertex.position = positions[posIndex[i] - 1];
                vertex.texCoord = texCoords[texIndex[i] - 1];
                vertex.normal = normals[normIndex[i] - 1];

                auto it = vertexMap.find(vertex);
                if (it != vertexMap.end()) {
                    indices.push_back(it->second);
                }
                else {
                    DWORD newIndex = static_cast<DWORD>(vertices.size());
                    vertices.push_back(vertex);
                    indices.push_back(newIndex);
                    vertexMap[vertex] = newIndex;
                }
            }
        }
    }

    fclose(file);
}