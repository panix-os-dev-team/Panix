;; Multiboot 2 boot protocol header
extern px_boot_stack
extern px_boot_multiboot2

%macro tag_begin 1-2 0
align 8
.start:
    dw %1
    dw %2
    dd (.end - .start)
%endmacro

%macro tag_end 0
.end:
%endmacro

MULTIBOOT_MAGIC     equ 0xE85250D6
MULTIBOOT_ARCH      equ 0
MULTIBOOT_LENGTH    equ (__multiboot_header_end - __multiboot2_header_start)
MULTIBOOT_CHECKSUM  equ -(MULTIBOOT_MAGIC + MULTIBOOT_ARCH + MULTIBOOT_LENGTH)

MULTIBOOT_TAG_ENTRY equ 3
MULTIBOOT_TAG_LAST  equ 0

MULTIBOOT_ENTRY equ multiboot_entry

section .multiboot2
align 8
__multiboot2_header_start:
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_ARCH
    dd MULTIBOOT_LENGTH
    dd MULTIBOOT_CHECKSUM

__multiboot2_entry_tag:
    tag_begin MULTIBOOT_TAG_ENTRY, 1
    dd  MULTIBOOT_ENTRY
    tag_end

__multiboot2_last_tag:
    tag_begin MULTIBOOT_TAG_LAST
    tag_end

__multiboot_header_end:

;; Multiboot 2 boot protocol entry
section .text

global multiboot_entry:function (multiboot_entry.end - multiboot_entry)
multiboot_entry:
    cli
    cld

    ; To set up a stack, we set the esp register to point to the top of our
    ; stack (as it grows downwards on x86 systems).
    mov esp, px_boot_stack
    xor ebp, ebp

    push eax ; Push the multiboot magic
    push ebx ; Push the multiboot header adress.

    call px_boot_multiboot2

    cli

.hang:
    hlt
    jmp .hang

.end:
