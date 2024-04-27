#include<iostream>
#include<vector>
#include<math.h>
#include<fstream>
#include<thread>
#include<sstream>
using namespace std;

// Colin3DLibrary
// Made by Arunakemi Colin Varela
// for the Computer Graphics course at ESCOM
// Last change : 20 / 01 / 2021
// Enjoy

// Math functions
double magnitud(vector<double> u)
{
    return sqrt(pow(u[0], 2) + pow(u[1], 2) + pow(u[2], 2));
}
double dotProd(vector<double> u, vector<double> v)
{
    double ans = 0;
    for (int i = 0; i < 3; i++)
    {
        ans += u[i] * v[i];
    }
    return ans;
}
vector<double> sumarVector(vector<double> u, vector<double> v)
{
    vector<double> nuevo(4, 1);
    for (int i = 0; i < 3; i++)
        nuevo[i] = u[i] + v[i];
    return nuevo;
}
vector<double> restarVector(vector<double> u, vector<double> v)
{
    vector<double> nuevo(4, 1);
    for (int i = 0; i < 3; i++)
        nuevo[i] = u[i] - v[i];
    return nuevo;
}
vector<double> reflejoVector(vector<double> u, vector<double> n)
{
    vector<double> r(3,0);
    double k = 2 * dotProd(u,n) * (1 / magnitud(n));
    for(int i = 0; i < 3; i++)
        r[i] = u[i] - (k * n[i]);
    return r;
}

// Classes
class Material
{
    double ambientK, diffuseK, specularK, specularAngle;
    
    public: 

    Material(double aK, double d,double s,double ns)
    {
        ambientK = aK;
        diffuseK = d;
        specularK = s;
        specularAngle = ns;
    }
    Material()
    {
        ambientK = 1;
        diffuseK = 1;
        specularK = 0;
        specularAngle = 1;
    }
    double getAmbientK() { return ambientK; }
    double getDiffuseK() { return diffuseK; }
    double getSpecularK() { return specularK; }
    double getSpecularAngle() { return specularAngle; }
    void setAmbientK(double x) { ambientK = x; }
    void setDiffuseK(double x) { diffuseK = x; }
    void setSpecularK(double x) { specularK = x; }
    void setSpecularAngle(double x) { specularAngle = x; }
};

class Luz
{
    vector<double> position;
    vector<int> color;
    vector<double> attenVar;
    vector<double> direction; 
    double angle;
    int type = 0; // 0 - Point light, 1 - SpotLight
    
    public: 

    Luz(vector<double> p, vector<int> c, vector<double> v)
    {
        position = p; color = c; attenVar = v; type = 0;
    }

    Luz(vector<double> p, vector<int> c, vector<double> v, vector<double> dir, double a)
    {
        position = p; color = c; attenVar = v; direction = dir; type = 1; angle = a;
    }

    // Bloque set
    void setPosition(vector<double> p) { position = p; }
    void setColor(vector<int> c) { color = c; }
    void setAttenVar(vector<double> v) { attenVar = v; }
    void setDirectionAngle(vector<double> d, double a) { direction = d; type = 1; angle = a; }
    void setType(int t) { type = t; } // 0 - point , 1 - spot
    // Bloque get
    vector<double> getPosition() { return position; }
    vector<int> getColor() { return color; }
    vector<double> getAttenVar() { return attenVar; }
    vector<double> getDirection() { return direction; }
    double getAngle() { return angle; }
    int getType() { return type; }
};

class Transformacion
{
    vector<vector<double>> tabla;
    public:
    // Constructors
    Transformacion() {}
    Transformacion(float f)
    {    setEscalar(f);                }
    Transformacion(float x, float y, float z)
    {    setTranslacion(x,y,z);       }
    Transformacion(char axis, float angle)
    {    setRotacion(axis,angle);       }

    // Asignación de tipo de transformacion
    void setRotacion(char axis, float angle) 
    {
        vector<vector<double>> nueva(4, vector<double>(4, 0));
        for (int i = 0; i < 4; i++)
            nueva[i][i] = 1;
        switch (axis)
        {
            case 'x':
                nueva[1][1] = cos(angle);
                nueva[2][1] = sin(angle);
                nueva[1][2] = - nueva[2][1];
                nueva[2][2] = nueva[1][1];
                break;
            case 'y':
                nueva[0][0] = cos(angle);
                nueva[0][2] = sin(angle);
                nueva[2][0] = - nueva[0][2];
                nueva[2][2] = nueva[0][0];
                break;
            case 'z':
                nueva[0][0] = cos(angle);
                nueva[1][0] = sin(angle);
                nueva[0][1] = - nueva[1][0];
                nueva[1][1] = nueva[0][0]; 
                break;
            default:
                cout << "Eje de rotacion " << axis << " invalido!";
                break;
        }
        tabla = nueva;
    }
    void setEscalar(float factor) 
    {
        vector<vector<double>> nueva;
        for (int i = 0; i < 4; i++)
        {
            vector<double> r;
            for (int i2 = 0; i2 < 4; i2++)
                r.push_back((i==i2)? ((i!=3)? factor : 1) : 0);
            nueva.push_back(r);
        }
        tabla = nueva;
    }
    void setTranslacion(float x, float y, float z) 
    {
        vector<vector<double>> nueva(4, vector<double>(4, 0));
        for (int i = 0; i < 4; i++)
            nueva[i][i] = 1;
        nueva[0][3] = x;
        nueva[1][3] = y;
        nueva[2][3] = z;
        tabla = nueva;
    }

    // Manejo de tabla independiente
    vector<vector<double>> getTabla() { return tabla; }
    void setTabla(vector<vector<double>> nueva) { tabla = nueva; }

    // Union de transformaciones
    Transformacion unir(Transformacion t)
    {
        vector<vector<double>> tabla2 = t.getTabla();
        vector<vector<double>> nueva(4, vector<double>(4, 0));
        for (int i = 0; i < 4; i++)
            for (int i2 = 0; i2 < 4; i2++)
                for (int i3 = 0; i3 < 4; i3++)
                    nueva[i][i2] += tabla[i][i3] * tabla2[i3][i2];
        Transformacion tran;
        tran.setTabla(nueva);
        return tran;
    }
};

class Object3D
{
    vector<vector<double>> puntos;
    vector<vector<double>> caras;
    vector<vector<double>> lados;
    vector<vector<double>> normales;
    vector<vector<double>> vertexNormals;

    vector<Material> faceMaterial;
    vector<int> rgb;
    bool triangulado = false;
    bool vlf = false;
    bool con_normales = false;
    bool con_material = false;
    bool con_vertexNormals = false;

    public:

    //Construcción, set y retorno de tipos
    Object3D()
    { rgb = vector<int>(3,0); }
    Object3D(string s, int i) //0 - obj, 1 - vlf, 2 - vlf normal
    { 
      rgb = vector<int>(3,0); 
      if(i==1) leerVlf(s); 
      else if(i==2) leerVlfNormal(s); 
      else if(i==3) leerVlfVertexNormal(s);
      else leerObj(s); 
    }
    Object3D(vector<int> colores)
    { rgb = colores; }
    Object3D(string s, int i, vector<int> colores) //0 - obj, 1 - vlf, 2 - vlf normal
    { 
      rgb = colores; 
      if(i==1) leerVlf(s); 
      else if(i==2) leerVlfNormal(s); 
      else if(i==3) leerVlfVertexNormal(s);
      else leerObj(s); 
    }

    void setHomemadeObjecto3D(vector<vector<double>> _puntos, vector<vector<double>> _caras)
    {
        caras = _caras; puntos = _puntos;
        for(vector<double> &p : puntos)
        {
            if(p.size() < 4) p.push_back(1);
            p[1] *= -1;
        }
    }
    
    bool isTriangulado() { return triangulado; }
    bool isVLF() { return vlf; }
    bool hasNormales() { return con_normales; }
    bool hasVertexNormals() { return con_vertexNormals; }
    void setRGB(vector<int> c)
    { 
        for(int i=0;i<3;i++)
        {
            if(c[i]>255) rgb[i] = 255;
            else if(c[i]<0)   rgb[i] = 0;
            else rgb[i] = c[i];
        }
    }    
    void setMaterial(Material mt) { for(int i=0; i < faceMaterial.size(); i++) faceMaterial[i] = mt; }
    void setFaceMaterial(int i, Material mt){ faceMaterial[i] = mt; }
    void createMaterial(Material mt) { faceMaterial = vector<Material>(caras.size(), mt); con_material = true; }
    
    // Lecturas de archivos 
    bool leerObj(string s)
    {
        ifstream archivo(s + ".obj");
        if (archivo.is_open())
        {
            string linea;
            while (getline(archivo, linea))
            {
                istringstream indv(linea);
                string num;
                getline(indv, num, ' ');
                string type = num;
                if (type == "v")
                {
                    vector<double> xyzw;
                    while (getline(indv, num, ' '))
                        xyzw.push_back(stof(num));
                    puntos.push_back(xyzw);

                }
                if (type == "f")
                {
                    vector<double> faces;
                    while (getline(indv, num, ' '))
                        faces.push_back((double) stoi(num));
                    caras.push_back(faces);
                }
            }
            for(vector<double> &p : puntos)
            {
                if(p.size() < 4) p.push_back(1);
                p[1] *= -1;
            }
                
        }
        else
            { cout << "No se pudo leer el archivo " << s << ".obj !"; return false; }
        archivo.close();
        return true;
    }
    bool leerVlf(string s)
    {
        ifstream archivo(s + ".vlf", ios::binary | ios::in );
        if (archivo.is_open())
        {
            //cout << "leeremos el vlf\n";
            int tamPuntos, tamLados, tamCaras;
            archivo.read((char *) &tamPuntos, sizeof(int));
            archivo.read((char *) &tamLados, sizeof(int));
            archivo.read((char *) &tamCaras, sizeof(int));
            //cout << "leimos bien los tamañosxd\n"<<tamPuntos<<"\n"<<tamCaras<<"\n"<<tamLados;
            /*
            vector<vector<double>> new_puntos(tamPuntos, vector<double>(3, 0));
            archivo.read((char *) &new_puntos, sizeof(new_puntos));
            puntos = new_puntos;
            cout << "leimos bien los puntos\n";

            vector<vector<double>> new_lados(tamLados, vector<double>(2, 0));
            archivo.read((char *) &new_lados, sizeof(new_lados));
            lados = new_lados;
            cout << "leimos bien los lados\n";

            vector<vector<int>> new_caras(tamCaras, vector<int>(3, 0));
            archivo.read((char *) &new_caras, sizeof(new_caras));
            caras = new_caras;
            cout << "leimos bien los caras\n";
            //*/
            vector<vector<double>> new_puntos(tamPuntos, vector<double>(3, 0));
            vector<vector<double>> new_lados(tamLados, vector<double>(2, 0));
            vector<vector<double>> new_caras(tamCaras, vector<double>(3, 0));
            for (int i = 0; i < tamPuntos; i++)
                for (int cnt = 0; cnt < 3; cnt++)
                    archivo.read((char *)&new_puntos[i][cnt], sizeof(double));
            for (int i = 0; i < tamLados; i++)
                for (int cnt = 0; cnt < 2; cnt++)
                    archivo.read((char *)&new_lados[i][cnt], sizeof(double));
            for (int i = 0; i < tamCaras; i++)
                for (int cnt = 0; cnt < 3; cnt++)
                    archivo.read((char *)&new_caras[i][cnt], sizeof(double));
            puntos = new_puntos;
            lados = new_lados;
            caras = new_caras;
            for (int i = 0; i < puntos.size(); i++)
                puntos[i].push_back(1);
            triangulado = true; vlf = true;
        }
        else
        {
            cout << "No se pudo leer el archivo " << s << ".vlf !";
            return false;
        }
        archivo.close();
        return true;
    }
    bool leerVlfNormal(string s)
    {
        ifstream archivo(s + ".vlf", ios::binary | ios::in);
        if (archivo.is_open())
        {
            cout << "leeremos el vlf\n";
            int tamPuntos, tamLados, tamCaras;
            archivo.read((char *)&tamPuntos, sizeof(int));
            archivo.read((char *)&tamLados, sizeof(int));
            archivo.read((char *)&tamCaras, sizeof(int));
            cout << "leimos bien los tamañosxd\n";
            cout << tamPuntos << "\n";
            cout << tamLados << "\n";
            cout << tamCaras;

            vector<vector<double>> new_puntos(tamPuntos, vector<double>(3, 0));
            vector<vector<double>> new_lados(tamLados, vector<double>(2, 0));
            vector<vector<double>> new_caras(tamCaras, vector<double>(3, 0));
            vector<vector<double>> new_normales(tamCaras, vector<double>(3, 0));
            for (int i = 0; i < tamPuntos; i++)
                for (int cnt = 0; cnt < 3; cnt++)
                    archivo.read((char *)&new_puntos[i][cnt], sizeof(double));
            for (int i = 0; i < tamLados; i++)
                for (int cnt = 0; cnt < 2; cnt++)
                    archivo.read((char *)&new_lados[i][cnt], sizeof(double));
            for (int i = 0; i < tamCaras; i++)
                for (int cnt = 0; cnt < 3; cnt++)
                    archivo.read((char *)&new_caras[i][cnt], sizeof(double));
            for (int i = 0; i < tamCaras; i++)
                for (int cnt = 0; cnt < 3; cnt++)
                    archivo.read((char *)&new_normales[i][cnt], sizeof(double));
            puntos = new_puntos;
            lados = new_lados;
            caras = new_caras;
            normales = new_normales;
            for (int i = 0; i < puntos.size(); i++)
                puntos[i].push_back(1);
            for (int i = 0; i < normales.size(); i++)
                normales[i].push_back(1);
            con_normales = true;
            triangulado = true;
            vlf = true;
        }
        else
        {
            cout << "No se pudo leer el archivo " << s << ".vlf !";
            return false;
        }
        archivo.close();
        return true;
    }
    bool leerVlfVertexNormal(string s)
    {
        ifstream archivo(s + ".vlf", ios::binary | ios::in);
        if (archivo.is_open())
        {
            int tamPuntos, tamLados, tamCaras;
            archivo.read((char *)&tamPuntos, sizeof(int));
            archivo.read((char *)&tamLados, sizeof(int));
            archivo.read((char *)&tamCaras, sizeof(int));

            vector<vector<double>> new_puntos(tamPuntos, vector<double>(3, 0));
            vector<vector<double>> new_lados(tamLados, vector<double>(2, 0));
            vector<vector<double>> new_caras(tamCaras, vector<double>(3, 0));
            vector<vector<double>> new_normales(tamCaras, vector<double>(3, 0));
            vector<vector<double>> new_vertexNormals(tamPuntos, vector<double>(3, 0));
            for (int i = 0; i < tamPuntos; i++)
                for (int cnt = 0; cnt < 3; cnt++)
                    archivo.read((char *)&new_puntos[i][cnt], sizeof(double));
            for (int i = 0; i < tamLados; i++)
                for (int cnt = 0; cnt < 2; cnt++)
                    archivo.read((char *)&new_lados[i][cnt], sizeof(double));
            for (int i = 0; i < tamCaras; i++)
                for (int cnt = 0; cnt < 3; cnt++)
                    archivo.read((char *)&new_caras[i][cnt], sizeof(double));
            for (int i = 0; i < tamCaras; i++)
                for (int cnt = 0; cnt < 3; cnt++)
                    archivo.read((char *)&new_normales[i][cnt], sizeof(double));
            for (int i = 0; i < tamPuntos; i++)
                for (int cnt = 0; cnt < 3; cnt++)
                    archivo.read((char *)&new_vertexNormals[i][cnt], sizeof(double));
            puntos = new_puntos;
            lados = new_lados;
            caras = new_caras;
            normales = new_normales;
            vertexNormals = new_vertexNormals;
            for (int i = 0; i < puntos.size(); i++)
                puntos[i].push_back(1);
            for (int i = 0; i < normales.size(); i++)
                normales[i].push_back(1);
            for (int i = 0; i < vertexNormals.size(); i++)
                vertexNormals[i].push_back(1);
            con_normales = true;
            triangulado = true;
            con_vertexNormals = true;
            vlf = true;
        }
        else
        {
            cout << "No se pudo leer el archivo " << s << ".vlf !";
            return false;
        }
        archivo.close();
        return true;
    }
    
