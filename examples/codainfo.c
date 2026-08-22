	#include <coda-c.h>

int main() {
	printf("\n=== Coda-C Registered Classes\n");
	Dict cd=Class_dictionary();
	cleanO Pointer list=Dictionary_AllKeys(cd);
	for(int j=0;j<Pointer_count(list);++j) {
		char* key=list[j];
		Obj co=Dict_sub(cd,key); // class object
		Char msg=0;
		if (classO(co)->bits==bits_Root) msg=Os("*** root class ***");
		void *fp=OResponds(co,Info);
		if (!msg) {
    		Char Root_Info(Root self); // object default info, allow direct calls // prototype
			if (!fp || fp==Root_Info) msg=Os("*** custom class ***");
			}
		if (!msg) {
			cleanO Obj obj= Memory_newO(co,1);
			msg=obj_(Info,obj);
			}
		printf("Class: %-15.15s %s\n",key,msg);
		freeO(msg);
		}

	printf("\n=== Coda-C Registered Signatures\n");
	Dict sd=Signature_dictionary();
	cleanO Pointer sigs=Dictionary_AllKeys(sd);
	for(int j=0;j<Pointer_count(sigs);++j) {
		printf("%s(), ",sigs[j]);
		if (j%7==6) printf("\n");
		}
	printf("\n");

	printf("\n=== Coda-C Registered Versions\n");
	Dict vd=Version_dictionary();
	cleanO Pointer vers=Dictionary_AllKeys(vd);
	for(int j=0;j<Pointer_count(vers);++j) {
		char* key=vers[j];
		char* txt=Dict_sub(vd,key);
		printf("%s: %s\n",key,txt);
		}
	printf("\n");
	}


// OUTPUT

// === Coda-C Registered Classes
// Class: CArray          CArray[0] <Dynamic pointer array>
// Class: Array           Array[0] <Dynamic object array>
// Class: Char            Char[1]="" aka char*
// Class: ConstChar       ConstChar[1]="" aka char*
// Class: Dictionary      Dictionary[0] <Keyword array>
// Class: CDictionary     CDictionary[0] <C Keyword array of raw pointers>
// Class: Keyword         Keyword[8+1] { : (0)}
// Class: Pointer         Pointer[1] aka void**
// Class: Root            *** root class ***
// Class: Void            *** root class ***
// 
// === Coda-C Registered Signatures
// Info(), ToString(), xmlTag(), ToDelimiter(), ToStringSub(), get_count(), subInt(), 
// NewBlock(), addObject(), removeLast(), removeObject(), removeAll(), set_name(), get_name(), 
// FromString(), AllKeys(), setKey(), subKey(), removeKey(), 
// 
// === Coda-C Registered Versions
// Coda-C: 3.0, www.coda-c.com, Copyright (c) 2026  Stephen M. Jones, Affero GPL 3.

