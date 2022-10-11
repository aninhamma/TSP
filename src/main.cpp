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

      cout << n << "primeiroteste";

      removeDaLista(CL, n);//remove o no inserido da lista de candidados

  }
    
}


vector<InsertionInfo> calcularCustoInsercao(vector <int> &sequencia, vector <int> &CL){

  cout << "print inicio do calcular custo insercao" << endl;

  cout << "print tamanho da lista de candidatos" << CL.size() << endl;

  cout << sequencia.size() - 1 << endl;

  vector<InsertionInfo> custoInsercao((sequencia.size() - 1) * CL.size());

  cerr << CL.size() << " ";

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

void InserirNaSolucao(vector <int> &sequencia, int selecionado){
  
  sequencia.insert(sequencia.end() - 1, selecionado);
}

int main(int argc, char** argv) {

    readData(argc, argv, &dimension, &matrizAdj);
    printData();

    vector <int> CL;
    vector <int> sequencia;
    int i;

    //cout << "teste" << endl;

    for(i = 0; i < dimension; i++){
      CL.push_back(i+1);
    }

    for (i = 0; i < CL.size() - 1; i++) {
      cout << CL[i] << "->";
    }
    cout << CL.back() << endl;

    escolher3NosAleatorios(sequencia, CL);

    cout << endl;

    for(i = 0; i < sequencia.size() - 1; i++){
      cout << sequencia[i] << "->";
    }
      //cout << sequencia.back() << endl;
    
    

    double alpha = (double) rand() / RAND_MAX;

    //cout << "teste 2";

    cout << sequencia.size() << endl;

    for(i = 0; i < CL.size() - 1; i++){
      cout << CL[i] << "->";
    }
      cout << CL.back() << endl;

    while(!CL.empty()){

      //cout << "teste 3";
      vector<InsertionInfo> custoInsercao = calcularCustoInsercao(sequencia, CL);

      cout << "oi" << endl;
    
      ordenarEmOrdemCrescente(custoInsercao);

      int custoS = (int)(alpha * custoInsercao.size());

      cout << custoInsercao.size() << endl;
      //cout << "oi2" << endl;

      int selecionado = rand() % custoS;

      //cout << "oi3" << endl;

      InserirNaSolucao(sequencia, selecionado);

      //cout << "oi4" <<  selecionado << endl;

      removeDaLista(CL, selecionado);

      //cout << "oi5" << endl; 

    }

    for(i = 0; i < sequencia.size() - 1; i++) 
      cout << sequencia[i] << "->";
      cout << sequencia.back() << endl;

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







