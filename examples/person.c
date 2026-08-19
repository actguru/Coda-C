	#include <coda-c.h>

	struct Person_ { Char name; int age; };
	CodaClassDef(Person,struct Person_,Root); // for 'dtor' function's 'self'

#define class Person
	
CodaClassZeros(itor,kize,etor,ekeep,bits); // dtor removed from list
static void $(dtor) { freeO(_ name); printf("_ name was destroyed!\n"); }
CodaClass(Person,struct Person_,Root);

	property$(Char,name,_ name, setO_take(_ name,Char_Value(value)) );
	property$_(int,age);

class Person_WithNameAge(char *name,int age)  {
	Person self = newO(Person);
	Person_set_name(self,"Alice");
	Person_set_age( self,30);
	return(self);
	}

void $(sayHello) {
	printf("Hello, my name is %s and I am %d years old.\n", _ name, _ age);
	}
#undef class // Person

int main() {
	cleanO Person person = Person_WithNameAge("Alice",30);
	Person_sayHello(person);
	Person_set_age(person,31);
	Person_sayHello(person);
	}

// OUTPUT:
// Hello, my name is Alice and I am 30 years old.
// Hello, my name is Alice and I am 31 years old.
// _ name was destroyed!