    // Transformaciones
    void transformar(Transformacion trns)
    {
        vector<vector<double>> t = trns.getTabla();
        for (vector<double> &p : puntos)
        {
            vector<double> nuevop(4,0);
            for (int i = 0; i < 4; i++)
            {
                float valor = 0;
                for (int i2 = 0; i2 < 4; i2++)
                {
                    valor += t[i][i2] * p[i2];
                }
                //cout << "valor " << valor << "\n";
                nuevop[i] = valor;
            }
            p = nuevop;
        }
        if(con_normales)
        {
            for (vector<double> &p : normales)
            {
                vector<double> nuevop(4, 0);
                for (int i = 0; i < 4; i++)
                {
                    float valor = 0;
                    for (int i2 = 0; i2 < 4; i2++)
                    {
                        valor += t[i][i2] * p[i2];
                    }
                    nuevop[i] = valor;
                }
                p = nuevop;
            }
        }
        if(con_vertexNormals)
        {
            for (vector<double> &p : vertexNormals)
            {
                vector<double> nuevop(4, 0);
                for (int i = 0; i < 4; i++)
                {
                    float valor = 0;
                    for (int i2 = 0; i2 < 4; i2++)
                    {
                        valor += t[i][i2] * p[i2];
                    }
                    nuevop[i] = valor;
                }
                p = nuevop;
            }
        }
        //cout << "Transformacion completa! " << "\n";
    }
    void centrar(int rx, int ry)
    {
        float xmin = 0;
        float ymin = 0;
        float xmax = 0;
        float ymax = 0;

        for (vector<double> p : puntos)
        {
            if (p[0] < xmin)
                xmin = p[0];
            if (p[1] < ymin)
                ymin = p[1];

            if (p[0] > xmax)
                xmax = p[0];
            if (p[1] > ymax)
                ymax = p[1];
         }

        float dx = xmax - xmin;
        float dy = ymax - ymin;
        float factor = (rx<ry)? rx/dx : ry/dy;

        escalar(factor);
        ymin = 0;
        xmin = 0;
        float zmin = 0;
        for (vector<double> p : puntos)
        {
            if (p[0] < xmin)
                xmin = p[0];
            if (p[1] < ymin)
                ymin = p[1];
            if (p[2] < zmin)
                zmin = p[2];
        }

        mover( (xmin < 0)? -xmin : 0, 
               (ymin < 0)? -ymin : 0, 
               (zmin < 0)? -zmin : 0  );

    }
    void centrarOrigen()
    {
        int minx = (int) INFINITY, maxx = (int) -INFINITY, miny = (int) INFINITY, maxy = (int) -INFINITY, minz = (int) INFINITY, maxz = (int) -INFINITY;

        for (vector<double> p2 : puntos)
        {
            if (p2[0] < minx)
                minx = p2[0];
            if (p2[1] < miny)
                miny = p2[1];
            if (p2[2] < minz)
                minz = p2[2];

            if (p2[0] > maxx)
                maxx = p2[0];
            if (p2[1] > maxy)
                maxy = p2[1];
            if (p2[2] > maxz)
                maxz = p2[2];
        }

        int totx, toty, totz;

        totx = abs(maxx - minx);
        toty = abs(maxy - miny);
        totz = abs(maxz - minz);

        mover(-minx, -miny, -minz);
        mover(-totx/2,-toty/2,-totz/2);
    }
    void mover(float x, float y, float z)
    {
        Transformacion nueva(x,y,z);
        transformar(nueva);
    }
    void rotar(char a, float an)
    {
        Transformacion nueva(a,an);
        transformar(nueva);
    }
    void escalar(float esc)
    {
        Transformacion nueva(esc);
        transformar(nueva);
    }
    void regresarHomogeneo()
    {
        for (vector<double> &p : puntos)
            for (int i = 0; i < 3; i++)
                p[i] /= p[3];
        if(con_normales)
        {
            for (vector<double> &p : normales)
                for (int i = 0; i < 3; i++)
                    p[i] /= p[3];
        }
    }
    
    // Normales y triangulación
    void calcularNormales()
    {
        for(vector<double> c : caras)
        {
            vector<double> normal(4,1);        
            //calculamos normal
            double v1x, v1y, v1z, v2x, v2y, v2z;
            // Vector 1
            v1x = puntos[ lados[ c[0] - 1 ][1] - 1 ][0] - puntos[ lados[ c[0] - 1 ][0] - 1 ][0];
            v1y = puntos[ lados[ c[0] - 1 ][1] - 1 ][1] - puntos[ lados[ c[0] - 1 ][0] - 1 ][1];
            v1z = puntos[ lados[ c[0] - 1 ][1] - 1 ][2] - puntos[ lados[ c[0] - 1 ][0] - 1 ][2];
            // Vector 2
            v2x = puntos[ lados[ c[1] - 1 ][1] - 1 ][0] - puntos[ lados[ c[1] - 1 ][0] - 1 ][0];
            v2y = puntos[ lados[ c[1] - 1 ][1] - 1 ][1] - puntos[ lados[ c[1] - 1 ][0] - 1 ][1];
            v2z = puntos[ lados[ c[1] - 1 ][1] - 1 ][2] - puntos[ lados[ c[1] - 1 ][0] - 1 ][2];
            //Normal
            normal[0] = -1 * ((v1y * v2z) - (v1z * v2y));
            normal[1] = -1 * ((v1z * v2x) - (v1x * v2z));
            normal[2] = -1 * ((v1x * v2y) - (v1y * v2x));
            double mag = magnitud(normal);
            normal[0] /= mag;
            normal[1] /= mag;
            normal[2] /= mag;

            normal = sumarVector(normal, puntos[ lados[ c[0] - 1 ][0] - 1 ] );

            normales.push_back(normal);
        }
        con_normales = true;
    }
    void reCalcularNormales()
    {
        normales = vector<vector<double>>(0);
        calcularNormales();
    }
    void normalizarNormales()
    {
        for(int i = 0; i < normales.size(); i++)
        {
            normales[i] = restarVector(normales[i], puntos[ lados[ caras[i][0] - 1][0] - 1]);
            double mag = magnitud(normales[i]);
            normales[i][0] /= mag;
            normales[i][1] /= mag;
            normales[i][2] /= mag;
            normales[i] = sumarVector(normales[i], puntos[ lados[ caras[i][0] - 1][0] - 1]);
        }
    }
    void triangular()
    {
        for (int ci = 0; ci < caras.size(); ci++)
        {
            vector<double> caraAnalizada = caras[ci];
            if (caraAnalizada.size() > 3)
            {
                int cnt = 2;
                for (int i = caraAnalizada.size() - 3; i > 0; i--)
                {
                    vector<double> nueva_cara(3, 0);
                    nueva_cara[0] = caraAnalizada[0];
                    nueva_cara[1] = caraAnalizada[cnt];
                    nueva_cara[2] = caraAnalizada[cnt + 1];
                    cnt++;
                    caras.push_back(nueva_cara);
                }
            }
            caras[ci].resize(3);
        }
        triangulado = true;
    }
    void calcularVertexNormals()
    {
        vertexNormals.clear();
        vector<vector<double>> normalesOrigen = getNormales();
        for(int i=0; i < puntos.size(); i++)
        {
            vector<double> vn(3,0);
            vector<vector<double>> vadj;
            //Find all normals adjacent 
            for(int c=0; c < caras.size(); c++)
            {
                bool includes = false;
                for(int l=0; l<2; l++)
                {
                    if( (lados[caras[c][l] - 1][0] - 1) ==  i ) includes = true;
                    if( (lados[caras[c][l] - 1][1] - 1) ==  i ) includes = true;
                }
                if(includes) vadj.push_back(normalesOrigen[c]);
            }   
            //Promediamos 
            for(vector<double> v : vadj)
                vn = sumarVector(vn,v);
            for(int cnt=0; cnt<3; cnt++)
                vn[cnt] /= vadj.size();

            vn = sumarVector(puntos[i], vn);
            vertexNormals.push_back(vn);
        }
        con_vertexNormals = true;
    }

    // Conversiones a archivos y formatos
    bool convertirVLF(string s)
    {
        if(!vlf)
            convertirVLF();
        // Listo ahora solo es guardar puntos, lados, y caras en el vlf
        ofstream archivo(s + ".vlf", ios::binary | ios::out );
        if (archivo.is_open())
        {
            int tam = puntos.size();
            archivo.write( (char *) &tam, sizeof(int) );
            tam = lados.size();
            archivo.write((char *)&tam, sizeof(int));
            tam = caras.size();
            archivo.write((char *)&tam, sizeof(int));
            //cout << "Escribimos bien " << puntos.size() << "\n" << lados.size() << "\n" << caras.size();
            //archivo.write((char *) &puntos, sizeof(puntos));
            //archivo.write((char *) &lados, sizeof(lados));
            //archivo.write((char *) &caras, sizeof(caras));
            //*
            for (int i = 0; i < puntos.size(); i++)
                for (int cnt = 0; cnt < 3; cnt++)
                    archivo.write((char *) &puntos[i][cnt], sizeof(double));
            for (int i = 0; i < lados.size(); i++)
                for (int cnt = 0; cnt < 2; cnt++)
                    archivo.write((char *) &lados[i][cnt], sizeof(double));
            for (int i = 0; i < caras.size(); i++)
                for (int cnt = 0; cnt < 3; cnt++)
                    archivo.write((char *)&caras[i][cnt], sizeof(double));
            //*/
        }
        else
        {
            cout << "No se pudo leer el archivo " << s << ".obj !";
            return false;
        }
        archivo.close();
        return true;
    }
    bool convertirVLFNormal(string s)
    {
        if(!con_normales) convertirVLFNormal();
        // Listo ahora solo es guardar puntos, lados, y caras en el vlf
        ofstream archivo(s + ".vlf", ios::binary | ios::out);
        if (archivo.is_open())
        {
            int tam = puntos.size();
            archivo.write((char *)&tam, sizeof(int));
            tam = lados.size();
            archivo.write((char *)&tam, sizeof(int));
            tam = caras.size();
            archivo.write((char *)&tam, sizeof(int));

            for (int i = 0; i < puntos.size(); i++)
                for (int cnt = 0; cnt < 3; cnt++)
                    archivo.write((char *)&puntos[i][cnt], sizeof(double));
            for (int i = 0; i < lados.size(); i++)
                for (int cnt = 0; cnt < 2; cnt++)
                    archivo.write((char *)&lados[i][cnt], sizeof(double));
            for (int i = 0; i < caras.size(); i++)
                for (int cnt = 0; cnt < 3; cnt++)
                    archivo.write((char *)&caras[i][cnt], sizeof(double));
            for (int i = 0; i < normales.size(); i++)
                for (int cnt = 0; cnt < 3; cnt++)
                    archivo.write((char *)&normales[i][cnt], sizeof(double));
        }
        else
        {
            cout << "No se pudo leer el archivo " << s << ".obj !";
            return false;
        }
        archivo.close();
        return true;
        
    }   
    bool convertirVLFVertexNormal(string s)
    {
        if(!con_vertexNormals) convertirVLFVertexNormal();
        // Listo ahora solo es guardar puntos, lados, y caras en el vlf
        ofstream archivo(s + ".vlf", ios::binary | ios::out);
        if (archivo.is_open())
        {
            int tam = puntos.size();
            archivo.write((char *)&tam, sizeof(int));
            tam = lados.size();
            archivo.write((char *)&tam, sizeof(int));
            tam = caras.size();
            archivo.write((char *)&tam, sizeof(int));

            for (int i = 0; i < puntos.size(); i++)
                for (int cnt = 0; cnt < 3; cnt++)
                    archivo.write((char *)&puntos[i][cnt], sizeof(double));
            for (int i = 0; i < lados.size(); i++)
                for (int cnt = 0; cnt < 2; cnt++)
                    archivo.write((char *)&lados[i][cnt], sizeof(double));
            for (int i = 0; i < caras.size(); i++)
                for (int cnt = 0; cnt < 3; cnt++)
                    archivo.write((char *)&caras[i][cnt], sizeof(double));
            for (int i = 0; i < normales.size(); i++)
                for (int cnt = 0; cnt < 3; cnt++)
                    archivo.write((char *)&normales[i][cnt], sizeof(double));
            for (int i = 0; i < puntos.size(); i++)
                for (int cnt = 0; cnt < 3; cnt++)
                    archivo.write((char *)&vertexNormals[i][cnt], sizeof(double));
        }
        else
        {
            cout << "No se pudo leer el archivo " << s << ".obj !";
            return false;
        }
        archivo.close();
        return true;
    }
    void convertirVLF()
    {
        if (!triangulado) triangular();
        //Vamos a crear la lista de edges.
        if (!vlf)
        {
            int superf=0;
            vector<vector<double>> nuevasCaras;
            for (int ci = 0; ci < caras.size(); ci++)
            {
                int carastam = caras.size();
                vector<double> face = caras[ci];
                vector<double> newFace;
                for (int cnt = 0; cnt < 3; cnt++)
                {
                    vector<double> edge(2, 0);
                    edge[0] = face[cnt];
                    edge[1] = face[(cnt + 1) % 3];
                    //*
                    for (int i = 0; i < lados.size(); i++)
                    {
                        if (edge == lados[i])
                        {
                            superf++;
                            double aux = edge[0];
                            edge[0] = edge[1];
                            edge[1] = aux;
                            break;
                        }
                    }
                    //*/
                    lados.push_back(edge);
                    newFace.push_back(lados.size());
                }
                nuevasCaras.push_back(newFace);
            }
            caras = nuevasCaras;
            vlf = true;
        }
    }
    void convertirVLFNormal()
    {
        convertirVLF();
        if (!con_normales)
            calcularNormales();
    }
    void convertirVLFVertexNormal()
    {
        convertirVLFNormal();
        if(!con_vertexNormals)
            calcularVertexNormals();
    }
    
    // Bloque get
    vector<vector<double>> getPuntos() { return puntos; }
    vector<vector<double>> getLados() { return lados; }
    vector<vector<double>> getCaras() { return caras; }
    vector<vector<double>> getNormales() 
    {
        vector<vector<double>> normalesRegresar; // Restamos el vector para obtener normal en el origen
        for(int i=0; i < normales.size(); i++)
            normalesRegresar.push_back( restarVector(normales[i], puntos[lados[caras[i][0] - 1][0] - 1]) );
        return normalesRegresar; 
    }
    vector<int> getRGB() { return rgb; }
    vector<Material> getMateriales() { return faceMaterial; }
    vector<vector<double>> getVertexNormals() //{ return vertexNormals; }
    {
        vector<vector<double>> normalesRegresar; // Restamos el vector para obtener normal en el origen
        for (int i = 0; i < puntos.size(); i++)
            normalesRegresar.push_back(restarVector(vertexNormals[i], puntos[i]));
        return normalesRegresar;
    }
};

class Patch3D
{
    vector<vector<vector<double>>> puntosControl;
    vector<vector<double>> tablaHermite, tablaHermiteT;
    vector<vector<vector<double>>> puntos;
    bool puntos_calculados;
    double res_s, res_t; //Resolucion
    public: 
    Patch3D()
    {
        res_s = 100; res_t = 100;
        puntos_calculados = false;
        // Armamos tabla de constantes de Hermite
        tablaHermite = vector<vector<double>>(4,vector<double>(4,0));
        tablaHermite[0][0] = 2;
        tablaHermite[0][1] = -2;
        tablaHermite[0][2] = 1;
        tablaHermite[0][3] = 1;
        tablaHermite[1][0] = -3;
        tablaHermite[1][1] = 3;
        tablaHermite[1][2] = -2;
        tablaHermite[1][3] = -1;
        tablaHermite[2][2] = 1;
        tablaHermite[3][0] = 1;
        // Armamos la transposed 
        tablaHermiteT = vector<vector<double>>(4,vector<double>(4,0));
        for (int i = 0; i < 4; i++)
            for (int i2 = 0; i2 < 4; i2++)
                tablaHermiteT[i][i2] = tablaHermite[i2][i];
    }
    void setPuntosControl(vector<vector<double>> c1, vector<vector<double>> c2, vector<vector<double>> c3, vector<vector<double>> c4)
    {
        puntosControl = vector<vector<vector<double>>>(4, vector<vector<double>>(4,  vector<double>(3, 0)));
        for (int i = 0; i < 4; i++)
        {
            for (int i2 = 0; i2 < 4; i2++)
            {
                for (int xyz = 0; xyz < 3; xyz++)
                {
                    vector<vector<double>> curva;
                    curva = (i==0)? c1 : ((i==1)? c2 : ((i==2)? c3 : ((i==3)? c4 : vector<vector<double>>(4,vector<double>(3,0)))));
                    puntosControl[i][i2][xyz] = curva[i2][xyz];
                }   
            }
        }
    }
    void setResolucion(int s, int t) { res_s = s; res_t = t; }
    void leerArchivo(string s)
    {
        
    }
    void calcularPuntos()
    {
        puntos = vector<vector<vector<double>>>(res_s+1, vector<vector<double>>(res_t+1, vector<double>(3,0)));
        float step_s = 1 / res_s;
        float step_t = 1 / res_t;
        float acum_s = 0;
        float acum_t = 0;
        for (int s = 0; s <= res_s; s++)
        {
            // S
            vector<double> vs = vector<double>(4,1);
            vs[0] = pow(acum_s, 3);
            vs[1] = pow(acum_s, 2);
            vs[2] = acum_s;
            // S * H
            vector<double> vsh = vector<double>(4,1);
            for (int i = 0; i < 4; i++)
                vsh[i] = vs[0] * tablaHermite[0][i] + vs[1] * tablaHermite[1][i] + vs[2] * tablaHermite[2][i] + vs[3] * tablaHermite[3][i];
            acum_t = 0;
            // Entramos para T
            for (int t = 0; t <= res_t; t++)
            {
                // T
                vector<double> vt = vector<double>(4,1);
                vt[0] = pow(acum_t, 3);
                vt[1] = pow(acum_t, 2);
                vt[2] = acum_t;
                // Ht * T
                vector<double> vht = vector<double>(4,1);
                for (int i = 0; i < 4; i++)
                    vht[i] = vt[0] * tablaHermiteT[i][0] + vt[1] * tablaHermiteT[i][1] + vt[2] * tablaHermiteT[i][2] + vt[3] * tablaHermiteT[i][3];
                // para x y z
                for (int xyz = 0; xyz < 3; xyz++)
                {
                    double valorFinal = 0;
                    // SH * G
                    vector<double> vshg = vector<double>(4,1);
                    for (int i = 0; i < 4; i++)
                        vshg[i] = vsh[0] * puntosControl[0][i][xyz] + vsh[1] * puntosControl[1][i][xyz] + vsh[2] * puntosControl[2][i][xyz] + vsh[3] * puntosControl[3][i][xyz];
                    // SHG * HtT
                    for (int i = 0; i < 4; i++)
                        valorFinal += vshg[i] * vht[i];
                    // Asignamos
                    puntos[s][t][xyz] = valorFinal;
                }
                acum_t += step_t;
            }
            acum_s += step_s;
        }
        puntos_calculados = true;
    }
    Object3D convertirAObjecto3D(string s)
    {
        Object3D nuevo = convertirAObjecto3D();
        nuevo.convertirVLFVertexNormal(s);
        return nuevo;
    }
    Object3D convertirAObjecto3D()
    {
        if(!puntos_calculados) calcularPuntos();
        vector<vector<double>> p, c;
        vector<vector<double>> ids = vector<vector<double>>(res_s+1,vector<double>(res_t+1,0));
        double cnt = 1;
        for(int s=0; s<=res_s; s++)
        {
            for(int t=0; t<=res_t; t++)
            {    
                ids[s][t] = cnt;
                p.push_back(puntos[s][t]);
                cnt++;
            }
        }
        for (int s = 0; s < res_s; s++)
            for (int t = 0; t < res_t; t++)
                c.push_back({ids[s][t],ids[s][t+1],ids[s+1][t+1],ids[s+1][t]});
        Object3D nuevo;
        nuevo.setHomemadeObjecto3D(p,c);
        return nuevo;
    }
    Object3D convertirAObjecto3D_VlfVN()
    {
        Object3D nuevo = convertirAObjecto3D();
        nuevo.convertirVLFVertexNormal();
        return nuevo;
    }
};

