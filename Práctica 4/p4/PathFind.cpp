// ###### Config options ################

#define PRINT_PATHS 1

// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"

#ifdef PRINT_PATHS
#include "ppm.h"
#endif

using namespace Asedio;

Vector3 cellCenterToPosition(int i, int j, float cellWidth, float cellHeight){ 
    return Vector3((j * cellWidth) + cellWidth * 0.5f, (i * cellHeight) + cellHeight * 0.5f, 0); 
}

void DEF_LIB_EXPORTED calculateAdditionalCost(float** additionalCost
                   , int cellsWidth, int cellsHeight, float mapWidth, float mapHeight
                   , List<Object*> obstacles, List<Defense*> defenses) {

    float anchoCelda = mapWidth / cellsWidth;
    float altoCelda = mapHeight / cellsHeight;

    for(int i = 0 ; i < cellsHeight ; ++i) {
        for(int j = 0 ; j < cellsWidth ; ++j) {

            Vector3 cellPosition = cellCenterToPosition(i, j, anchoCelda, altoCelda);
            
            //Para el coste sumamos la distancia euclidea existente entre la celda donde esta situada
            //la defensa principal y la celda actual.
            std::list<Defense*>::iterator it = defenses.begin();
            additionalCost[i][j] = _distance((*it)->position, cellPosition);
        }
    }  
}  

bool comparar(AStarNode* i, AStarNode* j){
	return (i->F > j->F);
}

void DEF_LIB_EXPORTED calculatePath(AStarNode* originNode, AStarNode* targetNode
                   , int cellsWidth, int cellsHeight, float mapWidth, float mapHeight
                   , float** additionalCost, std::list<Vector3> &path) {

    bool encontrado = false;
    float anchoCelda = mapWidth/cellsWidth;
    float altoCelda = mapHeight/cellsHeight;

    //Aplicamos algoritmo A*:
 
    AStarNode* current = originNode;
    std::vector<AStarNode*> abiertos;   
    std::vector<AStarNode*> cerrados;

    current->H = _sdistance(current->position, targetNode->position);   //Distancia estimada entre el nodo actual(origen) y el objetivo
    current->F = current->G + current->H;
    abiertos.push_back(current);
    std::make_heap(abiertos.begin(), abiertos.end(), comparar);

    while(encontrado == false && abiertos.size() > 0){
    	current = abiertos.front();
    	std::pop_heap(abiertos.begin(), abiertos.end(), comparar);
    	abiertos.pop_back();
    	cerrados.push_back(current);

    	if(current == targetNode)
    		encontrado = true;
    	else{ 
    		std::list<AStarNode*>::iterator it = current->adjacents.begin();
    		for(it; it != current->adjacents.end(); it++){
    			if(cerrados.end() == std::find(cerrados.begin(), cerrados.end(), (*it))){
    				if(abiertos.end() == std::find(abiertos.begin(), abiertos.end(), (*it))){
    					int posX = (*it)->position.x/anchoCelda;
    					int posY = (*it)->position.y/altoCelda;
    					(*it)->parent = current;
    					(*it)->G = current->G + _distance(current->position, (*it)->position) + additionalCost[posX][posY];
    					(*it)->H = _sdistance((*it)->position, targetNode->position);
    					(*it)->F = (*it)->G + (*it)->H;
    					abiertos.push_back((*it));
    					std::make_heap(abiertos.begin(), abiertos.end(), comparar);
    				}
    				else{  
    					float distancia = _distance(current->position, (*it)->position);
    					if((*it)->G > current->G + distancia){
    						(*it)->parent = current;
    						(*it)->G = current->G + distancia;
    						(*it)->F = (*it)->G + (*it)->H;
    						std::sort_heap(abiertos.begin(), abiertos.end(), comparar);
    					}
    				}
    			}
    		}
    	}
    }

    //Ahora recuperamos el camino a seguir por el UCO:

    current = targetNode;
    path.push_front(current->position);

    while(current->parent != originNode){
        current = current->parent;
        path.push_front(current->position);
    }
}
