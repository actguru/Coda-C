/* www.coda-c.com  coda-h.c

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
	#include "./coda-c.h"
	#define defer_call(fun,ptr) $CLEANUP(defer_call_cleanup) \
		pointer OBind1_2M(defer_,__LINE__)[2]={fun,ptr}; \
		defer_NOP(OBind1_2M(defer_,__LINE__))
	#define cc_inString(c,str) ((-1)!=cc_pos(c,str))
	#define cs_inString(key,str) ((-1)!=cs_pos(key,str))
	#define file_lsType(path) file_typeCode(path,1)
	#define file_type(path)   file_typeCode(path,0)
	#define file_putc(os,cc)  fputc(cc,os)
	#define file_read(is,buf,nel)  fread( buf,1,nel,is)
	#define file_write(os,buf,nel) fwrite(buf,1,nel,os)
	#define Error_F(...) OErrorSet(Char_F(__VA_ARGS__))
	#define OAbort(...) return(Error_F(__VA_ARGS__))
	#include <errno.h>
	#define MXTOK	20
	typedef	struct	{ char name[128]; char pre[128]; char post[128]; } TOKEN;
 #define printDefs(...) do { FILE *os_=defsos(); if (!os_) return(0); fprintf(os_,__VA_ARGS__); } while(0)
 #define printPriv(...) do { FILE *os_=privos(); if (!os_) return(0); fprintf(os_,__VA_ARGS__); } while(0)
 #define printSsrc(...) do { FILE *os_=ssrcos(); if (!os_) return(0); fprintf(os_,__VA_ARGS__); } while(0)
	#define Flimsy __attribute__  ((weak))
	#define returnOK	return(okString)
int chpospar(char c,char *a);
extern int nclass;
extern char defs_temp[];
extern char defs_file[128];
extern char priv_temp[];
extern char priv_file[128];
extern char ssrc_temp[128];
extern char ssrc_file[128];
extern int verbose;
extern int uniHeader;
int goodfile(char *file);
Obj codaDefine(Array files);
extern int define2_privflag;
Obj define2(char *a,char *file,int lno);
Obj define1(char *file);
Obj define_OAka(char *a,char *file,int lno,char *org);
Obj define_AkaS(char *a,char *file,int lno,char *org);
Obj CodaClass2(char *a,char *file,int lno,char *org);
Obj CodaClass3(char *a,char *file,int lno,char *org);
Obj CodaClassX(char *a,char *file,int lno,char *org,bool privdefsXXX);
void setGenOs(Char dgroup);
int checkGroup(char *var);
int isGroupCc(int cc,int first);
int closeAll(void);
int close1(FILE *os,char *temp,char *file);
int installText(char *temp,char *file);
void whiteAtEnd(char *file,int lno,char *a);
void longestLine(char *file,int lno,char *a);
void longestShow(void);
void longest_reset(void);
extern Char theClass;
bool define_class(const char *a);
void header_lines(void);
void header_pass(Array list,char *extent);
void header_check(char *file);
void Array_sort(Array array,void *IcmpfunVVC,void *context);
Array Char_Split(char* string,char* delimiter);
char* cs_parse(char *b,char *a,char c);
int cs_rmc(char *a,char c);
void cs_rightTrim(char *str);
void cs_leftTrim(char *str);
void cs_trim(char *str);
void defer_call_cleanup(void *vp);
void defer_NOP(void *vp);
char* file_ext(char *path);
Array file_Folder(char *path);
int file_getCs(FILE *f,char *s,int mx);
int file_rename(char* temp,char *file);
bool cc_isCap(int c);
bool cc_isLow(int c);
bool cc_isAlpha(int c);
bool cc_is09(int c);
bool cc_isWhite(int c);
int cc_toCap(int c);
int cc_toLow(int c);
int cc_pos(char c,const char *str);
int cc_posCap(char c,const char *str);
int cc_revPos(char c,const char *str);
bool cs_prefix(const char *key,const char *str);
bool cs_prefixCap(const char *key,const char *str);
void cs_copy(char *dest,const char *sour);
bool cs_exactCap(const char *str1,const char *str2);
int cs_pos(const char *key,const char *str);
int cs_posCap(const char *key,const char *str);
int4 cs_toInt4(const char *string);
double cs_toReal(const char *string);
huge file_size(char *path);
huge file_time(char *path);
int file_typeCode(char *path,bool reportlinks);
Obj OErrorSet(Obj $CONSUMED obj);
Obj OErrorObject(void);
Char OError(void);
void pointer_sort(pointer base,int nel,void *IfunVVC,void *context);
Obj codaLink(char *str,char *file,int lno,char *org);
Obj codaLinkX(char *str,char *file,int lno,char *org);
extern int quiet;
int main(int argc,char **argv);
void aboutCoda(void);
void makeCodaH(void);
Obj mktok(char *a);
extern TOKEN tok[MXTOK+1];
extern int  ntok;
void noedit(FILE *os,char *file);
extern Array sigArray;
Obj osig(char *str,char *file,int lno,char *ooo);
int isIdent(char *a);
extern FILE *defs_os;
extern FILE *priv_os;
extern FILE *ssrc_os;
FILE *defsos(void);
FILE *privos(void);
FILE *ssrcos(void);
Array outtok_Array(char *file,int lno,char* fun,Char *akaptr);
Obj passthru(char *path,int top);
char *pastpara(char *b,char *a,char c);
Obj getter(char *str,char *file,int lno,char *org);
Obj setter(char *str,char *file,int lno,char *org);
Obj property(char *str,char *file,int lno,char *org);
Obj propfun(char *key,char *str,char *file,int lno,char *org);
Obj proto2(char *path);
int stripcomm(char *a,int incomm,char *file,int lno);
int cs_posSyntax(char *key,char *str);
int cc_posSyntax(char c1,char *str);
int curly_count(char *str);
void Flimsy bogus2(void);
extern ConstChar ks_Done;
Obj proto1(char *a,int must,char *path,int lno);
int hasToken1(char *a);
void autoGetter(FILE *os,char *str,char *comm);
extern Obj okString;
Obj codaStatic(char *str,char *srcfile,int lno,char *org);
Obj printBlob1(Void blob,int asglob,FILE *os);
int file_nameOk(char *name);
void codacVersions(bool copyrights);
Void Void_FromFile(char *path);
void zdtclt16(char *a);
	#include <sys/stat.h>

	#define Assert_ assert
	#define codaMsg Msg_

int chpospar(char c,char *a) {
	char *cp; int deep=0;

	if (c==')')	++deep;

	for(cp=a; *a; ++a)	{
		if (*a=='(') ++deep;
		if (*a==')') --deep;
		if (!deep && *a==c) return(a-cp);
		}
	return(-1);
	}

int nclass=0;
char defs_temp[]="_temp_generated.H-";
char defs_file[128]="coda_generated.H";
char priv_temp[]="_temp_private.H-";
char priv_file[128]="coda_private.H";
char ssrc_temp[128]="_temp_generated.c-";
char ssrc_file[128]="coda_generated.c";
int verbose=0;
int uniHeader=0;

int goodfile(char *file) {
	if (!file_nameOk(file)) return(0);
	char *ext=file_ext(file);
	if (cs_exactCap("c",ext))   return(1);
	if (cs_exactCap("m",ext))   return(1);
	return(0);
	}

Obj codaDefine(Array files) {

	int pass,j,nel=Array_count(files);
	for(pass=0;pass<4;++pass) {
		for(j=0;j<nel;++j) {
			Char file=Array_sub(files,j);
			if (goodfile(file)) {
				Char oks=okString;
				if (pass==0) oks=passthru(file,1);
				ei (pass==1) oks=define1(file);
				ei (pass==2) oks=passthru(file,0);
				ei (pass==3) oks=proto2(file);
				if (!oks) return(0);
				}
			}
		if (pass==1) {
			if (sigArray) {
				printDefs("// | Signatures\n");
				int j,nel=Array_count(sigArray);
				if (!quiet) Msg_("%d signature(s) defined.",nel);
				for(j=0;j<nel;++j) {
					printDefs("%s",(char*) Array_sub(sigArray,j) );
					}
				}
			}
		}
	longestShow();
	returnOK;
	}

	typedef struct StrFun_ {
		char *str;
		Obj (*fun)(char *a,char *file,int lno,char *org);
		int noSemi;
		} StrFun;

	StrFun table[]={
		{ "CodaClass",			CodaClass2,		0 },
		{ "CodaAka", 			define_OAka,	0 },
		{ "CodaAkaC", 			define_OAka,	0 },
		{ "CodaAkaS", 			define_AkaS,	0 },
		{ "CodaStatic", 		codaStatic,		0 },
		{ "CodaLink", 			codaLink,		0 },
		{ "CodaLinkX", 			codaLinkX,		0 },
		{ "OSig", 				osig,			0 },
		{ "OSig$", 				osig,			0 },
		{ "Coda_Class",			CodaClass3,		0 },
		{ "getter$", 			getter,			0 },
		{ "setter$", 			setter,			0 },
		{ "property$", 			property,		0 },
		{ "property$_",			property,		0 },
		{ "property$$",			property,		0 },
		{ "propertyO_",			property,		0 },
		{ "propertyO$",			property,		0 },
		};
	#define tablenel (sizeof(table)/sizeof(table[0]))

int define2_privflag=0;

Obj define2(char *a,char *file,int lno) {
	char bb[1024]; cs_copy(bb,a);
	int x=cs_pos("//",bb);
		define2_privflag=cs_inString("PRIVATE",bb);
		if (x!=EOF) bb[x]=0;
	for(int j=0;j<tablenel;++j) {
		int noSemi=table[j].noSemi;
		char key[64]; sprintf(key,"%s(",table[j].str);
		int p=cs_pos(key,bb);
		if (p && p!=EOF) {
			int quotetrap=0;
			for(int ix=0;ix<p;++ix) if (cc_inString(bb[ix],"'\"/")) quotetrap=1;
			if (quotetrap) continue;
			cs_copy(bb,bb+p);
			}
		if (cs_prefix(key,bb)) {
			int k=cc_pos(';',bb); if (noSemi) k=cs_length(bb);
			if (k!=EOF) { bb[k]=0; cs_trim(bb); int kkk=cc_revPos(')',bb);
				if (kkk!=EOF)	{
					bb[kkk]=0; cs_trim(bb);
					if (!table[j].fun(bb+cs_length(key),file,lno,a)) return(0);
					}
				if (noSemi) continue;
				cs_copy(bb,bb+k+1);
				cs_trim(bb);
				j=(-1);
				}
			}
		}
	returnOK;
	}

Obj define1(char *file) {
	FILE *is=fopen(file,"rb"); if (!is) OAbort("can't read(%s)",file);
	char a[4096]; int incomm=0;
	for(int lno=1;EOF!=file_getCs(is,a,sizeof(a));++lno) {
		whiteAtEnd(file,lno,a);
		incomm=stripcomm(a,incomm,0,lno);
		if (define_class(a)) continue;
		cs_trim(a);
		if (!define2(a,file,lno)) return(0);
		}
	fclose(is);
	freeO(theClass); theClass=0;
	returnOK;
	}

Obj define_OAka(char *a,char *file,int lno,char *org) {
	char b[1024];
	cs_parse(b,a,','); cs_trim(a); cs_trim(b);
	if (!*a || !*b) returnOK;
	if (!theClass)	OAbort("***: Aka w/o CodaClass! %s:%d",file,lno);
	printDefs("		#define %s_%s(obj,...)	%s_%s((Obj)obj,##__VA_ARGS__)\n",theClass,b,a,b);
	returnOK;
	}

Obj define_AkaS(char *a,char *file,int lno,char *org) {
	char b[1024]; cs_parse(b,a,','); cs_trim(a); cs_trim(b); if (!*a || !*b) returnOK;
	if (!theClass)	OAbort("***: AkaS w/o CodaClass! %s:%d",file,lno);
	printDefs("		#define %s_%s(obj,...)	%s_%s(superO(obj),##__VA_ARGS__)\n",theClass,b,a,b);
	returnOK;
	}

	static Obj setupClass(char *name,char *type,bool private,char *args,char *file) {
		FILE *os=(private ? privos() : defsos() ); if (!os) return(0);
		int len=cs_length(type);
		char type2[len+1]; cs_copy(type2,type);
		if (type2[len-1]!='*') Quit_("%s; bad type(%s) CodaClass(%s...)",__func__,type2,name);
		type2[len-1]=0;
		fprintf(os,"CodaClassDef(%s,%s,%s); // Class: %s [%s] %s\n",name,type2,args,name,args,file);
		returnOK;
		}

Obj CodaClass2(char *a,char *file,int lno,char *org) { return CodaClassX(a,file,lno,org,1); }
Obj CodaClass3(char *a,char *file,int lno,char *org) { return CodaClassX(a,file,lno,org,0); }

Obj CodaClassX(char *a,char *file,int lno,char *org,bool privdefsXXX) {

	char name[256]; cs_parse(name,a,','); cs_trim(a); cs_trim(name);
	char base[256]; cs_parse(base,a,','); cs_trim(a); cs_trim(base);
	char supc[256]; cs_parse(supc,a,','); cs_trim(a); cs_trim(supc);
	if (!*base || !*name) returnOK;
	char type[300]; sprintf(type,"%s*",base);

	if (verbose) Msg_("// CodaClass: [%s] aka { %s }; [%s] %s:%d",name,type,a,file,lno);

	if (!theClass || !cs_exact(name,theClass))
		OAbort("CodaClass(%s), class #define error(%s) %s:%d",name,theClass,file,lno);

	char *comment="";
	int pos=cs_pos("//",org);
	if (pos!=EOF) {
		comment=org+pos+2;
		while(cc_isWhite(*comment)) ++comment;
		}

	if (!setupClass(name,type,define2_privflag,supc,comment)) return(0);
	++nclass;
	returnOK;
	}

void setGenOs(Char dgroup) {
	if (!dgroup) dgroup=Os("coda");
	if (!checkGroup(dgroup)) Msg_("group(%s) name rejected for generated 'C'.",dgroup);
	  else	{
		sprintf(ssrc_file,"%s_generated.c",dgroup);
		sprintf(defs_file,"%s_generated.%c",dgroup,(uniHeader?'U':'H'));
		sprintf(priv_file,"coda_private.%c",(uniHeader?'U':'H'));
		}
	}

int checkGroup(char *var) {
	int j,cc;
	for(j=0;0!=(cc=var[j]);++j) {
		if (!isGroupCc(cc,!j)) return(0);
		}
	return(j>0);
	}

int isGroupCc(int cc,int first) {
	if (cc>='a' && cc<='z') return(1);
	if (cc>='A' && cc<='Z') return(1);
	if (first) return(0);
	if (cc>='0' && cc<='9') return(1);
	return(0);
	}

int closeAll() {
	int changes=0;
	changes+=close1(defs_os,defs_temp,defs_file);
	changes+=close1(ssrc_os,ssrc_temp,ssrc_file);
	changes+=close1(priv_os,priv_temp,priv_file);
	return(changes);
	}

	#include <unistd.h>

int close1(FILE *os,char *temp,char *file) {
	int changes=0;
	if (os) {
		fclose(os);
		changes+=installText(temp,file);
		}
	  else	{
		FILE *is1=fopen(file,"rb");
		if (is1) {
			fclose(is1);
			Msg_("Removing obsolete file: %s",file);
			unlink(file);
			++changes;
			}
	  	}
	return(changes);
	}

int installText(char *temp,char *file) {

	FILE *is1=fopen(file,"rb");
	if (is1) {
		FILE *is2=fopen(temp,"rb");
		if (!is2) Die_("can't read: %s ?",temp);
		cleanO Char buf1=newOC(Char,1024);
		cleanO Char buf2=newOC(Char,1024);
		int lno=0,diff=0;
		for(lno=1;EOF!=file_getCs(is1,buf1,1024);++lno) {
			if (EOF==  file_getCs(is2,buf2,1024)) { diff=1; break; }
			if (cs_prefix("// ",buf1) && cs_prefix("// ",buf2) && lno<=6) continue;
			if (!cs_exact(buf1,buf2)) { diff=1; break; }
			}
		if (EOF!=file_getCs(is2,buf2,1024)) diff=1;
		fclose(is2); is2=0;
		fclose(is1); is1=0;
		if (!diff) { unlink(temp); return(0); }
		}

	int err=file_rename(temp,file);
	if (err) Die_("Can't rename (%s) to (%s) [%d]",temp,file,errno);

	return(1);
	}

void whiteAtEnd(char *file,int lno,char *a) {
	int len=cs_length(a);
	if (len>0) {
		int cc=a[len-1];
		if (cc==' ' || cc=='\t') {
			Msg_("White @ EOL:%s:%d",file,lno);
			}
		}
	longestLine(file,lno,a);
	}

static int4 long_Length=0;
static Char long_File=0;
static int4 long_Line=0;

static int cs_tabLength(char *a) {
	int len=0;
	for(int j=0;a[j];++j) {
		if (a[j]=='\t') len+=(4-(len%4));
		  else ++len;
		}
	return(len);
	}

void longestLine(char *file,int lno,char *a) {
	int len=cs_tabLength(a);
	if (len>long_Length) {
		long_Length=len;
		long_Line=lno;
		freeO(long_File); long_File=Char_Value(file);
		}
	}

void longestShow() {
	if (!long_File) return;
	if (long_Length<=112) return;
	Msg_("Longest line: %3d chars?  (%s:%d)",long_Length,long_File,long_Line);
	}

void longest_reset() {
	long_Length=0;
	freeO(long_File); long_File=0;
	long_Line=0;
	}

Char theClass=0;

static bool proto2define(const char *frag) {
	cleanO Char temp=Char_Value(frag);
	cs_trim(temp); if (!cs_prefix("class",temp)) return(0);
	cs_copy(temp,temp+cs_length("class")); if (*temp!=' ' && *temp!='\t') return(0);
	int pos=cs_pos("//",temp); if (pos!=EOF) temp[pos]=0;
	cs_trim(temp);
	if (cc_inString(' ',temp) || cc_inString('\t',temp)) return(0);
	freeO(theClass); theClass=keepO(temp);
	return(1);
	}

static bool proto2undef(const char *frag) {
	cleanO Char temp=Char_Value(frag);
	cs_trim(temp); if (!cs_prefix("class",temp)) return(0);
	cs_copy(temp,temp+cs_length("class")); if (*temp!=' ' && *temp!='\t') return(0);
	freeO(theClass); theClass=0;
	return(1);
	}

bool define_class(const char *a) {
	while(cc_inString(*a," \t")) ++a;
	if (cs_prefix("#define",a)) return proto2define(a+cs_length("#define"));
	if (cs_prefix("#undef",a))  return proto2undef(a+cs_length("#undef"));
	return(0);
	}

void header_lines() {
	cleanO Array list=file_Folder(".");
	header_pass(list,"h");

	longest_reset();
	codaMsg("    --- generated headers ---");
	header_pass(list,"H");
	}

void header_pass(Array list,char *extent) {
	int nel=Array_count(list);
	for(int j=0;j<nel;++j) {
		Char ent=Array_sub(list,j);
		if (*ent=='.' || *ent=='_') continue;
		char *ext=file_ext(ent);
		if (cs_exact(extent,ext)) header_check(ent);
		}
	longestShow();
	}

void header_check(char *file) {
	Msg_("Check: %s",file);
	FILE *is=fopen(file,"rb"); if (!is) Die_("Can't read: %s",file);
	defer_call(fclose,is);
	char a[4096];
	for(int lno=1;EOF!=file_getCs(is,a,sizeof(a));++lno) {
		whiteAtEnd(file,lno,a);
		}
	}

void Array_sort(Array array,void *IcmpfunVVC,void *context) {
	int nel=Array_count(array);
	if (nel>1) pointer_sort(Array_rawAddress(array),nel,IcmpfunVVC,context);
	}

Array Char_Split(char* string,char* delimiter) {
	Array array=newO(Array);
	while(1) {
		int p=cs_pos(delimiter,string); if (p==EOF) break;
		Char hunk=Char_F("%.*s",p,string);
		Array_takeObject(array,hunk);
		string+=(p+cs_length(delimiter));
		}
	Array_takeObject(array,Char_Value(string));
	return(array);
	}

char* cs_parse(char *b,char *a,char c) {
	int i; *b=0; if (!a || !*a) return(0);
	if ((i=cc_pos(c,a))<0)  { cs_copy(b,a); *a=0; }
	  else  				{ cs_blockCopy(b,a,i); b[i]=0; cs_copy(a,a+i+1); }
	return(b);
	}

int cs_rmc(char *a,char c) {
	char *d; int count=0;
	for(d=a;*a;++a) if (*a!=c) *d++ = *a; else ++count;
	*d=0;
	return(count);
	}

void cs_rightTrim(char *str) {
	for(int i=cs_length(str)-1; i>=0 && cc_isWhite(str[i]); --i) str[i]=0;
	}

void cs_leftTrim(char *str) {
	int i; for(i=0;cc_isWhite(str[i]);++i) ;
	if (i) cs_copy(str,str+i);
	}

void cs_trim(char *str) {
	cs_leftTrim(str);
	cs_rightTrim(str);
	}

void defer_call_cleanup(void *vp) {
	if (vp) {
		Pointer self=(Pointer)vp;
		void (*fun)(pointer)=self[0];
		if (fun) fun(self[1]);
		}
	}

void defer_NOP(void *vp) { }

char* file_ext(char *path) {
	int dot=cc_revPos('.',path); if (dot==EOF) return("");
	int slash=cc_revPos('/',path); if (slash>dot) return("");
	return(path+dot+1);
	}

	#include <dirent.h>

Array file_Folder(char *path) {
	DIR *dir1=opendir(path); if (!dir1) return(0);
	Array array=newO(Array);
	Array_set_name(array,path);
	struct dirent *dirfile;
	while(0!=(dirfile=readdir(dir1))) {
		char *file=dirfile->d_name;
		if (file[0]=='.' && (!file[1] || (file[1]=='.' && !file[2]))) continue;
		Array_takeObject(array,Char_Value(file));
		}
	closedir(dir1);
	return array;
	}

int file_getCs(FILE *f,char *s,int mx) {
	int c=0,i;
	for(i=0;--mx>0 && (c=fgetc(f))!=EOF && c!='\n';++i) s[i]=c;
	s[i]=0;
	return( (c==EOF && i==0) ? EOF : i );
	}

	#if WIN32
		#include <windows.h>
	#endif

int file_rename(char* temp,char *file) {
  #if !WIN32
	return rename(temp,file);
  #else
	return (!MoveFileEx(temp,file,MOVEFILE_REPLACE_EXISTING));
  #endif
  }

bool cc_isCap(int c)   { return( c>='A' && c<= 'Z' ); }
bool cc_isLow(int c)   { return( c>='a' && c<= 'z' ); }
bool cc_isAlpha(int c) { return( cc_isLow(c) || cc_isCap(c) ); }
bool cc_is09(int c)    { return( c>='0' && c<= '9' ); }
bool cc_isWhite(int c) { return cc_inString(c," \t\n\r\f"); }

int cc_toCap(int c)   { return( c>='a' && c<='z' ? c-32 : c); }
int cc_toLow(int c)   { return( c>='A' && c<='Z' ? c+32 : c); }

int cc_pos(char c,const char *str) {
	for(const char *cp=str;*cp;++cp) if (*cp==c) return(cp-str);
	return(EOF);
	}

int cc_posCap(char c,const char *str)  {
	c=cc_toCap(c);
	for(const char *cp=str;*cp;++cp) if (cc_toCap(*cp)==c) return(cp-str);
	return(EOF);
	}

int cc_revPos(char c,const char *str) {
	const char *cp=str+cs_length(str);
	while(--cp>=str) if (*cp==c) break;
	return(cp-str);
	}

bool cs_prefix(const char *key,const char *str) {
	while(*key) if (*key++ != *str++)	return(0);
	return(1);
	}

bool cs_prefixCap(const char *key,const char *str) {
	while(*key) if (cc_toCap(*key++)!=cc_toCap(*str++))	return(0);
	return(1);
	}

void cs_copy(char *dest,const char *sour) {
	int cc; while(1) { cc=(*sour++); if (!cc) break; (*dest++)=cc; }
	*dest=0;
	}

bool cs_exactCap(const char *str1,const char *str2) {
	while(*str1) if (cc_toCap(*str1++)!=cc_toCap(*str2++))	return(0);
	return(*str2==0);
	}

int cs_pos(const char *key,const char *str) {
	int pos=0,j,c; if (!*key) return(EOF);
	c= *key++; if (!*key) return cc_pos(c,str);
	while(1) {
		if (!(j=cc_pos(c,str)+1)) return(EOF);
		str+=j; pos+=j;
		if (cs_prefix(key,str)) return(--pos);
		}
	}

int cs_posCap(const char *key,const char *str) {
	int pos=0,j,cc; if (!*key) return(EOF);
	cc= *key++; if (!*key) return cc_posCap(cc,str);
	while(1) {
		if (!(j=cc_posCap(cc,str)+1)) return(EOF);
		str+=j; pos+=j;
		if (cs_prefixCap(key,str)) return(--pos);
		}
	}

int4 cs_toInt4(const char *string) {
	return strtol(string,0,10);
	}

double cs_toReal(const char *string) {
	return strtod(string,0);
	}

huge file_size(char *path) {
	struct stat a; if (stat(path,&a)) return(EOF);
	return(a.st_size);
	}

huge file_time(char *path) {
	struct stat a; if (stat(path,&a)) return(EOF);
	return(a.st_mtime);
	}

	#if WIN32
		#define lstat stat
		#define S_IFLNK  0xC300
		#define S_IFSOCK 0xC400
	#endif

int file_typeCode(char *path,bool reportlinks) {
	struct stat a;
	if (reportlinks) { if (lstat(path,&a)) return('?'); }
	  else           { if (stat(path,&a))  return('?'); }
	switch(a.st_mode & S_IFMT)	{
		case S_IFREG:	return('-');
		case S_IFDIR:	return('d');
		case S_IFCHR:	return('c');
		case S_IFBLK:	return('b');
		case S_IFIFO:	return('p');
		case S_IFLNK:	return('l');
		case S_IFSOCK:	return('s');
		default: 		return('u');
		}
	}

	static __thread Obj lastError=0;

Obj OErrorSet(Obj $CONSUMED obj) {
	freeO(lastError);
	lastError=obj;
	return(0);
	}

Obj OErrorObject() {
	return(lastError);
	}

Char OError() {
	if (!lastError) return(0);
	if (isa_(lastError,Char)) return(lastError);
	if (isa_(lastError,Dictionary)) {
		Char desc=Dict_sub(lastError,"description");
		if (isa_(desc,Char)) return(desc);
		return(Os("Error Object--Dictionary w/o 'description'"));
		}
	return Os("Error Object--unknown.");
	}

	typedef struct { void *context; int (*fun)(const void *aa, const void *bb, void *context); } ZsortData;

#if LINUX
	static int zsortSwap(const void *aa, const void *bb,void *context) {
		ZsortData *data=(ZsortData*)context;
		return data->fun(*((void **)aa),*((void **)bb),data->context);
		}
#else
	static int zsortSwap(void *context, const void *aa, const void *bb) {
		ZsortData *data=(ZsortData*)context;
		return data->fun(*((void **)aa),*((void **)bb),data->context);
		}
#endif

void pointer_sort(pointer base,int nel,void *IfunVVC,void *context) {
	ZsortData tmp; tmp.context=context; tmp.fun=IfunVVC;
	#if LINUX
		qsort_r(base, nel, sizeof(void *), zsortSwap, &tmp);
	#elif WIN32
		qsort_s(base, nel, sizeof(void *), zsortSwap, &tmp);
	#elif APPLE
		qsort_r(base, nel, sizeof(void *), &tmp, zsortSwap);
	#else
		#error "pointer_sort: unknown operating system";
	#endif
	}

Obj codaLink(char *str,char *file,int lno,char *org) {
	cs_trim(str);
	printSsrc(" void* CodaLink_%s=%s; // %s\n",str,str,file);
	returnOK;
	}

Obj codaLinkX(char *str,char *file,int lno,char *org) {
	cs_trim(str);
	printSsrc("	extern void %s();\n",str);
	printSsrc(" void* CodaLink_%s=%s; // %s\n",str,str,file);
	returnOK;
	}

	Char codaFile=Os("coda.h");
int quiet=0;

int main(int argc,char **argv) {
	int force=0,newheader=0;

	for(int j=1;j<argc;++j)	{
		Char arg=argv[j];
		if (cs_exactCap("-h",arg)) { aboutCoda(); return(0); }
		ei (cs_exact("-f",arg))  force=1;
		ei (cs_exact("-n",arg))  newheader=1;
		ei (cs_exact("-v",arg))  verbose=1;
		ei (cs_exact("-q",arg))  quiet=1;
		ei (cs_exact("-l",arg)) { header_lines(); return(0); }
		else Quit_("coda: unknown argument(%s), use: coda -help",arg);
		}

	if (!quiet) Msg_("Coda-C Header Generator # -h for help");

	if (file_type(codaFile)!='-' && !force && !newheader)
		Quit_("This is not a coda source folder.\n"
			  "Create file '%s' or use option -f or -n to generate headers",codaFile);

	if (newheader) makeCodaH();
	setGenOs(0);

	cleanO Array files=file_Folder("."); Assert_(files);
	Array_sort(files,strcmp,0);
	Char oks=codaDefine(files);
	if (!oks) Die_("Fatal error: %s *Stop*",OError());
	if (!defs_os && !priv_os)
			 Msg_("No *.c source files located containing #defines, globals, or visible functions.");
	ei (nclass && !quiet) Msg_("%d object classes defined in %s.",nclass,defs_file);
	if (defs_os && !quiet) Msg_("Proto types as: %s.",defs_file);
	if (priv_os && !quiet) Msg_("Proto types as: %s.",priv_file);
	if (ssrc_os && !quiet) Msg_("Generated 'C'.: %s.",ssrc_file);

	if (closeAll()) {
		Msg_("Updated");
		}
	  else	{
		Msg_("No changes.");
		}

	return(0);
	}

void aboutCoda() {
	printf("\r\n");
	printf("codah: Coda-C Header Generator  Version 6.1  www.coda-c.com\n");
	printf("format: codah [options]  (The default is to generate headers)\n");
	printf("        -h  options for coda-c\n");
	printf("        -f  force header generation without a \"coda.h\" file\n");
	printf("        -n  create a \"coda.h\" file and generate headers\n");
	printf("        -v  verbose: report additional information while processing\n");
	printf("        -q  Quiet--less messages.\n");
	printf("        -l  Check header lines, length/white@end (*.h, *.H)\n");
	codacVersions(0) ;
	}

void makeCodaH() {
	static char file[]="coda.h";

	int type=file_lsType(file);
	if (type!='?') return;

	FILE *os=fopen(file,"wb"); if (!os) return;

	fprintf(os,"#pragma once\n");
	fprintf(os,"\n");
	fprintf(os,"	#include <coda-c.h>\n");
	fprintf(os,"	#include \"coda_generated.H\"\n");
	fprintf(os,"\n");

	fclose(os);
	Msg_("Creating: %s",file);
	}

Obj mktok(char *a) {
	char b[128];

	cs_trim(a);
	ntok=0;
	while(pastpara(b,a,','))	{
		cs_trim(b); cs_trim(a);
		if (ntok>=MXTOK)		OAbort("too many tokens! %d",ntok);
		if (cs_length(b)>120)	OAbort("token too long (%s)",b);
		cs_copy(tok[ntok].name,b);
		*tok[ntok].pre=0;
		*tok[ntok].post=0;
		++ntok;
		}
	returnOK;
	}

TOKEN tok[MXTOK+1]={};
int  ntok=0;

void noedit(FILE *os,char *file) {
	char dates[64]; cs_copy(dates,"<DATE/TIME>");
	zdtclt16(dates);

	fprintf(os,"// +------------------------------------------------------------+\n");
	fprintf(os,"// | Coda created file: %-20.20s @ %-16.16s |\n",file,dates);
	fprintf(os,"// |                                                            |\n");
	fprintf(os,"// | Created with the Coda-C Header Generator                   |\n");
	fprintf(os,"// +------------------------------------------------------------+\n");
	}

	static Dictionary sigs=0;
Array sigArray=0;

Obj osig(char *str,char *file,int lno,char *ooo) {
	int once=cs_pos("OSig",ooo);
	if (once==EOF) Quit_("%s; error 'OSig' not found?",__func__);
	int twice=cs_pos("OSig",ooo+once+4);
	if (twice!=EOF) Quit_("%s; Only 1 'OSig' per line! |%s| ?",__func__,ooo);

	cleanO Char org=Char_Value(ooo);
	int newpos=cs_pos("OSig$(",org);
	bool newsig=(newpos!=EOF);
	if (newsig) cs_copy(org+newpos+4,org+newpos+5);
	int orglen=cs_length(org);
	char extra[256]="";
	int k=cc_pos(')',str);
	if (!newsig) {
		if (k==EOF) returnOK;
		str[k]=0;
		}
	if (newsig) {
		if (k!=EOF) returnOK;
		k=cc_pos(',',str);
		if (k==EOF) cs_copy(extra,")(Obj);");
		if (k!=EOF) {
			snprintf(extra,sizeof(extra),")(Obj,%s);",str+k+1);
			str[k]=0;
			}
		orglen=newpos+cs_length("OSig(")+( k!=EOF ? k : cs_length(str) );
		}
	cs_trim(str);
	if (!isIdent(str)) returnOK;

	if (!sigs) sigs=newO(Dictionary);
	if (Dict_sub(sigs,str)) {
		codaMsg("*** Ignoring Duplicate Signature: %s ***  %s:%d",str,file,lno);
		returnOK;
		}
	Dict_set(sigs,str,Os("dup"));

	int semi=cc_pos(';',org);
	if (!newsig && semi!=EOF) {
		orglen=semi+1;
		}

	char *comment="";
	int pos=cs_pos("//",org);
	if (pos!=EOF) {
		comment=org+pos+2;
		while(cc_isWhite(*comment)) ++comment;
		}

	printDefs("	extern ConstChar sig_%s; // Signature Key // %s\n",str,comment);

	if (!sigArray) sigArray=newO(Array);
	Char siggy=(!newsig ? Char_F("	%.*s // Call Signature // %s\n",orglen,org,comment)
						: Char_F("	%.*s%s // Call Signature // %s\n",orglen,org,extra,comment) );
	Array_take(sigArray,siggy);

	if (verbose) codaMsg("OSig(%s)",str);
	if (EOF==cs_pos("typedef",ooo)) Msg_("Warning OSig(%s) missing typedef",str);
	returnOK;
	}

int isIdent(char *a) {
	int j,len=cs_length(a);
	for(j=0;j<len;++j) {
		int cc=a[j];
		if (cc_isAlpha(cc) || cc_is09(cc) || cc=='_') ;
		  else return(0);
		if (j==0 && cc_is09(cc)) return(0);
		}
	return(j>0);
	}

FILE *defs_os=0;
FILE *priv_os=0;
FILE *ssrc_os=0;

FILE *defsos() {
	if (!defs_os) {
		defs_os=fopen(defs_temp,"wb"); if (!defs_os) OAbort("can't write: %s",defs_temp);
		noedit(defs_os,defs_file);
		}
	return(defs_os);
	}

FILE *privos() {
	if (!priv_os) {
		priv_os=fopen(priv_temp,"wb"); if (!priv_os) OAbort("can't write: %s",priv_temp);
		noedit(priv_os,priv_file);
		}
	return(priv_os);
	}

FILE *ssrcos() {
	if (!ssrc_os) {
		ssrc_os=fopen(ssrc_temp,"wb"); if (!ssrc_os) OAbort("can't write: %s",ssrc_temp);
		noedit(ssrc_os,ssrc_file);
		}
	return(ssrc_os);
	}

Array outtok_Array(char *file,int lno,char* fun,Char *akaptr) {
	cleanO Array list=newO(Array);
	for(int j=0;j<ntok;++j)	{
		char *name=tok[j].name;
		Char item=0;
		if (!*tok[j].pre)	{
			if (cs_exact("va_etc",name)) item=Os("...");
			ei (cs_exact("...",name))	 item=Os("...");
			ei (cc_inString(' ',name))	 item=Char_F("%s%s",name,tok[j].post);
			ei (cs_exact("void",name)) {
				if (ntok==1) item=Os("void");
				else OAbort("%s:%d: token(void) only allowed as a single argument.",file,lno);
				}
			else {
				int funlen=cs_length(fun);
				char c1=(funlen>=1 ? fun[funlen-1] : 0);
				char c2=(funlen>=2 ? fun[funlen-2] : 0);
				if (theClass && j==0 && c1=='$' && cc_inString(c2," \t") ) {
					if (akaptr) {
						funlen-=2;
						char *tempfun=fun;
						if (funlen==cs_length("class") && cs_prefix("class",fun)) {
							tempfun=theClass; funlen=cs_length(tempfun);
							}
						Char aka=Char_F("%.*s %s_%s",funlen,tempfun,theClass,name);
						*akaptr=aka;
						}
					item=Char_F("%s self",theClass);
					}
				  else {
					OAbort("%s:%d: token(%s) without type?",file,lno,name);
					}
				}
			}
		  else	item=Char_F("%s%s%s", tok[j].pre, name, tok[j].post);
		Array_takeObject(list,item);
		}
	if (!ntok) Array_takeObject(list,Os("void"));
	return keepO(list);
	}

 static char *target(char *a,char *old) ;

Obj passthru(char *path,int top) {
	char *tarsys=0;
	char *key="//|",*keyout="// |";
	if (top) { key="//>"; keyout="// >"; }
	FILE *is=fopen(path,"rb"); if (!is) OAbort("Can't read: %s",path);
	int incomm=0;
	int firsta[2]={1,1};
	char a[4096];
	for(int lno=1;EOF!=file_getCs(is,a,sizeof(a));++lno)	{
		incomm=stripcomm(a,incomm,0,lno);
		bool isPriv=(cs_pos("PRIVATE",a)!=EOF);
		tarsys=target(a,tarsys);
		if (cs_prefix(key,a)) {
			int len=cs_length(a);
			if (len>=5 && cs_exact("\\$", a+len-2)) a[len-1]=0;
			ei (len>=5 && cs_exact("\\\\",a+len-2)) a[len-1]=0;
			FILE *os=(isPriv ? privos() : defsos() ); if (!os) return(0);
			if (firsta[isPriv]) {
				firsta[isPriv]=0;
				fprintf(os,"%s %s\n",keyout,path);
				}
			if (tarsys) fprintf(os,"#if %s\n",tarsys);
						fprintf(os,"%s\n",a+3);
			if (tarsys) fprintf(os,"#endif\n");
			}
		}
	fclose(is);
	returnOK;
	}

 static char *target(char *a,char *old) {
	int  len=cs_length(a);
	int comm=cs_pos("//",a); if (comm==EOF) comm=len;
	int endi=cs_pos("#endif",a); if (endi!=EOF && endi<comm) return(0);
	int elat=cs_pos("#else",a);  if (elat!=EOF && elat<comm) return(0);
	int ifat=cs_pos("#if",a);
	if (ifat==EOF || ifat>comm) ifat=cs_pos("#elif",a);
	if (ifat==EOF || ifat>comm) return(old);
	static char *keys[]={ "!APPLE","!LINUX","!WIN32", "APPLE","LINUX","WIN32", 0};
	char *found=0; int fat=(-99);
	for(int j=0;keys[j];++j) {
		char *key=keys[j];
		int pos=cs_pos(key,a); if (pos==EOF || pos>comm) continue;
		if (pos==fat+1) continue;
		if (found) return(0);
		found=key; fat=pos;
		}
	return(found);
	}

char *pastpara(char *b,char *a,char c) {
	int i;

	*b=0;
	if (!a || !*a) return(0);
	if ((i=chpospar(c,a))<0)  { cs_copy(b,a); 			  *a=0; 			}
	  else  				{ cs_blockCopy(b,a,i); b[i]=0; cs_copy(a,a+i+1); 	}

	return(b);
	}

Obj getter(char *str,char *file,int lno,char *org)   { return propfun("getter",str,file,lno,org); }
Obj setter(char *str,char *file,int lno,char *org)   { return propfun("setter",str,file,lno,org); }
Obj property(char *str,char *file,int lno,char *org) { return propfun("property",str,file,lno,org); }

Obj propfun(char *key,char *str,char *file,int lno,char *org) {
	if (cs_inString("#define",org)) returnOK;

	cleanO Char type=Char_Value(str);
	int pos=cc_pos(',',type); if (pos==EOF) returnOK;
	type[pos]=0; cs_trim(type);

	Char vary=type+pos+1;
	pos=cc_pos(',',vary); if (pos==EOF) pos=cs_length(vary);
	vary[pos]=0; cs_trim(vary);

	if (!theClass)	OAbort("***: %s w/o CodaClass! %s:%d",key,file,lno);

	if (*key=='g' || *key=='p') {
		printDefs("%s %s_%s(%s self);\n",type,theClass,vary,theClass);
		}
	if (*key=='s' || *key=='p') {
		printDefs("void %s_set_%s(%s self,%s %s);\n", theClass,vary, theClass, type,vary);
		}
	returnOK;
	}

	static bool isToken1(char *bb) ;

Obj proto2(char *path) {

	FILE *is=fopen(path,"rb"); if (!is) OAbort("Can't read: %s",path);

	char a[4096]; int curly=0,atimp=0,incomm=0; Char prefix1=0;
	for(int lno=1;EOF!=file_getCs(is,a,sizeof(a));++lno) {
		incomm=stripcomm(a,incomm,path,lno);
		if (cs_prefixCap("@end",a))	{ atimp=0; curly=0; continue; }
		if (cs_prefixCap("@i",a))	{ atimp=1; continue; }
		if (define_class(a)) continue;

		int checkit=(!curly);
		curly+=curly_count(a);
		if (checkit && !atimp && !cs_prefix("__",a)) {
			cleanO Char bb=Char_Value(a);
			cleanO Char temp=(prefix1 ? Char_F("%s %s",prefix1,a) : 0);
			char *string=(prefix1 ? temp : a);
			Obj ret=proto1(string,0,path,lno);
				if (!ret) return(0);
			if (ret!=ks_Done && isToken1(bb)) {
				Char oldstr=prefix1;
				if (!oldstr) prefix1=Char_Value(bb);
				  else {     prefix1=Char_F("%s %s",prefix1,bb); freeO(oldstr); oldstr=0; }
				}
			else { freeO(prefix1); prefix1=0; }
			}
		else { freeO(prefix1); prefix1=0; }
		}
	fclose(is);
	if (curly) codaMsg("*** Curly brace mismatch(%d), %s ***",curly,path);
	freeO(theClass); theClass=0;
	returnOK;
	}

int stripcomm(char *a,int incomm,char *file,int lno) {
	int pos=0;
	if (incomm) {
		pos=cs_pos("*/",a); if (pos==EOF) { *a=0; return(1); }
		cs_copy(a,a+pos+2);
		}
	for(char *cp=a;1;) {
		pos=cs_posSyntax("/*",cp); if (pos==EOF) break;
		int ssc=cs_posSyntax("//",cp);
		if (ssc!=EOF && ssc<pos) {
			if (file) codaMsg("Note: Commented(//) comment(/*) file: %s:%d.",file,lno);
			return(0);
			}
		cp+=pos;
		pos=cs_pos("*/",cp); if (pos==EOF) { *cp=0; return(1); }
		cs_copy(cp,cp+pos+2);
		}
	return(0);
	}

