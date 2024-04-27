#include <iostream>
#include <vector>
#include"Colin3DLibrary.h"
using namespace std;
void example_1();
void example_2();
void example_3();
void example_4();
// Made by Arunakemi Colin Varela
// for the Computer Graphics course at ESCOM
int main(int argc, char const *argv[])
{
    // Here are some examples on how to use the "Colin3DLibrary.h"!
    // Uncomment the example you want to test, comment the others and run the program

       example_1(); // Animation made by rotating the monkey and using different rendering techniches
    // example_2(); // Four Monkeys on the center using Phong
    // example_3(); // A cube made out of Six patches lined up together (animation of the cube rotating)
    // example_4(); // Animation of the cube of patches scaling the tangents 

    // If you have any more questions feel free to message me on my GitHub or my twitter @venado_cosmico
    return 42;
}
void example_1()
{
    // Animation made by rotating the monkey and using different 
    // rendering techniches 
    Raster fullhd(1550,1250);
    fullhd.setBackgroundColor(0x10);
    fullhd.clear();
    fullhd.setAmbientLight({0, 0, 0});
    fullhd.setCamara('z', false, 15000);

    Object3D mono("Monkey",0);
    mono.rotar('y', M_PI);
    mono.escalar(200);
    mono.mover(0,0,30000);
    mono.convertirVLFVertexNormal("monoVN2");
    mono.createMaterial(Material(1, 0.9, 0.2, 50));
    mono.setRGB({255, 100, 100});
    fullhd.setColor(255, 200, 200);
    Luz luz({-250, -250, 27000}, {255, 255, 255}, {0.0000005, 0.000001, 0});

    Transformacion t(0,0,30000);
    t = t.unir(Transformacion('y', 2.4 * (M_PI/180)));
    t = t.unir(Transformacion(0,0,-30000));

    for(int x = 0; x < 901; x++)
    {
             if(x < 151) fullhd.paintWireFrameScene({mono},false);
        else if(x < 301) fullhd.paintWireFrameScene({mono},true);
        else if(x < 451) fullhd.paint3DZBuffer(mono,{255,100,100});
        else if(x < 601) fullhd.paintFlatScene({mono},{luz});
        else if(x < 751) fullhd.paintGouraudScene({mono},{luz});
        else if(x < 901) fullhd.paintPhongScene({mono},{luz});

        fullhd.make(to_string(x));
        fullhd.clear();
        mono.transformar(t);
    }
}
void example_2()
{
    // Four Monkeys on the center using Phong
    int rx = 1920;
    int ry = 1920;
    Raster fullhd(rx, ry);
    fullhd.setColor(0, 0, 0);
    fullhd.setCamara('z', false, 5000);

    Object3D mono("monoVN", 3);
    mono.rotar('y', M_PI);
    mono.escalar(100);
    mono.mover(0, 0, 8000);

    int extra = 120;
    vector<Object3D> monos;
    mono.mover((-rx / 3) - extra, -ry / 3, 0);
    monos.push_back(mono);
    mono.mover((rx * 2 / 3) + extra * 2, 0, 0);
    monos.push_back(mono);
    mono.mover(0, ry * 2 / 3, 0);
    monos.push_back(mono);
    mono.mover((-rx * 2 / 3) - extra * 2, 0, 0);
    monos.push_back(mono);

    vector<Luz> luces;
    //luces.push_back(Luz({-800, -900, 7000}, {255, 255, 100}, {0.0000008, 0.00005, 0}));
    //luces.push_back(Luz({900, 900, 7000}, {100, 100, 100}, {0.000001, 0.0001, 0}));
    luces.push_back(Luz({0, 0, 7000}, {255, 255, 255}, {0.0000008, 0.00001, 0}));

    fullhd.setAmbientLight({-140, -140, -140});
    for (int i = 0; i < monos.size(); i++)
        monos[i].createMaterial(Material(0.8, 0.55, 0.3, 10000));

    monos[0].setRGB({100, 0, 0});
    monos[1].setRGB({100, 0, 100});
    monos[2].setRGB({0, 100, 0});
    monos[3].setRGB({0, 0, 100});
    fullhd.paintPhongScene(monos, luces);
    fullhd.make("Four_monkeys");
}
void example_3()
{
    // A cube made out of Six patches lined up together (animation of the cube rotating)
    Raster fullhd(1920, 1080);
    fullhd.setCamara('z', false, 7000);
    fullhd.setAmbientLight({-30, -30, -30});
    //*
    Patch3D patch1, patch2, patch3, patch4, patch5, patch6;
    patch1.setResolucion(25, 25);
    patch2.setResolucion(25, 25);
    patch3.setResolucion(25, 25);
    patch4.setResolucion(25, 25);
    patch5.setResolucion(25, 25);
    patch6.setResolucion(25, 25);
    double m,m2,m3;
    m = 500;
    m2 = 500;
    m3 = 0;
    patch1.setPuntosControl(
        {{m, m, m}, {m, m, -m}, {m2, m2, m2}, {-m2, -m2, m2}},
        {{-m, m, m}, {-m, m, -m}, {-m2, m2, m2}, {m2, -m2, m2}},
        {{m2, m2, m2}, {m2, m2, -m2}, {m3, m3, m3}, {m3, m3, m3}},
        {{m2, -m2, -m2}, {m2, -m2, m2}, {m3, m3, m3}, {m3, m3, m3}}
    );
    patch2.setPuntosControl(
        {{m, -m, -m}, {m, m, -m}, {m2, -m2, -m2}, {-m2, -m2,m2}},
        {{m, -m, m}, {m, m, m}, {m2, -m2, m2}, {-m2, -m2, -m2}},
        {{m2, -m2, -m2}, {m2, m2, -m2}, {m3, m3, m3}, {m3, m3, m3}},
        {{-m2, m2, -m2}, {-m2, -m2, -m2}, {m3, m3, m3}, {m3, m3, m3}}
    );
    patch3.setPuntosControl(
        {{m, m, m}, {-m, m, m}, {m2, m2, m2}, {m2, -m2,-m2}},
        {{m, -m, m}, {-m, -m, m}, {m2, -m2, m2}, {m2, m2, -m2}},
        {{m2, m2, m2}, {-m2, m2, m2}, {m3, m3, m3}, {m3, m3, m3}},
        {{-m2, m2, -m2}, {m2, m2, -m2}, {m3, m3, m3}, {m3, m3, m3}}
    );
    patch4.setPuntosControl(
        {{m, -m, -m}, {m, -m, m}, {m2, -m2, -m2}, {-m2, m2,-m2}},
        {{-m, -m, -m}, {-m, -m,m}, {-m2, -m2, -m2}, {m2, m2, -m2}},
        {{m2, -m2, -m2}, {m2, -m2, m2}, {m3, m3, m3}, {m3, m3, m3}},
        {{m2, m2, m2}, {m2, m2, -m2}, {m3, m3, m3}, {m3, m3, m3}}
    );
    patch5.setPuntosControl(
        {{-m, m, -m}, {m, m, -m}, {-m2, m2, -m2}, {-m2, -m2,m2}},
        {{-m,-m, -m}, {m, -m, -m}, {-m2, -m2, -m2}, {-m2, m2, m2}},
        {{-m2, m2, -m2}, {m2, m2, -m2}, {m3, m3, m3}, {m3, m3, m3}},
        {{m2, m2, m2}, {-m2, m2, m2}, {m3, m3, m3}, {m3, m3, m3}}
    );
    patch6.setPuntosControl(
        {{-m, m, -m}, {-m, -m, -m}, {-m2, m2, -m2}, {m2, m2,m2}},
        {{-m, m, m}, {-m, -m, m}, {-m2, m2, m2}, {m2, m2, -m2}},
        {{-m2, m2, -m2}, {-m2, -m2, -m2}, {m3, m3, m3}, {m3, m3, m3}},
        {{m2, -m2, -m2}, {m2, m2, -m2}, {m3, m3, m3}, {m3, m3, m3}}
    );//*/

    vector<Object3D> pt;
    pt.push_back(patch1.convertirAObjecto3D("p1"));
    pt.push_back(patch2.convertirAObjecto3D("p2"));
    pt.push_back(patch3.convertirAObjecto3D("p3"));
    pt.push_back(patch4.convertirAObjecto3D("p4"));
    pt.push_back(patch5.convertirAObjecto3D("p5"));
    pt.push_back(patch6.convertirAObjecto3D("p6"));
    //*/

    Luz l({-200, -300, 13500}, {200, 200, 255}, {0.000001, 0.000001, 0});
    int cnt = 0;
    for(Object3D &p : pt)
    {
        p.createMaterial(Material(1, 0.3, 1, 100));
        p.setRGB({cnt * 30, (int) pow(cnt, 2) * 5, (int) pow(cnt, 2) * 8});
        cnt++;
        //Object3D p("intentoPatch", 3);
        //p.escalar(23);
        //p.rotar('x', 40* (M_PI/180));
        //p.centrarOrigen();
        p.mover(0, 0, 15000);
    }
    fullhd.setColor(50, 50, 50);
    //fullhd.paint3DZBuffer(p,{255,50,50});
    fullhd.setBackgroundColor(0x50);
    Transformacion t(0, 0, 15000);
    t = t.unir(Transformacion('y', 10 * (M_PI / 180)));
    t = t.unir(Transformacion('x', 3 * (M_PI / 180)));
    t = t.unir(Transformacion(0, 0, -15000));
    for (int i = 0; i <= 100; i++)
    {
        fullhd.clear();
        //fullhd.paintGouraudScene(pt, /*true);/*/{l});
        fullhd.paintPhongScene(pt, /*true);/*/{l});
        //fullhd.paintWireFrameScene(pt,false);
        fullhd.make(to_string(i));
        for(Object3D &p : pt)
            p.transformar(t);
    }
}
void example_4()
{
    // Animation of the cube of patches scaling the tangents 
    Raster fullhd(1920, 1080);
    fullhd.setCamara('z', false, 7000);
    fullhd.setAmbientLight({-30, -30, -30});
    Luz l({0, -300, 13500}, {200, 200, 255}, {0.000002, 0.000001, 0});
    fullhd.setColor(50, 50, 50);
    fullhd.setBackgroundColor(0x50);
    Transformacion t(0, 0, 15000);
    t = t.unir(Transformacion('y', 10 * (M_PI / 180)));
    t = t.unir(Transformacion('x', 3 * (M_PI / 180)));
    t = t.unir(Transformacion(0, 0, -15000));
    //*
    double m, m2, m3;
    m = 500;
    m2 = 0;
    m3 = 0;
    for (int i = 0; i <= 300; i++)
    {
        
        Patch3D patch1, patch2, patch3, patch4, patch5, patch6;
        patch1.setResolucion(25, 25);
        patch2.setResolucion(25, 25);
        patch3.setResolucion(25, 25);
        patch4.setResolucion(25, 25);
        patch5.setResolucion(25, 25);
        patch6.setResolucion(25, 25);
        
        patch1.setPuntosControl(
            {{m, m, m}, {m, m, -m}, {m2, m2, m2}, {-m2, -m2, m2}},
            {{-m, m, m}, {-m, m, -m}, {-m2, m2, m2}, {m2, -m2, m2}},
            {{m2, m2, m2}, {m2, m2, -m2}, {m3, m3, m3}, {m3, m3, m3}},
            {{m2, -m2, -m2}, {m2, -m2, m2}, {m3, m3, m3}, {m3, m3, m3}});
        patch2.setPuntosControl(
            {{m, -m, -m}, {m, m, -m}, {m2, -m2, -m2}, {-m2, -m2, m2}},
            {{m, -m, m}, {m, m, m}, {m2, -m2, m2}, {-m2, -m2, -m2}},
            {{m2, -m2, -m2}, {m2, m2, -m2}, {m3, m3, m3}, {m3, m3, m3}},
            {{-m2, m2, -m2}, {-m2, -m2, -m2}, {m3, m3, m3}, {m3, m3, m3}});
        patch3.setPuntosControl(
            {{m, m, m}, {-m, m, m}, {m2, m2, m2}, {m2, -m2, -m2}},
            {{m, -m, m}, {-m, -m, m}, {m2, -m2, m2}, {m2, m2, -m2}},
            {{m2, m2, m2}, {-m2, m2, m2}, {m3, m3, m3}, {m3, m3, m3}},
            {{-m2, m2, -m2}, {m2, m2, -m2}, {m3, m3, m3}, {m3, m3, m3}});
        patch4.setPuntosControl(
            {{m, -m, -m}, {m, -m, m}, {m2, -m2, -m2}, {-m2, m2, -m2}},
            {{-m, -m, -m}, {-m, -m, m}, {-m2, -m2, -m2}, {m2, m2, -m2}},
            {{m2, -m2, -m2}, {m2, -m2, m2}, {m3, m3, m3}, {m3, m3, m3}},
            {{m2, m2, m2}, {m2, m2, -m2}, {m3, m3, m3}, {m3, m3, m3}});
        patch5.setPuntosControl(
            {{-m, m, -m}, {m, m, -m}, {-m2, m2, -m2}, {-m2, -m2, m2}},
            {{-m, -m, -m}, {m, -m, -m}, {-m2, -m2, -m2}, {-m2, m2, m2}},
            {{-m2, m2, -m2}, {m2, m2, -m2}, {m3, m3, m3}, {m3, m3, m3}},
            {{m2, m2, m2}, {-m2, m2, m2}, {m3, m3, m3}, {m3, m3, m3}});
        patch6.setPuntosControl(
            {{-m, m, -m}, {-m, -m, -m}, {-m2, m2, -m2}, {m2, m2, m2}},
            {{-m, m, m}, {-m, -m, m}, {-m2, m2, m2}, {m2, m2, -m2}},
            {{-m2, m2, -m2}, {-m2, -m2, -m2}, {m3, m3, m3}, {m3, m3, m3}},
            {{m2, -m2, -m2}, {m2, m2, -m2}, {m3, m3, m3}, {m3, m3, m3}}); //*/

        vector<Object3D> pt;
        pt.push_back(patch1.convertirAObjecto3D_VlfVN());
        pt.push_back(patch2.convertirAObjecto3D_VlfVN());
        pt.push_back(patch3.convertirAObjecto3D_VlfVN());
        pt.push_back(patch4.convertirAObjecto3D_VlfVN());
        pt.push_back(patch5.convertirAObjecto3D_VlfVN());
        pt.push_back(patch6.convertirAObjecto3D_VlfVN());
        //*/
        int cnt = 1;
        for (Object3D &p : pt)
        {
            p.createMaterial(Material(1, 0.3, 1, 100));
            p.setRGB({cnt * 35, (int)(pow(cnt, 3) * 5)/4, (int)(pow(cnt, 2) * 8)/2});
            cnt++;
            //Object3D p("intentoPatch", 3);
            //p.escalar(23);
            //p.rotar('x', 40* (M_PI/180));
            //p.centrarOrigen();
            p.mover(0, 0, 15000);
        }
    
        fullhd.clear();
        for (int times = 0; times < i; times++)
            for (Object3D &p : pt)
                p.transformar(t);
        //fullhd.paintGouraudScene(pt, /*true);/*/{l});
        fullhd.paintPhongScene(pt, /*true);/*/ {l});
        //fullhd.paintWireFrameScene(pt,false);
        fullhd.make(to_string(i));
        

        m2 += 5;
    }
}
