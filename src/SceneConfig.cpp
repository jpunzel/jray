#include "SceneConfig.h"
#include <exception>

SceneConfig::SceneConfig(const YAML::Node &yaml) : yaml(yaml)
{
    parse_yaml();
}
SceneConfig::SceneConfig(const std::string &filename)
{
  	try {
		yaml = YAML::LoadFile(filename);
	} catch (YAML::Exception &e) {
		std::cerr << "Error parsing YAML file: " << e.what() << std::endl;
	}
    parse_yaml();
}

void SceneConfig::parse_yaml()
{
    if ( !yaml.IsSequence() ) {
        std::cerr << "YAML Error: File needs to be a list (each 'add' or 'define' preceded with -)" << std::endl;
        exit(1);
    }
    for( YAML::const_iterator it=yaml.begin(); it != yaml.end(); ++it ) {
        if ( !it->IsMap() ) {
            yaml_error( *it, "Not a map object");
            continue;
        }
        if ( (*it)["add"] && !(*it)["define"] ) {
            parse_yaml_add( (*it)["add"].as<std::string>() , *it );
        } else if ( (*it)["define"] && !(*it)["add"] ) {
            parse_yaml_define( (*it)["define"].as<std::string>() , *it );
        } else {
            yaml_error( *it, "Object must be either an 'add' or 'define'");
            continue;
        }

    }
}


void SceneConfig::parse_yaml_define(const std::string &name, const YAML::Node &node)
{
    if ( define_map.find(name) != define_map.end() ) {
        yaml_error(node, "Name " + name + " already defined");
    } else if ( node["extend"] ) {
        std::string to_extend = node["extend"].as<std::string>();
        auto it = define_map.find(to_extend);
        if ( it != define_map.end() ) {
            YAML::Node src_value = it->second;
            if ( !src_value.IsMap() ) {
                yaml_error(node, "Value to extend is not a map");
                return;
            }
            if ( node["value"] ) {
                // Assemble replacement node
                YAML::Node repl = YAML::Clone(src_value);
                for ( YAML::const_iterator it2 = node["value"].begin(); it2 != node["value"].end(); ++it2 ) {
                    std::string key = it2->first.as<std::string>();
                    // Delete first if key exists in replacement node.
                    // yaml-cpp does not merge keys?
                    if ( repl[key] ) {
                        repl.remove(key);
                    }
                    repl[key] = it2->second;
                }

                //YAML::Node repl = merge_nodes(src_value, node["value"]);
                define_map.insert(std::make_pair(name, repl));
            } else {
                yaml_error(node, "Cannot extend: no 'value' object specified");
                return;
            }

        } else {
            yaml_error(node, "Cannot extend: definition '" + to_extend + "' does not exist");
            return;
        }

    } else if ( node["value"] ) {
        define_map.insert(std::make_pair(name, node["value"] ));
    } else {
        yaml_error(node, "No 'value' object found with definition of '" + name + "'");
        return;
    }
}

void SceneConfig::parse_yaml_add(const std::string &type, const YAML::Node &node, const std::shared_ptr<Shape> &parent)
{
    YAML::Node defined;
    std::shared_ptr<Shape> s;


    if (type == "camera")
        parse_yaml_add_camera(node);
    else if (type == "light")
        parse_yaml_add_light(node);
    else if (type == "sphere" || type == "plane" || type == "cube" || type == "cylinder" || type == "cone" ||
             type == "group" || type == "csg" || type == "obj") {
        if ( s = parse_yaml_make_shape(type, node, parent) ) {
            world.addShape(s);
        }
    }
    else if ( lookup_defined_yaml_node(type, defined) ) {
        // clone and extend defined node with data from current node
        YAML::Node merged = merge_nodes(node, defined);
        if (merged["add"]) {
            parse_yaml_add(merged["add"].as<std::string>(), merged, parent);
        }
    }
    else {
        yaml_error(node, "Unknown object type \"" + type + "\"");
    }
}