class Raster
{
    // Pixel variables
    vector<vector<vector<unsigned char>>> image;
    vector<vector<vector<double>>> pixelCoordinate;
    vector<vector<double>> z_buffer;
    vector<vector<Material>> pixelMat;
    vector<vector<vector<double>>> pixelNormal;
    int rx, ry;
    unsigned char r, g , b;
    unsigned char backgroundColor = 0xFA;
    // Camera variables
    bool camaraOrtogonal = true;
    char camaraEje = 'z';
    float f = 5;
    // Lightning variables
    vector<int> ambientLight = vector<int>(3,0);
    
    public:
    // Constructor
    Raster(int _rx, int _ry)
    {
        rx=_rx; ry=_ry;
        image.resize(rx, vector<vector<unsigned char>>(ry, vector<unsigned char> (3,backgroundColor) ));
        pixelCoordinate.resize(rx, vector<vector<double>>(ry, vector<double>(2, (double) INFINITY)));
        pixelNormal.resize(rx, vector<vector<double>>(ry, vector<double>(2, 0)));
        pixelMat.resize(rx, vector<Material>(ry, Material()));
        z_buffer.resize(rx, vector<double>(ry,  (double) INFINITY ));
    }
    
    // Raster configuration
    void setColor(int _r, int _g, int _b)
    {   r = _r;     g = _g;     b = _b;  }
    void setBackgroundColor(unsigned char bc) { backgroundColor = bc; } 
    void setCamara(char eje, bool ortogonal, float focal) 
    { camaraEje = eje; camaraOrtogonal = ortogonal; f = focal; }
    void setAmbientLight(vector<int> rgb) { ambientLight = rgb; }
    
    // Pixel related functions 
    void clear()
    {
        for (int i = 0; i < image.size(); i++)
            for (int i2 = 0; i2 < image[i].size(); i2++)
            { 
                image[i][i2][0] = backgroundColor;
                image[i][i2][1] = backgroundColor;
                image[i][i2][2] = backgroundColor;
                z_buffer[i][i2] = (double) INFINITY;
                pixelCoordinate[i][i2][0] = (double) INFINITY;
                pixelCoordinate[i][i2][1] = (double) INFINITY;
            }
    }
    void pixel(int x, int y){  pixel(x,y,{r,g,b}); }
    void pixel(int x, int y, vector<int> color)
    {  
        if(x>=rx||y>=ry||y<0||x<0) return;
        image[x][y][0] = color[0];
        image[x][y][1] = color[1];
        image[x][y][2] = color[2];
    }
    void pixel(int x, int y, double z) {  pixel(x,y,z,{r,g,b}); }
    void pixel(int x, int y, double z, vector<int> color) //considera al z_buffer
    {  
        if(x>=rx||y>=ry||y<0||x<0) return;
        if(z < z_buffer[x][y])
        {
            image[x][y][0] = (color[0]>255)? 255 : ((color[0]<0)? 0 : color[0]);
            image[x][y][1] = (color[1]>255)? 255 : ((color[1]<0)? 0 : color[1]);
            image[x][y][2] = (color[2]>255)? 255 : ((color[2]<0)? 0 : color[2]);
            z_buffer[x][y] = z;
        }
    }
    void pixel(int ix, int iy, int ox, int oy, double z)
    {
        pixel(ix,iy,ox,oy,z,{r,g,b});
    }
    void pixel(int ix, int iy, int ox, int oy, double z, vector<int> color) //considera al z_buffer y agrega x,y originales
    {  
        if(ix>=rx||iy>=ry||iy<0||ix<0) return;
        if(z < z_buffer[ix][iy])
        {
            image[ix][iy][0] = color[0];
            image[ix][iy][1] = color[1];
            image[ix][iy][2] = color[2];
            z_buffer[ix][iy] = z;
            // Actualiza valor x, y original
            pixelCoordinate[ix][iy][0] = ox;
            pixelCoordinate[ix][iy][1] = oy;
        }
    }
    void pixelMaterial(int ix, int iy, int ox, int oy, double z, vector<int> color, Material mt, vector<double> n) // Materiales baby
    {
        if (ix >= rx || iy >= ry || iy < 0 || ix < 0)
            return;
        if (z < z_buffer[ix][iy])
        {
            image[ix][iy][0] = color[0];
            image[ix][iy][1] = color[1];
            image[ix][iy][2] = color[2];
            z_buffer[ix][iy] = z;
            // Actualiza valor x, y original
            pixelCoordinate[ix][iy][0] = ox;
            pixelCoordinate[ix][iy][1] = oy;
            // Material y normal del pixel
            pixelMat[ix][iy] = mt;
            pixelNormal[ix][iy] = n;
        }
    }
    