int cs_posSyntax(char *key,char *str) {
	int pos=0,j,c; if (!*key) return(EOF);
	c= *key++; if (!*key) return cc_posSyntax(c,str);
	while(1) {
		j=cc_posSyntax(c,str)+1; if (!j) return(EOF);
		str+=j; pos+=j;
		if (cs_prefix(key,str)) return(--pos);
		}
	}

int cc_posSyntax(char c1,char *str) {
	int quote=0,back=0;
	for(char *cp=str;*cp;++cp) {
		int cc=(*cp);
		if (quote) {
			if (back) back=0;
			ei (cc=='\\') back=1;
			ei (cc==quote) quote=0;
			}
		ei (cc=='\"' || cc=='\'') quote=cc;
		ei (cc==c1) return(cp-str);
		}
	return(EOF);
	}

int curly_count(char *str) {
	int quote=0,back=0,count=0,slash=(-10);
	for(int j=0;str[j];++j) {
		int cc=str[j];
		if (quote) {
			if (back) back=0;
			ei (cc=='\\') back=1;
			ei (cc==quote) quote=0;
			}
		ei (cc=='\"' || cc=='\'') quote=cc;
		ei (cc=='{') ++count;
		ei (cc=='}') --count;
		ei (cc=='/') {
			if (slash==j-1) break;
			slash=j;
			}
		}
	return(count);
	}

