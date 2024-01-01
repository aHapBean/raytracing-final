//main.cc
#include "utils/rtweekend.h"

#include "geometry/hittable_list.h"
#include "geometry/sphere.h"
#include "utils/carmera.h"
#include "material/material.h"
#include "geometry/bvh.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "utils/stb_image.h"
#include "geometry/xy_rect.h"
#include "geometry/box.h"
#include "geometry/volumn.h"     //NOTE 解决了循环引用问题，找出原因->重构
#include "geometry/triangle.h"
#include <utility>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "utils/obj_loader.h"
#include "scene/scene.h"
#include <string> 
#include <thread>
#include <mutex>
#include <map>
// #include "std_image.h"
std::mutex myMutex;

//demo画质
// const int image_width = 250;
// const int image_height = 150;   // 5 : 3
// const int samples_per_pixel = 100;
// const int max_depth = 30;

//最终作品画质
const int image_width = 1200;
const int image_height = 720;   // 5 : 3
const int samples_per_pixel = 10000;
const int max_depth = 30;

vec3 ray_color(const ray& r, const vec3& background, const hittable& world, int depth) {
    hit_record rec;
    
    if (depth <= 0)
        return vec3(0,0,0);

    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    ray scattered;
    vec3 attenuation;
    vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth-1);
}
//main.cc

bool check(unsigned char* che) {
    if(che == NULL) {
        std::cerr << "check error, ptr NULL !!" << std::endl;
        return false;
    }
    return true;
}

