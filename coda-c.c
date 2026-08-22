/* www.coda-c.com  coda-c.c

Copyright (C) 2026 Stephen M. Jones

This file is part of Coda-C.

Coda-C is free software: you can redistribute it and/or modify it
under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

Coda-C is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with Coda-C. If not, see <https://www.gnu.org/licenses/>.

*/
	#define QWebsite "www.coda-c.com"
	#define QCopyYears	"2026"
	#define QVersion	"3.0"

	#include "./coda-c.h"
	#define _GNU_SOURCE 1

	#if     WIN32
		#define malloc_size(xxx)	_msize(xxx)
		#define timegm _mkgmtime
	#elif __linux__
		#include <malloc.h>
		#define malloc_size(xxx)	malloc_usable_size(xxx)
	#else
		#include <malloc/malloc.h>
	#endif

	#include <stdatomic.h>
	#include <stdarg.h>
	#include <stddef.h>
	#include <math.h>
	#include <time.h>
	#include <sys/stat.h>
	#include <limits.h>
	#include <errno.h>

	static_assert(EOF==(-1),"EOF has a bad value.");

Obj CArray_insertAt(CArray self,int index,Obj obj);
void Array_dtor(Array self,CodaCLASS *clas);
void Array_itor(Array self);
Char Char_FromString(Char self,char *str);
extern CDictionary Class_dictionary_dict;
void CMux_dtor(pointer cmux);
void *CMux_Data4Key(pointer cmux,char *key);
void *CMux_KeyData(pointer cmux,char *key,pointer data);
void Dictionary_dtor(Dictionary self,CodaCLASS *clas);
extern CArray Global_objectPointers;
Obj Memory_newOO(CodaCLASS *clas,int count);
Char Root_Info(Root self);
extern CDictionary Signature_dictionary_dict;
extern CDictionary Version_dictionary_dict;

void clean_C(const void *vp) { freeC(*(void **)vp); }

pointer alocC(int4 size) {
	pointer address=calloc(1,size);
	if (!address) Die_Object(Os("alocC; out of memory!"),1);
	return(address);
	}

void freeC(pointer address) {
	free(address);
	}

double Array_FACTOR=1.60;

#define class CArray

	struct Array_ { Char name; Pointer alla; int count,nel,zerox; } ;

	CodaClassZeros(etor,bits,kize,ekeep);
	#define CArray_dtor Array_dtor
	#define CArray_itor Array_itor
CodaClass(CArray,struct Array_,Root);

method$(Char,Info) {
	char *name=Array_name(self);
	if (!name) name="Dynamic pointer array";
	return Char_F("%s[%d] <%s>",kindO(self),Array_count(self),name);
	}

static int count2nel(int count) {
	if (count<32) count=32;
	int nel=(int)llround(count*Array_FACTOR); assert(nel>count+3);
	return(nel);
	}
	#define eleAddr(ptr,index) (ptr->alla + ptr->zerox + index)

static void C_Array_Release(Array self,DTOR etor,int start,int nel) {
	if (!etor) return;
	Pointer vec=(void *)eleAddr(self,start);
	for(int j=nel-1;j>=0;--j) etor(vec[j],self);
	}

static void CArray_copyBlock(CArray self,int dix,int count,Pointer block) {
	Pointer pdest=eleAddr(self,dix);
	for(int j=0;j<count;++j) *pdest++ = *block++;
	}

static void CArray_copyBlockFromEnd(CArray self,int dix,int count,Pointer block) {
	Pointer pdest=eleAddr(self,dix+count); block+=count;
	for(int j=0;j<count;++j) *(--pdest) = *(--block);
	}

	static void CArray_initCap(CArray self,int capacity) ;
static CArray CArray_NewCap(int capacity) {
	CArray self=newO(CArray); freeC(_ alla);
		CArray_initCap(self,capacity);
	return(self);
	}

	static void CArray_Upgrade(CArray dest,int dix,int count,Pointer block,char *msg) ;
static void CArray_Smaller(CArray dest) {
	if (dest->nel<=32) return;
	int tenth=dest->nel/10;
	if (dest->count>tenth) return;
	CArray_Upgrade(dest,0,0,NULL,"Downsize");
	}

void Array_removeBlock(Array self,int dix,int count) {
	if (count<1) return;
	assert(dix>=0 && dix+count<=_ count);
	C_Array_Release(self,Memory_etor(self),dix,count);
	if (dix==0)				_ zerox+=count;
	ei (dix+count==_ count)	;
	else CArray_copyBlock(self,dix,_ count-dix-count,eleAddr(self,dix+count));
	_ count-=count;
	CArray_Smaller(self);
	}

static void CArray_Upgrade(CArray dest,int dix,int count,Pointer block,char *msg) {
	int capa=(int)llround((dest->count+count)*Array_FACTOR);
	cleanO CArray grade=CArray_NewCap(capa); assert(grade->nel>dest->count+count);
		grade->count=dest->count+count;
		grade->zerox=(grade->nel - grade->count)/3; assert(grade->zerox>=1);
	CArray_copyBlock(grade,        0,dix,eleAddr(dest,0));
	CArray_copyBlock(grade,      dix,count,block);
	CArray_copyBlock(grade,dix+count,dest->count-dix,eleAddr(dest,dix));
	Char destName=dest->name;
	struct Array_ temp=(*dest); (*dest)=(*grade); (*grade)=temp;
	dest->name=destName; grade->name=0;
	}

