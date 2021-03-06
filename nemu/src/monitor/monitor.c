#include <isa.h>
#include <memory/paddr.h>


void init_rand();
void init_log(const char *log_file);
void init_mem();
void init_difftest(char *ref_so_file, long img_size, int port);
void init_device();
void init_sdb();
void init_disasm(const char *triple);

static void welcome() {
  Log("Trace: %s", MUXDEF(CONFIG_TRACE, ASNI_FMT("ON", ASNI_FG_GREEN), ASNI_FMT("OFF", ASNI_FG_RED)));
  IFDEF(CONFIG_TRACE, Log("If trace is enabled, a log file will be generated "
        "to record the trace. This may lead to a large log file. "
        "If it is not necessary, you can disable it in menuconfig"));
  Log("Build time: %s, %s", __TIME__, __DATE__);
  printf("Welcome to %s-NEMU!\n", ASNI_FMT(str(__GUEST_ISA__), ASNI_FG_YELLOW ASNI_BG_RED));
  printf("For help, type \"help\"\n");
  //Log("Exercise: Please remove me in the source code and compile NEMU again.");
  //assert(0);
}

#ifndef CONFIG_TARGET_AM
#include <getopt.h>

#ifdef CONFIG_FTRACE
#include <elf.h>
char strtab[4096], strtab_child;
Elf32_Sym sym[256], sym_child[256];
int sym_num = 0, sym_child_num = 0;
int recursion_depth = 0;

void ftrace_call(vaddr_t pc, vaddr_t dest){
  for(int i = 0; i < sym_num; i++){
    //printf("%d " FMT_WORD " " FMT_WORD "\n", i, sym[i].st_value, sym[i].st_size);
    //printf("%d\n", sym[i].st_info);
    //printf("standard:%d\n/STT", STT_FUNC);
    if (sym[i].st_info == 18){
      //printf(":::::\n");
      if (sym[i].st_value <= dest && dest < sym[i].st_value + sym[i].st_size){
        if (sym[i].st_value <= pc && pc < sym[i].st_value + sym[i].st_size)
          continue;
        char *func_name = strtab + sym[i].st_name;
        log_write(FMT_WORD ": ", pc);
        for(int i = 0; i < recursion_depth; i++) log_write(" ");
        log_write("call [@%s" FMT_WORD "]\n", func_name, sym[i].st_value);
        recursion_depth++;
        return;
      }
    }
  }
  // parse_elf((void*)0x83000000, strtab_child, sym_child);
}
void ftrace_ret(vaddr_t pc, vaddr_t dest){
  //printf(FMT_WORD " " FMT_WORD "\n", pc, dest);
  recursion_depth--;
  log_write(FMT_WORD ": ", pc);
  for(int i = 0; i < recursion_depth; i++) log_write(" ");
  for(int i = 0; i < sym_num; i++){
    if (sym[i].st_info == 18){
      if (sym[i].st_value <= pc && pc < sym[i].st_value + sym[i].st_size){
        char *func_name = strtab + sym[i].st_name;
        log_write("ret [@%s]\n", func_name);
        return;
      }
    }
  }
  // parse_elf((void*)0x83000000, strtab_child, sym_child);
}
void parse_elf(char *str, char *strtab, Elf32_Sym *sym){
  // printf("%s\n\n\n", str);
  FILE *fp;
  fp = fopen(str, "r");

  // read elf_head
  Elf32_Ehdr elf_head;
  int a;
  a = fread(&elf_head, sizeof(Elf32_Ehdr), 1, fp);
  int shnum = elf_head.e_shnum, shstrndx = elf_head.e_shstrndx; 
  // printf("ident  : %s\n", elf_head.e_ident);
  // printf("strndx : %d\n", strndx);

  // read shdr
  Elf32_Shdr *shdr = (Elf32_Shdr*)malloc(sizeof(Elf32_Shdr) * shnum);
  a = fseek(fp, elf_head.e_shoff, SEEK_SET);
  a = fread(shdr, sizeof(Elf32_Shdr) * shnum, 1, fp);
  
  // read shstrtab
  rewind(fp);
  a = fseek(fp, shdr[shstrndx].sh_offset, SEEK_SET);
  char shstrtab[shdr[shstrndx].sh_size];
  a = fread(shstrtab, shdr[shstrndx].sh_size, 1, fp);

  // locate strtab and symtab
  int strndx = 0, symndx = 0;
  for(int i = 0; i < shnum; i++){
    char *shname = shstrtab + shdr[i].sh_name;
    //printf("%d : %s\n", i, shname);
    if (strcmp(shname, ".strtab") == 0) strndx = i;
    else if (strcmp(shname, ".symtab") == 0) symndx = i;
  }

  // read strtab
  rewind(fp);
  a = fseek(fp, shdr[strndx].sh_offset, SEEK_SET);
  a = fread(strtab, shdr[strndx].sh_size, 1, fp);
  // read symtab
  rewind(fp);
  sym_num = shdr[symndx].sh_size / sizeof(Elf32_Sym);
  // printf("%d\n", sym_num);
  a = fseek(fp, shdr[symndx].sh_offset, SEEK_SET);
  a = fread(sym, shdr[symndx].sh_size, 1, fp);

  if(a) a = a; // avoid Werror check
  free(shdr);
}
#endif

