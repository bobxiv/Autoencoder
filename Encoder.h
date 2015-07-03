#include "PML.h"

double sigmodea(double v);

class Encoder {
private:
	PML* Coder;  //Sirve para codificar la informacion
	PML* Decoder;//Sirve para decodificar la informacion
public:
	
	//Inicializa el Coder y Encoder con los PML que se pasan como
	//parametro
	Encoder(PML* c, PML* d);
	
	//No inicializa el Code y Decoder, sino que se crean en Iniciar
	Encoder();
	
	~Encoder();
	
	//Utiliza el Coder para codificar la entrada in y sacarla en out
	//Parametros:
	//- in  tupla de datos a codificar
	//- out tupla de datos codificados
	bool Comprimir(const vector<vec>& in, vector<vec>& out);
	
	//Utiliza el Decoder para decodificar la entrada in y sacarla en out
	//Parametros:
	//- in  tupla de datos a decodificar
	//- out tupla de datos decodificados
	bool Descomprimir(const vector<vec>& in, vector<vec>& out);
	
	//Inicializa el Coder y el Decoder, creandolos para los patrones Patrones
	//Parametros:
	//- Patrones  patrones que desea poder ser codificado y decodificado
	//- nin       cantidad de patrones en la primera y ultima capa
	double Iniciar(vector<vec> Patrones, int nc, int *ppc, int ls=-1, double fa=0.01, double fm=0.3);
};
