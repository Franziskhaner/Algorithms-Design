// ###### Config options ################



// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"
#include "cronometro.h"

using namespace Asedio;         

//Definimos la clase Celda
class Celda{

	public:
        int id, valor;
        Vector3 pos;
        Celda():pos(Vector3()){}
		Celda(int a, int b=0, Vector3 c = Vector3()):id(a), valor(b), pos(Vector3(c.x, c.y)){}
		Vector3 id_a_posicion(int id, int numCeldas, int anchoCelda){
            Vector3 posicion;
            int col = id%numCeldas;
            int fil = id/numCeldas;
            posicion.x = col*anchoCelda + (anchoCelda/2);
            posicion.y = fil*anchoCelda + (anchoCelda/2);
            return posicion;
		}
		
		Celda operator = (const Celda &c){
			id = c.id;
			valor = c.valor;
			pos.x = c.pos.x;
			pos.y = c.pos.y;
			
			return *this;
		}
		bool operator < (const Celda &c){ return valor < c.valor; }
};

//Método para rellenar las posiciones de una lista de celdas dada
void posicionarCeldas(List<Celda> &lista, int numCeldas, int anchoCelda){

    int i=0;
    List<Celda>::iterator itCeldas = lista.begin();

    while(itCeldas !=lista.end()){
        (*itCeldas).id = i;
        (*itCeldas).pos = (*itCeldas).id_a_posicion(i, numCeldas, anchoCelda);
        itCeldas++;
        i++;
    }
}
 
//Método para rellenar las posiciones de una matriz de celdas dada
void posicionarCeldas(Celda** matrizCel, int numCeldas, int anchoCelda){

    int valorId=0;

    for(int i=0; i<numCeldas; i++)
    	for(int j=0; j<numCeldas; j++){
		    matrizCel[i][j].id = valorId;
		    matrizCel[i][j].pos = matrizCel[i][j].id_a_posicion(valorId, numCeldas, anchoCelda);
		    valorId++;
   		 }
}

float defaultCellValue(int row, int col, bool** freeCells, int nCellsWidth, int nCellsHeight
    , float mapWidth, float mapHeight, List<Object*> obstacles, List<Defense*> defenses) {
    	
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;

    Vector3 cellPosition((col * cellWidth) + cellWidth * 0.5f, (row * cellHeight) + cellHeight * 0.5f, 0);
    	
    float val = 0;
    for (List<Object*>::iterator it=obstacles.begin(); it != obstacles.end(); ++it) {
	    val += _distance(cellPosition, (*it)->position);
    }

    return val;
}

// FUNCIÓN DE FACTIBILIDAD

bool esFactible (List<Object*> listaObstaculos, List<Defense*> defensasColocadas, float mapWidth,
 float mapHeight, Defense* defensaActual, Celda celdaActual){

	//Comprobamos que el radio de una defensa no sobrepasa los límites de nuestro mapa:
	if(((celdaActual.pos.x + defensaActual->radio) >= mapWidth || (celdaActual.pos.x - defensaActual->radio) < 0)
        || ((celdaActual.pos.y + defensaActual->radio) >= mapHeight || (celdaActual.pos.y - defensaActual->radio) < 0))
		 return false;

	//Comprobamos que en la posicion de la celda en la que vamos a colocar nuestra defensa no hay colision con otra defensa:
	List<Defense*>::iterator itDefensas = defensasColocadas.begin();
	while(itDefensas != defensasColocadas.end()){
        if(_distance((*itDefensas)->position, celdaActual.pos) <= ((*itDefensas)->radio + defensaActual->radio))
            return false;
		itDefensas++;
	}
	
	//Comprobamos que no hay colisión con un obstáculo:
	List<Object*>::iterator itObstaculos = listaObstaculos.begin();
	while(itObstaculos != listaObstaculos.end()){
		if(_distance((*itObstaculos)->position, celdaActual.pos) <= ((*itObstaculos)->radio + defensaActual->radio)){
			return false;
		}
		itObstaculos++;
	}
	
    return true;
}

//SIN PRE-ORDENACION:

void sinPreOrdenacion(Celda** mCeldas, int numCeldas, Celda &celdaActual){

	int idCelda=0;
	int valorMax=0;
	int a, b;
	
	for(int i=0; i<numCeldas; i++)
		for(int j=0; j<numCeldas; j++){
			if(mCeldas[i][j].valor > valorMax){	
				valorMax = mCeldas[i][j].valor;				
				celdaActual.pos = mCeldas[i][j].pos;
				celdaActual.id = mCeldas[i][j].id;	//Guardamos el id de la celda para utilizarlo posteriormente.		
				a = i;
				b = j;
			}
		}
		
	mCeldas[a][b].valor = -1;	//Una vez que hemos obtenido la posicion que hace referencia a la celda con mayor valor
									//la ponemos a -1 para que no volvamos a compararla con esta celda.
}

//Método que selecciona la celda del vector utilizado por los algoritmos de ordenación

Celda seleccion(std::vector<Celda>& v){
	
	Celda cellAux;

	cellAux = (*v.begin());	//Copiamos la primera celda del vector ya ordenado a nuestra celda auxiliar, la cual devolveremos al algoritmo voraz. (Para ésto debemos sobrecargar el operador de asignación).
	
	v.erase(v.begin());	//Y borramos esa primera celda para que en la próxima iteración del voraz utilicemos la siguiente celda del vector.
	
	return cellAux;
}

void DEF_LIB_EXPORTED placeDefenses3(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , List<Object*> obstacles, List<Defense*> defenses) {

    float cellWidth = mapWidth / nCellsWidth;   //Ancho celda
    float cellHeight = mapHeight / nCellsHeight;    //Alto celda
	List<Celda> listaCeldas(nCellsWidth*nCellsHeight);	//Lista de celdas que iteraremos en el voraz.
	List<Defense*> defensasColocadas;
    
    posicionarCeldas(listaCeldas, nCellsWidth, cellWidth);    //Damos posiciones a nuestra lista de celdas.
	 
	Celda** matrizCeldas = new Celda*[nCellsWidth];	//Matriz de celdas que vamos a pasar a la función cellValue.
    for(int i=0; i<nCellsWidth; i++)
        matrizCeldas[i] = new Celda[nCellsWidth];
        
	posicionarCeldas(matrizCeldas, nCellsWidth, cellWidth);	//Damos posiciones a nuestra matriz de celdas.
	
	for(int i=0; i<nCellsWidth; i++)
		for(int j=0; j<nCellsWidth; j++){
			matrizCeldas[i][j].valor = (int)defaultCellValue(i, j, freeCells, nCellsWidth, nCellsHeight
			, mapWidth, mapHeight, obstacles, defenses);	//Damos valores a nuestra matriz de celdas.
		}
		
	List<Celda>::iterator currentCell = listaCeldas.begin();
	List<Defense*>::iterator currentDefense = defenses.begin();

	while(currentDefense != defenses.end() && currentCell != listaCeldas.end()) {
		
		sinPreOrdenacion(matrizCeldas, nCellsWidth, (*currentCell));
		
		if(esFactible(obstacles, defensasColocadas, mapWidth, mapHeight, (*currentDefense), (*currentCell))){
			
			(*currentDefense)->position.x = (*currentCell).pos.x;
			(*currentDefense)->position.y = (*currentCell).pos.y;
			(*currentDefense)->position.z = 0; 
		    defensasColocadas.push_back((*currentDefense));
			currentDefense++;
		}
		currentCell++;
	}
}
