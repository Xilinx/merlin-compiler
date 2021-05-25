#!/bin/csh

# build environment for cygwin

# bison -d calc1.y    produces calc1.tab.c
# bison -y -d calc1.y produces y.tab.c (the standard yacc output)

# for calc1 and calc2, you may have to
# include the following in the lex ".l" file:
#    extern int yylval;

# calc1
bison -t -v -y -d tldm.y
flex -l tldm.l
#g++ -c y.tab.c lex.yy.c ../xml_parser.cpp -I../ 
g++ -g -O0 y.tab.c lex.yy.c tldm_parser.cpp ../xml_parser.cpp ../file_parser.cpp -DLINUX_COMMANDS -DDEBUG -I../ -o tldm_parser
#gdb tldm_parser.exe < idct_sch.tldm

cp tldm_parser ${ESLOPT_HOME}/bin/
