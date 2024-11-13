#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* Alunos:
   Amanda Beatriz Matos Palacio Ribeiro
   Felipe Camargo Machado Figueiredo */

/* Constantes */
#define ERROR_FILE_MSG	"Nao foi possivel abrir o arquivo!\n"
#define INVALID_OPTION	"Opcao invalida! Tente novamente!"

/* Definicoes de tipo */
enum boolean {
	FALSO=0, VERDADEIRO=1
};

/* Prototipos */
FILE* carregue(char quadro[9][9]); //9
FILE* carregue_continue_jogo (char quadro[9][9], char *nome_arquivo); //8
void carregue_novo_jogo(char quadro[9][9], char *nome_arquivo); //5
FILE* crie_arquivo_binario(char quadro[9][9]); //6
int determine_quadrante(int x, int y);
int eh_valido(const char quadro[9][9], int x, int y, int valor); //4
int eh_valido_na_coluna(const char quadro[9][9], int y, int valor); //1
int eh_valido_no_quadrante3x3(const char quadro[9][9], int x, int y, int valor); //3
int eh_valido_na_linha(const char quadro[9][9], int x, int valor); //2
int existe_posicao_vazia(const char quadro[9][9]);
void imprima(const char quadro[9][9]);
void jogue();
void resolve_completo(FILE*, char quadro[9][9]); 
void resolve_um_passo(char quadro[9][9]); //10
void salve_jogada_bin(FILE* fb, char quadro[9][9]); //7

/* Funcoes auxiliares */
int fim_x(int quadr);
int fim_y(int quadr);
int leia_opcao();
void gen_random(char *s, const int len);
int ini_x(int quadr);
int ini_y(int quadr);
void menu();
void menu_arquivo();

int main() {

	// inicia o jogo
	jogue();

	return 0;
}

/* -----------------------------------------------------------------------------
 * CARREGAR
 * Inicializa o SUDOKU a partir de um novo jogo ou estado de jogo anterior
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
FILE* carregue(char quadro[9][9]) {
	int opcao;

	menu_arquivo();
	opcao = leia_opcao(); // A funacao leia opcao retorna um numero int

	FILE *jogoCarregado;
	char nome_arquivo[30];

	switch(opcao) {

		// carregar novo sudoku
		case 1:
			printf("Digite o nome do arquivo (sem seu formato):\n");
			scanf("%s", nome_arquivo);
			carregue_novo_jogo(quadro, nome_arquivo);
			jogoCarregado = NULL;
			return jogoCarregado;
		break;

		// continuar jogo
		case 2:
			printf("Digite o nome do arquivo (sem seu formato):\n");
			scanf("%s", nome_arquivo);
			jogoCarregado = carregue_continue_jogo(quadro, nome_arquivo);
		break;

		// retornar ao menu anterior
		case 9:
		break;

		default:
			printf("%s", INVALID_OPTION);
			return carregue(quadro);
		break;
	}
}

/* -----------------------------------------------------------------------------
 * CARREGAR CONTINUAR JOGO
 * Carrega um estado de jogo a partir de um arquivo binario
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
FILE* carregue_continue_jogo (char quadro[9][9], char *nome_arquivo) {
	/* A opção 2 é utilizada para continuar um jogo Sudoku. É solicitado o nome do arquivo
(binário) que você deseja continuar. Toda vez que você inicia um novo jogo é criado um arquivo
binário de nome aleatório para armazenar este jogo. Esse arquivo binário possui o seguinte
formato: um número inteiro, n, informando o número de jogadas e n + 1 quadros. 
 Cria-se o arquivo e já é salvo o estado inicial do quadro. */
	FILE *jogoAnterior;

	strcat(nome_arquivo, ".bin");
	jogoAnterior = fopen(nome_arquivo, "rb");
	if (jogoAnterior == NULL) {
		printf("%s", ERROR_FILE_MSG);
		return NULL;
	}
	fseek(jogoAnterior, -(sizeof(char)*81), SEEK_END);
	fread(quadro, sizeof(char), 81, jogoAnterior);
	fclose(jogoAnterior);

	return fopen(nome_arquivo , "rb+");
}

