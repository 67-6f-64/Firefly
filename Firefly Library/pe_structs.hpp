#pragma once

#pragma pack(push, 1)
/* DOS header */
typedef struct _dos_header
{
	unsigned __int16 magic;					// magic number
	unsigned __int16 last_page_bytes;		// bytes on last page of file
	unsigned __int16 page_count;			// pages in file
	unsigned __int16 relocation_count;		// relocations in file
	unsigned __int16 paragraph_header_size;	// size of header in paragraphs
	unsigned __int16 minimum_paragraphs;	// minimum extra paragraphs needed
	unsigned __int16 maximum_paragraphs;	// maximum extra paragraphs needed
	unsigned __int16 initial_ss_value;		// initial (relative) SS value
	unsigned __int16 initial_sp_value;		// initial SP value
	unsigned __int16 checksum;				// checksum
	unsigned __int16 initial_ip_value;		// initial IP value
	unsigned __int16 initial_cs_value;		// initial (relative) CS value
	unsigned __int16 relocation_offset;		// file address of relocation table
	unsigned __int16 overlay_number;		// overlay number
	unsigned __int16 res_1[4];				// reserved words
	unsigned __int16 oem_id;				// OEM identifier (for x_13)
	unsigned __int16 oem_info;				// OEM information; x_12 specific
	unsigned __int16 res_2[10];				// reserved words
	unsigned __int32 pe_offset;				// file address of exe header
} dos_header;
#pragma pack(pop)

/* PE header */
#define file_executable_image 0x0002		// File is executable  (i.e. no unresolved external references).
#define file_dll 0x2000						// File is a DLL.

typedef struct _pe_header
{
	unsigned __int32 signature;
	unsigned __int16 machine;				// processor (ARM, MIPS, intel, etc)
	unsigned __int16 section_count;			// number of sections
	unsigned __int32 time_date_stamp;		// timedatestamp
	unsigned __int32 symbol_pointer;		// pointer to symbol table
	unsigned __int32 symbol_count;			// number of symbols
	unsigned __int16 optional_header_size;	// size of optional header
	unsigned __int16 characteristics;
} pe_header;

/* optional header */
#define number_of_directory_entries		16

#define directory_entry_export			0	// export table
#define directory_entry_import			1	// import table
#define directory_entry_basereloc		5	// base relocation table
#define directory_entry_tls				9	// TLS directory

typedef struct _data_directory
{
	unsigned __int32 virtual_address;
	unsigned __int32 size;
} data_directory;

typedef struct _optional_header
{
	unsigned __int16 magic;
	unsigned __int8 major_linker_version;	// major linker version
	unsigned __int8 minor_linker_version;	// minor linker version
	unsigned __int32 code_size;				// size of code
	unsigned __int32 initialized_size;		// size of initialized data
	unsigned __int32 uninitialized_size;	// size of uninitialized data
	unsigned __int32 entry_point_address;	// address of entry point
	unsigned __int32 code_base;				// base of code
	unsigned __int32 data_base;				// base of data
	unsigned __int32 image_base;			// base of image
	unsigned __int32 section_alignment;		// section alignment
	unsigned __int32 file_alignment;		// file alignment
	unsigned __int16 major_os_version;		// major operating system version
	unsigned __int16 minor_os_version;		// minor operating system version
	unsigned __int16 major_image_version;
	unsigned __int16 minor_image_version;
	unsigned __int16 major_subsystem_version;
	unsigned __int16 minor_subsystem_version;
	unsigned __int32 win32_version;			// win32 version value
	unsigned __int32 image_size;			// size of image
	unsigned __int32 headers_size;			// size of headers
	unsigned __int32 checksum;				// checksum
	unsigned __int16 subsystem;				// subsystem
	unsigned __int16 dll_characteristics;	// dll characteristics
	unsigned __int32 stack_reserve_size;	// size of stack reserve
	unsigned __int32 stack_commit_size;		// size of stack commit
	unsigned __int32 heap_reserve_size;		// size of heap reserve
	unsigned __int32 heap_commit_size;		// size of heap commit
	unsigned __int32 load_flags;			// load flags
	unsigned __int32 data_directory_count;	// number of RVA and sizes
	data_directory data_directory[number_of_directory_entries];
} optional_header;

/* nt headers */
typedef struct _nt_header
{
	pe_header file_header;
	optional_header optional_header;
} nt_header;

