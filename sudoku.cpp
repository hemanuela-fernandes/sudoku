#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* Constantes */
#define ERROR_FILE_MSG	"Nao foi possivel abrir o arquivo!\n"
#define INVALID_OPTION	"Opcao invalida! Tente novamente!"

/* Definicoes de tipo */
enum boolean {
	FALSO=0, VERDADEIRO=1
};

/* Prototipos */
FILE* carregue(char quadro[9][9]);
FILE* carregue_continue_jogo (char quadro[9][9], char *nome_arquivo);
void carregue_novo_jogo(char quadro[9][9], char *nome_arquivo);
FILE* crie_arquivo_binario(char quadro[9][9]);
int determine_quadrante(int x, int y);
int eh_valido(const char quadro[9][9], int x, int y, int valor);
int eh_valido_na_coluna(const char quadro[9][9], int y, int valor);
int eh_valido_no_quadrante3x3(const char quadro[9][9], int x, int y, int valor);
int eh_valido_na_linha(const char quadro[9][9], int x, int valor);
int existe_posicao_vazia(const char quadro[9][9]);
void imprima(const char quadro[9][9]);
void jogue();
void resolve_completo(FILE*, char quadro[9][9]);
void resolve_um_passo(char quadro[9][9]);
void salve_jogada_bin(FILE* fb, char quadro[9][9]);

/* Funcoes auxiliares */
int fim_x(int quadr);
int fim_y(int quadr);
int leia_opcao();
void gen_random(char *s, const int len);
int ini_x(int quadr);
int ini_y(int quadr);
void menu();
void menu_arquivo();


/* -----------------------------------------------------------------------------
 * -----------------------------------------------------------------------------
 * MAIN
 * /////////////////////////////////////////////////////////////////////////////
 */
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
	opcao = leia_opcao();//retorna um int

	FILE *jogoCarregado;
	char nome_arquivo[30];

	// TODO Função incompleta

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
			printf("Digite o nome do arquivo do jogo que deseja ser continuado:\n");
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
	/*um arquivo binario com as informações de um jogo deve ser aberto
	 e continuado
	*/

	FILE *previousGame;

	strcat(nome_arquivo, ".bin");
	previousGame = fopen(nome_arquivo, "rb");
	if (previousGame == NULL) {
		printf("%s", ERROR_FILE_MSG);
		return NULL;
	}
	fseek(previousGame, -(sizeof(char)*81), SEEK_END);
	fread(quadro, sizeof(char), 81, previousGame);
	fclose(previousGame);

	return fopen(nome_arquivo , "rb+");
}

/* -----------------------------------------------------------------------------
 * CARREGAR NOVO JOGO
 * Carrega um novo jogo do Sudoku
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void carregue_novo_jogo(char quadro[9][9], char *nome_arquivo) {
	/*aqui deve ser escolhido um dos modelos de matriz em qrquivo txt
	 disponibilizados onde 0 sçao posições vazias.
	*/
	FILE *newGame;
	int i, j;

	newGame = fopen(strcat(nome_arquivo, ".txt"), "r");
	if (newGame == NULL) {
		printf("%s", ERROR_FILE_MSG); //ver se o arquivo abriu corretamente
		return;
	}
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			fscanf(newGame, "%d", &quadro[i][j]);
		}
	}
	fclose(newGame);

}

