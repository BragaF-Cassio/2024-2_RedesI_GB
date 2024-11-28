#include "gametcp.h"

int retRecv;
int loc_sockfd, sockNum, tamanho;
int conectado = 0;
char data_array[ECHOMAX];
int player = 0;

int main(int argc, char *argv[]) {
    
	if (argc != 3) {
		printf("Parametros:<remote_ip> <port> \n");
		exit(1);
	}

    printf("\nVocê sera Jogador 1 ou Jogador 2? ");
    scanf(" %d", &player);
    fgets(data_array, ECHOMAX, stdin);

    if(player < 1 || player > 2) {
        printf("\nJogador inválido!");
        return 0;
    }
    printf("\nJogador %d selecionado!\n", player);

	game = CONECTANDO;

    if(player == 1) {
        create_server(atoi(argv[2]));
    } else {
        create_client(argv[1], atoi(argv[2]));
    }

    if(!conectado) {
        printf("\nFalha ao conectar, saindo...\n");
        return 0;
    }
    printf("> Jogador conectou!\n");

	game = CONECTANDO;
	limpa_tabuleiros();

    if(player == 1) {
		game = POSICIONANDO_BARCOS;
        tcp_send_data();
        if(strncmp(data_array,"sair", 4) == 0) {
            printf("\nEncerrando aplicacao...\n");
            return 0;
        } 
    }
	game = AGUARDANDO_POSICIONAR_BARCOS;

    do {
		tcp_read_data();
		if(strncmp(data_array,"sair", 4) == 0) break;

		tcp_send_data();
		
	} while(strncmp(data_array,"sair", 4));

    if(player == 1) {
        /* fechamento do socket local */ 
        close(loc_sockfd);
    }
    printf("\nEncerrando aplicacao...\n");
	close(sockNum);

	return 0;
}

int create_client(char *host, int port) {

	/* ESTACAO REMOTA */
	char *rem_hostname;
	int rem_port;
	/* Estrutura: familia + endereco IP + porta */
	struct sockaddr_in rem_addr;
	int msgAdd = 1;
	int retRecv;

	/* Construcao da estrutura do endereco local */
	/* Preenchendo a estrutura socket loc_addr (familia, IP, porta) */
	rem_hostname = host;
	rem_port = port;
	rem_addr.sin_family = AF_INET; /* familia do protocolo*/
	rem_addr.sin_addr.s_addr = inet_addr(rem_hostname); /* endereco IP local */
	rem_addr.sin_port = htons(rem_port); /* porta local  */

   	/* Cria o socket para enviar e receber datagramas */
	/* parametros(familia, tipo, protocolo) */
	sockNum = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sockNum < 0) {
		perror("Criando stream socket");
		exit(1);
	}
	printf("> Conectando no servidor '%s:%d'\n", rem_hostname, rem_port);

   	/* Estabelece uma conexao remota */
	/* parametros(descritor socket, estrutura do endereco remoto, comprimento do endereco) */
	if (connect(sockNum, (struct sockaddr *) &rem_addr, sizeof(rem_addr)) < 0) {
		perror("Conectando stream socket");
		exit(1);
	}

    conectado = 1;
}

int create_server(int receivePort) {

	/* Estrutura: familia + endereco IP + porta */
	struct sockaddr_in loc_addr;

   	/* Cria o socket para enviar e receber datagramas */
	/* parametros(familia, tipo, protocolo) */
	loc_sockfd = socket(AF_INET, SOCK_STREAM, 0);	
	
	if (loc_sockfd < 0) {
		perror("Criando stream socket");
		exit(1);
	}

	/* Construcao da estrutura do endereco local */
	/* Preenchendo a estrutura socket loc_addr (familia, IP, porta) */
	loc_addr.sin_family = AF_INET; /* familia do protocolo*/
	loc_addr.sin_addr.s_addr = INADDR_ANY; /* endereco IP local */
	//loc_addr.sin_port = htons(atoi(argv[1])); /* porta local  */
	loc_addr.sin_port = htons(receivePort); /* porta local  */
	bzero(&(loc_addr.sin_zero), 8);

   	/* Bind para o endereco local*/
	/* parametros(descritor socket, estrutura do endereco local, comprimento do endereco) */
	if (bind(loc_sockfd, (struct sockaddr *) &loc_addr, sizeof(struct sockaddr)) < 0) {
		perror("Ligando stream socket");
		exit(1);
	}
	
	/* parametros(descritor socket,
	numeros de conexoes em espera sem serem aceites pelo accept)*/
	listen(loc_sockfd, 5);
	printf("> Aguardando jogador entrar na partida...\n");

	tamanho = sizeof(struct sockaddr_in);
   	/* Accept permite aceitar um pedido de conexao, devolve um novo "socket" ja ligado ao emissor do pedido e o "socket" original*/
	/* parametros(descritor socket, estrutura do endereco local, comprimento do endereco)*/
    sockNum = accept(loc_sockfd, (struct sockaddr *)&loc_addr, &tamanho);

    conectado = 1;
}

