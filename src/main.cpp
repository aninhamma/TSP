#include "readData.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

double ** matrizAdj; // matriz de adjacencia
int dimension; // quantidade total de vertices

void printData();

struct InsertionInfo{
  int noInserido; // k a ser inserido
  int arestaRemovida; // aresta {i,j} na qual o k vai ser inserido
  double custo; // delta
};

void removeDaLista (vector <int> &CL, int n){//funcao que remove o no inserido da lista de candidatos
  
  CL.erase(CL.begin() + n); 
   
}

void escolher3NosAleatorios(vector <int> &sequencia, vector <int> &CL){//funcao que escolhe os 3 primeiros nos da solucao(uma sequencia inicial)

  int n = 0;//comecando o no com o primeiro elemento inserido na sequencia
  int i;

  sequencia = {1,1};//inicilaizei a sequencia com a primeira cidade

  removeDaLista(CL, n);//remove o primeiro no inserido

  unsigned seed = time(0);

  srand(seed);

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

double calculateSwapCost(int i, int j, vector <int> &sequencia){ //i e j sao cidades que serao trocadas
  double delta;

  double parteDaSeqInicial = -(matrizAdj[sequencia[i]][sequencia[i + 1]] + matrizAdj[sequencia[i]][sequencia[i - 1]] +
                              matrizAdj[sequencia[j]][sequencia[j + 1]] + matrizAdj[sequencia[j]][sequencia[j - 1]]);//parte da sequencia inicial que sera retirada quando as cidades trocadas nao forem vizinhas

  if(j = i + 1){ //para quando as cidades trocadas forem vizinhas
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
  int best_i = 0, best_j = 0;
  for(int i = 1; i < sequencia.size() - 1; i++){
    for(int j = i + 1; j < sequencia.size() - 1; j++){
      delta = calculateSwapCost(i, j, sequencia);

      cout << "delta: " << delta << endl; 

        if(delta < bestDelta){
          bestDelta = delta;
          best_i = i;
          best_j = j;
        }
    }
  }

  if(bestDelta < 0){
    swap(sequencia[best_i], sequencia[best_j]);
    custo = custo - delta;
    return true; 
  }else{
    return false;
  }
  
  cout << "custo apos swap: " << custo << endl;
}

int main(int argc, char** argv) {

    readData(argc, argv, &dimension, &matrizAdj);
    printData();

    vector <int> CL;
    vector <int> sequencia;//primeira solucao
    vector <int> best;//solucao melhorada
    int i;
    double custo, teste;

    for(i = 0; i < dimension; i++){
      CL.push_back(i+1);
    }

    cout << "Lista de candidatos: " << endl;

    for (i = 0; i < CL.size() - 1; i++) {
      cout << CL[i] << "->";
    }
    cout << CL.back() << endl;

    escolher3NosAleatorios(sequencia, CL);

    cout << "Tres cidades iniciais: " << endl;

    for(i = 0; i < sequencia.size() - 1; i++){
      cout << sequencia[i] << "->";
    }
      cout << sequencia.back() << endl;
    
    

    double alpha = (double) rand() / RAND_MAX;

    vector<InsertionInfo> custoInsercao = calcularCustoInsercao(sequencia, CL);

    while(!CL.empty()){
    
      ordenarEmOrdemCrescente(custoInsercao);

      int custoS = (int)(alpha * custoInsercao.size());

      int selecionado = rand() % custoS;

      InserirNaSolucao(sequencia, custoInsercao[selecionado].noInserido, custoInsercao[selecionado].arestaRemovida); 
    
      for(int i = 0; i < CL.size(); i++){
        int k = custoInsercao[selecionado].noInserido;
        if(CL[i] == k){
          removeDaLista(CL, i);
        }
      }

      custoInsercao = calcularCustoInsercao(sequencia, CL);


    }

    cout << "Solucao inicial: " << endl;

    for(i = 0; i < sequencia.size() - 1; i++) 
      cout << sequencia[i] << "->";
      cout << sequencia.back() << endl;

    custo = custoDaSolucao(sequencia);

    cout << "custo da solucao inicial: " << custoDaSolucao << endl;


    //.......................Busca local......................//

    //best = sequencia; //solucao melhorada recebe a solucao inicial   

    bestImprovementSwap(sequencia, custo);


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







