#include "../geometry/hittable.h"
#include "../geometry/hittable_list.h"
#include "../material/material.h"
#include "../geometry/sphere.h"
#include "../geometry/bvh.h"
#include "../utils/vec3.h"
#include "../utils/carmera.h"
#include "../geometry/triangle.h"
#include "../geometry/xy_rect.h"
#include "../geometry/box.h"
#include "../texture/texture.h"
// #include ""
// #include "**"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

hittable_list random_scene() {
    hittable_list world;
    world.add(make_shared<sphere>(
        vec3(0,-1000,0), 1000, make_shared<lambertian>(make_shared<constant_texture>(vec3(0.5, 0.5, 0.5)))
    ));


     int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            vec3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());
            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = vec3::random() * vec3::random();
                    world.add(
                        make_shared<sphere>(center, 0.2, make_shared<lambertian>(make_shared<constant_texture>(albedo))));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = vec3::random(.5, 1);
                    auto fuzz = random_double(0, .5);
                    world.add(
                        make_shared<sphere>(center, 0.2, make_shared<metal>(albedo, fuzz)));
                } else {
                    // glass
                    world.add(make_shared<sphere>(center, 0.2, make_shared<dielectric>(1.5)));
                }
            }
        }
    }


    world.add(make_shared<sphere>(vec3(0, 1, 0), 1.0, make_shared<dielectric>(1.5)));

    world.add(
        make_shared<sphere>(vec3(-4, 1, 0), 1.0, make_shared<lambertian>(make_shared<constant_texture>(vec3(0.4, 0.2, 0.1)))));

    world.add(
        make_shared<sphere>(vec3(4, 1, 0), 1.0, make_shared<metal>(vec3(0.7, 0.6, 0.5), 0.0)));

    return world;
}

hittable_list motion_blur_random_scene() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(
        make_shared<constant_texture>(vec3(0.2, 0.3, 0.1)),
        make_shared<constant_texture>(vec3(0.9, 0.9, 0.9))
    );

    world.add(make_shared<sphere>(vec3(0,-1000,0), 1000, make_shared<lambertian>(checker)));

    int i = 1;
    for (int a = -10; a < 10; a++) {
        for (int b = -10; b < 10; b++) {
            auto choose_mat = random_double();
            vec3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());
            if ((center - vec3(4, .2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = vec3::random() * vec3::random();
                    world.add(make_shared<moving_sphere>(
                        center, center + vec3(0, random_double(0,.5), 0), 0.0, 1.0, 0.2,
                        make_shared<lambertian>(make_shared<constant_texture>(albedo))));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = vec3::random(.5, 1);
                    auto fuzz = random_double(0, .5);
                    world.add(
                        make_shared<sphere>(center, 0.2, make_shared<metal>(albedo, fuzz)));
                } else {
                    // glass
                    world.add(make_shared<sphere>(center, 0.2, make_shared<dielectric>(1.5)));
                }
            }
        }
    }

    world.add(make_shared<sphere>(vec3(0, 1, 0), 1.0, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(
        vec3(-4, 1, 0), 1.0, make_shared<lambertian>(make_shared<constant_texture>(vec3(0.4, 0.2, 0.1)))));
    world.add(make_shared<sphere>(
        vec3(4, 1, 0), 1.0, make_shared<metal>(vec3(0.7, 0.6, 0.5), 0.0)));

    world = static_cast<hittable_list>(make_shared<bvh_node>(world, 0, 1));
    return world;
}