void Array_takeBlock(Array self,int index,int count,pointer block) {
	if (count<1) return;
	assert(index>=0 && index<=_ count);
	Char msg=0; int tailcount=_ count - index;
	if (index==0) { msg="Expand Front";
		if (count<= _ zerox) {
			_ zerox-=count; _ count+=count;
			return CArray_copyBlock(self,index,count,block);
			}
		}
	ei (index==_ count) { msg="Expand End";
		if (_ nel - _ zerox - _ count >= count) {
			_ count+=count;
			return CArray_copyBlock(self,index,count,block);
			}
		}
	ei (index<tailcount) { msg="Insert Front";
		if (count<=_ zerox)	{
			_ zerox-=count; _ count+=count;
			       CArray_copyBlock(self,0,index,eleAddr(self,count));
			return CArray_copyBlock(self,index,count,block);
			}
		}
	else { msg="Insert End";
		if (_ nel - _ zerox - _ count >= count) {
			_ count+=count;
			CArray_copyBlockFromEnd(self,index+count,tailcount,eleAddr(self,index));
			return CArray_copyBlock(self,index,count,block);
			}
	  	}
	CArray_Upgrade(self,index,count,block,msg);
	}

Obj CArray_insertAt(CArray self,int index,Obj obj) {
	Array_takeBlock(self,index,1,&obj);
	return(obj);
	}

method$(Char,ToString) { return Char_F("%s[%d]",kindO(self),_ count); }

#undef class

#define class Array

void $(dtor,CodaCLASS *clas) {
	freeO(_ name); _ name=0;
	C_Array_Release(self,clas->etor,0,_ count);
	freeC(_ alla);
	}

static void CArray_initCap(CArray self,int capacity) {
	_ count=0;
	_ nel=count2nel(_ count);
	if (capacity && _ nel<capacity) _ nel=capacity;
	_ zerox=(_ nel - _ count)/3; assert(_ zerox>=1);
	_ alla=alocC(_ nel*sizeof(pointer));
	}

void $(itor) { CArray_initCap(self,0); }

int $(get_count) {
	if (!self) return(0);
	return(_ count);
	}

Obj $(subInt,int ix) {
	if (ix<0 || ix>=_ count) return(0);
	return(*eleAddr(self,ix));
	}

pointer $(rawAddress) {
	return( (pointer)eleAddr(self,0) );
	}

#undef class
typedef Obj  OSig(addObject)(Obj array,Obj obj) ;               sig_(addObject);
typedef Obj  OSig(subInt)(Obj array,int ix) ;                   sig_(subInt);
typedef int  OSig(removeObject)(Obj array,Obj obj) ;            sig_(removeObject);
typedef void OSig(removeLast)(Obj array) ;                      sig_(removeLast);
typedef void OSig(removeAll)(Obj array) ;                       sig_(removeAll);
typedef Char OSig(ToDelimiter)(Obj container,int index) ;       sig_(ToDelimiter);
typedef Char OSig(ToStringSub)(Obj container,int index) ;       sig_(ToStringSub);
typedef Obj  OSig(NewBlock)(Obj proto,int count,pointer data) ; sig_(NewBlock);

#define class Array

	CodaClassZeros(bits,kize);
	#define Array_etor  freeO
	#define Array_ekeep keepO
CodaClass(Array,struct Array_,Root);

method$(Char,Info) {
	char *name=_$(get_name);
	if (!name) name="Dynamic object array";
	return Char_F("%s[%d] <%s>",kindO(self),_$(get_count),name);
	}

Array Array_NewBlock(Array proto,int count,pointer block) {
	Array self=Memory_newO(proto?proto:Class_Array,1);
	_$(insertBlock,0,count,block);
	return(self);
	}

void $(insertBlock,int index,int count,pointer block) {
	Pointer source=block; DTOR keeper=Memory_ekeep(self);
	if (keeper) for(int j=0;j<count;++j) keeper(source[j],self);
	Array_takeBlock(self,index,count,block);
	}

Obj $(insertAt,int index,Obj obj) {
	DTOR keeper=Memory_ekeep(self); if (keeper) keeper(obj,self);
	return CArray_insertAt(self,index,obj);
	}

void $(removeAt,int dix) {
	Array_removeBlock(self,dix,1);
	}

Obj $(addObject,Obj obj) {
	return _$2(insertAt,_$(get_count),obj);
	}

void $(takeObject,Obj $CONSUMED obj) {
	CArray_insertAt(self,_$(get_count),obj);
	}

void $(removeLast) {
	_$2(removeAt,_$(get_count)-1);
	}

Obj $(replaceAt,int ix,Obj obj) {
	Pointer ptr=Array_rawAddress(self); assert(ix>=0 && ix<Array_count(self));
		DTOR keeper=Memory_ekeep(self); if (keeper) keeper(obj,self);
		DTOR etor=Memory_etor(self); if (etor) etor(ptr[ix],self);
		ptr[ix]=obj;
	return(obj);
	}

method$(Char,xmlTag) { return Os("array"); }

int $(removeObject,Obj object) {
	int removed=0;
	for(int j=0;j<Array_count(self);++j) {
		if (Array_sub(self,j)==object) {
			Array_removeAt(self,j);
			--j; ++removed;
			}
		}
	return(removed);
	}

void $(removeAll) {
	Array_removeBlock(self,0,Array_count(self));
	}

method$(Char,ToDelimiter,int index) {
	if (index==1 && Array_name(self)) return Char_F("<%s>(",Array_name(self));
	static Char da[]={ Os(", "), Os("("), Os(")"), Os("?") }; return da[index&3];
	}

method$(Char,ToStringSub,int index) { return ToContainer( Array_sub(self,index) ); }

void $(set_name,char* name) {
	setO_take(_ name,(name && *name ? Char_Value(name) : 0));
	}

