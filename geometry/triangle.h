#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "../utils/vec3.h"

class triangle: public hittable {
    public:
        triangle() {
            printf("empty triangle created !!");
        }
        triangle(vec3 _x, vec3 _y, vec3 _z, vec3 normx, vec3 normy, vec3 normz, shared_ptr<material> m)
        : p0(_x), p1(_y), p2(_z), mat_ptr(m), norm_x(normx), norm_y(normy), norm_z(normz) {
            if(norm_x.x() == norm_y.x() && norm_x.y() == norm_y.y() && norm_x.z() == norm_y.z()) {
                vec3 norm = cross(p1 - p0, p2 - p0);
                norm = normalize(norm);
                norm_x = norm;
                norm_y = norm;
                norm_z = norm;
            }
        };
    virtual bool hit(const ray &r, double tmin, double tmax, hit_record &rec) const;
    virtual bool bounding_box(double t0, double t1, aabb &output_box) const ;

    public:
        vec3 p0,p1,p2;
        vec3 norm_x, norm_y, norm_z;
        shared_ptr<material> mat_ptr;
    vec3 interpolate_normal(double a, double b, double c) const {
        return a * norm_x + b * norm_y + c * norm_z;
    }
};
bool triangle::hit(const ray &r, double t_min, double t_max, hit_record &rec) const { //这里是const 则成员函数也得是const
    // Moller Trumbore Algorithm
    //  r.orig + r.dir * t
    vec3 O = r.orig, D = r.dir;
    vec3 P0 = p0, P1 = p1, P2 = p2;
    vec3 E1 = P1 - P0, E2 = P2 - P0;
    vec3 S = O - P0, S1 = cross(D, E2), S2 = cross(S, E1);
    double inv = 1 / dot(S1, E1);
    double t = dot(S2, E2) * inv;   //Games ppt raytracing 1
    double b1 = dot(S1, S) * inv;
    double b2 = dot(S2, D) * inv;
    //如何计算该点的normal
    if(t > t_min && t < t_max && (b1 >= 0 && b1 <= 1) && (b2 >= 0 && b2 <= 1) && ((1 - b1 - b2 >= 0) && (1 - b1 - b2 <= 1))) {
        rec.t = t;
        rec.p = r.at(rec.t);
        vec3 outward_normal = this->interpolate_normal(1 - b1 - b2, b1, b2);
        rec.set_face_normal(r, outward_normal);
        rec.u = b1;
        rec.v = b2; //= gamma   //改了这里
        rec.mat_ptr = mat_ptr;
        return true;
    }
    return false;
}

bool triangle::bounding_box(double t0, double t1, aabb &output_box) const {
    double max_x = ffmax(p0[0], ffmax(p1[0], p2[0]));
    double max_y = ffmax(p0[1], ffmax(p1[1], p2[1]));
    double max_z = ffmax(p0[2], ffmax(p1[2], p2[2]));

    double min_x = ffmin(p0[0], ffmin(p1[0], p2[0]));
    double min_y = ffmin(p0[1], ffmin(p1[1], p2[1]));
    double min_z = ffmin(p0[2], ffmin(p1[2], p2[2]));
    output_box = aabb(
        vec3(min_x - 0.001, min_y - 0.001, min_z - 0.001),
        vec3(max_x + 0.001, max_y + 0.001, max_z + 0.001)   //fuck here
    );

    return true;
}

#endif 