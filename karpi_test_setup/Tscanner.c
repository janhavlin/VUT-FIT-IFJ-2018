#include "Tscanner.h"
/**
	Represents input code.
	
	Current representation:
	[	
		def factorial (n)
			if n < 2 then
				result = 1
			else
				decremented_n = n - 1
				temp_result = factorial decremented_n
				result = n * temp_result
			end # if
			result
		end 
		print "Zadejte␣cislo␣pro␣vypocet␣faktorialu:␣"
		a = inputi
		if a < 0 then
			print "\nFaktorial␣nelze␣spocitat\n"
		else
			vysl = factorial a
			print("\nVysledek␣je:", vysl, "\n")
		end
	]			
*/

#define TOK_AMOUNT 68

TToken testArray[TOK_AMOUNT] = {
	{TOK_KEY, .data.s = "def"},
	{TOK_ID, .data.s = "id"},
	{TOK_LEFT_BRACKET, .data.s = "("},
	{TOK_ID, .data.s = "id"},
	{TOK_RIGHT_BRACKET, .data.s = ")"},
	{TOK_EOL, .data.s = "eol"},//def factorial (n)
	{TOK_KEY, .data.s = "if"},
	{TOK_EXPR, .data.s = "expr"},
	{TOK_KEY, .data.s = "then"},
	{TOK_EOL, .data.s = "eol"},//if n < 2 then
	{TOK_ID, .data.s = "id"},
	{TOK_ASSIGN, .data.s = "="},
	{TOK_EXPR, .data.s = "expr"},
	{TOK_EOL, .data.s = "eol"},//result = 1
	{TOK_KEY, .data.s = "else"},
	{TOK_EOL, .data.s = "eol"},//else
	{TOK_ID, .data.s = "id"},
	{TOK_ASSIGN, .data.s = "="},
	{TOK_EXPR, .data.s = "expr"},
	{TOK_EOL, .data.s = "eol"},//decremented_n = n - 1
	{TOK_ID, .data.s = "id"},
	{TOK_ASSIGN, .data.s = "="},
	{TOK_ID, .data.s = "id"},
	{TOK_ID, .data.s = "id"},
	{TOK_EOL, .data.s = "eol"},//temp_result = factorial decremented_n
	{TOK_ID, .data.s = "id"},
	{TOK_ASSIGN, .data.s = "="},
	{TOK_EXPR, .data.s = "expr"},
	{TOK_EOL, .data.s = "eol"},//result = n * temp_result
	{TOK_KEY, .data.s = "end"},
	{TOK_EOL, .data.s = "eol"},//end
	{TOK_ID, .data.s = "id"},
	{TOK_EOL, .data.s = "eol"},//result
	{TOK_KEY, .data.s = "end"},
	{TOK_EOL, .data.s = "eol"},//end
	{TOK_ID, .data.s = "id"},
	{TOK_STRING, .data.s = "string"},
	{TOK_EOL, .data.s = "eol"},//print "Zadejte␣"
	{TOK_ID, .data.s = "id"},
	{TOK_ASSIGN, .data.s = "="},
	{TOK_EXPR, .data.s = "id"},
	{TOK_EOL, .data.s = "eol"},//a = inputi
	{TOK_KEY, .data.s = "if"},
	{TOK_EXPR, .data.s = "expr"},
	{TOK_KEY, .data.s = "then"},
	{TOK_EOL, .data.s = "eol"},//if a < 0 then
	{TOK_ID, .data.s = "id"},
	{TOK_STRING, .data.s = "string"},
	{TOK_EOL, .data.s = "eol"},//print "Zadejte␣"
	{TOK_KEY, .data.s = "else"},
	{TOK_EOL, .data.s = "eol"},//else
	{TOK_ID, .data.s = "id"},
	{TOK_ASSIGN, .data.s = "="},
	{TOK_ID, .data.s = "id"},
	{TOK_ID, .data.s = "id"},
	{TOK_EOL, .data.s = "eol"},//vysl = factorial a
	{TOK_ID, .data.s = "id"},
	{TOK_LEFT_BRACKET, .data.s = "("},
	{TOK_STRING, .data.s = "string"},
	{TOK_COMMA, .data.s = ","},
	{TOK_ID, .data.s = "id"},
	{TOK_COMMA, .data.s = ","},
	{TOK_STRING, .data.s = "string"},
	{TOK_RIGHT_BRACKET, .data.s = ")"},
	{TOK_EOL, .data.s = "eol"},//print("\nVysledek␣je:", vysl, "\n")
	{TOK_KEY, .data.s = "end"},
	{TOK_EOL, .data.s = "eol"},//end
	{TOK_EOF, .data.s = "eof"},//eof
};


TToken * getToken(FILE *f) {
	static int index = 0;
	if (index < TOK_AMOUNT)
		return &(testArray[index++]);
	else
		return NULL;
}
    
