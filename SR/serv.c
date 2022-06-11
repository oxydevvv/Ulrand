#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

int deck[100]={0};
int manche=1;
int joueurs;
int check=0;
int anull=0;
bool loose=false;
int Nbrecu;
int pile[100];
bool conti=true;

void *jeu(void *arg)
{
    int socket = *(int*)arg;
    char msg[] = "Quel type de joueur ? 0 = bot / 1 = humain";
    char msg2[] = "Quel est le nom du joueur ?";
    char msg3[] = "C'est perdu, voulez vous continuez ? 0 = non / 1 = oui";
    char msg4[] = "C'est gagné ! Nouvelle manche !";
    int type, choix, recu, suite;
    int mainJ[50]={0};
    bool valid;
    char nom[30];
    char bot[]="bot";
    bool fmanche=false;
    send(socket, msg, strlen(msg)+1, 0);
    recv(socket, &type, sizeof(type), 0);
    if(type==0)
    {
        printf("Joueur robot\n");
        strcpy(nom, bot);
    }
    else
    {
        printf("Joueur humain\n");
        send(socket, msg2, strlen(msg2)+1, 0);
        recv(socket, &nom, sizeof(nom), 0);
    }
    check++;
    while (check!=joueurs && anull!=40)
    {
        printf("attente joueurs");
        sleep(5);
        anull++;
    }
    if(anull==40)
        printf("attente trop longue");
    else
    {
        while(conti==true)
        {
            Nbrecu=0;
            for (int i = 0; i < 100 ; i++)
            {
                deck[i] = i+1;
            }
            for(int i=0; i<manche; i++)
            {
                valid=false;
                while (valid==false)
                {
                    choix = (rand() % 100) + 1;
                    if (deck[choix]!=0)
                    {
                       mainJ[i]=deck[choix];
                       deck[choix]=0;
                       valid=true;
                    }
                }
            }
            send(socket, mainJ, sizeof(mainJ)+1, 0);
            while(manche*joueurs!=Nbrecu && loose==false)
            {
                recv(socket, &recu, sizeof(recu), 0);
                if(recu!=0)
                {
                    printf("%s a placer la carte numéro %d\n", nom, recu);
                    if(Nbrecu==0)
                        pile[0]=recu;
                    else
                    {
                        if(pile[Nbrecu-1]>recu)
                        {
                            loose=true;
                            fmanche=true;
                        }
                        else
                            pile[Nbrecu]=recu;
                    }
                    Nbrecu++;
                    if(manche*joueurs==Nbrecu)
                        fmanche=true;
                }
                send(socket, &fmanche, sizeof(fmanche), 0);
            }
            send(socket, &loose, sizeof(loose), 0);
            if(loose==true)
            {
                send(socket, msg3, strlen(msg3)+1, 0);
                recv(socket, &suite, sizeof(suite), 0);
                manche=1;
                if(suite==0)
                {
                    conti=false;
                    printf("%s a voulu stopper de jouer", nom);
                }
            }
            else
            {
                send(socket, msg4, strlen(msg4)+1, 0);
                manche++;
            }
            if(manche*joueurs>100)
                conti=false;
            send(socket, &conti, sizeof(conti), 0);
        }
    }
    close(socket);
    free(arg);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int sockServer = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addrServer;
    addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");//à changer en fonction de l'utilisation, ici on reste sur le pc
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(30000);

    bind(sockServer, (const struct sockaddr *)&addrServer, sizeof(addrServer));
    printf("liaison : %d\n", sockServer);

    listen(sockServer, 5); //connexion max
    printf("écoute\n");

    printf("Combien de joueurs ?");
    scanf("%d", &joueurs);
    pthread_t threads[joueurs];
    for(int i = 0; i<joueurs; i++)
    {
        struct sockaddr_in addrClient;
        socklen_t csize = sizeof(addrClient);
        int sockClient = accept(sockServer, (struct sockaddr *)&addrClient, &csize);
        printf("accept\n");

        printf("client: %d\n", sockClient);

        int *arg = malloc(sizeof(int));
        *arg = sockClient;
        pthread_create(&threads[i], NULL, jeu, arg);
    }

    for(int i = 0; i < joueurs; i++)
    {
        pthread_join(threads[i], NULL);
    }

    close(sockServer);
    printf("close\n");
    return 0;
}