static bool isToken1(char *bb) {
	int len=cs_length(bb); if (len>0 && bb[len-1]==',') return(1);

	int cc=(*bb);
	if (cc=='_' || (cc>='A' && cc<='Z') || (cc>='a' && cc<='z')) ;
	  else return(0);

	int pos=cs_posSyntax("//",bb); if (pos!=EOF) bb[pos]=0;
	cs_rightTrim(bb);

	if (EOF!=cc_posSyntax(';',bb)) return(0);
	if (EOF!=cc_posSyntax('(',bb)) return(0);

	if (cs_prefix("__",bb)) return(0);
	return(1);
	}

/* fix #1 -- should not prototype
int bogus1() { return(0); }
*/

void
Flimsy
bogus2()
{
}

void
__attribute__  ((weak))
bogus3()
{
}

ConstChar ks_Done=Os("done");

	static Char firstToken(char *a) ;

Obj proto1(char *a,int must,char *path,int lno) {
	int cc=(*a); if (cc && cc_inString(cc," \t#/")) returnOK;

	int isPriv=(cs_pos("PRIVATE",a)!=EOF);
	int k=cc_pos('=',a);
	int jj=cs_pos("//",a);
	int bb=cc_pos('{',a);
	if (k!=EOF && (jj==EOF || k<jj) && (bb==EOF || k<bb)) {
		char *comm=""; if (jj!=EOF) comm=a+jj;
		a[k]=0;
		if (cs_prefix("CodaClass",a)) returnOK;

		if (!hasToken1(a)) {
			codaMsg("Skipping def[%s] missing type token. %s:%d",a,path,lno);
			returnOK;
			}

		FILE *os=(isPriv?privos():defsos()); if (!os) return(0);

		Char first=firstToken(a);
		if (*first=='e') ;
		ei (*first=='s') ;
		ei (*first=='t') ;
		else {
			fprintf(os,"extern %s;%s%s\n",a,(*comm?" ":""),comm);
			}
		return ks_Done;
		}

	int isMacro=(cs_pos("MACRO",a)!=EOF);
	k=cs_pos("//",a); if (k==EOF) k=cs_length(a);
	int j=cc_pos('(',a);
	if (j==EOF)	{ if (must) OAbort("can't locate proceedure OPEN!(%s)",path); returnOK; }
	if (k<j)	{ if (must) OAbort("proc-open is commented out(%s)",path); returnOK; }

	a[j]=0; char *cp=a+j+1;
	cs_rightTrim(a);
	if (!hasToken1(a)) returnOK;
	j=chpospar(')',cp);
	if (j==EOF)	{ if (must) OAbort("can't locate proceedure CLOSE!(%s)",path); returnOK; }
	cp[j]=0;

	if (!mktok(cp)) return(0);

	char *ap=cp+j+1;
	char *org=ap,*comm="";
	j=cs_pos("//",ap);
	if (j==EOF)	ap="";  else ap+=j+cs_length("//");
	if (cs_pos("__",ap)==EOF) ap="";

	if (!*ap) {
		if (j!=EOF) comm=org+j;
		}
	  else	{
	  	int p=cs_pos("//",ap);
		if (p!=EOF) {
			comm=Char_Value(ap+p);
			ap[p]=0;
			}
	  	}

	FILE *os=(isPriv?privos():defsos()); if (!os) return(0);

	Char first=firstToken(a);
	if (*first=='e') ;
	ei (*first=='s') ;
	ei (*first=='t') ;
	else {
		Char endw=Os("");
		if (*first=='s') endw=Os(" __attribute__((used))");

		Char aka=0; cleanO Array list=outtok_Array(path,lno,a,&aka);
		defer_call(freeO,aka);
		Char newaka=0; if (!aka && theClass && cs_prefix("class",a) && cc_inString(a[5]," \t")) {
			aka=Char_F("%s %s",theClass,a+6); newaka=aka;
			}
		defer_call(freeO,newaka);
		if (!aka && theClass && cs_exact("class",a)) aka=theClass;
		if (list) {
			if (!isMacro) {
				cleanO Char arglist=Array_Join(list,",");
				int nout=0;
				nout+=fprintf(os,"%s(",(aka?aka:a));
				nout+=fprintf(os,"%s",arglist);
				nout+=fprintf(os,")%s%s;",ap,endw);
				if (nout>=4000) Die_("Line too long? nout %d",nout);
				fprintf(os,"%s%s\n",(*comm?" ":""),comm);
				autoGetter(os,(aka?aka:a),comm);
				if (*ap) codaMsg("NOTE: old style comment attribute(%s) %s:%d",ap,path,lno);
				}
			}
		  else codaMsg("Warning: function: %s() %s",a,OError());
		}
	return ks_Done;
	}

