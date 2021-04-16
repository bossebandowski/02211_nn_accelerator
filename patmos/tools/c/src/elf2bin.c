#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <stddef.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <gelf.h>
#include <libelf.h>

static Elf *open_elf(int fd)
{
  // check libelf version
  elf_version(EV_CURRENT);

  Elf *elf = elf_begin(fd, ELF_C_READ, NULL);
  assert(elf);

  // check file kind
  Elf_Kind ek = elf_kind(elf);
  assert(ek == ELF_K_ELF);

  // check class
  int ec = gelf_getclass(elf);
  assert(ec == ELFCLASS32);

  // get elf header
  GElf_Ehdr hdr;
  GElf_Ehdr *tmphdr = gelf_getehdr(elf, &hdr);
  assert(tmphdr);

  return elf;
}

static void copy_segment(int infd, size_t src_pos, size_t src_size,
                         int outfd, size_t dst_pos, size_t dst_size)
{
  // allocate buffer
  char *buf = malloc(src_size);
  assert(buf);

  // copy from the file into the main memory
  lseek(infd, src_pos, SEEK_SET);
  read(infd, buf, src_size);

  // reposition and write to output file
  lseek(outfd, dst_pos, SEEK_SET);
  write(outfd, buf, src_size);
  
  free(buf);

  // pad to correct size
  size_t k;
  for (k = src_size; k < dst_size; k++) {
    const int b = 0;
    write(outfd, &b, 1);
  }
}

static void elf2bin_exec(Elf *elf, int infd, int outfd, int flat)
{
  // get program headers
  size_t n, i;
  int ntmp = elf_getphdrnum (elf, &n);
  assert(ntmp == 0);

  // copy executable segments
  for(i = 0; i < n; i++)
  {
    // get program header
    GElf_Phdr phdr;
    GElf_Phdr *phdrtmp = gelf_getphdr(elf, i, &phdr);
    assert(phdrtmp);

    if (phdr.p_type == PT_LOAD
        && ((phdr.p_flags & PF_X) || flat))
    {
      // some assertions
      assert(phdr.p_vaddr == phdr.p_paddr);
      assert(phdr.p_filesz <= phdr.p_memsz);

      copy_segment(infd, phdr.p_offset, phdr.p_filesz,
                   outfd, phdr.p_paddr, phdr.p_memsz);
    }
  }
}

static void elf2bin_data(Elf *elf, int infd, int outfd, unsigned displace)
{
  // get elf header
  GElf_Ehdr hdr;
  GElf_Ehdr *tmphdr = gelf_getehdr(elf, &hdr);
  assert(tmphdr);

  // get program headers
  size_t n, i;
  int ntmp = elf_getphdrnum (elf, &n);
  assert(ntmp == 0);

  int max_pos = 0x10;

  // copy read-only data segments
  for(i = 0; i < n; i++)
  {
    // get program header
    GElf_Phdr phdr;
    GElf_Phdr *phdrtmp = gelf_getphdr(elf, i, &phdr);
    assert(phdrtmp);

    if (phdr.p_type == PT_LOAD
        && !(phdr.p_flags & PF_X) && !(phdr.p_flags & PF_W))
    {
      // some assertions
      assert(phdr.p_vaddr == phdr.p_paddr);
      assert(phdr.p_filesz <= phdr.p_memsz);

      copy_segment(infd, phdr.p_offset, phdr.p_filesz,
                   outfd, phdr.p_paddr-displace, phdr.p_memsz);

      unsigned last_pos = phdr.p_paddr-displace+phdr.p_memsz;
      if (last_pos > max_pos) {
        max_pos = last_pos;
      }
    }
  }

  // copy writable data segment (only one allowed)
  int seen_wrdata = 0;
  for(i = 0; i < n; i++)
  {
    // get program header
    GElf_Phdr phdr;
    GElf_Phdr *phdrtmp = gelf_getphdr(elf, i, &phdr);
    assert(phdrtmp);

    if (phdr.p_type == PT_LOAD
        && !(phdr.p_flags & PF_X) && (phdr.p_flags & PF_W))
    {
      // some assertions
      assert(phdr.p_vaddr == phdr.p_paddr);
      assert(phdr.p_filesz <= phdr.p_memsz);
      assert(!seen_wrdata);

      copy_segment(infd, phdr.p_offset, phdr.p_filesz,
                   outfd, max_pos, phdr.p_filesz);

      // write information for run-time loading
      unsigned src_start = htonl(displace+max_pos);
      unsigned src_size = htonl(phdr.p_filesz);
      unsigned dst_start = htonl(phdr.p_paddr);
      unsigned dst_size = htonl(phdr.p_memsz);
      lseek(outfd, 0, SEEK_SET);
      write(outfd, &src_start, 4);
      write(outfd, &src_size, 4);
      write(outfd, &dst_start, 4);
      write(outfd, &dst_size, 4);

      seen_wrdata = 1;
    }
  }

  // pad to word boundary
  while (lseek(outfd, 0, SEEK_END) & 0x03) {
    const int b = 0;
    write(outfd, &b, 1);
  }
}

void usage(char *name) {
  fprintf(stderr, "Usage: %s [-d <disp>] <infile> <outfile1> <outfile2> | %s -f <infile> <outfile>\n", name, name);
}

int main(int argc, char* argv[]) {

    int opt;
    int flat = 0;
    unsigned displace = 0;
    int seen_displace = 0;

    while ((opt = getopt(argc, argv, "fd:")) != -1) {
      switch (opt) {
      case 'f':
        if (flat || seen_displace) {
          usage(argv[0]);
          exit(-1);
        } else {
          flat = 1;
        }
        break;
      case 'd':
        if (flat || seen_displace) {
          usage(argv[0]);
          exit(-1);
        } else {
          displace = strtol(optarg, NULL, 0);
          seen_displace = 1;
        }
        break;
      default:  /* '?' */
        usage(argv[0]);
        exit(-1);
      }
    }

    if ((!flat && (argc - optind) != 3) || (flat && (argc - optind) != 2)) {
        usage(argv[0]);
        exit(-1);
    }

    int infd = open(argv[optind], O_RDONLY, 0);
    if (infd == -1) {
      perror("Cannot open input file:");
    }

    int outfd_exec = open(argv[optind+1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (outfd_exec == -1) {
      perror("Cannot open output file:");
    }

    int outfd_data = -1;
    if (!flat) {
      outfd_data = open(argv[optind+2], O_CREAT | O_WRONLY | O_TRUNC, 0644);
      if (outfd_data == -1) {
        perror("Cannot open output file:");
      }
    }

    Elf *elf = open_elf(infd);

    elf2bin_exec(elf, infd, outfd_exec, flat);
    if (!flat) { elf2bin_data(elf, infd, outfd_data, displace); }
    
    elf_end(elf);

    close(infd);
    close(outfd_exec);
    if (!flat) { close(outfd_data); }
    
    return 0;
}
