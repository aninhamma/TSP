#include "readData.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

double ** matrizAdj; // matriz de adjacencia
int dimension; // quantidade total de vertices

void printData();

struct InsertionInfo{
  int noInserido; // k a ser inserido
  int arestaRemovida; // aresta {i,j} na qual o k vai ser inserido
  double custo; // delta
};

void printSolucao(vector<int> sequencia){
  for(int i = 0; i < sequencia.size() - 1; i++) 
    cout << sequencia[i] << "->";
    cout << sequencia.back() << endl;
}

void removeDaLista (vector <int> &CL, int n){//funcao que remove o no inserido da lista de candidatos
  
  CL.erase(CL.begin() + n); 
   
}

void escolher3NosAleatorios(vector <int> &sequencia, vector <int> &CL){//funcao que escolhe os 3 primeiros nos da solucao(uma sequencia inicial)

  int n = 0;//comecando o no com o primeiro elemento inserido na sequencia
  int i;

  sequencia = {1,1};//inicilaizei a sequencia com a primeira cidade

  removeDaLista(CL, n);//remove o primeiro no inserido

  for(i = 0; i < 3; i++){ //for para repetir o processo de escolha aleatoria para pegar tres nos
  
      n = rand() % CL.size(); //escolhe aleatoriamente um no da lista de candidatos

      sequencia.insert(sequencia.end() - 1, CL[n]);//insere o no escolhido na penultima posicao

      removeDaLista(CL, n);//remove o no inserido da lista de candidados

  }
    
}


vector<InsertionInfo> calcularCustoInsercao(vector <int> &sequencia, vector <int> &CL){

  vector<InsertionInfo> custoInsercao((sequencia.size() - 1) * CL.size());

  int l = 0;
  for(int a = 0, b = 1; a < sequencia.size() - 1; a++, b++){
    int i = sequencia[a];
    int j = sequencia[b];

    for(auto k : CL){
      //cout << i << j << k << l;
      custoInsercao[l].custo = matrizAdj[i][k] + matrizAdj[i][j] - matrizAdj[i][j];
      custoInsercao[l].noInserido = k;
      custoInsercao[l].arestaRemovida = a;
      l++;
    }
  }
  return custoInsercao;
}

bool compares(InsertionInfo a, InsertionInfo b){
  return a.custo < b.custo; 
}

void ordenarEmOrdemCrescente(vector<InsertionInfo> &custoInsercao){
  sort(custoInsercao.begin(), custoInsercao.end(), compares);

}

void InserirNaSolucao(vector <int> &sequencia, int selecionado, int arestaRemovida){

  sequencia.insert(sequencia.begin() + arestaRemovida + 1, selecionado);
}

double custoDaSolucao(vector<int> &sequencia){
  double custo = 0;
  for(int i = 0, j = 1; i < sequencia.size() - 1; i++, j++){
    custo += matrizAdj[sequencia[i]][sequencia[j]];
  }
  return custo;
}

vector<int> construcao(){
  vector <int> CL;
  vector <int> sequencia;//primeira solucao
  //vector <int> best;//solucao melhorada
  int i;
  double custo = 0;

  for(i = 0; i < dimension; i++){
    CL.push_back(i+1);
  }

  //cout << "Lista de candidatos: " << endl;

  //printSolucao(CL);

  escolher3NosAleatorios(sequencia, CL);

  //cout << "Tres cidades iniciais: " << endl;

  //printSolucao(sequencia);
    
  double alpha = (double) rand() / RAND_MAX;

  //cout << alpha << endl;

  vector<InsertionInfo> custoInsercao = calcularCustoInsercao(sequencia, CL);

  while(!CL.empty()){

    ordenarEmOrdemCrescente(custoInsercao);

    int custoS = (int)(ceil (alpha * custoInsercao.size()));

    //cout << custoInsercao.size() << endl;

    //cout << custoS << endl;

    int selecionado = rand() % custoS;

    //cout << selecionado << endl;

    InserirNaSolucao(sequencia, custoInsercao[selecionado].noInserido, custoInsercao[selecionado].arestaRemovida); 

    for(int i = 0; i < CL.size(); i++){
      int k = custoInsercao[selecionado].noInserido;
      if(CL[i] == k){
        removeDaLista(CL, i);
      }
    }

    custoInsercao = calcularCustoInsercao(sequencia, CL);


  }

  //cout << "Solucao inicial: " << endl; 

  //printSolucao(sequencia);

  custo = custoDaSolucao(sequencia);

  //cout << "custo da solucao inicial: " << custo << endl;

  return sequencia;
  
}

