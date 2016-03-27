%option nounput

%{
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include "rule_parse.h"

void yyerror(const char *); // we need to forward declare these functions ,
int yyparse(void); // don ’t worry about them
%}

%%

[ \t\n]+       ; // do nothing on whitespace
'\.?[a-zA-Z0-9\/\-\.\+]*'   { yylval.str_val = new std::string(yytext); return STRING; }
"#".*          ;
";"            return SEMICOLON;
"{"            return LBRACE;
"}"            return RBRACE;
"mime"         return MIME;
"type"         return TYPE;
"extension"    return EXTENSION;
"category"     return CATEGORY;
"unknown"      return UNKNOWN;
"then"         return THEN;
"store"        return STORE;
"discard"      return DISCARD;
"queue"        return QUEUE;
"log"          return LOG;
.              { printf("Unknown token"); yyterminate(); };

%%

void yyerror(const char *s) {
   std::cout << " <error> " << s << std::endl;
   exit(1);
}

int yywrap(void) { return 1; }

#ifdef RULER
int main(int argc, char *argv[]) {
   extern FILE * yyin;

   if(argc != 2) {
      printf("usage : ./ruler filename \n");
      exit(1);
   }

   FILE *file = fopen(argv[1], "r");
   if(file == NULL){
      printf("couldn ’t open %s\n", argv[1]);
      exit(1);
   }

   yyin = file; // now flex reads from file

   // parse through the input until there is no more:
   do {
      yyparse();
   } while (!feof(yyin));


   fclose(file);
}
#endif