void tcp_read_data() {
	if(game == AGUARDANDO_POSICIONAR_BARCOS) {
		printf("\nAguardando barcos adversarios...\n");
	} else {
		printf("\nAguardando resposta...\n");
	}
    
    /* parametros(descritor socket, endereco da memoria, tamanho da memoria, flag) */
    memset(data_array, '\0', sizeof(data_array)); 
    retRecv = recv(sockNum, &data_array, sizeof(data_array), 0);
    if(retRecv > 0) {
        data_array[retRecv] = '\0';
		if(game == AGUARDANDO_POSICIONAR_BARCOS) {
			ler_do_json(data_array);
			if(player == 1) {
				game = MEU_TURNO;
			} else {
				game = POSICIONANDO_BARCOS;
			}
		} else if(game == AGUARDANDO_TURNO_ADVERSARIO) {
			
			int tiroX = data_array[0] - '0';
			int tiroY = data_array[1] - '0';

			printf("Inimigo atirou em %dx%d\n", tiroX, tiroY);
			if(atira(&meuTabuleiro[0][0], &tabelaTirosInimigo[0][0], tiroY, tiroX) > 0 && minhasCasasAcertadas < TOTAL_CASAS) {
				inimigoCasasAcertadas++;
				printf("Inimigo acertou um barco!\n");
			} else {
				printf("Inimigo errou!\n");
			}

			//mostrar_tabuleiro(&tabelaTiros[0][0]);

			if(inimigoCasasAcertadas >= TOTAL_CASAS) {
				printf("\nQUE PENA, VOCÊ PERDEU!\n");
			}

			game = MEU_TURNO;
		} else {
			printf("Recebi %s\n", data_array);
		}
        
    } else {
        printf("Um erro ocorreu no recebimento, finalizando...\n");
        sprintf(data_array, "sair");
    }
}

void tcp_send_data() {
	int ret;
    int len;

	if(!conectado) {
		printf("Aguarda a conexão para enviar dados.\n");
		return;
	}

	printf("\n> ");

    memset(data_array, 0, sizeof(data_array)); 

	if(game == POSICIONANDO_BARCOS) {
		gera_posicoes_barcos();

		ret = send(sockNum, json_meus_barcos, strlen(json_meus_barcos), 0);

		if(player == 1) {
			game = AGUARDANDO_POSICIONAR_BARCOS;
		} else {
			game = AGUARDANDO_TURNO_ADVERSARIO;
		}
	} else {
		int tiroX, tiroY;

		do{
			printf("\nAtire em (x y): ");
			fflush(stdin);
			scanf("%d %d", &tiroX, &tiroY);
			fgets(data_array, ECHOMAX, stdin);
			len = strcspn(data_array, "\n");
			if(len > 0) data_array[len] = '\0';
		} while (tiroX < 0 || tiroX > 9 || tiroY < 0 || tiroY > 9);
		
		if(atira(&tabuleiroAdversario[0][0], &tabelaTiros[0][0], tiroY, tiroX) > 0 && inimigoCasasAcertadas < TOTAL_CASAS) {
			minhasCasasAcertadas++;
		}

		mostrar_tabuleiro(&tabelaTiros[0][0]);

		if(minhasCasasAcertadas >= TOTAL_CASAS) {
			printf("\nPARABÉNS, VOCÊ VENCEU!\n");
		}

		sprintf(data_array, "%d%d\0", tiroX, tiroY);

		ret = send(sockNum, data_array, strlen(data_array), 0);

		game = AGUARDANDO_TURNO_ADVERSARIO;
	}
	
	if(ret <= 0) {
		printf("Falha ao enviar dados!");
		sprintf(data_array, "sair");
	}

	return;
}