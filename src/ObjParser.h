#pragma once
#include "Group.h"
#include "Point.h"
#include "Triangle.h"
#include <vector>

class ObjParser
{
public:
    ObjParser() { };
    ObjParser(const std::string &filename);
    ObjParser(std::istream &is);
    
    std::vector<Point> vertices;
    std::vector<Vector> normals;

    std::shared_ptr<Group> obj;
    std::shared_ptr<Group> cur_group;

private:

    void parse(std::istream &is);

};