// 最终作品场景设计
std::pair<hittable_list, camera> test_capitan() {
    //load with mtl file
    hittable_list objects;
        
    auto text = make_shared<constant_texture>(vec3(1., 1., 1.));

    int nx, ny, nn;
    
    std::vector<vec3> triangle_ls,normals;
    std::vector<double> store_u, store_v;
    std::vector<std::string> pic_names;
    std::map<std::string, unsigned char*> rec;rec.clear();
    // std::string base_path = "/data/wangshaobo/zxd/raytracing-in-CG-course/";        // need to be changed through your computer path
    std::string base_path = "/home/ahapbean/computer_graphics/final_ray_tracing/";

    auto ground_text = make_shared<constant_texture>(vec3(0.5, 0.5, 0.5));
    unsigned char *avengers_data = stbi_load((base_path + "models/avengers.jpg").c_str(), &nx, &ny, &nn, 3);
    auto avengers_text = make_shared<image_texture>(avengers_data, nx, ny);

    unsigned char *mikky_data = stbi_load((base_path + "models/mikky.jpg").c_str(), &nx, &ny, &nn, 3);
    auto mikky_text = make_shared<image_texture>(mikky_data, nx, ny);


    unsigned char *ground_data = stbi_load((base_path + "models/ground3.jpg").c_str(), &nx, &ny, &nn, 3);
    auto ground_text_img = make_shared<image_texture>(ground_data, nx, ny);

    unsigned char *earth_data = stbi_load((base_path + "models/earthmap.jpg").c_str(), &nx, &ny, &nn, 3);
    auto earth_text_img = make_shared<image_texture>(earth_data, nx, ny);

    //加点星星
    unsigned char *jupiter_data = stbi_load((base_path + "models/jupiter.jpg").c_str(), &nx, &ny, &nn, 3);
    auto jupiter_text_img = make_shared<image_texture>(jupiter_data, nx, ny);

    unsigned char *sun_data = stbi_load((base_path + "models/sun.jpg").c_str(), &nx, &ny, &nn, 3);
    auto sun_text_img = make_shared<image_texture>(sun_data, nx, ny);

    if(!(check(ground_data)  && check(mikky_data) && check(ground_data) && check(jupiter_data) && check(sun_data)) && check(earth_data)) {
        std::cerr << "error loading jpgs, exit here!!" << std::endl;
        exit(-1);
    }

    //木星
    double x = -1.30, y = 2.35, z = -0.34, r = 0.2;
    objects.add(make_shared<sphere>(vec3(x, y, z), r, make_shared<lambertian>(jupiter_text_img)));

    //太阳
    x = -0.35, y = 1.67, z = -0.8, r = 0.13;
    objects.add(make_shared<sphere>(vec3(x, y, z), r, make_shared<lambertian>(sun_text_img)));

    //地球
    x = 0.7, y = 2.1, z = 0.0, r = 0.24;
    objects.add(make_shared<sphere>(vec3(x, y, z), r, make_shared<lambertian>(earth_text_img)));


    //放两个玻璃球
    int ran_num = 2;
    x = 1.0, y = 1.5, z = -0.2, r = 0.18;
    objects.add(make_shared<sphere>(vec3(x, y, z), r,  make_shared<dielectric>(1.5)));
    x = -1.5, y = 0.8, z = -0.4, r = 0.18;
    objects.add(make_shared<sphere>(vec3(x, y, z), r,  make_shared<dielectric>(1.5))); 
    

    ran_num = 2;
    x = -0.51, y = 2.57, z = -0.84, r = 0.1;
    objects.add(make_shared<sphere>(vec3(x, y, z), r, make_shared<dielectric>(1.6)));
    x = -1.10, y = 1.67, z = -0.91, r = 0.1;
    objects.add(make_shared<sphere>(vec3(x, y, z), r, make_shared<dielectric>(1.6)));
    x = 1.24, y = 1.95, z = -0.28, r = 0.08;
    objects.add(make_shared<sphere>(vec3(x, y, z), r, make_shared<dielectric>(1.6)));
    x = -0.16, y = 2.90, z = -0.36, r = 0.11;
    objects.add(make_shared<sphere>(vec3(x, y, z), r, make_shared<dielectric>(1.6)));
    

    //地面
    double z_inter = -1;
    double y_inter = 0;
    double x_inter = 1.7;       // 漫威图片 1:1.41
    //CG word  2 : 1
    // double y_limit = 3.2;    //限制    对于CG word
    double y_limit = 6;         //限制    对于星空

    //down 的贴图是1:1的
    auto down = make_shared<xz_rect>(-2 * x_inter, 2 * x_inter, z_inter, z_inter + 4 * x_inter, y_inter, make_shared<lambertian>(ground_text_img));
    auto left = make_shared<yz_rect>(y_inter, y_limit, z_inter, 50, -x_inter, make_shared<dielectric>(1.5));
    auto right = make_shared<zy_rect>(z_inter, z_inter +  1.3 * y_limit, y_inter, y_limit, x_inter, make_shared<lambertian>(mikky_text)); //1.3 : 1
    
    //注意yz_rect的贴图哪边是"x"
    //利用zy_rect解决问题

    auto back = make_shared<xy_rect>(-x_inter, x_inter, y_inter, 1.41 * (2 * x_inter), z_inter, make_shared<lambertian>(avengers_text));
    objects.add(down);
    objects.add(left);
    objects.add(right);
    objects.add(back);
    // (1.7, 0, -1)
    // (-1.7, 0 ,-1)


    auto white = make_shared<constant_texture>(vec3(1.,1.,1.));
    //校徽
    auto tex_dat = stbi_load((base_path + "models/school_badge.jpg").c_str(), &nx, &ny, &nn, 3);   //校徽 3 ： 1
    if (tex_dat == nullptr) {
        std::cerr << "Error loading texture from path: " << (base_path + "models/school_badge.jpg").c_str() << std::endl;
        std::exit(EXIT_FAILURE);  
    }
    auto badge_text = make_shared<image_texture>(tex_dat, nx, ny);
    double badge_scale = 0.13;
    auto rect = make_shared<xy_rect>(-3 * badge_scale, 3 * badge_scale, -1 * badge_scale, 1 * badge_scale, 0.61 * badge_scale, make_shared<lambertian>(badge_text));
    auto boxx = make_shared<box>(vec3(-3,-1,-0.6) * badge_scale, vec3(3,1,0.6) * badge_scale, make_shared<lambertian>(white));
    vec3 disp_badge(-0.9,badge_scale,4.6);    //地面的y=0 offset
    auto new_rect = make_shared<translate>(make_shared<rotate_y>(rect, -20), disp_badge);
    auto new_boxx = make_shared<translate>(make_shared<rotate_y>(boxx, -20), disp_badge);


    objects.add(new_rect);
    objects.add(new_boxx);

    //cg word
    auto tex_dat2 = stbi_load((base_path + "models/cg_word2.png").c_str(), &nx, &ny, &nn, 3);   //校徽 3 ： 1
    if (tex_dat2 == nullptr) {
        std::cerr << "Error loading texture from path: " << (base_path + "models/cg_word.jpg").c_str() << std::endl;
        std::exit(EXIT_FAILURE);  
    }
    auto badge_text2 = make_shared<image_texture>(tex_dat2, nx, ny);
    auto rect2 = make_shared<xy_rect>(-3 * badge_scale, 3 * badge_scale, -1 * badge_scale, 1 * badge_scale, 0.61 * badge_scale, make_shared<lambertian>(badge_text2));
    auto boxx2 = make_shared<box>(vec3(-3,-1,-0.6) * badge_scale, vec3(3,1,0.6) * badge_scale, make_shared<lambertian>(white));
    vec3 disp_badge2(0.8,badge_scale,4.8);    //地面的y=0 offset
    auto new_rect2 = make_shared<translate>(make_shared<rotate_y>(rect2, 20), disp_badge2);
    auto new_boxx2 = make_shared<translate>(make_shared<rotate_y>(boxx2, 20), disp_badge2);
    objects.add(new_rect2);
    objects.add(new_boxx2);

    //spider man
    std::string spider_base_path = base_path + "models/spiderman/";
    std::string obj_name = "spider.obj";

    std::string full_path = spider_base_path + obj_name;
    const char* path = full_path.c_str();

    obj_load_with_texture_and_diff_pics(path, triangle_ls, store_u, store_v, normals, pic_names);

    double people_scale = 0.5; //正则化到-0.5-0.5
    double people_z = 3.8;
    double people_x = 0.4;
    for(int i = 0; i < normals.size(); ++i) {
        vec3 point[3];
        double u[3], v[3];
        for(int j = 0; j < 3; ++j) {
            point[j] = triangle_ls[3 * i + j];  
            u[j] = store_u[3 * i + j];
            v[j] = store_v[3 * i + j];
        }
        std::string path = base_path + pic_names[i];
        unsigned char* tex_data;
        if (rec.find(path) != rec.end()) {
            tex_data = rec[path];
        } else {
            tex_data = stbi_load((spider_base_path + pic_names[i]).c_str(), &nx, &ny, &nn, 3);  
            if (tex_data == nullptr) {
                std::cerr << "Error loading texture from path: " << (spider_base_path + pic_names[i]) << std::endl;
                std::exit(EXIT_FAILURE);  
            }
            rec[path] = tex_data;
            std::cerr << "new texture pic added: " << pic_names[i] << std::endl;
        }
        
        auto patrick_texture = make_shared<obj_texture>(tex_data, nx, ny, u[0], v[0], u[1] - u[0], u[2] - u[0], v[1] - v[0], v[2] - v[0]);    
        auto mater = make_shared<lambertian>(patrick_texture);
        auto normal = normals[i];
        auto obj = make_shared<triangle>(point[0] * people_scale, point[1] * people_scale, point[2] * people_scale, normal, normal, normal, mater);
        auto newwtmp_obj = make_shared<rotate_y>(obj, 30.);
        auto new_obj = make_shared<translate>(newwtmp_obj, vec3(0.5 + people_x - 0.1, people_scale + 0.5, people_z + 0.1));
        objects.add(new_obj);
    }

    //美国队长
    rec.clear();
    std::string capitan_base_path = base_path + "models/capitan/";
    obj_name = "Capitan.obj";
    full_path = capitan_base_path + obj_name;
    const char* path2 = full_path.c_str();

    obj_load_with_texture_and_diff_pics(path2, triangle_ls, store_u, store_v, normals, pic_names);  

    for(int i = 0; i < normals.size(); ++i) {
        vec3 point[3];
        double u[3], v[3];
        for(int j = 0; j < 3; ++j) {
            point[j] = triangle_ls[3 * i + j];  
            u[j] = store_u[3 * i + j];
            v[j] = store_v[3 * i + j];
        }
        std::string path = capitan_base_path + pic_names[i];
        unsigned char* tex_data;
        if (rec.find(path) != rec.end()) {
            tex_data = rec[path];
        } else {
            tex_data = stbi_load((capitan_base_path + pic_names[i]).c_str(), &nx, &ny, &nn, 3); 
            if (tex_data == nullptr) {
                std::cerr << "Error loading texture from path: " << (capitan_base_path + pic_names[i]) << std::endl;
                std::exit(EXIT_FAILURE);  
            }
            rec[path] = tex_data;
            std::cerr << "new texture pic added: " << pic_names[i] << std::endl;
        }
        
        auto patrick_texture = make_shared<obj_texture>(tex_data, nx, ny, u[0], v[0], u[1] - u[0], u[2] - u[0], v[1] - v[0], v[2] - v[0]);    
        auto mater = make_shared<lambertian>(patrick_texture);
        auto normal = normals[i];
        double capitan_scale = 1.05;
        double capitan_thin = 1.0;
        auto obj = make_shared<triangle>(point[0] * people_scale * capitan_scale * capitan_thin, point[1] * people_scale * capitan_scale, point[2] * people_scale * capitan_scale, normal, normal, normal, mater);
        auto new_obj = make_shared<translate>(obj, vec3(0. + people_x - 0.12, people_scale * capitan_scale, people_z));
        //美队向右边移动一点
        objects.add(new_obj);
    }

    capitan_base_path = base_path + "models/capitan/";
    obj_name = "Capitan_shield.obj";
    full_path = capitan_base_path + obj_name;
    const char* path4 = full_path.c_str();
    obj_load_with_texture_and_diff_pics(path4, triangle_ls, store_u, store_v, normals, pic_names); 

    for(int i = 0; i < normals.size(); ++i) {
        vec3 point[3];
        double u[3], v[3];
        for(int j = 0; j < 3; ++j) {
            point[j] = triangle_ls[3 * i + j];  
            u[j] = store_u[3 * i + j];
            v[j] = store_v[3 * i + j];
        }
        std::string path = capitan_base_path + pic_names[i];
        unsigned char* tex_data;
        if (rec.find(path) != rec.end()) {
            tex_data = rec[path];
        } else {
            tex_data = stbi_load((capitan_base_path + pic_names[i]).c_str(), &nx, &ny, &nn, 3); 
            if (tex_data == nullptr) {
                std::cerr << "Error loading texture from path: " << (capitan_base_path + pic_names[i]) << std::endl;
                std::exit(EXIT_FAILURE); 
            }
            rec[path] = tex_data;
            std::cerr << "new texture pic added: " << pic_names[i] << std::endl;
        }
        
        auto patrick_texture = make_shared<obj_texture>(tex_data, nx, ny, u[0], v[0], u[1] - u[0], u[2] - u[0], v[1] - v[0], v[2] - v[0]);    
        auto mater = make_shared<lambertian>(patrick_texture);
        auto normal = normals[i];
        double capitan_scale = 0.45;
        double capitan_thin = 1.0;

        auto obj = make_shared<triangle>(point[0] * people_scale * capitan_scale * capitan_thin, point[1] * people_scale * capitan_scale, point[2] * people_scale * capitan_scale, normal, normal, normal, mater);
        auto objj = make_shared<rotate_y>(obj, 180);
        auto new_obj = make_shared<translate>(objj, vec3(0. + people_x - 0.12 + 0.2, people_scale * capitan_scale + 0.2, people_z + 0.7));
        //美队盾牌
        objects.add(new_obj);
    }

    //iron man
    rec.clear();
    std::string iron_base_path = base_path + "models/iron_man/";
    obj_name = "Iron_Man.obj";
    full_path = iron_base_path + obj_name;
    const char* path3 = full_path.c_str();

    obj_load_with_texture_and_diff_pics(path3, triangle_ls, store_u, store_v, normals, pic_names);  

    for(int i = 0; i < normals.size(); ++i) {
        vec3 point[3];
        double u[3], v[3];
        for(int j = 0; j < 3; ++j) {
            point[j] = triangle_ls[3 * i + j];  
            u[j] = store_u[3 * i + j];
            v[j] = store_v[3 * i + j];
        }
        std::string path = iron_base_path + pic_names[i];
        unsigned char* tex_data;
        if (rec.find(path) != rec.end()) {
            tex_data = rec[path];
        } else {
            tex_data = stbi_load((iron_base_path + pic_names[i]).c_str(), &nx, &ny, &nn, 3); 
            if (tex_data == nullptr) {
                std::cerr << "Error loading texture from path: " << (iron_base_path + pic_names[i]) << std::endl;
                std::exit(EXIT_FAILURE);  
            }
            rec[path] = tex_data;
            std::cerr << "new texture pic added: " << pic_names[i] << std::endl;
        }
        
        auto patrick_texture = make_shared<obj_texture>(tex_data, nx, ny, u[0], v[0], u[1] - u[0], u[2] - u[0], v[1] - v[0], v[2] - v[0]);    
        auto mater = make_shared<lambertian>(patrick_texture);
        auto normal = normals[i];

        auto obj = make_shared<triangle>(point[0] * people_scale, point[1] * people_scale, point[2] * people_scale, normal, normal, normal, mater);
        auto new_obj = make_shared<translate>(obj, vec3(-1.0 + people_x + 0.25, people_scale + 0.15, people_z - 0.8));
        // 钢铁侠往后飞起来
        objects.add(new_obj);
    }


    auto difflight = make_shared<diffuse_light>(make_shared<constant_texture>(vec3(8,8,8)));
    auto difflight2 = make_shared<diffuse_light>(make_shared<constant_texture>(vec3(1.5,1.5,1.5)));
    auto lig = make_shared<xz_rect>(-x_inter, x_inter, z_inter, 4, 8, difflight);
    auto lig2 = make_shared<xy_rect>(-x_inter, x_inter, y_inter, 2, 9, difflight2);
    objects.add(lig);
    objects.add(lig2);
    // auto lig = make_shared<xz_rect>(-x_inter, x_inter, -2.5,  2.5, 0, difflight);
    // auto trans_light = make_shared<rotate_x>(lig, 45);
    // auto trans_light2 = make_shared<translate>(trans_light, vec3(0, 8, 5));
    // objects.add(trans_light2);

    objects = static_cast<hittable_list>(make_shared<bvh_node>(objects, 0, 1));
    vec3 lookfrom(0,0.7,8);
    // vec3 lookfrom(-5,2,9);
    vec3 lookat(0,0.8,0);   //应该大概是人的眼睛的高度
    vec3 vup(0,1,0);
    auto dist_to_focus = 1.0;
    auto aperture = 0.0;
    camera cam(lookfrom, lookat, vup, 30, double(image_width) / image_height, aperture, dist_to_focus, 0.0, 1.0);  //调大之后模糊可能和相机参数有关
    return std::make_pair(objects, cam);
}

