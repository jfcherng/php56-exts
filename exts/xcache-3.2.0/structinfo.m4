define(`ELEMENTSOF__IO_marker', `"_next","_sbuf","_pos"')
define(`COUNTOF__IO_marker', `3')
define(`SIZEOF__IO_marker', `(  sizeof(((_IO_marker*)NULL)->_next) + sizeof(((_IO_marker*)NULL)->_sbuf) + sizeof(((_IO_marker*)NULL)->_pos)  )')


define(`ELEMENTSOF___FILE', `"_flags","_IO_read_ptr","_IO_read_end","_IO_read_base","_IO_write_base","_IO_write_ptr","_IO_write_end","_IO_buf_base","_IO_buf_end","_IO_save_base","_IO_backup_base","_IO_save_end","_markers","_chain","_fileno","_flags2","_old_offset","_cur_column","_vtable_offset","_shortbuf","_lock","_offset","__pad1","__pad2","__pad3","__pad4","__pad5","_mode","_unused2"')
define(`COUNTOF___FILE', `29')
define(`SIZEOF___FILE', `(  sizeof(((__FILE*)NULL)->_flags) + sizeof(((__FILE*)NULL)->_IO_read_ptr) + sizeof(((__FILE*)NULL)->_IO_read_end) + sizeof(((__FILE*)NULL)->_IO_read_base) + sizeof(((__FILE*)NULL)->_IO_write_base) + sizeof(((__FILE*)NULL)->_IO_write_ptr) + sizeof(((__FILE*)NULL)->_IO_write_end) + sizeof(((__FILE*)NULL)->_IO_buf_base) + sizeof(((__FILE*)NULL)->_IO_buf_end) + sizeof(((__FILE*)NULL)->_IO_save_base) + sizeof(((__FILE*)NULL)->_IO_backup_base) + sizeof(((__FILE*)NULL)->_IO_save_end) + sizeof(((__FILE*)NULL)->_markers) + sizeof(((__FILE*)NULL)->_chain) + sizeof(((__FILE*)NULL)->_fileno) + sizeof(((__FILE*)NULL)->_flags2) + sizeof(((__FILE*)NULL)->_old_offset) + sizeof(((__FILE*)NULL)->_cur_column) + sizeof(((__FILE*)NULL)->_vtable_offset) + sizeof(((__FILE*)NULL)->_shortbuf) + sizeof(((__FILE*)NULL)->_lock) + sizeof(((__FILE*)NULL)->_offset) + sizeof(((__FILE*)NULL)->__pad1) + sizeof(((__FILE*)NULL)->__pad2) + sizeof(((__FILE*)NULL)->__pad3) + sizeof(((__FILE*)NULL)->__pad4) + sizeof(((__FILE*)NULL)->__pad5) + sizeof(((__FILE*)NULL)->_mode) + sizeof(((__FILE*)NULL)->_unused2)  )')


define(`ELEMENTSOF_zend_leak_info', `"addr","size","filename","lineno","orig_filename","orig_lineno"')
define(`COUNTOF_zend_leak_info', `6')
define(`SIZEOF_zend_leak_info', `(  sizeof(((zend_leak_info*)NULL)->addr) + sizeof(((zend_leak_info*)NULL)->size) + sizeof(((zend_leak_info*)NULL)->filename) + sizeof(((zend_leak_info*)NULL)->lineno) + sizeof(((zend_leak_info*)NULL)->orig_filename) + sizeof(((zend_leak_info*)NULL)->orig_lineno)  )')


define(`ELEMENTSOF_zend_mm_segment', `"size","next_segment"')
define(`COUNTOF_zend_mm_segment', `2')
define(`SIZEOF_zend_mm_segment', `(  sizeof(((zend_mm_segment*)NULL)->size) + sizeof(((zend_mm_segment*)NULL)->next_segment)  )')


define(`ELEMENTSOF_zend_mm_mem_handlers', `"name","init","dtor","compact","_alloc","_realloc","_free"')
define(`COUNTOF_zend_mm_mem_handlers', `7')
define(`SIZEOF_zend_mm_mem_handlers', `(  sizeof(((zend_mm_mem_handlers*)NULL)->name) + sizeof(((zend_mm_mem_handlers*)NULL)->init) + sizeof(((zend_mm_mem_handlers*)NULL)->dtor) + sizeof(((zend_mm_mem_handlers*)NULL)->compact) + sizeof(((zend_mm_mem_handlers*)NULL)->_alloc) + sizeof(((zend_mm_mem_handlers*)NULL)->_realloc) + sizeof(((zend_mm_mem_handlers*)NULL)->_free)  )')


define(`ELEMENTSOF_zend_mm_storage', `"handlers","data"')
define(`COUNTOF_zend_mm_storage', `2')
define(`SIZEOF_zend_mm_storage', `(  sizeof(((zend_mm_storage*)NULL)->handlers) + sizeof(((zend_mm_storage*)NULL)->data)  )')


define(`ELEMENTSOF_zend_object_value', `"handle","handlers"')
define(`COUNTOF_zend_object_value', `2')
define(`SIZEOF_zend_object_value', `(  sizeof(((zend_object_value*)NULL)->handle) + sizeof(((zend_object_value*)NULL)->handlers)  )')


define(`ELEMENTSOF_Bucket', `"h","nKeyLength","pData","pDataPtr","pListNext","pListLast","pNext","pLast","arKey"')
define(`COUNTOF_Bucket', `9')
define(`SIZEOF_Bucket', `(  sizeof(((Bucket*)NULL)->h) + sizeof(((Bucket*)NULL)->nKeyLength) + sizeof(((Bucket*)NULL)->pData) + sizeof(((Bucket*)NULL)->pDataPtr) + sizeof(((Bucket*)NULL)->pListNext) + sizeof(((Bucket*)NULL)->pListLast) + sizeof(((Bucket*)NULL)->pNext) + sizeof(((Bucket*)NULL)->pLast) + sizeof(((Bucket*)NULL)->arKey)  )')


define(`ELEMENTSOF_HashTable', `"nTableSize","nTableMask","nNumOfElements","nNextFreeElement","pInternalPointer","pListHead","pListTail","arBuckets","pDestructor","persistent","nApplyCount","bApplyProtection"')
define(`COUNTOF_HashTable', `12')
define(`SIZEOF_HashTable', `(  sizeof(((HashTable*)NULL)->nTableSize) + sizeof(((HashTable*)NULL)->nTableMask) + sizeof(((HashTable*)NULL)->nNumOfElements) + sizeof(((HashTable*)NULL)->nNextFreeElement) + sizeof(((HashTable*)NULL)->pInternalPointer) + sizeof(((HashTable*)NULL)->pListHead) + sizeof(((HashTable*)NULL)->pListTail) + sizeof(((HashTable*)NULL)->arBuckets) + sizeof(((HashTable*)NULL)->pDestructor) + sizeof(((HashTable*)NULL)->persistent) + sizeof(((HashTable*)NULL)->nApplyCount) + sizeof(((HashTable*)NULL)->bApplyProtection)  )')


define(`ELEMENTSOF_zend_hash_key', `"arKey","nKeyLength","h"')
define(`COUNTOF_zend_hash_key', `3')
define(`SIZEOF_zend_hash_key', `(  sizeof(((zend_hash_key*)NULL)->arKey) + sizeof(((zend_hash_key*)NULL)->nKeyLength) + sizeof(((zend_hash_key*)NULL)->h)  )')


define(`ELEMENTSOF_HashPointer', `"pos","h"')
define(`COUNTOF_HashPointer', `2')
define(`SIZEOF_HashPointer', `(  sizeof(((HashPointer*)NULL)->pos) + sizeof(((HashPointer*)NULL)->h)  )')


define(`ELEMENTSOF_TsHashTable', `"hash","reader"')
define(`COUNTOF_TsHashTable', `2')
define(`SIZEOF_TsHashTable', `(  sizeof(((TsHashTable*)NULL)->hash) + sizeof(((TsHashTable*)NULL)->reader)  )')


define(`ELEMENTSOF_zend_llist_element', `"next","prev","data"')
define(`COUNTOF_zend_llist_element', `3')
define(`SIZEOF_zend_llist_element', `(  sizeof(((zend_llist_element*)NULL)->next) + sizeof(((zend_llist_element*)NULL)->prev) + sizeof(((zend_llist_element*)NULL)->data)  )')


define(`ELEMENTSOF_zend_llist', `"head","tail","count","size","dtor","persistent","traverse_ptr"')
define(`COUNTOF_zend_llist', `7')
define(`SIZEOF_zend_llist', `(  sizeof(((zend_llist*)NULL)->head) + sizeof(((zend_llist*)NULL)->tail) + sizeof(((zend_llist*)NULL)->count) + sizeof(((zend_llist*)NULL)->size) + sizeof(((zend_llist*)NULL)->dtor) + sizeof(((zend_llist*)NULL)->persistent) + sizeof(((zend_llist*)NULL)->traverse_ptr)  )')


define(`ELEMENTSOF_zend_guard', `"in_get","in_set","in_unset","in_isset","dummy"')
define(`COUNTOF_zend_guard', `5')
define(`SIZEOF_zend_guard', `(  sizeof(((zend_guard*)NULL)->in_get) + sizeof(((zend_guard*)NULL)->in_set) + sizeof(((zend_guard*)NULL)->in_unset) + sizeof(((zend_guard*)NULL)->in_isset) + sizeof(((zend_guard*)NULL)->dummy)  )')


define(`ELEMENTSOF_zend_object', `"ce","properties","properties_table","guards"')
define(`COUNTOF_zend_object', `4')
define(`SIZEOF_zend_object', `(  sizeof(((zend_object*)NULL)->ce) + sizeof(((zend_object*)NULL)->properties) + sizeof(((zend_object*)NULL)->properties_table) + sizeof(((zend_object*)NULL)->guards)  )')


define(`ELEMENTSOF_zend_object_handlers', `"add_ref","del_ref","clone_obj","read_property","write_property","read_dimension","write_dimension","get_property_ptr_ptr","get","set","has_property","unset_property","has_dimension","unset_dimension","get_properties","get_method","call_method","get_constructor","get_class_entry","get_class_name","compare_objects","cast_object","count_elements","get_debug_info","get_closure","get_gc","do_operation","compare"')
define(`COUNTOF_zend_object_handlers', `28')
define(`SIZEOF_zend_object_handlers', `(  sizeof(((zend_object_handlers*)NULL)->add_ref) + sizeof(((zend_object_handlers*)NULL)->del_ref) + sizeof(((zend_object_handlers*)NULL)->clone_obj) + sizeof(((zend_object_handlers*)NULL)->read_property) + sizeof(((zend_object_handlers*)NULL)->write_property) + sizeof(((zend_object_handlers*)NULL)->read_dimension) + sizeof(((zend_object_handlers*)NULL)->write_dimension) + sizeof(((zend_object_handlers*)NULL)->get_property_ptr_ptr) + sizeof(((zend_object_handlers*)NULL)->get) + sizeof(((zend_object_handlers*)NULL)->set) + sizeof(((zend_object_handlers*)NULL)->has_property) + sizeof(((zend_object_handlers*)NULL)->unset_property) + sizeof(((zend_object_handlers*)NULL)->has_dimension) + sizeof(((zend_object_handlers*)NULL)->unset_dimension) + sizeof(((zend_object_handlers*)NULL)->get_properties) + sizeof(((zend_object_handlers*)NULL)->get_method) + sizeof(((zend_object_handlers*)NULL)->call_method) + sizeof(((zend_object_handlers*)NULL)->get_constructor) + sizeof(((zend_object_handlers*)NULL)->get_class_entry) + sizeof(((zend_object_handlers*)NULL)->get_class_name) + sizeof(((zend_object_handlers*)NULL)->compare_objects) + sizeof(((zend_object_handlers*)NULL)->cast_object) + sizeof(((zend_object_handlers*)NULL)->count_elements) + sizeof(((zend_object_handlers*)NULL)->get_debug_info) + sizeof(((zend_object_handlers*)NULL)->get_closure) + sizeof(((zend_object_handlers*)NULL)->get_gc) + sizeof(((zend_object_handlers*)NULL)->do_operation) + sizeof(((zend_object_handlers*)NULL)->compare)  )')


define(`ELEMENTSOF_zend_ast', `"kind","children","u"')
define(`COUNTOF_zend_ast', `3')
define(`SIZEOF_zend_ast', `(  sizeof(((zend_ast*)NULL)->kind) + sizeof(((zend_ast*)NULL)->children) + sizeof(((zend_ast*)NULL)->u)  )')


define(`ELEMENTSOF_zval', `"value","refcount__gc","type","is_ref__gc"')
define(`COUNTOF_zval', `4')
define(`SIZEOF_zval', `(  sizeof(((zval*)NULL)->value) + sizeof(((zval*)NULL)->refcount__gc) + sizeof(((zval*)NULL)->type) + sizeof(((zval*)NULL)->is_ref__gc)  )')


define(`ELEMENTSOF_zend_object_iterator_funcs', `"dtor","valid","get_current_data","get_current_key","move_forward","rewind","invalidate_current"')
define(`COUNTOF_zend_object_iterator_funcs', `7')
define(`SIZEOF_zend_object_iterator_funcs', `(  sizeof(((zend_object_iterator_funcs*)NULL)->dtor) + sizeof(((zend_object_iterator_funcs*)NULL)->valid) + sizeof(((zend_object_iterator_funcs*)NULL)->get_current_data) + sizeof(((zend_object_iterator_funcs*)NULL)->get_current_key) + sizeof(((zend_object_iterator_funcs*)NULL)->move_forward) + sizeof(((zend_object_iterator_funcs*)NULL)->rewind) + sizeof(((zend_object_iterator_funcs*)NULL)->invalidate_current)  )')


define(`ELEMENTSOF_zend_object_iterator', `"data","funcs","index"')
define(`COUNTOF_zend_object_iterator', `3')
define(`SIZEOF_zend_object_iterator', `(  sizeof(((zend_object_iterator*)NULL)->data) + sizeof(((zend_object_iterator*)NULL)->funcs) + sizeof(((zend_object_iterator*)NULL)->index)  )')


define(`ELEMENTSOF_zend_class_iterator_funcs', `"funcs","zf_new_iterator","zf_valid","zf_current","zf_key","zf_next","zf_rewind"')
define(`COUNTOF_zend_class_iterator_funcs', `7')
define(`SIZEOF_zend_class_iterator_funcs', `(  sizeof(((zend_class_iterator_funcs*)NULL)->funcs) + sizeof(((zend_class_iterator_funcs*)NULL)->zf_new_iterator) + sizeof(((zend_class_iterator_funcs*)NULL)->zf_valid) + sizeof(((zend_class_iterator_funcs*)NULL)->zf_current) + sizeof(((zend_class_iterator_funcs*)NULL)->zf_key) + sizeof(((zend_class_iterator_funcs*)NULL)->zf_next) + sizeof(((zend_class_iterator_funcs*)NULL)->zf_rewind)  )')


define(`ELEMENTSOF__zend_trait_method_reference', `"method_name","mname_len","ce","class_name","cname_len"')
define(`COUNTOF__zend_trait_method_reference', `5')
define(`SIZEOF__zend_trait_method_reference', `(  sizeof(((_zend_trait_method_reference*)NULL)->method_name) + sizeof(((_zend_trait_method_reference*)NULL)->mname_len) + sizeof(((_zend_trait_method_reference*)NULL)->ce) + sizeof(((_zend_trait_method_reference*)NULL)->class_name) + sizeof(((_zend_trait_method_reference*)NULL)->cname_len)  )')


define(`ELEMENTSOF_zend_trait_method_reference', `"method_name","mname_len","ce","class_name","cname_len"')
define(`COUNTOF_zend_trait_method_reference', `5')
define(`SIZEOF_zend_trait_method_reference', `(  sizeof(((zend_trait_method_reference*)NULL)->method_name) + sizeof(((zend_trait_method_reference*)NULL)->mname_len) + sizeof(((zend_trait_method_reference*)NULL)->ce) + sizeof(((zend_trait_method_reference*)NULL)->class_name) + sizeof(((zend_trait_method_reference*)NULL)->cname_len)  )')
define(`ELEMENTSOF__zend_trait_precedence', `"trait_method","exclude_from_classes"')
define(`COUNTOF__zend_trait_precedence', `2')
define(`SIZEOF__zend_trait_precedence', `(  sizeof(((_zend_trait_precedence*)NULL)->trait_method) + sizeof(((_zend_trait_precedence*)NULL)->exclude_from_classes)  )')


define(`ELEMENTSOF_zend_trait_precedence', `"trait_method","exclude_from_classes"')
define(`COUNTOF_zend_trait_precedence', `2')
define(`SIZEOF_zend_trait_precedence', `(  sizeof(((zend_trait_precedence*)NULL)->trait_method) + sizeof(((zend_trait_precedence*)NULL)->exclude_from_classes)  )')
define(`ELEMENTSOF__zend_trait_alias', `"trait_method","alias","alias_len","modifiers"')
define(`COUNTOF__zend_trait_alias', `4')
define(`SIZEOF__zend_trait_alias', `(  sizeof(((_zend_trait_alias*)NULL)->trait_method) + sizeof(((_zend_trait_alias*)NULL)->alias) + sizeof(((_zend_trait_alias*)NULL)->alias_len) + sizeof(((_zend_trait_alias*)NULL)->modifiers)  )')


define(`ELEMENTSOF_zend_trait_alias', `"trait_method","alias","alias_len","modifiers"')
define(`COUNTOF_zend_trait_alias', `4')
define(`SIZEOF_zend_trait_alias', `(  sizeof(((zend_trait_alias*)NULL)->trait_method) + sizeof(((zend_trait_alias*)NULL)->alias) + sizeof(((zend_trait_alias*)NULL)->alias_len) + sizeof(((zend_trait_alias*)NULL)->modifiers)  )')
define(`ELEMENTSOF_zend_class_entry', `"type","name","name_length","parent","refcount","ce_flags","function_table","properties_info","default_properties_table","default_static_members_table","static_members_table","constants_table","default_properties_count","default_static_members_count","constructor","destructor","clone","__get","__set","__unset","__isset","__call","__callstatic","__tostring","__debugInfo","serialize_func","unserialize_func","iterator_funcs","create_object","get_iterator","interface_gets_implemented","get_static_method","serialize","unserialize","interfaces","num_interfaces","traits","num_traits","trait_aliases","trait_precedences","info"')
define(`COUNTOF_zend_class_entry', `41')
define(`SIZEOF_zend_class_entry', `(  sizeof(((zend_class_entry*)NULL)->type) + sizeof(((zend_class_entry*)NULL)->name) + sizeof(((zend_class_entry*)NULL)->name_length) + sizeof(((zend_class_entry*)NULL)->parent) + sizeof(((zend_class_entry*)NULL)->refcount) + sizeof(((zend_class_entry*)NULL)->ce_flags) + sizeof(((zend_class_entry*)NULL)->function_table) + sizeof(((zend_class_entry*)NULL)->properties_info) + sizeof(((zend_class_entry*)NULL)->default_properties_table) + sizeof(((zend_class_entry*)NULL)->default_static_members_table) + sizeof(((zend_class_entry*)NULL)->static_members_table) + sizeof(((zend_class_entry*)NULL)->constants_table) + sizeof(((zend_class_entry*)NULL)->default_properties_count) + sizeof(((zend_class_entry*)NULL)->default_static_members_count) + sizeof(((zend_class_entry*)NULL)->constructor) + sizeof(((zend_class_entry*)NULL)->destructor) + sizeof(((zend_class_entry*)NULL)->clone) + sizeof(((zend_class_entry*)NULL)->__get) + sizeof(((zend_class_entry*)NULL)->__set) + sizeof(((zend_class_entry*)NULL)->__unset) + sizeof(((zend_class_entry*)NULL)->__isset) + sizeof(((zend_class_entry*)NULL)->__call) + sizeof(((zend_class_entry*)NULL)->__callstatic) + sizeof(((zend_class_entry*)NULL)->__tostring) + sizeof(((zend_class_entry*)NULL)->__debugInfo) + sizeof(((zend_class_entry*)NULL)->serialize_func) + sizeof(((zend_class_entry*)NULL)->unserialize_func) + sizeof(((zend_class_entry*)NULL)->iterator_funcs) + sizeof(((zend_class_entry*)NULL)->create_object) + sizeof(((zend_class_entry*)NULL)->get_iterator) + sizeof(((zend_class_entry*)NULL)->interface_gets_implemented) + sizeof(((zend_class_entry*)NULL)->get_static_method) + sizeof(((zend_class_entry*)NULL)->serialize) + sizeof(((zend_class_entry*)NULL)->unserialize) + sizeof(((zend_class_entry*)NULL)->interfaces) + sizeof(((zend_class_entry*)NULL)->num_interfaces) + sizeof(((zend_class_entry*)NULL)->traits) + sizeof(((zend_class_entry*)NULL)->num_traits) + sizeof(((zend_class_entry*)NULL)->trait_aliases) + sizeof(((zend_class_entry*)NULL)->trait_precedences) + sizeof(((zend_class_entry*)NULL)->info)  )')


define(`ELEMENTSOF_zend_mmap', `"len","pos","map","buf","old_handle","old_closer"')
define(`COUNTOF_zend_mmap', `6')
define(`SIZEOF_zend_mmap', `(  sizeof(((zend_mmap*)NULL)->len) + sizeof(((zend_mmap*)NULL)->pos) + sizeof(((zend_mmap*)NULL)->map) + sizeof(((zend_mmap*)NULL)->buf) + sizeof(((zend_mmap*)NULL)->old_handle) + sizeof(((zend_mmap*)NULL)->old_closer)  )')


define(`ELEMENTSOF_zend_stream', `"handle","isatty","mmap","reader","fsizer","closer"')
define(`COUNTOF_zend_stream', `6')
define(`SIZEOF_zend_stream', `(  sizeof(((zend_stream*)NULL)->handle) + sizeof(((zend_stream*)NULL)->isatty) + sizeof(((zend_stream*)NULL)->mmap) + sizeof(((zend_stream*)NULL)->reader) + sizeof(((zend_stream*)NULL)->fsizer) + sizeof(((zend_stream*)NULL)->closer)  )')


define(`ELEMENTSOF_zend_file_handle', `"type","filename","opened_path","handle","free_filename"')
define(`COUNTOF_zend_file_handle', `5')
define(`SIZEOF_zend_file_handle', `(  sizeof(((zend_file_handle*)NULL)->type) + sizeof(((zend_file_handle*)NULL)->filename) + sizeof(((zend_file_handle*)NULL)->opened_path) + sizeof(((zend_file_handle*)NULL)->handle) + sizeof(((zend_file_handle*)NULL)->free_filename)  )')


define(`ELEMENTSOF_zend_utility_functions', `"error_function","printf_function","write_function","fopen_function","message_handler","block_interruptions","unblock_interruptions","get_configuration_directive","ticks_function","on_timeout","stream_open_function","vspprintf_function","getenv_function","resolve_path_function"')
define(`COUNTOF_zend_utility_functions', `14')
define(`SIZEOF_zend_utility_functions', `(  sizeof(((zend_utility_functions*)NULL)->error_function) + sizeof(((zend_utility_functions*)NULL)->printf_function) + sizeof(((zend_utility_functions*)NULL)->write_function) + sizeof(((zend_utility_functions*)NULL)->fopen_function) + sizeof(((zend_utility_functions*)NULL)->message_handler) + sizeof(((zend_utility_functions*)NULL)->block_interruptions) + sizeof(((zend_utility_functions*)NULL)->unblock_interruptions) + sizeof(((zend_utility_functions*)NULL)->get_configuration_directive) + sizeof(((zend_utility_functions*)NULL)->ticks_function) + sizeof(((zend_utility_functions*)NULL)->on_timeout) + sizeof(((zend_utility_functions*)NULL)->stream_open_function) + sizeof(((zend_utility_functions*)NULL)->vspprintf_function) + sizeof(((zend_utility_functions*)NULL)->getenv_function) + sizeof(((zend_utility_functions*)NULL)->resolve_path_function)  )')


define(`ELEMENTSOF_zend_utility_values', `"import_use_extension","import_use_extension_length","html_errors"')
define(`COUNTOF_zend_utility_values', `3')
define(`SIZEOF_zend_utility_values', `(  sizeof(((zend_utility_values*)NULL)->import_use_extension) + sizeof(((zend_utility_values*)NULL)->import_use_extension_length) + sizeof(((zend_utility_values*)NULL)->html_errors)  )')


define(`ELEMENTSOF_gc_root_buffer', `"prev","next","handle","u"')
define(`COUNTOF_gc_root_buffer', `4')
define(`SIZEOF_gc_root_buffer', `(  sizeof(((gc_root_buffer*)NULL)->prev) + sizeof(((gc_root_buffer*)NULL)->next) + sizeof(((gc_root_buffer*)NULL)->handle) + sizeof(((gc_root_buffer*)NULL)->u)  )')


define(`ELEMENTSOF_zval_gc_info', `"z","u"')
define(`COUNTOF_zval_gc_info', `2')
define(`SIZEOF_zval_gc_info', `(  sizeof(((zval_gc_info*)NULL)->z) + sizeof(((zval_gc_info*)NULL)->u)  )')


define(`ELEMENTSOF_zend_gc_globals', `"gc_enabled","gc_active","buf","roots","unused","first_unused","last_unused","zval_to_free","free_list","next_to_free","gc_runs","collected"')
define(`COUNTOF_zend_gc_globals', `12')
define(`SIZEOF_zend_gc_globals', `(  sizeof(((zend_gc_globals*)NULL)->gc_enabled) + sizeof(((zend_gc_globals*)NULL)->gc_active) + sizeof(((zend_gc_globals*)NULL)->buf) + sizeof(((zend_gc_globals*)NULL)->roots) + sizeof(((zend_gc_globals*)NULL)->unused) + sizeof(((zend_gc_globals*)NULL)->first_unused) + sizeof(((zend_gc_globals*)NULL)->last_unused) + sizeof(((zend_gc_globals*)NULL)->zval_to_free) + sizeof(((zend_gc_globals*)NULL)->free_list) + sizeof(((zend_gc_globals*)NULL)->next_to_free) + sizeof(((zend_gc_globals*)NULL)->gc_runs) + sizeof(((zend_gc_globals*)NULL)->collected)  )')


define(`ELEMENTSOF_zend_error_handling', `"handling","exception","user_handler"')
define(`COUNTOF_zend_error_handling', `3')
define(`SIZEOF_zend_error_handling', `(  sizeof(((zend_error_handling*)NULL)->handling) + sizeof(((zend_error_handling*)NULL)->exception) + sizeof(((zend_error_handling*)NULL)->user_handler)  )')


define(`ELEMENTSOF_zend_compiler_context', `"opcodes_size","vars_size","literals_size","current_brk_cont","backpatch_count","nested_calls","used_stack","in_finally","labels"')
define(`COUNTOF_zend_compiler_context', `9')
define(`SIZEOF_zend_compiler_context', `(  sizeof(((zend_compiler_context*)NULL)->opcodes_size) + sizeof(((zend_compiler_context*)NULL)->vars_size) + sizeof(((zend_compiler_context*)NULL)->literals_size) + sizeof(((zend_compiler_context*)NULL)->current_brk_cont) + sizeof(((zend_compiler_context*)NULL)->backpatch_count) + sizeof(((zend_compiler_context*)NULL)->nested_calls) + sizeof(((zend_compiler_context*)NULL)->used_stack) + sizeof(((zend_compiler_context*)NULL)->in_finally) + sizeof(((zend_compiler_context*)NULL)->labels)  )')


define(`ELEMENTSOF_zend_literal', `"constant","hash_value","cache_slot"')
define(`COUNTOF_zend_literal', `3')
define(`SIZEOF_zend_literal', `(  sizeof(((zend_literal*)NULL)->constant) + sizeof(((zend_literal*)NULL)->hash_value) + sizeof(((zend_literal*)NULL)->cache_slot)  )')


define(`ELEMENTSOF_znode', `"op_type","u","EA"')
define(`COUNTOF_znode', `3')
define(`SIZEOF_znode', `(  sizeof(((znode*)NULL)->op_type) + sizeof(((znode*)NULL)->u) + sizeof(((znode*)NULL)->EA)  )')


define(`ELEMENTSOF_zend_op', `"handler","op1","op2","result","extended_value","lineno","opcode","op1_type","op2_type","result_type"')
define(`COUNTOF_zend_op', `10')
define(`SIZEOF_zend_op', `(  sizeof(((zend_op*)NULL)->handler) + sizeof(((zend_op*)NULL)->op1) + sizeof(((zend_op*)NULL)->op2) + sizeof(((zend_op*)NULL)->result) + sizeof(((zend_op*)NULL)->extended_value) + sizeof(((zend_op*)NULL)->lineno) + sizeof(((zend_op*)NULL)->opcode) + sizeof(((zend_op*)NULL)->op1_type) + sizeof(((zend_op*)NULL)->op2_type) + sizeof(((zend_op*)NULL)->result_type)  )')


define(`ELEMENTSOF_zend_brk_cont_element', `"start","cont","brk","parent"')
define(`COUNTOF_zend_brk_cont_element', `4')
define(`SIZEOF_zend_brk_cont_element', `(  sizeof(((zend_brk_cont_element*)NULL)->start) + sizeof(((zend_brk_cont_element*)NULL)->cont) + sizeof(((zend_brk_cont_element*)NULL)->brk) + sizeof(((zend_brk_cont_element*)NULL)->parent)  )')


define(`ELEMENTSOF_zend_label', `"brk_cont","opline_num"')
define(`COUNTOF_zend_label', `2')
define(`SIZEOF_zend_label', `(  sizeof(((zend_label*)NULL)->brk_cont) + sizeof(((zend_label*)NULL)->opline_num)  )')


define(`ELEMENTSOF_zend_try_catch_element', `"try_op","catch_op","finally_op","finally_end"')
define(`COUNTOF_zend_try_catch_element', `4')
define(`SIZEOF_zend_try_catch_element', `(  sizeof(((zend_try_catch_element*)NULL)->try_op) + sizeof(((zend_try_catch_element*)NULL)->catch_op) + sizeof(((zend_try_catch_element*)NULL)->finally_op) + sizeof(((zend_try_catch_element*)NULL)->finally_end)  )')


define(`ELEMENTSOF_zend_property_info', `"flags","name","name_length","h","offset","doc_comment","doc_comment_len","ce"')
define(`COUNTOF_zend_property_info', `8')
define(`SIZEOF_zend_property_info', `(  sizeof(((zend_property_info*)NULL)->flags) + sizeof(((zend_property_info*)NULL)->name) + sizeof(((zend_property_info*)NULL)->name_length) + sizeof(((zend_property_info*)NULL)->h) + sizeof(((zend_property_info*)NULL)->offset) + sizeof(((zend_property_info*)NULL)->doc_comment) + sizeof(((zend_property_info*)NULL)->doc_comment_len) + sizeof(((zend_property_info*)NULL)->ce)  )')


define(`ELEMENTSOF_zend_arg_info', `"name","name_len","class_name","class_name_len","type_hint","pass_by_reference","allow_null","is_variadic"')
define(`COUNTOF_zend_arg_info', `8')
define(`SIZEOF_zend_arg_info', `(  sizeof(((zend_arg_info*)NULL)->name) + sizeof(((zend_arg_info*)NULL)->name_len) + sizeof(((zend_arg_info*)NULL)->class_name) + sizeof(((zend_arg_info*)NULL)->class_name_len) + sizeof(((zend_arg_info*)NULL)->type_hint) + sizeof(((zend_arg_info*)NULL)->pass_by_reference) + sizeof(((zend_arg_info*)NULL)->allow_null) + sizeof(((zend_arg_info*)NULL)->is_variadic)  )')


define(`ELEMENTSOF_zend_internal_function_info', `"_name","_name_len","_class_name","required_num_args","_type_hint","return_reference","_allow_null","_is_variadic"')
define(`COUNTOF_zend_internal_function_info', `8')
define(`SIZEOF_zend_internal_function_info', `(  sizeof(((zend_internal_function_info*)NULL)->_name) + sizeof(((zend_internal_function_info*)NULL)->_name_len) + sizeof(((zend_internal_function_info*)NULL)->_class_name) + sizeof(((zend_internal_function_info*)NULL)->required_num_args) + sizeof(((zend_internal_function_info*)NULL)->_type_hint) + sizeof(((zend_internal_function_info*)NULL)->return_reference) + sizeof(((zend_internal_function_info*)NULL)->_allow_null) + sizeof(((zend_internal_function_info*)NULL)->_is_variadic)  )')


define(`ELEMENTSOF_zend_compiled_variable', `"name","name_len","hash_value"')
define(`COUNTOF_zend_compiled_variable', `3')
define(`SIZEOF_zend_compiled_variable', `(  sizeof(((zend_compiled_variable*)NULL)->name) + sizeof(((zend_compiled_variable*)NULL)->name_len) + sizeof(((zend_compiled_variable*)NULL)->hash_value)  )')


define(`ELEMENTSOF_zend_op_array', `"type","function_name","scope","fn_flags","prototype","num_args","required_num_args","arg_info","refcount","opcodes","last","vars","last_var","T","nested_calls","used_stack","brk_cont_array","last_brk_cont","try_catch_array","last_try_catch","has_finally_block","static_variables","this_var","filename","line_start","line_end","doc_comment","doc_comment_len","early_binding","literals","last_literal","run_time_cache","last_cache_slot","reserved"')
define(`COUNTOF_zend_op_array', `34')
define(`SIZEOF_zend_op_array', `(  sizeof(((zend_op_array*)NULL)->type) + sizeof(((zend_op_array*)NULL)->function_name) + sizeof(((zend_op_array*)NULL)->scope) + sizeof(((zend_op_array*)NULL)->fn_flags) + sizeof(((zend_op_array*)NULL)->prototype) + sizeof(((zend_op_array*)NULL)->num_args) + sizeof(((zend_op_array*)NULL)->required_num_args) + sizeof(((zend_op_array*)NULL)->arg_info) + sizeof(((zend_op_array*)NULL)->refcount) + sizeof(((zend_op_array*)NULL)->opcodes) + sizeof(((zend_op_array*)NULL)->last) + sizeof(((zend_op_array*)NULL)->vars) + sizeof(((zend_op_array*)NULL)->last_var) + sizeof(((zend_op_array*)NULL)->T) + sizeof(((zend_op_array*)NULL)->nested_calls) + sizeof(((zend_op_array*)NULL)->used_stack) + sizeof(((zend_op_array*)NULL)->brk_cont_array) + sizeof(((zend_op_array*)NULL)->last_brk_cont) + sizeof(((zend_op_array*)NULL)->try_catch_array) + sizeof(((zend_op_array*)NULL)->last_try_catch) + sizeof(((zend_op_array*)NULL)->has_finally_block) + sizeof(((zend_op_array*)NULL)->static_variables) + sizeof(((zend_op_array*)NULL)->this_var) + sizeof(((zend_op_array*)NULL)->filename) + sizeof(((zend_op_array*)NULL)->line_start) + sizeof(((zend_op_array*)NULL)->line_end) + sizeof(((zend_op_array*)NULL)->doc_comment) + sizeof(((zend_op_array*)NULL)->doc_comment_len) + sizeof(((zend_op_array*)NULL)->early_binding) + sizeof(((zend_op_array*)NULL)->literals) + sizeof(((zend_op_array*)NULL)->last_literal) + sizeof(((zend_op_array*)NULL)->run_time_cache) + sizeof(((zend_op_array*)NULL)->last_cache_slot) + sizeof(((zend_op_array*)NULL)->reserved)  )')


define(`ELEMENTSOF_zend_internal_function', `"type","function_name","scope","fn_flags","prototype","num_args","required_num_args","arg_info","handler","module"')
define(`COUNTOF_zend_internal_function', `10')
define(`SIZEOF_zend_internal_function', `(  sizeof(((zend_internal_function*)NULL)->type) + sizeof(((zend_internal_function*)NULL)->function_name) + sizeof(((zend_internal_function*)NULL)->scope) + sizeof(((zend_internal_function*)NULL)->fn_flags) + sizeof(((zend_internal_function*)NULL)->prototype) + sizeof(((zend_internal_function*)NULL)->num_args) + sizeof(((zend_internal_function*)NULL)->required_num_args) + sizeof(((zend_internal_function*)NULL)->arg_info) + sizeof(((zend_internal_function*)NULL)->handler) + sizeof(((zend_internal_function*)NULL)->module)  )')


define(`ELEMENTSOF_zend_function_state', `"function","arguments"')
define(`COUNTOF_zend_function_state', `2')
define(`SIZEOF_zend_function_state', `(  sizeof(((zend_function_state*)NULL)->function) + sizeof(((zend_function_state*)NULL)->arguments)  )')


define(`ELEMENTSOF_zend_function_call_entry', `"fbc","arg_num","uses_argument_unpacking"')
define(`COUNTOF_zend_function_call_entry', `3')
define(`SIZEOF_zend_function_call_entry', `(  sizeof(((zend_function_call_entry*)NULL)->fbc) + sizeof(((zend_function_call_entry*)NULL)->arg_num) + sizeof(((zend_function_call_entry*)NULL)->uses_argument_unpacking)  )')


define(`ELEMENTSOF_zend_switch_entry', `"cond","default_case","control_var"')
define(`COUNTOF_zend_switch_entry', `3')
define(`SIZEOF_zend_switch_entry', `(  sizeof(((zend_switch_entry*)NULL)->cond) + sizeof(((zend_switch_entry*)NULL)->default_case) + sizeof(((zend_switch_entry*)NULL)->control_var)  )')


define(`ELEMENTSOF_list_llist_element', `"var","dimensions","value"')
define(`COUNTOF_list_llist_element', `3')
define(`SIZEOF_list_llist_element', `(  sizeof(((list_llist_element*)NULL)->var) + sizeof(((list_llist_element*)NULL)->dimensions) + sizeof(((list_llist_element*)NULL)->value)  )')


define(`ELEMENTSOF_call_slot', `"fbc","object","called_scope","num_additional_args","is_ctor_call","is_ctor_result_used"')
define(`COUNTOF_call_slot', `6')
define(`SIZEOF_call_slot', `(  sizeof(((call_slot*)NULL)->fbc) + sizeof(((call_slot*)NULL)->object) + sizeof(((call_slot*)NULL)->called_scope) + sizeof(((call_slot*)NULL)->num_additional_args) + sizeof(((call_slot*)NULL)->is_ctor_call) + sizeof(((call_slot*)NULL)->is_ctor_result_used)  )')


define(`ELEMENTSOF_zend_execute_data', `"opline","function_state","op_array","object","symbol_table","prev_execute_data","old_error_reporting","nested","original_return_value","current_scope","current_called_scope","current_this","fast_ret","delayed_exception","call_slots","call"')
define(`COUNTOF_zend_execute_data', `16')
define(`SIZEOF_zend_execute_data', `(  sizeof(((zend_execute_data*)NULL)->opline) + sizeof(((zend_execute_data*)NULL)->function_state) + sizeof(((zend_execute_data*)NULL)->op_array) + sizeof(((zend_execute_data*)NULL)->object) + sizeof(((zend_execute_data*)NULL)->symbol_table) + sizeof(((zend_execute_data*)NULL)->prev_execute_data) + sizeof(((zend_execute_data*)NULL)->old_error_reporting) + sizeof(((zend_execute_data*)NULL)->nested) + sizeof(((zend_execute_data*)NULL)->original_return_value) + sizeof(((zend_execute_data*)NULL)->current_scope) + sizeof(((zend_execute_data*)NULL)->current_called_scope) + sizeof(((zend_execute_data*)NULL)->current_this) + sizeof(((zend_execute_data*)NULL)->fast_ret) + sizeof(((zend_execute_data*)NULL)->delayed_exception) + sizeof(((zend_execute_data*)NULL)->call_slots) + sizeof(((zend_execute_data*)NULL)->call)  )')


define(`ELEMENTSOF_zend_stack', `"top","max","elements"')
define(`COUNTOF_zend_stack', `3')
define(`SIZEOF_zend_stack', `(  sizeof(((zend_stack*)NULL)->top) + sizeof(((zend_stack*)NULL)->max) + sizeof(((zend_stack*)NULL)->elements)  )')


define(`ELEMENTSOF_zend_ptr_stack', `"top","max","elements","top_element","persistent"')
define(`COUNTOF_zend_ptr_stack', `5')
define(`SIZEOF_zend_ptr_stack', `(  sizeof(((zend_ptr_stack*)NULL)->top) + sizeof(((zend_ptr_stack*)NULL)->max) + sizeof(((zend_ptr_stack*)NULL)->elements) + sizeof(((zend_ptr_stack*)NULL)->top_element) + sizeof(((zend_ptr_stack*)NULL)->persistent)  )')


define(`ELEMENTSOF_zend_object_store_bucket', `"destructor_called","valid","apply_count","bucket"')
define(`COUNTOF_zend_object_store_bucket', `4')
define(`SIZEOF_zend_object_store_bucket', `(  sizeof(((zend_object_store_bucket*)NULL)->destructor_called) + sizeof(((zend_object_store_bucket*)NULL)->valid) + sizeof(((zend_object_store_bucket*)NULL)->apply_count) + sizeof(((zend_object_store_bucket*)NULL)->bucket)  )')


define(`ELEMENTSOF_zend_objects_store', `"object_buckets","top","size","free_list_head"')
define(`COUNTOF_zend_objects_store', `4')
define(`SIZEOF_zend_objects_store', `(  sizeof(((zend_objects_store*)NULL)->object_buckets) + sizeof(((zend_objects_store*)NULL)->top) + sizeof(((zend_objects_store*)NULL)->size) + sizeof(((zend_objects_store*)NULL)->free_list_head)  )')


define(`ELEMENTSOF_zend_multibyte_functions', `"provider_name","encoding_fetcher","encoding_name_getter","lexer_compatibility_checker","encoding_detector","encoding_converter","encoding_list_parser","internal_encoding_getter","internal_encoding_setter"')
define(`COUNTOF_zend_multibyte_functions', `9')
define(`SIZEOF_zend_multibyte_functions', `(  sizeof(((zend_multibyte_functions*)NULL)->provider_name) + sizeof(((zend_multibyte_functions*)NULL)->encoding_fetcher) + sizeof(((zend_multibyte_functions*)NULL)->encoding_name_getter) + sizeof(((zend_multibyte_functions*)NULL)->lexer_compatibility_checker) + sizeof(((zend_multibyte_functions*)NULL)->encoding_detector) + sizeof(((zend_multibyte_functions*)NULL)->encoding_converter) + sizeof(((zend_multibyte_functions*)NULL)->encoding_list_parser) + sizeof(((zend_multibyte_functions*)NULL)->internal_encoding_getter) + sizeof(((zend_multibyte_functions*)NULL)->internal_encoding_setter)  )')


define(`ELEMENTSOF_zend_declarables', `"ticks"')
define(`COUNTOF_zend_declarables', `1')
define(`SIZEOF_zend_declarables', `(  sizeof(((zend_declarables*)NULL)->ticks)  )')


define(`ELEMENTSOF_zend_compiler_globals', `"bp_stack","switch_cond_stack","foreach_copy_stack","object_stack","declare_stack","active_class_entry","list_llist","dimension_llist","list_stack","function_call_stack","compiled_filename","zend_lineno","active_op_array","function_table","class_table","filenames_table","auto_globals","parse_error","in_compilation","short_tags","asp_tags","declarables","unclean_shutdown","ini_parser_unbuffered_errors","open_files","catch_begin","ini_parser_param","interactive","start_lineno","increment_lineno","implementing_class","access_type","doc_comment","doc_comment_len","compiler_options","current_namespace","current_import","current_import_function","current_import_const","in_namespace","has_bracketed_namespaces","const_filenames","context","context_stack","interned_strings_start","interned_strings_end","interned_strings_top","interned_strings_snapshot_top","interned_empty_string","interned_strings","script_encoding_list","script_encoding_list_size","multibyte","detect_unicode","encoding_declared"')
define(`COUNTOF_zend_compiler_globals', `55')
define(`SIZEOF_zend_compiler_globals', `(  sizeof(((zend_compiler_globals*)NULL)->bp_stack) + sizeof(((zend_compiler_globals*)NULL)->switch_cond_stack) + sizeof(((zend_compiler_globals*)NULL)->foreach_copy_stack) + sizeof(((zend_compiler_globals*)NULL)->object_stack) + sizeof(((zend_compiler_globals*)NULL)->declare_stack) + sizeof(((zend_compiler_globals*)NULL)->active_class_entry) + sizeof(((zend_compiler_globals*)NULL)->list_llist) + sizeof(((zend_compiler_globals*)NULL)->dimension_llist) + sizeof(((zend_compiler_globals*)NULL)->list_stack) + sizeof(((zend_compiler_globals*)NULL)->function_call_stack) + sizeof(((zend_compiler_globals*)NULL)->compiled_filename) + sizeof(((zend_compiler_globals*)NULL)->zend_lineno) + sizeof(((zend_compiler_globals*)NULL)->active_op_array) + sizeof(((zend_compiler_globals*)NULL)->function_table) + sizeof(((zend_compiler_globals*)NULL)->class_table) + sizeof(((zend_compiler_globals*)NULL)->filenames_table) + sizeof(((zend_compiler_globals*)NULL)->auto_globals) + sizeof(((zend_compiler_globals*)NULL)->parse_error) + sizeof(((zend_compiler_globals*)NULL)->in_compilation) + sizeof(((zend_compiler_globals*)NULL)->short_tags) + sizeof(((zend_compiler_globals*)NULL)->asp_tags) + sizeof(((zend_compiler_globals*)NULL)->declarables) + sizeof(((zend_compiler_globals*)NULL)->unclean_shutdown) + sizeof(((zend_compiler_globals*)NULL)->ini_parser_unbuffered_errors) + sizeof(((zend_compiler_globals*)NULL)->open_files) + sizeof(((zend_compiler_globals*)NULL)->catch_begin) + sizeof(((zend_compiler_globals*)NULL)->ini_parser_param) + sizeof(((zend_compiler_globals*)NULL)->interactive) + sizeof(((zend_compiler_globals*)NULL)->start_lineno) + sizeof(((zend_compiler_globals*)NULL)->increment_lineno) + sizeof(((zend_compiler_globals*)NULL)->implementing_class) + sizeof(((zend_compiler_globals*)NULL)->access_type) + sizeof(((zend_compiler_globals*)NULL)->doc_comment) + sizeof(((zend_compiler_globals*)NULL)->doc_comment_len) + sizeof(((zend_compiler_globals*)NULL)->compiler_options) + sizeof(((zend_compiler_globals*)NULL)->current_namespace) + sizeof(((zend_compiler_globals*)NULL)->current_import) + sizeof(((zend_compiler_globals*)NULL)->current_import_function) + sizeof(((zend_compiler_globals*)NULL)->current_import_const) + sizeof(((zend_compiler_globals*)NULL)->in_namespace) + sizeof(((zend_compiler_globals*)NULL)->has_bracketed_namespaces) + sizeof(((zend_compiler_globals*)NULL)->const_filenames) + sizeof(((zend_compiler_globals*)NULL)->context) + sizeof(((zend_compiler_globals*)NULL)->context_stack) + sizeof(((zend_compiler_globals*)NULL)->interned_strings_start) + sizeof(((zend_compiler_globals*)NULL)->interned_strings_end) + sizeof(((zend_compiler_globals*)NULL)->interned_strings_top) + sizeof(((zend_compiler_globals*)NULL)->interned_strings_snapshot_top) + sizeof(((zend_compiler_globals*)NULL)->interned_empty_string) + sizeof(((zend_compiler_globals*)NULL)->interned_strings) + sizeof(((zend_compiler_globals*)NULL)->script_encoding_list) + sizeof(((zend_compiler_globals*)NULL)->script_encoding_list_size) + sizeof(((zend_compiler_globals*)NULL)->multibyte) + sizeof(((zend_compiler_globals*)NULL)->detect_unicode) + sizeof(((zend_compiler_globals*)NULL)->encoding_declared)  )')


define(`ELEMENTSOF_zend_executor_globals', `"return_value_ptr_ptr","uninitialized_zval","uninitialized_zval_ptr","error_zval","error_zval_ptr","symtable_cache","symtable_cache_limit","symtable_cache_ptr","opline_ptr","active_symbol_table","symbol_table","included_files","bailout","error_reporting","orig_error_reporting","exit_status","active_op_array","function_table","class_table","zend_constants","scope","called_scope","This","precision","ticks_count","in_execution","in_autoload","autoload_func","full_tables_cleanup","no_extensions","regular_list","persistent_list","argument_stack","user_error_handler_error_reporting","user_error_handler","user_exception_handler","user_error_handlers_error_reporting","user_error_handlers","user_exception_handlers","error_handling","exception_class","timeout_seconds","lambda_count","ini_directives","modified_ini_directives","error_reporting_ini_entry","objects_store","exception","prev_exception","opline_before_exception","exception_op","current_execute_data","current_module","std_property_info","active","start_op","saved_fpu_cw_ptr","saved_fpu_cw","reserved"')
define(`COUNTOF_zend_executor_globals', `59')
define(`SIZEOF_zend_executor_globals', `(  sizeof(((zend_executor_globals*)NULL)->return_value_ptr_ptr) + sizeof(((zend_executor_globals*)NULL)->uninitialized_zval) + sizeof(((zend_executor_globals*)NULL)->uninitialized_zval_ptr) + sizeof(((zend_executor_globals*)NULL)->error_zval) + sizeof(((zend_executor_globals*)NULL)->error_zval_ptr) + sizeof(((zend_executor_globals*)NULL)->symtable_cache) + sizeof(((zend_executor_globals*)NULL)->symtable_cache_limit) + sizeof(((zend_executor_globals*)NULL)->symtable_cache_ptr) + sizeof(((zend_executor_globals*)NULL)->opline_ptr) + sizeof(((zend_executor_globals*)NULL)->active_symbol_table) + sizeof(((zend_executor_globals*)NULL)->symbol_table) + sizeof(((zend_executor_globals*)NULL)->included_files) + sizeof(((zend_executor_globals*)NULL)->bailout) + sizeof(((zend_executor_globals*)NULL)->error_reporting) + sizeof(((zend_executor_globals*)NULL)->orig_error_reporting) + sizeof(((zend_executor_globals*)NULL)->exit_status) + sizeof(((zend_executor_globals*)NULL)->active_op_array) + sizeof(((zend_executor_globals*)NULL)->function_table) + sizeof(((zend_executor_globals*)NULL)->class_table) + sizeof(((zend_executor_globals*)NULL)->zend_constants) + sizeof(((zend_executor_globals*)NULL)->scope) + sizeof(((zend_executor_globals*)NULL)->called_scope) + sizeof(((zend_executor_globals*)NULL)->This) + sizeof(((zend_executor_globals*)NULL)->precision) + sizeof(((zend_executor_globals*)NULL)->ticks_count) + sizeof(((zend_executor_globals*)NULL)->in_execution) + sizeof(((zend_executor_globals*)NULL)->in_autoload) + sizeof(((zend_executor_globals*)NULL)->autoload_func) + sizeof(((zend_executor_globals*)NULL)->full_tables_cleanup) + sizeof(((zend_executor_globals*)NULL)->no_extensions) + sizeof(((zend_executor_globals*)NULL)->regular_list) + sizeof(((zend_executor_globals*)NULL)->persistent_list) + sizeof(((zend_executor_globals*)NULL)->argument_stack) + sizeof(((zend_executor_globals*)NULL)->user_error_handler_error_reporting) + sizeof(((zend_executor_globals*)NULL)->user_error_handler) + sizeof(((zend_executor_globals*)NULL)->user_exception_handler) + sizeof(((zend_executor_globals*)NULL)->user_error_handlers_error_reporting) + sizeof(((zend_executor_globals*)NULL)->user_error_handlers) + sizeof(((zend_executor_globals*)NULL)->user_exception_handlers) + sizeof(((zend_executor_globals*)NULL)->error_handling) + sizeof(((zend_executor_globals*)NULL)->exception_class) + sizeof(((zend_executor_globals*)NULL)->timeout_seconds) + sizeof(((zend_executor_globals*)NULL)->lambda_count) + sizeof(((zend_executor_globals*)NULL)->ini_directives) + sizeof(((zend_executor_globals*)NULL)->modified_ini_directives) + sizeof(((zend_executor_globals*)NULL)->error_reporting_ini_entry) + sizeof(((zend_executor_globals*)NULL)->objects_store) + sizeof(((zend_executor_globals*)NULL)->exception) + sizeof(((zend_executor_globals*)NULL)->prev_exception) + sizeof(((zend_executor_globals*)NULL)->opline_before_exception) + sizeof(((zend_executor_globals*)NULL)->exception_op) + sizeof(((zend_executor_globals*)NULL)->current_execute_data) + sizeof(((zend_executor_globals*)NULL)->current_module) + sizeof(((zend_executor_globals*)NULL)->std_property_info) + sizeof(((zend_executor_globals*)NULL)->active) + sizeof(((zend_executor_globals*)NULL)->start_op) + sizeof(((zend_executor_globals*)NULL)->saved_fpu_cw_ptr) + sizeof(((zend_executor_globals*)NULL)->saved_fpu_cw) + sizeof(((zend_executor_globals*)NULL)->reserved)  )')


define(`ELEMENTSOF_zend_ini_scanner_globals', `"yy_in","yy_out","yy_leng","yy_start","yy_text","yy_cursor","yy_marker","yy_limit","yy_state","state_stack","filename","lineno","scanner_mode"')
define(`COUNTOF_zend_ini_scanner_globals', `13')
define(`SIZEOF_zend_ini_scanner_globals', `(  sizeof(((zend_ini_scanner_globals*)NULL)->yy_in) + sizeof(((zend_ini_scanner_globals*)NULL)->yy_out) + sizeof(((zend_ini_scanner_globals*)NULL)->yy_leng) + sizeof(((zend_ini_scanner_globals*)NULL)->yy_start) + sizeof(((zend_ini_scanner_globals*)NULL)->yy_text) + sizeof(((zend_ini_scanner_globals*)NULL)->yy_cursor) + sizeof(((zend_ini_scanner_globals*)NULL)->yy_marker) + sizeof(((zend_ini_scanner_globals*)NULL)->yy_limit) + sizeof(((zend_ini_scanner_globals*)NULL)->yy_state) + sizeof(((zend_ini_scanner_globals*)NULL)->state_stack) + sizeof(((zend_ini_scanner_globals*)NULL)->filename) + sizeof(((zend_ini_scanner_globals*)NULL)->lineno) + sizeof(((zend_ini_scanner_globals*)NULL)->scanner_mode)  )')


define(`ELEMENTSOF_zend_php_scanner_globals', `"yy_in","yy_out","yy_leng","yy_start","yy_text","yy_cursor","yy_marker","yy_limit","yy_state","state_stack","heredoc_label_stack","script_org","script_org_size","script_filtered","script_filtered_size","input_filter","output_filter","script_encoding"')
define(`COUNTOF_zend_php_scanner_globals', `18')
define(`SIZEOF_zend_php_scanner_globals', `(  sizeof(((zend_php_scanner_globals*)NULL)->yy_in) + sizeof(((zend_php_scanner_globals*)NULL)->yy_out) + sizeof(((zend_php_scanner_globals*)NULL)->yy_leng) + sizeof(((zend_php_scanner_globals*)NULL)->yy_start) + sizeof(((zend_php_scanner_globals*)NULL)->yy_text) + sizeof(((zend_php_scanner_globals*)NULL)->yy_cursor) + sizeof(((zend_php_scanner_globals*)NULL)->yy_marker) + sizeof(((zend_php_scanner_globals*)NULL)->yy_limit) + sizeof(((zend_php_scanner_globals*)NULL)->yy_state) + sizeof(((zend_php_scanner_globals*)NULL)->state_stack) + sizeof(((zend_php_scanner_globals*)NULL)->heredoc_label_stack) + sizeof(((zend_php_scanner_globals*)NULL)->script_org) + sizeof(((zend_php_scanner_globals*)NULL)->script_org_size) + sizeof(((zend_php_scanner_globals*)NULL)->script_filtered) + sizeof(((zend_php_scanner_globals*)NULL)->script_filtered_size) + sizeof(((zend_php_scanner_globals*)NULL)->input_filter) + sizeof(((zend_php_scanner_globals*)NULL)->output_filter) + sizeof(((zend_php_scanner_globals*)NULL)->script_encoding)  )')


define(`ELEMENTSOF_zend_auto_global', `"name","name_len","auto_global_callback","jit","armed"')
define(`COUNTOF_zend_auto_global', `5')
define(`SIZEOF_zend_auto_global', `(  sizeof(((zend_auto_global*)NULL)->name) + sizeof(((zend_auto_global*)NULL)->name_len) + sizeof(((zend_auto_global*)NULL)->auto_global_callback) + sizeof(((zend_auto_global*)NULL)->jit) + sizeof(((zend_auto_global*)NULL)->armed)  )')


define(`ELEMENTSOF_zend_module_entry', `"size","zend_api","zend_debug","zts","ini_entry","deps","name","functions","module_startup_func","module_shutdown_func","request_startup_func","request_shutdown_func","info_func","version","globals_size","globals_ptr","globals_ctor","globals_dtor","post_deactivate_func","module_started","type","handle","module_number","build_id"')
define(`COUNTOF_zend_module_entry', `24')
define(`SIZEOF_zend_module_entry', `(  sizeof(((zend_module_entry*)NULL)->size) + sizeof(((zend_module_entry*)NULL)->zend_api) + sizeof(((zend_module_entry*)NULL)->zend_debug) + sizeof(((zend_module_entry*)NULL)->zts) + sizeof(((zend_module_entry*)NULL)->ini_entry) + sizeof(((zend_module_entry*)NULL)->deps) + sizeof(((zend_module_entry*)NULL)->name) + sizeof(((zend_module_entry*)NULL)->functions) + sizeof(((zend_module_entry*)NULL)->module_startup_func) + sizeof(((zend_module_entry*)NULL)->module_shutdown_func) + sizeof(((zend_module_entry*)NULL)->request_startup_func) + sizeof(((zend_module_entry*)NULL)->request_shutdown_func) + sizeof(((zend_module_entry*)NULL)->info_func) + sizeof(((zend_module_entry*)NULL)->version) + sizeof(((zend_module_entry*)NULL)->globals_size) + sizeof(((zend_module_entry*)NULL)->globals_ptr) + sizeof(((zend_module_entry*)NULL)->globals_ctor) + sizeof(((zend_module_entry*)NULL)->globals_dtor) + sizeof(((zend_module_entry*)NULL)->post_deactivate_func) + sizeof(((zend_module_entry*)NULL)->module_started) + sizeof(((zend_module_entry*)NULL)->type) + sizeof(((zend_module_entry*)NULL)->handle) + sizeof(((zend_module_entry*)NULL)->module_number) + sizeof(((zend_module_entry*)NULL)->build_id)  )')


define(`ELEMENTSOF_zend_module_dep', `"name","rel","version","type"')
define(`COUNTOF_zend_module_dep', `4')
define(`SIZEOF_zend_module_dep', `(  sizeof(((zend_module_dep*)NULL)->name) + sizeof(((zend_module_dep*)NULL)->rel) + sizeof(((zend_module_dep*)NULL)->version) + sizeof(((zend_module_dep*)NULL)->type)  )')


define(`ELEMENTSOF_zend_rsrc_list_entry', `"ptr","type","refcount"')
define(`COUNTOF_zend_rsrc_list_entry', `3')
define(`SIZEOF_zend_rsrc_list_entry', `(  sizeof(((zend_rsrc_list_entry*)NULL)->ptr) + sizeof(((zend_rsrc_list_entry*)NULL)->type) + sizeof(((zend_rsrc_list_entry*)NULL)->refcount)  )')


define(`ELEMENTSOF_zend_rsrc_list_dtors_entry', `"list_dtor","plist_dtor","list_dtor_ex","plist_dtor_ex","type_name","module_number","resource_id","type"')
define(`COUNTOF_zend_rsrc_list_dtors_entry', `8')
define(`SIZEOF_zend_rsrc_list_dtors_entry', `(  sizeof(((zend_rsrc_list_dtors_entry*)NULL)->list_dtor) + sizeof(((zend_rsrc_list_dtors_entry*)NULL)->plist_dtor) + sizeof(((zend_rsrc_list_dtors_entry*)NULL)->list_dtor_ex) + sizeof(((zend_rsrc_list_dtors_entry*)NULL)->plist_dtor_ex) + sizeof(((zend_rsrc_list_dtors_entry*)NULL)->type_name) + sizeof(((zend_rsrc_list_dtors_entry*)NULL)->module_number) + sizeof(((zend_rsrc_list_dtors_entry*)NULL)->resource_id) + sizeof(((zend_rsrc_list_dtors_entry*)NULL)->type)  )')


define(`ELEMENTSOF_*zend_vm_stack', `"top","end","prev"')
define(`COUNTOF_*zend_vm_stack', `3')
define(`SIZEOF_*zend_vm_stack', `(  sizeof(((*zend_vm_stack*)NULL)->top) + sizeof(((*zend_vm_stack*)NULL)->end) + sizeof(((*zend_vm_stack*)NULL)->prev)  )')


define(`ELEMENTSOF_zend_free_op', `"var"')
define(`COUNTOF_zend_free_op', `1')
define(`SIZEOF_zend_free_op', `(  sizeof(((zend_free_op*)NULL)->var)  )')


define(`ELEMENTSOF_zend_function_entry', `"fname","handler","arg_info","num_args","flags"')
define(`COUNTOF_zend_function_entry', `5')
define(`SIZEOF_zend_function_entry', `(  sizeof(((zend_function_entry*)NULL)->fname) + sizeof(((zend_function_entry*)NULL)->handler) + sizeof(((zend_function_entry*)NULL)->arg_info) + sizeof(((zend_function_entry*)NULL)->num_args) + sizeof(((zend_function_entry*)NULL)->flags)  )')


define(`ELEMENTSOF_zend_fcall_info', `"size","function_table","function_name","symbol_table","retval_ptr_ptr","param_count","params","object_ptr","no_separation"')
define(`COUNTOF_zend_fcall_info', `9')
define(`SIZEOF_zend_fcall_info', `(  sizeof(((zend_fcall_info*)NULL)->size) + sizeof(((zend_fcall_info*)NULL)->function_table) + sizeof(((zend_fcall_info*)NULL)->function_name) + sizeof(((zend_fcall_info*)NULL)->symbol_table) + sizeof(((zend_fcall_info*)NULL)->retval_ptr_ptr) + sizeof(((zend_fcall_info*)NULL)->param_count) + sizeof(((zend_fcall_info*)NULL)->params) + sizeof(((zend_fcall_info*)NULL)->object_ptr) + sizeof(((zend_fcall_info*)NULL)->no_separation)  )')


define(`ELEMENTSOF_zend_fcall_info_cache', `"initialized","function_handler","calling_scope","called_scope","object_ptr"')
define(`COUNTOF_zend_fcall_info_cache', `5')
define(`SIZEOF_zend_fcall_info_cache', `(  sizeof(((zend_fcall_info_cache*)NULL)->initialized) + sizeof(((zend_fcall_info_cache*)NULL)->function_handler) + sizeof(((zend_fcall_info_cache*)NULL)->calling_scope) + sizeof(((zend_fcall_info_cache*)NULL)->called_scope) + sizeof(((zend_fcall_info_cache*)NULL)->object_ptr)  )')


define(`ELEMENTSOF_php_output_buffer', `"data","size","used","free","_res"')
define(`COUNTOF_php_output_buffer', `5')
define(`SIZEOF_php_output_buffer', `(  sizeof(((php_output_buffer*)NULL)->data) + sizeof(((php_output_buffer*)NULL)->size) + sizeof(((php_output_buffer*)NULL)->used) + sizeof(((php_output_buffer*)NULL)->free) + sizeof(((php_output_buffer*)NULL)->_res)  )')


define(`ELEMENTSOF_php_output_context', `"op","in","out"')
define(`COUNTOF_php_output_context', `3')
define(`SIZEOF_php_output_context', `(  sizeof(((php_output_context*)NULL)->op) + sizeof(((php_output_context*)NULL)->in) + sizeof(((php_output_context*)NULL)->out)  )')


define(`ELEMENTSOF_php_output_handler_user_func_t', `"fci","fcc","zoh"')
define(`COUNTOF_php_output_handler_user_func_t', `3')
define(`SIZEOF_php_output_handler_user_func_t', `(  sizeof(((php_output_handler_user_func_t*)NULL)->fci) + sizeof(((php_output_handler_user_func_t*)NULL)->fcc) + sizeof(((php_output_handler_user_func_t*)NULL)->zoh)  )')


define(`ELEMENTSOF_php_output_handler', `"name","name_len","flags","level","size","buffer","opaq","dtor","func"')
define(`COUNTOF_php_output_handler', `9')
define(`SIZEOF_php_output_handler', `(  sizeof(((php_output_handler*)NULL)->name) + sizeof(((php_output_handler*)NULL)->name_len) + sizeof(((php_output_handler*)NULL)->flags) + sizeof(((php_output_handler*)NULL)->level) + sizeof(((php_output_handler*)NULL)->size) + sizeof(((php_output_handler*)NULL)->buffer) + sizeof(((php_output_handler*)NULL)->opaq) + sizeof(((php_output_handler*)NULL)->dtor) + sizeof(((php_output_handler*)NULL)->func)  )')


define(`ELEMENTSOF_zend_output_globals', `"flags","handlers","active","running","output_start_filename","output_start_lineno"')
define(`COUNTOF_zend_output_globals', `6')
define(`SIZEOF_zend_output_globals', `(  sizeof(((zend_output_globals*)NULL)->flags) + sizeof(((zend_output_globals*)NULL)->handlers) + sizeof(((zend_output_globals*)NULL)->active) + sizeof(((zend_output_globals*)NULL)->running) + sizeof(((zend_output_globals*)NULL)->output_start_filename) + sizeof(((zend_output_globals*)NULL)->output_start_lineno)  )')


define(`ELEMENTSOF_php_stream_notifier', `"func","dtor","ptr","mask","progress","progress_max"')
define(`COUNTOF_php_stream_notifier', `6')
define(`SIZEOF_php_stream_notifier', `(  sizeof(((php_stream_notifier*)NULL)->func) + sizeof(((php_stream_notifier*)NULL)->dtor) + sizeof(((php_stream_notifier*)NULL)->ptr) + sizeof(((php_stream_notifier*)NULL)->mask) + sizeof(((php_stream_notifier*)NULL)->progress) + sizeof(((php_stream_notifier*)NULL)->progress_max)  )')


define(`ELEMENTSOF_php_stream_context', `"notifier","options","rsrc_id"')
define(`COUNTOF_php_stream_context', `3')
define(`SIZEOF_php_stream_context', `(  sizeof(((php_stream_context*)NULL)->notifier) + sizeof(((php_stream_context*)NULL)->options) + sizeof(((php_stream_context*)NULL)->rsrc_id)  )')


define(`ELEMENTSOF_php_stream_bucket', `"next","prev","brigade","buf","buflen","own_buf","is_persistent","refcount"')
define(`COUNTOF_php_stream_bucket', `8')
define(`SIZEOF_php_stream_bucket', `(  sizeof(((php_stream_bucket*)NULL)->next) + sizeof(((php_stream_bucket*)NULL)->prev) + sizeof(((php_stream_bucket*)NULL)->brigade) + sizeof(((php_stream_bucket*)NULL)->buf) + sizeof(((php_stream_bucket*)NULL)->buflen) + sizeof(((php_stream_bucket*)NULL)->own_buf) + sizeof(((php_stream_bucket*)NULL)->is_persistent) + sizeof(((php_stream_bucket*)NULL)->refcount)  )')


define(`ELEMENTSOF_php_stream_bucket_brigade', `"head","tail"')
define(`COUNTOF_php_stream_bucket_brigade', `2')
define(`SIZEOF_php_stream_bucket_brigade', `(  sizeof(((php_stream_bucket_brigade*)NULL)->head) + sizeof(((php_stream_bucket_brigade*)NULL)->tail)  )')


define(`ELEMENTSOF_php_stream_filter_ops', `"filter","stream","thisfilter","buckets_in","buckets_out","bytes_consumed","dtor","label"')
define(`COUNTOF_php_stream_filter_ops', `8')
define(`SIZEOF_php_stream_filter_ops', `(  sizeof(((php_stream_filter_ops*)NULL)->filter) + sizeof(((php_stream_filter_ops*)NULL)->stream) + sizeof(((php_stream_filter_ops*)NULL)->thisfilter) + sizeof(((php_stream_filter_ops*)NULL)->buckets_in) + sizeof(((php_stream_filter_ops*)NULL)->buckets_out) + sizeof(((php_stream_filter_ops*)NULL)->bytes_consumed) + sizeof(((php_stream_filter_ops*)NULL)->dtor) + sizeof(((php_stream_filter_ops*)NULL)->label)  )')


define(`ELEMENTSOF_php_stream_filter_chain', `"head","tail","stream"')
define(`COUNTOF_php_stream_filter_chain', `3')
define(`SIZEOF_php_stream_filter_chain', `(  sizeof(((php_stream_filter_chain*)NULL)->head) + sizeof(((php_stream_filter_chain*)NULL)->tail) + sizeof(((php_stream_filter_chain*)NULL)->stream)  )')


define(`ELEMENTSOF_php_stream_filter', `"fops","abstract","next","prev","is_persistent","chain","buffer","rsrc_id"')
define(`COUNTOF_php_stream_filter', `8')
define(`SIZEOF_php_stream_filter', `(  sizeof(((php_stream_filter*)NULL)->fops) + sizeof(((php_stream_filter*)NULL)->abstract) + sizeof(((php_stream_filter*)NULL)->next) + sizeof(((php_stream_filter*)NULL)->prev) + sizeof(((php_stream_filter*)NULL)->is_persistent) + sizeof(((php_stream_filter*)NULL)->chain) + sizeof(((php_stream_filter*)NULL)->buffer) + sizeof(((php_stream_filter*)NULL)->rsrc_id)  )')


define(`ELEMENTSOF_php_stream_filter_factory', `"create_filter"')
define(`COUNTOF_php_stream_filter_factory', `1')
define(`SIZEOF_php_stream_filter_factory', `(  sizeof(((php_stream_filter_factory*)NULL)->create_filter)  )')


define(`ELEMENTSOF_php_stream_statbuf', `"sb"')
define(`COUNTOF_php_stream_statbuf', `1')
define(`SIZEOF_php_stream_statbuf', `(  sizeof(((php_stream_statbuf*)NULL)->sb)  )')


define(`ELEMENTSOF_php_stream_dirent', `"d_name"')
define(`COUNTOF_php_stream_dirent', `1')
define(`SIZEOF_php_stream_dirent', `(  sizeof(((php_stream_dirent*)NULL)->d_name)  )')


define(`ELEMENTSOF_php_stream_ops', `"write","read","close","flush","label","seek","cast","stat","set_option"')
define(`COUNTOF_php_stream_ops', `9')
define(`SIZEOF_php_stream_ops', `(  sizeof(((php_stream_ops*)NULL)->write) + sizeof(((php_stream_ops*)NULL)->read) + sizeof(((php_stream_ops*)NULL)->close) + sizeof(((php_stream_ops*)NULL)->flush) + sizeof(((php_stream_ops*)NULL)->label) + sizeof(((php_stream_ops*)NULL)->seek) + sizeof(((php_stream_ops*)NULL)->cast) + sizeof(((php_stream_ops*)NULL)->stat) + sizeof(((php_stream_ops*)NULL)->set_option)  )')


define(`ELEMENTSOF_php_stream_wrapper_ops', `"stream_opener","stream_closer","stream_stat","url_stat","dir_opener","label","unlink","rename","stream_mkdir","stream_rmdir","stream_metadata"')
define(`COUNTOF_php_stream_wrapper_ops', `11')
define(`SIZEOF_php_stream_wrapper_ops', `(  sizeof(((php_stream_wrapper_ops*)NULL)->stream_opener) + sizeof(((php_stream_wrapper_ops*)NULL)->stream_closer) + sizeof(((php_stream_wrapper_ops*)NULL)->stream_stat) + sizeof(((php_stream_wrapper_ops*)NULL)->url_stat) + sizeof(((php_stream_wrapper_ops*)NULL)->dir_opener) + sizeof(((php_stream_wrapper_ops*)NULL)->label) + sizeof(((php_stream_wrapper_ops*)NULL)->unlink) + sizeof(((php_stream_wrapper_ops*)NULL)->rename) + sizeof(((php_stream_wrapper_ops*)NULL)->stream_mkdir) + sizeof(((php_stream_wrapper_ops*)NULL)->stream_rmdir) + sizeof(((php_stream_wrapper_ops*)NULL)->stream_metadata)  )')


define(`ELEMENTSOF_php_stream_wrapper', `"wops","abstract","is_url"')
define(`COUNTOF_php_stream_wrapper', `3')
define(`SIZEOF_php_stream_wrapper', `(  sizeof(((php_stream_wrapper*)NULL)->wops) + sizeof(((php_stream_wrapper*)NULL)->abstract) + sizeof(((php_stream_wrapper*)NULL)->is_url)  )')


define(`ELEMENTSOF_php_stream', `"ops","abstract","readfilters","writefilters","wrapper","wrapperthis","wrapperdata","fgetss_state","is_persistent","mode","rsrc_id","in_free","fclose_stdiocast","stdiocast","orig_path","context","flags","position","readbuf","readbuflen","readpos","writepos","chunk_size","eof","enclosing_stream"')
define(`COUNTOF_php_stream', `25')
define(`SIZEOF_php_stream', `(  sizeof(((php_stream*)NULL)->ops) + sizeof(((php_stream*)NULL)->abstract) + sizeof(((php_stream*)NULL)->readfilters) + sizeof(((php_stream*)NULL)->writefilters) + sizeof(((php_stream*)NULL)->wrapper) + sizeof(((php_stream*)NULL)->wrapperthis) + sizeof(((php_stream*)NULL)->wrapperdata) + sizeof(((php_stream*)NULL)->fgetss_state) + sizeof(((php_stream*)NULL)->is_persistent) + sizeof(((php_stream*)NULL)->mode) + sizeof(((php_stream*)NULL)->rsrc_id) + sizeof(((php_stream*)NULL)->in_free) + sizeof(((php_stream*)NULL)->fclose_stdiocast) + sizeof(((php_stream*)NULL)->stdiocast) + sizeof(((php_stream*)NULL)->orig_path) + sizeof(((php_stream*)NULL)->context) + sizeof(((php_stream*)NULL)->flags) + sizeof(((php_stream*)NULL)->position) + sizeof(((php_stream*)NULL)->readbuf) + sizeof(((php_stream*)NULL)->readbuflen) + sizeof(((php_stream*)NULL)->readpos) + sizeof(((php_stream*)NULL)->writepos) + sizeof(((php_stream*)NULL)->chunk_size) + sizeof(((php_stream*)NULL)->eof) + sizeof(((php_stream*)NULL)->enclosing_stream)  )')


define(`ELEMENTSOF_php_stream_xport_param', `"op","want_addr","want_textaddr","want_errortext","how","inputs","outputs"')
define(`COUNTOF_php_stream_xport_param', `7')
define(`SIZEOF_php_stream_xport_param', `(  sizeof(((php_stream_xport_param*)NULL)->op) + sizeof(((php_stream_xport_param*)NULL)->want_addr) + sizeof(((php_stream_xport_param*)NULL)->want_textaddr) + sizeof(((php_stream_xport_param*)NULL)->want_errortext) + sizeof(((php_stream_xport_param*)NULL)->how) + sizeof(((php_stream_xport_param*)NULL)->inputs) + sizeof(((php_stream_xport_param*)NULL)->outputs)  )')


define(`ELEMENTSOF_php_stream_xport_crypto_param', `"op","inputs","outputs"')
define(`COUNTOF_php_stream_xport_crypto_param', `3')
define(`SIZEOF_php_stream_xport_crypto_param', `(  sizeof(((php_stream_xport_crypto_param*)NULL)->op) + sizeof(((php_stream_xport_crypto_param*)NULL)->inputs) + sizeof(((php_stream_xport_crypto_param*)NULL)->outputs)  )')


define(`ELEMENTSOF_php_stream_mmap_range', `"offset","length","mode","mapped"')
define(`COUNTOF_php_stream_mmap_range', `4')
define(`SIZEOF_php_stream_mmap_range', `(  sizeof(((php_stream_mmap_range*)NULL)->offset) + sizeof(((php_stream_mmap_range*)NULL)->length) + sizeof(((php_stream_mmap_range*)NULL)->mode) + sizeof(((php_stream_mmap_range*)NULL)->mapped)  )')


define(`ELEMENTSOF_arg_separators', `"output","input"')
define(`COUNTOF_arg_separators', `2')
define(`SIZEOF_arg_separators', `(  sizeof(((arg_separators*)NULL)->output) + sizeof(((arg_separators*)NULL)->input)  )')


define(`ELEMENTSOF_php_core_globals', `"implicit_flush","output_buffering","sql_safe_mode","enable_dl","output_handler","unserialize_callback_func","serialize_precision","memory_limit","max_input_time","track_errors","display_errors","display_startup_errors","log_errors","log_errors_max_len","ignore_repeated_errors","ignore_repeated_source","report_memleaks","error_log","doc_root","user_dir","include_path","open_basedir","extension_dir","php_binary","sys_temp_dir","upload_tmp_dir","upload_max_filesize","error_append_string","error_prepend_string","auto_prepend_file","auto_append_file","input_encoding","internal_encoding","output_encoding","arg_separator","variables_order","rfc1867_protected_variables","connection_status","ignore_user_abort","header_is_being_sent","tick_functions","http_globals","expose_php","register_argc_argv","auto_globals_jit","docref_root","docref_ext","html_errors","xmlrpc_errors","xmlrpc_error_number","activated_auto_globals","modules_activated","file_uploads","during_request_startup","allow_url_fopen","enable_post_data_reading","always_populate_raw_post_data","report_zend_debug","last_error_type","last_error_message","last_error_file","last_error_lineno","disable_functions","disable_classes","allow_url_include","exit_on_timeout","max_input_nesting_level","max_input_vars","in_user_include","user_ini_filename","user_ini_cache_ttl","request_order","mail_x_header","mail_log","in_error_log"')
define(`COUNTOF_php_core_globals', `75')
define(`SIZEOF_php_core_globals', `(  sizeof(((php_core_globals*)NULL)->implicit_flush) + sizeof(((php_core_globals*)NULL)->output_buffering) + sizeof(((php_core_globals*)NULL)->sql_safe_mode) + sizeof(((php_core_globals*)NULL)->enable_dl) + sizeof(((php_core_globals*)NULL)->output_handler) + sizeof(((php_core_globals*)NULL)->unserialize_callback_func) + sizeof(((php_core_globals*)NULL)->serialize_precision) + sizeof(((php_core_globals*)NULL)->memory_limit) + sizeof(((php_core_globals*)NULL)->max_input_time) + sizeof(((php_core_globals*)NULL)->track_errors) + sizeof(((php_core_globals*)NULL)->display_errors) + sizeof(((php_core_globals*)NULL)->display_startup_errors) + sizeof(((php_core_globals*)NULL)->log_errors) + sizeof(((php_core_globals*)NULL)->log_errors_max_len) + sizeof(((php_core_globals*)NULL)->ignore_repeated_errors) + sizeof(((php_core_globals*)NULL)->ignore_repeated_source) + sizeof(((php_core_globals*)NULL)->report_memleaks) + sizeof(((php_core_globals*)NULL)->error_log) + sizeof(((php_core_globals*)NULL)->doc_root) + sizeof(((php_core_globals*)NULL)->user_dir) + sizeof(((php_core_globals*)NULL)->include_path) + sizeof(((php_core_globals*)NULL)->open_basedir) + sizeof(((php_core_globals*)NULL)->extension_dir) + sizeof(((php_core_globals*)NULL)->php_binary) + sizeof(((php_core_globals*)NULL)->sys_temp_dir) + sizeof(((php_core_globals*)NULL)->upload_tmp_dir) + sizeof(((php_core_globals*)NULL)->upload_max_filesize) + sizeof(((php_core_globals*)NULL)->error_append_string) + sizeof(((php_core_globals*)NULL)->error_prepend_string) + sizeof(((php_core_globals*)NULL)->auto_prepend_file) + sizeof(((php_core_globals*)NULL)->auto_append_file) + sizeof(((php_core_globals*)NULL)->input_encoding) + sizeof(((php_core_globals*)NULL)->internal_encoding) + sizeof(((php_core_globals*)NULL)->output_encoding) + sizeof(((php_core_globals*)NULL)->arg_separator) + sizeof(((php_core_globals*)NULL)->variables_order) + sizeof(((php_core_globals*)NULL)->rfc1867_protected_variables) + sizeof(((php_core_globals*)NULL)->connection_status) + sizeof(((php_core_globals*)NULL)->ignore_user_abort) + sizeof(((php_core_globals*)NULL)->header_is_being_sent) + sizeof(((php_core_globals*)NULL)->tick_functions) + sizeof(((php_core_globals*)NULL)->http_globals) + sizeof(((php_core_globals*)NULL)->expose_php) + sizeof(((php_core_globals*)NULL)->register_argc_argv) + sizeof(((php_core_globals*)NULL)->auto_globals_jit) + sizeof(((php_core_globals*)NULL)->docref_root) + sizeof(((php_core_globals*)NULL)->docref_ext) + sizeof(((php_core_globals*)NULL)->html_errors) + sizeof(((php_core_globals*)NULL)->xmlrpc_errors) + sizeof(((php_core_globals*)NULL)->xmlrpc_error_number) + sizeof(((php_core_globals*)NULL)->activated_auto_globals) + sizeof(((php_core_globals*)NULL)->modules_activated) + sizeof(((php_core_globals*)NULL)->file_uploads) + sizeof(((php_core_globals*)NULL)->during_request_startup) + sizeof(((php_core_globals*)NULL)->allow_url_fopen) + sizeof(((php_core_globals*)NULL)->enable_post_data_reading) + sizeof(((php_core_globals*)NULL)->always_populate_raw_post_data) + sizeof(((php_core_globals*)NULL)->report_zend_debug) + sizeof(((php_core_globals*)NULL)->last_error_type) + sizeof(((php_core_globals*)NULL)->last_error_message) + sizeof(((php_core_globals*)NULL)->last_error_file) + sizeof(((php_core_globals*)NULL)->last_error_lineno) + sizeof(((php_core_globals*)NULL)->disable_functions) + sizeof(((php_core_globals*)NULL)->disable_classes) + sizeof(((php_core_globals*)NULL)->allow_url_include) + sizeof(((php_core_globals*)NULL)->exit_on_timeout) + sizeof(((php_core_globals*)NULL)->max_input_nesting_level) + sizeof(((php_core_globals*)NULL)->max_input_vars) + sizeof(((php_core_globals*)NULL)->in_user_include) + sizeof(((php_core_globals*)NULL)->user_ini_filename) + sizeof(((php_core_globals*)NULL)->user_ini_cache_ttl) + sizeof(((php_core_globals*)NULL)->request_order) + sizeof(((php_core_globals*)NULL)->mail_x_header) + sizeof(((php_core_globals*)NULL)->mail_log) + sizeof(((php_core_globals*)NULL)->in_error_log)  )')


define(`ELEMENTSOF_zend_ini_entry', `"module_number","modifiable","name","name_length","on_modify","mh_arg1","mh_arg2","mh_arg3","value","value_length","orig_value","orig_value_length","orig_modifiable","modified","displayer"')
define(`COUNTOF_zend_ini_entry', `15')
define(`SIZEOF_zend_ini_entry', `(  sizeof(((zend_ini_entry*)NULL)->module_number) + sizeof(((zend_ini_entry*)NULL)->modifiable) + sizeof(((zend_ini_entry*)NULL)->name) + sizeof(((zend_ini_entry*)NULL)->name_length) + sizeof(((zend_ini_entry*)NULL)->on_modify) + sizeof(((zend_ini_entry*)NULL)->mh_arg1) + sizeof(((zend_ini_entry*)NULL)->mh_arg2) + sizeof(((zend_ini_entry*)NULL)->mh_arg3) + sizeof(((zend_ini_entry*)NULL)->value) + sizeof(((zend_ini_entry*)NULL)->value_length) + sizeof(((zend_ini_entry*)NULL)->orig_value) + sizeof(((zend_ini_entry*)NULL)->orig_value_length) + sizeof(((zend_ini_entry*)NULL)->orig_modifiable) + sizeof(((zend_ini_entry*)NULL)->modified) + sizeof(((zend_ini_entry*)NULL)->displayer)  )')


define(`ELEMENTSOF_zend_ini_parser_param', `"ini_parser_cb","arg"')
define(`COUNTOF_zend_ini_parser_param', `2')
define(`SIZEOF_zend_ini_parser_param', `(  sizeof(((zend_ini_parser_param*)NULL)->ini_parser_cb) + sizeof(((zend_ini_parser_param*)NULL)->arg)  )')


define(`ELEMENTSOF_cwd_state', `"cwd","cwd_length"')
define(`COUNTOF_cwd_state', `2')
define(`SIZEOF_cwd_state', `(  sizeof(((cwd_state*)NULL)->cwd) + sizeof(((cwd_state*)NULL)->cwd_length)  )')


define(`ELEMENTSOF_realpath_cache_bucket', `"key","path","path_len","realpath","realpath_len","is_dir","expires","next"')
define(`COUNTOF_realpath_cache_bucket', `8')
define(`SIZEOF_realpath_cache_bucket', `(  sizeof(((realpath_cache_bucket*)NULL)->key) + sizeof(((realpath_cache_bucket*)NULL)->path) + sizeof(((realpath_cache_bucket*)NULL)->path_len) + sizeof(((realpath_cache_bucket*)NULL)->realpath) + sizeof(((realpath_cache_bucket*)NULL)->realpath_len) + sizeof(((realpath_cache_bucket*)NULL)->is_dir) + sizeof(((realpath_cache_bucket*)NULL)->expires) + sizeof(((realpath_cache_bucket*)NULL)->next)  )')


define(`ELEMENTSOF_virtual_cwd_globals', `"cwd","realpath_cache_size","realpath_cache_size_limit","realpath_cache_ttl","realpath_cache"')
define(`COUNTOF_virtual_cwd_globals', `5')
define(`SIZEOF_virtual_cwd_globals', `(  sizeof(((virtual_cwd_globals*)NULL)->cwd) + sizeof(((virtual_cwd_globals*)NULL)->realpath_cache_size) + sizeof(((virtual_cwd_globals*)NULL)->realpath_cache_size_limit) + sizeof(((virtual_cwd_globals*)NULL)->realpath_cache_ttl) + sizeof(((virtual_cwd_globals*)NULL)->realpath_cache)  )')


define(`ELEMENTSOF_zend_constant', `"value","flags","name","name_len","module_number"')
define(`COUNTOF_zend_constant', `5')
define(`SIZEOF_zend_constant', `(  sizeof(((zend_constant*)NULL)->value) + sizeof(((zend_constant*)NULL)->flags) + sizeof(((zend_constant*)NULL)->name) + sizeof(((zend_constant*)NULL)->name_len) + sizeof(((zend_constant*)NULL)->module_number)  )')


define(`ELEMENTSOF__xc_shm_t', `"handlers"')
define(`COUNTOF__xc_shm_t', `1')
define(`SIZEOF__xc_shm_t', `(  sizeof(((_xc_shm_t*)NULL)->handlers)  )')


define(`ELEMENTSOF_xc_shm_t', `"handlers"')
define(`COUNTOF_xc_shm_t', `1')
define(`SIZEOF_xc_shm_t', `(  sizeof(((xc_shm_t*)NULL)->handlers)  )')
define(`ELEMENTSOF_xc_shm_handlers_t', `"can_readonly","is_readwrite","is_readonly","to_readwrite","to_readonly","init","destroy","meminit","memdestroy"')
define(`COUNTOF_xc_shm_handlers_t', `9')
define(`SIZEOF_xc_shm_handlers_t', `(  sizeof(((xc_shm_handlers_t*)NULL)->can_readonly) + sizeof(((xc_shm_handlers_t*)NULL)->is_readwrite) + sizeof(((xc_shm_handlers_t*)NULL)->is_readonly) + sizeof(((xc_shm_handlers_t*)NULL)->to_readwrite) + sizeof(((xc_shm_handlers_t*)NULL)->to_readonly) + sizeof(((xc_shm_handlers_t*)NULL)->init) + sizeof(((xc_shm_handlers_t*)NULL)->destroy) + sizeof(((xc_shm_handlers_t*)NULL)->meminit) + sizeof(((xc_shm_handlers_t*)NULL)->memdestroy)  )')


define(`ELEMENTSOF_xc_op_array_info_detail_t', `"index","info"')
define(`COUNTOF_xc_op_array_info_detail_t', `2')
define(`SIZEOF_xc_op_array_info_detail_t', `(  sizeof(((xc_op_array_info_detail_t*)NULL)->index) + sizeof(((xc_op_array_info_detail_t*)NULL)->info)  )')


define(`ELEMENTSOF_xc_op_array_info_t', `"literalinfo_cnt","literalinfos"')
define(`COUNTOF_xc_op_array_info_t', `2')
define(`SIZEOF_xc_op_array_info_t', `(  sizeof(((xc_op_array_info_t*)NULL)->literalinfo_cnt) + sizeof(((xc_op_array_info_t*)NULL)->literalinfos)  )')


define(`ELEMENTSOF_xc_classinfo_t', `"key","key_size","h","methodinfo_cnt","methodinfos","cest"')
define(`COUNTOF_xc_classinfo_t', `6')
define(`SIZEOF_xc_classinfo_t', `(  sizeof(((xc_classinfo_t*)NULL)->key) + sizeof(((xc_classinfo_t*)NULL)->key_size) + sizeof(((xc_classinfo_t*)NULL)->h) + sizeof(((xc_classinfo_t*)NULL)->methodinfo_cnt) + sizeof(((xc_classinfo_t*)NULL)->methodinfos) + sizeof(((xc_classinfo_t*)NULL)->cest)  )')


define(`ELEMENTSOF_xc_constinfo_t', `"key","key_size","h","constant"')
define(`COUNTOF_xc_constinfo_t', `4')
define(`SIZEOF_xc_constinfo_t', `(  sizeof(((xc_constinfo_t*)NULL)->key) + sizeof(((xc_constinfo_t*)NULL)->key_size) + sizeof(((xc_constinfo_t*)NULL)->h) + sizeof(((xc_constinfo_t*)NULL)->constant)  )')


define(`ELEMENTSOF_xc_funcinfo_t', `"key","key_size","h","op_array_info","func"')
define(`COUNTOF_xc_funcinfo_t', `5')
define(`SIZEOF_xc_funcinfo_t', `(  sizeof(((xc_funcinfo_t*)NULL)->key) + sizeof(((xc_funcinfo_t*)NULL)->key_size) + sizeof(((xc_funcinfo_t*)NULL)->h) + sizeof(((xc_funcinfo_t*)NULL)->op_array_info) + sizeof(((xc_funcinfo_t*)NULL)->func)  )')


define(`ELEMENTSOF_xc_autoglobal_t', `"key","key_len","h"')
define(`COUNTOF_xc_autoglobal_t', `3')
define(`SIZEOF_xc_autoglobal_t', `(  sizeof(((xc_autoglobal_t*)NULL)->key) + sizeof(((xc_autoglobal_t*)NULL)->key_len) + sizeof(((xc_autoglobal_t*)NULL)->h)  )')


define(`ELEMENTSOF_xc_md5sum_t', `"digest"')
define(`COUNTOF_xc_md5sum_t', `1')
define(`SIZEOF_xc_md5sum_t', `(  sizeof(((xc_md5sum_t*)NULL)->digest)  )')


define(`ELEMENTSOF_xc_entry_data_php_t', `"next","hvalue","md5","refcount","hits","size","op_array_info","op_array","constinfo_cnt","constinfos","funcinfo_cnt","funcinfos","classinfo_cnt","classinfos","autoglobal_cnt","autoglobals","compilererror_cnt","compilererrors","have_references"')
define(`COUNTOF_xc_entry_data_php_t', `19')
define(`SIZEOF_xc_entry_data_php_t', `(  sizeof(((xc_entry_data_php_t*)NULL)->next) + sizeof(((xc_entry_data_php_t*)NULL)->hvalue) + sizeof(((xc_entry_data_php_t*)NULL)->md5) + sizeof(((xc_entry_data_php_t*)NULL)->refcount) + sizeof(((xc_entry_data_php_t*)NULL)->hits) + sizeof(((xc_entry_data_php_t*)NULL)->size) + sizeof(((xc_entry_data_php_t*)NULL)->op_array_info) + sizeof(((xc_entry_data_php_t*)NULL)->op_array) + sizeof(((xc_entry_data_php_t*)NULL)->constinfo_cnt) + sizeof(((xc_entry_data_php_t*)NULL)->constinfos) + sizeof(((xc_entry_data_php_t*)NULL)->funcinfo_cnt) + sizeof(((xc_entry_data_php_t*)NULL)->funcinfos) + sizeof(((xc_entry_data_php_t*)NULL)->classinfo_cnt) + sizeof(((xc_entry_data_php_t*)NULL)->classinfos) + sizeof(((xc_entry_data_php_t*)NULL)->autoglobal_cnt) + sizeof(((xc_entry_data_php_t*)NULL)->autoglobals) + sizeof(((xc_entry_data_php_t*)NULL)->compilererror_cnt) + sizeof(((xc_entry_data_php_t*)NULL)->compilererrors) + sizeof(((xc_entry_data_php_t*)NULL)->have_references)  )')


define(`ELEMENTSOF_xc_entry_t', `"next","size","ctime","atime","dtime","hits","ttl","name"')
define(`COUNTOF_xc_entry_t', `8')
define(`SIZEOF_xc_entry_t', `(  sizeof(((xc_entry_t*)NULL)->next) + sizeof(((xc_entry_t*)NULL)->size) + sizeof(((xc_entry_t*)NULL)->ctime) + sizeof(((xc_entry_t*)NULL)->atime) + sizeof(((xc_entry_t*)NULL)->dtime) + sizeof(((xc_entry_t*)NULL)->hits) + sizeof(((xc_entry_t*)NULL)->ttl) + sizeof(((xc_entry_t*)NULL)->name)  )')


define(`ELEMENTSOF_xc_entry_php_t', `"entry","php","refcount","file_mtime","file_size","file_device","file_inode","filepath_len","filepath","dirpath_len","dirpath"')
define(`COUNTOF_xc_entry_php_t', `11')
define(`SIZEOF_xc_entry_php_t', `(  sizeof(((xc_entry_php_t*)NULL)->entry) + sizeof(((xc_entry_php_t*)NULL)->php) + sizeof(((xc_entry_php_t*)NULL)->refcount) + sizeof(((xc_entry_php_t*)NULL)->file_mtime) + sizeof(((xc_entry_php_t*)NULL)->file_size) + sizeof(((xc_entry_php_t*)NULL)->file_device) + sizeof(((xc_entry_php_t*)NULL)->file_inode) + sizeof(((xc_entry_php_t*)NULL)->filepath_len) + sizeof(((xc_entry_php_t*)NULL)->filepath) + sizeof(((xc_entry_php_t*)NULL)->dirpath_len) + sizeof(((xc_entry_php_t*)NULL)->dirpath)  )')


define(`ELEMENTSOF_xc_entry_var_t', `"entry","value","have_references"')
define(`COUNTOF_xc_entry_var_t', `3')
define(`SIZEOF_xc_entry_var_t', `(  sizeof(((xc_entry_var_t*)NULL)->entry) + sizeof(((xc_entry_var_t*)NULL)->value) + sizeof(((xc_entry_var_t*)NULL)->have_references)  )')


define(`ELEMENTSOF_xc_entry_hash_t', `"cacheid","entryslotid"')
define(`COUNTOF_xc_entry_hash_t', `2')
define(`SIZEOF_xc_entry_hash_t', `(  sizeof(((xc_entry_hash_t*)NULL)->cacheid) + sizeof(((xc_entry_hash_t*)NULL)->entryslotid)  )')


define(`ELEMENTSOF_xc_gc_op_array_t', `"num_args","arg_info","literals","opcodes"')
define(`COUNTOF_xc_gc_op_array_t', `4')
define(`SIZEOF_xc_gc_op_array_t', `(  sizeof(((xc_gc_op_array_t*)NULL)->num_args) + sizeof(((xc_gc_op_array_t*)NULL)->arg_info) + sizeof(((xc_gc_op_array_t*)NULL)->literals) + sizeof(((xc_gc_op_array_t*)NULL)->opcodes)  )')


define(`ELEMENTSOF_xc_compilererror_t', `"type","lineno","error_len","error"')
define(`COUNTOF_xc_compilererror_t', `4')
define(`SIZEOF_xc_compilererror_t', `(  sizeof(((xc_compilererror_t*)NULL)->type) + sizeof(((xc_compilererror_t*)NULL)->lineno) + sizeof(((xc_compilererror_t*)NULL)->error_len) + sizeof(((xc_compilererror_t*)NULL)->error)  )')


define(`ELEMENTSOF_xc_compile_result_t', `"op_array","function_table","class_table"')
define(`COUNTOF_xc_compile_result_t', `3')
define(`SIZEOF_xc_compile_result_t', `(  sizeof(((xc_compile_result_t*)NULL)->op_array) + sizeof(((xc_compile_result_t*)NULL)->function_table) + sizeof(((xc_compile_result_t*)NULL)->class_table)  )')