void SceneConfig::parse_yaml_add_camera(const YAML::Node &node)
{
    Point from, to;
    Vector up;

    if (node["width"]) {
        size_t w = node["width"].as<size_t>();
        if ( w <= 0 || w > 65536 ) {
            yaml_error(node, "Width is out of proper range!");
        }
        camera.hsize = w;
    }
    if (node["height"]) {
        size_t h = node["height"].as<size_t>();
        if ( h <= 0 || h > 65536 ) {
            yaml_error(node, "Height is out of proper range!");
        }
        camera.vsize = h;
    }
    if (node["field-of-view"]) {
        camera.setFov(node["field-of-view"].as<double>());
    }
    if (node["focal-length"]) {
        camera.setFocalLength(node["focal-length"].as<double>());
    }
    if (node["aperture"]) {
        camera.setAperture(node["aperture"].as<double>() / 2.0); // sets radius
    }
    if (node["focal-samples"]) {
        camera.setFocalSamples(node["focal-samples"].as<size_t>());
    }
    if (node["supersampling"]) {
        camera.setSupersamplingLevel(node["supersampling"].as<size_t>());
    }
    if ( (node["from"] || node["to"] || node["up"]) && 
        !(node["from"] && node["to"] && node["up"]) ) {
            yaml_error(node, "Camera object must include all three (or none) of: 'from', 'to', 'up'");
    } else if ( node["from"] && node["to"] && node["up"] ) {
        from = node["from"].as<Point>();
        to = node["to"].as<Point>();
        up = node["up"].as<Vector>();
        camera.setTransform(from, to, up);
    }
}

void SceneConfig::parse_yaml_add_light(const YAML::Node &node)
{
    Point p;
    Color intensity;
    
    if ( node["at"] ) {
        p = node["at"].as<Point>();
    } else if ( node["corner"] ) {
        p = node["corner"].as<Point>();   
    } else {
        yaml_error(node, "No point specified for light");
    }
    if ( node["intensity"] ) {
        intensity = node["intensity"].as<Color>();
    } else {
        yaml_error(node, "No intensity specified for light");
    }

    // If we don't have any attributes for an area light, add a simple point light.
    if ( !node["uvec"] && !node["vvec"] && !node["usteps"] && !node["vsteps"] && !node["jitter"] ) {
        world.addLight(Light(p, intensity));
    } else if (node["uvec"] && node["vvec"] && node["usteps"] && node["vsteps"]) {
        Vector uvec = node["uvec"].as<Vector>();
        Vector vvec = node["vvec"].as<Vector>();
        int usteps = node["usteps"].as<int>();
        int vsteps = node["vsteps"].as<int>();
        bool jitter;
        if (node["jitter"]) {
            jitter = node["jitter"].as<bool>();
        } else {
            jitter = false;
        }
        world.addLight(Light(p, uvec, usteps, vvec, vsteps, intensity, jitter));
    } else {
        yaml_error(node, "Missing one of these attributes for area light: uvec, vvec, usteps, vsteps");
    }
}

std::shared_ptr<Shape> SceneConfig::parse_yaml_make_shape(const std::string &type, const YAML::Node &node, const std::shared_ptr<Shape> &parent)
{
    YAML::Node defined;

    if (type == "sphere")
        return parse_yaml_make_sphere(node, parent);
    else if (type == "plane")
        return parse_yaml_make_plane(node, parent);
    else if (type == "cube")
        return parse_yaml_make_cube(node, parent);
    else if (type == "cylinder")
        return parse_yaml_make_cylinder(node, parent);
    else if (type == "cone")
        return parse_yaml_make_cone(node, parent);
    else if (type == "group")
        return parse_yaml_make_group(node, parent);
    else if (type == "csg")
        return parse_yaml_make_csg(node, parent);
    else if (type == "obj")
        return parse_yaml_make_obj(node, parent);
    else if ( lookup_defined_yaml_node(type, defined) ) {
        // clone and extend defined node with data from current node
        YAML::Node merged = merge_nodes(node, defined);
        if (merged["add"]) {
            return parse_yaml_make_shape(merged["add"].as<std::string>(), merged, parent);
        }
    }
    else {
        yaml_error(node, "Unknown object type \"" + type + "\"");
    }
    return nullptr;
}

std::shared_ptr<Shape> SceneConfig::parse_yaml_make_sphere(const YAML::Node &node, const std::shared_ptr<Shape> &parent)
{
    std::shared_ptr<Shape> s = Sphere::make();
    return parse_yaml_make_shape_common(s, node, parent);
}