    // Line Drawing
    void drawLineNaive(int x1, int y1, int x2, int y2)
    {
        pixel(x1, y1);
        if (x1 == x2 && y1 == x2) return;
        pixel(x2, y2);

        float m, dx, dy;
        dy = y2 - y1;
        dx = x2 - x1;

        if (abs(dx) >= abs(dy))
        {
            if (x1 > x2) //Swap
            {
                swap(y1, y2);
                swap(x1, x2);
                dy = y2 - y1;
                dx = x2 - x1;
            }
            m = dy / dx;
            float b = y1 - m*x1;
            for (float x = x1 + 1; x < x2; x++)
            {
                float y;
                y = round(x * m + b);
                pixel(x, y);
            }
        }
        else
        {
            if (y1 > y2) //Swap
            {
                swap(y1, y2);
                swap(x1, x2);
                dy = y2 - y1;
                dx = x2 - x1;
            }
            m = dy / dx;
            float b = y1 - m*x1;
            for (float y = y1 + 1; y < y2; y++)
            {
                float x;
                x = round((y-b) / m);
                pixel(x, y);
            }
        }
    }
    void drawLineDDA(int x1, int y1, int x2, int y2)
    {
        pixel(x1, y1);
        if (x1 == x2 && y1 == x2) return;
        pixel(x2, y2);

        float dx, dy;

        dy = y2 - y1;
        dx = x2 - x1;

        if (abs(dx) >= abs(dy))
        {
            if (x1 > x2) //Swap
            {
                swap(y1, y2);
                swap(x1, x2);
                dy = y2 - y1;
                dx = x2 - x1;
            }
            float m = dy / dx;
            float y = y1;
            for (float x = x1 + 1; x < x2; x++)
            {
                y = y + m;
                pixel(x, round(y));
            }
        }
        else
        {
            if (y1 > y2) //Swap
            {
                swap(y1, y2);
                swap(x1, x2);
                dy = y2 - y1;
                dx = x2 - x1;
            }
            float divm = dx / dy;
            float x = x1;
            for (float y = y1 + 1; y < y2; y++)
            {
                x = divm + x;
                pixel(round(x), y);
            }
        }
    }   
    void drawLineBresenham(int x1, int y1, int x2, int y2)
    {

        pixel(x1, y1);
        if(x1==x2 && y1==x2) return;
        pixel(x2, y2);

        int dx, dy;
        dy = y2 - y1;
        dx = x2 - x1;

        if (abs(dx) >= abs(dy))
        {
            if (x1 > x2) //Swap
            {
                swap(y1, y2);
                swap(x1, x2);
                dy = y2 - y1;
                dx = x2 - x1;
            }

            int valor1 = 2 * abs(dy);
            int valor2 = valor1 - (2 * dx);

            int d = valor1 - dx;

            int y = y1;
            int add = (dy < 0) ? -1 : 1;
            for (int x = x1 + 1; x < x2; x++)
            {
                if(d < 0)
                    d += valor1;
                else
                {
                    d += valor2;
                    y += add;
                }
                pixel(x, y);
            }
        }
        else
        {
            if (y1 > y2) //Swap
            {
                swap(y1, y2);
                swap(x1, x2);
                dy = y2 - y1;
                dx = x2 - x1;
            }
            int valor1 = 2 * abs(dx);
            int valor2 = valor1 - (2 * dy);

            int d = valor1 - dy;

            int x = x1;
            int add = (dx < 0) ? -1 : 1;
            for (int y = y1 + 1; y < y2; y++)
            {
                if (d < 0)
                    d += valor1;
                else
                {
                    d += valor2;
                    x += add;
                }
                pixel(x, y);
            }
        }
    }
    void drawLineBresenhamThread(int num, int x1, int y1, int x2, int y2)
    {
        int dx, dy;
        dy = y2 - y1;
        dx = x2 - x1;
        vector<int> xs;
        vector<int> ys;
        xs.push_back(x1);
        ys.push_back(y1);
        for (int i = 1; i < num; i++)
        {
            xs.push_back(x1 + round(dx * i / num));
            ys.push_back(y1 + round(dy * i / num));
        }
        xs.push_back(x2);
        ys.push_back(y2);
        vector<thread> ts;
        for (int i = 0; i < num; i++)
        {
            thread t(&Raster::drawLineBresenham, this, xs[i], ys[i], xs[i + 1], ys[i + 1]);
            ts.push_back(move(t));
        }
        for (int i = 0; i < num; i++)
        {
            ts[i].join();
        }
    }
    vector<vector<int>> drawLineBresenhamPoints(int x1, int y1, int x2, int y2)
    {
        vector<vector<int>> toReturn;
        vector<int> toInsert(2,0);
        if (x1 == x2 && y1 == x2)
        {
            toInsert[0] = x1;
            toInsert[1] = y1;
            toReturn.push_back(toInsert);
            return toReturn;
        }
            
        int dx, dy;
        dy = y2 - y1;
        dx = x2 - x1;

        if (abs(dx) >= abs(dy))
        {
            if (x1 > x2) //Swap
            {
                swap(y1, y2);
                swap(x1, x2);
                dy = y2 - y1;
                dx = x2 - x1;
            }

            toInsert[0] = x1;
            toInsert[1] = y1;
            toReturn.push_back(toInsert);

            int valor1 = 2 * abs(dy);
            int valor2 = valor1 - (2 * dx);

            int d = valor1 - dx;

            int y = y1;
            int add = (dy < 0) ? -1 : 1;
            for (int x = x1 + 1; x < x2; x++)
            {
                if (d < 0)
                    d += valor1;
                else
                {
                    d += valor2;
                    y += add;
                }
                toInsert[0] = x;
                toInsert[1] = y;
                toReturn.push_back(toInsert);
            }
        }
        else
        {
            if (y1 > y2) //Swap
            {
                swap(y1, y2);
                swap(x1, x2);
                dy = y2 - y1;
                dx = x2 - x1;
            }

            toInsert[0] = x1;
            toInsert[1] = y1;
            toReturn.push_back(toInsert);

            int valor1 = 2 * abs(dx);
            int valor2 = valor1 - (2 * dy);

            int d = valor1 - dy;

            int x = x1;
            int add = (dx < 0) ? -1 : 1;
            for (int y = y1 + 1; y < y2; y++)
            {
                if (d < 0)
                    d += valor1;
                else
                {
                    d += valor2;
                    x += add;
                }
                toInsert[0] = x;
                toInsert[1] = y;
                toReturn.push_back(toInsert);
            }
        }

        toInsert[0] = x2;
        toInsert[1] = y2;
        toReturn.push_back(toInsert);
        return toReturn;
    }
    vector<vector<double>> drawLineBresenhamPointsZ(int x1, int y1, double z1, int x2, int y2, double z2)
    {
        vector<vector<double>> toReturn;
        vector<double> toInsert(3, 0);
        toInsert[0] = x1;
        toInsert[1] = y1;
        toInsert[2] = z1;
        toReturn.push_back(toInsert);
        if (x1 == x2 && y1 == x2)
            return toReturn;
        toInsert[0] = x2;
        toInsert[1] = y2;
        toInsert[2] = z2;
        toReturn.push_back(toInsert);

        int dx, dy;
        dy = y2 - y1;
        dx = x2 - x1;

        if (abs(dx) >= abs(dy))
        {
            if (x1 > x2) //Swap
            {
                swap(y1, y2);
                swap(x1, x2);
                swap(z1, z2);
                dy = y2 - y1;
                dx = x2 - x1;
            }
            int valor1 = 2 * abs(dy);
            int valor2 = valor1 - (2 * dx);

            int d = valor1 - dx;

            int y = y1;
            int add = (dy < 0) ? -1 : 1;

            double zstep = (z2 - z1) / abs(y2-y1);
            double zacum = z1 + zstep;
            for (int x = x1 + 1; x < x2; x++)
            {
                if (d < 0)
                    d += valor1;
                else
                {
                    d += valor2;
                    y += add;
                }
                toInsert[0] = x;
                toInsert[1] = y;
                toInsert[2] = zacum;
                toReturn.push_back(toInsert);
                zacum += zstep;
            }
        }
        else
        {
            if (y1 > y2) //Swap
            {
                swap(y1, y2);
                swap(x1, x2);
                swap(z1, z2);
                dy = y2 - y1;
                dx = x2 - x1;
            }

            int valor1 = 2 * abs(dx);
            int valor2 = valor1 - (2 * dy);

            int d = valor1 - dy;

            int x = x1;
            int add = (dx < 0) ? -1 : 1;

            double zstep = (z2 - z1) / abs(y2-y1);
            double zacum = z1 + zstep;
            for (int y = y1 + 1; y < y2; y++)
            {
                if (d < 0)
                    d += valor1;
                else
                {
                    d += valor2;
                    x += add;
                }
                toInsert[0] = x;
                toInsert[1] = y;
                toInsert[2] = zacum;
                toReturn.push_back(toInsert);
                zacum += zstep;
            }
        }
        return toReturn;
    }
    //Interpolacion de los x , y originales
    vector<vector<double>> drawLineBresenhamPointsZ(int x1, int y1, double ox1, double oy1, double z1, int x2, int y2, double ox2, double oy2, double z2)
    {
        vector<vector<double>> toReturn;
        vector<double> toInsert(5, 0);
        toInsert[0] = x1;
        toInsert[1] = y1;
        toInsert[2] = z1;
        toInsert[3] = ox1;
        toInsert[4] = oy1;
        toReturn.push_back(toInsert);
        if (x1 == x2 && y1 == x2)
            return toReturn;
        toInsert[0] = x2;
        toInsert[1] = y2;
        toInsert[2] = z2;
        toInsert[3] = ox2;
        toInsert[4] = oy2;
        toReturn.push_back(toInsert);

        int dx, dy;
        dy = y2 - y1;
        dx = x2 - x1;

        if (abs(dx) >= abs(dy))
        {
            if (x1 > x2) //Swap
            {
                swap(y1, y2);
                swap(x1, x2);
                swap(z1, z2);
                swap(ox1, ox2);
                swap(oy1, oy2);
                dy = y2 - y1;
                dx = x2 - x1;
            }
            int valor1 = 2 * abs(dy);
            int valor2 = valor1 - (2 * dx);

            int d = valor1 - dx;

            int y = y1;
            int add = (dy < 0) ? -1 : 1;

            double zstep = (z2 - z1) / abs(x2-x1);
            double zacum = z1 + zstep;

            double xstep = (ox2 - ox1) / abs(x2-x1);
            double xacum = ox1 + xstep;

            double ystep = (oy2 - oy1) / abs(x2-x1);
            double yacum = oy1 + ystep;

            for (int x = x1 + 1; x < x2; x++)
            {
                if (d < 0)
                    d += valor1;
                else
                {
                    d += valor2;
                    y += add;
                }
                toInsert[0] = x;
                toInsert[1] = y;
                toInsert[2] = zacum;
                toInsert[3] = xacum;
                toInsert[4] = yacum;
                toReturn.push_back(toInsert);
                zacum += zstep;
                xacum += xstep;
                yacum += ystep;
            }
        }
        else
        {
            if (y1 > y2) //Swap
            {
                swap(y1, y2);
                swap(x1, x2);
                swap(z1, z2);
                swap(ox1, ox2);
                swap(oy1, oy2);
                dy = y2 - y1;
                dx = x2 - x1;
            }

            int valor1 = 2 * abs(dx);
            int valor2 = valor1 - (2 * dy);

            int d = valor1 - dy;

            int x = x1;
            int add = (dx < 0) ? -1 : 1;

            double zstep = (z2 - z1) / abs(y2-y1);
            double zacum = z1 + zstep;

            double xstep = (ox2 - ox1) / abs(y2-y1);
            double xacum = ox1 + xstep;

            double ystep = (oy2 - oy1) / abs(y2-y1);
            double yacum = oy1 + ystep;

            for (int y = y1 + 1; y < y2; y++)
            {
                if (d < 0)
                    d += valor1;
                else
                {
                    d += valor2;
                    x += add;
                }
                toInsert[0] = x;
                toInsert[1] = y;
                toInsert[2] = zacum;
                toInsert[3] = xacum;
                toInsert[4] = yacum;
                toReturn.push_back(toInsert);
                zacum += zstep;
                xacum += xstep;
                yacum += ystep;
            }
        }
        return toReturn;
    }
    //Interpolando normales tambien
    vector<vector<double>> drawLineBresenhamPointsZVNormals(int x1, int y1, double ox1, double oy1, double z1, int x2, int y2, double ox2, double oy2, double z2, vector<double> n1, vector<double> n2)
    {
        vector<vector<double>> toReturn;
        vector<double> toInsert(8, 0);
        toInsert[0] = x1;
        toInsert[1] = y1;
        toInsert[2] = z1;
        toInsert[3] = ox1;
        toInsert[4] = oy1;
        toInsert[5] = n1[0];
        toInsert[6] = n1[1];
        toInsert[7] = n1[2];
        toReturn.push_back(toInsert);
        if (x1 == x2 && y1 == x2)
            return toReturn;
        toInsert[0] = x2;
        toInsert[1] = y2;
        toInsert[2] = z2;
        toInsert[3] = ox2;
        toInsert[4] = oy2;
        toInsert[5] = n2[0];
        toInsert[6] = n2[1];
        toInsert[7] = n2[2];
        toReturn.push_back(toInsert);

        int dx, dy;
        dy = y2 - y1;
        dx = x2 - x1;

        if (abs(dx) >= abs(dy))
        {
            if (x1 > x2) //Swap
            {
                swap(y1, y2);
                swap(x1, x2);
                swap(z1, z2);
                swap(ox1, ox2);
                swap(oy1, oy2);
                swap(n1, n2);
                dy = y2 - y1;
                dx = x2 - x1;
            }
            int valor1 = 2 * abs(dy);
            int valor2 = valor1 - (2 * dx);

            int d = valor1 - dx;

            int y = y1;
            int add = (dy < 0) ? -1 : 1;

            double zstep = (z2 - z1) / abs(x2-x1);
            double zacum = z1 + zstep;

            double xstep = (ox2 - ox1) / abs(x2-x1);
            double xacum = ox1 + xstep;

            double ystep = (oy2 - oy1) / abs(x2-x1);
            double yacum = oy1 + ystep;

            double xnstep = (n2[0] - n1[0]) / abs(x2 - x1);
            double xnacum = n1[0] + xnstep;
            double ynstep = (n2[1] - n1[1]) / abs(x2 - x1);
            double ynacum = n1[1] + ynstep;
            double znstep = (n2[2] - n1[2]) / abs(x2 - x1);
            double znacum = n1[2] + znstep;

            
            for (int x = x1 + 1; x < x2; x++)
            {
                if (d < 0)
                    d += valor1;
                else
                {
                    d += valor2;
                    y += add;
                }
                toInsert[0] = x;
                toInsert[1] = y;
                toInsert[2] = zacum;
                toInsert[3] = xacum;
                toInsert[4] = yacum;
                toInsert[5] = xnacum;
                toInsert[6] = ynacum;
                toInsert[7] = znacum;
                toReturn.push_back(toInsert);
                zacum += zstep;
                xacum += xstep;
                yacum += ystep;
                xnacum += xnstep;
                ynacum += ynstep;
                znacum += znstep;
            }
        }
        else
        {
            if (y1 > y2) //Swap
            {
                swap(y1, y2);
                swap(x1, x2);
                swap(z1, z2);
                swap(ox1, ox2);
                swap(oy1, oy2);
                swap(n1, n2);
                dy = y2 - y1;
                dx = x2 - x1;
            }

            int valor1 = 2 * abs(dx);
            int valor2 = valor1 - (2 * dy);

            int d = valor1 - dy;

            int x = x1;
            int add = (dx < 0) ? -1 : 1;

            double zstep = (z2 - z1) / abs(y2-y1);
            double zacum = z1 + zstep;

            double xstep = (ox2 - ox1) / abs(y2-y1);
            double xacum = ox1 + xstep;

            double ystep = (oy2 - oy1) / abs(y2-y1);
            double yacum = oy1 + ystep;

            double xnstep = (n2[0] - n1[0]) / abs(y2 - y1);
            double xnacum = n1[0] + xnstep;
            double ynstep = (n2[1] - n1[1]) / abs(y2 - y1);
            double ynacum = n1[1] + ynstep;
            double znstep = (n2[2] - n1[2]) / abs(y2 - y1);
            double znacum = n1[2] + znstep;

            for (int y = y1 + 1; y < y2; y++)
            {
                if (d < 0)
                    d += valor1;
                else
                {
                    d += valor2;
                    x += add;
                }
                toInsert[0] = x;
                toInsert[1] = y;
                toInsert[2] = zacum;
                toInsert[3] = xacum;
                toInsert[4] = yacum;
                toInsert[5] = xnacum;
                toInsert[6] = ynacum;
                toInsert[7] = znacum;
                toReturn.push_back(toInsert);
                zacum += zstep;
                xacum += xstep;
                yacum += ystep;
                xnacum += xnstep;
                ynacum += ynstep;
                znacum += znstep;
            }
        }
        return toReturn;
    }
    //Interpolando colores .w.
    vector<vector<double>> drawLineBresenhamPointsGouraud(int x1, int y1, double z1, int x2, int y2, double z2, vector<double> n1, vector<double> n2)
    {
        vector<vector<double>> toReturn;
        vector<double> toInsert(6, 0);
        toInsert[0] = x1;
        toInsert[1] = y1;
        toInsert[2] = z1;
        toInsert[3] = n1[0];
        toInsert[4] = n1[1];
        toInsert[5] = n1[2];
        toReturn.push_back(toInsert);
        if (x1 == x2 && y1 == x2)
            return toReturn;
        toInsert[0] = x2;
        toInsert[1] = y2;
        toInsert[2] = z2;
        toInsert[3] = n2[0];
        toInsert[4] = n2[1];
        toInsert[5] = n2[2];
        toReturn.push_back(toInsert);

        int dx, dy;
        dy = y2 - y1;
        dx = x2 - x1;

        if (abs(dx) >= abs(dy))
        {
            if (x1 > x2) //Swap
            {
                swap(y1, y2);
                swap(x1, x2);
                swap(z1, z2);
                swap(n1, n2);
                dy = y2 - y1;
                dx = x2 - x1;
            }
            int valor1 = 2 * abs(dy);
            int valor2 = valor1 - (2 * dx);

            int d = valor1 - dx;

            int y = y1;
            int add = (dy < 0) ? -1 : 1;

            double zstep = (z2 - z1) / abs(x2 - x1);
            double zacum = z1 + zstep;

            double xnstep = (n2[0] - n1[0]) / abs(x2 - x1);
            double xnacum = n1[0] + xnstep; //R
            double ynstep = (n2[1] - n1[1]) / abs(x2 - x1);
            double ynacum = n1[1] + ynstep; //G
            double znstep = (n2[2] - n1[2]) / abs(x2 - x1);
            double znacum = n1[2] + znstep; //B

            for (int x = x1 + 1; x < x2; x++)
            {
                if (d < 0)
                    d += valor1;
                else
                {
                    d += valor2;
                    y += add;
                }
                toInsert[0] = x;
                toInsert[1] = y;
                toInsert[2] = zacum;
                toInsert[3] = xnacum;
                toInsert[4] = ynacum;
                toInsert[5] = znacum;
                toReturn.push_back(toInsert);
                zacum += zstep;
                xnacum += xnstep;
                ynacum += ynstep;
                znacum += znstep;
            }
        }
        else
        {
            if (y1 > y2) //Swap
            {
                swap(y1, y2);
                swap(x1, x2);
                swap(z1, z2);
                swap(n1, n2);
                dy = y2 - y1;
                dx = x2 - x1;
            }

            int valor1 = 2 * abs(dx);
            int valor2 = valor1 - (2 * dy);

            int d = valor1 - dy;

            int x = x1;
            int add = (dx < 0) ? -1 : 1;

            double zstep = (z2 - z1) / abs(y2 - y1);
            double zacum = z1 + zstep;

            double xnstep = (n2[0] - n1[0]) / abs(y2 - y1);
            double xnacum = n1[0] + xnstep; //R
            double ynstep = (n2[1] - n1[1]) / abs(y2 - y1);
            double ynacum = n1[1] + ynstep; //G
            double znstep = (n2[2] - n1[2]) / abs(y2 - y1);
            double znacum = n1[2] + znstep; //B

            for (int y = y1 + 1; y < y2; y++)
            {
                if (d < 0)
                    d += valor1;
                else
                {
                    d += valor2;
                    x += add;
                }
                toInsert[0] = x;
                toInsert[1] = y;
                toInsert[2] = zacum;
                toInsert[3] = xnacum;
                toInsert[4] = ynacum;
                toInsert[5] = znacum;
                toReturn.push_back(toInsert);
                zacum += zstep;
                xnacum += xnstep;
                ynacum += ynstep;
                znacum += znstep;
            }
        }
        return toReturn;
    }
    
