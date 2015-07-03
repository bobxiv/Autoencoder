#include<iostream>
#include<fstream>
#include<algorithm>
#include<vector>
#include <cstring>
#include"Encoder.h"
#include <png++/png.hpp>
#include <cstdlib>
#include "EncoderLvq.h"
using namespace std;

#define yuvSpace
#define lvq



void LoadImage(vector<vec>& out, int& w, int& h, int rad, string name) {
	name.append(".png");
	png::image< png::rgb_pixel > image(name.c_str());
	w=image.get_width();
	h=image.get_height();
	w-=w%rad;
	h-=h%rad;
	int k=0;
	int rad2=rad*rad;
	out.resize(h*w/(rad2),vec(rad2*3));
	double r,g,b,y,u,v;
	for(int i=h-1; i>=0; i-=rad) {
		for(int j=w-1; j>=0; j-=rad) {
			vec& outk=out[k++];
			for(int l=0; l<rad2; ++l) {
#ifdef yuvSpace
				png::rgb_pixel& pixel=image[i-(l%rad)][j-(l/rad)];
				r=double(pixel.red)/255;
				g=double(pixel.green)/255;
				b=double(pixel.blue)/255;
				y = 0.299*r + 0.587*g + 0.114*b;
				u = 0.564333333*(b-y);
				v = 0.71327*(r-y);
				outk[3*l]=y*y*2-1;
				outk[3*l+1]=u;
				outk[3*l+2]=v;
#else
				outk[3*l]=(double(pixel.red)/127.5 - 1;
				outk[3*l+1]=(double(pixel.green)/127.5 - 1;
				outk[3*l+2]=(double(pixel.blue)/127.5 - 1;
#endif
			}
		}
	}
}

void SaveImage(vector<vec>& in, int w, int h, int rad, string name) {
	name.append(".png");
	png::image< png::rgb_pixel > image(w,h);
	int k=0;
	int rad2=rad*rad;
	double r,g,b,y,u,v;
	for(int i=h-1; i>=0; i-=rad) {
		for(int j=w-1; j>=0; j-=rad) {
			vec& ink=in[k++];
			for(int l=0; l<rad2; ++l) {
#ifdef yuvSpace
				y=sqrt((ink[3*l]+1)/2);
				u=ink[3*l+1]; 
				v=ink[3*l+2];
				r = y + 1.140*v;
				g = y - 0.395*u - 0.581*v;
				b = y + 2.032*u;
				
				r=(r>1)? 1 : r; r=(r<0)? 0 : r;
				g=(g>1)? 1 : g; g=(g<0)? 0 : g;
				b=(b>1)? 1 : b; b=(b<0)? 0 : b;
				r*=255.0;
				g*=255.0; 
				b*=255.0;
				
				image[i-(l%rad)][j-(l/rad)]=png::rgb_pixel(r, g, b);
#else				
				image[i-(l%rad)][j-(l/rad)]=png::rgb_pixel((ink[3*l]+1)*127.5, (ink[3*l+1]+1)*127.5, (ink[3*l+2]+1)*127.5);
#endif
			}
		}
	}
	image.write(name.c_str());
}

int main(int argc, char* argv[])
{
	int nc=3, ls=10;
	double fa=0.01, fm=0.3;
	int rad=3, nin=rad*rad*3;
	int* ppc = new int[nc];
	ppc[0]=nin*1.2;
	ppc[1]=5; 
	ppc[2]=nin;
	int w, h, i;

	ofstream resultado("Resultados.txt", ios::app);

	resultado<<endl<<"Comprimiendo con arquitectura:"<<endl<<"	Numero de capas: "<<nc<<endl;
	for(int i=0; i<nc; ++i)
		resultado<<"		Capa["<<i<<"] : "<<ppc[i]<<endl;
	resultado<<"Entrenando con:"<<endl<<"	Pasos: "<<ls<<endl;
	resultado<<"	Factor de aprendizaje: "<<fa<<endl<<"	Factor de momento: "<<fm<<endl;
	resultado<<"Tamaño de los patrones: "<<rad<<"x"<<rad<<endl;
	resultado<<"Factor de compresion: "<<double(ppc[nc/2])/double(nin)<<endl;
	char n='0';
	double error;
	for(i=0; i<6; ++i) {
		string name="Grandes/Prueba-";
		name+=n;
		vector<vec> entrada;
		LoadImage(entrada,w,h,rad,name.c_str());
		
#ifdef lvq
		EncoderLvq enc;
		vector<int> compData(entrada.size());
#else
		Encoder enc;
		vector<vec> compData(entrada.size());
#endif
		vector<vec> descompData(entrada.size());
		
		error = enc.Iniciar(entrada, 1000, 0.02, 0.2);
		resultado<<name<<"    ->   Error cuadratico medio: "<<error/(w*h)<<endl;
		
		enc.Comprimir(entrada, compData);
		enc.Descomprimir(compData, descompData);
		
		name="Salida/Salida-";
		name+=n;
		SaveImage(descompData, w, h, rad, name.c_str());
		++n;
	}
	resultado.close();
	delete[] ppc;
	return 0;
}