Char $(get_name) { return(_ name); }

$boot(StephenMJones) {
	CodaSig(get_count); CodaSig(subInt);
	CodaSig(NewBlock);
		CodaSig(addObject); CodaSig(removeLast);
		CodaSig(removeObject); CodaSig(removeAll);
	CodaSig(set_name); CodaSig(get_name);
	}

ConstChar Os_Set=Os("<set>");

void $(toaSet) { _$(set_name,0); _ name = Os_Set; }

bool $(isaSet) { return(_ name == Os_Set); }

#undef class

Char Array_Join(Array array,char *delimiter) {
	if (!array) return(0);
	if (!delimiter) delimiter="";
	int j,total=0,nel=Array_count(array),dsize=cs_length(delimiter),nout=0;
	for(j=0;j<nel;++j) {
		Obj obj=Array_sub(array,j);
  		if (isa_(obj,Char)) { ++nout; total+=cs_length(obj); }
		}
	if (nout<1)	return Char_Value("");
	Char blob=newOC(Char,total+1+dsize*nout);
	char *at=blob; nout=0; int check=0;
	for(j=0;j<nel;++j) {
		Obj obj=Array_sub(array,j);
  		if (isa_(obj,Char)) {
			if (nout) { cs_strcopy(at,delimiter); at+=dsize; check+=dsize; }
			int size=cs_length(obj); cs_strcopy(at,obj); at+=size; check+=size;
			++nout;
			}
		}
	++check;
	assert(check == total+1+dsize*(nout-1) );
	return(blob);
	}
typedef Char OSig(ToString)(Obj obj);               sig_(ToString);
typedef Obj  OSig(FromString)(Obj proto,char *str); sig_(FromString);

#define class Char
CodaClassZerosC();
CodaClass(Char,char,Root);

class Char_Value(const char *string) {
	if (!string) return(0);
	char *cp=newOC(Char,cs_length(string)+1); cs_strcopy(cp,string); return(cp);
	}

class $FORMAT12 Char_F(char *cs,...) {
	char buf[4096]; va_list ap; va_start(ap,cs); vsnprintf(buf,sizeof(buf),cs,ap); va_end(ap);
	return Char_Value(buf);
	}

method$(Char,Info) {
	char b[48]; snprintf(b,40,"%s",self); b[40]=0;
	return Char_F("%s[%d]=\"%s\" aka char*",kindO(self),obj_(get_count,self),b);
	}

Char $(FromString,char *str) { return Char_Value(str); }

method$(Char,ToString) { return(keepO(self)); }

method$(Char,xmlTag) { return Os("string"); }

class $(NewBlock,int count,pointer address) {
	if (count<=0) return(0);
	class cp=newOC(class,count); memcpy(cp,address,count); return(cp);
	}

$boot(StephenMJones) { CodaSig(FromString); CodaSig(NewBlock); }
#undef class

#define class ConstChar
CodaClassTransC(); CodaClass(ConstChar,char,Char);
#undef class

CDictionary Class_dictionary_dict=0;

CDictionary Class_dictionary(void) {
	if (!Class_dictionary_dict) {
		Class_dictionary_dict=newO(CDictionary);
		Dictionary_set_name(Class_dictionary_dict,"The*Class*Dictionary");
		Global_objectAddress(&Class_dictionary_dict);
		}
	return(Class_dictionary_dict);
	}

void Class_register(Obj obj) {
	Dict_set(Class_dictionary(),classO(obj)->kClass,obj);
	}

	typedef struct keydata_ { char *key; pointer data; } keydata;

	typedef struct CMux_ {
		keydata *array;
		int4 nel,mask,count,empty;
		int4 pow,rebuilding,maxHop,flags;
		int4 (*hasher)(const char *key);
		char resv[64-sizeof(pointer)-8*sizeof(int4)-sizeof(pointer)];
		} *CMux;

 static_assert(CodaCOSize==sizeof(struct CMux_),"TOTAL SIZE must equal 64");

	#define CMUXMINPOW  4
	#define CMUXMAXPOW 28

 void CMux_itor2(CMux mux,int pow) {
	mux->pow=pow;
	mux->nel=(1<<pow);
	mux->mask=(mux->nel -1);
	mux->array=alocC(sizeof(keydata)*mux->nel);
	mux->empty=0;
	mux->maxHop=0;
	}

static void CMux_itor(CMux mux) {
	CMux_itor2(mux,CMUXMINPOW);
	}

void CMux_dtor(pointer cmux) {
	CMux mux=cmux;
	freeC(mux->array); mux->array=0;
	}

void *CMux_Data4Key(pointer cmux,char *key) {
	CMux mux=cmux; if (!mux->array) return(0);
	int j,h=(int)(intptr_t)key;
	for(j=0;j<mux->nel;++j) {
		int ix= (h+j)&mux->mask;
		keydata *ele=(&mux->array[ix]);
		char *cp=ele->key; if (!cp) break;
		if (cp==key) return ele->data;
		}
	return(0);
	}

static void CMux_Rebuild(CMux mux,int count) {
	assert(!mux->rebuilding);
	int pow=0;
	for(pow=CMUXMINPOW;pow<CMUXMAXPOW;++pow)  {
		int nel=(1<<pow);
		if (nel>=count) break;
		}
	CMux grade=alocO(sizeof(struct CMux_)); CMux_itor2(grade,pow);
		grade->rebuilding=1;
		for(int j=0;j<mux->nel;++j) {
			keydata *ele=(&mux->array[j]);
			if (ele->data) CMux_KeyData(grade,ele->key,ele->data);
			}
		grade->rebuilding=0;

		struct CMux_ temp;
		temp=(*mux); (*mux)=(*grade); (*grade)=temp;
	CMux_dtor(grade); freeO(grade);
	}