double calculateSwapCost(int i, int j, vector <int> &sequencia){ //i e j sao cidades que serao trocadas
  double delta;

  double parteDaSeqInicial = -(matrizAdj[sequencia[i]][sequencia[i + 1]] + matrizAdj[sequencia[i]][sequencia[i - 1]] +
                              matrizAdj[sequencia[j]][sequencia[j + 1]] + matrizAdj[sequencia[j]][sequencia[j - 1]]);//parte da sequencia inicial que sera retirada quando as cidades trocadas nao forem vizinhas

  if(j == i + 1){ //para quando as cidades trocadas forem vizinhas
    delta = -matrizAdj[sequencia[i - 1]][sequencia[i]] - matrizAdj[sequencia[j]][sequencia[j + 1]] + 
            matrizAdj[sequencia[i - 1]][sequencia[j]] + matrizAdj[sequencia[i]][sequencia[j + 1]];
  }else{
    delta = parteDaSeqInicial + matrizAdj[sequencia[i]][sequencia[j + 1]] + matrizAdj[sequencia[i]][sequencia[j - 1]] + matrizAdj[sequencia[j]][sequencia[i + 1]] + matrizAdj[sequencia[j]][sequencia[i - 1]];
  }

  return delta;

}

bool bestImprovementSwap(vector <int> &sequencia, double custo){
  double delta;
  double bestDelta = 0;
  double custoTeste;
  vector<int> copia = sequencia;

  int best_i = 0, best_j = 0;
  for(int i = 1; i < sequencia.size() - 1; i++){
    for(int j = i + 1; j < sequencia.size() - 1; j++){
      swap(copia[i], copia[j]);

      delta = calculateSwapCost(i, j, sequencia); 
      custoTeste = custoDaSolucao(copia);

      if(custo + delta != custoTeste){
        cout << custo + delta << endl;
        cout << custoTeste << endl;
        cout << "i: " << i << "j: " << j << endl;
      }

      copia = sequencia;

        if(delta < bestDelta){
          //cout << "comparacao dos deltas" << endl;
          bestDelta = delta; 
          best_i = i;
          best_j = j;
        }
    }
  }

  if(bestDelta < 0){
    swap(sequencia[best_i], sequencia[best_j]);
    cout << "custo antes da troca: " << custo << endl;
    custo = custo + delta;
    cout << "custo apos a troca: " << custo << endl; 
    return true; 
  }else{
    return false;
  }
  
}

double calculate2OptCost(int i, int j, vector<int> &sequencia){
  double delta;

  delta = -matrizAdj[sequencia[i - 1]][sequencia[i]] - matrizAdj[sequencia[j]][sequencia[j + 1]] + matrizAdj[sequencia[i - 1]][sequencia[j]] + matrizAdj[sequencia[i]][sequencia[j + 1]];

  return delta;
}

