#include <bits/stdc++.h>

using namespace std;

struct Node {
    unsigned char caracter;
    unsigned int freq;
    struct Node * right;
    struct Node * left;
    Node(unsigned char c, unsigned int f, Node* r = nullptr, Node* l = nullptr) : caracter(c), freq(f), right(r), left(l) {}
    // Operador de comparação para a priority_queue, usando > para uma pq de menor para maior frequência
    bool operator<(const Node& outro) const { return freq > outro.freq; }
};

Node * build_tree_from_priority_queue(priority_queue<Node>& pq) {
    while(pq.size() > 1) {
        Node* first = new Node(pq.top()); pq.pop(); 
        Node* second = new Node(pq.top()); pq.pop();
        Node* parent = new Node(NULL, first->freq + second->freq, first, second);
        pq.push(*parent);
    } 
    return new Node(pq.top()); 
}

void create_dictionary (map<char, string>& dictionary, Node * node, string path) { 
    // quando chegar nos nós
    if(!node->left and !node->right) dictionary[node->caracter] = path;
    if(node->left ) create_dictionary(dictionary, node->left , path+"0");
    if(node->right) create_dictionary(dictionary, node->right, path+"1");
}

string encode (string text, map<char, string>& dictionary) {
    string code;
    for(auto & c: text) code += dictionary[c];
    return code;
}

string decode (string code, Node * root) {
    Node * aux = root;
    string text;
    for(auto &x: code) {
        if(x == '0') aux = aux->left;
        else         aux = aux->right; 
        if(!aux->left and !aux->right){
            text += aux->caracter;
            aux = root;
        }
    }
    return text;
}


int main(void) {
 
    string nomeArquivo = "input.txt";
    ifstream arquivo(nomeArquivo); // abre o arquivo em modo leitura

    vector<int> tabela_frequencia(257,0);
    
    string text;

    char caractere; // para guardar cada caractere do arquivo
    while (arquivo.get(caractere)) {
        text+=caractere;
        tabela_frequencia[caractere]++;
    }

    arquivo.close();

    // fila de prioridade
    priority_queue<Node> pq;
    for(int i = 0; i<257; ++i) if(tabela_frequencia[i]) {
        Node no(i,tabela_frequencia[i]);
        pq.push(no);
    }

    // Montagem da arvore
    Node * tree = build_tree_from_priority_queue(pq);

    // dicionario 
    map<char,string> dictionary;
    create_dictionary(dictionary, tree,"");

    // TODO: Compactacao e Descompactacao

    cout << decode(encode(text,dictionary),tree) << endl; 

    delete tree; 
}
