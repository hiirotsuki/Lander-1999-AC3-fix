#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* sha256sum a689ebc324740e49caeec04690ad63f037a6da70cd33e65ddd22c498388f8bb5 */
#define PATCH_ADDR      0x00465AE4u   /* imm32 of MOV [0x5363d8], 0x4657e0 @ 0x465ADE */

#define SAM_IMM         0x00482402u
#define SAM_READWRITE   0x0002001Fu
#define SAM_READONLY    0x00020019u

static const unsigned char g_shellcode[] = {
    /* 000 */ 0xA1, 0x30, 0x13, 0x56, 0x00,                  /* mov  eax, [0x561330]   ; this->pMediaEventEx */
    /* 005 */ 0x85, 0xC0,                                    /* test eax, eax                      */
    /* 007 */ 0x0F, 0x84, 0xF2, 0x00, 0x00, 0x00,            /* jz   give_up (0FF)                 */
    /* 00D */ 0xA1, 0x2C, 0x13, 0x56, 0x00,                  /* mov  eax, [0x56132C]   ; this->pMediaControl */
    /* 012 */ 0x85, 0xC0,                                    /* test eax, eax                      */
    /* 014 */ 0x0F, 0x84, 0xE5, 0x00, 0x00, 0x00,            /* jz   give_up (0FF)                 */
    /* 01A */ 0xB9, 0x88, 0x12, 0x56, 0x00,                  /* mov  ecx, 0x561288                 */
    /* 01F */ 0xB8, 0x30, 0x5C, 0x46, 0x00,                  /* mov  eax, FUN_00465c30             */
    /* 024 */ 0xFF, 0xD0,                                    /* call eax        ; event pump       */
    /* 026 */ 0x85, 0xC0,                                    /* test eax, eax                      */
    /* 028 */ 0x0F, 0x85, 0xC4, 0x00, 0x00, 0x00,            /* jnz  done (0F2)                    */
    /* 02E */ 0xB9, 0x88, 0x12, 0x56, 0x00,                  /* mov  ecx, 0x561288                 */
    /* 033 */ 0xB8, 0xE0, 0x5F, 0x46, 0x00,                  /* mov  eax, FUN_00465fe0             */
    /* 038 */ 0xFF, 0xD0,                                    /* call eax        ; Stop()           */
    /* 03A */ 0xB9, 0x88, 0x12, 0x56, 0x00,                  /* mov  ecx, 0x561288                 */
    /* 03F */ 0x8B, 0x41, 0x08,                              /* mov  eax, [ecx+8]                  */
    /* 042 */ 0x85, 0xC0,                                    /* test eax, eax                      */
    /* 044 */ 0x0F, 0x85, 0x9C, 0x00, 0x00, 0x00,            /* jnz  just_run (0E6)                */
    /* 04A */ 0x6A, 0x00,                                    /* push 0                             */
    /* 04C */ 0xB9, 0x88, 0x12, 0x56, 0x00,                  /* mov  ecx, 0x561288                 */
    /* 051 */ 0xB8, 0x90, 0x59, 0x46, 0x00,                  /* mov  eax, FUN_00465990             */
    /* 056 */ 0xFF, 0xD0,                                    /* call eax        ; pick track       */
    /* 058 */ 0xB9, 0x88, 0x12, 0x56, 0x00,                  /* mov  ecx, 0x561288                 */
    /* 05D */ 0xB8, 0x90, 0x5E, 0x46, 0x00,                  /* mov  eax, FUN_00465e90             */
    /* 062 */ 0xFF, 0xD0,                                    /* call eax        ; teardown         */
    /* 064 */ 0xB9, 0x88, 0x12, 0x56, 0x00,                  /* mov  ecx, 0x561288                 */
    /* 069 */ 0xB8, 0xC0, 0x5C, 0x46, 0x00,                  /* mov  eax, FUN_00465cc0             */
    /* 06E */ 0xFF, 0xD0,                                    /* call eax        ; new graph        */
    /* 070 */ 0x85, 0xC0,                                    /* test eax, eax                      */
    /* 072 */ 0x0F, 0x85, 0x87, 0x00, 0x00, 0x00,            /* jnz  give_up (0FF)                 */
    /* 078 */ 0x6A, 0x40,                                    /* push 0x40       ; cchWideChar      */
    /* 07A */ 0x68, 0xA0, 0x12, 0x56, 0x00,                  /* push 0x5612A0   ; this+0x18        */
    /* 07F */ 0x6A, 0xFF,                                    /* push -1         ; cbMultiByte      */
    /* 081 */ 0x68, 0x50, 0x13, 0x56, 0x00,                  /* push 0x561350   ; ansi path        */
    /* 086 */ 0x6A, 0x00,                                    /* push 0          ; dwFlags          */
    /* 088 */ 0x6A, 0x00,                                    /* push 0          ; CodePage         */
    /* 08A */ 0xFF, 0x15, 0x4C, 0x90, 0x48, 0x00,            /* call [MultiByteToWideChar]         */
    /* 090 */ 0xA1, 0x50, 0x14, 0x56, 0x00,                  /* mov  eax, [0x561450]               */
    /* 095 */ 0x85, 0xC0,                                    /* test eax, eax                      */
    /* 097 */ 0x0F, 0x84, 0x14, 0x00, 0x00, 0x00,            /* jz   simple (0B1)                  */
    /* 09D */ 0xB9, 0x88, 0x12, 0x56, 0x00,                  /* mov  ecx, 0x561288                 */
    /* 0A2 */ 0xB8, 0x70, 0x5D, 0x46, 0x00,                  /* mov  eax, FUN_00465d70             */
    /* 0A7 */ 0xFF, 0xD0,                                    /* call eax     ; manual connect      */
    /* 0A9 */ 0x85, 0xC0,                                    /* test eax, eax                      */
    /* 0AB */ 0x0F, 0x84, 0x14, 0x00, 0x00, 0x00,            /* jz   notify (0C5)                  */
    /* 0B1 */ 0xB9, 0x88, 0x12, 0x56, 0x00,                  /* mov  ecx, 0x561288   ; simple:     */
    /* 0B6 */ 0xB8, 0x50, 0x5D, 0x46, 0x00,                  /* mov  eax, FUN_00465d50             */
    /* 0BB */ 0xFF, 0xD0,                                    /* call eax     ; RenderFile          */
    /* 0BD */ 0x85, 0xC0,                                    /* test eax, eax                      */
    /* 0BF */ 0x0F, 0x85, 0x2E, 0x00, 0x00, 0x00,            /* jnz  teardown (0F3)                */
    /* 0C5 */ 0xA1, 0x30, 0x13, 0x56, 0x00,                  /* mov  eax, [0x561330] ; notify:     */
    /* 0CA */ 0x85, 0xC0,                                    /* test eax, eax                      */
    /* 0CC */ 0x0F, 0x84, 0x21, 0x00, 0x00, 0x00,            /* jz   teardown (0F3)                */
    /* 0D2 */ 0x8B, 0x15, 0x88, 0x12, 0x56, 0x00,            /* mov  edx, [0x561288] ; hwnd        */
    /* 0D8 */ 0x8B, 0x08,                                    /* mov  ecx, [eax]      ; vtbl        */
    /* 0DA */ 0x6A, 0x00,                                    /* push 0                             */
    /* 0DC */ 0x68, 0x0D, 0x04, 0x00, 0x00,                  /* push 0x40D                         */
    /* 0E1 */ 0x52,                                          /* push edx                           */
    /* 0E2 */ 0x50,                                          /* push eax                           */
    /* 0E3 */ 0xFF, 0x51, 0x34,                              /* call [ecx+0x34]      ; SetNotifyWindow */
    /* 0E6 */ 0xB9, 0x88, 0x12, 0x56, 0x00,                  /* mov  ecx, 0x561288   ; just_run:   */
    /* 0EB */ 0xB8, 0x90, 0x5F, 0x46, 0x00,                  /* mov  eax, FUN_00465f90             */
    /* 0F0 */ 0xFF, 0xD0,                                    /* call eax        ; Run()            */
    /* 0F2 */ 0xC3,                                          /* ret                  ; done:       */
    /* 0F3 */ 0xB9, 0x88, 0x12, 0x56, 0x00,                  /* mov  ecx, 0x561288   ; teardown:   */
    /* 0F8 */ 0xB8, 0x90, 0x5E, 0x46, 0x00,                  /* mov  eax, FUN_00465e90             */
    /* 0FD */ 0xFF, 0xD0,                                    /* call eax                           */
    /* 0FF */ 0xC7, 0x05, 0xD8, 0x63, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, /* mov  dword [0x5363d8], 0   ; give_up: */
    /* 109 */ 0xC3                                           /* ret                                */
};

