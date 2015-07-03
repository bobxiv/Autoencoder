#ifndef PML_H
#include "vec.h"
#include "utiles.h"
using namespace std;
#define PML_H

class PML {
	friend class Encoder;
private:
	vector< vector<vec> > w;
	unsigned int nin;
	double (*umbral)(double);
public:
	PML(unsigned int n, unsigned int ncapas, int* perporlayer, double (*u)(double));
	PML() : w(), nin(0), umbral(NULL) {};
	~PML();
	double entrenar(vector<vec> P,unsigned int limitsteps, double tol, double factor, double momento);
	void Partir(PML* &coder, PML* &decoder);
	bool eval(vec, vec&, bool biased=false) const;
	bool printWeight();
	unsigned int eval(const vec& entrada, bool biased=false) const;
	void forget();
	bool save(string name);
	bool load(string name);
};

#endif