std::shared_ptr<Shape> SceneConfig::parse_yaml_make_plane(const YAML::Node &node, const std::shared_ptr<Shape> &parent)
{
    std::shared_ptr<Shape> p = Plane::make();
    return parse_yaml_make_shape_common(p, node, parent);
}

std::shared_ptr<Shape> SceneConfig::parse_yaml_make_cube(const YAML::Node &node, const std::shared_ptr<Shape> &parent)
{
    std::shared_ptr<Shape> c = Cube::make();
    return parse_yaml_make_shape_common(c, node, parent);
}

std::shared_ptr<Shape> SceneConfig::parse_yaml_make_cylinder(const YAML::Node &node, const std::shared_ptr<Shape> &parent)
{
    double min, max;
    min = max = std::numeric_limits<double>::infinity();
    bool closed = false;
    if (node["min"]) {
        if (node["min"].IsScalar())
            min = node["min"].as<double>();
        else
            yaml_error(node, "cylinder min must be numeric");
    }
    if (node["max"]) {
        if (node["max"].IsScalar())
            max = node["max"].as<double>();
        else
            yaml_error(node, "cylinder max must be numeric");
    }
    if (node["closed"]) {
        if (node["closed"].IsScalar())
            closed = node["closed"].as<bool>();
        else
            yaml_error(node, "cylinder closed flag must be boolean");
    }
    
    std::shared_ptr<Shape> c = Cylinder::make(min,max,closed);
    return parse_yaml_make_shape_common(c, node, parent);
}

std::shared_ptr<Shape> SceneConfig::parse_yaml_make_cone(const YAML::Node &node, const std::shared_ptr<Shape> &parent)
{
    double min, max;
    min = max = std::numeric_limits<double>::infinity();
    bool closed = false;
    if (node["min"]) {
        if (node["min"].IsScalar())
            min = node["min"].as<double>();
        else
            yaml_error(node, "cone min must be numeric");
    }
    if (node["max"]) {
        if (node["max"].IsScalar())
            max = node["max"].as<double>();
        else
            yaml_error(node, "cone max must be numeric");
    }
    if (node["closed"]) {
        if (node["closed"].IsScalar())
            closed = node["closed"].as<bool>();
        else
            yaml_error(node, "cone closed flag must be boolean");
    }
    
    std::shared_ptr<Shape> c = Cone::make(min,max,closed);
    return parse_yaml_make_shape_common(c, node, parent);
}

std::shared_ptr<Shape> SceneConfig::parse_yaml_make_group(const YAML::Node &node, const std::shared_ptr<Shape> &parent)
{
    auto g = Group::make();

    // recursively parse_yaml_make_shape() on every child in children
    if ( node["children"] && node["children"].IsSequence() ) {
        for ( YAML::const_iterator it = node["children"].begin(); it != node["children"].end(); ++it ) {
            if ( (*it)["add"] ) { 
                parse_yaml_make_shape( (*it)["add"].as<std::string>() , *it, g);
            }
        }
    }

    // Create BVH to vastly speed up large groups
    g->divide(4);

    return parse_yaml_make_shape_common(g, node, parent);
}

std::shared_ptr<Shape> SceneConfig::parse_yaml_make_csg(const YAML::Node &node, const std::shared_ptr<Shape> &parent)
{
    if ( !node["operation"] ) {
        yaml_error(node, "No CSG 'operation' specified");
    }
    std::string operation = node["operation"].as<std::string>();
    if ( operation != "union" && operation != "difference" && operation != "intersect" ) {
        yaml_error(node, "CSG operation must be one of: union, intersect, difference");
    }

    unsigned short op;
    if ( operation == "union" ) {
        op = CSG_UNION;
    } else if (operation == "intersect") {
        op = CSG_INTERSECT;
    } else if (operation == "difference") {
        op = CSG_DIFFERENCE;
    }

    auto c = CSG::make(op);

    if ( !node["left"] || !node["right"] ) {
        yaml_error(node, "CSG Shape requires both left and right children");
    }

    if ( node["left"] && node["left"].IsMap() ) {
        if ( node["left"]["type"] ) {
            parse_yaml_make_shape( node["left"]["type"].as<std::string>(), node["left"], c);
        }
    }
    if ( node["right"] && node["right"].IsMap() ) {
        if ( node["right"]["type"] ) {
            parse_yaml_make_shape( node["right"]["type"].as<std::string>(), node["right"], c);
        }
    }

    if ( !c->left || !c->right ) {
        yaml_error(node, "Bad CSG specified");
    }
    return parse_yaml_make_shape_common(c, node, parent);
}

