#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

sf::Transform unRotateMatrix(sf::Vector3f r) {
    return sf::Transform(
        1.f, 0.f, 0.f, 
        0.f, cos(r.x), -sin(r.x),
        0.f, sin(r.x), cos(r.x)
    ) * sf::Transform(
        cos(r.y), 0.f, sin(r.y),
        0.f, 1.f, 0.f,
        -sin(r.y), 0, cos(r.y)
    ) * sf::Transform(
        cos(r.z), -sin(r.z), 0.f,
        sin(r.z), cos(r.z), 0.f,
        0.f, 0.f, 1.f
    );
}

sf::Transform rotateMatrix(sf::Vector3f r) {
    return sf::Transform(
        cos(r.z), -sin(r.z), 0.f,
        sin(r.z), cos(r.z), 0.f,
        0.f, 0.f, 1.f
    ) * sf::Transform(
        cos(r.y), 0.f, sin(r.y),
        0.f, 1.f, 0.f,
        -sin(r.y), 0, cos(r.y)
    ) * sf::Transform(
        1.f, 0.f, 0.f, 
        0.f, cos(r.x), -sin(r.x),
        0.f, sin(r.x), cos(r.x)
    );
}

inline float dot(sf::Vector3f a, sf::Vector3f b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline float mod(sf::Vector3f v) {
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

inline sf::Vector3f normalize(sf::Vector3f v) {
    return v/mod(v);
}

class Shaded {
protected:
    sf::Shader* shader;
public:
    void bindShader(sf::Shader* shader) { this->shader = shader; }
};

class Objekt3D : public Shaded {
protected:
    sf::Vector3f rotvec;
    sf::Transform rot, unrot;
public:
    sf::Vector3f coord;

    Objekt3D(): coord(), rotvec(), rot() {}
    Objekt3D(sf::Vector3f cd, sf::Vector3f rt)
        :coord(cd), rotvec(rt) {
        rot = rotateMatrix(rt);
        unrot = unRotateMatrix(-rt);
    }

    sf::Vector3f getRot() { return rotvec; }
    void setRot(sf::Vector3f rt) {
        rotvec = rt;
        rot = rotateMatrix(rt);
        unrot = unRotateMatrix(-rt);
    }
    
    void setInShader(const std::string name) {
        shader->setUniform(name+".coord", coord);
        shader->setUniform(name+".rot", sf::Glsl::Mat3(rot));
        shader->setUniform(name+".unrot", sf::Glsl::Mat3(unrot));
    }

    void coordInShader(const std::string name) {
        shader->setUniform(name+".coord", coord);
    }

    void rotInShader(const std::string name) {
        shader->setUniform(name+".rot", sf::Glsl::Mat3(rot));
        shader->setUniform(name+".unrot", sf::Glsl::Mat3(unrot));
    }
};

class SizedObjekt3D : public Objekt3D {
protected:
    sf::Vector3f mas;
public:
    sf::Glsl::Vec4 color;

    SizedObjekt3D(): Objekt3D(), mas(1., 1., 1.), color(1., 1., 1., 1.) {}
    SizedObjekt3D(sf::Vector3f cd, sf::Vector3f ms, sf::Vector3f rt)
        :Objekt3D(cd, rt), mas(ms), color(1., 1., 1., 1.) {}
    SizedObjekt3D(sf::Vector3f cd, sf::Vector3f ms, sf::Vector3f rt, sf::Glsl::Vec4 col)
        :Objekt3D(cd, rt), mas(ms), color(col) {}
    
    void setInShader(const std::string name) {
        shader->setUniform(name+".coord", coord);
        shader->setUniform(name+".mas", mas);
        shader->setUniform(name+".rot", sf::Glsl::Mat3(rot));
        shader->setUniform(name+".unrot", sf::Glsl::Mat3(unrot));
        shader->setUniform(name+".color", color);
    }

    void masInShader(const std::string name) {
        shader->setUniform(name+".mas", mas);
    }
};

class Camera : public Objekt3D {
public:
    Camera(): Objekt3D() {}
    Camera(sf::Vector3f cd, sf::Vector3f rt)
        :Objekt3D(cd, rt) {}
};

class Light : Shaded {
public:
    sf::Vector3f dir;
    sf::Glsl::Vec4 color;

    Light(): dir(), color() {};
    Light(sf::Vector3f dr, sf::Glsl::Vec4 cl): dir(normalize(dr)), color(cl) {}

    void setInShader(const std::string name) {
        shader->setUniform(name, dir);
        shader->setUniform(name+"Col", color);
    }

    void dirInShader(const std::string name) {
        shader->setUniform(name, dir);
    }

    void colInShader(const std::string name) {
        shader->setUniform(name+"Col", color);
    }
};

class Elips : public SizedObjekt3D {
public:
    Elips(): SizedObjekt3D() {}
    Elips(sf::Vector3f cd, sf::Vector3f ms, sf::Vector3f rt)
        :SizedObjekt3D(cd, ms, rt) {}
    Elips(sf::Vector3f cd, sf::Vector3f ms, sf::Vector3f rt, sf::Glsl::Vec4 col)
        :SizedObjekt3D(cd, ms, rt, col) {}
};

class Box: public SizedObjekt3D {
public:
    Box(): SizedObjekt3D() {}
    Box(sf::Vector3f cd, sf::Vector3f ms, sf::Vector3f rt)
        :SizedObjekt3D(cd, ms, rt) {}
    Box(sf::Vector3f cd, sf::Vector3f ms, sf::Vector3f rt, sf::Glsl::Vec4 col)
        :SizedObjekt3D(cd, ms, rt, col) {}
};

class Plane : public Shaded{
protected:
    sf::Vector3f norm;
public:
    sf::Vector3f coord;
    sf::Glsl::Vec4 color;

    Plane(): coord(), norm(0., 0., 1.), color(1., 1., 1., 1.) {}
    Plane(sf::Vector3f cd, sf::Vector3f nm)
        :coord(cd), norm(normalize(nm)), color(1., 1., 1., 1.) {}
    Plane(sf::Vector3f cd, sf::Vector3f nm, sf::Glsl::Vec4 col)
        :coord(cd), norm(normalize(nm)), color(col) {}
    
    sf::Vector3f getNorm() { return norm; }
    void setNorm(sf::Vector3f& nm) {
        norm = normalize(nm);
    }
    void setInShader(const std::string name) {
        shader->setUniform(name+".coord", sf::Glsl::Vec4(norm.x, norm.y, norm.z, -dot(coord, norm)));
        shader->setUniform(name+".color", color);
    }
};