/* section header */
typedef struct _section_header
{
	unsigned __int8 name[8];

	union
	{
		unsigned __int32 physical_address;	// physical address
		unsigned __int32 virtual_size;		// virtual size
	} misc;

	unsigned __int32 virtual_address;		// virtual address
	unsigned __int32 raw_data_size;			// size of raw data
	unsigned __int32 raw_data_pointer;		// pointer to raw data
	unsigned __int32 relocations_pointer;	// pointer to relocations
	unsigned __int32 line_numbers_pointer;	// pointer to line numbers
	unsigned __int16 relocation_count;		// number of relocations
	unsigned __int16 line_number_count;		// number of line numbers
	unsigned __int32 characteristics;
} section_header;

/* export directory */
typedef struct _export_directory
{
	unsigned __int32 characteristics;
	unsigned __int32 time_date_stamp;
	unsigned __int16 major_version;
	unsigned __int16 minor_version;
	unsigned __int32 name;
	unsigned __int32 base;
	unsigned __int32 function_count;		// number of functions
	unsigned __int32 name_count;			// number of names
	unsigned __int32 functions_address;		// address of functions (RVA from base of image)
	unsigned __int32 names_address;			// address of names (RVA from base of image)
	unsigned __int32 name_ordinals_address;	// address of name ordinals (RVA from base of image)
} export_directory;

/* import directory */
typedef struct _import_by_name
{
	unsigned __int16 hint;
	unsigned __int8 name[1];
} import_by_name;

typedef struct _thunk_data
{
	union
	{
		unsigned __int32 forwarder_string;	// unsigned char*
		unsigned __int32 function;			// unsigned int*
		unsigned __int32 ordinal;
		unsigned __int32 data_address;		// address of data (import_by_name*)
	};
} thunk_data;

typedef struct _import_descriptor
{
	union
	{
		unsigned __int32 characteristics;	// 0 for terminating null import descriptor
		unsigned __int32 original_first_thunk;	// RVA to original unbound IAT (thunk_data_32*/thunk_data_64*)
	};

	unsigned __int32 time_date_stamp;		// 0 if not bound | 1 if bound, and real date/time stamp in IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT (new BIND) | O.W. date/time stamp of DLL bound to (old BIND)
	unsigned __int32 forwarder_chain;		// -1 if no forwarders
	unsigned __int32 name;
	unsigned __int32 first_thunk;			// RVA to IAT (if bound this IAT has actual addresses)
} import_descriptor;

/* tls */
typedef void(__stdcall* tls_callback_t)(void* DllHandle, unsigned int Reason, void* Reserved);

typedef struct _tls_directory
{
	unsigned __int32 raw_data_start_address;
	unsigned __int32 raw_data_end_address;
	unsigned __int32 index_address;					// unsigned int*
	unsigned __int32 callback_address;				// tls_callback_t*
	unsigned __int32 zero_fill_size;

	union
	{
		unsigned __int32 characteristics;

		struct
		{
			unsigned __int32 reserved_01 : 20;
			unsigned __int32 alignment : 4;
			unsigned __int32 reserved_02 : 8;
		};
	};
} tls_directory;

/* peb */
typedef union _large_integer
{
	struct
	{
		unsigned __int32 low_part;
		unsigned __int32 high_part;
	};

	unsigned __int64 quad_part;
} large_integer;

typedef struct _unicode_string
{
	unsigned __int16 length;
	unsigned __int16 maximum_length;
	wchar_t* buffer;
} unicode_string;

typedef struct _list_entry
{
	struct _list_entry* flink;
	struct _list_entry* blink;
} list_entry;

typedef struct _loaded_image
{
	char* module_name;
	void* file_handle;
	unsigned char* mapped_address;
	nt_header* file_header;
	section_header* last_rva_section;
	unsigned __int32 section_count;			// number of sections
	section_header* sections;
	unsigned __int32 characteristics;
	unsigned __int8 system_image;
	unsigned __int8 dos_image;
	unsigned __int8 read_only;
	unsigned __int8 version;
	list_entry links;
	unsigned __int32 image_size;			// size of image
} loaded_image;

typedef struct _rtl_drive_letter_curdir
{
	unsigned __int16 flags;
	unsigned __int16 length;
	unsigned __int32 time_stamp;
	unicode_string dos_path;
} rtl_drive_letter_curdir;

typedef struct _ldr_module
{
	list_entry in_load_order_module_list;
	list_entry in_memory_order_module_list;
	list_entry in_initialize_order_module_list;
	void* base_address;
	void* entry_point;
	unsigned __int32 image_size;
	unicode_string full_dll_name;
	unicode_string base_dll_name;
	unsigned __int32 flags;
	unsigned __int16 load_count;
	unsigned __int16 tls_index;
	list_entry hash_table_entry;
	unsigned __int32 time_date_stamp;
} ldr_module;