static void CMux_Check(CMux mux) {
	int eigth=mux->nel/8;
	if (mux->nel - mux->count - mux->empty < eigth) {
		CMux_Rebuild(mux,mux->count*2);
		}
	}

void *CMux_KeyData(pointer cmux,char *key,pointer data) {
	CMux mux=cmux; if (!mux->array) CMux_itor(mux);

	int j,h=(int)(intptr_t)key;
	for(j=0;j<mux->nel;++j) {
		int ix= (h+j)&mux->mask;
		keydata *ele=(&mux->array[ix]);
		if (!ele->key || ele->key==key) {
			if (!data && !ele->data) return(0);
			ei (!ele->key) ++mux->count;
			ei (!data &&  ele->data) { --mux->count; ++mux->empty; }
			ei ( data && !ele->data) { ++mux->count; --mux->empty; }
			ele->key=key;
			ele->data=data;
			if (j>mux->maxHop) mux->maxHop=j;
			CMux_Check(mux);
			return(0);
			}
		}
	Die_("CMux; hash full, dynamic expansion failed.");
	return(data);
	}

$boot() {
	Version_register( "Coda-C",
	Os(QVersion ", " QWebsite ", Copyright (c) " QCopyYears "  Stephen M. Jones, Affero GPL 3."
	"\000"));
	static_assert(sizeof(huge)==8,"wrong size huge?");
	}

	typedef struct CodaStructMeta_ {
		CodaCLASS *clas;
		unsigned int size:24;
		unsigned int fsegs:8;
		_Atomic unsigned short count;
		unsigned short metasize;
		} CodaStructMeta;

	#define meta2obj(meta)	( (void *)(&meta[1]))
	#define obj2meta(obj)	( &((CodaStructMeta *)obj)[-1] )
	#define obj2meta0(obj)  ( (CodaStructMeta *)( ((void *)obj) - obj2meta(obj)->metasize ) )

	enum { CodaMaxMeta=99, };

bool OisaClass(Obj obj,Obj classobj) {
	CodaCLASS *hunt=classO(classobj);
	CodaCLASS *clas=classO(obj);
	for(int j=0;j<=CodaMaxMeta;++j) {
		if (clas==hunt) return(1);
		clas=clas->superClass; if (!clas) break;
		}
	return(0);
	}

typedef void OSig(setKey)(Obj dict,char *key,Obj obj) ; sig_(setKey);
typedef Obj  OSig(subKey)(Obj dict,char *key) ;         sig_(subKey);
typedef bool OSig(removeKey)(Obj dict,char *key) ;      sig_(removeKey);
typedef Pointer OSig(AllKeys)(Obj dict) ;               sig_(AllKeys);
typedef Char OSig(xmlTag)(Obj obj);                     sig_(xmlTag);

#define class Dictionary

	typedef struct csKeyword_ {
		struct csKeyword_ *nxt;
		pointer data;
		char key[0];
		} csKeyword;

	struct Dictionary_ {
		csKeyword *list;
		Char name;
		struct CMux_ mux;
		};

void $(dtor,CodaCLASS *clas) {
	if (_ mux.hasher) CMux_dtor(& _ mux);
	csKeyword *ptr,*nxt=0; DTOR etor=clas->etor;
	for(ptr=_ list;ptr;ptr=nxt) {
		nxt=ptr->nxt;
		if (etor) etor(ptr->data,self);
		freeC(ptr);
		}
	freeO(_ name); _ name=0; _ list=0; _ mux.count=0;
	}

	CodaClassZeros(itor,bits,kize);
	#define Dictionary_etor  freeO
	#define Dictionary_ekeep keepO
CodaClass(Dictionary,struct Dictionary_,Root);

int $(get_count) {
	if (!self) return(0);
	return(_ mux.count);
	}

void $(setKey,char *key,Obj obj) {
	if (_ mux.hasher) { DictHash_keyobj(self,key,obj); return; }
	if (!obj) { _$(removeKey,key); return; }
	DTOR keeper=Memory_ekeep(self); if (keeper) keeper(obj,self);
	csKeyword *ptr;
	for(ptr=_ list;ptr;ptr=ptr->nxt) { if (cs_exact(key,ptr->key)) break; }
	if (ptr) {
		DTOR etor=Memory_etor(self);
		if (etor) etor(ptr->data,self);
		}
	  else {
		++_ mux.count;
		ptr=alocC(sizeof(csKeyword)+cs_length(key)+1);
			cs_strcopy(ptr->key,key);
		ptr->nxt=_ list; _ list=ptr;
		}
	ptr->data=obj;
	_$(auto);
	}

bool $(removeKey,char *key) {
	if (_ mux.hasher) return DictHash_keydel(self,key);
	csKeyword *ptr,*pre;
	for(pre=0,ptr=_ list;ptr;pre=ptr,ptr=ptr->nxt) { if (cs_exact(key,ptr->key)) break; }
	if (ptr) {
		DTOR etor=Memory_etor(self); if (etor) etor(ptr->data,self);
		if (!pre) _ list=ptr->nxt;
		  else	  pre->nxt=ptr->nxt;
		freeC(ptr); --_ mux.count;
		return(1);
		}
	return(0);
	}