std::shared_ptr<Shape> SceneConfig::parse_yaml_make_obj(const YAML::Node &node, const std::shared_ptr<Shape> &parent)
{
    std::string filename;
    if (node["file"]) {
        if (node["file"].IsScalar())
            filename = node["file"].as<std::string>();
        else
            yaml_error(node, "invalid filename");
    }

    ObjParser parser;
    try {
        parser = ObjParser(filename);
    } catch (const std::exception &e) {
        std::cerr << "Error reading file: " << filename << std::endl;
        exit(1);
    }

    return parse_yaml_make_shape_common(parser.obj, node, parent);
}

std::shared_ptr<Shape> SceneConfig::parse_yaml_make_shape_common(const std::shared_ptr<Shape> &s, const YAML::Node &node, const std::shared_ptr<Shape> &parent) 
{
    Material m;
    Matrix transform = Matrix::identity(4);

    if ( !parent && !node["material"] ) {
        yaml_warning(node, "No material specified for shape. Will use defaults.");
    } else if ( node["material"] ) {
        YAML::Node material_node;
        // Check for existing definition matching name
        if ( node["material"].IsScalar() ) {
            std::string def = node["material"].as<std::string>();
            if ( ! lookup_defined_yaml_node(def, material_node) ) {
                yaml_error(node, "Cannot find definition matching '" + def + "'");
            }
        } else {
            material_node = node["material"];
        }

        if ( material_node["color"] ) {
            m.setColor(material_node["color"].as<Color>());
        }
        if ( material_node["diffuse"] ) {
            m.setDiffuse(material_node["diffuse"].as<double>());
        }
        if ( material_node["ambient"] ) {
            m.setAmbient(material_node["ambient"].as<double>());
        }
        if ( material_node["specular"] ) {
            m.setSpecular(material_node["specular"].as<double>());
        }
        if ( material_node["shininess"] ) {
            m.setShininess(material_node["shininess"].as<double>());
        }
        if ( material_node["reflective"] ) {
            m.setReflective(material_node["reflective"].as<double>());
        }
        if ( material_node["transparency"] ) {
            m.setTransparency(material_node["transparency"].as<double>());
        }
        if ( material_node["refractive-index"] ) {
            m.setRefractiveIndex(material_node["refractive-index"].as<double>());
        }

        if ( material_node["pattern"] ) {
            // Pattern can be either a sequence of 3 float values (a Color to construct as a SolidPattern)
            // or:
            // A map containing the following:
            // type: <checkers, ring, gradient, stripes, blended, xzgrid, yzgrid, xygrid>
            // pattern_a: first sub-pattern
            // pattern_b: second sub-pattern

            std::shared_ptr<Pattern> ptr;
            if ( (ptr = parse_yaml_make_pattern(material_node["pattern"], s)) != nullptr ) {
                m.setPattern( ptr );
            } else {
                yaml_error(node, "Could not construct pattern");
            }
        }

        s->setMaterial(m);

    } // end material handling

    if ( node["transform"] ) {
        for ( YAML::const_iterator it = node["transform"].begin(); it != node["transform"].end(); ++it ) {
            parse_yaml_apply_transform(*it, transform);
        }
    }

    if ( node["shadow"] ) {
        if ( node["shadow"].IsScalar() ) {
            std::string val = node["shadow"].as<std::string>();
            if ( val == "true" ) {
                s->castsShadow(true);
            } else if ( val == "false" ) {
                s->castsShadow(false);
            } else {
                yaml_error(node, "shadow must be true or false");
            }
        } else {
            yaml_error(node, "Invalid shadow flag");
        }
    }

    s->setTransform(transform);

    if (parent) {
        std::shared_ptr<Group> g = std::dynamic_pointer_cast<Group>(parent);
        std::shared_ptr<CSG> c = std::dynamic_pointer_cast<CSG>(parent);
        if (g) {
            g->addChild(s);
        }
        if (c) {
            c->addChild(s);
        }
    }

    return s;

}