void sdb_set_batch_mode();

static char *log_file = NULL;
static char *diff_so_file = NULL;
static char *img_file = NULL;
static int difftest_port = 1234;

static long load_img() {
  if (img_file == NULL) {
    Log("No image is given. Use the default build-in image.");
    return 4096; // built-in image size
  }

#ifdef CONFIG_FTRACE
  static char dest[2048];
  strcpy(dest, img_file);
  int len = strlen(dest);
  strcpy(dest + len - 3, "elf");
  printf("%s\n", dest);
  parse_elf(dest, strtab, sym);
#endif

  FILE *fp = fopen(img_file, "rb");
  Assert(fp, "Can not open '%s'", img_file);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  Log("The image is %s, size = %ld", img_file, size);

  fseek(fp, 0, SEEK_SET);
  int ret = fread(guest_to_host(RESET_VECTOR), size, 1, fp);
  assert(ret == 1);

  fclose(fp);
  return size;
}

static int parse_args(int argc, char *argv[]) {
  const struct option table[] = {
    {"batch"    , no_argument      , NULL, 'b'},
    {"log"      , required_argument, NULL, 'l'},
    {"diff"     , required_argument, NULL, 'd'},
    {"port"     , required_argument, NULL, 'p'},
    {"help"     , no_argument      , NULL, 'h'},
    {0          , 0                , NULL,  0 },
  };
  int o;
  // printf("%d\n\n", argc);
  // for(int i = 0; i < argc; i++){
  //   printf("%s\n\n", argv[i]);
  // }
  while ( (o = getopt_long(argc, argv, "-bhfl:d:p:", table, NULL)) != -1) {
    switch (o) {
      case 'b': sdb_set_batch_mode(); break;
      case 'p': sscanf(optarg, "%d", &difftest_port); break;
      case 'f': 
      case 'l': log_file = optarg; break;
      case 'd': diff_so_file = optarg; break;
      case 1: img_file = optarg; return optind - 1;
      default:
        printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        printf("\t-b,--batch              run with batch mode\n");
        printf("\t-l,--log=FILE           output log to FILE\n");
        printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
        printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
        printf("\n");
        exit(0);
    }
  }
 
  return 0;
}

void init_monitor(int argc, char *argv[]) {
  /* Perform some global initialization. */

  /* Parse arguments. */
  parse_args(argc, argv);

  /* Set random seed. */
  init_rand();

  /* Open the log file. */
  init_log(log_file);

  /* Initialize memory. */
  init_mem();

  /* Initialize devices. */
  IFDEF(CONFIG_DEVICE, init_device());

  /* Perform ISA dependent initialization. */
  init_isa();

  /* Load the image to memory. This will overwrite the built-in image. */
  long img_size = load_img();

  /* Initialize differential testing. */
  init_difftest(diff_so_file, img_size, difftest_port);

  /* Initialize the simple debugger. */
  init_sdb();

  IFDEF(CONFIG_ITRACE, init_disasm(
    MUXDEF(CONFIG_ISA_x86,     "i686",
    MUXDEF(CONFIG_ISA_mips32,  "mipsel",
    MUXDEF(CONFIG_ISA_riscv32, "riscv32",
    MUXDEF(CONFIG_ISA_riscv64, "riscv64", "bad")))) "-pc-linux-gnu"
  ));

  /* Display welcome message. */
  welcome();
}
#else // CONFIG_TARGET_AM
static long load_img() {
  extern char bin_start, bin_end;
  size_t size = &bin_end - &bin_start;
  Log("img size = %ld", size);
  memcpy(guest_to_host(RESET_VECTOR), &bin_start, size);
  return size;
}

void am_init_monitor() {
  init_rand();
  init_mem();
  init_isa();
  load_img();
  IFDEF(CONFIG_DEVICE, init_device());
  welcome();
}
#endif
