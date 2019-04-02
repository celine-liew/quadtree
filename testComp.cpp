#define CATCH_CONFIG_MAIN
#include "cs221util/catch.hpp"
#include <vector>
#include <sys/stat.h>
#include <iostream>
#include "cs221util/PNG.h"
#include "cs221util/HSLAPixel.h"
#include "stats.h"
#include "toqutree.h"

using namespace std;
using namespace cs221util;


TEST_CASE("stats::basic rectArea","[weight=1][part=stats]"){

    PNG data; data.resize(2,2);

    stats s(data);
    pair<int,int> ul(0,0);
    pair<int,int> lr(1,1);
    long result = s.rectArea(ul,lr);

    REQUIRE(result == 4);
    
}

TEST_CASE("stats::basic getAvg","[weight=1][part=stats]"){
    PNG data; data.resize(2,2);
    for (int i = 0; i < 2; i ++){
        for (int j = 0; j < 2; j++){
            HSLAPixel * p = data.getPixel(i,j);
            p->h = 135*j + i * 90;
            p->s = 1.0;
            p->l = 0.5;
            p->a = 1.0;
        }
    }
    stats s(data);
    pair<int,int> ul(0,0);
    pair<int,int> lr(1,1);
    HSLAPixel result = s.getAvg(ul,lr);
    std::cout << result << endl;
    HSLAPixel expected(112.5,1.0, 0.5);

    REQUIRE(result == expected);
}

TEST_CASE("stats::basic getAvg 2","[weight=1][part=stats]"){
    PNG data; data.resize(3,3);
    for (int i = 0; i < 3; i ++){
        for (int j = 0; j < 3; j++){
            HSLAPixel * p = data.getPixel(i,j);
            p->h = 50*j + i * 90;
            p->s = 0.5;
            p->l = 0.4;
            p->a = 1.0;
        }
    }
    stats s(data);
    pair<int,int> ul(0,1);
    pair<int,int> lr(0,1);
    HSLAPixel result = s.getAvg(ul,lr);
    std::cout << "size 0,1 0,1: " << result << endl;
    HSLAPixel expected(230,0.5, 0.4);

    REQUIRE(result == expected);
}

TEST_CASE("stats::basic getAvg 3","[weight=1][part=stats]"){
    PNG data; data.resize(3,3);
    for (int i = 0; i < 3; i ++){
        for (int j = 0; j < 3; j++){
            HSLAPixel * p = data.getPixel(i,j);
            p->h = 90*j + i * 50;
            p->s = 0.5;
            p->l = 0.4;
            p->a = 1.0;
        }
    }
    stats s(data);
    pair<int,int> ul(0,0);
    pair<int,int> lr(0,1);
    HSLAPixel result = s.getAvg(ul,lr);
    std::cout << "size 1,0 1,0: " << result << endl;
    HSLAPixel expected(230,0.5, 0.4);

    REQUIRE(result == expected);
}

// TEST_CASE("stats::basic getAvg 4","[weight=1][part=stats]"){
//     PNG data; data.resize(3,3);
//     for (int i = 0; i < 3; i ++){
//         for (int j = 0; j < 3; j++){
//             HSLAPixel * p = data.getPixel(i,j);
//             p->h = 40.242 *j + i * 100;
//             p->s = 0.75;
//             p->l = 0.45;
//             p->a = 1.0;
//         }
//     }
//     stats s(data);
//     pair<int,int> ul(0,0);
//     pair<int,int> lr(0,1);
//     HSLAPixel result = s.getAvg(ul,lr);
//     std::cout << "size 0,0 0,1: " << result << endl;
//     HSLAPixel expected(50,0.75, 0.45);

//     REQUIRE(result == expected);
// }

// TEST_CASE("stats::basic getAvg 5","[weight=1][part=stats]"){
//     PNG data; data.resize(4,4);
//     for (int i = 0; i < 4; i ++){
//         for (int j = 0; j < 4; j++){
//             HSLAPixel * p = data.getPixel(i,j);
//             p->h = 7 *j + i * 4;
//             p->s = 0.625;
//             p->l =  0.5;
//             p->a = 1.0;
//         }
//     }
//     stats s(data);
//     pair<int,int> ul(1,1);
//     pair<int,int> lr(2,3);
//     HSLAPixel result = s.getAvg(ul,lr);
//     // std::cout << "size 3,3: " << result << endl;
//     HSLAPixel expected(157.5,1.0, 0.5);

//     REQUIRE(result == expected);
// }

TEST_CASE("stats::basic entropy","[weight=1][part=stats]"){
    // std::cout << "basic entropy" << endl;
    PNG data; data.resize(2,2);
    for (int i = 0; i < 2; i ++){
        for (int j = 0; j < 2; j++){
            HSLAPixel * p = data.getPixel(i,j);
            p->h = 135*j + i * 90;
            p->s = 1.0;
            p->l = 0.5;
            p->a = 1.0;
        }
    }
    stats s(data);
    pair<int,int> ul(0,0);
    pair<int,int> lr(1,1);
    long result = s.entropy(ul,lr);

    REQUIRE(result == 2);
}

// TEST_CASE("toqutree::basic ctor render","[weight=1][part=toqutree]"){
//     PNG img;
//     img.readFromFile("images/stanleySquare.png");

//     toqutree t1(img,9);
//     // std::cout << "built the tree, now render it! " << endl;
//     PNG out = t1.render();
//     out.convert();

//     REQUIRE(out==img);
// }

// TEST_CASE("toqutree::basic copy","[weight=1][part=toqutree]"){
//     PNG img;
//     img.readFromFile("images/geo.png");

//     toqutree t1(img,5);
//     toqutree t1copy(t1);

//     PNG out = t1copy.render();

//     REQUIRE(out==img);
// }

// TEST_CASE("toqutree::basic prune","[weight=1][part=toqutree]"){
//     std::cout << "basic prune" << endl;
//     PNG img;
//     img.readFromFile("images/ada.png");
    
//     toqutree t1(img,9);

//     t1.prune(0.05);
//     // std::cout << "after prune " << endl;
//     PNG result = t1.render();

//     PNG expected; expected.readFromFile("images/adaPrune.05.png");
//     result.convert();

//     REQUIRE(expected==result);
// }

