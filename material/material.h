/*
一个材质类应该封装两个功能进去:

1.生成散射后的光线(或者说它吸收了入射光线)
2.如果发生散射, 决定光线会变暗多少(attenuate)
*/

/*
循环包含问题：
这个错误提示表明编译器在material.h文件中找不到hit_record类型的定义。这通常是由于循环包含（circular inclusion）或者头文件依赖关系问题引起的。
*/
#ifndef MATERIAL_H
#define MATERIAL_H
#include "../utils/ray.h"
#include "../geometry/hittable.h"
#include "../utils/rtweekend.h"
#include "../texture/texture.h"
// struct hit_record;

class material {
    public:
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
        ) const = 0;
        virtual vec3 emitted(double u, double v, const vec3& p) const {
            return vec3(0,0,0);
        }
};


//material.h
class lambertian : public material {
    public:
        lambertian(shared_ptr<texture> a) : albedo(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
        ) const {
            vec3 scatter_direction = rec.normal + random_unit_vector(); //随机思想，随机方向出射光
            scattered = ray(rec.p, scatter_direction, r_in.time());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }

    public:
        shared_ptr<texture> albedo;
};

//useless
class wrapMaterial : public material {
    public:
    wrapMaterial(vec3 _p[3] ,double _u[3], double _v[3], shared_ptr<texture> a) : albedo(a) {
        for(int i = 0; i < 3; ++i) {
            color[i] = albedo->value(_u[i], _v[i], _p[i]);
            // std::cerr << _u[i] << " " << _v[i] << " color: " << color[i] << std::endl;
            p[i] = _p[i];
            u[i] = _u[i];
            v[i] = _v[i];
        }
    }
    vec3 get_barycentric_coor(const vec3 p[3], const vec3 &point) const {      //这个需要check一下
        vec3 v0 = p[1] - p[0];
        vec3 v1 = p[2] - p[0];
        vec3 v2 = point - p[0];

        double dot00 = dot(v0, v0);
        double dot01 = dot(v0, v1);
        double dot02 = dot(v0, v2);
        double dot11 = dot(v1, v1);
        double dot12 = dot(v1, v2);

        double invDenom = 1 / (dot00 * dot11 - dot01 * dot01);

        double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
        double v = (dot00 * dot12 - dot01 * dot02) * invDenom;
        double w = 1.0 - u - v;

        return vec3(u, v, w);
    }
    virtual bool scatter(
        const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
    ) const {
        vec3 scatter_direction = rec.normal + random_unit_vector(); //随机思想，随机方向出射光
        scattered = ray(rec.p, scatter_direction, r_in.time());
        
        vec3 bary_coor = get_barycentric_coor(p, rec.p);
        double alpha = bary_coor[0], beta = bary_coor[1], gamma = bary_coor[2];
        attenuation = alpha * color[0] + beta * color[1] + gamma * color[2];
        // attenuation = color[0];
        return true;
    }
    public:
        vec3 p[3];
        vec3 color[3];
        double u[3], v[3];
        shared_ptr<texture> albedo;
};

class diffuse_light : public material  {
    public:
        diffuse_light(shared_ptr<texture> a) : emit(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
        ) const {
            return false;
        }

        virtual vec3 emitted(double u, double v, const vec3& p) const {
            return emit->value(u, v, p);
        }

    public:
        shared_ptr<texture> emit;
};

class metal : public material {
    public:
        metal(const vec3& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
        ) const {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);//dot<0我们认为吸收
        }

    public:
        vec3 albedo;
        double fuzz;
};

class dielectric : public material {
    public:
        dielectric(double ri) : ref_idx(ri) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
        ) const {
            attenuation = vec3(1.0, 1.0, 1.0);
            double etai_over_etat = (rec.front_face) ? (1.0 / ref_idx) : (ref_idx);

            vec3 unit_direction = unit_vector(r_in.direction());
            double cos_theta = ffmin(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
            if (etai_over_etat * sin_theta > 1.0 ) {
                vec3 reflected = reflect(unit_direction, rec.normal);
                scattered = ray(rec.p, reflected);
                return true;
            }
            double reflect_prob = schlick(cos_theta, etai_over_etat);
            if (random_double() < reflect_prob)
            {
                vec3 reflected = reflect(unit_direction, rec.normal);
                scattered = ray(rec.p, reflected);
                return true;
            }
            vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
            scattered = ray(rec.p, refracted);
            return true;
        }

    public:
        double ref_idx;
};


class isotropic : public material {
    public:
        isotropic(shared_ptr<texture> a) : albedo(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
        ) const  {
            scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }

    public:
        shared_ptr<texture> albedo;
};

#endif