int hasToken1(char *a) {
	for(char *cp=a;*cp;++cp) {
		char cc=(*cp);
		if (cc==' ' || cc=='\t') return(1);
		if (cc=='=') return(0);
		}
	return(0);
	}

static Char firstToken(char *a) {
	if (cs_prefix("static",a)) {
		int cc=a[cs_length("static")];
		if (cc==' ' || cc=='\t') return Os("static");
		}
	ei (cs_prefix("extern",a)) {
		int cc=a[cs_length("extern")];
		if (cc==' ' || cc=='\t') return Os("extern");
		}
	ei (cs_prefix("typedef",a)) {
		int cc=a[cs_length("typedef")];
		if (cc==' ' || cc=='\t') return Os("typedef");
		}
	return Os("OTHER");
	}

void autoGetter(FILE *os,char *str,char *comm) {
	int poss=cc_revPos(' ',str);
	int post=cc_revPos('\t',str);
	int pos=(poss>post ? poss : post);
	if (pos==EOF) return;
	str+=pos+1;
	pos=cs_pos("_get_",str); if (pos==EOF) return;
	fprintf(os,"#define %.*s_%s %s //auto-getter%s\n",pos,str,str+pos+5,str,(comm?comm:""));
	}

Obj okString=Os("Ok");

Obj codaStatic(char *str,char *srcfile,int lno,char *org) {

	cleanO Array list=Char_Split(str,",");
	if (Array_count(list)!=2) {
		Msg_("%s; must have 2 args: global_name, file",__func__);
		returnOK;
		}
	Char name=Array_sub(list,0); cs_trim(name);
	Char file=Array_sub(list,1); cs_rmc(file,'\"'); cs_trim(file);

	if (verbose) codaMsg(">>Coda*Static(glob=%s,file=%s,%s_length=LENGTH)",name,file,name);

	Void blob=Void_FromFile(file);
	if (!blob) OAbort("codaStatic: Can't read(%s)",file);
	int size=sizeO(blob); if (size<1) OAbort("empty file(%s)?",file);

	FILE *os=ssrcos(); assert(os);

	printDefs("extern ConstChar %s;  // Static[%d]\n",name,size);
	printDefs("extern int %s_length; // Static[%d]\n",name,size);

	printSsrc(" int %s_length=%d;\n",name,size);
	printSsrc(" ConstChar %s=Os(\n",name);
	printBlob1(blob,1,os);
	printSsrc(");\n\n");
	returnOK;
	}

