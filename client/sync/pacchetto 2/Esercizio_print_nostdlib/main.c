#define EXIT 60
#define WRITE 1
 
void program_entry(void);
 
void exit(int err){
    __asm__("syscall"
        :
        :"a"(EXIT), "D"(err));
   
}
 
void _start() {
    program_entry();
    exit(0);
}
 
 
void program_entry(){
    char s[] = "Hello sysprog!\n";
    unsigned l = sizeof(s);
    unsigned fd =1;
    unsigned ret = 0;
    __asm__("syscall":
        "=a" (ret) :
        "a" (WRITE), "D" (fd), "S" (s), "d" (l));
//La a sarebbe il primo parametro, ovvero quale system call lanciare, e dalla lista delle systemcal di linux, io devo usare la ////WRITE, numero 1 in linux. Occhio che in windows cambia
//La D sarebbe il ssecondo parametro, quale file descriptor usare, e l'output è 1, std_err sarebbe 2
// La S sarebbe cosa metterci, ed è la stringa
// la d sarebbe la lunghezza della stringa

//Quell =a sarebbe cosa tornare se tutto funziona correttamente
}