hittable_list two_spheres() {
    hittable_list objects;

    auto checker = make_shared<checker_texture>(
        make_shared<constant_texture>(vec3(0.2, 0.3, 0.1)),
        make_shared<constant_texture>(vec3(0.9, 0.9, 0.9))
    );

    objects.add(make_shared<sphere>(vec3(0,-10, 0), 10, make_shared<lambertian>(checker)));
    objects.add(make_shared<sphere>(vec3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    return objects;
}

std::pair<hittable_list, camera> two_perlin_spheres() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(vec3(0,-1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(vec3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    vec3 lookfrom(13,2,3);
    vec3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;

    camera cam(lookfrom, lookat, vup, 20, 1.0, aperture, dist_to_focus, 0.0, 1.0);
    return std::make_pair(objects, cam);
}

std::pair<hittable_list, camera> test_obj_loader() {
    hittable_list objects;
        
    auto text = make_shared<constant_texture>(vec3(0.5, 0., 0.5));
    // auto text = make_shared<constant_texture>(vec3(0.0, 0., 0.0));

    // objects.add(make_shared<triangle>(vec3(-1,0,0), vec3(1,0,0), vec3(0,2,0), vec3(0,0,1), vec3(0,0,1), vec3(0,0,1), make_shared<lambertian>(text)));

    //test obj loader
    Assimp::Importer importer;
    double scale = 0.1;
    // const aiScene *scene = importer.ReadFile("/home/ahapbean/computer_graphics/PPCA-Raytracer-2022/models/1.obj", aiProcess_Triangulate);
    const aiScene *scene = importer.ReadFile("/home/ahapbean/computer_graphics/raytracing/cub.obj", aiProcess_Triangulate);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error loading OBJ file: " << importer.GetErrorString() << std::endl;
        exit(-1);
    }
    aiVector3D objectCenter(0., 0.0, 0.);
    int numVertices = 0;
    // std::cerr << scene->mNumMeshes << std::endl;
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];
        for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
            aiFace face = mesh->mFaces[j];
            aiVector3D vertex0 = mesh->mVertices[face.mIndices[0]]; //fuck here
            aiVector3D vertex1 = mesh->mVertices[face.mIndices[1]];
            aiVector3D vertex2 = mesh->mVertices[face.mIndices[2]];
            objectCenter += vertex0 + vertex1 + vertex2;
            numVertices += 3;
            vec3 normal = cross(convert_to_vec3(vertex0), convert_to_vec3(vertex1));
            normal = normalize(normal);
            objects.add(make_shared<triangle>(convert_to_vec3(vertex0), convert_to_vec3(vertex1), convert_to_vec3(vertex2), normal, normal, normal, make_shared<lambertian>(text)));
            
            // std::cerr << "here" << std::endl;
            //这个法向量怎么算???
        }
    }
    vec3 vertex0(1,0,0), vertex1(-1,0,0), vertex2(0,2,0);
    vec3 normal = cross(vertex0, vertex2);
    normal = normalize(normal);
    // objects.add(make_shared<triangle>(vertex0, vertex1, vertex2, normal, normal, normal, make_shared<lambertian>(text)));
    shared_ptr<hittable> box1 = make_shared<box>(vec3(0, 0, 0), vec3(2, 2, 2), make_shared<lambertian>(text));
    box1 = make_shared<rotate_y>(box1,  15);
    // box1 = make_shared<translate>(box1, vec3(265,0,295));
    // objects.add(box1);

    
    objectCenter /= numVertices;
    objects = static_cast<hittable_list>(make_shared<bvh_node>(objects, 0, 1));
    //去掉这句变慢好多，说明是坐标问题或者我的bound_box有问题

    //三角形类 + bvh有问题

    vec3 lookfrom(0,1,10);
    // vec3 lookfrom(0,0,10);
    // vec3 lookat = convert_to_vec3(objectCenter);
    vec3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 1.0;
    auto aperture = 0.0;

    camera cam(lookfrom, lookat, vup, 40, 1.0, aperture, dist_to_focus, 0.0, 1.0);
    std::cerr << lookat << std::endl;
    return std::make_pair(objects, cam);
}

hittable_list earth() {
    int nx, ny, nn;
    unsigned char* texture_data = stbi_load("/home/ahapbean/computer_graphics/raytracing/earthmap.jpg", &nx, &ny, &nn, 0);

    auto earth_surface =
        make_shared<lambertian>(make_shared<image_texture>(texture_data, nx, ny));
    auto globe = make_shared<sphere>(vec3(0,0,0), 2, earth_surface);

    return hittable_list(globe);
}

hittable_list simple_light() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(vec3(0,-1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(vec3(0,2,0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(make_shared<constant_texture>(vec3(4,4,4)));
    objects.add(make_shared<sphere>(vec3(0,7,0), 2, difflight));
    objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));

    return objects;
}

std::pair<hittable_list, camera> cornell_box() {
    hittable_list objects;

    auto red = make_shared<lambertian>(make_shared<constant_texture>(vec3(0.65, 0.05, 0.05)));
    auto white = make_shared<lambertian>(make_shared<constant_texture>(vec3(0.73, 0.73, 0.73)));
    auto green = make_shared<lambertian>(make_shared<constant_texture>(vec3(0.12, 0.45, 0.15)));
    auto light = make_shared<diffuse_light>(make_shared<constant_texture>(vec3(15, 15, 15)));



    objects.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green)));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 555, white)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white)));

    shared_ptr<hittable> box1 = make_shared<box>(vec3(0, 0, 0), vec3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1,  15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    objects.add(box1);

    shared_ptr<hittable> box2 = make_shared<box>(vec3(0,0,0), vec3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));
    objects.add(box2);

    vec3 lookfrom(278, 278, -800);
    vec3 lookat(278,278,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    auto vfov = 40.0;

    camera cam(lookfrom, lookat, vup, vfov, 1.0, aperture, dist_to_focus, 0.0, 1.0);

    return std::make_pair(objects, cam);
}