std::shared_ptr<Pattern> SceneConfig::parse_yaml_make_pattern(const YAML::Node &node, const std::shared_ptr<Shape> &shape)
{
    // Handle base case where pattern is a sequence representing a solid pattern color.
    if ( node.IsSequence() ) {
        if ( node.size() == 3 ) {
            double r = node[0].as<double>();
            double g = node[1].as<double>();
            double b = node[2].as<double>();
            return SolidPattern::make( Color(r,g,b) );
        } else {
            yaml_error(node, "Pattern sequence value must be a 3 float value RGB array");
            return nullptr;
        }
    }

    // Handle case where pattern is a scalar, a string, and exists in the define_map
    if ( node.IsScalar() ) {
        std::string def = node.as<std::string>();
        YAML::Node src_node;
        if ( lookup_defined_yaml_node(def, src_node) ) {
            return parse_yaml_make_pattern(src_node, shape);
        } else {
            yaml_error(node, "Pattern definition '" + def + "' not found");
            return nullptr;
        }
    }
    
    // Handle case where pattern is a YAML map. Recursively descend to construct sub-patterns.
    if ( node.IsMap() ) {

        std::string type;
        Matrix transform = Matrix::identity(4);

        if ( node["type"] ) {
            type = node["type"].as<std::string>();
        }
        if ( !node["type"] || ( type != "map" && type != "checkers" && type != "ring" && type != "gradient" && type != "stripes" &&
                                type != "blended" && type != "xzgrid" && type != "yzgrid" && type != "xygrid") ) {
            yaml_error(node, "Pattern definition must include type: 'map', 'checkers', 'ring', 'gradient', 'stripes', 'blended', 'xzgrid', 'yzgrid', 'xygrid'");
            return nullptr;
        }


        // Handle special grid pattern types first. These do not have sub-patterns.
        if ( type == "xzgrid" )
            return XZGridPattern::make(shape);
        if ( type == "yzgrid" )
            return YZGridPattern::make(shape);
        if ( type == "xygrid" )
            return XYGridPattern::make(shape);

        // Adjust transform matrix if needed using same process as shapes
        if ( node["transform"] ) {
            for ( YAML::const_iterator it = node["transform"].begin(); it != node["transform"].end(); ++it ) {
                parse_yaml_apply_transform(*it, transform);
            }
        }

        // Handle UV Map patterns. These require us to first construct a UVPattern instance and determine
        // the appropriate UV mapping function.
        if ( type == "map" ) {
            std::string mapping;
            if ( node["mapping"] ) {
                mapping = node["mapping"].as<std::string>();
            }
            if ( !node["mapping"] || ( mapping != "spherical" && mapping != "cylindrical" && mapping != "planar" && mapping != "cube") ) {
                yaml_error(node, "Pattern type 'map' must include 'mapping' directive of one of the following: spherical, cylindrical, planar, cube");
                return nullptr;
            }
            std::shared_ptr<UVPattern> uvpattern;
            std::shared_ptr<UVPattern> uvp_left, uvp_right, uvp_up, uvp_down, uvp_front, uvp_back;
            if (!node["uv_pattern"] && ! (node["left"] && node["right"] && node["up"] && node["down"] && node["front"] && node["back"])) {
                yaml_error(node, "Bad uv_pattern specified");
                return nullptr;
            }
            if ( mapping == "cube" && node["left"] && node["right"] && node["up"] && node["down"] && node["front"] && node["back"] ) {
                uvp_left = parse_yaml_make_uv_pattern(node["left"]);
                uvp_right = parse_yaml_make_uv_pattern(node["right"]);
                uvp_up = parse_yaml_make_uv_pattern(node["up"]);
                uvp_down = parse_yaml_make_uv_pattern(node["down"]);
                uvp_front = parse_yaml_make_uv_pattern(node["front"]);
                uvp_back = parse_yaml_make_uv_pattern(node["back"]);
                if ( !uvp_left || !uvp_right || !uvp_up || !uvp_down || !uvp_front || !uvp_back ) {
                    yaml_error(node, "Bad cube face pattern given");
                    return nullptr;
                }
            } else if ( node["uv_pattern"] ){
                uvpattern = parse_yaml_make_uv_pattern(node["uv_pattern"]);
                if (!uvpattern) {
                    yaml_error(node, "Could not create uv pattern");
                    return nullptr;
                }
            } else {
                yaml_error(node, "Missing uv pattern specification");
                return nullptr;
            }
            if ( mapping == "spherical" ) {
                return TextureMapPattern::make(transform, uvpattern, &UVPattern::sphericalMap);
            }
            if ( mapping == "planar" ) {
                return TextureMapPattern::make(transform, uvpattern, &UVPattern::planarMap);
            }
            if ( mapping == "cylindrical" ) {
                return TextureMapPattern::make(transform, uvpattern, &UVPattern::cylindricalMap);
            }
            if ( mapping == "cube" ) {
                return CubeMapPattern::make(transform, uvp_left, uvp_right, uvp_front, uvp_back, uvp_up, uvp_down);
            }
        }

        // Now handle sub-patterns
        if ( ! ( (node["pattern_a"] && node["pattern_b"]) || node["colors"] ) ||
                ( node["pattern_a"] && node["pattern_b"] && node["colors"] ) ) {
            yaml_error(node, "Pattern of this type must include: 'pattern_a' and 'pattern_b' OR 'colors'");
            return nullptr;
        }

        std::shared_ptr<Pattern> pa;
        std::shared_ptr<Pattern> pb;

        if ( node["colors"] && node["colors"].IsSequence() && node["colors"].size() == 2 ) {
            auto c0 = node["colors"][0];
            auto c1 = node["colors"][1];
            if ( c0.size() == 3 && c1.size() == 3 ) {
                double r0 = c0[0].as<double>();
                double g0 = c0[1].as<double>();
                double b0 = c0[2].as<double>();
                double r1 = c1[0].as<double>();
                double g1 = c1[1].as<double>();
                double b1 = c1[2].as<double>();
                pa = SolidPattern::make( Color(r0,g0,b0) );
                pb = SolidPattern::make( Color(r1,g1,b1) );
            } else {
                yaml_error(node, "Color sequence values must be a 3 float value RGB array");
                return nullptr;
            }
        } else if (node["colors"]) {
            yaml_error(node, "'colors' must be list of two 3-element float arrays");
            return nullptr;
        } else if ( node["pattern_a"] && node["pattern_b"] ) {
            pa = parse_yaml_make_pattern( node["pattern_a"] , shape);
            pb = parse_yaml_make_pattern( node["pattern_b"] , shape);
        }

        if ( pa == nullptr || pb == nullptr ) {
            return nullptr;
        }

        if ( type == "checkers" )
            return CheckerPattern::make(transform, pa, pb);
        if ( type == "ring" )
            return RingPattern::make(transform, pa, pb);
        if ( type == "gradient" )
            return GradientPattern::make(transform, pa, pb);
        if ( type == "stripes" )
            return StripePattern::make(transform, pa, pb);
        if ( type == "blended" )
            return BlendedPattern::make(transform, pa, pb);
        


    }

    // Just in case.
    return nullptr;
}