int $(removeObject,Obj obj) {
	int removed=0;
	DTOR etor=Memory_etor(self);
	for(csKeyword *pre=0,*nxt=0,*ptr=_ list;ptr;pre=ptr,ptr=nxt) {
		nxt=ptr->nxt;
		if (ptr->data==obj) {
			if (_ mux.hasher) {
				if (!DictHash_keydel(self,ptr->key)) Die_("Internal error, hash delete failed!");
				}
			  else {
				if (etor) etor(ptr->data,self);
				if (!pre) _ list=ptr->nxt;
				  else	  pre->nxt=ptr->nxt;
				freeC(ptr); --_ mux.count;
				}
			++removed;
			ptr=pre;
			}
		}
	return(removed);
	}

Obj $(subKey,char *key) {
	if (_ mux.hasher) return DictHash_obj4key(self,key);
	for(csKeyword *ptr=_ list;ptr;ptr=ptr->nxt) if (cs_exact(key,ptr->key)) return(ptr->data);
	return(0);
	}

Pointer $(AllKeys) {
	int nel=Dictionary_count(self); if (nel<=0) return(0);
	Pointer vec=newOC(Pointer,nel);
	csKeyword *ptr=_ list;
	for(int j=0;ptr;ptr=ptr->nxt,++j)
		vec[(nel-1-j)]=ptr->key;
	return(vec);
	}

method$(Char,Info) {
	int nel=Dictionary_count(self);
	char *name=Dictionary_name(self);
	if (!name) name="Keyword array";
	return Char_F("%s[%d] <%s>",kindO(self),nel,name);
	}

method$(Char,xmlTag) { return Os("dict"); }

method$(Char,ToDelimiter,int index) {
	if (index==1 && Dictionary_name(self)) return Char_F("<%s>{",Dictionary_name(self));
	static Char da[]={ Os(", "), Os("{"), Os("}"), Os("?") }; return da[index&3];
	}

method$(Char,ToStringSub,int index) {
	Keyword key=Dictionary_keywordSub(self,index);
	cleanO Array temp=newO(Array);
	Array_takeObject(temp,Char_Value(key->word));
	Array_takeObject(temp,ToContainer(key->item));
	return Array_Join(temp," : ");
	}

void $(set_name,char* name) {
	setO_take(_ name,(name && *name ? Char_Value(name) : 0));
	}

Char $(get_name) { return _ name; }

$boot(StephenMJones) {
	CodaSig(get_count); CodaSig(AllKeys);
	CodaSig(set_name); CodaSig(get_name);
	CodaSig(setKey); CodaSig(subKey);
	CodaSig(removeKey); CodaSig(removeObject);
	}

void $(takeKey,char *key,Obj $CONSUMED obj) {
	Dict_set(self,key,obj);
	freeO(obj);
	}

#undef class

#define class CDictionary
	CodaClassZeros(etor,itor,bits,kize,ekeep);
	#define CDictionary_dtor    Dictionary_dtor
CodaClass(CDictionary,struct Dictionary_,Root);

method$(Char,Info) {
	int nel=Dictionary_get_count(self);
	char *name=Dictionary_get_name(self);
	if (!name) name="C Keyword array of raw pointers";
	return Char_F("%s[%d] <%s>",kindO(self),nel,name);
	}

method$(Char,ToString) { return Char_F("%s[%d]",kindO(self),Dictionary_get_count(self)); }

#undef class
	void Dictionary_auto(Dictionary dict) { }
	bool DictHash_keyobj(Dictionary dict,char *key,pointer obj) { Die_("%s; is not defined.",__func__); }
	bool DictHash_keydel(Dictionary dict,char *key) { Die_("%s; is not defined.",__func__); }
	Obj DictHash_obj4key(Dictionary dict,char *key) {  Die_("%s; is not defined.",__func__); }

Keyword Dictionary_scan(Dictionary dict) {
	if (!dict) return(0);
	csKeyword *head=dict->list;
	if (!head) return(0);
	return((Keyword)&head->data);
	}

Keyword Dictionary_next(Keyword element) {
	Pointer dataptr=(Pointer)element;
	csKeyword *ptr=(csKeyword *)(&dataptr[-1]);
	if (!ptr->nxt) return(0);
	return((Keyword)&ptr->nxt->data);
	}

Keyword Dictionary_keywordSub(Dictionary dict,int index) {
	csKeyword *ptr; int j=0;
	for(ptr=dict->list;ptr;ptr=ptr->nxt,++j) if (index==j) break;
	if (!ptr) return(0);
	return((Keyword)&ptr->data);
	}

#if 1==0

Keyword Dictionary_keywordFromWord(char *word) {
	}

#endif

CArray Global_objectPointers=0;

void Global_objectAddress(pointer address) {
	if (!Global_objectPointers) Global_objectPointers=newO(CArray);
	Array_addObject(Global_objectPointers,address);
	}

void Global_objectsZero(void) {
	if (!Global_objectPointers) return;
	int nel=Array_count(Global_objectPointers);
	Pointer vector=Array_rawAddress(Global_objectPointers);
	for(int j=nel-1;j>=0;--j) {
		Pointer address=vector[j];
		freeO(*address); *address=0;
		}
	freeO(Global_objectPointers); Global_objectPointers=0;
	}
#define class Keyword

static void $(dtor) { freeO(_ item); }

	CodaClassZeros(etor,itor,bits,ekeep);
	#define Keyword_kize    sizeof(struct Keyword_)
CodaClass(Keyword,struct Keyword_,Root);

class Keyword_Value(char *word,Obj item) {
	if (!word) return(0);
	class self=newOC(class,cs_length(word)+1);
		cs_strcopy(_ word,word);
		_ item=keepO(item);
	return(self);
	}

class Keyword_TakeItem(char *word,Obj item) {
	if (!word) return(0);
	class self=newOC(class,cs_length(word)+1);
		cs_strcopy(_ word,word);
		_ item=item;
	return(self);
	}