/* -----------------------------------------------------------------------------
 * CARREGAR NOVO JOGO
 * Carrega um novo jogo do Sudoku
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void carregue_novo_jogo(char quadro[9][9], char *nome_arquivo) {
	/* Nessa funcao, conforme a descricao do trabalho espera-se que seja exigido
	o nome de um arquivo txt contendo a matriz [9][9], sendo 0 a posicao vazia.
	Desejamos entao abrir o arquivo referenciado na entradas de dados da funcao
	e carregar no quadro de informacoes recebidos.*/
	FILE *novoJogo;

	novoJogo = fopen(strcat(nome_arquivo, ".txt"), "r");
	if (novoJogo == NULL) {
		printf("%s", ERROR_FILE_MSG); //Testar
		return;
	}
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			fscanf(novoJogo, "%d", &quadro[i][j]);
		}
	}
	fclose(novoJogo);
}

/* -----------------------------------------------------------------------------
 * CRIAR ARQUIVO BINARIO
 * Criar arquivo binario
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
FILE* crie_arquivo_binario(char quadro[9][9]) {
	/* Sempre que se inicia um jogo, cria-se um arquivo binario de nome aleatorio
	para armazenar o jogo. O nome pode ser obtido com a funcao gem_radom.
	Esse arquivo tem formato: um numero inteiro n (numJogadas), e n+1 
	quadros */
	FILE *jogoBin;
	char nome[10];
	int numJogadas = 0;

	gen_random(nome, 5);
	strcat(nome , ".bin");
	jogoBin = fopen(nome, "wb");
	if (jogoBin == NULL) {
		printf("%s", ERROR_FILE_MSG);
		return NULL;
	}
	fwrite(&numJogadas, sizeof(int), 1, jogoBin);
	fwrite(quadro, sizeof(quadro[0][0]), 81, jogoBin);
	fclose(jogoBin);

	return fopen(nome , "rb");

}

/* -----------------------------------------------------------------------------
 * DETERMINAR QUADRANTE
 * Dado as posicoes x e y, determina o quadrante do quadro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int determine_quadrante(int x, int y) {
	if (x < 3 && y < 3)
		return 1;
	else if (x < 3 && y < 6)
		return 2;
	else if (x < 3 && y < 9)
		return 3;
	else if (x < 6 && y < 3)
		return 4;
	else if (x < 6 && y < 6)
		return 5;
	else if (x < 6 && y < 9)
		return 6;
	else if (x < 9 && y < 3)
		return 7;
	else if (x < 9 && y < 6)
		return 8;
	else
		return 9;
}

/* -----------------------------------------------------------------------------
 * E VALIDO
 * Determina se um valor na posicao x e y eh valido
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int eh_valido(const char quadro[9][9], int x, int y, int valor) {

	// verifica as tres condicoes
	if (!eh_valido_na_coluna(quadro, y, valor))
		return FALSO;
	if (!eh_valido_na_linha(quadro, x, valor))
		return FALSO;
	if (!eh_valido_no_quadrante3x3(quadro, x, y, valor))
		return FALSO;

	return VERDADEIRO;
}

/* -----------------------------------------------------------------------------
 * E VALIDO NA COLUNA
 * Verifica se um valor na coluna y e valido
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int eh_valido_na_coluna(const char quadro[9][9], int y, int valor) {
	// Essa implementacao requer que cheque todos os nove elementos
    // de uma coluna y, comparandos-os com o int valor
    // seu retorno e um "inteiro"(tipo boolean definido) FALSO(0) ou VERDADEIRO(1)	
	for (int i = 0; i < 9; i++) {
        if (quadro[i][y] == valor) {
            return FALSO;
        }
    }
    return VERDADEIRO;
}

/* -----------------------------------------------------------------------------
 * E VALIDO NA LINHA
 * Verifica se um valor na linha x e valido
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int eh_valido_na_linha(const char quadro[9][9], int x, int valor) {
	// Essa implementacao requer que cheque todos os 9 elmentos de uma linha
    // x, comparando-os ao int valor
    // seu retorno e um "inteiro"(tipo boolean definido) FALSO(0) ou VERDADEIRO(1)
    for (int i = 0; i < 9; i++) {
        if (quadro[x][i] == valor) {
            return FALSO;
        }
    }
    return VERDADEIRO;
}

/* -----------------------------------------------------------------------------
 * E VALIDO NO QUADRO 3X3
 * Verifica se um valor e valido no quadrante da posicao x, y
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int eh_valido_no_quadrante3x3(const char quadro[9][9], int x, int y, int valor) {
	/*Essa implementacao requer que cheque todos os 9 elmentos de de um
    quadrante que a posicao (x,y) pertence, comparando-os ao int valor.
    Podemos descobrir o quadrante usando a funcao fornecida determine_quadrante,
    para cada resultado fornecido teremos um comeco e final distintos das 
    posicoes [i][f] checadas (lembrando que elas nao sao lineares, mas sao
    sempre combinacoes dos mesmos 3 numeros) que podem ser obtidos com
    as funcoes ini_x, ini_y, fim_x e fim_y
    seu retorno e um "inteiro"(tipo boolean definido) FALSO(0) ou VERDADEIRO(1)
    int quadrante = determine_quadrante(x, y); */
    int quadrante = determine_quadrante(x,y);
    int x_ini = ini_x(quadrante);
    int y_ini = ini_y(quadrante);
    int x_fim = fim_x(quadrante);
    int y_fim = fim_y(quadrante);

    for (int i = x_ini; i < x_fim; i++) {
        for (int j = y_ini; j < y_fim; j++) {
            if (quadro[i][j] == valor) {
                return FALSO;
            }
        }
    }
    return VERDADEIRO;
}