hittable_list cornell_smoke() {
    hittable_list objects;

    auto red = make_shared<lambertian>(make_shared<constant_texture>(vec3(0.65, 0.05, 0.05)));
    auto white = make_shared<lambertian>(make_shared<constant_texture>(vec3(0.73, 0.73, 0.73)));
    auto green = make_shared<lambertian>(make_shared<constant_texture>(vec3(0.12, 0.45, 0.15)));
    auto light = make_shared<diffuse_light>(make_shared<constant_texture>(vec3(7, 7, 7)));

    objects.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green)));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 555, white)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white)));

    shared_ptr<hittable> box1 = make_shared<box>(vec3(0,0,0), vec3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1,  15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));

    shared_ptr<hittable> box2 = make_shared<box>(vec3(0,0,0), vec3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));

    objects.add(
        make_shared<constant_medium>(box1, 0.01, make_shared<constant_texture>(vec3(0.,0.,0.))));
    objects.add(
        make_shared<constant_medium>(box2, 0.01, make_shared<constant_texture>(vec3(1.,1.,1.))));

    return objects;
}
//注意看一下这个代码
std::pair<hittable_list, camera> final_scene() {
    hittable_list boxes1;
    auto ground =
        make_shared<lambertian>(make_shared<constant_texture>(vec3(0.48, 0.83, 0.53)));

    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1,101);
            auto z1 = z0 + w;

            boxes1.add(make_shared<box>(vec3(x0,y0,z0), vec3(x1,y1,z1), ground));
        }
    }

    hittable_list objects;

    objects.add(make_shared<bvh_node>(boxes1, 0, 1));

    auto light = make_shared<diffuse_light>(make_shared<constant_texture>(vec3(7, 7, 7)));
    objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));

    auto center1 = vec3(400, 400, 200);
    auto center2 = center1 + vec3(30,0,0);
    auto moving_sphere_material =
        make_shared<lambertian>(make_shared<constant_texture>(vec3(0.7, 0.3, 0.1)));
    objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));

    objects.add(make_shared<sphere>(vec3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    objects.add(make_shared<sphere>(
        vec3(0, 150, 145), 50, make_shared<metal>(vec3(0.8, 0.8, 0.9), 10.0)
    ));

    auto boundary = make_shared<sphere>(vec3(360, 150, 145), 70, make_shared<dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(
        boundary, 0.2, make_shared<constant_texture>(vec3(0.2, 0.4, 0.9))
    ));
    boundary = make_shared<sphere>(vec3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    objects.add(make_shared<constant_medium>(
        boundary, .0001, make_shared<constant_texture>(vec3(1,1,1))));

    int nx, ny, nn;
    auto tex_data = stbi_load("/home/ahapbean/computer_graphics/raytracing/earthmap.jpg", &nx, &ny, &nn, 0);
    auto emat = make_shared<lambertian>(make_shared<image_texture>(tex_data, nx, ny));
    objects.add(make_shared<sphere>(vec3(400,200, 400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.1);
    objects.add(make_shared<sphere>(vec3(220,280, 300), 80, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    auto white = make_shared<lambertian>(make_shared<constant_texture>(vec3(0.73, 0.73, 0.73)));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(vec3::random(0,165), 10, white));
    }

    objects.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2, 0.0, 1.0), 15),
            vec3(-100,270,395)
        )
    );

    const auto aspect_ratio = 1.0;  //NOTE here
    vec3 lookfrom(478, 278, -600);
    vec3 lookat(278,278,0);
    vec3 vup(0,1,0);    //一般就是相机正放
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    auto vfov = 40.0;
    // camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
    

    return std::make_pair(objects, cam);
}