method$(Char,Info) {
	cleanO Char msg=obj_(Info,_ item);
	return Char_F("Keyword[%d+%d] {%s : %s}",classO(self)->kize,Root_count(self),_ word,msg);
	}

method$(Char,xmlTag) { return Os("keyword"); }

method$(Char,ToString) {
	cleanO Char msg=ToContainer(_ item);
	return Char_F("<keyword>{%s : %s}",_ word,msg);
	}

void $(set_item,Obj object) { setO(_ item,object); }

Obj $(get_item) { return(_ item); }

#undef class

	#define alocOCE(sz,cl,ex)	Memory_aloc(sz,cl,ex)

	static_assert(CodaMetaSize==sizeof(CodaStructMeta),"MetaSize Error.");

Obj Memory_alocO(int size) { return Memory_aloc(size,0,0); }

_Atomic huge Object_Counter=0;

Obj Memory_aloc(int size,CodaCLASS *clas,int extra) {
	if (size<=0) Die_("size %d ? class: %s",size,(clas ? clas->kClass : "?ZERO?"));
	CodaStructMeta *first=alocC(extra+CodaMetaSize+size);
	CodaStructMeta *meta=first; if (extra) meta= (CodaStructMeta *) ( ((char *)first) + extra);
	meta->metasize=extra+CodaMetaSize;
	meta->size=size;
	meta->count=1;
	meta->fsegs=0;
	meta->clas=clas;
	++Object_Counter;
	return(meta2obj(meta));
	}

	CodaStructMeta *Memory_metanext( CodaStructMeta *meta) ;
	int Memory_totalMeta0Size(CodaStructMeta *meta) ;

bool Memory_SecurityOn=1;

void Memory_free(Obj obj) {
	if (!obj) return;
	CodaStructMeta *meta=obj2meta(obj); if (meta->metasize==0) return;
	meta=obj2meta0(obj);
	if (!meta->count) return;
	int count= --meta->count;
	if (count==0) {
		CodaStructMeta *ptr=meta;
		for(int j=0;ptr && j<CodaMaxMeta;ptr=Memory_metanext(ptr),++j) {
			CodaCLASS *clas=ptr->clas;
			while (clas) {
				if (clas->dtor) clas->dtor(meta2obj(ptr),clas);
				if (clas->bits!=bits_Trans) break;
				clas=clas->superClass;
				}
			}
		assert(!ptr);
		if (Memory_SecurityOn) cs_blockZero(meta,Memory_totalMeta0Size(meta));
		freeC(meta);
		--Object_Counter;
		}
	}

Obj Memory_keep(Obj obj) {
	if (!obj) return(obj);
	if (!obj2meta(obj)->metasize) return(obj);
	CodaStructMeta *meta=obj2meta0(obj);
	if (meta->count) ++meta->count;
	return(obj);
	}

void Memory_clean(const void *vp) {
	Memory_free(*(Obj *)vp);
	}

CodaCLASS* Memory_class(Obj obj) {
	if (!obj) return(&Isa_Void);
	CodaStructMeta *meta=obj2meta(obj);
	if (meta->metasize==0) return(&Isa_ConstChar);
	if (!meta->clas) return(&Isa_Void);
	return(meta->clas);
	}

DTOR Memory_etor(Obj obj) { return classO(obj)->etor; }

DTOR Memory_ekeep(Obj obj) { return classO(obj)->ekeep; }

char* Memory_kind(Obj obj) { return classO(obj)->kClass; }

int4 Memory_size(Obj obj) {
	if (!obj) return(0);
	CodaStructMeta *meta=obj2meta(obj);
	if (meta->metasize==0) return cs_length(obj)+1;
	return(meta->size);
	}

int4 Memory_count(Obj obj) {
	if (!obj) return(-1);
	CodaStructMeta *meta=obj2meta(obj);
	if (meta->metasize==0) return(0);
	return obj2meta0(obj)->count;
	}

CodaCLASS* Memory_reclass(Obj obj,CodaCLASS *clas) {
	assert(obj);
	CodaStructMeta *meta=obj2meta(obj);
	assert(meta->metasize);
	void *old=meta->clas; meta->clas=clas; return(old);
	}

	static int pow2align=0;
int Memory_align(int pow2) { int old=pow2align; pow2align=pow2; return(old); }

static pointer Memory_pack(void *enclosed,int newsize,void *clas) {
	assert(enclosed && newsize>0); assert(pow2align==0 || newsize%pow2align==0);
	CodaStructMeta* eeta=obj2meta(enclosed);
	int avail= eeta->metasize - 2*CodaMetaSize;
	if (newsize>avail) Die_("Internal Error: newsize %d, avail %d",newsize,avail);
	CodaStructMeta* meta= (CodaStructMeta*)( ((char *)eeta) - newsize - CodaMetaSize);

	meta->metasize= eeta->metasize - newsize - CodaMetaSize;
	meta->size=newsize;
	meta->count=1;
	meta->fsegs= eeta->fsegs+1;
	meta->clas=clas;
	return(meta2obj(meta));
	}

 static Obj Memory_newOOR(CodaCLASS *clas,int count,int extra) {
	if (!clas) Die_(">>>use alocO() instead of newO() to make VOID class objects!");
	assert(clas!=NULL && count>0 && extra>=0);
	assert(clas->superNel>=1); assert(clas->size>0);
	int size=count*clas->size,trans=0;
	if (clas->kize) size=clas->kize+count;
	int supNel=clas->superNel;
	if (clas->bits<0)	{
		size=0; trans=1; assert(count>=1 && supNel>=1);
		if (supNel!=1 && count!=1)
			Die_("You can't create multiple of Arrayed Classes(%d * %d)",supNel,count);
		supNel*=count;
		}
	assert(size+extra<0x7FFFFF);
	Obj obj=0;
	if (clas->superClass && clas->superClass->bits!=bits_Root) {
		if (count!=1 && !trans)	{
			Die_("You can only ARRAY Bases classes(super==Root) OR Transparent classes! { %s }",clas->kClass);
			}
		obj=Memory_newOOR(clas->superClass,supNel,extra+size+(size?CodaMetaSize:0));
		if (size)	obj=Memory_pack(obj,size,clas);
		  else		Memory_reclass(obj,clas);
		}
	  else	{
		obj=alocOCE(size,clas,extra);
		}
	if (clas->itor) clas->itor(obj,clas);
	return(obj);
	}

