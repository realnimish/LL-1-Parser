%{
#include<stdio.h>
%}
%%

[A-Za-z][A-Za-z0-9_]*   {
                FILE *fptr = fopen("input.txt", "a");
                // printf("word: %s\n",yytext);
                fprintf(fptr,"%s %s\n","id",yytext);
                 fclose(fptr);
            }

[\t \n]+      {
                 /*ignore */
            }
[0-9]+      {
                FILE *fptr = fopen("input.txt", "a");
                // printf("num: %s\n",yytext);
                fprintf(fptr,"%s %s\n","num",yytext);
                fclose(fptr);
            }
[)(+*/-]    {
                FILE *fptr = fopen("input.txt", "a");
                // printf("edge: %s\n",yytext);
                fprintf(fptr,"%s %s\n",yytext,yytext);
                 fclose(fptr);
            }
.   {
            // printf("ISSUE: %s\n",yytext);
    }


%%

int yywrap() {}

void main(){
    FILE *fptr = fopen("input.txt", "w");
    if(fptr==NULL)
        fptr = fopen("scores.dat", "wb");
    yylex();
}