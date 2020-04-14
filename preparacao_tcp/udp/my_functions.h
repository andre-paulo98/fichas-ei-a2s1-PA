/**
 * @file my_functions.c
 * @brief Library feita com base nos auxiliares fornecidos
 * @date 12/12/2019
 * @author Andre Carvalho - 2181840
 * @author Andre Paulo - 2181841
*/

#ifndef MY_FUNCTION_H
#define MY_FUNCTION_H
/**************************************/
/*               UDP                  */
/**************************************/

#define UPD_OPEN_ERROR 20

/**
 * @brief Cria e devolve um descritor para um socket da família AF_INET (IPv4) UDP
 *
 * @return int Descritor para socket
 */
int my_udp_socket(void);



/**************************************/
/*               TCP                  */
/**************************************/

#define TCP_OPEN_ERROR 30
#define TCP_LISTEN_ERROR 31
#define TCP_ACCEPT_ERROR 32
#define TCP_CONNECT_ERROR 33
#define TCP_CLI_INFO_ERROR 34

/**
 * @brief Cria e devolve um descritor para um socket da família AF_INET (IPv4)
 *
 * @return int Descritor para socket
 */
int my_tcp_socket(void);

/**
 * @brief Obtem a informaçoes relativas ao socket conectado e preenche a extrutura client
 *
 * @param socket Socket a que se refere
 * @return struct sockaddr_in Extrutura a ser preenchida com as informaçoes da ligação
 */
struct sockaddr_in  my_tcp_client_info(int socket);

/**
 * @brief A função my_tcp_listen() serve para indicar que o socket servidor já está disponível
 * para receber pedidos de ligações no socket.
 * Para além disso, o parâmetro queue_max permite definir o tamanho da fila de espera de
 * ligações aceites para o socket. Apesar de este parâmetro permitir definir esse
 * limite, a sua interpretação poderá ser diferente entre diferentes plataformas e sistemas
 * operativos.
 *
 * @param socket Socket do qual pertende obter informação
 * @param queue_max Numero maximo de ligaçoes em espera
 * @return int Sucesso –> 0 | Insucesso –> -1 e a variável errno é preenchida com o código de erro correspondente.
 */
int my_tcp_listen(int socket, int queue_max);

/**
 * @brief A função my_tcp_accept(), tal como o nome sugere, aceita as ligações dos clientes que se encontram
 * na lista de espera. Esta função cria e devolve um descritor para um novo socket conectado que passará a
 * ser utilizado para comunicar com o cliente que iniciou a ligação. Para além disso, a função preenche a estrutura
 * indicada com o endereço e o porto do cliente (client), caso os valores passados não sejam NULL. Este descritor
 * será utilizado para comunicar com o cliente que iniciou a ligação, sendo um canal de comunicação um para um.
 *
 * @param socket Socket do servidor
 * @param client Estrutura que indicada com o endereço e o porto do cliente
 * @return int Sucesso –> int >= 0, identificador para o socket do cliente | Insucesso –> -1 e a variável errno é preenchida com o código de erro correspondente
 */
int my_tcp_accept(int socket, struct sockaddr_in *client);

/**
 * @brief Esta função liga um cliente ao servidor indicado através do endereço e porto definidos na variável server.
 *
 * @param socket Socket do cliente
 * @param server Informações do servidor
 */
void my_connect(int socket, struct sockaddr_in * server);




/**************************************/
/*            COMMON                  */
/**************************************/
#define IP_CONVERT_ERROR 10
#define BIND_ERROR 11
#define CLOSE_SOCKET_ERROR 12
#define SEND_ERROR 13
#define RECV_ERROR 14

/**
 * @brief Preenche a estrutura com o IP, Porto e família de endereço
 * @param ip para utilizar
 * @param port para utilizar
 * @return estrutura configurada
 */
struct sockaddr_in my_set_server_info(const char *ip, short port);
/**
 * @brief A função bind permite mapear um socket para um determinado porto e interface de rede local
 * @param socket descritor do socket a mapear.
 * @param interface para fazer bind do socket, ex: INADDR_ANY
 * @param port porta para ficar a ouvir
 */