std::shared_ptr<UVPattern> SceneConfig::parse_yaml_make_uv_pattern(const YAML::Node &node)
{

    // Handle case where pattern is a scalar, a string, and exists in the define_map
    if ( node.IsScalar() ) {
        std::string def = node.as<std::string>();
        YAML::Node src_node;
        if ( lookup_defined_yaml_node(def, src_node) ) {
            return parse_yaml_make_uv_pattern(src_node);
        } else {
            yaml_error(node, "UV Pattern definition '" + def + "' not found");
            return nullptr;
        }
    }

    if ( node.IsMap() ) {
        std::string type;
        if ( node["type"] ) {
            type = node["type"].as<std::string>();
        }
        if ( !node["type"] || ( type != "checkers" && type != "align_check" && type != "image" ) ) {
            yaml_error(node, "uv_pattern definition must include type: 'checkers', 'align_check', 'image'");
            return nullptr;
        }
        if (type == "checkers") {
            if ( !node["width"] || !node["height"] ) {
                yaml_error(node, "checkers uv_pattern requires width and height");
                return nullptr;
            }
            unsigned width = node["width"].as<unsigned>();
            unsigned height = node["height"].as<unsigned>();
            Color color_a;
            Color color_b;
            if ( node["colors"] && node["colors"].IsSequence() && node["colors"].size() == 2 ) {
                auto c0 = node["colors"][0];
                auto c1 = node["colors"][1];
                if ( c0.size() == 3 && c1.size() == 3 ) {
                    double r0 = c0[0].as<double>();
                    double g0 = c0[1].as<double>();
                    double b0 = c0[2].as<double>();
                    double r1 = c1[0].as<double>();
                    double g1 = c1[1].as<double>();
                    double b1 = c1[2].as<double>();
                    color_a = Color(r0,g0,b0);
                    color_b = Color(r1,g1,b1);
                } else {
                    yaml_error(node, "Color sequence values must be a 3 float value RGB array");
                    return nullptr;
                }
            } else {
                yaml_error(node, "Invalid number of colors specified");
            }
            return UVCheckersPattern::make(width, height, color_a, color_b);
        }
        if (type == "align_check") {
            Color color_main, color_ul, color_ur, color_bl, color_br;
            if ( node["colors"] && node["colors"].IsMap() && node["colors"]["main"] && node["colors"]["ul"] && node["colors"]["ur"] && node["colors"]["bl"] && node["colors"]["br"] ) {
                auto c_m = node["colors"]["main"];
                auto c_ul = node["colors"]["ul"];
                auto c_ur = node["colors"]["ur"];
                auto c_bl = node["colors"]["bl"];
                auto c_br = node["colors"]["br"];
                if ( c_m.IsSequence() && c_m.size() == 3 ) { 
                    color_main = Color(c_m[0].as<double>(), c_m[1].as<double>(), c_m[2].as<double>());
                } else { yaml_error(node, "Invalid align_check color: main"); }
                if ( c_ul.IsSequence() && c_ul.size() == 3 ) { 
                    color_ul = Color(c_ul[0].as<double>(), c_ul[1].as<double>(), c_ul[2].as<double>());
                } else { yaml_error(node, "Invalid align_check color: ul"); }
                if ( c_ur.IsSequence() && c_ur.size() == 3 ) { 
                    color_ur = Color(c_ur[0].as<double>(), c_ur[1].as<double>(), c_ur[2].as<double>());
                } else { yaml_error(node, "Invalid align_check color: ur"); }
                if ( c_bl.IsSequence() && c_bl.size() == 3 ) { 
                    color_bl = Color(c_bl[0].as<double>(), c_bl[1].as<double>(), c_bl[2].as<double>());
                } else { yaml_error(node, "Invalid align_check color: bl"); }
                if ( c_br.IsSequence() && c_br.size() == 3 ) { 
                    color_br = Color(c_br[0].as<double>(), c_br[1].as<double>(), c_br[2].as<double>());
                } else { yaml_error(node, "Invalid align_check color: br"); }

                return UVAlignCheckPattern::make(color_main, color_ul, color_ur, color_bl, color_br);
            } else {
                 yaml_error(node, "align_check pattern requires color values for: main, ul, ur, bl, br");
                 return nullptr;
            }
        }
        if (type == "image") {
            if (node["file"]) {
                std::string filename = node["file"].as<std::string>();
                std::shared_ptr<UVPattern> uvp;
                try {
                    uvp = UVImagePattern::make(filename);
                } catch (Glib::FileError &e) {
                    yaml_error(node, std::string("Error creating pattern from image file: ") + e.what() );
                    return nullptr;
                }
                return uvp;
            } else {
                yaml_error(node, "Need filename for image uv_pattern");
                return nullptr;
            }
        }

    }

    return nullptr; // Just in case
    
}