/* -----------------------------------------------------------------------------
 * EXISTE CAMPO VAZIO
 * Verifica se existe um campo nao preenchido
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int existe_posicao_vazia(const char quadro[9][9]) {
	int i, j;

	for(i = 0; i < 9; i++) {
		for(j = 0; j < 9; j++) {
			if (quadro[i][j] == 0)
				return VERDADEIRO;
		}
	}

	return FALSO;
}

/* -----------------------------------------------------------------------------
 * IMPRIMIR
 * Imprime o quadro recebido do sudoku
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void imprima(const char quadro[9][9]) {
	int i, j;

	puts("~~~~~~~~ SUDOKU ~~~~~~~~");
	puts("    0 1 2   3 4 5   6 7 8");
	for (i = 0; i < 9; i++) {
		if (i % 3 == 0)
			puts("  -------------------------");
		for (j = 0; j < 9; j++) {

			if (j == 0)
				printf("%d | ", i);
			else if (j % 3 == 0)
				printf("| ");

			if (quadro[i][j] == 0)
				printf("- ");
			else
				printf("%d ", quadro[i][j]);
		}
		puts("|");
	}
	puts("  -------------------------");
}

/* -----------------------------------------------------------------------------
 * JOGAR
 * Realiza toda a logica do jogo
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void jogue() {
	int opcao;
	char quadro[9][9] = { {0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0}
	};
	FILE *fb = NULL;

	opcao = 0;

	while (opcao != 9) {
		// imprima na tela o quadro atual
		imprima(quadro);

		// apresente um menu com as opcoes
		menu();
		opcao = leia_opcao();

		switch (opcao) {

		// carregue sudoku
		case 1:
			fb = carregue(quadro);

			if (fb == NULL) {
				fb = crie_arquivo_binario(quadro);
			}
			break;

		// preencha quadro com um valor
		case 2: {
			int x, y, valor;

			printf("Entre com a posicao e o valor (linha, coluna, valor): ");
			scanf("%d %d %d", &x, &y, &valor);


			if (eh_valido(quadro, x, y, valor)) {
				quadro[x][y] = valor;
				salve_jogada_bin(fb, quadro);
			}
			else
				puts("Valor ou posicao incorreta! Tente novamente!");
		}
			break;

		// resolva 1 passo
		case 3:
			resolve_um_passo(quadro);
			salve_jogada_bin(fb, quadro);
			puts("Um passo resolvido!");
			break;

		// resolva o sudoku completo
		case 4:
			resolve_completo(fb, quadro);
			break;

		case 9:
			puts("Programa finalizado ..");
			break;

		default:
			puts(INVALID_OPTION);
			break;
		}
	}
}

/* -----------------------------------------------------------------------------
 * RESOLVER
 * Resolve o sudoku
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void resolve_completo(FILE *fb, char quadro[9][9]) {
	while(existe_posicao_vazia(quadro)) {
		resolve_um_passo(quadro);
		salve_jogada_bin(fb, quadro);
	}
	return;
}

/* -----------------------------------------------------------------------------
 * RESOLVER UM PASSO
 * Preenche apenas um campo vazio
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void resolve_um_passo(char quadro[9][9]) {
	int contador = 0;
	int resposta;
	

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (quadro[i][j] == 0) {
				int contador = 0;
				for (int tentativa = 1; tentativa <= 9; tentativa++) {
					if (eh_valido(quadro, i, j, tentativa)) {
						contador++;
						resposta = tentativa;
					}
				}
				if (contador == 1) {
					quadro[i][j] = resposta;
					return;
				}
			}
		}
	}
}

/* -----------------------------------------------------------------------------
 * SALVAR JOGADA BINARIO
 * Salva o estado atual do quadro no arquivo binario
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void salve_jogada_bin (FILE *fb, char quadro[9][9]) {
	/* Essa funcao deve salvar o quadro n + 1 da jogada e atualizar o numero de 
	jogadas feitas */
	int numJogadas = 0; //Numero de jogadas feitas
	
	if (fb != NULL) {
		fseek(fb, 0 , SEEK_SET); //Acha o começo do arquivo para atualizar o numero de jogadas
		fread(&numJogadas, sizeof(int), 1, fb);
		numJogadas++;
		fseek(fb, 0 , SEEK_SET);
		fwrite(&numJogadas, sizeof(int), 1, fb); //Atualiza numJogadas
		fseek(fb, -1 , SEEK_END);
		fwrite(quadro, sizeof(quadro[0][0]), 81, fb);//escreve novo quadro
	}
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 							FUNCOES AUXILIARES
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/* -----------------------------------------------------------------------------
 * FIM X
 * Indice final da linha para o quadrante recebido como parametro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int fim_x(int quadr) {
	switch(quadr) {
		case 1:
		case 2:
		case 3:
			return 2;

		case 4:
		case 5:
		case 6:
			return 5;

		default:
			return 8;
	}
}

/* -----------------------------------------------------------------------------
 * FIM Y
 * Indice final da coluna para o quadrante recebido como parametro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int fim_y(int quadr) {
	switch(quadr) {
		case 1:
		case 4:
		case 7:
			return 2;

		case 2:
		case 5:
		case 8:
			return 5;

		default:
			return 8;
	}
}

/* -----------------------------------------------------------------------------
 * GEN_RANDOM
 * Gera uma cadeia de caracteres randomica de tamanho len
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void gen_random(char *s, const int len) {
	srand(time(NULL));
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
	int i;

    for (i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

/* -----------------------------------------------------------------------------
 * INI X
 * Indice inicial da linha para o quadrante recebido como parametro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int ini_x(int quadr) {
	switch(quadr) {
		case 1:
		case 2:
		case 3:
			return 0;

		case 4:
		case 5:
		case 6:
			return 3;

		default:
			return 6;
	}
}

/* -----------------------------------------------------------------------------
 * INI Y
 * Indice inicial da coluna para o quadrante recebido como parametro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int ini_y(int quadr) {
	switch(quadr) {
		case 1:
		case 4:
		case 7:
			return 0;

		case 2:
		case 5:
		case 8:
			return 3;

		default:
			return 6;
	}
}

/* -----------------------------------------------------------------------------
 * LE OPCAO
 * Imprime a mensagem a faz a leitura da opcao
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int leia_opcao () {
	int opcao;

	printf("Opcao: ");
	scanf("%d", &opcao);

	return opcao;
}

/* -----------------------------------------------------------------------------
 * MENU
 * Imprime o menu de opcoes
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void menu() {
	puts("\n~~~~~~~~ SUDOKU ~~~~~~~~");
	puts("[1] - Carregar jogo");
	puts("[2] - Jogar");
	puts("[3] - Resolver um passo");
	puts("[4] - Resolver");
	puts("[9] - Finalizar");
	puts("--------");
}

/* -----------------------------------------------------------------------------
 * MENU ARQUIVO
 * Imprime o menu de opcoes do arquivo
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void menu_arquivo() {
	puts("\n~~~~~ MENU ARQUIVO ~~~~~");
	puts("[1] - Novo jogo");
	puts("[2] - Continuar jogo");
	puts("[9] - Retornar ao menu anterior");
	puts("--------");
}
