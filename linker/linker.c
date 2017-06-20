#include "linker.h"

int main(int argc, char** argv)
{
    open_files();

    elf_header* elfheader = (elf_header*) calloc(1, sizeof(elf_header));

    elfheader->magic_number = 0x7F454C46;
    elfheader->bit_flag = 0x1;
    elfheader->endianness = 0x2;
    elfheader->version = 0x0;
    elfheader->abi = 0x0;
    elfheader->abi_unused = 0x0;
    elfheader->type = 0x4;
    elfheader->instr_set = 0x0;
    elfheader->version_2 = 0x0;
    elfheader->entry_point = 0x0;
    elfheader->header_table = 

    apply_headers();
    copy_text();
    relocate_addrs();
    apply_section_header();
}

int open_files()
{
    elf_file = fopen("helloworld.elf", "w+");
    object_file = fopen("helloworld.o", "r");
}

int apply_headers()
{
    for (int i = 0; i < sizeof(elf_header); i++)
    {
        fputc((char) *(elf_header+i), elf_file);
    }
}

int copy_text()
{
    //get data section start
}
