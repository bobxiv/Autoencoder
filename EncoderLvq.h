#ifndef ENCODERLVQ_H
#include <vector>
#include <cfloat>
#include "vec.h"
using namespace std;
#define ENCODERLVQ_H

class EncoderLvq {
private:
	vector<vec> CodeBook;
public:
	//No inicializa el Code y Decoder, sino que se crean en Iniciar
	EncoderLvq();
	
	~EncoderLvq();
	
	//Utiliza el Coder para codificar la entrada in y sacarla en out
	//Parametros:
	//- in  tupla de datos a codificar
	//- out tupla de datos codificados
	bool Comprimir(const vector<vec>& in, vector<int>& out);
	
	//Utiliza el Decoder para decodificar la entrada in y sacarla en out
	//Parametros:
	//- in  tupla de datos a decodificar
	//- out tupla de datos decodificados
	bool Descomprimir(const vector<int>& in, vector<vec>& out);
	
	//Inicializa el Coder y el Decoder, creandolos para los patrones Patrones
	//Parametros:
	//- Patrones  patrones que desea poder ser codificado y decodificado
	//- nin       cantidad de patrones en la primera y ultima capa
	double Iniciar(vector<vec> Patrones, unsigned int ls, double fa, double fc);
};

#endif

