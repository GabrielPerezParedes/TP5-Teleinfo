#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // port to start the server on
    int SERVER_PORT = 8877;

    // socket address used for the server
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;

    // htons: host to network short: transforms a value in host byte
    // ordering format to a short value in network byte ordering format
    server_address.sin_port = htons(SERVER_PORT);

    // htons: host to network long: same as htons but to long
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    // create a UDP socket, creation returns -1 on failure
    int sock;
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("could not create socket\n");
        perror("Error: ");
        return 1;
    }

    // bind it to listen to the incoming connections on the created server
    // address, will return -1 on error
    if ((bind(sock, (struct sockaddr *)&server_address,
              sizeof(server_address))) < 0) {
        printf("could not bind socket\n");
        perror("Error: ");
        return 1;
    }

    // socket address used to store client address
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);;

    // iniciamos variables utiles
    int next = 1; // siguiente del seq que recibimos
    int totalMensajes = 0; // total_received
    int correct = 0;
    int incorrect = 0;
    //char datos[500]; // data
    int seqBin = 0; // #seq
    // run indefinitely
    while (seqBin<1000) { // para capturar solamente 1000 paquetes
        char buffer[500];
        //next = seqBin + 1;
        totalMensajes = totalMensajes + 1; // recibimos un nuevo mensaje

        // read content into buffer from an incoming client
        int len = recvfrom(sock, buffer, sizeof(buffer), 0,
                           (struct sockaddr *)&client_address,
                           &client_address_len);

        //memcpy(datos, buffer, sizeof(buffer)-1);

        // inet_ntoa prints user friendly representation of the
        // ip address
        buffer[len] = '\0';
        //uint16_t seqBin = ((uint16_t)buffer[0] << 8) | buffer[1];
        seqBin = (uint8_t)buffer[1] | (uint8_t)buffer[0] << 8;
        //////////////////////////////////////////////////////////////
        
        if(next == seqBin){ // verificamos que esta recibiendo el next correcto
            //printf("buf[1]: %d, buf[0]: %d, ", (uint8_t)buffer[1], (uint8_t)buffer[0]);
            correct = correct + 1;
            printf("IP Client: %s   ", inet_ntoa(client_address.sin_addr)); // ip cliente
            printf("seq: %d   ", seqBin); // # de secuencia
            printf("data_len: %ld\n", strlen(buffer+2)); // tamano payload
            printf("data: %s   ", buffer+2);
            printf("Sequence: CORRECT   ");
            printf("next: %d\n", seqBin+1);
            printf("total received: %d   ", totalMensajes);
            printf("correct: %d   ", correct);
            printf("incorrect: %d   ", incorrect);
            printf("lost: %d\n", seqBin-totalMensajes);
        } else{
            //printf("buf[1]: %d, buf[0]: %d, ", (uint8_t)buffer[1], (uint8_t)buffer[0]);
            incorrect = incorrect + 1;
            printf("IP Client: %s   ", inet_ntoa(client_address.sin_addr)); // ip cliente
            printf("seq: %d   ", seqBin); // # de secuencia
            printf("data_len: %ld\n", strlen(buffer+2)); // tamano payload
            printf("data: %s   ", buffer+2);
            printf("Sequence: INCORRECT   ");
            printf("next: %d\n", seqBin+1);
            printf("total received: %d   ", totalMensajes);
            printf("correct: %d   ", correct);
            printf("incorrect: %d   ", incorrect);
            printf("lost: %d\n", seqBin-totalMensajes);
        }

        next = seqBin + 1; // aqui recien actualizamos el next para poder hacer la comparacion antes

        // send same content back to the client ("echo")
        sendto(sock, buffer, len, 0, (struct sockaddr *)&client_address,
               sizeof(client_address));
    }

    return 0;
}