std::pair<hittable_list, camera> test() {
    hittable_list objects;
        
    auto text = make_shared<constant_texture>(vec3(0.5, 0., 0.5));

    // objects.add(make_shared<triangle>(vec3(-1,0,0), vec3(1,0,0), vec3

    //test obj loader
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile("models/2.obj", aiProcess_Triangulate);
    // const aiScene *scene = importer.ReadFile("/data/wangshaobo/zxd/raytracing-in-CG-course/cub.obj", aiProcess_Triangulate);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error loading OBJ file: " << importer.GetErrorString() << std::endl;
        exit(-1);
    }
    vec3 objectCenter(0., 0., 0.);
    int numVertices = 0;
    // std::cerr << scene->mNumMeshes << std::endl;
    std::vector<vec3> ls;ls.clear();
    std::vector<double> store_u, store_v;  
    std::vector<vec3> normals;normals.clear();
    store_u.clear();store_v.clear();

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];
        for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
            aiFace face = mesh->mFaces[j];
            for(int i = 0; i < 3; ++i) {    // must be triangle !
                ls.push_back(convert_to_vec3(mesh->mVertices[face.mIndices[i]]));
                // store_u.push_back(mesh->mTextureCoords[0][face.mIndices[i]].x);
                // store_v.push_back(mesh->mTextureCoords[0][face.mIndices[i]].y);
                objectCenter += ls.back();
            }
            numVertices += 3;
            normals.push_back(vec3(0,0,0));
        }
    }
    /*
    // 法向量
            aiVector3D normal = mesh->mNormals[vertexIndex];
            vec3 normalVec = convert_to_vec3(normal);

            // 纹理坐标 (u, v)
            aiVector3D texCoord = mesh->mTextureCoords[0][vertexIndex];  // Assuming only one set of texture coordinates
            vec2 uv(texCoord.x, texCoord.y);
    */

    
    objectCenter /= numVertices;
    double abs_Z = -1e18, abs_Y = -1e18, abs_X = -1e18;
    // box1 = make_shared<translate>(box1, vec3(265,0,295));
    for(int i = 0; i < ls.size(); ++i) {
        // std::cerr << ls[i] << " " << convert_to_vec3(objectCenter) << std::endl
        // std::cerr << "after trans" << ls[i] - convert_to_vec3(objectCenter) << std::endl;
        // std::cerr << "after trans" << ls[i] - convert_to_vec3(objectCenter) << std::endl;
        // std::cerr << "after trans" << ls[i] - convert_to_vec3(objectCenter) << std::endl;
        ls[i] += vec3(-objectCenter[0], -objectCenter[1], -objectCenter[2]);
        abs_Z = ffmax(ffmax(abs(ls[i].z()), ffmax(abs(ls[i].z()), abs(ls[i].z()))), abs_Z);
        abs_Y = ffmax(ffmax(abs(ls[i].y()), ffmax(abs(ls[i].y()), abs(ls[i].y()))), abs_Y);
        abs_X = ffmax(ffmax(abs(ls[i].x()), ffmax(abs(ls[i].x()), abs(ls[i].x()))), abs_X);
    }
    //normalize物体
    vec3 scale(1/abs_X, 1/abs_Y, 1/abs_Z);  //用于sacle
    for(int i = 0; i < ls.size(); ++i) {
        // std::cerr << ls[i] << " " << convert_to_vec3(objectCenter) << std::endl;
        ls[i] = ls[i] * scale;
    }
    for(int i = 0; i < normals.size(); ++i) {   //recalculate normal
        vec3 x = ls[3 * i], y = ls[3 * i + 1], z = ls[3 * i + 2];
        vec3 v1 = x - y, v2 = z - y;
        vec3 normal = cross(v1, v2);
        normal = normalize(normal);
        normals[i] = normal;
    }
    std::cerr << ls.size() << " " << store_u.size() << " " << store_v.size() << std::endl;
    // exit(-1);
    for(int i = 0; i < normals.size(); ++i) {
        vec3 point[3];
        for(int j = 0; j < 3; ++j) {
            point[j] = ls[3 * i + j];   
        }
        auto normal = normals[i];
        auto obj = make_shared<triangle>(point[0], point[1], point[2], normal, normal, normal, make_shared<lambertian>(text));
        auto new_obj = make_shared<rotate_z>(obj, 90.0);
        //旋转基本没有问题
        // auto new_obj = make_shared<rotate_x>(obj, -45.0);
        // auto new_obj1 = make_shared<rotate_y>(obj, 45.0);
        objects.add(new_obj);
    }

    objects = static_cast<hittable_list>(make_shared<bvh_node>(objects, 0, 1));
    // vec3 lookfrom(0,0,10);
    vec3 lookfrom(0,2,5);
    vec3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 1.0;
    auto aperture = 0.0;
    camera cam(lookfrom, lookat, vup, 40, 1.0, aperture, dist_to_focus, 0.0, 1.0);
    // std::cerr << lookat << std::endl;
    return std::make_pair(objects, cam);
}
