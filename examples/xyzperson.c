	#include <coda-c.h>
	int cc_toCap(int c)    { return( c>='a' && c<='z' ? c-32 : c); }
	void cs_cap(char *str) { while(*str) { *str =cc_toCap(*str); ++str; } }

	typedef void OSig$(sayHello) ;
	typedef void OSig$(saySomething,char* greeting) ;

#define class XyzPerson
CodaClassZerosC();
CodaClass(XyzPerson,char,Root);

method$(void,sayHello) { obj_(saySomething,self,"Hello, world!"); }

method$(void,saySomething,char* greeting) { printf("%s\n", greeting); }
#undef class // XyzPerson

#define class XyzShoutingPerson
CodaClassZerosC();
CodaClass(XyzShoutingPerson,short,XyzPerson);

method$(void,saySomething,char* greeting) {
	cleanO Char upper=Char_Value(greeting); cs_cap(upper);
	super_(saySomething,self,upper);
	}
#undef class // XyzShoutingPerson

int main() {
	cleanO XyzPerson ppp=newO(XyzPerson);
	obj_(sayHello,ppp);

	cleanO XyzShoutingPerson sss=newO(XyzShoutingPerson);
	obj_(sayHello,sss);
	}

// OUTPUT:
// Hello, world!
// HELLO, WORLD!
