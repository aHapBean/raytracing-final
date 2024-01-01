#ifndef TEXTURE_H
#define TEXTURE_H
#include "../utils/rtweekend.h"
#include "../material/perlin.h"
#include "../utils/stb_image.h"
#include <opencv2/opencv.hpp>

class texture {
    public:
        virtual vec3 value(double u, double v, const vec3& p) const = 0;
};

class constant_texture : public texture {
    public:
        constant_texture() {}
        constant_texture(vec3 c) : color(c) {}

        virtual vec3 value(double u, double v, const vec3& p) const {
            return color;
        }

    public:
        vec3 color;
};

class checker_texture : public texture {
    public:
        checker_texture() {}
        checker_texture(shared_ptr<texture> t0, shared_ptr<texture> t1): even(t0), odd(t1) {}

        virtual vec3 value(double u, double v, const vec3& p) const {
            auto sines = sin(10*p.x())*sin(10*p.y())*sin(10*p.z());
            if (sines < 0)
                return odd->value(u, v, p);
            else
                return even->value(u, v, p);
        }

    public:
        shared_ptr<texture> odd;
        shared_ptr<texture> even;
};


class noise_texture : public texture {
    public:
        noise_texture() {}
        noise_texture(double sc) : scale(sc) {}

        virtual vec3 value(double u, double v, const vec3& p) const {
            return vec3(1,1,1) * 0.5 * (1 + sin(scale*p.z() + 10*noise.turb(p)));
        }

    public:
        perlin noise;
        double scale;
};

class image_texture : public texture {
    public:
        image_texture() {}
        image_texture(unsigned char *pixels, int A, int B)
            : data(pixels), nx(A), ny(B) {}

        ~image_texture() {
            if (data != NULL) {
                delete data;
                data = NULL;    //here
            }
        }

        virtual vec3 value(double u, double v, const vec3& p) const {
            // If we have no texture data, then always emit cyan (as a debugging aid).
            if (data == nullptr)
                return vec3(0,1,1);

            auto i = static_cast<int>((  u)*nx);
            auto j = static_cast<int>((1-v)*ny-0.001);

            if (i < 0) i = 0;
            if (j < 0) j = 0;
            if (i > nx-1) i = nx-1;
            if (j > ny-1) j = ny-1;

            auto r = static_cast<int>(data[3*i + 3*nx*j+0]) / 255.0;
            auto g = static_cast<int>(data[3*i + 3*nx*j+1]) / 255.0;
            auto b = static_cast<int>(data[3*i + 3*nx*j+2]) / 255.0;

            return vec3(r, g, b);
        }

    public:
        unsigned char *data;
        int nx, ny;
};

class obj_texture : public texture {
    public:
        obj_texture() {}
        obj_texture(unsigned char *pixels, int A, int B, double _u1, double _v1, double _du1, double _du2, double _dv1, double _dv2)
            : data(pixels), nx(A), ny(B) {
                ver_u = _u1;
                ver_v = _v1;
                du1 = _du1;
                dv1 = _dv1;
                du2 = _du2;
                dv2 = _dv2;
            }

        ~obj_texture() {    //内存泄漏
        }
        //NOTE 这个地方得u,v具有对应关系，详见report
        virtual vec3 value(double u, double v, const vec3& p) const {
            double pu1 = du1 * u;
            double pv1 = dv1 * u;
            double pu2 = du2 * v;
            double pv2 = dv2 * v;
            double pre_u = u, pre_v = v;
            u = (pu1 + pu2 + ver_u);  
            v = (pv1 + pv2 + ver_v);  //here to check

            if (data == nullptr)
                return vec3(0,1,1);

            auto i = static_cast<int>((u)*nx);    
            auto j = static_cast<int>((1 - v)*ny-0.001);    

            if (i < 0) i = 0;
            if (j < 0) j = 0;
            if (i > nx-1) i = nx-1;
            if (j > ny-1) j = ny-1;

            auto r = static_cast<int>(data[3*i + 3*nx*j+0]) / 255.0;
            auto g = static_cast<int>(data[3*i + 3*nx*j+1]) / 255.0;
            auto b = static_cast<int>(data[3*i + 3*nx*j+2]) / 255.0;
            return vec3(r, g, b);
        }

    public:
        double ver_u, ver_v, du1, du2, dv1, dv2;
        unsigned char *data;
        int nx, ny;
};

class opencv_obj_texture : public texture {
    public:
        opencv_obj_texture() {}
        opencv_obj_texture(const char *path, int A, int B, double _u1, double _v1, double _du1, double _du2, double _dv1, double _dv2)
            : nx(A), ny(B) {
                image = cv::imread("/home/ahapbean/computer_graphics/raytracing/models/patrick.jpg", cv::IMREAD_COLOR);
                ver_u = _u1;
                ver_v = _v1;
                du1 = _du1;
                dv1 = _dv1;
                du2 = _du2;
                dv2 = _dv2;
            }

        ~opencv_obj_texture() {    //内存泄漏
        }

        virtual vec3 value(double u, double v, const vec3& p) const {
            double pu1 = du1 * u;
            double pv1 = dv1 * u;
            double pu2 = du2 * v;
            double pv2 = dv2 * v;
            double pre_u = u, pre_v = v;
            u = pu1 + pu2 + ver_u;  
            v = pv1 + pv2 + ver_v;  //here to che   
            // std::cerr << ver_u << " " << ver_v << " " << u << " " << v << " " << du1 << " " << du2 << " " << pre_u << " " << pre_v << std::endl;
            int x = static_cast<int>(u * image.cols);   // int ????
            int y = static_cast<int>(v * image.rows);

            // Ensure coordinates are within the valid range
            x = std::max(0, std::min(x, image.cols - 1));
            y = std::max(0, std::min(y, image.rows - 1));

            // Get RGB values (remember the BGR order)
            auto pixel = image.at<cv::Vec3b>(y, x);
            vec3 rgb = vec3(pixel[0] / 255., pixel[1] / 255., pixel[2] / 255.);
            // std::cerr << rgb << std::endl;
            return rgb;
        }

    public:
        double ver_u, ver_v, du1, du2, dv1, dv2;
        cv::Mat image;
        int nx, ny;
};

// 当有图案texture的时候
// class triangle_texture : public texture {
//     private:
//         vec3 p0, p1, p2;
//         vec3 color1, color2, color3
// };
#endif 