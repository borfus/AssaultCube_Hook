#include <Windows.h>

#define define_dll_read_memory(T) \
T dll_read_memory_##T(LPVOID address) { \
	return *((T*)address); \
}

#define define_dll_write_memory(T) \
void dll_write_memory_##T(LPVOID address, T val) { \
	*((T*)address) = val; \
}

#define define_dll_read_memory_pointer(T) \
T* dll_read_memory_pointer_##T(LPVOID address) { \
	return ((T*)address); \
}

#define define_dll_protect_memory(T) \
DWORD dll_protect_memory_##T(LPVOID address, DWORD prot) { \
	DWORD old_prot; \
	VirtualProtect(address, sizeof(T), prot, &old_prot); \
	return old_prot; \
}

DWORD dll_protect_memory_bytes(LPVOID address, DWORD prot, int size) {
	DWORD old_prot;
	VirtualProtect(address, size, prot, &old_prot);
	return old_prot;
}

void dll_write_memory_bytes(LPVOID address, BYTE* val, int size) {
	DWORD old_prot = dll_protect_memory_bytes((LPVOID)address, PAGE_EXECUTE_READWRITE, size);

	for (int i = 0; i < size; i++) {
		*((BYTE*)(address)+i) = *(val + i);
	}

	dll_protect_memory_bytes((LPVOID)address, old_prot, size);
}

void dll_nop_memory(DWORD address, int size) {
	DWORD old_prot = dll_protect_memory_bytes((LPVOID)address, PAGE_EXECUTE_READWRITE, size);
	BYTE nop_byte = 0x90;

	for (int i = 0; i < size; i++) {	
		dll_write_memory_bytes((LPVOID)(address + i), &nop_byte, 1);
	}

	dll_protect_memory_bytes((LPVOID)address, old_prot, size);
}

DWORD* dll_jump_through_pointers(DWORD* pointer_list, int list_size) {
	DWORD current_value = *(DWORD*)*pointer_list;

	for (int i = 1; i < list_size; i++) {
		current_value = *(DWORD*)(current_value + *(DWORD*)(pointer_list + i));
	}

	return (DWORD*)current_value;
}

#define dll_read_memory(T) dll_read_memory_##T
#define dll_write_memory(T) dll_write_memory_##T
#define dll_read_memory_pointer(T) dll_read_memory_pointer_##T
#define dll_protect_memory(T) dll_protect_memory_##T

define_dll_read_memory(DWORD)
define_dll_write_memory(DWORD)
define_dll_write_memory(BYTE)
define_dll_read_memory_pointer(DWORD)
define_dll_protect_memory(DWORD)

// EXAMPLES
// DWORD val = dll_read_memory(DWORD)(adr);
// dll_write_memory(DWORD)(adr, val);

// Pointer ex
// DWORD* val_pointer = dll_read_memory_pointer(DWORD)(address); 
// DWORD value = *val_pointer;  // 'read'
// (*val_pointer)++;			// 'write'

// dll_nop_memory(address, 4);
