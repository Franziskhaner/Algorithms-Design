// ###### Config options ################

//#define PRINT_DEFENSE_STRATEGY 1    // generate map images

// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"

#ifdef PRINT_DEFENSE_STRATEGY
#include "ppm.h"
#endif

#ifdef CUSTOM_RAND_GENERATOR
RAND_TYPE SimpleRandomGenerator::a;
#endif

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

//Método para calcular el valor de las celdas para la defensa principal
void cellValue(Celda** matrizCeldas, int numCeldasAncho, int numCeldasAlto) {

	//Vamos a dividir el mapa en 4 cuadrantes y a darle valores a las celdas en función de su proximidad al centro del mapa

	int valor1=0;

	for(int fil=0; fil<numCeldasAlto/2; fil++){
		//PRIMER CUADRANTE
		int col=0;
		while(col < numCeldasAncho/2){
			matrizCeldas[fil][col].valor = valor1;
			valor1 += 10;
			col++;
		}

		//SEGUNDO CUADRANTE
		valor1 -= 10;
		col = numCeldasAncho/2;
		while(col < numCeldasAncho){
			matrizCeldas[fil][col].valor = valor1;
			valor1 -= 10;
			col++;
		}

		valor1 += 20;
	}

	int valor2=0;

	for(int fil=numCeldasAlto; fil>numCeldasAlto/2; fil--){	//Ahora vamos a recorrer la matriz de abajo hacia arriba
		//TERCER CUADRANTE
		int col=0;
		while(col < numCeldasAncho/2){
			matrizCeldas[fil-1][col].valor = valor2;
			valor2 += 10;
			col++;
		}

		//CUARTO CUADRANTE
		valor2 -=10;
		col = numCeldasAncho/2;
		while(col < numCeldasAncho){
			matrizCeldas[fil-1][col].valor = valor2;
			valor2 -= 10;
			col++;
		}
		valor2 += 20;
	}
}

//Método para calcular el valor de las celdas para las defenas secundarias
void cellValueSecundarias(Celda** matrizCeldas, int numCeldasAncho, int numCeldasAlto, bool** freeCells, int fila, int columna) {

	//Vamos a dividir el mapa en 4 cuadrantes y a darle valores a las celdas en función de su proximidad al centro del mapa

	int valor1=0;

	for(int fil=0; fil<numCeldasAlto/2; fil++){
		//PRIMER CUADRANTE
		int col=0;
		while(col < numCeldasAncho/2){
			matrizCeldas[fil][col].valor = valor1;
			valor1 += 10;
			col++;
		}

		//SEGUNDO CUADRANTE
		valor1 -= 10;
		col = numCeldasAncho/2;
		while(col < numCeldasAncho){
			matrizCeldas[fil][col].valor = valor1;
			valor1 -= 10;
			col++;
		}

		valor1 += 20;
	}

	int valor2=0;

	for(int fil=numCeldasAlto; fil>numCeldasAlto/2; fil--){	//Ahora vamos a recorrer la matriz de abajo hacia arriba
		//TERCER CUADRANTE
		int col=0;
		while(col < numCeldasAncho/2){
			matrizCeldas[fil-1][col].valor = valor2;
			valor2 += 10;
			col++;
		}

		//CUARTO CUADRANTE
		valor2 -=10;
		col = numCeldasAncho/2;
		while(col < numCeldasAncho){
			matrizCeldas[fil-1][col].valor = valor2;
			valor2 -= 10;
			col++;
		}
		valor2 += 20;
	}
	 
	//Aumentamos el valor de las celdas adyacentes a la celda principal.
	int aumento=0;
	
	for(int k=0; k<4; k++){
		
		if(k==1){
			aumento=20;
			matrizCeldas[fila][columna-k].valor = matrizCeldas[fila][columna].valor+aumento;	
			matrizCeldas[fila][columna+k].valor = matrizCeldas[fila][columna].valor+aumento;
			matrizCeldas[fila-k][columna].valor = matrizCeldas[fila][columna].valor+aumento;
			matrizCeldas[fila+k][columna].valor = matrizCeldas[fila][columna].valor+aumento;
			matrizCeldas[fila-k][columna+k].valor = matrizCeldas[fila][columna].valor+aumento;
			matrizCeldas[fila-k][columna-k].valor = matrizCeldas[fila][columna].valor+aumento;
			matrizCeldas[fila+k][columna+k].valor = matrizCeldas[fila][columna].valor+aumento;
			matrizCeldas[fila+k][columna-k].valor = matrizCeldas[fila][columna].valor+aumento;
		}
		if(k==2){
			aumento=15;
			matrizCeldas[fila][columna-k].valor = matrizCeldas[fila][columna].valor+aumento;	
			matrizCeldas[fila][columna+k].valor = matrizCeldas[fila][columna].valor+aumento;
			matrizCeldas[fila-k][columna].valor = matrizCeldas[fila][columna].valor+aumento;
			matrizCeldas[fila+k][columna].valor = matrizCeldas[fila][columna].valor+aumento;
			matrizCeldas[fila-k][columna+k].valor = matrizCeldas[fila][columna].valor+aumento;
			matrizCeldas[fila-k][columna-k].valor = matrizCeldas[fila][columna].valor+aumento;
			matrizCeldas[fila+k][columna+k].valor = matrizCeldas[fila][columna].valor+aumento;
			matrizCeldas[fila+k][columna-k].valor = matrizCeldas[fila][columna].valor+aumento;
			matrizCeldas[fila-1][columna+k].valor = matrizCeldas[fila][columna].valor+aumento;
			matrizCeldas[fila-1][columna-k].valor = matrizCeldas[fila][columna].valor+aumento;
			matrizCeldas[fila+1][columna+k].valor = matrizCeldas[fila][columna].valor+aumento;
			matrizCeldas[fila+1][columna-k].valor = matrizCeldas[fila][columna].valor+aumento;
			matrizCeldas[fila-k][columna+1].valor = matrizCeldas[fila][columna].valor+aumento;
			matrizCeldas[fila-k][columna-1].valor = matrizCeldas[fila][columna].valor+aumento;
			matrizCeldas[fila+k][columna+1].valor = matrizCeldas[fila][columna].valor+aumento;
			matrizCeldas[fila+k][columna-1].valor = matrizCeldas[fila][columna].valor+aumento;
		}
	}	
}

