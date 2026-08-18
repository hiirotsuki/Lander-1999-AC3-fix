/* // SPDX-License-Identifier: GPLv2 or later */

#include <windows.h>

#define SAM_READWRITE   0x0002001Fu   /* ...plus QUERY_VALUE|ENUM|NOTIFY */
#define SAM_READONLY    0x00020019u   /* KEY_READ                        */
#define LANDER_KEY      "SOFTWARE\\Psygnosis\\Studios\\Lander"

#if defined(LANDER_GLIDE)

#define SAM_IMM         0x0047AE62u   /* imm32 of PUSH 0x20006 @ 0x0047AE61   */
#define PATCH_ADDR      0x00461E74u   /* imm32 of MOV [0x5470C0], 0x461B70    */

#if defined(LANDER_NOCD)
#define CDCHECK_ADDR    0x00415CF0u
static const unsigned char g_cdcheck[] = {
    0xB8, 0x01, 0x00, 0x00, 0x00,   /* mov eax, 1 */
    0xC3                            /* ret        */
};
#endif

static const unsigned char g_handler[] = {
    /* 000 */ 0xA1, 0x78, 0xEF, 0x59, 0x00,                            /* mov  eax, [0x59EF78]  ; this->pMediaEventEx */
    /* 005 */ 0x85, 0xC0,                                              /* test eax, eax                              */
    /* 007 */ 0x0F, 0x84, 0xF2, 0x00, 0x00, 0x00,                      /* jz   give_up (0FF)                         */
    /* 00D */ 0xA1, 0x74, 0xEF, 0x59, 0x00,                            /* mov  eax, [0x59EF74]  ; this->pMediaControl */
    /* 012 */ 0x85, 0xC0,                                              /* test eax, eax                              */
    /* 014 */ 0x0F, 0x84, 0xE5, 0x00, 0x00, 0x00,                      /* jz   give_up (0FF)                         */
    /* 01A */ 0xB9, 0xD0, 0xEE, 0x59, 0x00,                            /* mov  ecx, 0x59EED0                         */
    /* 01F */ 0xB8, 0xC0, 0x1F, 0x46, 0x00,                            /* mov  eax, 0x461FC0   ; FUN_00465c30        */
    /* 024 */ 0xFF, 0xD0,                                              /* call eax        ; event pump               */
    /* 026 */ 0x85, 0xC0,                                              /* test eax, eax                              */
    /* 028 */ 0x0F, 0x85, 0xC4, 0x00, 0x00, 0x00,                      /* jnz  done (0F2)                            */
    /* 02E */ 0xB9, 0xD0, 0xEE, 0x59, 0x00,                            /* mov  ecx, 0x59EED0                         */
    /* 033 */ 0xB8, 0x70, 0x23, 0x46, 0x00,                            /* mov  eax, 0x462370   ; FUN_00465fe0        */
    /* 038 */ 0xFF, 0xD0,                                              /* call eax        ; Stop()                   */
    /* 03A */ 0xB9, 0xD0, 0xEE, 0x59, 0x00,                            /* mov  ecx, 0x59EED0                         */
    /* 03F */ 0x8B, 0x41, 0x08,                                        /* mov  eax, [ecx+8]    ; flag_0x8            */
    /* 042 */ 0x85, 0xC0,                                              /* test eax, eax                              */
    /* 044 */ 0x0F, 0x85, 0x9C, 0x00, 0x00, 0x00,                      /* jnz  just_run (0E6)                        */
    /* 04A */ 0x6A, 0x00,                                              /* push 0                                     */
    /* 04C */ 0xB9, 0xD0, 0xEE, 0x59, 0x00,                            /* mov  ecx, 0x59EED0                         */
    /* 051 */ 0xB8, 0x20, 0x1D, 0x46, 0x00,                            /* mov  eax, 0x461D20   ; FUN_00465990        */
    /* 056 */ 0xFF, 0xD0,                                              /* call eax        ; pick track               */
    /* 058 */ 0xB9, 0xD0, 0xEE, 0x59, 0x00,                            /* mov  ecx, 0x59EED0                         */
    /* 05D */ 0xB8, 0x20, 0x22, 0x46, 0x00,                            /* mov  eax, 0x462220   ; FUN_00465e90        */
    /* 062 */ 0xFF, 0xD0,                                              /* call eax        ; teardown                 */
    /* 064 */ 0xB9, 0xD0, 0xEE, 0x59, 0x00,                            /* mov  ecx, 0x59EED0                         */
    /* 069 */ 0xB8, 0x50, 0x20, 0x46, 0x00,                            /* mov  eax, 0x462050   ; FUN_00465cc0        */
    /* 06E */ 0xFF, 0xD0,                                              /* call eax        ; new graph                */
    /* 070 */ 0x85, 0xC0,                                              /* test eax, eax                              */
    /* 072 */ 0x0F, 0x85, 0x87, 0x00, 0x00, 0x00,                      /* jnz  give_up (0FF)                         */
    /* 078 */ 0x6A, 0x40,                                              /* push 0x40       ; cchWideChar              */
    /* 07A */ 0x68, 0xE8, 0xEE, 0x59, 0x00,                            /* push 0x59EEE8  ; this+0x18                 */
    /* 07F */ 0x6A, 0xFF,                                              /* push -1         ; cbMultiByte              */
    /* 081 */ 0x68, 0x98, 0xEF, 0x59, 0x00,                            /* push 0x59EF98  ; ansi path                 */
    /* 086 */ 0x6A, 0x00,                                              /* push 0          ; dwFlags                  */
    /* 088 */ 0x6A, 0x00,                                              /* push 0          ; CodePage                 */
    /* 08A */ 0xFF, 0x15, 0xB8, 0x20, 0x48, 0x00,                      /* call [MultiByteToWideChar]                 */
    /* 090 */ 0xA1, 0x98, 0xF0, 0x59, 0x00,                            /* mov  eax, [0x59F098]                       */
    /* 095 */ 0x85, 0xC0,                                              /* test eax, eax                              */
    /* 097 */ 0x0F, 0x84, 0x14, 0x00, 0x00, 0x00,                      /* jz   simple (0B1)                          */
    /* 09D */ 0xB9, 0xD0, 0xEE, 0x59, 0x00,                            /* mov  ecx, 0x59EED0                         */
    /* 0A2 */ 0xB8, 0x00, 0x21, 0x46, 0x00,                            /* mov  eax, 0x462100   ; FUN_00465d70        */
    /* 0A7 */ 0xFF, 0xD0,                                              /* call eax        ; manual connect           */
    /* 0A9 */ 0x85, 0xC0,                                              /* test eax, eax                              */
    /* 0AB */ 0x0F, 0x84, 0x14, 0x00, 0x00, 0x00,                      /* jz   notify (0C5)                          */
    /* 0B1 */ 0xB9, 0xD0, 0xEE, 0x59, 0x00,                            /* mov  ecx, 0x59EED0  ; simple:              */
    /* 0B6 */ 0xB8, 0xE0, 0x20, 0x46, 0x00,                            /* mov  eax, 0x4620E0   ; FUN_00465d50        */
    /* 0BB */ 0xFF, 0xD0,                                              /* call eax        ; RenderFile               */
    /* 0BD */ 0x85, 0xC0,                                              /* test eax, eax                              */
    /* 0BF */ 0x0F, 0x85, 0x2E, 0x00, 0x00, 0x00,                      /* jnz  teardown (0F3)                        */
    /* 0C5 */ 0xA1, 0x78, 0xEF, 0x59, 0x00,                            /* mov  eax, [0x59EF78]  ; notify:            */
    /* 0CA */ 0x85, 0xC0,                                              /* test eax, eax                              */
    /* 0CC */ 0x0F, 0x84, 0x21, 0x00, 0x00, 0x00,                      /* jz   teardown (0F3)                        */
    /* 0D2 */ 0x8B, 0x15, 0xD0, 0xEE, 0x59, 0x00,                      /* mov  edx, [0x59EED0]  ; hwnd               */
    /* 0D8 */ 0x8B, 0x08,                                              /* mov  ecx, [eax]      ; vtbl                */
    /* 0DA */ 0x6A, 0x00,                                              /* push 0                                     */
    /* 0DC */ 0x68, 0x0D, 0x04, 0x00, 0x00,                            /* push 0x40D                                 */
    /* 0E1 */ 0x52,                                                    /* push edx                                   */
    /* 0E2 */ 0x50,                                                    /* push eax                                   */
    /* 0E3 */ 0xFF, 0x51, 0x34,                                        /* call [ecx+0x34]      ; SetNotifyWindow     */
    /* 0E6 */ 0xB9, 0xD0, 0xEE, 0x59, 0x00,                            /* mov  ecx, 0x59EED0  ; just_run:            */
    /* 0EB */ 0xB8, 0x20, 0x23, 0x46, 0x00,                            /* mov  eax, 0x462320   ; FUN_00465f90        */
    /* 0F0 */ 0xFF, 0xD0,                                              /* call eax        ; Run()                    */
    /* 0F2 */ 0xC3,                                                    /* ret                  ; done:               */
    /* 0F3 */ 0xB9, 0xD0, 0xEE, 0x59, 0x00,                            /* mov  ecx, 0x59EED0  ; teardown:            */
    /* 0F8 */ 0xB8, 0x20, 0x22, 0x46, 0x00,                            /* mov  eax, 0x462220   ; FUN_00465e90        */
    /* 0FD */ 0xFF, 0xD0,                                              /* call eax                                   */
    /* 0FF */ 0xC7, 0x05, 0xC0, 0x70, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, /* mov  dword [0x5470C0], 0   ; give_up:   */
    /* 109 */ 0xC3                                                     /* ret                                        */
};