Obj printBlob1(Void blob,int asglob,FILE *os) {
	Assert_(os);
	int count=sizeO(blob);
	char buf[256],*raw=blob;
	while(count>0) {
		char *cp=buf; int j;
		*cp++ = '\t';
		*cp++ = '"';
		for(j=0;j<count && (cp-buf)<80;++j) {
			int cc=(raw[j]&0xFF);
			if (cc>=32 && cc<127 && cc!='\"' && cc!='\\') *cp++ = cc;
			else cp+=sprintf(cp,"\\%03o",cc);
			}
		*cp++ = '"';
		*cp=0;
		if (asglob) fprintf(os,"%s\n",buf);
		  else      fprintf(os,"%s \\\n",buf);
		raw+=j;
		count-=j;
		}
	returnOK;
	}

int file_nameOk(char *name) {
	int len=cs_length(name); if (len<1) return(0);
	int cc=name[0];
	if (cc=='.' || cc==' ') return(0);
	for(int j=0;j<len;++j) {
		cc=(name[j] & 0xFF);
		if (cc<32) return(0);
		}
	return(1);
	}

void codacVersions(bool copyrights) {
	CDictionary dict=Version_dictionary();
	cleanO Pointer list=Dictionary_AllKeys(dict);
	int nel=Pointer_count(list);
	for(int j=nel-1;j>=0;--j) {
		char *key=list[j];
		char *txt=Dict_sub(dict,key);
		Msg_("Library %s: %s",key,txt);
		if (copyrights) {
			char *cpy=txt+cs_length(txt)+1;
			Msg_("%s",cpy);
			}
		}
	}

#include <time.h>

void zdtclt16(char *a) {
	struct tm *tim; time_t clock;

	clock=time(0L);

	tim=localtime(&clock);
	sprintf(a,"%02d/%02d/%04d %02d:%02d",
		tim->tm_mon+1,
		tim->tm_mday,
		tim->tm_year+1900,
		tim->tm_hour,
		tim->tm_min);
	}

Void Void_FromFile(char *path) {
	int type=file_type(path);
	if (type=='?') OAbort("file does not exist.");
	ei (type!='-') OAbort("file is not regular.");
	huge size=file_size(path); if (size<1) OAbort("file is empty.");
	cleanO Void data=alocO(size);
	FILE *is=fopen(path,"rb"); if (!is) OAbort("can't open/read file.");
	huge got=file_read(is,data,size);
	fclose(is);
	if (got!=size) OAbort("did not read entire file?");
	return(keepO(data));
	}