bool bestImprovement2Opt(vector <int> &sequencia, double custo){
  double delta;
  double bestDelta = 0; 
  double custoTeste;
  vector<int> copia = sequencia;

  int best_i = 0, best_j = 0;
  for(int i = 1; i < sequencia.size() - 1; i++){
    for(int j = i + 1; j < sequencia.size() - 1; j++){
      reverse(copia.begin() + i, copia.begin() + j + 1);

      delta = calculate2OptCost(i, j, sequencia);
      custoTeste = custoDaSolucao(copia);

      if(custo + delta != custoTeste){
        cout << custo + delta << endl;
        cout << custoTeste << endl;
        cout << "i: " << i << "j: " << j << endl;
      }

      copia = sequencia;

        if(delta < bestDelta){
          bestDelta = delta;  
          best_i = i;
          best_j = j;

        }
    }
  }

  if(bestDelta < 0){
    reverse(sequencia.begin() + best_i, sequencia.begin() + best_j + 1);
    cout << "custo antes do 2opt: " << custo << endl;
    custo = custo + bestDelta;
    cout << "custo apos o 2opt: " << custo << endl;
    return true; 
  }else{
    return false;
  }
  
}

double calculateReinsertionCost(int i, int j, vector<int> &sequencia){
  double delta;

  if(i < j){
    delta = -matrizAdj[sequencia[i - 1]][sequencia[i]] - matrizAdj[sequencia[i]][sequencia[i + 1]] - matrizAdj[sequencia[j]][sequencia[j + 1]] + matrizAdj[sequencia[i]][sequencia[j]] + matrizAdj[sequencia[i]][sequencia[j + 1]] + matrizAdj[sequencia[i - 1]][sequencia[i + 1]];
  }else{
    delta = -matrizAdj[sequencia[j]][sequencia[j - 1]] - matrizAdj[sequencia[i - 1]][sequencia[i]] - matrizAdj[sequencia[i]][sequencia[i + 1]] + matrizAdj[sequencia[i]][sequencia[j]] + matrizAdj[sequencia[i - 1]][sequencia[i + 1]] + matrizAdj[sequencia[j - 1]][sequencia[i]];
  }
  return delta;
}
  

bool bestImprovementReinsertion(vector <int> &sequencia, double custo){
  double delta;
  double bestDelta = 0;
  double custoTeste;
  vector<int> copia = sequencia;

  int best_i = 0, best_j = 0;
  for(int i = 1; i < sequencia.size() - 1; i++){
    for(int j = 1; j < sequencia.size() - 1; j++){
     
      if(i != j){
        copia.erase(copia.begin() + i);
        copia.insert(copia.begin() + j, sequencia[i]);
        delta = calculateReinsertionCost(i, j, sequencia);
        custoTeste = custoDaSolucao(copia);

        if(custo + delta != custoTeste){
          cout << custo + delta << endl;
          cout << custoTeste << endl;
          cout << "i: " << i << "j: " << j << endl;
        }

      }

      copia = sequencia;

        if(delta < bestDelta){
          bestDelta = delta;
          best_i = i;
          best_j = j;
        }
    }
  }

  if(bestDelta < 0){
    sequencia.erase(sequencia.begin() + best_i);
    sequencia.insert(sequencia.begin() + best_j, sequencia[best_i]);

    cout << "custo antes do reinsertion: " << custo << endl;
    custo = custo + bestDelta;
    cout << "custo apos o reinsertion: " << custo << endl;
    return true; 
  }else{
    return false;
  }
  
}

double calculateOrOpt2Cost(int i, int j, vector<int> &sequencia){
  double delta;

  if(i < j){
    delta = -matrizAdj[sequencia[i - 1]][sequencia[i]] - matrizAdj[sequencia[i + 1]][sequencia[i + 2]] - matrizAdj[sequencia[j + 1]][sequencia[j + 2]] + matrizAdj[sequencia[i + 1]][sequencia[j + 2]] + matrizAdj[sequencia[i]][sequencia[j + 1]] + matrizAdj[sequencia[i - 1]][sequencia[i + 2]];
  }else{
    delta = -matrizAdj[sequencia[j - 1]][sequencia[j]] - matrizAdj[sequencia[i - 1]][sequencia[i]] - matrizAdj[sequencia[i + 1]][sequencia[i + 2]] + matrizAdj[sequencia[j - 1]][sequencia[i]] + matrizAdj[sequencia[i + 1]][sequencia[j]] + matrizAdj[sequencia[i - 1]][sequencia[i + 2]];
  }
  return delta; 
}

