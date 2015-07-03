#include "PML.h"
#include "utiles.h"
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <cfloat>
using namespace std;

PML::PML(unsigned int n, unsigned int ncapas, int* perporlayer, double (*u)(double)) : w(ncapas) {
	srand(time(NULL));
	nin=n;
	umbral=u;
	w[0].resize(perporlayer[0]);
	unsigned int i, j, k;
	for(i=0; i<w[0].size(); i++) {
		w[0][i]=vec(nin+1);
		for(k=0; k<w[0][i].s; k++) w[0][i][k]=double(rand()) * 2 / double(RAND_MAX) - 1;
	}
	for(i=1; i<w.size(); i++) {
		w[i].resize( perporlayer[i] );
		for(j=0; j<w[i].size(); j++) {
			w[i][j]=vec(perporlayer[i-1]+1);
			for(k=0; k<w[i][j].s; k++) w[i][j][k]=double(rand()) * 2 / double(RAND_MAX) - 1;
		}
	}
}

PML::~PML() {
	
}


double PML::entrenar(vector<vec> P,unsigned int limitsteps, double tol, double factor, double momento) {
	tic();
	
	int i, j, k, ipatron, ipm;
	unsigned int nc=w.size(), nc1=nc-1, epoca;
	double error;
	vector<vec> des(P.size());
	vector<int> map(P.size());
	for(i=P.size()-1;i>=0; i--) { 
		map[i]=i;
		des[i]=P[i];
		P[i].push_begin(-1);
	}
	//vector de salidas de cada capa
	vector< vec > y(nc);
	vector< vec > delta(nc);
	for(i=nc1-1; i>=0; i--) {
		delta[i].alloc(w[i].size(), false);
		y[i].alloc(w[i].size()+1, false);
		//No cambian en las distintas epocas, es el bias
		y[i][0]=-1;
	}
	y[nc1].alloc(w[nc1].size(),false);
	//vector de deltaw. Se usa para el termino de momento
	vector< vector <vec> > dw(w.size());
	for(i=dw.size()-1; i>=0; i--) dw[i] = vector<vec>(w[i].size(), vec(w[i][0].s));
	
	//algunas referencias para acceder mas rapido
	const vec& y0=y[0];
	const vector<vec>& w0=w[0];
	vec& delta0 = delta[0];
	const vector<vec>& w1=w[1];
	vector<vec>& dw0=dw[0];
	const vec& yla=y[nc1-1];
	vec& deltala = delta[nc1-1];
	const vec& yl=y[nc1];
	vec& deltal = delta[nc1];
	const vector<vec>& wl=w[nc1];
	vector<vec>& dwl=dw[nc1];
	
	for(epoca=0; epoca<limitsteps; epoca++) {
		error=0;
		for(ipatron=P.size()-1; ipatron>=0; ipatron--) {
			ipm=map[ipatron];
			//calculo el vector y de salidas de cada capa
			const vec& patron=P[ipm];
			for(i=y[0].s-1;i>0;i--) y0[i]=umbral(w0[i-1]*patron);
			for(i=nc1-1; i>0; i--) {
				vec& yi=y[i];
				vector<vec>& wi=w[i];
				for(j=yi.s-1;j>0;j--) yi[j]=umbral(wi[j-1]*y[i-1]);
			}
			for(j=y.back().s-1;j>=0;j--) yl[j]=umbral(wl[j]*yla);
			
			//ahora hago la retropropagacion, empezando por la ultima capa;
			delta0 = (des[ipm] - yl);
			error+=delta0.mod();
			
			for(i=wl.size()-1; i>=0; i--) {
				delta0[i]*=(1+yl[i])*(1-yl[i]);
				dwl[i]=(yla * delta0[i]) + (dwl[i]*momento);
			}
			
			//ahora continuo con las capas ocultas
			for(i=nc1-1; i>0; i--) {
				vector<vec>& dwi=dw[i];
				vec& yi=y[i]; vec& yia=y[i-1];
				vec& deltai=delta[i]; vec& deltaia=delta[i-1];
				vector<vec>& wip=w[i+1];
				for(j=deltai.s-1;j>=0;j--) {
					double& deltaj = deltai[j];
					deltaj = 0;
					for(k=deltaia.s-1;k>=0;k--) deltaj+=0.5*(deltaia[k]*wip[k][j+1]);
					deltaj*=(1+yi[j+1])*(1-yi[j+1]);
					dwi[j]=(yia * deltaj) + (dwi[j]*momento);
				}
			}
			
			//y por ultimo con la primera capa
			for(j=deltal.s-1;j>=0;j--) {
				double& deltaj = deltal[j];
				deltaj = 0;
				for(k=deltala.s-1;k>=0;k--) deltaj+=0.5*(deltala[k]*w1[k][j+1]);
				deltaj*=(1+y0[j+1])*(1-y0[j+1]);
				dw0[j] = (patron * deltaj) + (dw0[j]*momento);
			}
	
			//actualizamos los pesos
			for(i=w.size()-1; i>=0; i--) {
				vector<vec>& wi=w[i];
				const vector<vec>& dwi=dw[i];
				for(j=wi.size()-1; j>=0; j--)
					wi[j] += (dwi[j] * factor);
			}
		}
		//Mostramos el error
		cout<<"Epoca: "<<epoca<<"  ->  Error: "<<error<<endl;
		//mezclamos los patrones
		for(i=map.size()/10; i>=0; i--) 
			swap(map[rand()%map.size()], map[rand()%map.size()]);
		
	}
	//ahora medimos el error
	error=0;
	vec out( nin );
	for(j=P.size()-1; j>=0; j--) {
		this->eval(P[j], out, true);
		error += (des[j] - out).mod();
	}

	cout<<"Entrenamiento finalizado."<<endl<<"Error: "<<error<<endl<<"Tiempo tardado: "<<toc()<<endl;
	return error;
}



