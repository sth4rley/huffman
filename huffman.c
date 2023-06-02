#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM 256

typedef struct node {
  unsigned char caracter;
  unsigned int freq;
  struct node *next;
  struct node *right;
  struct node *left;
} NODE;

typedef struct {
  NODE *front;
  unsigned int size;
} PriorityQueue;

unsigned int * init_freq_table(unsigned char * text) {
  unsigned int * t = calloc(sizeof(int),TAM);
  for (int i = 0; i < TAM; i++) t[i] = 0; // init
  for (int i = 0; text[i]; i++) t[text[i]]++; // fill
  return t;

}

char is_empty(PriorityQueue *queue) { return !(queue->front); }

// Insere um nó na fila de prioridade
void enqueue(PriorityQueue *queue, NODE *node) {
  if (is_empty(queue)) {
    queue->front = node;
  }
  // Se o novo nó tiver prioridade menor que o primeiro elemento da fila
  else if (node->freq < queue->front->freq) {
    node->next = queue->front;
    queue->front = node;
  }

  // Se o novo nó tiver prioridade maior que o primeiro elemento da fila
  else {
    NODE *p = queue->front;
    while (p->next && p->next->freq < node->freq)
      p = p->next;
    node->next = p->next;
    p->next = node;
  }

  queue->size++;
}

PriorityQueue * init_queue_from_freq_table(unsigned int *freq_table) {

  PriorityQueue * queue = malloc(sizeof(PriorityQueue));
// isso aq preenche a fila com os caracteres que aparecem no texto
  // init
  queue->front = NULL;
  queue->size = 0;

  // fill
  for (int i = 0; i < TAM; i++)
    if (freq_table[i] > 0) {
      NODE *new = (NODE *)malloc(sizeof(NODE));
      new->caracter = i;
      new->freq = freq_table[i];
      new->right = new->left = new->next = NULL;
      enqueue(queue, new);
    }
  
  return queue; 

}

// remove o primeiro elemento da fila e retorna-o
NODE *dequeue(PriorityQueue *queue) {
  // Se a fila estiver vazia retorna NULL
  NODE *aux = NULL;

  if (queue->front) {
    // Remove o primeiro elemento da fila
    aux = queue->front;
    queue->front = aux->next;
    aux->next = NULL;
    queue->size--;
  }

  // Retorna o nó removido
  return aux;
}

NODE *build_tree_from_priority_queue(PriorityQueue *queue) {
  while (queue->size > 1) {
    // remove os dois primeiros elementos da fila de prioridade e os armazena em
    // primeiro e segundo
    NODE * first = dequeue(queue);
    NODE * second = dequeue(queue);
    // cria um novo nó e o insere na fila de prioridade ordenada de acordo com a
    // sua frequencia (prioridade)
    NODE * new = malloc(sizeof(NODE));

    new->freq = first->freq + second->freq;
    new->left = first;
    new->right = second;
    new->next = NULL;
    enqueue(queue, new);
  }
  return queue->front;
}

// determina a altura da arvore
int tree_height(NODE *root) {
  if (!root)
    return -1;
  else {
    int left = tree_height(root->left) + 1;
    int right = tree_height(root->right) + 1;
    if (left > right)
      return left;
    else
      return right;
  }
}

// aloca o dicionario
unsigned char **allocate_dictionary(unsigned int column) {
  unsigned char **dictionary = malloc(sizeof(char *) * TAM);
  for (int i = 0; i < TAM; i++) dictionary[i] = calloc(column, sizeof(char));
  return dictionary;
}

void generate_dictionary_from_tree(unsigned char **dictionary, NODE *root, char *path, unsigned int column) {
  char left[column], right[column];
  if (root->left == NULL && root->right == NULL) {
    strcpy((char *)dictionary[root->caracter], path);
  } else {
    strcpy(left, path);
    strcpy(right, path);

    strcat(left, "0");
    strcat(right, "1");

    generate_dictionary_from_tree(dictionary, root->left, left, column);
    generate_dictionary_from_tree(dictionary, root->right, right, column);
  }
}

// retorna o tamanho do codigo do texto usando o dicionario passado como
// parametro
int calculate_string_size(char **dictionary, unsigned char *text) {
  int tam = 0;
  for (int i = 0; text[i]; i++)
    tam += strlen(dictionary[text[i]]);
  return tam + 1;
}

unsigned char *encode_text_from_dictionary(unsigned char **dictionary, unsigned char *text) {
  unsigned char *code = calloc(calculate_string_size((char **)dictionary, text), sizeof(char));
  for (int i = 0; text[i]; i++) strcat((char*)code, (char *) dictionary[text[i]]);
  return code;
}

