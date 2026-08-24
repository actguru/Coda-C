// Sort input lines

	#include <coda-c.h>

int compare(const void *aa,const void *bb) { return strcmp(*(char **)aa, *(char **)bb); }

int main() {
	cleanO Array list=newO(Array);
	while(1) {
		char aa[1024],*bb=fgets(aa,sizeof(aa),stdin); if (!bb) break;
		Array_take(list,Char_Value(bb));
		}
	int nel=Array_count(list);
	qsort(Array_rawAddress(list),nel,sizeof(void *),compare);
	for(int j=0;j<nel;++j) {
		printf("%s",(char *)Array_sub(list,j));
		}
	}

/*<stdin> test input
a
eeeee
ccc
bb
dddd
*/

/*<stdout>
a
bb
ccc
dddd
eeeee
*/