Obj Memory_newOO(CodaCLASS *clas,int count) { return Memory_newOOR(clas,count,0); }

Obj Memory_newO(Obj obj,int nel) {
	if (nel<1) Die_("newOC() called with count=%d.",nel);
	return Memory_newOO(Memory_class(obj),nel);
	}

struct CodaStructMeta_ *Memory_metanext( struct CodaStructMeta_ *meta) {
	if (meta->fsegs<=0) return(0);
	return ((void*)meta)+meta->size+CodaMetaSize;
	}

	int Memory_totalMeta0Size(CodaStructMeta *meta) {
		int j=0,total=0;
		for(j=0;meta && j<CodaMaxMeta;meta=Memory_metanext(meta),++j) { total+=meta->size+CodaMetaSize; }
		assert(j<CodaMaxMeta);
		return(total);
		}

CodaCLASS* baseClass(CodaCLASS* clas) {
	if (clas) while(clas->bits<0) {
		clas=clas->superClass;
		if (!clas) break;
		}
	return(clas);
	}

Obj Memory_objectToClass(Obj obj,CodaCLASS* newClass) {
	if (!obj || !newClass)	return(obj);
	CodaCLASS *clas=Memory_class(obj);
	if (!clas)				return(obj);
	if (clas==newClass)		return(obj);

	CodaCLASS *base=baseClass(newClass);
	CodaStructMeta *meta=obj2meta(obj);
	if (meta->metasize==0) return(obj);
	meta=obj2meta0(obj);
	for(CodaStructMeta *ptr=meta;ptr;ptr=Memory_metanext(ptr)) {
		if (baseClass(ptr->clas)==base) return meta2obj(ptr);
		}
	return(0);
	}

Obj Memory_superObject(Obj obj) {
	if (obj) {
		CodaStructMeta *meta=obj2meta(obj);
		if (meta->metasize==0) return(0);
		meta=Memory_metanext(meta); if (!meta) return(0);
		return meta2obj(meta);
		}
	return(obj);
	}

void (*Msg_hook)(Char msg,int log)=0;

void Msg_Object(Char $CONSUMED msg,int log) {
	if (Msg_hook) Msg_hook(msg,log);
	  else	fprintf(stderr,"%s\n",msg);
	freeO(msg);
	}

void (*Die_hook)(Char msg,int trace)=0;

Obj $NORETURN Die_Object(Char $CONSUMED msg,int trace) {
	if (Die_hook) Die_hook(msg,trace);
	  else	fprintf(stderr,"%s\n",msg);
	freeO(msg);
	if (!trace) exit(-1);
	abort();
	exit(-2);
	}

#define class Pointer
CodaClassZerosC();
CodaClass(Pointer,pointer,Root);

class Pointer_Value(pointer value) {
	class self=newO(class); *self=value; return(self);
	}

class $(NewBlock,int count,pointer block) {
	if (count<=0) return(0);
	class source=block; self=newOC(class,count);
	for(int j=0;j<count;++j) self[j]=source[j];
	return(self);
	} $boot1(NewBlock);

method$(Char,Info) { return Char_F("Pointer[%d] aka void**",Pointer_count(self)); }

method$(Char,ToDelimiter,int index) {
	static Char da[]={ Os(", "), Os("<pointer>["), Os("]"), Os("?") }; return da[index&3];
	}

method$(Char,ToStringSub,int index) { return Char_F("%p",self[index]); }

method$(Char,ToString) { return Char_F("%p",*self); }

#undef class

int Pointer_length(pointer address) {
	Pointer self=address; if (!self) return(0);
	int nel=0; for(;self[nel];++nel) ;
	return(nel);
	}

typedef int   OSig(get_count)(Obj obj) ;           sig_(get_count);
typedef Char  OSig(Info)(Obj obj) ;                sig_(Info);
typedef void  OSig(set_name)(Obj obj,char* name) ; sig_(set_name);
typedef Char  OSig(get_name)(Obj obj) ;            sig_(get_name);

#define class Root

CodaClassZeros(dtor,etor,itor,kize,ekeep); CodaClass_Root();
Coda_Class(Root,void,0);

Char (*Root_Diag)(Obj self)=0;

Char $(Info) {
	char *class=kindO(self);
	int nel=sizeO(self);
	cleanO Char msg=(Root_Diag?Root_Diag(self):Os(""));
	char* sep=(Root_Diag?"\n":"");
	int count=Root_count(self);
	int kize=classO(self)->kize;
	if (kize)	return Char_F("Class:%s[%d+%d][size=%d]%s%s",class,kize,count,nel,sep,msg);
	  			return Char_F("Class:%s[%d][size=%d]%s%s",   class,     count,nel,sep,msg);
	} $boot1(Info);

method$(Char,get_name) { return(0); }