vec3 global_colors[image_width][image_height];

// multi threads rendering
void render_tile(int start_column, int end_column, camera &cam, const vec3& background, const hittable_list& world) {

    for (int i = start_column; i < end_column; ++i) {
        for (int j = image_height - 1; j >= 0; --j) {
            vec3 color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / image_width;
                auto v = (j + random_double()) / image_height;
                ray r = cam.get_ray(u, v);
                color += ray_color(r, background, world, max_depth);
            }
            {
                std::lock_guard<std::mutex> lock(myMutex);
                global_colors[i][j] = color;
            }
        }
        if(start_column == 0 or true) {
            std::cerr << "\rcolumns remained: " << end_column - i - 1 << ' ' << std::flush;
        }
    }
}

int main() {

    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    auto R = cos(pi/4);

    std::pair<hittable_list, camera> pair = test_capitan();
    hittable_list world = pair.first;
    camera cam = pair.second;
    const vec3 background(0,0,0);

    const int num_threads = std::thread::hardware_concurrency();        //得到可用线程数
    std::vector<std::thread> threads;
    int column_per_thread = image_width / num_threads;
    int remaining_column = image_width % num_threads;
    int start_column = 0;
    int end_column = 0;

    std::cerr << "multi threads start, column per thread: " << column_per_thread << " num threads: " << num_threads << std::endl;
    for(int i = 0; i < num_threads; ++i) {
        start_column = end_column;
        if(i == num_threads - 1) {
            end_column = start_column + column_per_thread + remaining_column;
        } else { 
            end_column = start_column + column_per_thread;              //不包括
        }
        
        // std::cerr << end_column << std::endl;
        threads.emplace_back(render_tile, start_column, end_column, std::ref(cam), std::ref(background), std::ref(world));
    }
    for (auto& thread : threads) {
        thread.join();
    }

    for (int j = image_height-1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            global_colors[i][j].write_color(std::cout, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
    return 0;

    // for (int j = image_height-1; j >= 0; --j) {
    //     std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
    //     for (int i = 0; i < image_width; ++i) {
    //         vec3 color(0, 0, 0);
    //         for (int s = 0; s < samples_per_pixel; ++s) {
    //             auto u = (i + random_double()) / image_width;
    //             auto v = (j + random_double()) / image_height;
    //             ray r = cam.get_ray(u, v);                  //动效的核心在这里，发射不同时间的光线！！！
    //             color += ray_color(r, background, world, max_depth);   
    //         }
    //         color.write_color(std::cout, samples_per_pixel);
    //     }
    // }


    // std::cerr << "\nDone.\n";
}

