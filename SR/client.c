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

int main(int argc, char *argv[])
{
    int sockClient = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addrClient;
    int * type;
    char nom[30];
    int mainJ[50]={0};
    int * choix;
    bool fmanche=false;
    bool res;
    int * conti;
    bool partie=true;
    int min=0;
    bool vide=false;
    int somme=0;
    char msg[50];
    char msg2[50];
    char msg3[50];
    char msg4[50];
    addrClient.sin_addr.s_addr = inet_addr("127.0.0.1");//� changer en fonction de l'utilisation, ici on reste sur le pc
    addrClient.sin_family = AF_INET;
    addrClient.sin_port = htons(30000);
    connect(sockClient, (const struct sockaddr *)&addrClient, sizeof(addrClient));
    printf("connecte\n");
    recv(sockClient, msg, 50, 0);
    printf("%s\n", msg);
    scanf("%d", type);
    while(*type!='0' || *type!='1')
    {
        printf("Il faut suivre les consignes\n");
        scanf("%d", type);
    }
    send(sockClient, &type, sizeof(type), 0);
    if(*type=='1')
    {
        recv(sockClient, msg2, 50, 0);
        printf("%s\n", msg2);
        scanf("%s", nom);
        send(sockClient, &nom, sizeof(nom), 0);
    }

    while(partie==true)
    {
        recv(sockClient, &mainJ, 50, 0);

        printf ("Votre main est : ");
        for(int i=0; i<50; i++)
        {
            if (mainJ[i]!=0)
                printf("%d", mainJ[i]);
            else
                i+=50;
        }
        if(*type=='1')
        {
            while(fmanche==false)
            {
                printf("\n Quelle carte voulez vous jouer ? (la carte la plus � gauche est la num�ro 0)\n");
                scanf("%d", choix);
                send(sockClient, &mainJ[*choix], sizeof(mainJ[*choix]), 0);
                mainJ[*choix]=0;
                recv(sockClient, &fmanche, sizeof(fmanche), 0);
            }
            recv(sockClient, &res, sizeof(res), 0);
            if(res==true)
            {
                recv(sockClient, msg3, 50, 0);
                printf("%s\n", msg3);
                scanf("%d", conti);
                while(*conti != '0' || *conti != '1')
                {
                    printf("Il faut suivre les consignes\n");
                    scanf("%d", conti);
                }
                send(sockClient, &conti, sizeof(conti), 0);
            }
            else
            {
                recv(sockClient, msg4, 50, 0);
                printf("%s\n", msg4);
            }
        }
        else
        {
            while(fmanche==false)
            {
                min=101;
                somme=0;
                for(int i=0; i<50; i++)
                    somme+=mainJ[i];
                if(somme==0)
                    vide=true;
                if(vide==true)
                {
                    choix=0;
                    sleep(3);
                }
                else
                {
                    for(int i=0; i<50; i++)
                    {
                        if (mainJ[i]!=0 && mainJ[i]<min)
                        {
                            min=mainJ[i];
                            *choix=i;
                        }
                    }
                    sleep((int)((mainJ[*choix]/10)+(rand()%5)));
                }
                send(sockClient, &mainJ[*choix], sizeof(mainJ[*choix]), 0);
                mainJ[*choix]=0;
                recv(sockClient, &fmanche, sizeof(fmanche), 0);
            }
            recv(sockClient, &res, sizeof(res), 0);
            if(res==true)
            {
                recv(sockClient, msg3, 50, 0);
                *conti=1;
                send(sockClient, &conti, sizeof(conti), 0);
            }
            else
                recv(sockClient, msg4, 50, 0);
        }
        recv(sockClient, &partie, sizeof(partie), 0);
    }
    close(sockClient);

    return 0;
}