//FUNCIÓN DE SELECCIÓN:

void seleccionCelda(Celda** mCeldas, int numCeldas, Celda &celdaActual){

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

//COLOCAR DEFENSAS

void DEF_LIB_EXPORTED placeDefenses(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , std::list<Object*> obstacles, std::list<Defense*> defenses) {

    float cellWidth = mapWidth / nCellsWidth;   //Ancho celda
    float cellHeight = mapHeight / nCellsHeight;    //Alto celda
	List<Defense*> defensasColocadas;
	List<Celda> listaCeldas(nCellsWidth*nCellsHeight);	//Lista de celdas que iteraremos en el voraz.
	
	Celda** matrizCeldas = new Celda*[nCellsWidth];	//Matriz de celdas que vamos a pasar a la función cellValue.
    for(int i=0; i<nCellsWidth; i++)
        matrizCeldas[i] = new Celda[nCellsWidth];
        
    posicionarCeldas(listaCeldas, nCellsWidth, cellWidth);    //Damos posiciones a nuestra lista de celdas.
                                                     
	posicionarCeldas(matrizCeldas, nCellsWidth, cellWidth);	//Damos posiciones a nuestra matriz de celdas.

	cellValue(matrizCeldas, nCellsWidth, nCellsHeight);	//Calculamos el valor de las celdas para la defensa principal.

	//ALGORITMO VORAZ PARA LA DEFENSA PRINCIPAL
	
    List<Celda>::iterator celdaActual = listaCeldas.begin();
	List<Defense*>::iterator defensaActual = defenses.begin();
	bool colocada=false;
	int col, fil;
	
	while(celdaActual != listaCeldas.end() && colocada==false){

		seleccionCelda(matrizCeldas, nCellsWidth, (*celdaActual));	//Llamamos a la función selección

		if(esFactible(obstacles, defensasColocadas, mapWidth, mapHeight, (*defensaActual), (*celdaActual))){

			(*defensaActual)->position.x = (*celdaActual).pos.x;
			(*defensaActual)->position.y = (*celdaActual).pos.y;
			(*defensaActual)->position.z = (*celdaActual).pos.z;
		    defensasColocadas.push_back((*defensaActual));
		    colocada = true;	//Si colocamos la defensa principal ponemos a true nuestra variable booleana
    		
    		col = (*celdaActual).id % nCellsWidth;	//Nos quedamos con la fila y la columna de la celda en la que hemos colocado
    		fil = (*celdaActual).id / nCellsWidth;	//a la defensa extractora para luego pasarsela a cellValueSecundarias.
    	}
    	celdaActual++;
    }
		 
    cellValueSecundarias(matrizCeldas, nCellsWidth, nCellsHeight, freeCells, fil, col);	//Ahora vamos a calcular el valor de las celdas 																							para las defensas secundarias.

	//ALGORITMO VORAZ PARA LAS DEFENSAS SECUNDARIAS
	
	celdaActual = listaCeldas.begin();
	defensaActual = ++defenses.begin();

	while(defensaActual != defenses.end() && celdaActual != listaCeldas.end()) {

		seleccionCelda(matrizCeldas, nCellsWidth, (*celdaActual));	//Llamamos a la función selección

		if(esFactible(obstacles, defensasColocadas, mapWidth, mapHeight, (*defensaActual), (*celdaActual))){

			(*defensaActual)->position.x = (*celdaActual).pos.x;
			(*defensaActual)->position.y = (*celdaActual).pos.y;
			(*defensaActual)->position.z = (*celdaActual).pos.z;
            defensasColocadas.push_back((*defensaActual));
			defensaActual++;
		}
		celdaActual++;
	}
	
#ifdef PRINT_DEFENSE_STRATEGY

	float** cellValues = new float* [nCellsHeight];
	for(int i = 0; i < nCellsHeight; ++i) {
	   cellValues[i] = new float[nCellsWidth];
	   for(int j = 0; j < nCellsWidth; ++j) {
	       cellValues[i][j] = ((int)(cellValue(i, j))) % 256;
	   }
	}
	dPrintMap("strategy.ppm", nCellsHeight, nCellsWidth, cellHeight, cellWidth, freeCells
	                     , cellValues, std::list<Defense*>(), true);

	for(int i = 0; i < nCellsHeight ; ++i)
	    delete [] cellValues[i];
	delete [] cellValues;
	cellValues = NULL;

#endif

}
