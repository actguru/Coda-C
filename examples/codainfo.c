	#include <coda-c.h>

int main() {
	printf("\n");
	printf("========Coda-C Registered Classes========\n\n");
	Dict cd=Class_dictionary();
	cleanO Pointer list=Dictionary_AllKeys(cd);
	int nel=Pointer_count(list);
	for(int j=0;j<nel;++j) {
		char* key=list[j];
		Obj co=Dict_sub(cd,key);
		CodaCLASS *clas=classO(co);
		if (clas->bits==bits_Root) {
			printf("Class: %-20.20s %s\n",key,"*** root class ***");
			continue;
			}

		void *fp=OResponds(co,Info);
    	Char Root_Info(Root self); // object default info, allow direct calls // PRIVATE
		if (!fp || fp==Root_Info) {
	    	printf("Class: %-20.20s %s\n",key,"*** Custom Object Class ***");
		    continue;
			}
		cleanO Obj obj=Memory_newO(co,1);
		char *xml=0; if (OResponds(obj,xmlTag)) xml=obj_(xmlTag,obj);
		char a[64]; *a=0; if (xml) snprintf(a,sizeof(a),", xmlTag=\"%s\"",xml);
		cleanO Char msg=obj_(Info,obj);
		printf("Class: %-20.20s %s%s\n",key,msg,a);
		}

	printf("\n");
	printf("========Coda-C Registered Signatures========\n\n");
	Dict sd=Signature_dictionary();
	Pointer sigs=Dictionary_AllKeys(sd);
	for(int j=0;j<Pointer_count(sigs);++j) {
		char* key=sigs[j];
		printf("%s(), ",key);
		}
	printf("\n");

	printf("\n");
	printf("========Coda-C Registered Versions========\n\n");
	Dict vd=Version_dictionary();
	Pointer vers=Dictionary_AllKeys(vd);
	for(int j=0;j<Pointer_count(vers);++j) {
		char* key=vers[j];
		char* txt=Dict_sub(vd,key);
		printf("%s: %s",key,txt);
		}
	printf("\n");
	}

