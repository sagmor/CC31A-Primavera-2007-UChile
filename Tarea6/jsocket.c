#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <strings.h> 
#include <string.h>

#include "jsocket.h"


/*
 * Retorna un socket para conexion
 */

int j_socket()
{
    int sz = 1;
    int fd; 

    /* fd = socket(PF_INET, SOCK_STREAM, 0); */
		if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			return -1;
		}
		
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &sz, sizeof(int)) == -1) {
			return -1;
    }

    return fd;
}

static struct sockaddr_in portname;

/*
 * Pone un "nombre" (port) a un socket
 * y lo prepara para recibir conexiones
 * retorna 0 si OK, -1 si no
 */

int j_bind(int s, int port)
{
	/* ponemos el nombre */
	bzero(&portname, sizeof portname); 
	portname.sin_port = htons(port);
	portname.sin_family = AF_INET;
	portname.sin_addr.s_addr = INADDR_ANY;

	/* lo asociamos a el socket */
	if( bind(s, (struct sockaddr *) &portname, sizeof portname) != 0)
		return -1;

	if (listen(s, 5)==-1)
		return -1;

	return(0);
}

/*
 * Acepta una conexion pendiente o se bloquea esperando una
 * retorna un fd si OK, -1 si no
 */
int j_accept(int s)
{
	struct sockaddr_in from;
	unsigned int size = 0;

	return accept(s, (struct sockaddr *) &from, &size) ;
}

/*
 * Se conecta con un port conocido
 * retorna 0 si OK, -1 si no
 */
int j_connect(int s, const char *host, int port)
{
	struct hostent *hp;
	int i;

	/* Traducir nombre a direccion IP */
	hp = gethostbyname(host);
	if( hp == NULL )
		return(-1);

	/* Especificar port del servidor */
	bzero(&portname, sizeof portname);
	portname.sin_port = htons(port);
	portname.sin_family = AF_INET;

	/* Trato de conectarme con todas las direcciones IP del servidor */ 
	for(i=0; hp->h_addr_list[i] != NULL; i++) {
		bcopy( hp->h_addr_list[i], &portname.sin_addr.s_addr, hp->h_length);

		if(connect(s, (struct sockaddr *)&portname, sizeof portname) == 0)
			return(0);
	}                                                 

	/* No logre' conectarme */
	return(-1);
}
