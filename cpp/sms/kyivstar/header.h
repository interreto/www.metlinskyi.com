#pragma comment(linker,"/MERGE:.rdata=.text")
#pragma comment(linker,"/FILEALIGN:512 /SECTION:.text,EWRX /IGNORE:4078")

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport)int Operator(char * lp);
__declspec(dllexport)bool Send(char* phone,char* msg);

#ifdef __cplusplus
}
#endif 