void SceneConfig::parse_yaml_apply_transform(const YAML::Node &node, Matrix &transform)
{
    Matrix tmp = transform;

    // Base case: node is not a matching defined string. Simply apply the transformation to given matrix.
    if ( node.IsSequence() ) {
        if ( node[0].as<std::string>() == "rotate-x" ) {
            if ( node.size() == 2 ) {
                tmp = transform.rotate_x( node[1].as<double>() );
            } else {
                yaml_error(node, "Rotate takes single floating point argument");
            }
        }
        if ( node[0].as<std::string>() == "rotate-y" ) {
            if ( node.size() == 2 ) {
                tmp = transform.rotate_y( node[1].as<double>() );
            } else {
                yaml_error(node, "Rotate takes single floating point argument");
            }
        }
        if ( node[0].as<std::string>() == "rotate-z" ) {
            if ( node.size() == 2 ) {
                tmp = transform.rotate_z( node[1].as<double>() );
            } else {
                yaml_error(node, "Rotate takes single floating point argument");
            }
        }
        if ( node[0].as<std::string>() == "scale" ) {
            if ( node.size() == 4 ) {
                tmp = transform.scale( node[1].as<double>(), node[2].as<double>(), node[3].as<double>() );
            } else {
                yaml_error(node, "Scale requires 3 floating point arguments");
            }
        }
        if ( node[0].as<std::string>() == "translate" ) {
            if ( node.size() == 4 ) {
                tmp = transform.translate( node[1].as<double>(), node[2].as<double>(), node[3].as<double>() );
            } else {
                yaml_error(node, "Translate requires 3 floating point arguments");
            }
        }
        if ( node[0].as<std::string>() == "shear" ) {
            if ( node.size() == 7 ) {
                tmp = transform.shear( node[1].as<double>(),
                                              node[2].as<double>(),
                                              node[3].as<double>(),
                                              node[4].as<double>(),
                                              node[5].as<double>(),
                                              node[6].as<double>() );
            } else {
                yaml_error(node, "Shear requires 6 floating point arguments: x_y, x_z, y_x, y_z, z_x, z_y");
            }
        }
        // Finally, ensure that the transform matrix we ended up with is still invertible.
        // If not, leave the provided one unchanged and throw an error.
        if ( tmp.det() != 0 ) {
            transform = tmp;
        } else {
            yaml_error(node, "Invalid transformation! Resulting matrix is non-invertible.");
        }
    } else if ( node.IsScalar() ) { // Recursive case as long as defined name is valid
        std::string def = node.as<std::string>();
        YAML::Node dest;

        if ( lookup_defined_yaml_node(def, dest) ) {
            for ( YAML::const_iterator it = dest.begin(); it != dest.end(); ++it ) {
                parse_yaml_apply_transform(*it, transform);
            }
        }
    }


}

