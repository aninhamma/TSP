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
  double custoTeste;
  vector<int> copia = sequencia;

  int best_i = 0, best_j = 0;
  for(int i = 1; i < sequencia.size() - 1; i++){
    for(int j = i + 1; j < sequencia.size() - 1; j++){
      swap(copia[i], copia[j]);

      delta = calculateSwapCost(i, j, sequencia); 
      custoTeste = custoDaSolucao(copia);

      if(custo + delta == custoTeste){
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
    //custoTeste = custoDaSolucao(sequencia);
    //cout << "Custo teste swap: " << custoTeste << endl; 

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
  int best_i = 0, best_j = 0;
  for(int i = 1; i < sequencia.size() - 1; i++){
    for(int j = i + 1; j < sequencia.size() - 1; j++){
      delta = calculate2OptCost(i, j, sequencia);

        if(delta < bestDelta){
          bestDelta = delta;  
          best_i = i;
          best_j = j;

        }
    }
  }

  if(bestDelta < 0){
    reverse(sequencia.begin() + best_i, sequencia.begin() + best_j);
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
    delta = matrizAdj[sequencia[i - 1]][sequencia[i]] - matrizAdj[sequencia[i]][sequencia[i + 1]] - matrizAdj[sequencia[j]][sequencia[j + 1]] + matrizAdj[sequencia[i]][sequencia[j]] + matrizAdj[sequencia[i]][sequencia[j + 1]] + matrizAdj[sequencia[i - 1]][sequencia[j - 1]];
  }else{
    delta = matrizAdj[sequencia[j]][sequencia[j + 1]] - matrizAdj[sequencia[i - 1]][sequencia[i]] - matrizAdj[sequencia[i]][sequencia[i + 1]] + matrizAdj[sequencia[i]][sequencia[j]] + matrizAdj[sequencia[i - 1]][sequencia[i + 1]] + matrizAdj[sequencia[j - 1]][sequencia[i]];
  }
  return delta;
}
  

bool bestImprovementReinsertion(vector <int> &sequencia, double custo){
  double delta;
  double bestDelta = 0;
  int best_i = 0, best_j = 0;
  for(int i = 1; i < sequencia.size() - 1; i++){
    for(int j = 1; j < sequencia.size() - 1; j++){
      if(i != j){
        delta = calculateReinsertionCost(i, j, sequencia);
      }
      //cout << "Delta: " << delta << endl;
      
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
    delta = matrizAdj[sequencia[i - 1]][sequencia[i]] - matrizAdj[sequencia[i + 1]][sequencia[i + 2]] - matrizAdj[sequencia[j]][sequencia[j + 1]] + matrizAdj[sequencia[j]][sequencia[i]] + matrizAdj[sequencia[i + 1]][sequencia[j + 1]] + matrizAdj[sequencia[i - 1]][sequencia[j]];
  }else{
    delta = matrizAdj[sequencia[j - 1]][sequencia[j]] - matrizAdj[sequencia[i - 1]][sequencia[i]] - matrizAdj[sequencia[i + 1]][sequencia[i + 2]] + matrizAdj[sequencia[j - 1]][sequencia[i]] + matrizAdj[sequencia[i + 1]][sequencia[j]] + matrizAdj[sequencia[j + 1]][sequencia[i + 2]];
  }
  return delta;
}

bool bestImprovementOrOpt2(vector <int> &sequencia, double custo){
  double delta;
  double bestDelta = 0;
  int best_i = 0, best_j = 0;
  for(int i = 1; i < sequencia.size() - 1; i++){
    for(int j = 1; j < sequencia.size() - 1; j++){
      if(i != j){
        delta = calculateOrOpt2Cost(i, j, sequencia);
      }
      //cout << "Delta: " << delta << endl;
      
        if(delta < bestDelta){
          bestDelta = delta;
          best_i = i;
          best_j = j;
        }
    }
  }

  if(bestDelta < 0){
    sequencia.erase(sequencia.begin() + best_i, sequencia.begin() + best_i + 2);
    sequencia.insert(sequencia.begin() + best_j, sequencia[best_i], sequencia[best_i] + 2);
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
    delta = matrizAdj[sequencia[i - 1]][sequencia[i]] - matrizAdj[sequencia[i + 2]][sequencia[i + 3]] - matrizAdj[sequencia[j]][sequencia[j + 1]] + matrizAdj[sequencia[i - 1]][sequencia[j]] + matrizAdj[sequencia[j]][sequencia[i]] + matrizAdj[sequencia[j - 1]][sequencia[j + 1]];
  }else{
    delta = matrizAdj[sequencia[j - 1]][sequencia[j]] - matrizAdj[sequencia[i - 1]][sequencia[i]] - matrizAdj[sequencia[i + 2]][sequencia[i + 3]] + matrizAdj[sequencia[j - 1]][sequencia[i]] + matrizAdj[sequencia[i + 2]][sequencia[j]] + matrizAdj[sequencia[j + 1]][sequencia[i + 3]];
  }
  return delta;
}

bool bestImprovementOrOpt3(vector <int> &sequencia, double custo){
  double delta;
  double bestDelta = 0;
  int best_i = 0, best_j = 0;
  for(int i = 1; i < sequencia.size() - 1; i++){
    for(int j = 1; j < sequencia.size() - 1; j++){ 
      if(i != j){
        delta = calculateOrOpt3Cost(i, j, sequencia); 
      }
      //cout << "Delta: " << delta << endl;
      
        if(delta < bestDelta){
          bestDelta = delta;
          best_i = i;
          best_j = j;
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

/*void buscaLocal(vector<int> &sequencia; double &custo){
  vector<int> NL = {1, 2, 3, 4, 5};
  bool improved = false;

  while(NL.empty() == false){
    int n = rand() % NL.size();
    switch (NL[n]){
      case 1:
        improved = bestImprovementSwap(sequencia, custo);
        break;
      case 2:
        improved = bestImprovement2Opt(sequencia, custo);
        break;
      case 3:
        improved = bestImprovemenReinsertion(sequencia, custo);
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
}*/

int main(int argc, char** argv) {

    readData(argc, argv, &dimension, &matrizAdj);
    printData();  

    vector <int> CL;
    vector <int> sequencia;//primeira solucao
    vector <int> best;//solucao melhorada
    int i;
    double custo = 0;

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

    cout << "custo da solucao inicial: " << custo << endl;

    vector<int> seqTeste = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};

    vector<int> seqTeste2 = {1,2,3,4,9,6,7,8,5,10,11,12,13,14};

    double custo1, custoPri;
    double deltaTeste;

    int k = 4;
    int l = 8;


    custoPri = custoDaSolucao(seqTeste);

    deltaTeste = calculateSwapCost(k, l, seqTeste);

    swap(seqTeste[k], seqTeste[l]);

    custo1 = custoDaSolucao(seqTeste);

    cout << "delta real: " << custoPri - custo1 << endl;

    double deltaTeste2 = calculateSwapCost(k, l, seqTeste);

    cout << "delta test 2: " << deltaTeste2 << endl;

    double deltaNovo = MatrizAdj[seqTeste[k]] [seqTeste[l + 1]] + matrizAdj[seqTeste[k]][seqTeste[l - 1]] + matrizAdj[seqTeste[l]][seqTeste[k - 1]]


    




    
    cout << "Custo primeiro: " << custoPri << endl;
    cout << "Custo 1: " << custo1 << endl;
    cout << "delta teste: " << deltaTest << endl;



    //.......................Busca local......................//

    //best = sequencia; //solucao melhorada recebe a solucao inicial  

   /* vector<int> copia = sequencia;
    double custoTeste; 
    double delta;

    for(int i = 1; i < copia.size() - 1; i++){
      for(int j = i + 1; j < copia.size() - 1; j++){

        swap(copia[i], copia[j]);

        custoTeste = custoDaSolucao(copia);

        cout << "Custo teste swap: " << custoTeste << endl;

        copia = sequencia;
      }
    }*/



    /*bestImprovementSwap(sequencia, custo);

    bestImprovement2Opt(sequencia, custo); 

    bestImprovementReinsertion(sequencia, custo);

    bestImprovementOrOpt2(sequencia, custo);

    bestImprovementOrOpt3(sequencia, custo);*/

    




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



