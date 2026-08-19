#pragma once
/* www.coda-c.com  coda-c.h

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

	#include <string.h>
	#include <stdlib.h>
	#include <stdbool.h>
	#include <stdint.h>
	typedef struct tm struct_tm;
	#include <stdio.h>
	#include <assert.h>

	typedef void*     Obj;
	typedef void*     pointer;
	typedef long long huge;
	typedef int32_t   int4;
	typedef char*     Char;
	typedef char*     ConstChar;

	#define $CONSTRUCTOR  __attribute__((constructor))
	#define $CLEANUP(fun) __attribute__((cleanup (fun)))
	#define $CONSUMED
	#define $FORMAT12     __attribute__((format (coda_printf, 1, 2)))
	#define $FORMAT23     __attribute__((format (coda_printf, 2, 3)))
	#define $NORETURN     __attribute__((__noreturn__))

	#if     WIN32
		#define coda_printf gnu_printf
	#elif __linux__
		#define LINUX 1
		#define coda_printf printf
	#else
		#define APPLE 1
		#define coda_printf printf
	#endif

	#define OBind1_2(class,fun) class##_##fun
	#define OBind1_2M(a,b) OBind1_2(a,b)
	#define OClass_(x) Class_ ## x
	#define OClass(x) OClass_(x)

	#define CodaSig(fun)       codac_sig(OClass(class),OBind1_2(sig,fun),OBind1_2M(class,fun))
	#define CodaAka(fun,clas2) codac_sig(OClass(class),OBind1_2(sig,fun),OBind1_2(clas2,fun))
	#define OSig(sig)			(*sig)
	#define OSig$(sig,...) (*sig)(Obj,##__VA_ARGS__); sig_(sig)
	#define sig_(name) ConstChar OBind1_2(sig,name)=Os(#name)

	#define $class(name) OBind1_2M(class,name)
	#define $boot(name) static $CONSTRUCTOR void $class(name)(void)

	typedef void (*DTOR)(pointer,pointer);

	#define CodaClassDef(clas,stor,...) typedef stor* clas; \
		extern clas OBind1_2(Class,clas); \
		extern CodaCLASS OBind1_2(Isa,clas)

	typedef struct CodaCLASS_ CodaCLASS;

	struct CodaCLASS_ {
		DTOR dtor;
		char *kClass;
		CodaCLASS *superClass;
		DTOR etor;
		int  size;
		short superNel;
		short bits;
		DTOR itor;
		int  kize,spare;
		pointer classObject;
		pointer custom;
		DTOR ekeep;
		struct Dictionary_* properties;
		pointer spare2,spare3,spare4;
		};

	#define	CodaMetaSize	(sizeof(void *)+8)
	#define CodaCOSize 64
	struct CodaPublicMeta_ {
		CodaCLASS *clas;
		char padding[CodaMetaSize-sizeof(pointer)-sizeof(unsigned short)];
		unsigned short metasize;
		};
	struct CodaPublicCORaw_ {
		struct CodaPublicMeta_ meta;
		char COData[CodaCOSize];
		};
	enum { CodaClass_SUPERNEL=1, CodaClass_CUSTOM=0 };
	#define ORegisterClass(CLASS) static $CONSTRUCTOR void OBind1_2(com_coda_c_www,CLASS)(void) { \
			Class_register(OClass(CLASS)); }

	#define CodaClass(CLASS,CodaType,super,...) CodaClassDef(CLASS,CodaType,super); \
			Coda_Class(CLASS,CodaType,&Isa_##super)
	#define Coda_Class(CLASS,CodaType,SUPER,...) \
		static struct CodaPublicCORaw_ OBind1_2(Meta,CLASS); \
		CodaCLASS OBind1_2(Isa,CLASS)={ \
			(DTOR)OBind1_2(CLASS,dtor), \
			#CLASS,\
			SUPER, \
			(DTOR)OBind1_2(CLASS,etor), \
			sizeof(CodaType), \
			CodaClass_SUPERNEL, \
			OBind1_2(CLASS,bits), \
			(DTOR)OBind1_2(CLASS,itor), \
			OBind1_2(CLASS,kize), \
			0, \
			&OBind1_2(Meta,CLASS).COData, \
			(pointer)CodaClass_CUSTOM, \
			(DTOR)OBind1_2(CLASS,ekeep), \
			}; \
		static struct CodaPublicCORaw_ OBind1_2(Meta,CLASS)={{&OBind1_2(Isa,CLASS),"",CodaMetaSize},""}; \
		CLASS OClass(CLASS)=(pointer)(&OBind1_2(Meta,CLASS).COData); \
		ORegisterClass(CLASS)

	enum { bits_Root = -1, bits_Trans =	-2 };

	#define	sizeat(type) sizeof(*((type)0))

	#define cs_length(string)	((int)strlen(string))
	#define cs_exact(str1,str2)	(0==strcmp(str1,str2))
	#define cs_strcopy(str1,str2)	strcpy(str1,str2)
	#define cs_blockCopy(dest,sour,nel) memcpy(dest,sour,nel)
	#define cs_blockFill(dest,chr,nel) memset(dest,chr,nel)
	#define cs_blockCmp(a,b,nel) memcmp(a,b,nel)
	#define cs_blockZero(addr,nel) cs_blockFill(addr,0,nel)
	#define alocS(size)  cs_blockZero(alloca(size),size);
#define CODA_shift(                          _1,_2,_3,_4,_5,_6,_7,_8,_9, N ,...) N
#define CODA_pad(...) CODA_shift(__VA_ARGS__,_9,_8,_7,_6,_5,_4,_3,_2,_1,_0)
#define OBind123_(one,two,three) one##two##three
#define OBind123(one,two,three) OBind123_(one,two,three)

#define CODA_1_Z(fun,eva,a)                 fun(#a,eva(a))
#define CODA_2_Z(fun,eva,a,b)               fun(#a,eva(a)), fun(#b,eva(b))
#define CODA_3_Z(fun,eva,a,b,c)             fun(#a,eva(a)), fun(#b,eva(b)), fun(#c,eva(c))
#define CODA_4_Z(fun,eva,a,b,c,d)           fun(#a,eva(a)), fun(#b,eva(b)), fun(#c,eva(c)), fun(#d,eva(d))
#define CODA_5_Z(fun,eva,a,b,c,d,e)         fun(#a,eva(a)), fun(#b,eva(b)), fun(#c,eva(c)), fun(#d,eva(d)), \
	fun(#e,eva(e))
#define CODA_6_Z(fun,eva,a,b,c,d,e,f)       fun(#a,eva(a)), fun(#b,eva(b)), fun(#c,eva(c)), fun(#d,eva(d)), \
	fun(#e,eva(e)), fun(#f,eva(f))
#define CODA_7_Z(fun,eva,a,b,c,d,e,f,g)     fun(#a,eva(a)), fun(#b,eva(b)), fun(#c,eva(c)), fun(#d,eva(d)), \
	fun(#e,eva(e)), fun(#f,eva(f)), fun(#g,eva(g))
#define CODA_8_Z(fun,eva,a,b,c,d,e,f,g,h)   fun(#a,eva(a)), fun(#b,eva(b)), fun(#c,eva(c)), fun(#d,eva(d)), \
	fun(#e,eva(e)), fun(#f,eva(f)), fun(#g,eva(g)), fun(#h,eva(h))
#define CODA_9_Z(fun,eva,a,b,c,d,e,f,g,h,i) fun(#a,eva(a)), fun(#b,eva(b)), fun(#c,eva(c)), fun(#d,eva(d)), \
	fun(#e,eva(e)), fun(#f,eva(f)), fun(#g,eva(g)), fun(#h,eva(h)), fun(#i,eva(i))
#define CODA_macro(macro, ...) macro(__VA_ARGS__)
#define CODA_Zx(fun,eva,...) CODA_macro(OBind123(CODA,CODA_pad(__VA_ARGS__),_Z),fun,eva,__VA_ARGS__)
#define CODA_1_F(fun,a)                 fun(a)
#define CODA_2_F(fun,a,b)               fun(a), fun(b)
#define CODA_3_F(fun,a,b,c)             fun(a), fun(b), fun(c)
#define CODA_4_F(fun,a,b,c,d)           fun(a), fun(b), fun(c), fun(d)
#define CODA_5_F(fun,a,b,c,d,e)         fun(a), fun(b), fun(c), fun(d), fun(e)
#define CODA_6_F(fun,a,b,c,d,e,f)       fun(a), fun(b), fun(c), fun(d), fun(e), fun(f)
#define CODA_7_F(fun,a,b,c,d,e,f,g)     fun(a), fun(b), fun(c), fun(d), fun(e), fun(f), fun(g)
#define CODA_8_F(fun,a,b,c,d,e,f,g,h)   fun(a), fun(b), fun(c), fun(d), fun(e), fun(f), fun(g), fun(h)
#define CODA_9_F(fun,a,b,c,d,e,f,g,h,i) fun(a), fun(b), fun(c), fun(d), fun(e), fun(f), fun(g), fun(h), fun(i)
#define CODA_ENUM(fun,...) enum { CODA_macro(OBind123(CODA,CODA_pad(__VA_ARGS__),_F),fun,__VA_ARGS__) }
#define CodaClassZero1(x) OBind1_2M(class,x)=0
#define CodaClassZeros(...) CODA_ENUM(CodaClassZero1,__VA_ARGS__)

#define CodaClassZerosC() CodaClassZeros(dtor,itor,kize,etor,ekeep,bits)
#define CodaClass_Trans() enum { OBind1_2M(class,bits)=bits_Trans }
#define CodaClass_Root()  enum { OBind1_2M(class,bits)=bits_Root }
#define CodaClassTransC() CodaClassZeros(dtor,itor,kize,etor,ekeep); CodaClass_Trans()

	#define ei else if
	#define $(sig,...) OBind1_2M(class,sig)(class self,##__VA_ARGS__)
	#define _$(sig,...) OBind1_2M(class,sig)(self,##__VA_ARGS__)
	#define _$2(sig,...) OBind1_2M(class,sig)(self,__VA_ARGS__)
	#define _ self->

	#define setO(var,obj) ({ __auto_type _vp=(&var);__auto_type _obj=obj;keepO(_obj);freeO(*_vp);*_vp=_obj; })
	#define setO_take(var,obj) ({ __auto_type _vp=(&var); freeO(*_vp); *_vp=obj; })

	#define getter$(type,var,body) type $(var) { return(body); }
	#define setter$(type,var,body) void $(OBind1_2(set,var),type value) { body; }
	#define self$  ({ class ooo=spindleO(self,&OBind1_2M(Isa,class)); assert(ooo); ooo; })
	#define property$(type,var,body1,body2) getter$(type,var,body1); setter$(type,var,body2)

	#define property$_(type,var) property$(type,var,self->var ,self->var=value)
	#define property$$(type,var) property$(type,var,self$->var,self$->var=value)
	#define propertyO_(type,var) property$(type,var,self->var ,setO(self->var,value))
	#define propertyO$(type,var) property$(type,var,self$->var,setO(self$->var,value))

	#define CodaStatic(glob,file,intvar) glob; intvar=OBind1_2(glob,length)

	#define Array_add(aaa,ooo)  Array_addObject(aaa,ooo)
	#define Array_take(aaa,ooo) Array_takeObject(aaa,ooo)
	#define Dict Dictionary
	#define Dict_set Dictionary_setKey
	#define Dict_take Dictionary_takeKey
	#define Os_(str) "\0\0" str
	#define Os(str)			((Os_(str))+2)
CodaClassDef(CArray,struct Array_,Root);
	extern ConstChar sig_addObject;
	extern ConstChar sig_subInt;
	extern ConstChar sig_removeObject;
	extern ConstChar sig_removeLast;
	extern ConstChar sig_removeAll;
	extern ConstChar sig_ToDelimiter;
	extern ConstChar sig_ToStringSub;
	extern ConstChar sig_NewBlock;
CodaClassDef(Array,struct Array_,Root);
	extern ConstChar sig_ToString;
	extern ConstChar sig_FromString;
CodaClassDef(Char,char,Root);
CodaClassDef(ConstChar,char,Char);
	extern ConstChar sig_setKey;
	extern ConstChar sig_subKey;
	extern ConstChar sig_removeKey;
	extern ConstChar sig_AllKeys;
	extern ConstChar sig_xmlTag;
CodaClassDef(Dictionary,struct Dictionary_,Root);
CodaClassDef(CDictionary,struct Dictionary_,Root);
CodaClassDef(Keyword,struct Keyword_,Root);
CodaClassDef(Pointer,pointer,Root);
	extern ConstChar sig_get_count;
	extern ConstChar sig_Info;
	extern ConstChar sig_set_name;
	extern ConstChar sig_get_name;
CodaClassDef(Root,void,0);
CodaClassDef(Void,void,0);
	typedef Obj  OSig(addObject)(Obj array,Obj obj) ;
	typedef Obj  OSig(subInt)(Obj array,int ix) ;
	typedef int  OSig(removeObject)(Obj array,Obj obj) ;
	typedef void OSig(removeLast)(Obj array) ;
	typedef void OSig(removeAll)(Obj array) ;
	typedef Char OSig(ToDelimiter)(Obj container,int index) ;
	typedef Char OSig(ToStringSub)(Obj container,int index) ;
	typedef Obj  OSig(NewBlock)(Obj proto,int count,pointer data) ;
	typedef Char OSig(ToString)(Obj obj);
	typedef Obj  OSig(FromString)(Obj proto,char *str);
	typedef void OSig(setKey)(Obj dict,char *key,Obj obj) ;
	typedef Obj  OSig(subKey)(Obj dict,char *key) ;
	typedef bool OSig(removeKey)(Obj dict,char *key) ;
	typedef Pointer OSig(AllKeys)(Obj dict) ;
	typedef Char OSig(xmlTag)(Obj obj);
	typedef int   OSig(get_count)(Obj obj) ;
	typedef Char  OSig(Info)(Obj obj) ;
	typedef void  OSig(set_name)(Obj obj,char* name) ;
	typedef Char  OSig(get_name)(Obj obj) ;
	#define cleanC const $CLEANUP(clean_C)
	#define Array_sub Array_subInt
	#define CodaCOSize 64
	#define isa_(obj,CLASS) OisaClass(obj,OClass(CLASS))
	#define Dict_sub Dictionary_subKey
	#define Dictionary_name Dictionary_get_name
	#define Dictionary_keywordFromWord(charptr)   ((Keyword)(charptr - offsetof(struct Keyword_,word) ))
	struct Keyword_ { Obj item; char word[0]; };
	#define cleanO 				const $CLEANUP(Memory_clean)
	#define	newO(class)			Memory_newO(OClass(class),1)
	#define	newOC(class,cnt)	Memory_newO(OClass(class),cnt)
	#define alocO(size)			Memory_alocO(size)
	#define keepO               Memory_keep
	#define freeO               Memory_free
	#define countO(obj)			Memory_count(obj)
	#define sizeO(obj)			Memory_size(obj)
	#define classO(obj)			Memory_class(obj)
	#define kindO(obj)			Memory_kind(obj)
	#define	superO(obj)			Memory_superObject(obj)
	#define Msg_(...) Msg_Object(Char_F(__VA_ARGS__),0)
	#define Log_(...) Msg_Object(Char_F(__VA_ARGS__),1)
	#define Quit_(cs,...) Die_Object(Char_F(cs,##__VA_ARGS__),0)
	#define Die_(cs,...) Die_Object(Char_F("Die:%s: " cs,__func__,##__VA_ARGS__),1)
	#define Pointer_count Root_get_count
	#define normalO(ooo) Memory_normalize(ooo)
	#define spindleO(ooo,ccc) Memory_spindle(ooo,ccc)
	#define O_Responds(obj,sig) ORespond3(obj,sig,0,1,0)
	#define OResponds(obj,SEL)  ORespond3(obj,sig_##SEL,0,1,0)
	#define obj_(SEL,obj,...) \
		({ Obj ooo=0; void *fun=ORespond3(obj,sig_##SEL,&ooo,0,0); ((SEL)fun)(ooo,##__VA_ARGS__); })

	#define OHasSuper(obj,SEL)  ORespond3(obj,sig_##SEL,0,1,&OBind1_2M(Isa,class))
	#define super_(SEL,obj,...) \
		({ Obj ooo=0; void *fun=ORespond3(obj,sig_##SEL,&ooo,0,&OBind1_2M(Isa,class)); \
		((SEL)fun)(ooo,##__VA_ARGS__); })

	#define $boot1(sigy) static $CONSTRUCTOR void $class(__LINE__)(void) { CodaSig(sigy); }

	#define method$(type,sig,...) \
		static type $(sig,##__VA_ARGS__); \
		$boot1(sig); \
		static type $(sig,##__VA_ARGS__)

	#define method$def(type,sig,...) \
		static type $(sig,##__VA_ARGS__)

	#define SubClassOk() static_assert(sizeat(class)%8==0,"subclass object wrong size?")
void clean_C(const void *vp);
pointer alocC(int4 size);
void freeC(pointer address);
void alocC_accountForMalloc(pointer address);
huge alocC_count(bool allocation,bool bytes);
void alocC_reset(void);
void alocC_counting(bool enable);
extern double Array_FACTOR;
void Array_removeBlock(Array self,int dix,int count);
void Array_takeBlock(Array self,int index,int count,pointer block);
int Array_get_count(Array self);
#define Array_count Array_get_count
Obj Array_subInt(Array self,int ix);
pointer Array_rawAddress(Array self);
Array Array_NewBlock(Array proto,int count,pointer block);
void Array_insertBlock(Array self,int index,int count,pointer block);
Obj Array_insertAt(Array self,int index,Obj obj);
void Array_removeAt(Array self,int dix);
Obj Array_addObject(Array self,Obj obj);
void Array_takeObject(Array self,Obj $CONSUMED obj);
void Array_removeLast(Array self);
Obj Array_replaceAt(Array self,int ix,Obj obj);
int Array_removeObject(Array self,Obj object);
void Array_removeAll(Array self);
void Array_set_name(Array self,char* name);
Char Array_get_name(Array self);
#define Array_name Array_get_name
extern ConstChar Os_Set;
void Array_toaSet(Array self);
bool Array_isaSet(Array self);
Char Array_Join(Array array,char *delimiter);
Char Char_Value(const char *string);
Char $FORMAT12 Char_F(char *cs,...);
Char Char_NewBlock(Char self,int count,pointer address);
CDictionary Class_dictionary(void);
void Class_register(Obj obj);
Char CMux_Diag(pointer cmux,char *name);
Char Dictionary_Diag(Dictionary dict);
Char Array_Diag(Array array);
bool OisaClass(Obj obj,Obj classobj);
int Dictionary_get_count(Dictionary self);
#define Dictionary_count Dictionary_get_count
void Dictionary_setKey(Dictionary self,char *key,Obj obj);
bool Dictionary_removeKey(Dictionary self,char *key);
int Dictionary_removeObject(Dictionary self,Obj obj);
Obj Dictionary_subKey(Dictionary self,char *key);
Pointer Dictionary_AllKeys(Dictionary self);
void Dictionary_set_name(Dictionary self,char* name);
Char Dictionary_get_name(Dictionary self);
#define Dictionary_name Dictionary_get_name
void Dictionary_takeKey(Dictionary self,char *key,Obj $CONSUMED obj);
void Dictionary_auto(Dictionary dict);
int4 Dictionary_autoDisable(Dictionary dict,bool disable);
void Dictionary_hash_default(Dictionary dict);
void Dictionary_hash(Dictionary dict,int4 (*hasher)(const char *string));
void Dictionary_hash_build(Dictionary dict);
bool DictHash_keyobj(Dictionary dict,char *key,pointer obj);
bool DictHash_keydel(Dictionary dict,char *key);
Obj DictHash_obj4key(Dictionary dict,char *key);
Keyword Dictionary_scan(Dictionary dict);
Keyword Dictionary_next(Keyword element);
Keyword Dictionary_keywordSub(Dictionary dict,int index);
void Global_objectAddress(pointer address);
void Global_objectsZero(void);
Keyword Keyword_Value(char *word,Obj item);
Keyword Keyword_TakeItem(char *word,Obj item);
void Keyword_set_item(Keyword self,Obj object);
Obj Keyword_get_item(Keyword self);
#define Keyword_item Keyword_get_item
Obj Memory_alocO(int size);
extern _Atomic huge Object_Counter;
Obj Memory_aloc(int size,CodaCLASS *clas,int extra);
extern bool Memory_SecurityOn;
void Memory_free(Obj obj);
Obj Memory_keep(Obj obj);
void Memory_clean(const void *vp);
CodaCLASS* Memory_class(Obj obj);
DTOR Memory_etor(Obj obj);
DTOR Memory_ekeep(Obj obj);
char* Memory_kind(Obj obj);
int4 Memory_size(Obj obj);
int4 Memory_count(Obj obj);
CodaCLASS* Memory_reclass(Obj obj,CodaCLASS *clas);
int Memory_align(int pow2);
Obj Memory_newO(Obj obj,int nel);
struct CodaStructMeta_ *Memory_metanext(struct CodaStructMeta_ *meta);
CodaCLASS* baseClass(CodaCLASS* clas);
Obj Memory_objectToClass(Obj obj,CodaCLASS* newClass);
Obj Memory_superObject(Obj obj);
extern void (*Msg_hook)(Char msg,int log);
void Msg_Object(Char $CONSUMED msg,int log);
extern void (*Die_hook)(Char msg,int trace);
Obj $NORETURN Die_Object(Char $CONSUMED msg,int trace);
Pointer Pointer_Value(pointer value);
Pointer Pointer_NewBlock(Pointer self,int count,pointer block);
int Pointer_length(pointer address);
extern Char (*Root_Diag)(Obj self);
int Root_get_count(Root self);
#define Root_count Root_get_count
CDictionary Signature_dictionary(void);
void Signature_register(Char sig);
Char Signature_fromString(char *str);
void codac_sig(pointer cmux,char *key,pointer data);
Obj Memory_normalize(Obj obj);
Obj Memory_spindle(Obj obj,CodaCLASS* newClass);
void *ORespond3(Obj obj,char *sel,Obj *spindle,int noDie,CodaCLASS *superOf);
Char ToContainer(Obj container);
CDictionary Version_dictionary(void);
void Version_register(char *key,char *text);
void codac_versions(void);