    // Painting 3D Objects 
    void paint3D(Object3D objeto)
    {
        /*
        for (int i = 0; i < puntosObj.size(); i++)
        {
            for (int i2 = 0; i2 < puntosObj[i].size(); i2++)
            {
                cout << puntosObj[i][i2] << " ";
            }
            cout << "\n";
        }
        cout << "\n\n\n";
        for (int i = 0; i < carasObj.size(); i++)
        {
            for (int i2 = 0; i2 < carasObj[i].size(); i2++)
            {
                cout << carasObj[i][i2] << " ";
            }
            cout << "\n";
        }
        //*/
        Object3D obj = objeto;
        if(!camaraOrtogonal)
        {
            /*
            cout << "trnsformando";
            // Proyectar
            Transformacion proyeccion;
            vector<vector<double>> tabla(4, vector<double>(4, 0));
            for (int i = 0; i < 3; i++)
                tabla[i][i] = 1;
            if(camaraEje == 'x')
                tabla[3][0] = 1 / f;
            else if (camaraEje == 'y')
                tabla[3][1] = 1 / f;
            else
                tabla[3][2] = 1 / f;

            for (int i = 0; i < tabla.size(); i++)
            {
                for (int i2 = 0; i2 < tabla[i].size(); i2++)
                {
                    cout << "-" << tabla[i][i2];
                }
                cout << "\n";
            }
            
            proyeccion.setTabla(tabla);
            obj.transformar(proyeccion);
            obj.regresarHomogeneo();
            */
            // Proyectar
            Transformacion proyeccion;
            vector<vector<double>> tabla(4, vector<double>(4, 0));
            for (int i = 0; i < 3; i++)
                tabla[i][i] = 1;
            if (camaraEje == 'x')
                tabla[3][0] = 1 / f;
            else if (camaraEje == 'y')
                tabla[3][1] = 1 / f;
            else
                tabla[3][2] = 1 / f;
            proyeccion.setTabla(tabla);
            obj.transformar(proyeccion);
            obj.mover(rx / 2, ry / 2, 0); //Arreglada perspectiva aqui tambien para poder tener vista en wireframe
            obj.regresarHomogeneo();
        }

        int ejeX = 0, ejeY = 1; //Suponiendo 'z'
        if (camaraEje == 'x')
        { ejeX = 2; ejeY = 1; }
        else if (camaraEje == 'y')
        { ejeY = 2; }

        if(!obj.isVLF())
        {
            vector<vector<double>> puntosObj = obj.getPuntos();
            vector<vector<double>> carasObj = obj.getCaras();
            for (int c = 0; c < carasObj.size(); c++)
            {
                int x1, y1, x2, y2;
                for (int p = 0; p < carasObj[c].size() - 1; p++)
                {
                    x1 = round( puntosObj[ carasObj[c][p] - 1 ][ejeX]     );
                    y1 = round( puntosObj[ carasObj[c][p] - 1 ][ejeY]     );
                    x2 = round( puntosObj[ carasObj[c][p + 1] - 1 ][ejeX] );
                    y2 = round( puntosObj[ carasObj[c][p + 1] - 1 ][ejeY] );
                    drawLineBresenham( x1, y1, x2, y2 );
                }
                //Pintamos del ultimo punto al primero de la cara
                x1 = round( puntosObj[carasObj[c][0] - 1][ejeX] );
                y1 = round( puntosObj[carasObj[c][0] - 1][ejeY] );
                drawLineBresenham(x2, y2, x1, y1); //Invertido pues ya tenemos en x2,y2 el ultimo punto
            }
        }
        else
        {
            vector<vector<double>> puntosObj = obj.getPuntos();
            vector<vector<double>> ladosObj = obj.getLados();
            vector<vector<double>> carasObj = obj.getCaras();
            for (int c = 0; c < carasObj.size(); c++)
            {
                for (int l = 0; l < 3; l++)
                {
                    int x1, y1, x2, y2;
                    x1 = round(puntosObj[ ladosObj[carasObj[c][l] - 1 ][0] - 1 ][ejeX]);
                    y1 = round(puntosObj[ ladosObj[carasObj[c][l] - 1 ][0] - 1 ][ejeY]);
                    x2 = round(puntosObj[ ladosObj[carasObj[c][l] - 1 ][1] - 1 ][ejeX]);
                    y2 = round(puntosObj[ ladosObj[carasObj[c][l] - 1 ][1] - 1 ][ejeY]);
                    drawLineBresenham(x1, y1, x2, y2);
                }
            }
        }
        
        return;
    }
    void paint3D(Object3D objeto, bool hideFace)
    {
        if(!hideFace) { paint3D(objeto); return; }

        Object3D obj = objeto;
        vector<double> vcam(3,0); //Vector de la camara

        if (!obj.hasNormales())
        { obj.convertirVLFNormal(); }
        vector<vector<double>> normalesObj = obj.getNormales();
        if(magnitud(normalesObj[0])!=1) { obj.normalizarNormales(); normalesObj = obj.getNormales(); }
        
        if (!camaraOrtogonal)
        {
            /*
            cout << "trnsformando";
            // Proyectar
            Transformacion proyeccion;
            vector<vector<double>> tabla(4, vector<double>(4, 0));
            for (int i = 0; i < 3; i++)
                tabla[i][i] = 1;
            if (camaraEje == 'x')
                tabla[3][0] = 1 / f;
            else if (camaraEje == 'y')
                tabla[3][1] = 1 / f;
            else
                tabla[3][2] = 1 / f;

            for (int i = 0; i < tabla.size(); i++)
            {
                for (int i2 = 0; i2 < tabla[i].size(); i2++)
                {
                    cout << "-" << tabla[i][i2];
                }
                cout << "\n";
            }
            proyeccion.setTabla(tabla);
            obj.transformar(proyeccion);
            obj.regresarHomogeneo();
            */
            // Proyectar
            Transformacion proyeccion;
            vector<vector<double>> tabla(4, vector<double>(4, 0));
            for (int i = 0; i < 3; i++)
                tabla[i][i] = 1;
            if (camaraEje == 'x')
                tabla[3][0] = 1 / f;
            else if (camaraEje == 'y')
                tabla[3][1] = 1 / f;
            else
                tabla[3][2] = 1 / f;
            proyeccion.setTabla(tabla);
            obj.transformar(proyeccion);
            obj.mover(rx / 2, ry / 2, 0); //Arreglada perspectiva aqui tambien para poder tener vista en wireframe
            obj.regresarHomogeneo();
        }

        int ejeX = 0, ejeY = 1; //Suponiendo 'z'
        if (camaraEje == 'x')
        {
            ejeX = 2;
            ejeY = 1;
            vcam[0] = 1;
        }
        else if (camaraEje == 'y')
        {
            ejeY = 2;
            vcam[1] = 1;
        }
        else vcam[2] = 1;

        
        vector<vector<double>> puntosObj = obj.getPuntos();
        vector<vector<double>> ladosObj = obj.getLados();
        vector<vector<double>> carasObj = obj.getCaras();

        for (int c = 0; c < carasObj.size(); c++)
        {
            double angle = acos(dotProd(normalesObj[c], vcam)) * (180 / M_PI); //Las normales y la camara estan normalizados.
            //checar angulo
            if(180 > angle && angle >= 90) //Show it! 
            {
                for (int l = 0; l < 3; l++)
                {
                    int x1, y1, x2, y2;
                    x1 = round(puntosObj[ladosObj[carasObj[c][l] - 1][0] - 1][ejeX]);
                    y1 = round(puntosObj[ladosObj[carasObj[c][l] - 1][0] - 1][ejeY]);
                    x2 = round(puntosObj[ladosObj[carasObj[c][l] - 1][1] - 1][ejeX]);
                    y2 = round(puntosObj[ladosObj[carasObj[c][l] - 1][1] - 1][ejeY]);
                    drawLineBresenham(x1, y1, x2, y2);
                }
            }
        }

        return;
    }
    void paint3DScanline(Object3D objeto)
    {   paint3DScanline(objeto,{ r , g , b }); }
    void paint3DScanline(Object3D objeto, vector<int> color)
    {
        Object3D obj = objeto;
        vector<double> vcam(3,0); //Vector de la camara

        if (!obj.hasNormales())
        { obj.convertirVLFNormal(); }
        vector<vector<double>> normalesObj = obj.getNormales();
        if(magnitud(normalesObj[0])!=1) { obj.normalizarNormales(); normalesObj = obj.getNormales(); }
        
        if (!camaraOrtogonal)
        {
            cout << "trnsformando";
            // Proyectar
            Transformacion proyeccion;
            vector<vector<double>> tabla(4, vector<double>(4, 0));
            for (int i = 0; i < 3; i++)
                tabla[i][i] = 1;
            if (camaraEje == 'x')
                tabla[3][0] = 1 / f;
            else if (camaraEje == 'y')
                tabla[3][1] = 1 / f;
            else
                tabla[3][2] = 1 / f;

            for (int i = 0; i < tabla.size(); i++)
            {
                for (int i2 = 0; i2 < tabla[i].size(); i2++)
                {
                    cout << "-" << tabla[i][i2];
                }
                cout << "\n";
            }

            proyeccion.setTabla(tabla);
            obj.transformar(proyeccion);
            obj.mover(rx / 2, ry / 2, 0);
            obj.regresarHomogeneo();
        }

        int ejeX = 0, ejeY = 1; //Suponiendo 'z'
        if (camaraEje == 'x')
        {
            ejeX = 2;
            ejeY = 1;
            vcam[0] = 1;
        }
        else if (camaraEje == 'y')
        {
            ejeY = 2;
            vcam[1] = 1;
        }
        else vcam[2] = 1;
        
        vector<vector<double>> puntosObj = obj.getPuntos();
        vector<vector<double>> ladosObj = obj.getLados();
        vector<vector<double>> carasObj = obj.getCaras();

        bool mismoColor = false;
        if(color[0] == r && color[1] == g && color[2] == b) mismoColor = true;

        for (int c = 0; c < carasObj.size(); c++)
        {
            double angle = acos(dotProd(normalesObj[c], vcam)) * (180 / M_PI); //La camara estan normalizados.
            //checar angulo
            if(180 > angle && angle >= 90) //Show it! 
            {
                vector<vector<int>> toPaint; 
                for (int l = 0; l < 3; l++)
                {
                    int x1, y1, x2, y2;
                    x1 = round(puntosObj[ladosObj[carasObj[c][l] - 1][0] - 1][ejeX]);
                    y1 = round(puntosObj[ladosObj[carasObj[c][l] - 1][0] - 1][ejeY]);
                    x2 = round(puntosObj[ladosObj[carasObj[c][l] - 1][1] - 1][ejeX]);
                    y2 = round(puntosObj[ladosObj[carasObj[c][l] - 1][1] - 1][ejeY]);
                    vector<vector<int>> puntos = drawLineBresenhamPoints(x1, y1, x2, y2);
                    for(vector<int> p : puntos) toPaint.push_back(p);
                }
                //Burbuja Sort
                int i, j;
                for (i = 0; i < toPaint.size() - 1; i++)
                {
                    for (j = 0; j < toPaint.size() - i - 1; j++)
                    {
                        if (toPaint[j][1] > toPaint[j + 1][1])
                        {
                            vector<int> aux = toPaint[j];
                            toPaint[j] = toPaint[j + 1];
                            toPaint[j + 1] = aux;
                        }
                    }
                }
                int yRev = toPaint[0][1];
                vector<vector<int>> bulk;
                for (int i = 0; i < toPaint.size(); i++)
                {
                    if(yRev == toPaint[i][1]) bulk.push_back(toPaint[i]);
                    else
                    {
                        //Paint points
                        int xmin = rx;
                        int xmax = -1;
                        for (int i2 = 0; i2 < bulk.size(); i2++)
                        {
                            if(xmin > bulk[i2][0]) xmin = bulk[i2][0];
                            if(xmax < bulk[i2][0]) xmax = bulk[i2][0];
                        }
                        for(int cnt = xmin; cnt <= xmax; cnt++)
                        {
                            if(!mismoColor)
                            {
                                bool original = false;
                                for (int i2 = 0; i2 < toPaint.size(); i2++)
                                {
                                    if(cnt == toPaint[i2][0] && yRev == toPaint[i2][1])
                                    {
                                        original = true;
                                        break;
                                    }
                                }
                                if(original) pixel(cnt,yRev);
                                else pixel(cnt, yRev, color);
                            }
                            else pixel(cnt,yRev);
                        }
                        //Change yrev 
                        yRev = toPaint[i][1];
                        bulk.clear();
                        bulk.push_back(toPaint[i]);
                    }
                }
                //Paint last bulk
                int xmin = rx;
                int xmax = -1;
                for (int i2 = 0; i2 < bulk.size(); i2++)
                {
                    if(xmin > bulk[i2][0]) xmin = bulk[i2][0];
                    if(xmax < bulk[i2][0]) xmax = bulk[i2][0];
                }
                for(int cnt = xmin; cnt <= xmax; cnt++)
                {
                   if(!mismoColor)
                   {
                       bool original = false;
                       for (int i2 = 0; i2 < toPaint.size(); i2++)
                       {
                           if(cnt == toPaint[i2][0] && yRev == toPaint[i2][1])
                           {
                               original = true;
                               break;
                           }
                       }
                       if(original) pixel(cnt,yRev);
                       else pixel(cnt, yRev, color);
                   }
                   else pixel(cnt,yRev);
                }
            }
        }

        return;
    }
    void paint3DZBuffer(Object3D objeto)
    {   paint3DZBuffer(objeto,{ r , g , b });}   
    void paint3DZBuffer(Object3D objeto, vector<int> color)
    {
        Object3D obj = objeto;
        vector<double> vcam(3,0); //Vector de la camara

        if (!obj.hasNormales())
        { obj.convertirVLFNormal(); }
        vector<vector<double>> normalesObj = obj.getNormales();
        //if(magnitud(normalesObj[0])!=1) { obj.normalizarNormales(); normalesObj = obj.getNormales(); }
        
        if (!camaraOrtogonal)
        {
            cout << "trnsformando";
            // Proyectar
            Transformacion proyeccion;
            vector<vector<double>> tabla(4, vector<double>(4, 0));
            for (int i = 0; i < 3; i++)
                tabla[i][i] = 1;
            if (camaraEje == 'x')
                tabla[3][0] = 1 / f;
            else if (camaraEje == 'y')
                tabla[3][1] = 1 / f;
            else
                tabla[3][2] = 1 / f;

            for (int i = 0; i < tabla.size(); i++)
            {
                for (int i2 = 0; i2 < tabla[i].size(); i2++)
                {
                    cout << "-" << tabla[i][i2];
                }
                cout << "\n";
            }

            proyeccion.setTabla(tabla);
            obj.transformar(proyeccion);
            obj.mover(rx / 2, ry / 2, 0);
            obj.regresarHomogeneo();
        }

        int ejeX = 0, ejeY = 1, ejeZ = 2; //Suponiendo 'z'
        if (camaraEje == 'x')
        {
            ejeX = 2;
            ejeY = 1;
            ejeZ = 0;
            vcam[0] = 1;
        }
        else if (camaraEje == 'y')
        {
            ejeY = 2;
            ejeZ = 1;
            vcam[1] = 1;
        }
        else vcam[2] = 1;
        
        vector<vector<double>> puntosObj = obj.getPuntos();
        vector<vector<double>> ladosObj = obj.getLados();
        vector<vector<double>> carasObj = obj.getCaras();

        vector<vector<double>> puntosOriginales = objeto.getPuntos();
        bool mismoColor = false;
        if(color[0] == r && color[1] == g && color[2] == b) mismoColor = true;

        for (int c = 0; c < carasObj.size(); c++)
        {
            double angle = acos((dotProd(normalesObj[c], vcam))/magnitud(normalesObj[c])) * (180 / M_PI); //La camara esta normalizados.
            //checar angulo
            if(180 >= angle && angle >= 90) //Show it! 
            {
                vector<vector<double>> toPaint;
                for (int l = 0; l < 3; l++)
                {
                    int x1, y1, x2, y2;
                    double z1, z2;
                    x1 = round(puntosObj[ladosObj[carasObj[c][l] - 1][0] - 1][ejeX]);
                    y1 = round(puntosObj[ladosObj[carasObj[c][l] - 1][0] - 1][ejeY]);
                    z1 = puntosOriginales[ladosObj[carasObj[c][l] - 1][0] - 1][ejeZ];
                    x2 = round(puntosObj[ladosObj[carasObj[c][l] - 1][1] - 1][ejeX]);
                    y2 = round(puntosObj[ladosObj[carasObj[c][l] - 1][1] - 1][ejeY]);
                    z2 = puntosOriginales[ladosObj[carasObj[c][l] - 1][1] - 1][ejeZ];
                    vector<vector<double>> puntos = drawLineBresenhamPointsZ(x1, y1, z1, x2, y2, z2);
                    for(vector<double> p : puntos) toPaint.push_back(p);
                }
                //Burbuja Sort
                int i, j;
                for (i = 0; i < toPaint.size() - 1; i++)
                {
                    for (j = 0; j < toPaint.size() - i - 1; j++)
                    {
                        if (toPaint[j][1] > toPaint[j + 1][1])
                        {
                            vector<double> aux = toPaint[j];
                            toPaint[j] = toPaint[j + 1];
                            toPaint[j + 1] = aux;
                        }
                    }
                }
                int yRev = toPaint[0][1];
                vector<vector<double>> bulk;
                for (int i = 0; i < toPaint.size(); i++)
                {
                    if(yRev == toPaint[i][1]) { bulk.push_back(toPaint[i]); }
                    else
                    {
                        //Paint points
                        int xmin = rx;
                        int xmax = -1;
                        double zmin = 0;
                        double zmax = 0;
                        for (int i2 = 0; i2 < bulk.size(); i2++)
                        {
                            if(xmin > bulk[i2][0]) { xmin = bulk[i2][0]; zmin = bulk[i2][2]; }
                            if(xmax < bulk[i2][0]) { xmax = bulk[i2][0]; zmax = bulk[i2][2]; }
                        }
                        pixel(xmin, yRev, zmin);
                        if(xmin != xmax)
                        {
                            pixel(xmax, yRev, zmax);
                            double stepz = (zmax - zmin) / abs(xmax - xmin);
                            double acumuladoz = zmin + stepz;
                            for (int cnt = xmin+1; cnt < xmax; cnt++)
                            {
                                if (!mismoColor)
                                {
                                    bool original = false;
                                    for (int i2 = 0; i2 < toPaint.size(); i2++)
                                    {
                                        if (cnt == toPaint[i2][0] && yRev == toPaint[i2][1])
                                        {
                                            original = true;
                                            break;
                                        }
                                    }
                                    if (original)
                                        pixel(cnt, yRev, acumuladoz);
                                    else
                                        pixel(cnt, yRev, acumuladoz, color);
                                }
                                else
                                    pixel(cnt, yRev, acumuladoz);
                                acumuladoz += stepz;
                            }
                        }
                        //Change yrev 
                        yRev = toPaint[i][1];
                        bulk.clear();
                        bulk.push_back(toPaint[i]);
                    }
                }
                //Paint last bulk
                int xmin = rx;
                int xmax = -1;
                double zmin = 0;
                double zmax = 0;
                for (int i2 = 0; i2 < bulk.size(); i2++)
                {
                    if(xmin >= bulk[i2][0]) { xmin = bulk[i2][0]; zmin = bulk[i2][2]; }
                    if(xmax <= bulk[i2][0]) { xmax = bulk[i2][0]; zmax = bulk[i2][2]; }
                }
                pixel(xmin, yRev, zmin);
                if(xmin != xmax)
                {
                    pixel(xmax, yRev, zmax);
                    double stepz = (zmax - zmin) / abs(xmax - xmin);
                    double acumuladoz = zmin + stepz;
                    for (int cnt = xmin+1; cnt < xmax; cnt++)
                    {
                        if (!mismoColor)
                        {
                            bool original = false;
                            for (int i2 = 0; i2 < toPaint.size(); i2++)
                            {
                                if (cnt == toPaint[i2][0] && yRev == toPaint[i2][1])
                                {
                                    original = true;
                                    break;
                                }
                            }
                            if (original)
                                pixel(cnt, yRev, acumuladoz);
                            else
                                pixel(cnt, yRev, acumuladoz, color);
                        }
                        else
                            pixel(cnt, yRev, acumuladoz);
                        acumuladoz += stepz;
                    }
                }
            }
        }
        return;
    }
    void paint3DForAttenuation(Object3D objeto)
    {
        Object3D obj = objeto;
        vector<double> vcam(3,0); //Vector de la camara

        if (!obj.hasNormales())
        { obj.convertirVLFNormal(); }
        vector<vector<double>> normalesObj = obj.getNormales();
        
        if (!camaraOrtogonal)
        {
            // Proyectar
            Transformacion proyeccion;
            vector<vector<double>> tabla(4, vector<double>(4, 0));
            for (int i = 0; i < 3; i++)
                tabla[i][i] = 1;
            if (camaraEje == 'x')
                tabla[3][0] = 1 / f;
            else if (camaraEje == 'y')
                tabla[3][1] = 1 / f;
            else
                tabla[3][2] = 1 / f;
            proyeccion.setTabla(tabla);
            obj.transformar(proyeccion);
            obj.regresarHomogeneo();
        }

        int ejeX = 0, ejeY = 1, ejeZ = 2; //Suponiendo 'z'
        if (camaraEje == 'x')
        {
            ejeX = 2;
            ejeY = 1;
            ejeZ = 0;
            vcam[0] = 1;
        }
        else if (camaraEje == 'y')
        {
            ejeY = 2;
            ejeZ = 1;
            vcam[1] = 1;
        }
        else vcam[2] = 1;
        
        vector<vector<double>> puntosObj = obj.getPuntos();
        vector<vector<double>> ladosObj = obj.getLados();
        vector<vector<double>> carasObj = obj.getCaras();
        vector<vector<double>> puntosOriginales = objeto.getPuntos();
        
        for (int c = 0; c < carasObj.size(); c++)
        {
            double angle = acos((dotProd(normalesObj[c], vcam))/magnitud(normalesObj[c])) * (180 / M_PI); //La camara estan normalizados.
            //checar angulo
            if(180 >= angle && angle >= 90) //Show it! 
            {
                vector<vector<double>> toPaint;
                vector<vector<double>> original;
                for (int l = 0; l < 3; l++)
                {
                    int x1, y1, x2, y2;
                    double z1, z2;
                    double ox1, ox2, oy1, oy2; //o stands for original (before projecting)
                    x1 = round(puntosObj[ladosObj[carasObj[c][l] - 1][0] - 1][ejeX]);
                    y1 = round(puntosObj[ladosObj[carasObj[c][l] - 1][0] - 1][ejeY]);
                    z1 = puntosOriginales[ladosObj[carasObj[c][l] - 1][0] - 1][ejeZ];
                    x2 = round(puntosObj[ladosObj[carasObj[c][l] - 1][1] - 1][ejeX]);
                    y2 = round(puntosObj[ladosObj[carasObj[c][l] - 1][1] - 1][ejeY]);
                    z2 = puntosOriginales[ladosObj[carasObj[c][l] - 1][1] - 1][ejeZ];

                    ox1 = puntosOriginales[ladosObj[carasObj[c][l] - 1][0] - 1][ejeX];
                    oy1 = puntosOriginales[ladosObj[carasObj[c][l] - 1][0] - 1][ejeY];
                    ox2 = puntosOriginales[ladosObj[carasObj[c][l] - 1][1] - 1][ejeX];
                    oy2 = puntosOriginales[ladosObj[carasObj[c][l] - 1][1] - 1][ejeY];

                    vector<vector<double>> puntos = drawLineBresenhamPointsZ(x1, y1, ox1, oy1, z1, x2, y2, ox2, oy2, z2);
                    //Con esto ya tenemos los x, y originales interpolados en [3] y [4]
                    for(vector<double> p : puntos)
                        toPaint.push_back(p);
                }
                //Burbuja Sort
                int i, j;
                for (i = 0; i < toPaint.size() - 1; i++)
                {
                    for (j = 0; j < toPaint.size() - i - 1; j++)
                    {
                        if (toPaint[j][1] > toPaint[j + 1][1])
                        {
                            vector<double> aux = toPaint[j];
                            toPaint[j] = toPaint[j + 1];
                            toPaint[j + 1] = aux;
                        }
                    }
                }
                int yRev = toPaint[0][1];
                vector<vector<double>> bulk;
                for (int i = 0; i < toPaint.size(); i++)
                {
                    if(yRev == toPaint[i][1]) { bulk.push_back(toPaint[i]); }
                    else
                    {
                        //Paint points
                        int xmin = rx;
                        int xmax = -1;
                        double zmin, zmax, oxmin, oxmax, oymin, oymax;
                        for (int i2 = 0; i2 < bulk.size(); i2++)
                        {
                            if(xmin > bulk[i2][0]) 
                            { 
                                xmin = bulk[i2][0]; 
                                zmin = bulk[i2][2];
                                oxmin = bulk[i2][3];
                                oymin = bulk[i2][4];
                            }
                            if(xmax < bulk[i2][0]) 
                            { 
                                xmax = bulk[i2][0]; 
                                zmax = bulk[i2][2];
                                oxmax = bulk[i2][3];
                                oymax = bulk[i2][4];
                            }
                        }
                        pixel(xmin, yRev, oxmin, oymin, zmin, obj.getRGB());
                        if(xmin != xmax)
                        {
                            pixel(xmax, yRev, oxmax, oymax, zmax, obj.getRGB());

                            double stepz = (zmax - zmin) / abs(xmax - xmin);
                            double acumuladoz = zmin + stepz;

                            double xstep = (oxmax - oxmin) / abs(xmax - xmin);
                            double xacum = oxmin + xstep;

                            double ystep = (oymax - oymin) / abs(xmax - xmin);
                            double yacum = oymin + ystep;

                            for (int cnt = xmin+1; cnt < xmax; cnt++)
                            {
                                pixel(cnt, yRev, xacum, yacum, acumuladoz, obj.getRGB());
                                acumuladoz += stepz;
                                xacum += xstep;
                                yacum += ystep;
                            }
                        }
                        //Change yrev 
                        yRev = toPaint[i][1];
                        bulk.clear();
                        bulk.push_back(toPaint[i]);
                    }
                }
                //Paint last bulk
                int xmin = rx;
                int xmax = -1;
                double zmin, zmax, oxmin, oxmax, oymin, oymax;
                for (int i2 = 0; i2 < bulk.size(); i2++)
                {
                    if (xmin > bulk[i2][0])
                    {
                        xmin = bulk[i2][0];
                        zmin = bulk[i2][2];
                        oxmin = bulk[i2][3];
                        oymin = bulk[i2][4];
                    }
                    if (xmax < bulk[i2][0])
                    {
                        xmax = bulk[i2][0];
                        zmax = bulk[i2][2];
                        oxmax = bulk[i2][3];
                        oymax = bulk[i2][4];
                    }
                }
                pixel(xmin, yRev, oxmin, oymin, zmin, obj.getRGB());
                if (xmin != xmax)
                {
                    pixel(xmax, yRev, oxmax, oymax, zmax, obj.getRGB());

                    double stepz = (zmax - zmin) / abs(xmax - xmin);
                    double acumuladoz = zmin + stepz;

                    double xstep = (oxmax - oxmin) / abs(xmax - xmin);
                    double xacum = oxmin + xstep;

                    double ystep = (oymax - oymin) / abs(xmax - xmin);
                    double yacum = oymin + ystep;

                    for (int cnt = xmin + 1; cnt < xmax; cnt++)
                    {
                        pixel(cnt, yRev, xacum, yacum, acumuladoz, obj.getRGB());
                        acumuladoz += stepz;
                        xacum += xstep;
                        yacum += ystep;
                    }
                }
            }
        }
        return;
    }
    // With material & Perspective fixed by moving proyected points
    void paint3DWithMaterial(Object3D objeto)
    {
        Object3D obj = objeto;
        vector<double> vcam(3,0); //Vector de la camara

        if (!obj.hasNormales())
        { obj.convertirVLFNormal(); }
        vector<vector<double>> normalesObj = obj.getNormales();
        
        if (!camaraOrtogonal)
        {
            // Proyectar
            Transformacion proyeccion;
            vector<vector<double>> tabla(4, vector<double>(4, 0));
            for (int i = 0; i < 3; i++)
                tabla[i][i] = 1;
            if (camaraEje == 'x')
                tabla[3][0] = 1 / f;
            else if (camaraEje == 'y')
                tabla[3][1] = 1 / f;
            else
                tabla[3][2] = 1 / f;
            proyeccion.setTabla(tabla);
            obj.transformar(proyeccion);
            obj.mover(rx/2,ry/2,0); //Mover proyeccion al centro de la imagen
            obj.regresarHomogeneo();
        }

        int ejeX = 0, ejeY = 1, ejeZ = 2; //Suponiendo 'z'
        if (camaraEje == 'x')
        {
            ejeX = 2;
            ejeY = 1;
            ejeZ = 0;
            vcam[0] = 1;
        }
        else if (camaraEje == 'y')
        {
            ejeY = 2;
            ejeZ = 1;
            vcam[1] = 1;
        }
        else vcam[2] = 1;
        
        vector<vector<double>> puntosObj = obj.getPuntos();
        vector<vector<double>> ladosObj = obj.getLados();
        vector<vector<double>> carasObj = obj.getCaras();
        vector<vector<double>> puntosOriginales = objeto.getPuntos();
        vector<Material> materialesObj = obj.getMateriales();

        for (int c = 0; c < carasObj.size(); c++)
        {
            double angle = acos((dotProd(normalesObj[c], vcam))/magnitud(normalesObj[c])) * (180 / M_PI); //La camara estan normalizados.
            //checar angulo
            if(180 >= angle && angle >= 90) //Show it! 
            {
                vector<vector<double>> toPaint;
                vector<vector<double>> original;
                for (int l = 0; l < 3; l++)
                {
                    int x1, y1, x2, y2;
                    double z1, z2;
                    double ox1, ox2, oy1, oy2; //o stands for original (before projecting)
                    x1 = round(puntosObj[ladosObj[carasObj[c][l] - 1][0] - 1][ejeX]);
                    y1 = round(puntosObj[ladosObj[carasObj[c][l] - 1][0] - 1][ejeY]);
                    z1 = puntosOriginales[ladosObj[carasObj[c][l] - 1][0] - 1][ejeZ];
                    x2 = round(puntosObj[ladosObj[carasObj[c][l] - 1][1] - 1][ejeX]);
                    y2 = round(puntosObj[ladosObj[carasObj[c][l] - 1][1] - 1][ejeY]);
                    z2 = puntosOriginales[ladosObj[carasObj[c][l] - 1][1] - 1][ejeZ];

                    ox1 = puntosOriginales[ladosObj[carasObj[c][l] - 1][0] - 1][ejeX];
                    oy1 = puntosOriginales[ladosObj[carasObj[c][l] - 1][0] - 1][ejeY];
                    ox2 = puntosOriginales[ladosObj[carasObj[c][l] - 1][1] - 1][ejeX];
                    oy2 = puntosOriginales[ladosObj[carasObj[c][l] - 1][1] - 1][ejeY];

                    vector<vector<double>> puntos = drawLineBresenhamPointsZ(x1, y1, ox1, oy1, z1, x2, y2, ox2, oy2, z2);
                    //Con esto ya tenemos los x, y originales interpolados en [3] y [4]
                    for(vector<double> p : puntos)
                        toPaint.push_back(p);
                }
                //Burbuja Sort
                int i, j;
                for (i = 0; i < toPaint.size() - 1; i++)
                {
                    for (j = 0; j < toPaint.size() - i - 1; j++)
                    {
                        if (toPaint[j][1] > toPaint[j + 1][1])
                        {
                            vector<double> aux = toPaint[j];
                            toPaint[j] = toPaint[j + 1];
                            toPaint[j + 1] = aux;
                        }
                    }
                }
                int yRev = toPaint[0][1];
                vector<vector<double>> bulk;
                for (int i = 0; i < toPaint.size(); i++)
                {
                    if(yRev == toPaint[i][1]) { bulk.push_back(toPaint[i]); }
                    else
                    {
                        //Paint points
                        int xmin = rx;
                        int xmax = -1;
                        double zmin, zmax, oxmin, oxmax, oymin, oymax;
                        for (int i2 = 0; i2 < bulk.size(); i2++)
                        {
                            if(xmin > bulk[i2][0]) 
                            { 
                                xmin = bulk[i2][0]; 
                                zmin = bulk[i2][2];
                                oxmin = bulk[i2][3];
                                oymin = bulk[i2][4];
                            }
                            if(xmax < bulk[i2][0]) 
                            { 
                                xmax = bulk[i2][0]; 
                                zmax = bulk[i2][2];
                                oxmax = bulk[i2][3];
                                oymax = bulk[i2][4];
                            }
                        }
                        pixelMaterial(xmin, yRev, oxmin, oymin, zmin, obj.getRGB(), materialesObj[c], normalesObj[c]);
                        if(xmin != xmax)
                        {
                            pixelMaterial(xmax, yRev, oxmax, oymax, zmax, obj.getRGB(), materialesObj[c], normalesObj[c]);

                            double stepz = (zmax - zmin) / abs(xmax - xmin);
                            double acumuladoz = zmin + stepz;

                            double xstep = (oxmax - oxmin) / abs(xmax - xmin);
                            double xacum = oxmin + xstep;

                            double ystep = (oymax - oymin) / abs(xmax - xmin);
                            double yacum = oymin + ystep;

                            for (int cnt = xmin+1; cnt < xmax; cnt++)
                            {
                                pixelMaterial(cnt, yRev, xacum, yacum, acumuladoz, obj.getRGB(), materialesObj[c], normalesObj[c]);
                                acumuladoz += stepz;
                                xacum += xstep;
                                yacum += ystep;
                            }
                        }
                        //Change yrev 
                        yRev = toPaint[i][1];
                        bulk.clear();
                        bulk.push_back(toPaint[i]);
                    }
                }
                //Paint last bulk
                int xmin = rx;
                int xmax = -1;
                double zmin, zmax, oxmin, oxmax, oymin, oymax;
                for (int i2 = 0; i2 < bulk.size(); i2++)
                {
                    if (xmin > bulk[i2][0])
                    {
                        xmin = bulk[i2][0];
                        zmin = bulk[i2][2];
                        oxmin = bulk[i2][3];
                        oymin = bulk[i2][4];
                    }
                    if (xmax < bulk[i2][0])
                    {
                        xmax = bulk[i2][0];
                        zmax = bulk[i2][2];
                        oxmax = bulk[i2][3];
                        oymax = bulk[i2][4];
                    }
                }
                pixelMaterial(xmin, yRev, oxmin, oymin, zmin, obj.getRGB(), materialesObj[c], normalesObj[c]);
                if (xmin != xmax)
                {
                    pixelMaterial(xmax, yRev, oxmax, oymax, zmax, obj.getRGB(), materialesObj[c], normalesObj[c]);

                    double stepz = (zmax - zmin) / abs(xmax - xmin);
                    double acumuladoz = zmin + stepz;

                    double xstep = (oxmax - oxmin) / abs(xmax - xmin);
                    double xacum = oxmin + xstep;

                    double ystep = (oymax - oymin) / abs(xmax - xmin);
                    double yacum = oymin + ystep;

                    for (int cnt = xmin + 1; cnt < xmax; cnt++)
                    {
                        pixelMaterial(cnt, yRev, xacum, yacum, acumuladoz, obj.getRGB(), materialesObj[c], normalesObj[c]);
                        acumuladoz += stepz;
                        xacum += xstep;
                        yacum += ystep;
                    }
                }
            }
        }
        return;
    }
    void paint3DPhong(Object3D objeto)
    {
        Object3D obj = objeto;
        vector<double> vcam(3,0); //Vector de la camara

        if (!obj.hasNormales())
        { obj.convertirVLFNormal(); }
        vector<vector<double>> normalesObj = obj.getNormales();
        
        if (!camaraOrtogonal)
        {
            // Proyectar
            Transformacion proyeccion;
            vector<vector<double>> tabla(4, vector<double>(4, 0));
            for (int i = 0; i < 3; i++)
                tabla[i][i] = 1;
            if (camaraEje == 'x')
                tabla[3][0] = 1 / f;
            else if (camaraEje == 'y')
                tabla[3][1] = 1 / f;
            else
                tabla[3][2] = 1 / f;
            proyeccion.setTabla(tabla);
            obj.transformar(proyeccion);
            obj.mover(rx/2,ry/2,0); //Mover proyeccion al centro de la imagen
            obj.regresarHomogeneo();
        }

        int ejeX = 0, ejeY = 1, ejeZ = 2; //Suponiendo 'z'
        if (camaraEje == 'x')
        {
            ejeX = 2;
            ejeY = 1;
            ejeZ = 0;
            vcam[0] = 1;
        }
        else if (camaraEje == 'y')
        {
            ejeY = 2;
            ejeZ = 1;
            vcam[1] = 1;
        }
        else vcam[2] = 1;
        
        vector<vector<double>> puntosObj = obj.getPuntos();
        vector<vector<double>> ladosObj = obj.getLados();
        vector<vector<double>> carasObj = obj.getCaras();
        vector<vector<double>> puntosOriginales = objeto.getPuntos();
        vector<Material> materialesObj = obj.getMateriales();
        //Calculate vertex Normals
        if(!objeto.hasVertexNormals()) objeto.convertirVLFVertexNormal();
        vector<vector<double>> vertexNormal = objeto.getVertexNormals();
        for (int c = 0; c < carasObj.size(); c++)
        {
            double angle = acos((dotProd(normalesObj[c], vcam))/magnitud(normalesObj[c])) * (180 / M_PI); //La camara estan normalizados.
            //checar angulo
            if(180 >= angle && angle >= 90) //Show it! 
            {
                vector<vector<double>> toPaint;
                for (int l = 0; l < 3; l++)
                {
                    int x1, y1, x2, y2;
                    double z1, z2;
                    double ox1, ox2, oy1, oy2; //o stands for original (before projecting)
                    x1 = round(puntosObj[ladosObj[carasObj[c][l] - 1][0] - 1][ejeX]);
                    y1 = round(puntosObj[ladosObj[carasObj[c][l] - 1][0] - 1][ejeY]);
                    z1 = puntosOriginales[ladosObj[carasObj[c][l] - 1][0] - 1][ejeZ];
                    x2 = round(puntosObj[ladosObj[carasObj[c][l] - 1][1] - 1][ejeX]);
                    y2 = round(puntosObj[ladosObj[carasObj[c][l] - 1][1] - 1][ejeY]);
                    z2 = puntosOriginales[ladosObj[carasObj[c][l] - 1][1] - 1][ejeZ];

                    ox1 = puntosOriginales[ladosObj[carasObj[c][l] - 1][0] - 1][ejeX];
                    oy1 = puntosOriginales[ladosObj[carasObj[c][l] - 1][0] - 1][ejeY];
                    ox2 = puntosOriginales[ladosObj[carasObj[c][l] - 1][1] - 1][ejeX];
                    oy2 = puntosOriginales[ladosObj[carasObj[c][l] - 1][1] - 1][ejeY];

                    vector<double> vn1 = vertexNormal[ladosObj[carasObj[c][l] - 1][0] - 1];
                    vector<double> vn2 = vertexNormal[ladosObj[carasObj[c][l] - 1][1] - 1];

                    vector<vector<double>> puntos = drawLineBresenhamPointsZVNormals(x1, y1, ox1, oy1, z1, x2, y2, ox2, oy2, z2, vn1, vn2);
                    //Con esto ya tenemos los x, y originales interpolados en [3] y [4] Y en [5][6][7] la normal del pixel
                    for(vector<double> p : puntos)
                        toPaint.push_back(p);
                }
                //Burbuja Sort
                int i, j;
                for (i = 0; i < toPaint.size() - 1; i++)
                {
                    for (j = 0; j < toPaint.size() - i - 1; j++)
                    {
                        if (toPaint[j][1] > toPaint[j + 1][1])
                        {
                            vector<double> aux = toPaint[j];
                            toPaint[j] = toPaint[j + 1];
                            toPaint[j + 1] = aux;
                        }
                    }
                }
                int yRev = toPaint[0][1];
                vector<vector<double>> bulk;
                for (int i = 0; i < toPaint.size(); i++)
                {
                    if(yRev == toPaint[i][1]) { bulk.push_back(toPaint[i]); }
                    else
                    {
                        //Paint points
                        int xmin = rx;
                        int xmax = -1;
                        double zmin, zmax, oxmin, oxmax, oymin, oymax;
                        double xnmin, xnmax, ynmin, ynmax, znmin, znmax; // Interpolando las normales
                        for (int i2 = 0; i2 < bulk.size(); i2++)
                        {
                            if(xmin > bulk[i2][0]) 
                            { 
                                xmin = bulk[i2][0]; 
                                zmin = bulk[i2][2];
                                oxmin = bulk[i2][3];
                                oymin = bulk[i2][4];
                                xnmin = bulk[i2][5];
                                ynmin = bulk[i2][6];
                                znmin = bulk[i2][7];
                            }
                            if(xmax < bulk[i2][0]) 
                            { 
                                xmax = bulk[i2][0]; 
                                zmax = bulk[i2][2];
                                oxmax = bulk[i2][3];
                                oymax = bulk[i2][4];
                                xnmax = bulk[i2][5];
                                ynmax = bulk[i2][6];
                                znmax = bulk[i2][7];
                            }
                        }
                        pixelMaterial(xmin, yRev, oxmin, oymin, zmin, obj.getRGB(), materialesObj[c], {xnmin, ynmin, znmin});
                        if(xmin != xmax)
                        {
                            pixelMaterial(xmax, yRev, oxmax, oymax, zmax, obj.getRGB(), materialesObj[c], {xnmax, ynmax, znmax});

                            double stepz = (zmax - zmin) / abs(xmax - xmin);
                            double acumuladoz = zmin + stepz;

                            double xstep = (oxmax - oxmin) / abs(xmax - xmin);
                            double xacum = oxmin + xstep;

                            double ystep = (oymax - oymin) / abs(xmax - xmin);
                            double yacum = oymin + ystep;

                            double xnstep = (xnmax - xnmin) / abs(xmax - xmin);
                            double xnacum = xnmin + xnstep;
                            double ynstep = (ynmax - ynmin) / abs(xmax - xmin);
                            double ynacum = ynmin + ynstep;
                            double znstep = (znmax - znmin) / abs(xmax - xmin);
                            double znacum = znmin + znstep;

                            for (int cnt = xmin+1; cnt < xmax; cnt++)
                            {
                                pixelMaterial(cnt, yRev, xacum, yacum, acumuladoz, obj.getRGB(), materialesObj[c], {xnacum, ynacum, znacum});
                                acumuladoz += stepz;
                                xacum += xstep;
                                yacum += ystep;
                                xnacum += xnstep;
                                ynacum += ynstep;
                                znacum += znstep;
                            }
                        }
                        //Change yrev 
                        yRev = toPaint[i][1];
                        bulk.clear();
                        bulk.push_back(toPaint[i]);
                    }
                }
                //Paint last bulk
                int xmin = rx;
                int xmax = -1;
                double zmin, zmax, oxmin, oxmax, oymin, oymax;
                double xnmin, xnmax, ynmin, ynmax, znmin, znmax; // Interpolando las normales
                for (int i2 = 0; i2 < bulk.size(); i2++)
                {
                    if (xmin > bulk[i2][0])
                    {
                        xmin = bulk[i2][0];
                        zmin = bulk[i2][2];
                        oxmin = bulk[i2][3];
                        oymin = bulk[i2][4];
                        xnmin = bulk[i2][5];
                        ynmin = bulk[i2][6];
                        znmin = bulk[i2][7];
                    }
                    if (xmax < bulk[i2][0])
                    {
                        xmax = bulk[i2][0];
                        zmax = bulk[i2][2];
                        oxmax = bulk[i2][3];
                        oymax = bulk[i2][4];
                        xnmax = bulk[i2][5];
                        ynmax = bulk[i2][6];
                        znmax = bulk[i2][7];
                    }
                }
                pixelMaterial(xmin, yRev, oxmin, oymin, zmin, obj.getRGB(), materialesObj[c], {xnmin, ynmin, znmin});
                if (xmin != xmax)
                {
                    pixelMaterial(xmax, yRev, oxmax, oymax, zmax, obj.getRGB(), materialesObj[c], {xnmax, ynmax, znmax});

                    double stepz = (zmax - zmin) / abs(xmax - xmin);
                    double acumuladoz = zmin + stepz;

                    double xstep = (oxmax - oxmin) / abs(xmax - xmin);
                    double xacum = oxmin + xstep;

                    double ystep = (oymax - oymin) / abs(xmax - xmin);
                    double yacum = oymin + ystep;

                    double xnstep = (xnmax - xnmin) / abs(xmax - xmin);
                    double xnacum = xnmin + xnstep;
                    double ynstep = (ynmax - ynmin) / abs(xmax - xmin);
                    double ynacum = ynmin + ynstep;
                    double znstep = (znmax - znmin) / abs(xmax - xmin);
                    double znacum = znmin + znstep;

                    for (int cnt = xmin + 1; cnt < xmax; cnt++)
                    {
                        pixelMaterial(cnt, yRev, xacum, yacum, acumuladoz, obj.getRGB(), materialesObj[c], {xnacum, ynacum, znacum});
                        acumuladoz += stepz;
                        xacum += xstep;
                        yacum += ystep;
                        xnacum += xnstep;
                        ynacum += ynstep;
                        znacum += znstep;
                    }
                }
            }
        }
        return;
    }

