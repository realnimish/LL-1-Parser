%{
#include<stdio.h>
%}
%%

[\t \n]+      {
                 /*ignore */
            }
[0-9]+      {
                FILE *fptr = fopen("input.txt", "a");
                fprintf(fptr,"%s %s\n","num",yytext);
                 fclose(fptr);
            }
[(+\-*/)]    {
                FILE *fptr = fopen("input.txt", "a");
                fprintf(fptr,"%s %s\n",yytext,yytext);
                 fclose(fptr);
            }
[A-Za-z$]+[A-Za-z0-9_]+   {
                FILE *fptr = fopen("input.txt", "a");
                fprintf(fptr,"%s %s\n","id",yytext);
                 fclose(fptr);
            }
.   {

    }


%%
void main(){
    yylex();
}