void my_bind(int socket, in_addr_t interfaces, short port);
/**
 * @brief A função my_send permite enviar mensagens ou dados através de um socket.
 * @param socket Descritor do socket
 * @param buffer Endereço no qual o conteúdo do datagram é escrito
 * @param length – tamanho da zona de memória de buf. É necessário ter em conta que o tamanho máximo teórico de um datagram é de 64 KiB, por isso, o tamanho do  parâmetro buf deve ser igual ou inferior a este valor (o valor mais apropriado seria o do tamanho da MTU da rede para minimizar a perda de pacotes).
 * @param flags permite aceder a modos alternativos das funções. Por exemplo, a opção MSG_DONTWAIT pode ser usada na função recvfrom para que esta deixe de ser bloqueante.
 * @param orig_addr Estrutura que indica o endereço associado ao datagram. Na operação de leitura, o parâmetro contém o endereço e porto de origem do datagram recebido. O endereço pode depois ser usado, por exemplo, para o servidor enviar a resposta ao cliente
 * @return Sucesso – devolve o número de bytes recebidos, no caso da função recvfrom. Insucesso – devolve -1
 */
ssize_t my_send(int socket, const void *buffer, size_t length, int flags);
/**
 * @brief A função my_receive permite receber mensagens ou dados através um socket
 * @param socket Descritor do socket
 * @param buffer Endereço no qual o conteúdo do datagram é lido
 * @param length – tamanho da zona de memória de buf. É necessário ter em conta que o tamanho máximo teórico de um datagram é de 64 KiB, por isso, o tamanho do  parâmetro buf deve ser igual ou inferior a este valor (o valor mais apropriado seria o do tamanho da MTU da rede para minimizar a perda de pacotes).
 * @param flags permite aceder a modos alternativos das funções.
 * @return Sucesso – devolve o número de bytes recebidos, no caso da função recvfrom. Insucesso – devolve -1
 */
ssize_t my_receive(int socket, void *buffer, size_t length, int flags);

/**
 * @brief A função my_receive permite receber mensagens ou dados através um socket
 * @param socket Descritor do socket
 * @param buffer Endereço no qual o conteúdo do datagram é lido
 * @param length – tamanho da zona de memória de buf. É necessário ter em conta que o tamanho máximo teórico de um datagram é de 64 KiB, por isso, o tamanho do  parâmetro buf deve ser igual ou inferior a este valor (o valor mais apropriado seria o do tamanho da MTU da rede para minimizar a perda de pacotes).
 * @param flags permite aceder a modos alternativos das funções.
 * @param orig_addr estrutura que indica o endereço associado ao datagram. Na operação de escrita, o parâmetro especifica o endereço e porto de destino para onde se pretende enviar o datagram.
 * @return Sucesso – devolve o número de bytes recebidos, no caso da função recvfrom. Insucesso – devolve -1
 */
ssize_t my_send_to(int socket, const void *buffer, size_t length, int flags, struct sockaddr_in *dest_addr);

/**
 * @brief A função my_receive_from permite receber mensagens ou dados através um socket
 * @param socket Descritor do socket
 * @param buffer Endereço no qual o conteúdo do datagram é escrito
 * @param length – tamanho da zona de memória de buf. É necessário ter em conta que o tamanho máximo teórico de um datagram é de 64 KiB, por isso, o tamanho do  parâmetro buf deve ser igual ou inferior a este valor (o valor mais apropriado seria o do tamanho da MTU da rede para minimizar a perda de pacotes).
 * @param flags permite aceder a modos alternativos das funções. Por exemplo, a opção MSG_DONTWAIT pode ser usada na função recvfrom para que esta deixe de ser bloqueante.
 * @param orig_addr Estrutura que indica o endereço associado ao datagram. Na operação de leitura, o parâmetro contém o endereço e porto de origem do datagram recebido. O endereço pode depois ser usado, por exemplo, para o servidor enviar a resposta ao cliente
 * @return Sucesso – devolve o número de bytes recebidos, no caso da função recvfrom. Insucesso – devolve -1
 */
ssize_t my_receive_from(int socket, void *buffer, size_t length, int flags, struct sockaddr_in *orig_addr);

/**
 * @brief Fechar o socket
 * @param socket para fechar
 */
void my_close(int socket);
#endif