    // Illumination functions
    void lightAttenuate(Luz lightSource)
    {
        vector<double> lightPosition = lightSource.getPosition();
        vector<int> lightColor = lightSource.getColor();
        vector<double> attenVar = lightSource.getAttenVar();
        for (int iy = 0; iy < ry; iy++)
        {
            for (int ix = 0; ix < rx; ix++)
            {
                if(pixelCoordinate[ix][iy][0] != (double)INFINITY && pixelCoordinate[ix][iy][1] != (double)INFINITY)
                {
                    vector<double> pixpos = vector<double>(3,0);
                    pixpos[0] = pixelCoordinate[ix][iy][0];
                    pixpos[1] = pixelCoordinate[ix][iy][1];
                    pixpos[2] = z_buffer[ix][iy];
                    double angle = 0;
                    double coneAngle = 10;
                    if(lightSource.getType() == 1)
                    {
                        vector<double> paraAngulo = restarVector(pixpos, lightPosition);
                        vector<double> dir = lightSource.getDirection();
                        angle = acos((dotProd( paraAngulo, dir))/(magnitud(paraAngulo)*magnitud(dir))) * (180 / M_PI);
                        coneAngle = lightSource.getAngle();
                    }
                    if(angle < coneAngle)
                    {
                        double d = magnitud(restarVector(pixpos,lightPosition));
                        double attenuation = 1 / ((attenVar[0] * d * d) + (attenVar[1] * d) + attenVar[2]);
                        vector<int> newColor = vector<int>(3,0);
                        newColor[0] = image[ix][iy][0] + lightColor[0] * attenuation; //r
                        newColor[1] = image[ix][iy][1] + lightColor[1] * attenuation; //g
                        newColor[2] = image[ix][iy][2] + lightColor[2] * attenuation; //b
                        for(int cnt=0; cnt < 3; cnt++)
                            if(newColor[cnt] > 255) newColor[cnt] = 255;
                        image[ix][iy][0] = newColor[0]; 
                        image[ix][iy][1] = newColor[1]; 
                        image[ix][iy][2] = newColor[2]; 
                    }
                }
            }
        }
    }
    void lightAttenuate(vector<Luz> lightList)
    {
        for (int iy = 0; iy < ry; iy++)
        {
            for (int ix = 0; ix < rx; ix++)
            {
                if (pixelCoordinate[ix][iy][0] != (double)INFINITY && pixelCoordinate[ix][iy][1] != (double)INFINITY)
                {
                    vector<double> lightPosition, attenVar, pixpos;
                    vector<int> lightColor;
                    pixpos = vector<double>(3, 0);
                    pixpos[0] = pixelCoordinate[ix][iy][0];
                    pixpos[1] = pixelCoordinate[ix][iy][1];
                    pixpos[2] = z_buffer[ix][iy];
                    vector<int> newColor = vector<int>(3, 0);
                    newColor[0] = image[ix][iy][0];
                    newColor[1] = image[ix][iy][1];
                    newColor[2] = image[ix][iy][2];
                    for(Luz lightSource : lightList)
                    {
                        lightPosition = lightSource.getPosition();
                        lightColor = lightSource.getColor();
                        attenVar = lightSource.getAttenVar();
                        double angle = 0;
                        double coneAngle = 10;
                        if(lightSource.getType() == 1)
                        {
                            vector<double> paraAngulo = restarVector(pixpos, lightPosition);
                            vector<double> dir = lightSource.getDirection();
                            angle = acos((dotProd( paraAngulo, dir))/(magnitud(paraAngulo)*magnitud(dir))) * (180 / M_PI);
                            coneAngle = lightSource.getAngle();
                        }
                        if(angle < coneAngle)
                        {
                            double d = magnitud(restarVector(pixpos, lightPosition));
                            double attenuation = 1 / ((attenVar[0] * d * d) + (attenVar[1] * d) + attenVar[2]);
                            newColor[0] += lightColor[0] * attenuation; //r
                            newColor[1] += lightColor[1] * attenuation; //g
                            newColor[2] += lightColor[2] * attenuation; //b
                        }
                    }
                    for (int cnt = 0; cnt < 3; cnt++)
                        if (newColor[cnt] > 255)
                            newColor[cnt] = 255;
                    image[ix][iy][0] = newColor[0];
                    image[ix][iy][1] = newColor[1];
                    image[ix][iy][2] = newColor[2];
                }
            }
        }
    }
    void lightAmbientLight() { lightAmbientLight(ambientLight); }
    void lightAmbientLight(vector<int> aL) 
    {
        for (int iy = 0; iy < ry; iy++)
        {
            for (int ix = 0; ix < rx; ix++)
            {
                if (pixelCoordinate[ix][iy][0] != (double)INFINITY && pixelCoordinate[ix][iy][1] != (double)INFINITY)
                {
                    vector<int> newColor = vector<int>(3, 0);
                    newColor[0] = image[ix][iy][0] + aL[0]; //r
                    newColor[1] = image[ix][iy][1] + aL[1]; //g
                    newColor[2] = image[ix][iy][2] + aL[2]; //b
                    for (int cnt = 0; cnt < 3; cnt++)
                        if (newColor[cnt] > 255)
                            newColor[cnt] = 255;
                    image[ix][iy][0] = newColor[0];
                    image[ix][iy][1] = newColor[1];
                    image[ix][iy][2] = newColor[2];
                }
            }
        }
    }
    void lightScene(vector<Luz> light) { lightScene(light, ambientLight); }
    void lightScene(vector<Luz> lightList, vector<int> aL)
    {
        for (int iy = 0; iy < ry; iy++)
        {
            for (int ix = 0; ix < rx; ix++)
            {
                if (pixelCoordinate[ix][iy][0] != (double)INFINITY && pixelCoordinate[ix][iy][1] != (double)INFINITY)
                {
                    vector<double> lightPosition, attenVar, pixpos;
                    vector<int> lightColor;
                    pixpos = vector<double>(3, 0);
                    pixpos[0] = pixelCoordinate[ix][iy][0];
                    pixpos[1] = pixelCoordinate[ix][iy][1];
                    pixpos[2] = z_buffer[ix][iy];
                    vector<int> newColor = vector<int>(3, 0);
                    newColor[0] = image[ix][iy][0];
                    newColor[1] = image[ix][iy][1];
                    newColor[2] = image[ix][iy][2];
                    for (Luz lightSource : lightList)
                    {
                        lightPosition = lightSource.getPosition();
                        lightColor = lightSource.getColor();
                        attenVar = lightSource.getAttenVar();
                        double angle = 0;
                        double coneAngle = 10;
                        if (lightSource.getType() == 1)
                        {
                            vector<double> paraAngulo = restarVector(pixpos, lightPosition);
                            vector<double> dir = lightSource.getDirection();
                            angle = acos((dotProd(paraAngulo, dir)) / (magnitud(paraAngulo) * magnitud(dir))) * (180 / M_PI);
                            coneAngle = lightSource.getAngle();
                        }
                        if (angle < coneAngle)
                        {
                            double d = magnitud(restarVector(pixpos, lightPosition));
                            double attenuation = 1 / ((attenVar[0] * d * d) + (attenVar[1] * d) + attenVar[2]);
                            newColor[0] += lightColor[0] * attenuation; //r
                            newColor[1] += lightColor[1] * attenuation; //g
                            newColor[2] += lightColor[2] * attenuation; //b
                        }
                    }
                    //Ambient Light
                    newColor[0] += aL[0]; //r
                    newColor[1] += aL[1]; //g
                    newColor[2] += aL[2]; //b
                    for (int cnt = 0; cnt < 3; cnt++)
                        if (newColor[cnt] > 255)
                            newColor[cnt] = 255;
                    image[ix][iy][0] = newColor[0];
                    image[ix][iy][1] = newColor[1];
                    image[ix][iy][2] = newColor[2];
                }
            }
        }
    }
    // With material
    void lightSceneMaterials(vector<Luz> lightList)
    {
        vector<double> vcam(3, 0); //Vector de la camara
        if (camaraEje == 'x')
            vcam[0] = 1;
        else if (camaraEje == 'y')
            vcam[1] = 1;
        else
            vcam[2] = 1;
        for (int iy = 0; iy < ry; iy++)
        {
            for (int ix = 0; ix < rx; ix++)
            {
                if (pixelCoordinate[ix][iy][0] != (double)INFINITY && pixelCoordinate[ix][iy][1] != (double)INFINITY)
                {
                    Material mt = pixelMat[ix][iy];
                    vector<double> pnormal = pixelNormal[ix][iy];
                    double ak, dk, sk, ns;
                    ak = mt.getAmbientK();
                    dk = mt.getDiffuseK();
                    sk = mt.getSpecularK();
                    ns = mt.getSpecularAngle();

                    vector<double> lightPosition, attenVar, pixpos;
                    vector<int> lightColor;
                    pixpos = vector<double>(3, 0);
                    pixpos[0] = pixelCoordinate[ix][iy][0];
                    pixpos[1] = pixelCoordinate[ix][iy][1];
                    pixpos[2] = z_buffer[ix][iy];
                    vector<int> newColor = vector<int>(3, 0);
                    newColor[0] = image[ix][iy][0];
                    newColor[1] = image[ix][iy][1];
                    newColor[2] = image[ix][iy][2];

                    for (Luz lightSource : lightList)
                    {
                        lightPosition = lightSource.getPosition();
                        lightColor = lightSource.getColor();
                        attenVar = lightSource.getAttenVar();
                        double angle = 0;
                        double coneAngle = 10;
                        vector<double> dir;

                        if (lightSource.getType() == 1)
                        {
                            vector<double> paraAngulo = restarVector(pixpos, lightPosition);
                            dir = lightSource.getDirection();
                            angle = acos((dotProd(paraAngulo, dir)) / (magnitud(paraAngulo) * magnitud(dir))) * (180 / M_PI);
                            coneAngle = lightSource.getAngle();
                        }
                        else
                        {
                            dir = restarVector(pixpos, lightPosition);
                        }
                        

                        if (angle < coneAngle)
                        {
                            //Check if light actually hits face
                            angle = acos((dotProd(pnormal, dir)) / (magnitud(pnormal) * magnitud(dir))) * (180 / M_PI);
                            //
                            if(180 >= angle && angle >= 90)
                            { //Show it! 
                                vector<double> reflejo = reflejoVector(dir, pnormal);
                                // Diffuse
                                double diffAngle = acos((dotProd(pnormal, reflejo)) / (magnitud(pnormal) * magnitud(reflejo)));
                                double diffValue = dk * cos(diffAngle);
                                if(diffValue < 0 ) 
                                    diffValue = 0;
                                // Specular
                                double specAngle = acos((dotProd(vcam, reflejo)) / (magnitud(vcam) * magnitud(reflejo)));
                                double specValue = sk * pow(cos(specAngle), ns);
                                // Lightning
                                double d = magnitud(restarVector(pixpos, lightPosition));
                                double attenuation = 1 / ((attenVar[0] * d * d) + (attenVar[1] * d) + attenVar[2]);
                                // Version 1 
                                for(int i=0 ; i < 3; i++)
                                {
                                    newColor[i] += lightColor[i] * attenuation * diffValue;
                                    newColor[i] += lightColor[i] * attenuation * specValue;
                                }
                                /* Version 2
                                for (int i = 0; i < 3; i++)
                                {
                                    newColor[i] += lightColor[i] * diffValue;
                                    newColor[i] += lightColor[i] * specValue;
                                    newColor[i] += lightColor[i] * attenuation;
                                }
                                //*/
                            }
                        }
                    }
                    //Ambient Light
                    newColor[0] += ak * ambientLight[0]; //r
                    newColor[1] += ak * ambientLight[1]; //g
                    newColor[2] += ak * ambientLight[2]; //b
                    for (int cnt = 0; cnt < 3; cnt++)
                    {
                        if (newColor[cnt] > 255)
                            newColor[cnt] = 255;
                        if (newColor[cnt] < 0)
                            newColor[cnt] = 0;
                    }
                    image[ix][iy][0] = newColor[0];
                    image[ix][iy][1] = newColor[1];
                    image[ix][iy][2] = newColor[2];
                }
            }
        }
    }