unsigned char *decode_text_from_tree(unsigned char *code, NODE *root) {
  NODE *aux = root;
  unsigned char *text = calloc(strlen((char*)code), sizeof(char));
  for (int i = 0; code[i]; i++) {
    // se for 0 vai para a esquerda
    if (code[i] == '0')
      aux = aux->left;
    // se for 1 vai para a direita
    else
      aux = aux->right;
    // se for uma folha
    if (aux->left == NULL && aux->right == NULL) {
      char string_of_char[2];
      string_of_char[0] = aux->caracter;
      string_of_char[1] = '\0';
      strcat((char *)text, string_of_char);
      aux = root;
    }
  }
  return text;
}

void compress(unsigned char *code) {
  int i = 0, j = 7;
  unsigned char mascara, byte = 0;
  FILE *file = fopen("output.huff", "wb");

  if (file) {
    while (code[i]) {
      mascara = 1;
      if (code[i++] == '1') {
        mascara = mascara << j;
        byte = byte | mascara;
      }
      j--;

      if (j < 0) {
        fwrite(&byte, sizeof(unsigned char), 1, file);
        byte = 0;
        j = 7;
      }
    }
    if (j != 7)
      fwrite(&byte, sizeof(unsigned char), 1, file);
    fclose(file);
  } else {
    printf("\n\nErro ao abrir/crair arquivo em compactar\n");
  }
}

unsigned int bit_is_set(unsigned char byte, int i) {
  unsigned char mascara = (1 << i);
  return byte & mascara;
}

void decompress(NODE *root) {
  FILE *file = fopen("output.huff", "rb");
  if (file) {
    NODE *aux = root;
    unsigned char byte;
    while (fread(&byte, sizeof(unsigned char), 1, file)) {
      for (int i = 7; i >= 0; i--) {
        if (bit_is_set(byte, i))
          aux = aux->right;
        else
          aux = aux->left;

        if (!aux->left && !aux->right) {
          printf("%c", aux->caracter);
          aux = root;
        }
      }
    }
    fclose(file);
  } else {
    printf("\nErro ao abrir arquivo em desccompactar\n");
  }
}

int input_size() {
  FILE *arq = fopen("input.txt", "r");
  int tam = 0;
  if (arq) {
    while (fgetc(arq) != -1)
      tam++;
    fclose(arq);
  } else
    printf("\nErro ao abrir arquivo em descobrir_tamanho \n");

  return tam;
}

unsigned char *get_input() {
  unsigned char *text = calloc(input_size() + 2, sizeof(unsigned char));
  FILE *arq = fopen("input.txt", "r");
  char letra;
  int i = 0;
  if (arq) {
    while (!feof(arq)) {
      letra = fgetc(arq);
      if (letra != -1) {
        text[i++] = letra;
      }
    }
    fclose(arq);
    return text;
  } else {
    printf("\nErro ao abrir arquivo em ler texto\n");
    return NULL;
  }
}

void free_dictionary(unsigned char **dictionary) {
  for (int i = 0; i < TAM; i++) {
    if (dictionary[i])
      free(dictionary[i]);
  }
  free(dictionary);
}

void free_tree(NODE *root) {
  if (root) {
    free_tree(root->left);
    free_tree(root->right);
    free(root);
  }
}

int main(void) {
  
  unsigned char *texto_entrada = get_input();
  unsigned int * tabela_frequencia = init_freq_table(texto_entrada);
  
  // MONTAGEM DA FILA DE PRIORIDADE A PARTIR DA TABELA DE FREQUENCIA
  PriorityQueue * queue = init_queue_from_freq_table(tabela_frequencia);
  NODE *arvore = build_tree_from_priority_queue(queue);
  unsigned int colunas = tree_height(arvore) + 1; // numero de colunas da matriz dicionario
  
  // MONTAGEM DO DICIONARIO DE CODIFICACAO A PARTIR DA ARVORE DE HUFFMAN
  unsigned char ** dicionario = allocate_dictionary(colunas);
  generate_dictionary_from_tree(dicionario, arvore, "", colunas);

  unsigned char * codigo = encode_text_from_dictionary(dicionario, texto_entrada);
  unsigned char * texto_decodificado = decode_text_from_tree(codigo, arvore);

  compress(codigo); // cria o arquivo codificiado


  // debug
  // printf("\nTEXTO DE ENTRADA: %s\n\n", texto_entrada);
  // printf("\n CODIGO: %s\n", codigo);
  // printf("\nTEXTO DECODIFICADO: %s\n", texto_decodificado);
  
  // DESCOMPACTACAO DO ARQUIVO COMPACTADO
  printf("\nARQUIVO DESCOMPACTADO!\n");
  printf("CONTEUDO DO ARQUIVO: \n");
  decompress(arvore);
  printf("\n\n");

  // LIBERACAO DE MEMORIA
  free(texto_entrada);
  free(codigo);
  free(texto_decodificado);
  free_dictionary(dicionario);
  free_tree(arvore);
}