/* -----------------------------------------------------------------------------
 * CRIAR ARQUIVO BINARIO
 * Criar arquivo binario
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
FILE* crie_arquivo_binario(char quadro[9][9]) {
	/*ao inicio de um jogo, vamos criar um arquivo binario com nome aleatorio
	para guardar o jogo, para iso usamos a funcao gen_random. O arquivo tem formato: 
	um numero inteiro n e n+1 quadros
	*/

	FILE *arquivoBin;
	char nome[10];
	int jogada = 0;

	gen_random(nome, 5);
	strcat(nome,".bin");

	arquivoBin = fopen(nome, "wb");
	if(arquivoBin == NULL){
		printf("%s", ERROR_FILE_MSG);
		return NULL;
	}

	printf("\nJogada: %d.\n", jogada);

	/*coloca a matrix 9x9 no arquivo binario*/
	rewind(arquivoBin); // ter certeza que está no começo do arquivo.
	fwrite(&jogada, sizeof(int), 1, arquivoBin);
	fwrite(quadro, sizeof(quadro[0][0]), 81, arquivoBin);  // 81 é quantos elemento tem
	fclose(arquivoBin);

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
	/*precisamos checar os 9 elementos de uma coluna y e comparar com o int valor
	retorna FALSO ou VERDADEIRO.(tipos boolean definidos, FALSO = 0, VERDADEIRO = 1)
	*/

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
	/*precisamos checar os 9 elementos de uma linha x, comparando ao int valor
	retorna FALSO ou VERDADEIRO.(tipos boolean definidos, FALSO = 0, VERDADEIRO = 1)
	*/
	int i;

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
	/*precisamos checar todos os elementos do quadrante em q a posição
	(x,y) está. e comparar os int valor. Para achar o quadrante podemos usar a funcao 
	determine_quadrante para cada resultado. Assim tendo um começo e final diferente das [i][f]
	checadas, que podem se obter pelas funções ini_y, ini_x, fim_y e fim_x. 
	retornam FALSO ou VERDADEIRO.(tipos boolean definidos, FALSO = 0, VERDADEIRO = 1)
	*/
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

			if(fb == NULL){//verifica há um arquivo disponivel(aerto)
				printf("Nenhum arquivo disponivel para jogo!");
				break;
			}

			if(existe_posicao_vazia(quadro)){
				printf("Entre com a posicao e o valor (linha, coluna, valor): ");
				scanf("%d %d %d", &x, &y, &valor);


				if (eh_valido(quadro, x, y, valor)) {
					quadro[x][y] = valor;
					salve_jogada_bin(fb, quadro);
				} else{
					puts("Valor ou posicao incorreta! Tente novamente!");
				}

			} else{
				printf("Sem posições vazias!!");
			}

			break;
			}

		// resolva 1 passo
		case 3:
			if (existe_posicao_vazia(quadro)){
				resolve_um_passo(quadro);
				salve_jogada_bin(fb, quadro);
				puts("Um passo resolvido!");
			}else{
				printf("Sem possibilidade de realiza um passo!");
			}
			
			break;

		// resolva o sudoku completo
		case 4:
			resolve_completo(fb, quadro);
			break;

		case 9:
		if (fb!= NULL){
			printf("fechando arquivo");
			fclose(fb);
		} else{
			printf("não há arquivo para ser fechado!");
		}
			puts("\nPrograma finalizado!");
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
	printf("Jogo resolvido!");
}

/* -----------------------------------------------------------------------------
 * RESOLVER UM PASSO
 * Preenche apenas um campo vazio
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void resolve_um_passo(char quadro[9][9]) {
	int cont, resp;
	int i, j, tenta;
	cont = 0;//contador de respostas validas

	for(int i = 0; i<9;i++){
		for(int j = 0; j<9; j++){
			if(quadro[i][j] == 0){
				for (int tentativa = 1; tentativa <= 9; tentativa++) {
					if (eh_valido(quadro, i, j, tentativa)) {
						resp = tenta;
						cont ++;
					}
				}
				//se houver só uma resposta
				if (cont == 1){
					quadro[i][j] = resp;
					return;//para "parar" o programa
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
	int num_jogada;

	if (fb != NULL){
		fseek(fb, 0, SEEK_SET); //acha o começo do arquivo e atualiza a quantidade de jogadas
		fread(&num_jogada, sizeof(int), 1, fb);
		num_jogada++;
		fseek(fb, 0, SEEK_SET);
		fwrite(&num_jogada, sizeof(int), 1, fb); // atualiza a variavel num_jogada
		fseek(fb, -1 , SEEK_END);
		fwrite(quadro, sizeof(quadro[0][0]), 81, fb); //escreve um quadro novo

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