    // Flat & Gouraud & Phong final functions
    void paintWireFrameScene(vector<Object3D> sceneObjs, bool hideFaces)
    {
        for(Object3D obj : sceneObjs)
            paint3D(obj, hideFaces);
    }
    void paintFlatScene(vector<Object3D> sceneObjs, vector<Luz> sceneLights)
    {
        for(Object3D obj : sceneObjs)
            paint3DWithMaterial(obj);
        lightSceneMaterials(sceneLights);
    }
    void paintPhongScene(vector<Object3D> sceneObjs, vector<Luz> sceneLights)
    {
        for (Object3D obj : sceneObjs)
            paint3DPhong(obj);
        lightSceneMaterials(sceneLights);
    }
    void paintGouraudScene(vector<Object3D> sceneObjs, vector<Luz> sceneLights)
    {
        vector<double> vcam(3, 0); //Vector de la camara
        int ejeX = 0, ejeY = 1, ejeZ = 2; //Suponiendo 'z'
        if (camaraEje == 'x')
        {
            ejeX = 2;
            ejeY = 1;
            ejeZ = 0;
            vcam[0] = 1;
        }
        else if (camaraEje == 'y')
        {
            ejeY = 2;
            ejeZ = 1;
            vcam[1] = 1;
        }
        else
            vcam[2] = 1;
            
        for(Object3D objeto : sceneObjs)
        {
            Object3D obj = objeto;
            if (!obj.hasNormales())
            {
                obj.convertirVLFNormal();
            }
            vector<vector<double>> normalesObj = obj.getNormales();

            if (!camaraOrtogonal)
            {
                // Proyectar
                Transformacion proyeccion;
                vector<vector<double>> tabla(4, vector<double>(4, 0));
                for (int i = 0; i < 3; i++)
                    tabla[i][i] = 1;
                if (camaraEje == 'x')
                    tabla[3][0] = 1 / f;
                else if (camaraEje == 'y')
                    tabla[3][1] = 1 / f;
                else
                    tabla[3][2] = 1 / f;
                proyeccion.setTabla(tabla);
                obj.transformar(proyeccion);
                obj.mover(rx / 2, ry / 2, 0); //Mover proyeccion al centro de la imagen
                obj.regresarHomogeneo();
            }
            vector<vector<double>> puntosObj = obj.getPuntos();
            vector<vector<double>> ladosObj = obj.getLados();
            vector<vector<double>> carasObj = obj.getCaras();
            vector<vector<double>> puntosOriginales = objeto.getPuntos();
            vector<Material> materialesObj = obj.getMateriales();
            //Calculate vertex Normals
            if (!objeto.hasVertexNormals())
                objeto.convertirVLFVertexNormal();
            vector<vector<double>> vertexNormal = objeto.getVertexNormals();
            for (int c = 0; c < carasObj.size(); c++)
            {
                double angle = acos((dotProd(normalesObj[c], vcam)) / magnitud(normalesObj[c])) * (180 / M_PI); //La camara estan normalizados.
                //checar angulo
                if (180 >= angle && angle >= 90) //Show it!
                {
                    vector<vector<double>> toPaint;
                    for (int l = 0; l < 3; l++)
                    {
                        int x1, y1, x2, y2;
                        double z1, z2;
                        double ox1, ox2, oy1, oy2; //o stands for original (before projecting)
                        x1 = round(puntosObj[ladosObj[carasObj[c][l] - 1][0] - 1][ejeX]);
                        y1 = round(puntosObj[ladosObj[carasObj[c][l] - 1][0] - 1][ejeY]);
                        z1 = puntosOriginales[ladosObj[carasObj[c][l] - 1][0] - 1][ejeZ];
                        x2 = round(puntosObj[ladosObj[carasObj[c][l] - 1][1] - 1][ejeX]);
                        y2 = round(puntosObj[ladosObj[carasObj[c][l] - 1][1] - 1][ejeY]);
                        z2 = puntosOriginales[ladosObj[carasObj[c][l] - 1][1] - 1][ejeZ];

                        ox1 = puntosOriginales[ladosObj[carasObj[c][l] - 1][0] - 1][ejeX];
                        oy1 = puntosOriginales[ladosObj[carasObj[c][l] - 1][0] - 1][ejeY];
                        ox2 = puntosOriginales[ladosObj[carasObj[c][l] - 1][1] - 1][ejeX];
                        oy2 = puntosOriginales[ladosObj[carasObj[c][l] - 1][1] - 1][ejeY];

                        vector<double> vn1 = vertexNormal[ladosObj[carasObj[c][l] - 1][0] - 1];
                        vector<double> vn2 = vertexNormal[ladosObj[carasObj[c][l] - 1][1] - 1];
                        
                        // Get vertex colors 
                        Material mt = obj.getMateriales()[c]; // Face material

                        double ak, dk, sk, ns;
                        ak = mt.getAmbientK();
                        dk = mt.getDiffuseK();
                        sk = mt.getSpecularK();
                        ns = mt.getSpecularAngle();

                        vector<double> lightPosition, attenVar, pixpos, pixpos2;
                        vector<int> lightColor;
                        pixpos = vector<double>(3, 0);
                        pixpos2 = vector<double>(3,0);
                        pixpos[0] = ox1;
                        pixpos[1] = oy1;
                        pixpos[2] = z1;
                        pixpos2[0] = ox2;
                        pixpos2[1] = oy2;
                        pixpos2[2] = z2;
                        vector<double> newColorV1 = vector<double>(3,0);
                        vector<double> newColorV2 = vector<double>(3,0);
                        vector<int> colorOriginal = obj.getRGB();
                        for(int d=0; d<3; d++)
                        {
                            newColorV1[d] = (colorOriginal[d]);
                            newColorV2[d] = (colorOriginal[d]);
                        }
                        

                        for (Luz lightSource : sceneLights)
                        {
                            lightPosition = lightSource.getPosition();
                            lightColor = lightSource.getColor();
                            attenVar = lightSource.getAttenVar();
                            double angle = 0;
                            double coneAngle = 10;
                            vector<double> dir;
                            bool v1_in = false, v2_in = false; // Is the vertex inside the cone?
                            if (lightSource.getType() == 1)
                            {
                                vector<double> paraAngulo = restarVector(pixpos, lightPosition);
                                dir = lightSource.getDirection();
                                angle = acos((dotProd(paraAngulo, dir)) / (magnitud(paraAngulo) * magnitud(dir))) * (180 / M_PI);
                                coneAngle = lightSource.getAngle();
                                if (angle < coneAngle) v1_in = true;
                                paraAngulo = restarVector(pixpos2, lightPosition);
                                angle = acos((dotProd(paraAngulo, dir)) / (magnitud(paraAngulo) * magnitud(dir))) * (180 / M_PI);
                                if (angle < coneAngle) v2_in = true;
                            }
                            else
                            {
                                dir = restarVector(pixpos, lightPosition);
                                v1_in = true;
                                v2_in = true;
                            }
                            // Pain vertex 1
                            if (v1_in)
                            {
                                //Check if light actually hits vertex
                                angle = acos((dotProd(vn1, dir)) / (magnitud(vn1) * magnitud(dir))) * (180 / M_PI);
                                //
                                if(180 >= angle && angle >= 90)
                                { //Show it! 
                                    vector<double> reflejo = reflejoVector(dir, vn1);
                                    // Diffuse
                                    double diffAngle = acos((dotProd(vn1, reflejo)) / (magnitud(vn1) * magnitud(reflejo)));
                                    double diffValue = dk * cos(diffAngle);
                                    if(diffValue < 0 ) 
                                        diffValue = 0;
                                    // Specular
                                    double specAngle = acos((dotProd(vcam, reflejo)) / (magnitud(vcam) * magnitud(reflejo)));
                                    double specValue = sk * pow(cos(specAngle),ns);
                                    // Lightning
                                    double d = magnitud(restarVector(pixpos, lightPosition));
                                    double attenuation = 1 / ((attenVar[0] * d * d) + (attenVar[1] * d) + attenVar[2]);
                                    // Version 1 
                                    for(int i=0 ; i < 3; i++)
                                    {
                                        newColorV1[i] += lightColor[i] * attenuation * diffValue;
                                        newColorV1[i] += lightColor[i] * attenuation * specValue;
                                    }
                                }
                            }
                            // Paint vertex 2
                            if (v2_in)
                            {
                                //Check if light actually hits vertex
                                angle = acos((dotProd(vn2, dir)) / (magnitud(vn2) * magnitud(dir))) * (180 / M_PI);
                                //
                                if(180 >= angle && angle >= 90)
                                { //Show it! 
                                    vector<double> reflejo = reflejoVector(dir, vn2);
                                    // Diffuse
                                    double diffAngle = acos((dotProd(vn2, reflejo)) / (magnitud(vn2) * magnitud(reflejo)));
                                    double diffValue = dk * cos(diffAngle);
                                    if(diffValue < 0 ) 
                                        diffValue = 0;
                                    // Specular
                                    double specAngle = acos((dotProd(vcam, reflejo)) / (magnitud(vcam) * magnitud(reflejo)));
                                    double specValue = sk * pow(cos(specAngle),ns);
                                    // Lightning
                                    double d = magnitud(restarVector(pixpos2, lightPosition));
                                    double attenuation = 1 / ((attenVar[0] * d * d) + (attenVar[1] * d) + attenVar[2]);
                                    // Version 1 
                                    for(int i=0 ; i < 3; i++)
                                    {
                                        newColorV2[i] += lightColor[i] * attenuation * diffValue;
                                        newColorV2[i] += lightColor[i] * attenuation * specValue;
                                    }
                                }
                            }
                        }
                        //Ambient Light
                        newColorV1[0] += ak * ambientLight[0]; //r
                        newColorV1[1] += ak * ambientLight[1]; //g
                        newColorV1[2] += ak * ambientLight[2]; //b
                        newColorV2[0] += ak * ambientLight[0]; //r
                        newColorV2[1] += ak * ambientLight[1]; //g
                        newColorV2[2] += ak * ambientLight[2]; //b
                        for (int cnt = 0; cnt < 3; cnt++)
                        {
                            if (newColorV1[cnt] > 255)
                                newColorV1[cnt] = 255;
                            if (newColorV2[cnt] > 255)
                                newColorV2[cnt] = 255;
                        }
                        //
                        vector<vector<double>> puntos = drawLineBresenhamPointsGouraud(x1, y1, z1, x2, y2, z2, newColorV1, newColorV2);
                        // COn esto ya tenemos en [4][5][6] el color del pixel interpolado
                        for (vector<double> p : puntos)
                            toPaint.push_back(p);
                    }
                    //Burbuja Sort
                    int i, j;
                    for (i = 0; i < toPaint.size() - 1; i++)
                    {
                        for (j = 0; j < toPaint.size() - i - 1; j++)
                        {
                            if (toPaint[j][1] > toPaint[j + 1][1])
                            {
                                vector<double> aux = toPaint[j];
                                toPaint[j] = toPaint[j + 1];
                                toPaint[j + 1] = aux;
                            }
                        }
                    }
                    int yRev = toPaint[0][1];
                    vector<vector<double>> bulk;
                    for (int i = 0; i < toPaint.size(); i++)
                    {
                        if (yRev == toPaint[i][1])
                        {
                            bulk.push_back(toPaint[i]);
                        }
                        else
                        {
                            //Paint points
                            int xmin = rx;
                            int xmax = -1;
                            double zmin, zmax;
                            double xnmin, xnmax, ynmin, ynmax, znmin, znmax; // Interpolando los colores
                            for (int i2 = 0; i2 < bulk.size(); i2++)
                            {
                                if (xmin > bulk[i2][0])
                                {
                                    xmin = bulk[i2][0];
                                    zmin = bulk[i2][2];
                                    xnmin = bulk[i2][3];
                                    ynmin = bulk[i2][4];
                                    znmin = bulk[i2][5];
                                }
                                if (xmax < bulk[i2][0])
                                {
                                    xmax = bulk[i2][0];
                                    zmax = bulk[i2][2];
                                    xnmax = bulk[i2][3];
                                    ynmax = bulk[i2][4];
                                    znmax = bulk[i2][5];
                                }
                            }
                            pixel(xmin, yRev, zmin, {(int) xnmin, (int) ynmin, (int) znmin});
                            if (xmin != xmax)
                            {
                                pixel(xmax, yRev, zmax, {(int)xnmax, (int)ynmax, (int)znmax});

                                double stepz = (zmax - zmin) / abs(xmax - xmin);
                                double acumuladoz = zmin + stepz;

                                double xnstep = (xnmax - xnmin) / abs(xmax - xmin);
                                double xnacum = xnmin + xnstep; //R
                                double ynstep = (ynmax - ynmin) / abs(xmax - xmin);
                                double ynacum = ynmin + ynstep; //G
                                double znstep = (znmax - znmin) / abs(xmax - xmin);
                                double znacum = znmin + znstep; //B

                                for (int cnt = xmin + 1; cnt < xmax; cnt++)
                                {
                                    pixel(cnt, yRev, acumuladoz, {(int)xnacum, (int)ynacum, (int)znacum});
                                    acumuladoz += stepz;
                                    xnacum += xnstep;
                                    ynacum += ynstep;
                                    znacum += znstep;
                                }
                            }
                            //Change yrev
                            yRev = toPaint[i][1];
                            bulk.clear();
                            bulk.push_back(toPaint[i]);
                        }
                    }
                    //Paint last bulk
                    int xmin = rx;
                    int xmax = -1;
                    double zmin, zmax;
                    double xnmin, xnmax, ynmin, ynmax, znmin, znmax; // Interpolando los colores
                    for (int i2 = 0; i2 < bulk.size(); i2++)
                    {
                        if (xmin > bulk[i2][0])
                        {
                            xmin = bulk[i2][0];
                            zmin = bulk[i2][2];
                            xnmin = bulk[i2][3];
                            ynmin = bulk[i2][4];
                            znmin = bulk[i2][5];
                        }
                        if (xmax < bulk[i2][0])
                        {
                            xmax = bulk[i2][0];
                            zmax = bulk[i2][2];
                            xnmax = bulk[i2][3];
                            ynmax = bulk[i2][4];
                            znmax = bulk[i2][5];
                        }
                    }
                    pixel(xmin, yRev, zmin, {(int) xnmin, (int) ynmin, (int) znmin});
                    if (xmin != xmax)
                    {
                        pixel(xmax, yRev, zmax, {(int)xnmax, (int)ynmax, (int)znmax});

                        double stepz = (zmax - zmin) / abs(xmax - xmin);
                        double acumuladoz = zmin + stepz;

                        double xnstep = (xnmax - xnmin) / abs(xmax - xmin);
                        double xnacum = xnmin + xnstep; //R
                        double ynstep = (ynmax - ynmin) / abs(xmax - xmin);
                        double ynacum = ynmin + ynstep; //G
                        double znstep = (znmax - znmin) / abs(xmax - xmin);
                        double znacum = znmin + znstep; //B

                        for (int cnt = xmin + 1; cnt < xmax; cnt++)
                        {
                            pixel(cnt, yRev, acumuladoz, {(int)xnacum, (int)ynacum, (int)znacum});
                            acumuladoz += stepz;
                            xnacum += xnstep;
                            ynacum += ynstep;
                            znacum += znstep;
                        }
                    }
                }
            }
        }
        return;
    }

    // Final functions
    void make(string s)
    {
        ofstream imagen(s + ".ppm");
        imagen << "P6\n"
            << to_string(rx) << " " << to_string(ry) << " 255\n";
        for (int iy = 0; iy < ry; iy++)
            for (int ix = 0; ix < rx; ix++)
                imagen << image[ix][iy][0] << image[ix][iy][1] << image[ix][iy][2];
        imagen.close();
        cout << "Imagen creada!\n";
    }
};