// If found, return true and write node to dest_out
// else return false, leave dest node unchanged
bool SceneConfig::lookup_defined_yaml_node(const std::string &name, YAML::Node &dest_out)
{
    std::map<std::string, YAML::Node>::iterator it;
    if ( ( it = define_map.find(name) ) != define_map.end() ) {
        dest_out = it->second;
        return true;
    }
    return false;
}

void SceneConfig::yaml_error(const YAML::Node &node, const std::string &error)
{
    std::cerr << "YAML Error near line " << node.Mark().line + 1 << ": " << error << std::endl;
}
void SceneConfig::yaml_warning(const YAML::Node &node, const std::string &error)
{
    std::cerr << "YAML Warning near line " << node.Mark().line + 1 << ": " << error << std::endl;
}


YAML::Node SceneConfig::merge_nodes(YAML::Node a, YAML::Node b)
{
    if (a.IsSequence() && b.IsSequence()) {
        auto c = YAML::Node(YAML::NodeType::Sequence);
        for (auto n : b) {
            c.push_back(n);
        }
        for (auto n : a) {
            c.push_back(n);
        }
        return c;
    }
    if (!b.IsMap())
        // If b is not a map, merge result is b, unless b is null
        return b.IsNull() ? a : b;
    if (!a.IsMap())
        // If a is not a map, merge result is b
        return b;
    if (!b.size())
        // If a is a map, and b is an empty map, return a
        return a;

    // Create a new map 'c' with the same mappings as a, merged with b
    auto c = YAML::Node(YAML::NodeType::Map);
    for (auto n : a) {
        if (n.first.IsScalar()) {
            const std::string &key = n.first.Scalar();
            auto t = YAML::Node(cnode(b)[key]);
            if (t) {
                c[n.first] = merge_nodes(n.second, t);
                continue;
            }
        }
        c[n.first] = n.second;
    }
    // Add the mappings from 'b' not already in 'c'
    for (auto n : b) {
        if (!n.first.IsScalar() || !cnode(c)[n.first.Scalar()]) {
            c[n.first] = n.second;
        }
    }
    return c;
}