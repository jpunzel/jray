#include "ObjParser.h"
#include <fstream>

ObjParser::ObjParser(const std::string &filename) : obj(Group::make()), cur_group(obj)
{
    std::ifstream file;
    file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    file.open(filename);
    file.exceptions(std::ifstream::badbit);
    std::cout << "Parsing file " << filename << "..." << std::flush;
    parse(file);
    std::cout << " Done." << std::endl;
    file.close();
}

ObjParser::ObjParser(std::istream &is) : obj(Group::make()), cur_group(obj)
{
    parse(is);
}

void ObjParser::parse(std::istream &is)
{
    std::string line;
    size_t linenum = 0;

    while (std::getline(is, line)) {
        linenum++;
        //std::cout << "Line num: " << linenum << std::endl;
        std::istringstream iss(line);
        std::string cmd;
        if ( !(iss >> cmd) ) { continue; }

        if ( cmd == "v" ) {
            Point p;
            if ( iss >> p ) {
                vertices.push_back(p);
            }
        }

        if ( cmd == "vn" ) {
            Vector n;
            if ( iss >> n ) {
                normals.push_back(n);
            }
        }

        if ( cmd == "f" ) {
            // vectors to hold parts of each space-deliniated vertex parameter
            std::vector<int> vindices; // vertex indices
            std::vector<int> nindices; // normal indices

            std::string vertex_tmp; // vertex specifier string, e.g. 1/2/3 or 2/102/1 or 4//303
            int vindex_tmp; // vertex index component
            int nindex_tmp; // normal index component
            bool normals_given = true; // will set this to false later if normal is unset for any vertex
            int nnormals = 0;

            while ( iss >> vertex_tmp ) {
                nindex_tmp = 0; // ensures any unset normal index has value 0 (works nicely since obj indices begin at 1)
                std::istringstream vertex_iss(vertex_tmp);
                std::string substr;

                std::vector<std::string> parts;
                while (vertex_iss.good()) {
                    std::getline(vertex_iss, substr, '/');
                    parts.push_back(substr);
                }
                vindex_tmp = std::stoi(parts[0]);
                if (parts.size() == 3) {
                    nindex_tmp = std::stoi(parts[2]);
                    nnormals++;
                }

                if (abs(vindex_tmp) > vertices.size()) { continue; }
                vindices.push_back(vindex_tmp);
                nindices.push_back(nindex_tmp); // will push 0 if field left blank, which is fine
            }
            int npoints = vindices.size();
            if (npoints < 3 || npoints > vertices.size() ) { continue; }
            if (nnormals > normals.size()) { continue; }

            if (nnormals < npoints) {
                normals_given = false;
            }
            
            // Triangulation loop for any arbitrary polygon.
            for (int i = 1; i <= (npoints - 2); ++i) {
                std::shared_ptr<Shape> t;
                int vi1 = vindices[0]-1;
                int vi2 = vindices[i]-1;
                int vi3 = vindices[i+1]-1;
                int ni1 = nindices[0]-1;
                int ni2 = nindices[i]-1;
                int ni3 = nindices[i+1]-1;

                vi1 = vi1 < 0 ? vertices.size() + vi1 + 1 : vi1;
                vi2 = vi2 < 0 ? vertices.size() + vi2 + 1 : vi2;
                vi3 = vi3 < 0 ? vertices.size() + vi3 + 1 : vi3;
                ni1 = ni1 < 0 ? normals.size() + ni1 + 1 : ni1;
                ni2 = ni2 < 0 ? normals.size() + ni2 + 1 : ni2;
                ni3 = ni3 < 0 ? normals.size() + ni3 + 1 : ni3;

                //std::cout << "Making Triangle " << vertices[vi1] << vertices[vi2] << vertices[vi3] << std::endl;
                if (normals_given) {
                    t = Triangle::make(vertices[vi1], vertices[vi2], vertices[vi3], normals[ni1], normals[ni2], normals[ni3]);
                } else {
                    t = Triangle::make(vertices[vi1], vertices[vi2], vertices[vi3]);
                }
                cur_group->addChild(t);
            }
        }

        // named groups are kinda pointless but we'll use the g command as a hint
        // to create a subgroup.
        if ( cmd == "g" ) {
            auto g = Group::make();
            cur_group = g;
            obj->addChild(g);
        }


    }

    obj->divide(4);

}