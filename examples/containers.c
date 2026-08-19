	#include <coda-c.h> // demonstrate "ToContainer()" and Object_Counter

#define class Crazy
CodaClassZerosC();
CodaClass(Crazy,short,Root);
#undef class // Crazy

int main() {
	printf("Object Count: %lld\n",Object_Counter);
	{ // scope block
		cleanO Array list=newO(Array);
		Array_take(list,Char_Value("red"));
		Array_add(list,Os("blue"));
		Array_add(list,Os("green"));
		Array_add(list,0);
		Array_take(list,newOC(Crazy,7));
		Array_take(list,alocO(5));

		Dict d=newO(Dict);
		Dict_set(d,"Key1",Os("3.14"));
		Dict_set(d,"Key2",Os("7.89"));
		Array_take(list,d);

		cleanO Char msg=ToContainer(list);
		printf("Array list:  %s\n",msg);
		cleanO Char info=obj_(Info,list);
		printf("list info.:  %s\n",info);
		}
	printf("Object Count: %lld\n",Object_Counter);
	Global_objectsZero(); // tear down coda-c system for Leak testing
	printf("Final  Count: %lld\n",Object_Counter);

	return 0;
	}

