#pragma once
#include "yaml-cpp/yaml.h"
#include "World.h"
#include "Plane.h"
#include "Sphere.h"
#include "Cube.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Camera.h"
#include "Pattern.h"
#include "Light.h"
#include "Group.h"
#include "CSG.h"
#include "ObjParser.h"
#include "UVPattern.h"
#include <memory>
#include <map>
#include <exception>

class SceneConfig {
public:

    SceneConfig(const YAML::Node &yaml);
    SceneConfig(const std::string &filename);

    World getWorld() const {
        return world;
    }
    Camera getCamera() const {
        return camera;
    }
    size_t getWidth() const {
        return camera.hsize;
    }
    size_t getHeight() const {
        return camera.vsize;
    }

private:

    void parse_yaml();
    void parse_yaml_define(const std::string &name, const YAML::Node &node);
    void parse_yaml_add(const std::string &type, const YAML::Node &node, const std::shared_ptr<Shape> &parent = nullptr);
    void parse_yaml_add_camera(const YAML::Node &node);
    void parse_yaml_add_light(const YAML::Node &node);
    std::shared_ptr<Shape> parse_yaml_make_shape(const std::string &type, const YAML::Node &node, const std::shared_ptr<Shape> &parent = nullptr);
    std::shared_ptr<Shape> parse_yaml_make_sphere(const YAML::Node &node, const std::shared_ptr<Shape> &parent = nullptr);
    std::shared_ptr<Shape> parse_yaml_make_plane(const YAML::Node &node, const std::shared_ptr<Shape> &parent = nullptr);
    std::shared_ptr<Shape> parse_yaml_make_cube(const YAML::Node &node, const std::shared_ptr<Shape> &parent = nullptr);
    std::shared_ptr<Shape> parse_yaml_make_cylinder(const YAML::Node &node, const std::shared_ptr<Shape> &parent = nullptr);
    std::shared_ptr<Shape> parse_yaml_make_cone(const YAML::Node &node, const std::shared_ptr<Shape> &parent = nullptr);
    std::shared_ptr<Shape> parse_yaml_make_group(const YAML::Node &node, const std::shared_ptr<Shape> &parent = nullptr);
    std::shared_ptr<Shape> parse_yaml_make_csg(const YAML::Node &node, const std::shared_ptr<Shape> &parent = nullptr);
    std::shared_ptr<Shape> parse_yaml_make_obj(const YAML::Node &node, const std::shared_ptr<Shape> &parent = nullptr);
    std::shared_ptr<Shape> parse_yaml_make_shape_common(const std::shared_ptr<Shape> &s, const YAML::Node &node, const std::shared_ptr<Shape> &parent = nullptr);
    std::shared_ptr<Pattern> parse_yaml_make_pattern(const YAML::Node &node, const std::shared_ptr<Shape> &shape);
    std::shared_ptr<UVPattern> parse_yaml_make_uv_pattern(const YAML::Node &node);


    void parse_yaml_apply_transform(const YAML::Node &node, Matrix &transform);

    bool lookup_defined_yaml_node(const std::string &name, YAML::Node &dest_out);

    void yaml_error(const YAML::Node &node, const std::string &error);
    void yaml_warning(const YAML::Node &node, const std::string &error);

    YAML::Node yaml;
    std::map<std::string, YAML::Node> define_map; // holds the YAML::Node 
                                                  // given by each of our yaml 'define' objects
    World world;
    Camera camera;

    // hacky utility function to do what yaml-cpp cannot
    YAML::Node merge_nodes(YAML::Node a, YAML::Node b);
    inline const YAML::Node & cnode(const YAML::Node &n) {
        return n;
    }

};

namespace YAML
{
    template <>
    struct convert<Vector>
    {
        static Node encode(const Vector &rhs)
        {
            Node node;
            node.push_back(rhs.x());
            node.push_back(rhs.y());
            node.push_back(rhs.z());
            return node;
        }

        static bool decode(const Node &node, Vector &rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
            {
                return false;
            }
            rhs[0] = node[0].as<double>();
            rhs[1] = node[1].as<double>();
            rhs[2] = node[2].as<double>();
            rhs[3] = 0;

            return true;
        }
    };
    template <>
    struct convert<Point>
    {
        static Node encode(const Point &rhs)
        {
            Node node;
            node.push_back(rhs.x());
            node.push_back(rhs.y());
            node.push_back(rhs.z());
            return node;
        }

        static bool decode(const Node &node, Point &rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
            {
                return false;
            }
            rhs[0] = node[0].as<double>();
            rhs[1] = node[1].as<double>();
            rhs[2] = node[2].as<double>();
            rhs[3] = 1;

            return true;
        }
    };
    template <>
    struct convert<Color>
    {
        static Node encode(const Color &rhs)
        {
            Node node;
            node.push_back(rhs.r());
            node.push_back(rhs.g());
            node.push_back(rhs.b());
            return node;
        }

        static bool decode(const Node &node, Color &rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
            {
                return false;
            }
            rhs[0] = node[0].as<double>();
            rhs[1] = node[1].as<double>();
            rhs[2] = node[2].as<double>();
            rhs[3] = 0;

            return true;
        }
    };
} // namespace YAML
