#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(){

    FILE* She = popen("fortune", "r");
    char thatsWhatSheSaid[4096];
    size_t size = fread(thatsWhatSheSaid, sizeof(char), 4096, She); //reading into buffer
    thatsWhatSheSaid[size] = '\0';


    printf("Fortune:\n %s", thatsWhatSheSaid);
    pclose(She);


    FILE* muuuuu = popen("cowsay", "w");
    fwrite(thatsWhatSheSaid, sizeof(char), strlen(thatsWhatSheSaid), muuuuu); //writing cowsay message
    pclose(muuuuu);


}










// int main(int argc, char** argv){
//     if (argc < 2 || argc > 4){
//         printf("Wrong number of arguments");
//         exit(-1);
//     }


//     if (argc == 2){
//         char * givenArgument = argv[1];
//         char command[4096] = "sort -k"; //to sort by specified column
        
//         //sorted by emails addresses -> by 2 column
//         if (strcmp(givenArgument, "nadawca")){
//             strcat(command, "2");
//         }
//         //sorted by date -> by 3 column
//         else if (strcmp(givenArgument, "data")){
//             strcat(command, "3");
//         }
//         else{
//             printf("Wrong argument");
//             exit(-1);
//         }

//         FILE* sortCommand = popen(command, "w"); //the one to write
//         FILE* mails = popen("mail", "r"); //opening mails

//         char buffer[4096];
//         while(fgets(buffer, strlen(buffer), mails) != NULL){
//             fputs(buffer, sortCommand);
//         }
//         pclose(sortCommand);
//         pclose(mails);
//         return 0;

//     }

//     else if (argc == 4){
//         char* mailAddress = argv[1];
//         char* title = argv[2];
//         char* text = argv[3];


//         //mail - s "title" address 
//         char command[4096] = "mail -s '";
//         strcat(command, title); //adding title
//         strcat(command, "' ");
//         strcat(command, mailAddress); //adding mailAddress


//         FILE* mail = popen(command, "w");
//         fputc('"', mail);
//         fputs(text, mail);
//         fputc('"', mail);
        

//         pclose(mail);
//         return 0;
//     }
// }
