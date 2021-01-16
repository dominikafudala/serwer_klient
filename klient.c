#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/types.h>   
#include <sys/ipc.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
                
typedef struct struktura_komunikatu{
        long int typ_komunikatu;
        char wiadomosc[16];
        int nadawca;
} Komunikat;
 
int msgid;
int koniec;
pid_t pid;

void *wysylanie(){
        Komunikat kom, *komunikat;
        komunikat=&kom;
        printf("------------q ZAKONCZ---------------\n"); 

        while(fgets(komunikat->wiadomosc,15,stdin)!=NULL){
        komunikat->typ_komunikatu = 1;
        komunikat->nadawca=pid;
    
        printf("------------q ZAKONCZ---------------\n"); 

        if(msgsnd(msgid,komunikat,sizeof(Komunikat) - sizeof(long int),0)==-1){
                perror("Blad(wysylanie)");
                msgctl(msgid, IPC_RMID, NULL);
                exit(-1);
        }

         
        if(strcmp(komunikat->wiadomosc, "q\n")==0){
                printf("KONCZENIE\n");
                break;
        }

}

return NULL;
}

void *odbieranie(){
        Komunikat kom, *komunikat;
        komunikat=&kom;
        while (1){
        if(msgrcv(msgid, komunikat, sizeof(komunikat->wiadomosc), pid, MSG_NOERROR) == -1){ //MSG_NOERROR - obcięcie komunikatu, jeśli jego dane są dłuższe niż msgsz bajtów.
                perror("Blad(odbieranie)");
                msgctl(msgid, IPC_RMID, NULL);
                exit(-1);
        }

        printf("Odebrany tekst: %s\n", komunikat->wiadomosc);

        if(strcmp(komunikat->wiadomosc, "Q\n")==0){
                printf("KONCZENIE\n");
                break;
        }

}

return NULL;
}

         
int main()
{
        pthread_t twysylanie, todbieranie;
        key_t klucz;
        pid=getpid();
 
        printf("tworzenie klucza\n");
        if ((klucz=ftok(".", 'a'))==-1){
                printf("blad tworzenia kolejki kom(klient)\n");
                exit(-1);}

        printf("tworzenie kolejki kom\n");
        if( (msgid=msgget(klucz, 0666)) == -1){
                printf("blad kolejki kom(klient)\n"); exit(-1);}
                //tworzenie dwóch wątków
                
        if(pthread_create(&twysylanie, NULL, wysylanie, NULL)!=0){
        perror("pthread_create wysylanie");
        exit(-1);
        }

        if(pthread_create(&todbieranie, NULL, odbieranie, NULL)!=0){
        perror("pthread_create odbieranie");
        exit(-1);
        }

// czekanie na ich zakończenie

        pthread_join(todbieranie, NULL);
        pthread_join(twysylanie, NULL);
}