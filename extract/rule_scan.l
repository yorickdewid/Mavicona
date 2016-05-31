%option nounput noyywrap

%{
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include "rule_parse.h"

void yyerror(const char *); // Forward declare these functions
int yyparse(void);
%}

%x include_ruleset

%%
include[ \t]*["'<]  BEGIN include_ruleset;
[ \t\n]+       ; // Do nothing on whitespace
['"][a-zA-Z0-9\/\-\.\+]*['"]   { yylval.str_val = new std::string(yytext); return STRING; }
"#".*          ; // Ignore whitespace
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

<include_ruleset>[ \t]*  // eat the whitespace
<include_ruleset>[^ \t\n]+["'>] { // got the include file name
   yytext[strlen(yytext)-1] = '\0';
   yyin = fopen(yytext, "r");
   if (!yyin) {
      fprintf(stderr, "Cannot read include %s", yytext);
      return 0;
   }

   yypush_buffer_state(yy_create_buffer( yyin, YY_BUF_SIZE ));
   BEGIN INITIAL;
}

<<EOF>> {
   yypop_buffer_state();

   if (!YY_CURRENT_BUFFER) {
      yyterminate();
   }
}

%%

void yyerror(const char *s) {
   std::cout << " <error> " << s << std::endl;
   exit(1);
}

#ifdef RULER
int main(int argc, char *argv[]) {
   extern FILE * yyin;
   extern void print_tree();

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

   /* parse through the input until there is no more: */
   do {
      yyparse();
   } while(!feof(yyin));

   print_tree();

   fclose(file);
}
#endif