#else

#define SAM_IMM         0x00482402u   /* imm32 of PUSH 0x20006 @ 0x00482401   */
#define PATCH_ADDR      0x00465AE4u   /* imm32 of MOV [0x5363D8], 0x4657E0    */

#if defined(LANDER_NOCD)
#define CDCHECK_ADDR    0x00416320u
static const unsigned char g_cdcheck[] = {
    0xB8, 0x01, 0x00, 0x00, 0x00,   /* mov eax, 1 */
    0xC3                            /* ret        */
};
#endif

static const unsigned char g_handler[] = {
    /* 000 */ 0xA1, 0x30, 0x13, 0x56, 0x00,                            /* mov  eax, [0x561330]  ; this->pMediaEventEx */
    /* 005 */ 0x85, 0xC0,                                              /* test eax, eax                              */
    /* 007 */ 0x0F, 0x84, 0xF2, 0x00, 0x00, 0x00,                      /* jz   give_up (0FF)                         */
    /* 00D */ 0xA1, 0x2C, 0x13, 0x56, 0x00,                            /* mov  eax, [0x56132C]  ; this->pMediaControl */
    /* 012 */ 0x85, 0xC0,                                              /* test eax, eax                              */
    /* 014 */ 0x0F, 0x84, 0xE5, 0x00, 0x00, 0x00,                      /* jz   give_up (0FF)                         */
    /* 01A */ 0xB9, 0x88, 0x12, 0x56, 0x00,                            /* mov  ecx, 0x561288                         */
    /* 01F */ 0xB8, 0x30, 0x5C, 0x46, 0x00,                            /* mov  eax, 0x465C30   ; FUN_00465c30        */
    /* 024 */ 0xFF, 0xD0,                                              /* call eax        ; event pump               */
    /* 026 */ 0x85, 0xC0,                                              /* test eax, eax                              */
    /* 028 */ 0x0F, 0x85, 0xC4, 0x00, 0x00, 0x00,                      /* jnz  done (0F2)                            */
    /* 02E */ 0xB9, 0x88, 0x12, 0x56, 0x00,                            /* mov  ecx, 0x561288                         */
    /* 033 */ 0xB8, 0xE0, 0x5F, 0x46, 0x00,                            /* mov  eax, 0x465FE0   ; FUN_00465fe0        */
    /* 038 */ 0xFF, 0xD0,                                              /* call eax        ; Stop()                   */
    /* 03A */ 0xB9, 0x88, 0x12, 0x56, 0x00,                            /* mov  ecx, 0x561288                         */
    /* 03F */ 0x8B, 0x41, 0x08,                                        /* mov  eax, [ecx+8]    ; flag_0x8            */
    /* 042 */ 0x85, 0xC0,                                              /* test eax, eax                              */
    /* 044 */ 0x0F, 0x85, 0x9C, 0x00, 0x00, 0x00,                      /* jnz  just_run (0E6)                        */
    /* 04A */ 0x6A, 0x00,                                              /* push 0                                     */
    /* 04C */ 0xB9, 0x88, 0x12, 0x56, 0x00,                            /* mov  ecx, 0x561288                         */
    /* 051 */ 0xB8, 0x90, 0x59, 0x46, 0x00,                            /* mov  eax, 0x465990   ; FUN_00465990        */
    /* 056 */ 0xFF, 0xD0,                                              /* call eax        ; pick track               */
    /* 058 */ 0xB9, 0x88, 0x12, 0x56, 0x00,                            /* mov  ecx, 0x561288                         */
    /* 05D */ 0xB8, 0x90, 0x5E, 0x46, 0x00,                            /* mov  eax, 0x465E90   ; FUN_00465e90        */
    /* 062 */ 0xFF, 0xD0,                                              /* call eax        ; teardown                 */
    /* 064 */ 0xB9, 0x88, 0x12, 0x56, 0x00,                            /* mov  ecx, 0x561288                         */
    /* 069 */ 0xB8, 0xC0, 0x5C, 0x46, 0x00,                            /* mov  eax, 0x465CC0   ; FUN_00465cc0        */
    /* 06E */ 0xFF, 0xD0,                                              /* call eax        ; new graph                */
    /* 070 */ 0x85, 0xC0,                                              /* test eax, eax                              */
    /* 072 */ 0x0F, 0x85, 0x87, 0x00, 0x00, 0x00,                      /* jnz  give_up (0FF)                         */
    /* 078 */ 0x6A, 0x40,                                              /* push 0x40       ; cchWideChar              */
    /* 07A */ 0x68, 0xA0, 0x12, 0x56, 0x00,                            /* push 0x5612A0  ; this+0x18                 */
    /* 07F */ 0x6A, 0xFF,                                              /* push -1         ; cbMultiByte              */
    /* 081 */ 0x68, 0x50, 0x13, 0x56, 0x00,                            /* push 0x561350  ; ansi path                 */
    /* 086 */ 0x6A, 0x00,                                              /* push 0          ; dwFlags                  */
    /* 088 */ 0x6A, 0x00,                                              /* push 0          ; CodePage                 */
    /* 08A */ 0xFF, 0x15, 0x4C, 0x90, 0x48, 0x00,                      /* call [MultiByteToWideChar]                 */
    /* 090 */ 0xA1, 0x50, 0x14, 0x56, 0x00,                            /* mov  eax, [0x561450]                       */
    /* 095 */ 0x85, 0xC0,                                              /* test eax, eax                              */
    /* 097 */ 0x0F, 0x84, 0x14, 0x00, 0x00, 0x00,                      /* jz   simple (0B1)                          */
    /* 09D */ 0xB9, 0x88, 0x12, 0x56, 0x00,                            /* mov  ecx, 0x561288                         */
    /* 0A2 */ 0xB8, 0x70, 0x5D, 0x46, 0x00,                            /* mov  eax, 0x465D70   ; FUN_00465d70        */
    /* 0A7 */ 0xFF, 0xD0,                                              /* call eax        ; manual connect           */
    /* 0A9 */ 0x85, 0xC0,                                              /* test eax, eax                              */
    /* 0AB */ 0x0F, 0x84, 0x14, 0x00, 0x00, 0x00,                      /* jz   notify (0C5)                          */
    /* 0B1 */ 0xB9, 0x88, 0x12, 0x56, 0x00,                            /* mov  ecx, 0x561288  ; simple:              */
    /* 0B6 */ 0xB8, 0x50, 0x5D, 0x46, 0x00,                            /* mov  eax, 0x465D50   ; FUN_00465d50        */
    /* 0BB */ 0xFF, 0xD0,                                              /* call eax        ; RenderFile               */
    /* 0BD */ 0x85, 0xC0,                                              /* test eax, eax                              */
    /* 0BF */ 0x0F, 0x85, 0x2E, 0x00, 0x00, 0x00,                      /* jnz  teardown (0F3)                        */
    /* 0C5 */ 0xA1, 0x30, 0x13, 0x56, 0x00,                            /* mov  eax, [0x561330]  ; notify:            */
    /* 0CA */ 0x85, 0xC0,                                              /* test eax, eax                              */
    /* 0CC */ 0x0F, 0x84, 0x21, 0x00, 0x00, 0x00,                      /* jz   teardown (0F3)                        */
    /* 0D2 */ 0x8B, 0x15, 0x88, 0x12, 0x56, 0x00,                      /* mov  edx, [0x561288]  ; hwnd               */
    /* 0D8 */ 0x8B, 0x08,                                              /* mov  ecx, [eax]      ; vtbl                */
    /* 0DA */ 0x6A, 0x00,                                              /* push 0                                     */
    /* 0DC */ 0x68, 0x0D, 0x04, 0x00, 0x00,                            /* push 0x40D                                 */
    /* 0E1 */ 0x52,                                                    /* push edx                                   */
    /* 0E2 */ 0x50,                                                    /* push eax                                   */
    /* 0E3 */ 0xFF, 0x51, 0x34,                                        /* call [ecx+0x34]      ; SetNotifyWindow     */
    /* 0E6 */ 0xB9, 0x88, 0x12, 0x56, 0x00,                            /* mov  ecx, 0x561288  ; just_run:            */
    /* 0EB */ 0xB8, 0x90, 0x5F, 0x46, 0x00,                            /* mov  eax, 0x465F90   ; FUN_00465f90        */
    /* 0F0 */ 0xFF, 0xD0,                                              /* call eax        ; Run()                    */
    /* 0F2 */ 0xC3,                                                    /* ret                  ; done:               */
    /* 0F3 */ 0xB9, 0x88, 0x12, 0x56, 0x00,                            /* mov  ecx, 0x561288  ; teardown:            */
    /* 0F8 */ 0xB8, 0x90, 0x5E, 0x46, 0x00,                            /* mov  eax, 0x465E90   ; FUN_00465e90        */
    /* 0FD */ 0xFF, 0xD0,                                              /* call eax                                   */
    /* 0FF */ 0xC7, 0x05, 0xD8, 0x63, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, /* mov  dword [0x5363D8], 0   ; give_up:   */
    /* 109 */ 0xC3                                                     /* ret                                        */
};

