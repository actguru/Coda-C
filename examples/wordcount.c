// Report the number of times each word occurs

	#include <coda-c.h>
	int cc_toLow(int c)   { return( c>='A' && c<='Z' ? c+32 : c); }

#define class Int4
CodaClassZerosC();
CodaClass(Int4,int,Root);
class Int4_Value(int value) { class self=newO(Int4); *self=value; return self; }
#undef class // Int4

static Char get_Word(FILE *is) {
	char buf[128]; int pos=0;
	while(1) { int cc=cc_toLow(fgetc(is)); if (cc==EOF) break;
	 	if (cc>='a' && cc<='z') { buf[pos++]=cc; if (pos>=100) break; }
		else if (pos) break;
		}
	buf[pos]=0; return (pos ? Char_Value(buf) : 0);
	}

int main() {
	cleanO Dictionary dict=newO(Dictionary);
	while(1) {
		cleanO Char word=get_Word(stdin); if (!word) break;
		Int4 count=Dict_sub(dict,word);
		if (count) ++(*count);
		  else Dict_take(dict,word,Int4_Value(1));
		}
	for(Keyword key=Dictionary_scan(dict);key;key=Dictionary_next(key)) {
		Int4 count=key->item; printf("%s=%d  ",key->word,*count);
		}
	printf("\n");
	}

/*<stdin>
All work and no play ...
All work and no play ...
All work and no play ...
Redrum.
*/

/*<stdout>
redrum=1  play=3  no=3  and=3  work=3  all=3  
*/

