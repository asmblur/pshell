#include <tamtypes.h>

#define MIPSI_J(__addr) (0x08000000 | ((((u32) __addr) << 4) >> 6))
#define MIPSI_JAL(__addr) (0x0C000000 | ((((u32) __addr) << 4) >> 6))
#define MIPSI_JR_RA() (0x03E00008)

#define ALIGN_16(x) ((x & 0xFFFFFFF0) + (((x & 0x0F) != 0) * 16))
#define IsReturn(__addr) (((u32 *) (__addr))[0] == 0x03E00008)

/* Describes an IRX import list.  */
typedef struct _ioplib_imp_list {
	u32	magic;          // 0x00
	struct _ioplib_imp_list *next;    // 0x04
	u16	version;        // 0x08
	u16	flags;          // 0x0A
	u8	name[8];        // 0x0C-0x13
	void	*imports[0];            // 0x14
} ioplib_imp_list;

/* Describes an IRX export library.  */
typedef struct _ioplib_exp_lib {
	struct _ioplib_exp_lib *prev;     // 0x00
	struct _ioplib_imp_list *links;  // 0x04
	u16	version;        // 0x08
	u16	flags;          // 0x0A
	u8	name[8];        // 0x0C
	void	*exports[0];            // 0x14
} ioplib_exp_lib;

typedef struct _ioplib_imp_entry {
    u32 jump;
    u16 fno;
    u16 ori_zero;
} ioplib_imp_entry;

typedef struct ioplib_mod_image_t
{
    struct ioplib_mod_image_t *next;               // 0x00
    u32 name;               // 0x04

    u16 version;            // 0x08
    u16 field_A;            // 0x0A

    u16 index;              // 0x0C
    u16 field_E;            // 0x0E

    void *entry;            // 0x10
    void *gp;               // 0x14
    void *load_addr;        // 0x18
    u32 text_size;          // 0x1C
    u32 data_size;          // 0x20
    u32 bss_size;           // 0x24
    u32 field_28;           // 0x28
    u32 field_2C;           // 0x2C
} ioplib_mod_image;

// Prototypes
ioplib_exp_lib *findLib(char *libName);
void fixupImportStubs(ioplib_exp_lib *lib, int callNum, void *addr);
int countExports(ioplib_exp_lib *lib);
ioplib_mod_image *findModByName(char *modName);
ioplib_mod_image *findModById(int id);
ioplib_mod_image *findModByAddr(u32 addr);
void printModuleInfo(ioplib_mod_image *mod);