int $(get_count) {
	CodaCLASS* clas=classO(self);
	if (clas->kize) return(sizeO(self) - clas->kize);
	return(sizeO(self)/clas->size);
	} $boot1(get_count);

method$(Char,ToString) {
	return Char_F("%s[%d]",kindO(self),_$(get_count));
	}

#undef class

CDictionary Signature_dictionary_dict=0;

CDictionary Signature_dictionary(void) {
	if (!Signature_dictionary_dict) {
		Signature_dictionary_dict=newO(CDictionary);
		Dictionary_set_name(Signature_dictionary_dict,"The*Signature*Dictionary");
		Global_objectAddress(&Signature_dictionary_dict);
		}
	return(Signature_dictionary_dict);
	}

void Signature_register(Char sig) {
	Dict_set(Signature_dictionary(),sig,sig);
	}

Char Signature_fromString(char *str) {
	return Dict_sub(Signature_dictionary(),str);
	}

void codac_sig(pointer cmux,char *key,pointer data) {
	CMux_KeyData(cmux,key,data);
	Signature_register(key);
	}

Obj Memory_normalize(Obj obj) {
	if (!obj) return(obj);
	CodaStructMeta *meta=obj2meta(obj); if (meta->metasize==0) return(obj);
	meta=obj2meta0(obj);
	return meta2obj(meta);
	}

Obj Memory_spindle(Obj obj,CodaCLASS* newClass) {
	if (!obj) return(0);
	CodaStructMeta *meta=obj2meta(obj);
	if (meta->metasize==0 || !meta->clas) return(0);
	CodaCLASS *base=baseClass(newClass);
	    if (baseClass(meta->clas)==base) return(obj);
	for(meta=obj2meta0(obj); meta; meta=Memory_metanext(meta)) {
		if (baseClass(meta->clas)==base) return meta2obj(meta);
		}
	return(0);
	}

void *ORespond3(Obj obj,char *sel,Obj *spindle,int noDie,CodaCLASS *superOf) {
	CodaCLASS *clas=0; CodaStructMeta *meta=0; bool superfound=(superOf ? 0 : 1); Obj rootobj=obj;
	if (obj) {
		meta=obj2meta(obj);
		if (meta->metasize==0) clas=(&Isa_ConstChar);
		  else {
			meta=obj2meta0(obj);
			obj=meta2obj(meta); clas=meta->clas;
			}
		}
	if (!clas) clas=(&Isa_Void);
	void *data=0; char *diagstr=clas->kClass;
	for(int j=0;1;++j) {
		if (superfound) { data=CMux_Data4Key(clas->classObject,sel); if (data) break; }
		if (superOf==clas) superfound=1;
		if (meta && clas->bits!=bits_Trans) {
			if (meta->metasize==0) meta=0;
			  else meta=Memory_metanext(meta);
			if (meta) obj=meta2obj(meta);
			}
		clas=clas->superClass;
		if (!clas || !meta) {
			if (clas && clas->bits==bits_Root) {
				if (superfound) data=CMux_Data4Key(clas->classObject,sel);
				if (data) { obj=rootobj; break; }
				}
			if (spindle && !noDie) Die_("Class:%s does not respond to: %s, superOf: %s",diagstr,sel,
				(superOf ? superOf->kClass : "n/a"));
			break;
			}
		if (j>CodaMaxMeta) Die_("Circular Isa-Classes for Class: %s",diagstr);
		}
	if (spindle) *spindle=obj;
	return(noDie==2 ? obj : data);
	}

Char ToContainer(Obj container) {
	void* hasele=OResponds(container,ToStringSub);
	void* hastos=OResponds(container,ToString);
	void* hasdel=OResponds(container,ToDelimiter);
	if (hasele && hasdel) {
		cleanO Array temp=newO(Array);
		Array_take(temp, obj_(ToDelimiter,container,1) );
		int nel=obj_(get_count,container);
		for(int j=0;j<nel;++j) {
			if (j) Array_take(temp, obj_(ToDelimiter,container,0) );
			Obj str=obj_(ToStringSub,container,j);
			Array_take(temp,str);
			}
		Array_take(temp, obj_(ToDelimiter,container,2) );
		return Array_Join(temp,0);
		}
	if (hasele) return obj_(ToStringSub,container,0);
	if (hastos) return obj_(ToString,container);
	return Os("?");
	}
CDictionary Version_dictionary_dict=0;

CDictionary Version_dictionary(void) {
	if (!Version_dictionary_dict) {
		Version_dictionary_dict=newO(CDictionary);
		Dictionary_set_name(Version_dictionary_dict,"The*Version*Dictionary");
		Global_objectAddress(&Version_dictionary_dict);
		}
	return(Version_dictionary_dict);
	}

void Version_register(char *key,char *text) {
	Dict_set(Version_dictionary(),key,text);
	}

void codac_versions(void) {
	CDictionary dict=Version_dictionary();
	for(Keyword key=Dictionary_scan(dict);key;key=Dictionary_next(key)) {
		fprintf(stderr,"%s: %s\n",key->word,(char *)key->item);
		}
	}

#define class Void

CodaClassZeros(dtor,etor,itor,kize,ekeep); CodaClass_Root();
Coda_Class(Void,void,0);

method$(Char,Info) {
	if (!self) return Os("(0)");
	return Char_F("Void_[%d]=%p void*",Root_count(self),self);
	}

method$(int,get_count) { return sizeO(self); }

method$(Char,get_name) { return(0); }

method$(Char,ToString) {
	if (!self) return Os("<Null>");
	return Char_F("Void[%d]",sizeO(self));
	}

#undef class