bool PML::eval(vec entrada, vec& out, bool biased) const{
	if(!biased) entrada.push_begin(-1);
	unsigned int i, nc=w.size()-1, wsi;
	int j;
	for(i=0; i<nc; i++) {
		const vector< vec >& wi=w[i];
		wsi=wi.size();
		out.alloc(wsi+1,false);
		out[0]=-1;
		for(j=wsi-1; j>=0; j--) out[j+1] = umbral(entrada*wi[j]);
		entrada=out;
	}
	const vector< vec >& wi=w[i];
	wsi=wi.size();
	out.alloc(wsi,false);
	for(j=wsi-1; j>=0; j--) out[j] = umbral(entrada*wi[j]);
	return true;
}

unsigned int PML::eval(const vec& entrada, bool biased) const{
	vec out;
	int r=0;
	this->eval(entrada, out, biased);
	for(int i=out.s-1; i>=0; i--) {
		r=r<<1;
		if(out[i]>0) r=r|1;
	}
	return r;
}

void PML::forget() {
	unsigned int i,j, k;
	for(i=0; i<w.size(); i++) {
		vector< vec >& wi=w[i];
		for(j=0; j<wi.size(); j++) {
			vec& wij=wi[j];
			for(k=0; k<wij.s; k++) wij[k]=double(rand()) * 2 / double(RAND_MAX) - 1;
		}
	}
}

bool PML::save(string name) {
	ofstream archi(name.c_str(), ios::trunc|ios::out);
	if(!archi.is_open()||!archi.good()) return false;
	unsigned int i, j, k, wpn;
	archi<<"PMC"<<" "<<w.size();
	for(i=0; i<<w.size(); i++) {
		vector<vec>& wi=w[i];
		wpn=wi[0].s;
		archi<<endl<<wi.size()<<", "<<wpn<<endl;
		for(j=0; j<wi.size(); j++) {
			vec& wij=wi[j];
			archi<<wij[0];
			for(k=1; k<wpn; k++) archi<<", "<<wij[k];
		}
	}
	archi.close();
	return true;
}

bool PML::load(string name) {
	ifstream archi(name.c_str() );
	if(!archi.is_open()||!archi.good()) return false;
	string type;
	archi>>type;
	if(type != "PMC") return false;
	unsigned int i, j, k, c, ppc, wpp;
	archi.ignore();
	archi>>c;
	archi.ignore();
	w.resize(c);
	for(i=0; i<c; i++) {
		archi>>ppc;
		archi.ignore();
		archi>>wpp;
		archi.ignore();
		vector<vec>& wi = w[i];
		wi.resize(ppc, vec(wpp));
		
		for(j=0; j<ppc; j++) {
			vec& wij=wi[j];
			for(k=1; k<wpp; k++) {
				archi>>wij[k];
				archi.ignore();
			}
		}
	}
	archi.close();
	return true;
}

bool PML::printWeight() {
	if(w.empty()) return false;
	unsigned int i, nc=w.size(), j, np, k, nw;
	for(i=0; i<nc; i++) {
		cout<<"Capa: "<<i<<endl;
		vector< vec >& wi = w[i];
		np = wi.size();
		for(j=0; j<np; j++) {
			cout<<"   Perceptron: "<<j<<endl;
			vec& wij = wi[j];
			nw=wij.s;
			for(k=0; k<nw; k++) cout<<"      w["<<k<<"] -> "<<wij[k]<<endl;
		}
	}
	return true;
}

void PML::Partir(PML* &coder, PML* &decoder) {
	if( coder != NULL )
		delete coder;
	if( decoder != NULL )
		delete decoder;
	int cc, cd, i,nc=w.size();
	if(nc%2) {
		cc = nc/2+1;
		cd = nc/2;
	}
	else {
		cc = nc/2;
		cd = nc/2;
	}
	coder   = new PML();
	decoder = new PML();
	
	//Coder
	coder->w.resize(cc);
	for(i=0; i < cc; i++)
		coder->w[i] = this->w[i];
	coder->umbral = this->umbral;
	coder->nin = this->nin;
	
	//Decoder
	decoder->w.resize(cd);
	for(i=0; i < cd; i++)
		decoder->w[i] = this->w[i+cc];
	decoder->umbral = this->umbral;
	decoder->nin = coder->w.back().size();
}