bool bestImprovementOrOpt2(vector <int> &sequencia, double custo){
  double delta;
  double bestDelta = 0; 
  double custoTeste;
  vector<int> copia = sequencia;

  int best_i = 0, best_j = 0;
  //cout << "tamanho sequencia: " << sequencia.size() << endl;

  for(int i = 1; i < sequencia.size() - 2; i++){
    for(int j = 1; j < sequencia.size() - 3; j++){
      //cout << "i: " << i << "j: " << j << endl;
    
      if(i != j){
        /*for(int k = 0; k < copia.size() - 1; k++){
          cout << copia[k] << "->";
        }
          cout << copia.back() << endl;*/

        copia.erase(copia.begin() + i, copia.begin() + i + 2);
        copia.insert(copia.begin() + j, &sequencia[i], &sequencia[i] + 2);
        /*for(int k = 0; k < copia.size() - 1; k++){
          cout << copia[k] << "->";
        }
          cout << copia.back() << endl;*/
        delta = calculateOrOpt2Cost(i, j, sequencia);
        custoTeste = custoDaSolucao(copia);
      
        if(custo + delta != custoTeste){
          cout << custo + delta << endl;
          cout << "custo teste: " << custoTeste << endl;
          cout << "i: " << i << "j: " << j << endl;
        }
      }
      //cout << "Antes da copia" << endl;
      copia = sequencia;
      
        if(delta < bestDelta){
          bestDelta = delta;
          best_i = i;
          best_j = j;
        }
    }
    //cout << "Teste apos o primeiro for" << endl;
  }
  //cout << "Teste 2" << endl;
  
  if(bestDelta < 0){
    sequencia.erase(sequencia.begin() + best_i, sequencia.begin() + best_i + 1);
    sequencia.insert(sequencia.begin() + best_j, sequencia[best_i], sequencia[best_i] + 1);
    cout << "custo antes do Oropt2: " << custo << endl;
    custo = custo + bestDelta; 
    cout << "custo apos o Oropt2: " << custo << endl;
    return true;  
  }else{
    return false;
  }
  
}

double calculateOrOpt3Cost(int i, int j, vector<int> &sequencia){
  double delta;

  if(i < j){
    delta = -matrizAdj[sequencia[i - 1]][sequencia[i]] - matrizAdj[sequencia[i + 2]][sequencia[i + 3]] - matrizAdj[sequencia[j + 2]][sequencia[j + 3]] + matrizAdj[sequencia[i - 1]][sequencia[i + 3]] + matrizAdj[sequencia[i]][sequencia[j + 2]] + matrizAdj[sequencia[i + 2]][sequencia[j + 3]];
  }else{
    delta = -matrizAdj[sequencia[j - 1]][sequencia[j]] - matrizAdj[sequencia[i - 1]][sequencia[i]] - matrizAdj[sequencia[i + 2]][sequencia[i + 3]] + matrizAdj[sequencia[j - 1]][sequencia[i]] + matrizAdj[sequencia[i + 2]][sequencia[j]] + matrizAdj[sequencia[i - 1]][sequencia[i + 3]];
  }
  return delta;
}

