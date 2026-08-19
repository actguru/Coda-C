//	Copyright (c) 2026 Stephen M. Jones, All rights reserved.  www.actmac.com

	#include <coda-c.h>

// Standard functions -------------------------------------------------------

	#include <dirent.h>
Array file_Folder(char *path) { // directory entries as Named Array of <Char>. or nil
	DIR *dir1=opendir(path); if (!dir1) return(0); // failed
	Array array=newO(Array); Array_set_name(array,path);
	struct dirent *dirfile;
	while(0!=(dirfile=readdir(dir1)))	{	
		char *file=dirfile->d_name;
		if (cs_exact(".",file) || cs_exact("..",file)) continue;
		cleanO Char name=Char_Value(file);
		Array_addObject(array,name);
		}
	closedir(dir1);
	return array;
	}

	#include <sys/stat.h>
int file_typeCode(char *path,int links) {
	struct stat a;
	if (links)	{ if (lstat(path,&a)) return('?'); }
	  else		{ if (stat(path,&a))  return('?'); }
	switch(a.st_mode & S_IFMT)	{
		case S_IFREG:	return('-'); //	-     Regular file.
		case S_IFDIR:	return('d'); //	d     Directory.
		case S_IFCHR:	return('c'); //	c     Character special file.
		case S_IFBLK:	return('b'); //	b     Block special file.
		case S_IFIFO:	return('p'); //	p     FIFO.
		case S_IFLNK:	return('l'); //	l     Symbolic link.
		case S_IFSOCK:	return('s'); //	s     Socket link.
		default: 		return('u'); // u     unknown entry
		}
	}

int file_getCs(FILE *f,char *s,int mx) { // get s from file stream. Return EOF else length
	int j,c=0;
	for(j=0;--mx>0 && (c=fgetc(f))!=EOF && c!='\n';++j) s[j]=c;
	s[j]=0;
	return( (c==EOF && j==0) ? EOF : j );
	}

int cs_cmpKey(char *key,char *str) { // does str[] start with key[]?
	while(*key) if (*key++ != *str++)	return(0);
	return(1);
	}

int cc_toCap(int c)   { return( c>='a' && c<='z' ? c-32 : c); } // Lowercase to uppercase

int cs_exactCap(char *str1,char *str2) { // exact compare, ignoring case
	while(*str1) if (cc_toCap(*str1++)!=cc_toCap(*str2++))	return(0);
	return(*str2==0);
	}

int cc_pos(char c,char *str)   { // Position of c in str[] or -1
	char *cp; for(cp=str;*str;++str) if (*str==c) return(str-cp);
	return(EOF); // (-1)
	}

int cs_pos(char *key,char *str) { // Position of key[] in str[]
	int pos=0,j,c; if (!*key) return(EOF); // -1
	c= *key++; if (!*key) return cc_pos(c,str);
	while(1) {
		if (!(j=cc_pos(c,str)+1)) return(EOF);
		str+=j; pos+=j;
		if (cs_cmpKey(key,str)) return(--pos);
		}
	}

// ------- program -----

	Array folders=0,words=0,extents=0;

void formatCgrep() {
	printf("------- 'C' source file searcher --------------------------------------\n");
	printf("To search for strings in 'C' source, in the current directory\n");
	printf("and resursively thru subdirectories.\n");
	printf("-----------------------------------------------------------------------\n");
	printf("format: cgrep [-ffolder ...] [.extent ...] word ...\n");
	printf("        .ext, add 'ext' to file extents to search\n");
	printf("        -.word, add '.word' to search list\n");
	printf("        defaults: -f., extents=c,m,cpp,cc,c++,h,hh,hpp\n");
	printf("example: cgrep Array Dictionary; cgrep addObject -f../MyLibrary\n");
	printf("-----------------------------------------------------------------------\n");
	printf("cgrep: version 1.1 (%s)\n",__TIMESTAMP__);
	printf("Copyright (c) 2026, Stephen M. Jones, All rights reserved.\n");
	printf("Websites: www.coda-c.com\n");
	exit(1);
	}

void defaultExtents() {
	static Char defext[]={Os(".c"),Os(".m"),Os(".cpp"),Os(".cc"),Os(".c++"),Os(".h"),Os(".hh"),Os(".hpp"),0};
	for(int j=0;defext[j];++j) Array_addObject(extents,defext[j]);
	}

int extok(char *a) {
	int nel=Array_count(extents);
	for(int j=0;j<nel;++j) {
		Char e=Array_sub(extents,j);
		int la=cs_length(a),le=cs_length(e);
		if (la>le && cs_exactCap(a+la-le,e)) return(1);
		}
	return(0);
	}

void cgrepf(char *file) {
	FILE *is; char a[1024]; int flag,j,lno,nel=Array_count(words);
	if (!(is=fopen(file,"rb")))	return;
	for(lno=1,flag=0;file_getCs(is,a,1000)!=EOF;++lno) { // FUTURE: GetString as object.
		for(j=0;j<nel;++j) {
			char *key=Array_sub(words,j);
			if (cs_pos(key,a)!=EOF)	{
				if (flag==0) { flag=1; printf("%s\n",file); }
				printf("%5d %s\n",lno,a);
				break;
				}
			}
		}
	fclose(is);
	}

void cgrep(Char folder,int level) {
	if (level>10) return; // loopy stopper.
	cleanO Array dir=file_Folder(folder); if (!dir) return;
	int j,nel=Array_count(dir);
	for(j=0;j<nel;++j) { char *file=Array_sub(dir,j);
		if (*file=='.') continue;
		cleanO Char newpath=Char_F("%s/%s",folder,file);
		if (sizeO(newpath)>1000)
			Quit_("cgrep; path too large(%-60.60s...)",newpath); // circular stop
		int type=file_typeCode(newpath,1);
		if (type=='d') cgrep(newpath,level+1);
		ei (type=='-' && extok(newpath)) cgrepf(newpath);
		}
	}

int main(int argc,char **argv) {
	folders=newO(Array); words=newO(Array); extents=newO(Array);
	for(int j=1;j<argc;++j) { char *cp=argv[j];
		if (cs_cmpKey("-f",cp)) { cp+=2;
			if (cs_length(cp)<1) Quit_("you must specify a folder with the -f option, like -f/users/beth");
			Array_addObject(folders,Char_Value(cp));
			}
		ei (cs_cmpKey(".",cp) && !cs_exact(".",cp) && !cs_exact("..",cp)) {
			Array_addObject(extents,Char_Value(cp));
			}
		ei (cs_cmpKey("-.",cp)) Array_addObject(words,Char_Value(cp+1));
		else Array_addObject(words,Char_Value(cp));
		}
	if (Array_count(words)<1) formatCgrep();
	if (Array_count(folders)<1) Array_addObject(folders,Char_Value("."));
	if (Array_count(extents)<1) defaultExtents();
	int nel=Array_count(folders);
	for(int j=0;j<nel;++j) cgrep(Array_sub(folders,j),0);
	return(0);
	}

