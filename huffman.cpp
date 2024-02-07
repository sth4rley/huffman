#include <iostream>
#include <fstream>
#include <queue>
#include <map>
#include <vector>


using namespace std;

string file_name = "input.txt";

struct Node {
    unsigned char character;
    unsigned int frequency;
    struct Node * right;
    struct Node * left;
    Node(unsigned char c, unsigned int f, Node* r = nullptr, Node* l = nullptr) : character(c), frequency(f), right(r), left(l) {}
    // Operador de comparação para a priority_queue, usando > para uma pq de menor para maior frequência
    bool operator<(const Node& outro) const { return frequency > outro.frequency; }
};

Node * build_tree_from_priority_queue(priority_queue<Node>& pq) {
    while(pq.size() > 1) {
        Node* first = new Node(pq.top()); pq.pop(); 
        Node* second = new Node(pq.top()); pq.pop();
        Node* parent = new Node(NULL, first->frequency + second->frequency, first, second);
        pq.push(*parent);
    } 
    return new Node(pq.top()); 
}

void create_dictionary (map<char, string>& dictionary, Node * node, string path) { 
    // quando chegar nos nós
    if(!node->left and !node->right) dictionary[node->character] = path;
    if(node->left) create_dictionary(dictionary, node->left , path + "0");
    if(node->right) create_dictionary(dictionary, node->right, path + "1");
}

void print_dictionary (map<char, string> & dictionary) {
    cout << "Dicionario: " << endl;
    for(const auto& pair: dictionary) {
       cout << pair.first << ": " << pair.second << endl; 
    }
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
            text += aux->character;
            aux = root;
        }
    }
    return text;
}

void compress(string code) {
    fstream file(file_name+".huff", ios::out);
    unsigned char byte = 0;

    int j = 7; 
    
    for(auto & bit: code) {
        unsigned char mask = 1;
        
        if (bit == '1') {
            mask = mask << j;
            byte = byte | mask;
        }

        j--;

        if (j < 0) {
            file << byte;
            byte = 0;
            j = 7;
        }

    }

    if (j != 7) file << byte;
    
    file.close();
}

unsigned int bit_is_set(unsigned char byte, int i) {
    unsigned char mask = (1 << i);
    return byte & mask;
}

// TODO: ifstream -> stdio? 
string decompress(Node * root) {

    ifstream file(file_name+".huff"); // abre o arquivo em modo leitura

    string text; 
    Node * aux = root;

    char byte;
    while (file.get(byte)) {
        for(int i = 7; i >=0; i--){
            if(bit_is_set(byte, i)) aux = aux->right;
            else                    aux = aux->left;
            if(!aux->left and !aux->right){
                text+= aux->character;
                aux = root;
            };
        }
    }
    file.close();

    return text;
}

int main(int argc, char ** argv) {
 
    if(argc >= 2) file_name = argv[1];

     // Abrindo o arquivo para leitura
    FILE *arquivo;
    arquivo = fopen(file_name.c_str(), "r");

    // Verificando se o arquivo foi aberto corretamente
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    string text;
    vector<int> frequency_table(257,0);

    int chr;
    while ((chr = fgetc(arquivo)) != EOF) {
        text += chr;
        frequency_table[(int)chr]++; 
    }

    fclose(arquivo);


    // fila de prioridade
    priority_queue<Node> pq;
    for(int i = 0; i<257; ++i) if(frequency_table[i]) {
        Node no(i,frequency_table[i]);
        pq.push(no);
    }

    // Montagem da arvore
    Node * tree = build_tree_from_priority_queue(pq);

    // dicionario 
    map<char,string> dictionary;
    create_dictionary(dictionary, tree,"");
    print_dictionary(dictionary);

    string code = encode(text, dictionary);

    compress(code);

    //cout << "Conteudo descompactado" << endl;
    //cout << decompress(tree) << endl; 

    delete tree; 
}