bool bestImprovementOrOpt3(vector <int> &sequencia, double custo){
  double delta;
  double bestDelta = 0;
  double custoTeste;
  double custoAntes = custo;
  vector<int> copia = sequencia;

  int best_i = 0, best_j = 0;
  //cout << "inicio" << endl;
  for(int i = 1; i < sequencia.size() - 3; i++){
    for(int j = 1; j < sequencia.size() - 4; j++){ 

      if(i != j){
        copia.erase(copia.begin() + i, copia.begin() + i + 3);
        copia.insert(copia.begin() + j, &sequencia[i], &sequencia[i] + 3);
        delta = calculateOrOpt3Cost(i, j, sequencia); 
      
        custoTeste = custoDaSolucao(copia);
       
        if(custo + delta != custoTeste){
          cout << custo + delta << endl;
          cout << custoTeste << endl;
          cout << "i: " << i << "j: " << j << endl;
        }

        copia = sequencia;
      
        if(delta < bestDelta){
          bestDelta = delta;
          best_i = i;
          best_j = j;
        }
      }
    }
  }

  if(bestDelta < 0){
    sequencia.erase(sequencia.begin() + best_i, sequencia.begin() + best_i + 3);
    sequencia.insert(sequencia.begin() + best_j, sequencia[best_i], sequencia[best_i] + 3);

    cout << "custo antes do Oropt3: " << custo << endl;
    custo = custo + bestDelta;
    cout << "custo apos o Oropt3: " << custo << endl;

    return true; 
  }else{
    return false;
  }
  
}

void buscaLocal(vector<int> &sequencia, double custo){
  vector<int> NL = {1, 2, 3, 4, 5};
  bool improved = false;
  double custoAlt = custo;

  while(NL.empty() == false){
    int n = rand() % NL.size();
    switch (NL[n]){
      case 1:
        bestImprovementSwap(sequencia, custoAlt);
        if(custoAlt < custo){
          improved = bestImprovementSwap(sequencia, custo);
        }
        break; 
      case 2:
        improved = bestImprovement2Opt(sequencia, custo);
        break;
      case 3:
        improved = bestImprovementReinsertion(sequencia, custo);
        break;
      case 4:
        improved = bestImprovementOrOpt2(sequencia, custo);
        break;
      case 5:
        improved = bestImprovementOrOpt3(sequencia, custo);
        break;
    }

    if(improved){
      NL = {1, 2, 3, 4, 5};
    }else{
      NL.erase(NL.begin() + n);
    }
  }
}

int main(int argc, char** argv){

    readData(argc, argv, &dimension, &matrizAdj);
    printData();  

    vector<int> sequencia, best, bestOfAll;
    double custo = 0;
    double custoBest;
    double custoBestOfAll = INFINITY;
    int maxIter = 50;
    int maxIterILS;

    if(dimension >= 150){
      maxIterILS = dimension / 2.0;
    }else{
      maxIterILS = dimension;
    }

    
    unsigned seed = time(0);
    cout << seed << endl;
    srand (1669752966);

    for(int i = 0; i < maxIter; i++){

      sequencia = construcao();
      best = sequencia;

      cout << "solucao inicial: " << endl; 

      printSolucao(sequencia);

      custo = custoDaSolucao(sequencia);
      custoBest = custo;

      cout << "custo da solucao inicial: " << custo << endl;

      int iterILS = 0;

      while(iterILS <= maxIterILS){
        buscaLocal(sequencia, custo);
        if(custo < custoBest){
          best = sequencia;
          printSolucao(best);
          iterILS = 0;
        }
      }
    

    //bestImprovementSwap(sequencia, custo);

    //bestImprovement2Opt(sequencia, custo); 

    //bestImprovementOrOpt2(sequencia, custo); 

    //bestImprovementOrOpt3(sequencia, custo);

    //bestImprovementReinsertion(sequencia, custo);

    

   }


    return 0; 

    

}

void printData(){
  cout << "dimension: " << dimension << endl;
  for(size_t i = 1; i <= dimension; i++){
    for(size_t j = 1; j <= dimension; j++){
      cout << matrizAdj[i][j] << " ";
    }
    cout << endl;
  }
}



//fazer a copia da solucao e depois realizar o movimento e calcular o custo total