static HANDLE g_proc = NULL;

static void bail(void)
{
    if (g_proc)
        TerminateProcess(g_proc, 1);
    exit(1);
}

static void fail(const char *msg)
{
    fprintf(stderr, "lander_ac3_fix: %s (GetLastError=%lu)\n", msg, GetLastError());
    bail();
}

static void poke(HANDLE proc, DWORD addr, const void *data, SIZE_T n, const char *what)
{
    SIZE_T written = 0;

    if (!WriteProcessMemory(proc, (void *)(uintptr_t)addr, data, n, &written) || written != n)
        fail(what);
}

static void poke_dword(HANDLE proc, DWORD addr, DWORD value, const char *what)
{
    poke(proc, addr, &value, sizeof(value), what);
}

static int is_elevated(void)
{
    HANDLE tok = NULL;
    TOKEN_ELEVATION el;
    DWORD cb = sizeof(el);
    int result = -1;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tok))
	{
        if (GetTokenInformation(tok, TokenElevation, &el, cb, &cb))
            result = el.TokenIsElevated ? 1 : 0;

        CloseHandle(tok);
    }
    return result;
}

int main(int argc, char **argv)
{
    char exePath[MAX_PATH];
    char workDir[MAX_PATH];
    char cmdline[MAX_PATH * 2];
    char *fileName = NULL;
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    void *cave;
    DWORD caveAddr, sam, exitCode = 0;
    int i, elevated;

    if (argc < 2)
	{
        fprintf(stderr, "usage: %s <path to LANDER.EXE> [game args...]\n", argv[0]);
        return 1;
    }

    if (!GetFullPathNameA(argv[1], MAX_PATH, exePath, &fileName) || fileName == NULL)
	{
        fail("GetFullPathName failed");
    }
    strcpy(workDir, exePath);
    workDir[fileName - exePath] = '\0';   /* strip the filename, keep the '\' */

    snprintf(cmdline, sizeof(cmdline), "\"%s\"", exePath);
    for (i = 2; i < argc; i++)
	{
        strncat(cmdline, " ", sizeof(cmdline) - strlen(cmdline) - 1);
        strncat(cmdline, argv[i], sizeof(cmdline) - strlen(cmdline) - 1);
    }

    /* The registry key lives under HKLM, so keeping KEY_SET_VALUE (which lets
     * the game save its settings on exit) only works with an admin token. When
     * we are not elevated, drop to plain KEY_READ: reading correctly is what
     * keeps the game from crashing, and the write would have failed anyway. */
    elevated = is_elevated();
    sam = (elevated == 1) ? SAM_READWRITE : SAM_READONLY;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessA(exePath, cmdline, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, workDir, &si, &pi))
	{
        fail("CreateProcess failed");
    }
    g_proc = pi.hProcess;

    poke_dword(pi.hProcess, SAM_IMM, sam, "registry samDesired patch failed");

    cave = VirtualAllocEx(pi.hProcess, NULL, sizeof(g_shellcode), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!cave) fail("VirtualAllocEx failed");

    caveAddr = (DWORD)(uintptr_t)cave;
    poke(pi.hProcess, caveAddr, g_shellcode, sizeof(g_shellcode), "shellcode write failed");
    poke_dword(pi.hProcess, PATCH_ADDR, caveAddr, "handler redirect failed");

    printf("lander_ac3_fix: exe      %s\n", exePath);
    printf("lander_ac3_fix: cwd      %s\n", workDir);
    printf("lander_ac3_fix: regsam   0x%08lX (%s)%s\n", (unsigned long)sam,
		(sam == SAM_READWRITE) ? "read+write, settings will be saved" : "read-only, settings will NOT be saved", (elevated == 1) ? "" : " [not elevated]");
    printf("lander_ac3_fix: cave     0x%08lX (%u bytes)\n", (unsigned long)caveAddr, (unsigned)sizeof(g_shellcode));
    printf("lander_ac3_fix: redirect 0x%08X -> 0x%08lX\n", PATCH_ADDR, (unsigned long)caveAddr);
    fflush(stdout);

    ResumeThread(pi.hThread);

    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &exitCode);
    if (exitCode >= 0xC0000000u) {
        printf("lander_ac3_fix: game terminated with exception 0x%08lX\n",
               (unsigned long)exitCode);
    } else {
        printf("lander_ac3_fix: game exited with code %lu\n", (unsigned long)exitCode);
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return 0;
}