typedef struct _peb_ldr_data
{
	unsigned __int32 length;
	unsigned __int8 initialized[4];
	void* ss_handle;
	list_entry in_load_order_module_list;
	list_entry in_memory_order_module_list;
	list_entry in_initialize_order_module_list;
} peb_ldr_data;

typedef struct _rtl_user_process_parameters
{
	unsigned __int32 maximum_length;
	unsigned __int32 length;
	unsigned __int32 flags;
	unsigned __int32 debug_flags;
	void* console_handle;
	unsigned __int32 console_flags;
	void* std_input_handle;
	void* std_output_handle;
	void* std_error_handle;
	unicode_string current_directory_path;
	void* current_directory_handle;
	unicode_string dll_path;
	unicode_string image_path_name;
	unicode_string command_line;
	void* environment;
	unsigned __int32 starting_position_left;
	unsigned __int32 starting_position_top;
	unsigned __int32 width;
	unsigned __int32 height;
	unsigned __int32 char_width;
	unsigned __int32 char_height;
	unsigned __int32 console_text_attributes;
	unsigned __int32 window_flags;
	unsigned __int32 show_window_flags;
	unicode_string window_title;
	unicode_string desktop_name;
	unicode_string shell_info;
	unicode_string runtime_data;
	rtl_drive_letter_curdir drive_letter_current_directory[32];
} rtl_user_process_parameters;

typedef void(*peb_lock_routine_t)(void* peb_lock);

typedef struct _peb_free_block
{
	struct _peb_free_block* next;
	unsigned __int32 size;
} peb_free_block;

typedef struct _peb
{
	unsigned __int8 inherited_address_space;
	unsigned __int8 read_image_file_exec_options;
	unsigned __int8 being_debugged;
	unsigned __int8 spare;
	void* mutant;
	void* image_base_address;
	peb_ldr_data* loader_data;
	_rtl_user_process_parameters* process_parameters;
	void* sub_system_data;
	void* process_heap;
	void* fast_peb_lock;
	peb_lock_routine_t fast_peb_lock_routine;
	peb_lock_routine_t fast_peb_unlock_routine;
	unsigned __int32 environment_update_count;
	void** kernel_callback_table;
	void* event_log_section;
	void* event_log;
	peb_free_block* free_list;
	unsigned __int32 tls_expansion_counter;
	void* tls_bitmap;
	unsigned __int32 tls_bitmap_bits[2];
	void* read_only_shared_memory_base;
	void* read_only_shared_memory_heap;
	void** read_only_static_server_data;
	void* ansi_code_page_data;
	void* oem_code_page_data;
	void* unicode_case_table_data;
	unsigned __int32 number_of_processors;
	unsigned __int32 nt_global_flag;
	unsigned __int8 spare2[4];
	large_integer critical_section_timeout;
	unsigned __int32 heap_segment_reserve;
	unsigned __int32 heap_segment_commit;
	unsigned __int32 heap_de_commit_total_free_treshold;
	unsigned __int32 heap_de_commit_free_block_treshold;
	unsigned __int32 number_of_heaps;
	unsigned __int32 maximum_number_of_heaps;
	void*** process_heaps;
	void* gdi_shared_handle_table;
	void* process_starter_helper;
	void* gdi_dc_attribute_list;
	void* loader_lock;
	unsigned __int32 os_major_version;
	unsigned __int32 os_minor_version;
	unsigned __int32 os_build_number;
	unsigned __int32 os_platform_id;
	unsigned __int32 image_sub_system;
	unsigned __int32 image_sub_system_major_version;
	unsigned __int32 image_sub_system_minor_version;
	unsigned __int32 gdi_handle_buffer[0x22];
	unsigned __int32 post_process_init_routine;
	unsigned __int32 tls_expansion_bitmap;
	unsigned __int8 tls_expansion_bitmap_bits[0x80];
	unsigned __int32 session_id;
} peb;

typedef struct _base_relocation_block
{
	unsigned __int32 page_address;
	unsigned __int32 block_size;
} base_relocation_block;

typedef struct _base_relocation_entry
{
	unsigned __int16 offset : 12;
	unsigned __int16 type : 4;
} base_relocation_entry;

typedef struct _base_relocation
{
	unsigned __int32 virtual_address;
	unsigned __int32 block_size;
} base_relocation;

#define count_relocation_entries(block_size) (block_size - sizeof(base_relocation_block)) / sizeof(base_relocation_entry)