#include "Encoder.h"


double sigmodea(double v) {
	return 2/(1+exp(-v))-1;
}

Encoder::~Encoder() {
	delete Coder;
	delete Decoder;
}

Encoder::Encoder(PML* c, PML* d) : Coder(c), Decoder(d) {}

Encoder::Encoder() : Coder(NULL), Decoder(NULL) {}

bool Encoder::Comprimir(const vector<vec>& in, vector<vec>& out) {
	if(Coder==NULL) return false;
	for(int j=in.size()-1; j>=0; --j)
		Coder->eval(in[j], out[j], false);
	return true;
}

bool Encoder::Descomprimir(const vector<vec>& in, vector<vec>& out) {
	if(Decoder==NULL) return false;
	for(int j=in.size()-1; j>=0; --j)
		Decoder->eval(in[j], out[j], false);
	return true;
}

double Encoder::Iniciar(vector<vec> Patrones, int nc, int *ppc, int ls, double fa, double fm) {
	//Arquitectura de red
	if(ls==-1) ls=1500000/Patrones.size();
	int nin=Patrones[0].s;
	double error;
	PML* nn = new PML(nin, nc, ppc, sigmodea);
	
	error=nn->entrenar(Patrones, ls/*iteraciones*/, 0/*tolerancia*/, fa/*aprendizaje*/, fm/*momento*/);
	
	nn->Partir(Coder, Decoder);//con la red entrenada se separan al codificador y
							   //al decodificador
	delete nn;
	return error;
}

