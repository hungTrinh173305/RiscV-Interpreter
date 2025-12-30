#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "linkedlist.h"
#include "hashtable.h"
#include "riscv.h"

/************** BEGIN HELPER FUNCTIONS PROVIDED FOR CONVENIENCE ***************/
const int R_TYPE = 0;
const int I_TYPE = 1;
const int MEM_TYPE = 2;
const int U_TYPE = 3;
const int UNKNOWN_TYPE = 4;

/**
 * Return the type of instruction for the given operation
 * Available options are R_TYPE, I_TYPE, MEM_TYPE, UNKNOWN_TYPE
 */
static int get_op_type(char *op)
{
    const char *r_type_op[] = {"add", "sub", "and", "or", "xor", "nor", "slt", "sll", "sra"};
    const char *i_type_op[] = {"addi", "andi", "ori", "xori", "slti"};
    const char *mem_type_op[] = {"lw", "lb", "sw", "sb"};
    const char *u_type_op[] = {"lui"};
    for (int i = 0; i < (int)(sizeof(r_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(r_type_op[i], op) == 0)
        {
            return R_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(i_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(i_type_op[i], op) == 0)
        {
            return I_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(mem_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(mem_type_op[i], op) == 0)
        {
            return MEM_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(u_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(u_type_op[i], op) == 0)
        {
            return U_TYPE;
        }
    }
    return UNKNOWN_TYPE;
}
/*************** END HELPER FUNCTIONS PROVIDED FOR CONVENIENCE ****************/

// Debugging
#define DEBUG_MSG(str) //printf(str); printf("\n"); fflush(stdout)

// Define named var for each struct
typedef struct insn_data_struct
{
    int rd;
    union {
        int rs1, upperimm;
    };
    union {
        int rs2, imm;
    };
} insn_data_t;

registers_t *registers;
// TODO: create any additional variables to store the state of the interpreter
hashtable_t *main_memory;
insn_data_t *insn_data;
char *rd_text, *rs1_text, *rs2_text;
const int RD_TEXT_SIZE = 5, RS1_TEXT_SIZE = 64, RS2_TEXT_SIZE = 64;

// Forward decl
void parse(char *instruction);
void parse_utype(char *instruction);

void init(registers_t *starting_registers)
{
    registers = starting_registers;
    // TODO: initialize any additional variables needed for state
    insn_data = malloc(sizeof(insn_data_t));
    rd_text = malloc(RD_TEXT_SIZE);
    rs1_text = malloc(RS1_TEXT_SIZE);
    rs2_text = malloc(RS2_TEXT_SIZE);
    main_memory = ht_init(257);
    
    //printf("sra_portable: %i\n", sra_portable(-4, 1));

    //printf("string alloc: %p, %p, %p\n", (void *)rd_text, (void *)rs1_text, (void *)rs2_text);

    /*
    const char *unit_test_1 = "s1, -462";
    char *unit_test_ptr = unit_test_1;
    parse(unit_test_ptr);
    DEBUG_MSG("Unit test: ");
    printf("rd: %i", insn_data->rd);
    printf("rs1: %i", insn_data->rs1);
    printf("rs2: %i", insn_data->rs2);
    */

    //exit(0);
}

int get_register_loc(char *reg)
{
    if (strcmp(reg, "fp") == 0) return 8;

    const char *abi_names[] = {
        "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
        "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
        "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
        "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6",
    };

    int number = atoi(&reg[1]);
    if (reg[0] == 'x')
    {
        return number;
    }
    else
    {
        for (int i = 0; i < (int)(sizeof(abi_names) / sizeof(char *)); i++)
        {
            if (strcmp(reg, abi_names[i]) == 0) return i;
        }
        return -1;
    }
}

static int min(int a, int b) { if (a < b) return a; else return b; }

// Minimal RegEx search parser because I can
#define REGEX_EXCEPT_ESCAPE_FAIL 128
#define REGEX_EXCEPT_ILLEGAL_KLEENE 129
#define REGEX_EXCEPT_ILLEGAL_BRACKET 130
#define REGEX_PARSER_MODE_NONE -1
#define REGEX_PARSER_MODE_SINGLE 0
#define REGEX_PARSER_MODE_STAR 1
#define REGEX_PARSER_MODE_CAPTURE 2
#define ESC_STR_COUNT 1
int regex_extract(char **text_ptr, const char **regex_ptr, char* str_alloc, size_t str_size)
{
    // Ensure size of string
    if (str_size <= 0) return 0;

    DEBUG_MSG("Starting RegEx...");

    // Extract pointers
    char *text = *text_ptr;
    char *regex = *regex_ptr;

    // Map from escape string to text
    const char escape_chr[ESC_STR_COUNT][2] = { 
        {'t', '\t'}
    };
    char regex_char = '\0';
    int parser_mode = -1;
    char *org_text_ptr = NULL;

    DEBUG_MSG("Start main loop:");

    while(*text != '\0' && *regex != '\0')
    {
        DEBUG_MSG("Main loop:");

        switch (*regex)
        {
            // Escape character
            case '\\':
            {
                regex++;
                if (*regex != '\0')
                {
                    for (int i = 0; i < ESC_STR_COUNT; i++)
                    {
                        if (*regex == escape_chr[i][0])
                        {
                            regex_char = escape_chr[i][1];
                            goto backslash_skip;
                        }
                    }
                    regex_char = *regex;
                    // jump to here if the definition is found in escape str
                    // to skip the direct assignment above
                    backslash_skip:
                }
                else exit(REGEX_EXCEPT_ESCAPE_FAIL);
                break;
            }
            case '*':
            {
                // Illegal
                exit(REGEX_EXCEPT_ILLEGAL_KLEENE);
            }
            case ')':
            {
                // Illegal
                exit(REGEX_EXCEPT_ILLEGAL_BRACKET);
            }
            case '(':
            {
                if (regex[1] != ')') exit(REGEX_EXCEPT_ILLEGAL_BRACKET);
                parser_mode = REGEX_PARSER_MODE_CAPTURE;
                regex++;
                break;
            }
            default:
            {
                regex_char = *regex;
            }
        }
        regex++;
        if (parser_mode == REGEX_PARSER_MODE_NONE) switch (*regex)
        {
            case '*': parser_mode = REGEX_PARSER_MODE_STAR; regex++; break;
            default: parser_mode = REGEX_PARSER_MODE_SINGLE; break;
        }

        //printf("Regex char: \'%c\', %i\n", regex_char, (int)regex_char);
        //printf("Regex ptr: \'%c\'\n", *regex);
        //printf("Text ptr: \'%c\'\n", *text);
        //printf("Mode: %i\n", parser_mode);
        //DEBUG_MSG(" Main loop 2:");
        
        switch (parser_mode)
        {
            case REGEX_PARSER_MODE_SINGLE:
            {
                if (*text != regex_char) return 0;
                text++;
                break;
            }
            case REGEX_PARSER_MODE_STAR:
            {
                while (*text != '\0' && *text == regex_char) text++;
                break;
            }
            case REGEX_PARSER_MODE_CAPTURE:
            {
                // Capture only valid RISC-V characters for now
                org_text_ptr = text;
                while (1)
                {
                    int is_valid = *text != '\0' && 
                        (isalnum((unsigned char)(*text)) || *text == '-' || *text == '.');
                    if (is_valid)
                    {
                        text++;
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
                break;
            }
            default: DEBUG_MSG("No parser mode assigned!\n"); break;
        }

        DEBUG_MSG(" Main loop 3:");

        regex_char = '\0';
        if (parser_mode == REGEX_PARSER_MODE_CAPTURE)
        {
            // Update pointers for future call
            DEBUG_MSG("  Updating:");
            *text_ptr = text;
            *regex_ptr = regex;

            // Fill the buffer with captured data
            int copied_str_count = (int)min((int)(text - org_text_ptr), str_size - 1);

            if (copied_str_count > 0)
            {
                DEBUG_MSG("  Mem-copying:");
                //printf("text_ptr_text: %s\n", org_text_ptr);
                //printf("regex_ptr_text: %s\n", regex);
                //printf("size: %i\n", copied_str_count);
                //printf("string alloc: %p\n", (void *)str_alloc);
                fflush(stdout);
                memcpy(str_alloc, org_text_ptr, copied_str_count);
                DEBUG_MSG("  Null-terminating:");
                str_alloc[copied_str_count] = '\0';
            }

            else return 0;

            DEBUG_MSG("  End.");
            return 1;
        }
        parser_mode = REGEX_PARSER_MODE_NONE;
    }
    return 0;
}

void preprocess_replace_tab(char *instruction)
{
    while (*instruction != '\0')
    {
        if (*instruction == '\t') *instruction = ' ';
        *instruction++;
    }
}

// regex: \s*()\s*,\s*()\s*,\s*()
void parse(char *instruction)
{
    preprocess_replace_tab(instruction);

    const char regex[] = " *() *, *() *, *()";
    const char *regex_ptr = regex;
    char *instruction_ptr = instruction;
    regex_extract(&instruction_ptr, &regex_ptr, rd_text, RD_TEXT_SIZE);
    insn_data->rd = get_register_loc(rd_text); // The first argument should always be a register

    regex_extract(&instruction_ptr, &regex_ptr, rs1_text, RS1_TEXT_SIZE);
    int rs1_try_parse = get_register_loc(rs1_text);
    if (rs1_try_parse == -1)
    {
        rs1_try_parse = (int)strtol(rs1_text, NULL, 0);
    }
    insn_data->rs1 = rs1_try_parse;

    if (regex_extract(&instruction_ptr, &regex_ptr, rs2_text, RS2_TEXT_SIZE))
    {
        int rs2_try_parse = get_register_loc(rs2_text);
        if (rs2_try_parse == -1)
        {
            rs2_try_parse = (int)strtol(rs2_text, NULL, 0);
        }
        insn_data->rs2 = rs2_try_parse;
    }
    else insn_data->rs2 = 0;
}

// regex: \s*()\s*,\s*()\s*,\s*()
void parse_saveload(char *instruction)
{
    preprocess_replace_tab(instruction);

    const char regex[] = " *() *, *()\\(()\\)";
    const char *regex_ptr = regex;
    char *instruction_ptr = instruction;
    regex_extract(&instruction_ptr, &regex_ptr, rd_text, RD_TEXT_SIZE);
    insn_data->rd = get_register_loc(rd_text); // The first argument should always be a register

    regex_extract(&instruction_ptr, &regex_ptr, rs1_text, RS1_TEXT_SIZE);
    int rs1_try_parse = get_register_loc(rs1_text);
    if (rs1_try_parse == -1)
    {
        rs1_try_parse = (int)strtol(rs1_text, NULL, 0);
    }
    insn_data->imm = rs1_try_parse;

    regex_extract(&instruction_ptr, &regex_ptr, rs2_text, RS2_TEXT_SIZE);
    int rs2_try_parse = get_register_loc(rs2_text);
    if (rs2_try_parse == -1)
    {
        rs2_try_parse = (int)strtol(rs2_text, NULL, 0);
    }
    insn_data->rs1 = rs2_try_parse;
}

// regex: \s*()\s*,\s*()  (for U-type: lui rd, imm)
void parse_utype(char *instruction)
{
    preprocess_replace_tab(instruction);

    const char regex[] = " *() *, *()";
    const char *regex_ptr = regex;
    char *instruction_ptr = instruction;
    regex_extract(&instruction_ptr, &regex_ptr, rd_text, RD_TEXT_SIZE);
    insn_data->rd = get_register_loc(rd_text); // The first argument should always be a register

    regex_extract(&instruction_ptr, &regex_ptr, rs1_text, RS1_TEXT_SIZE);
    // For U-type, the second operand is always an immediate value
    int imm_try_parse = get_register_loc(rs1_text);
    if (imm_try_parse == -1)
    {
        imm_try_parse = (int)strtol(rs1_text, NULL, 0);
    }
    insn_data->upperimm = imm_try_parse;
}

int sra_portable(int a, int sh)
{
    // In C, right shifting for negative number is implementation-defined.
    // bruh
    unsigned int srl_temp = (unsigned int)a >> (unsigned int)sh;
    unsigned int vacant = (a < 0) * ((unsigned int)-1 << (sizeof(unsigned int) * 8 - sh));
    return srl_temp | vacant;
}

void step(char *instruction)
{
    // Extracts and returns the substring before the first space character,
    // by replacing the space character with a null-terminator.
    // `instruction` now points to the next character after the space
    // See `man strsep` for how this library function works
    char *op = strsep(&instruction, " ");
    // Uses the provided helper function to determine the type of instruction
    int op_type = get_op_type(op);
    // Skip this instruction if it is not in our supported set of instructions
    if (op_type == UNKNOWN_TYPE)
    {
        return;
    }

    // TODO: write logic for evaluating instruction on current interpreter state

    // Parse the instruction
    if (op_type == MEM_TYPE) parse_saveload(instruction);
    else if (op_type == U_TYPE) parse_utype(instruction);
    else parse(instruction);

    if (op_type == R_TYPE)
    {
        int output = 0;
        int rs1 = registers->r[insn_data->rs1], rs2 = registers->r[insn_data->rs2];
        if (strcmp("add", op) == 0) output = rs1 + rs2;
        else if (strcmp("sub", op) == 0) output = rs1 - rs2;
        else if (strcmp("and", op) == 0) output = rs1 & rs2;
        else if (strcmp("or", op) == 0) output = rs1 | rs2;
        else if (strcmp("xor", op) == 0) output = rs1 ^ rs2;
        else if (strcmp("nor", op) == 0) output = ~(rs1 | rs2);
        else if (strcmp("slt", op) == 0) output = rs1 < rs2;
        else if (strcmp("sll", op) == 0) output = rs1 << rs2;
        else if (strcmp("sra", op) == 0) output = sra_portable(rs1, rs2);
        registers->r[insn_data->rd] = output;
    }
    else if (op_type == I_TYPE)
    {
        int output = 0;
        int rs1 = registers->r[insn_data->rs1], imm = insn_data->imm;
        if (strcmp("addi", op) == 0) output = rs1 + imm;
        else if (strcmp("andi", op) == 0) output = rs1 & imm;
        else if (strcmp("ori", op) == 0) output = rs1 | imm;
        else if (strcmp("xori", op) == 0) output = rs1 ^ imm;
        else if (strcmp("slti", op) == 0) output = rs1 < imm;
        registers->r[insn_data->rd] = output;
    }
    else if (op_type == MEM_TYPE)
    {
        int rd = registers->r[insn_data->rd], 
            rs1 = registers->r[insn_data->rs1], imm = insn_data->imm;

        int addr_aligned = (rs1 + imm) & ~3;
        int addr_offset = (rs1 + imm) & 3;
        if (strcmp("lw", op) == 0)
        {
            // Enforce that our address is 4-byte aligned
            registers->r[insn_data->rd] = ht_get(main_memory, addr_aligned);
        }
        else if (strcmp("lb", op) == 0)
        {
            // Extract single byte from int
            unsigned int raw_data = (unsigned int)ht_get(main_memory, addr_aligned);
            unsigned char masked_data = (unsigned char)((raw_data >> (addr_offset * 8)) & 0xFF);
            registers->r[insn_data->rd] = (int)((signed char)masked_data);
        }
        else if (strcmp("sw", op) == 0)
        {
            ht_add(main_memory, addr_aligned, rd);
        }
        else if (strcmp("sb", op) == 0)
        {
            // Set byte mask for writing to int
            unsigned char raw_data = (unsigned char)rd;
            unsigned int raw_data_mask = (raw_data << (addr_offset * 8));
            unsigned int zeroing_data_mask = ~(0xFF << (addr_offset * 8));

            unsigned int mem_data = (unsigned int)ht_get(main_memory, addr_aligned);
            unsigned int masked_data = (mem_data & zeroing_data_mask) | raw_data_mask;
            ht_add(main_memory, addr_aligned, (int)masked_data);
        }
    }
    else if (op_type == U_TYPE)
    { 
        int upperimm = insn_data->upperimm;
        registers->r[insn_data->rd] = upperimm << 12;
    }
    
    // Reset register 0, in case it is overwritten.
    registers->r[0] = 0;
}
