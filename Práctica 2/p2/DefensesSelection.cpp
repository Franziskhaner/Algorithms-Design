// ###### Config options ################


// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"

using namespace Asedio;

void valorDefensa (std::list<Defense*> defenses, float* &valores, unsigned int* &costes){

	int i=0;

	for(std::list<Defense*>::iterator itDefenses = defenses.begin(); itDefenses != defenses.end(); itDefenses++){
		valores[i] = ((*itDefenses)->damage + (*itDefenses)->range + (*itDefenses)->attacksPerSecond)*1.7 + ((*itDefenses)->health /(*itDefenses)->radio)*1.3;			//Rellenamos el vector con los valores de cada defensa
		costes[i] = (*itDefenses)->cost;	//Rellenamos el vector con los costes de cada defensa
		i++;
	}
}

void mochila(std::list<Defense*> defenses, float** &TSP, unsigned int ases, float* valores, unsigned int* costes){

	for(int j=0; j<ases; j++){	
		if(j < costes[0])
			TSP[0][j] = 0;
		else
			TSP[0][j] = valores[0];
	}

	for(int i=1; i<defenses.size(); i++){	
		for(int j=0; j<ases; j++){
			if(j < costes[i])
				TSP[i][j] = TSP[i-1][j];
			else
				TSP[i][j] = std::max(TSP[i-1][j], TSP[i-1][j-costes[i]]+valores[i]);
		}
	}
}


void DEF_LIB_EXPORTED selectDefenses(std::list<Defense*> defenses, unsigned int ases, std::list<int> &selectedIDs
            , float mapWidth, float mapHeight, std::list<Object*> obstacles) {

	//En primer lugar, insertamos la base extractora y restamos su coste a los recursos de los que disponemos
	std::list<Defense*>::iterator extractora = defenses.begin();
    selectedIDs.push_back((*extractora)->id);
    ases -= (*extractora)->cost;

	float* valores = new float[defenses.size()];
	unsigned int* costes = new unsigned int[defenses.size()];

	valorDefensa(defenses, valores, costes);	//Asignamos los valores y los costes a ambos vectores pasados por referencia

	float** TSP = new float*[defenses.size()];	//Inicialiamos nuestra tabla de subproblemas resueltos
	for(int i=0; i<defenses.size(); i++)
		 TSP[i]= new float[ases];

	mochila(defenses, TSP, ases, valores, costes);	//Algoritmo de la mochila

    std::list<Defense*>::iterator it = --defenses.end();
    int i = defenses.size()-1;
	unsigned int j = ases-1;

    while(j>0 && i>0) {
        if(TSP[i][j] != TSP[i-1][j]) {	//Si el valor de la TSP de la defensa actual es distinto al de la anterior,
	        selectedIDs.push_back((*it)->id);	//guardamos el id de la defenesa para luego colocarla
	        j -= (*it)->cost;	//y restamos su coste a nuestro numero de ases totales
	        it--;
	        i--;
        }
        else{		//Si no, pasamos a la siguiente defensa.
			it--;
			i--;
        }
    }

}
