// Coda-C JSON serialization example

	#include <coda-c.h> // jsonout.c

Dictionary sample_employee() { // lets structure data in a Dictionary,
	Dict empl=newO(Dict);      // a generic structure.
	Dict_set(empl,"name", Os("Jane Doe"));
	Dict_set(empl,"email", Os("jane.doe@example.com"));

	Array roles=newO(Array);
		Array_add(roles,Os("Admin"));
		Array_add(roles,Os("User"));
	Dict_take(empl,"roles",roles);

	Dict address=newO(Dict);
		Dict_set(address,"city", Os("Atlanta"));
		Dict_set(address,"zipcode",Os("30301"));
	Dict_take(empl,"address",address);
	return(empl);
	}

	typedef void OSig$(JSON_out,FILE* os,int indent); // virtual signature

int main() {
	Dict empl=sample_employee();
	obj_(JSON_out,empl,stdout,0); // lets just output the data for now as JSON
	printf("\n");
	}

// implement methods for the three classes we used
#define class Dictionary
	method$(void,JSON_out,FILE* os,int indent) {
		cleanO Pointer keys=Dictionary_AllKeys(self);
		indent+=4; printf("{\n%*s",indent,"");
		int nel=Pointer_count(keys);
		for(int j=0;j<nel;++j) {
			char* key=keys[j];
			printf("\"%s\" : ",key);
			obj_(JSON_out,Dict_sub(self,key),stdout,indent);
			if (j<nel-1) printf(",");
			printf("\n%*s",indent,"");
			}
		printf("}"); // indent-=4;
		}
#undef class // Dictionary

#define class Array
	method$(void,JSON_out,FILE* os,int indent) {
		int nel=Array_count(self);
		printf("[ ");
		for(int j=0;j<nel;++j) {
			if (j) printf(", ");
			obj_(JSON_out,Array_sub(self,j),stdout,indent);
			}
		printf(" ]");
		}
#undef class // Array

#define class Char
	method$(void,JSON_out,FILE* os,int indent) {
		printf("\"%s\"",self); // simple strings only
		// does not handle: " \\ or (cc<32) or (cc>127) etc.
		}
#undef class // Char

/* OUTPUT
{
    "name" : "Jane Doe",
    "email" : "jane.doe@example.com",
    "roles" : [ "Admin", "User" ],
    "address" : {
        "city" : "Atlanta",
        "zipcode" : "30301"
        }
    }
*/