#endif

/* feeble attempt to avoid CRT */
static void copy_bytes(void *dst, const void *src, SIZE_T n)
{
    volatile unsigned char *d = (volatile unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;

    while (n-- != 0) *d++ = *s++;
}

static BOOL poke(DWORD addr, const void *data, SIZE_T n)
{
    DWORD old = 0;

    if (!VirtualProtect((void *)addr, n, PAGE_EXECUTE_READWRITE, &old))
        return FALSE;

    copy_bytes((void *)addr, data, n);
    VirtualProtect((void *)addr, n, old, &old);
    return TRUE;
}

static BOOL poke_dword(DWORD addr, DWORD value)
{
    return poke(addr, &value, sizeof(value));
}

/* insanity... */
static DWORD probe_regsam(void)
{
    HKEY key = NULL;

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, LANDER_KEY, 0, SAM_READWRITE, &key) == ERROR_SUCCESS)
    {
        RegCloseKey(key);
        return SAM_READWRITE;
    }
    return SAM_READONLY;
}

static void apply_patches(void)
{
    void *cave;

    poke_dword(SAM_IMM, probe_regsam());

    cave = VirtualAlloc(NULL, sizeof(g_handler), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if(cave == NULL)
		return;

    copy_bytes(cave, g_handler, sizeof(g_handler));
    poke_dword(PATCH_ADDR, (DWORD)cave);

#if defined(LANDER_NOCD)
    poke(CDCHECK_ADDR, g_cdcheck, sizeof(g_cdcheck));
#endif
}

/* dinput wrapper */
typedef HRESULT (WINAPI *PFN_DirectInputCreateA)(HINSTANCE, DWORD, void **, void *);

static PFN_DirectInputCreateA g_real;

HRESULT WINAPI DirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, void **ppDI, void *punkOuter)
{
    if(g_real == NULL)
    {
        char path[MAX_PATH];
        UINT n = GetSystemDirectoryA(path, MAX_PATH);
        HMODULE real;

        lstrcatA(path, "\\dinput.dll");

        real = LoadLibraryA(path);
        if(real == NULL)
            return E_FAIL;

        g_real = (PFN_DirectInputCreateA)GetProcAddress(real, "DirectInputCreateA");
        if(g_real == NULL)
            return E_FAIL;
    }
    return g_real(hinst, dwVersion, ppDI, punkOuter);
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved)
{
    (void)reserved;

    if(reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hinst);
        apply_patches();
    }
    return TRUE;
}
