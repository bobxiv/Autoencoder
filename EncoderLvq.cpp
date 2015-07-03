#include "EncoderLvq.h"
using namespace std;

//No inicializa el Code y Decoder, sino que se crean en Iniciar
EncoderLvq::EncoderLvq() {
	
}

EncoderLvq::~EncoderLvq() {
	
}

//Utiliza el Coder para codificar la entrada in y sacarla en out
//Parametros:
//- in  tupla de datos a codificar
//- out tupla de datos codificados
bool EncoderLvq::Comprimir(const vector<vec>& in, vector<int>& out) {
	double dmin=DBL_MAX, d;
	int i, j, jmin;
	out.resize(in.size());
	for(i=0; i<in.size(); i++) {
		for(j=0; j<CodeBook.size(); j++) {
			d=in[i].dist2(CodeBook[j]);
			if(d < dmin) {
				dmin=d;
				jmin=j;
			}
		}
		out[i]=jmin;
	}
	return true;
}

//Utiliza el Decoder para decodificar la entrada in y sacarla en out
//Parametros:
//- in  tupla de datos a decodificar
//- out tupla de datos decodificados
bool EncoderLvq::Descomprimir(const vector<int>& in, vector<vec>& out) {
	out.resize(in.size());
	for(int i=0; i<in.size(); i++) {
		out[i] = CodeBook[in[i]];
	}
	return true;
}

//Inicializa el Coder y el Decoder, creandolos para los patrones Patrones
//Parametros:
//- Patrones  patrones que desea poder ser codificado y decodificado
//- nin       cantidad de patrones en la primera y ultima capa
double EncoderLvq::Iniciar(vector<vec> Patrones, unsigned int ls, double fa, double fc) {
	tic();
	CodeBook.resize( Patrones.size() * fc, vec(Patrones[0].s, false));
	int i, j, jmin;
	double d, dmin, error;
	while( ls-- ) {
		error=0;
		for(i=0; i<Patrones.size(); i++) {
			vec& patron=Patrones[i];
			for(j=0; j<CodeBook.size(); j++) {
				d=patron.dist2(CodeBook[j]);
				if(d < dmin) {
					dmin=d;
					jmin=j;
				}
				error+=d;
			}
			for(j=0; j<CodeBook.size(); j++) {
				if(j == jmin)
					CodeBook[j] += (patron-CodeBook[j])*fa;
				else
					CodeBook[j] -= (patron-CodeBook[j])*fa;
			}
		}
		error/=Patrones.size();
		//Mostramos el error
		cout<<"Epoca: "<<i<<"  ->  Error: "<<error<<endl;
	}
	cout<<"Tiempo tardado: "<<toc()<<endl;
	return error;
}


