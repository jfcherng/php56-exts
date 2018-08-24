
#include <string.h>
#include <stdio.h>

#include "php.h"
#include "zend_extensions.h"
#include "zend_compile.h"
#include "zend_API.h"
#include "zend_ini.h"

/* export: #include "xcache.h" :export */
#include "xcache.h"
/* export: #include "mod_cacher/xc_cache.h" :export */
#include "mod_cacher/xc_cache.h"
/* export: #include "xcache/xc_shm.h" :export */
#include "xcache/xc_shm.h"
/* export: #include "xcache/xc_allocator.h" :export */
#include "xcache/xc_allocator.h"
#include "xcache/xc_const_string.h"
#include "xcache/xc_utils.h"
#include "util/xc_align.h"
#include "xc_processor.h"
#include "xcache_globals.h"

#if defined(HARDENING_PATCH_HASH_PROTECT) && HARDENING_PATCH_HASH_PROTECT
extern unsigned int zend_hash_canary;
#endif




































































































































































































































































































































































































































































































































































































































































#ifndef NDEBUG
#	undef inline
#define inline
#endif

typedef zval *zval_ptr;
typedef zval *zval_ptr_nullable;
typedef char *xc_ztstring;
#ifdef ZEND_ENGINE_2_4
typedef zend_trait_alias *zend_trait_alias_ptr;
typedef zend_trait_precedence *zend_trait_precedence_ptr;
#endif
#ifdef ZEND_ENGINE_2_3
typedef int last_brk_cont_t;
#else
typedef zend_uint last_brk_cont_t;
#endif

typedef zend_uchar xc_zval_type_t;
typedef int xc_op_type;
typedef zend_uchar xc_opcode;
#ifdef IS_UNICODE
typedef UChar zstr_uchar;
#endif
typedef char  zstr_char;

#define MAX_DUP_STR_LEN 256
/* export: typedef struct _xc_processor_t xc_processor_t; :export {{{ */
struct _xc_processor_t {
	char *p;
	size_t size;
	HashTable strings;
	HashTable zvalptrs;
	zend_bool reference; /* enable if to deal with reference */
	zend_bool have_references;
	const xc_entry_php_t *entry_php_src;
	const xc_entry_php_t *entry_php_dst;
	const xc_entry_data_php_t *php_src;
	const xc_entry_data_php_t *php_dst;
	xc_shm_t                  *shm;
	xc_allocator_t            *allocator;
	const zend_class_entry *cache_ce;
	zend_ulong cache_class_index;

	const zend_op_array    *active_op_array_src;
	zend_op_array          *active_op_array_dst;
	const zend_class_entry *active_class_entry_src;
	zend_class_entry       *active_class_entry_dst;
	zend_uint                 active_class_index;
	zend_uint                 active_op_array_index;
	const xc_op_array_info_t *active_op_array_infos_src;

	zend_bool readonly_protection; /* wheather it's present */

};
/* }}} */
/* export: typedef struct _xc_dasm_t { const zend_op_array *active_op_array_src; } xc_dasm_t; :export {{{ */
/* }}} */
/* {{{ memsetptr */

/* }}} */
#ifdef HAVE_XCACHE_DPRINT
static void xc_dprint_indent(int indent) /* {{{ */
{
	int i;
	for (i = 0; i < indent; i ++) {
		fprintf(stderr, "  ");
	}
}
/* }}} */
static void xc_dprint_str_len(const char *str, int len) /* {{{ */
{
	const unsigned char *p = (const unsigned char *) str;
	int i;
	for (i = 0; i < len; i ++) {
		if (p[i] < 32 || p[i] == 127) {
			fprintf(stderr, "\\%03o", (unsigned int) p[i]);
		}
		else {
			fputc(p[i], stderr);
		}
	}
}
/* }}} */
#endif
/* {{{ xc_zstrlen_char */
static inline size_t xc_zstrlen_char(const_zstr s)
{
	return strlen(ZSTR_S(s));
}
/* }}} */
#ifdef IS_UNICODE
/* {{{ xc_zstrlen_uchar */
static inline size_t xc_zstrlen_uchar(zstr s)
{
	return u_strlen(ZSTR_U(s));
}
/* }}} */
/* {{{ xc_zstrlen */
static inline size_t xc_zstrlen(int type, const_zstr s)
{
	return type == IS_UNICODE ? xc_zstrlen_uchar(s) : xc_zstrlen_char(s);
}
/* }}} */
#else
/* {{{ xc_zstrlen */
#define xc_zstrlen(dummy, s) xc_zstrlen_char(s)
/* }}} */
#endif
/* {{{ xc_calc_string_n */
 
#undef C_RELAYLINE
#define C_RELAYLINE

static inline void xc_calc_string_n(xc_processor_t *processor, zend_uchar type, const_zstr str, long size ) {
	
	size_t realsize = UNISW(size, (type == IS_UNICODE) ? UBYTES(size) : size);
	long dummy = 1;

	if (realsize > MAX_DUP_STR_LEN) {
		
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(char) * realsize;
	
	
	
	
	
	

	}
	else if (zend_u_hash_add(&processor->strings, type, str, (uint) size, (void *) &dummy, sizeof(dummy), NULL) == SUCCESS) {
		/* new string */
		
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(char) * realsize;
	
	
	
	
	
	

	} 
	
	
}
/* }}} */
/* {{{ xc_store_string_n */
 
static inline zstr xc_store_string_n(xc_processor_t *processor, zend_uchar type, const_zstr str, long size ) {
	
	size_t realsize = UNISW(size, (type == IS_UNICODE) ? UBYTES(size) : size);
	zstr ret, *pret;

	if (realsize > MAX_DUP_STR_LEN) {
		
	
	
		
	
	
	/* allocate */
	
	
		
		
		ZSTR_V(ret) = (char *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(char) * realsize;

		
	
	
	
	
	

		memcpy(ZSTR_V(ret), ZSTR_V(str), realsize);
		return ret;
	}

	if (zend_u_hash_find(&processor->strings, type, str, (uint) size, (void **) &pret) == SUCCESS) {
		return *pret;
	}

	/* new string */
	
	
	
		
	
	
	/* allocate */
	
	
		
		
		ZSTR_V(ret) = (char *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(char) * realsize;

		
	
	
	
	
	

	memcpy(ZSTR_V(ret), ZSTR_V(str), realsize);
	zend_u_hash_add(&processor->strings, type, str, (uint) size, (void *) &ret, sizeof(zstr), NULL);
	return ret;

	
}
/* }}} */
/* {{{ xc_get_class_num
 * return class_index + 1
 */
static zend_ulong xc_get_class_num(xc_processor_t *processor, zend_class_entry *ce) {
	zend_uint i;
	const xc_entry_data_php_t *php = processor->php_src;
	zend_class_entry *ceptr;

	if (processor->cache_ce == ce) {
		return processor->cache_class_index + 1;
	}
	for (i = 0; i < php->classinfo_cnt; i ++) {
		ceptr = CestToCePtr(php->classinfos[i].cest);
		if (ZCEP_REFCOUNT_PTR(ceptr) == ZCEP_REFCOUNT_PTR(ce)) {
			processor->cache_ce = ceptr;
			processor->cache_class_index = i;
			return i + 1;
		}
	}
	assert(0);
	return (zend_ulong) -1;
}

/* }}} */
/* {{{ xc_get_class */
#ifdef ZEND_ENGINE_2
static zend_class_entry *xc_get_class(xc_processor_t *processor, zend_ulong class_num) {
	/* must be parent or currrent class */
	assert(class_num <= processor->active_class_index + 1);
	return CestToCePtr(processor->php_dst->classinfos[class_num - 1].cest);
}
#endif

/* }}} */
#ifdef ZEND_ENGINE_2
/* fix method on store */
static void xc_fix_method(xc_processor_t *processor, zend_op_array *dst TSRMLS_DC) /* {{{ */
{
	zend_function *zf = (zend_function *) dst;
	zend_class_entry *ce = processor->active_class_entry_dst;
	const zend_class_entry *srcce = processor->active_class_entry_src;

	/* Fixing up the default functions for objects here since
	 * we need to compare with the newly allocated functions
	 *
	 * caveat: a sub-class method can have the same name as the
	 * parent~s constructor and create problems.
	 */

	if (zf->common.fn_flags & ZEND_ACC_CTOR) {
		if (!ce->constructor) {
			ce->constructor = zf;
		}
	}
	else if (zf->common.fn_flags & ZEND_ACC_DTOR) {
		ce->destructor = zf;
	}
	else if (zf->common.fn_flags & ZEND_ACC_CLONE) {
		ce->clone = zf;
	}
	else {
	
#ifdef IS_UNICODE
		if (UG(unicode)) {
#define SET_IF_SAME_NAME(member) \
			do { \
				if (srcce->member && u_strcmp(ZSTR_U(zf->common.function_name), ZSTR_U(srcce->member->common.function_name)) == 0) { \
					ce->member = zf; \
				} \
			} \
			while(0)

			
		SET_IF_SAME_NAME(__get);
		SET_IF_SAME_NAME(__set);
#ifdef ZEND_ENGINE_2_1
		SET_IF_SAME_NAME(__unset);
		SET_IF_SAME_NAME(__isset);
#endif
		SET_IF_SAME_NAME(__call);
#ifdef ZEND_CALLSTATIC_FUNC_NAME
		SET_IF_SAME_NAME(__callstatic);
#endif
#if defined(ZEND_ENGINE_2_2) || PHP_MAJOR_VERSION >= 6
		SET_IF_SAME_NAME(__tostring);
#endif
#if defined(ZEND_ENGINE_2_6)
		SET_IF_SAME_NAME(__debugInfo);
#endif
	
#undef SET_IF_SAME_NAME
		}
		else
#endif
		do {
#define SET_IF_SAME_NAME(member) \
			do { \
				if (srcce->member && strcmp(ZSTR_S(zf->common.function_name), ZSTR_S(srcce->member->common.function_name)) == 0) { \
					ce->member = zf; \
				} \
			} \
			while(0)

			
		SET_IF_SAME_NAME(__get);
		SET_IF_SAME_NAME(__set);
#ifdef ZEND_ENGINE_2_1
		SET_IF_SAME_NAME(__unset);
		SET_IF_SAME_NAME(__isset);
#endif
		SET_IF_SAME_NAME(__call);
#ifdef ZEND_CALLSTATIC_FUNC_NAME
		SET_IF_SAME_NAME(__callstatic);
#endif
#if defined(ZEND_ENGINE_2_2) || PHP_MAJOR_VERSION >= 6
		SET_IF_SAME_NAME(__tostring);
#endif
#if defined(ZEND_ENGINE_2_6)
		SET_IF_SAME_NAME(__debugInfo);
#endif
	
#undef SET_IF_SAME_NAME
		} while (0);

	

	}
}
/* }}} */
#endif
/* {{{ call op_array ctor handler */
extern zend_bool xc_have_op_array_ctor;
static void xc_zend_extension_op_array_ctor_handler(zend_extension *extension, zend_op_array *op_array TSRMLS_DC)
{
	if (extension->op_array_ctor) {
		extension->op_array_ctor(op_array);
	}
}
/* }}} */
/* {{{ field name checker */

/* }}} */


/* export: xc_entry_var_t *xc_processor_store_xc_entry_var_t(xc_shm_t *shm, xc_allocator_t *allocator, xc_entry_var_t *src TSRMLS_DC); :export {{{ */
xc_entry_var_t *xc_processor_store_xc_entry_var_t(xc_shm_t *shm, xc_allocator_t *allocator, xc_entry_var_t *src TSRMLS_DC) {
	xc_entry_var_t *dst;
	xc_processor_t processor;

	memset(&processor, 0, sizeof(processor));
	processor.reference = 1;
	processor.shm = shm;
	processor.allocator = allocator;

	

	/* calc size */ {
		zend_hash_init(&processor.strings, 0, NULL, NULL, 0);
		if (processor.reference) {
			zend_hash_init(&processor.zvalptrs, 0, NULL, NULL, 0);
		}

		processor.size = 0;
		/* allocate */
		processor.size = ALIGN(processor.size + sizeof(src[0]));

		xc_calc_xc_entry_var_t(&processor, src TSRMLS_CC);
		if (processor.reference) {
			zend_hash_destroy(&processor.zvalptrs);
		}
		zend_hash_destroy(&processor.strings);
	}
	src->entry.size = processor.size;
	src->have_references = processor.have_references;

	
	/* store {{{ */
	{
		
		zend_hash_init(&processor.strings, 0, NULL, NULL, 0);
		if (processor.reference) {
			zend_hash_init(&processor.zvalptrs, 0, NULL, NULL, 0);
		}

		/* allocator :) */
		processor.p = (char *) processor.allocator->vtable->malloc(processor.allocator, processor.size);
		if (processor.p == NULL) {
			dst = NULL;
			goto err_alloc;
		}
		
		assert(processor.p == (char *) ALIGN(processor.p));

		/* allocate */
		dst = (xc_entry_var_t *) processor.p;
		processor.p = (char *) ALIGN(processor.p + sizeof(dst[0]));

		xc_store_xc_entry_var_t(&processor, dst, src TSRMLS_CC);
		
err_alloc:
		if (processor.reference) {
			zend_hash_destroy(&processor.zvalptrs);
		}
		zend_hash_destroy(&processor.strings);
	}
	/* }}} */

	

	return dst;
}
/* }}} */


/* export: xc_entry_php_t *xc_processor_store_xc_entry_php_t(xc_shm_t *shm, xc_allocator_t *allocator, xc_entry_php_t *src TSRMLS_DC); :export {{{ */
xc_entry_php_t *xc_processor_store_xc_entry_php_t(xc_shm_t *shm, xc_allocator_t *allocator, xc_entry_php_t *src TSRMLS_DC) {
	xc_entry_php_t *dst;
	xc_processor_t processor;

	memset(&processor, 0, sizeof(processor));
	processor.reference = 1;
	processor.shm = shm;
	processor.allocator = allocator;

	

	/* calc size */ {
		zend_hash_init(&processor.strings, 0, NULL, NULL, 0);
		if (processor.reference) {
			zend_hash_init(&processor.zvalptrs, 0, NULL, NULL, 0);
		}

		processor.size = 0;
		/* allocate */
		processor.size = ALIGN(processor.size + sizeof(src[0]));

		xc_calc_xc_entry_php_t(&processor, src TSRMLS_CC);
		if (processor.reference) {
			zend_hash_destroy(&processor.zvalptrs);
		}
		zend_hash_destroy(&processor.strings);
	}
	src->entry.size = processor.size;
	

	
	/* store {{{ */
	{
		
		zend_hash_init(&processor.strings, 0, NULL, NULL, 0);
		if (processor.reference) {
			zend_hash_init(&processor.zvalptrs, 0, NULL, NULL, 0);
		}

		/* allocator :) */
		processor.p = (char *) processor.allocator->vtable->malloc(processor.allocator, processor.size);
		if (processor.p == NULL) {
			dst = NULL;
			goto err_alloc;
		}
		
		assert(processor.p == (char *) ALIGN(processor.p));

		/* allocate */
		dst = (xc_entry_php_t *) processor.p;
		processor.p = (char *) ALIGN(processor.p + sizeof(dst[0]));

		xc_store_xc_entry_php_t(&processor, dst, src TSRMLS_CC);
		
err_alloc:
		if (processor.reference) {
			zend_hash_destroy(&processor.zvalptrs);
		}
		zend_hash_destroy(&processor.strings);
	}
	/* }}} */

	

	return dst;
}
/* }}} */


/* export: xc_entry_data_php_t *xc_processor_store_xc_entry_data_php_t(xc_shm_t *shm, xc_allocator_t *allocator, xc_entry_data_php_t *src TSRMLS_DC); :export {{{ */
xc_entry_data_php_t *xc_processor_store_xc_entry_data_php_t(xc_shm_t *shm, xc_allocator_t *allocator, xc_entry_data_php_t *src TSRMLS_DC) {
	xc_entry_data_php_t *dst;
	xc_processor_t processor;

	memset(&processor, 0, sizeof(processor));
	processor.reference = 1;
	processor.shm = shm;
	processor.allocator = allocator;

	

	/* calc size */ {
		zend_hash_init(&processor.strings, 0, NULL, NULL, 0);
		if (processor.reference) {
			zend_hash_init(&processor.zvalptrs, 0, NULL, NULL, 0);
		}

		processor.size = 0;
		/* allocate */
		processor.size = ALIGN(processor.size + sizeof(src[0]));

		xc_calc_xc_entry_data_php_t(&processor, src TSRMLS_CC);
		if (processor.reference) {
			zend_hash_destroy(&processor.zvalptrs);
		}
		zend_hash_destroy(&processor.strings);
	}
	src->size = processor.size;
	src->have_references = processor.have_references;
	

	
	/* store {{{ */
	{
		
		zend_hash_init(&processor.strings, 0, NULL, NULL, 0);
		if (processor.reference) {
			zend_hash_init(&processor.zvalptrs, 0, NULL, NULL, 0);
		}

		/* allocator :) */
		processor.p = (char *) processor.allocator->vtable->malloc(processor.allocator, processor.size);
		if (processor.p == NULL) {
			dst = NULL;
			goto err_alloc;
		}
		
		assert(processor.p == (char *) ALIGN(processor.p));

		/* allocate */
		dst = (xc_entry_data_php_t *) processor.p;
		processor.p = (char *) ALIGN(processor.p + sizeof(dst[0]));

		xc_store_xc_entry_data_php_t(&processor, dst, src TSRMLS_CC);
		
err_alloc:
		if (processor.reference) {
			zend_hash_destroy(&processor.zvalptrs);
		}
		zend_hash_destroy(&processor.strings);
	}
	/* }}} */

	

	return dst;
}
/* }}} */

/* export: xc_entry_php_t *xc_processor_restore_xc_entry_php_t(xc_entry_php_t *dst, const xc_entry_php_t *src TSRMLS_DC); :export {{{ */
xc_entry_php_t *xc_processor_restore_xc_entry_php_t(xc_entry_php_t *dst, const xc_entry_php_t *src TSRMLS_DC) {
	xc_processor_t processor;

	memset(&processor, 0, sizeof(processor));
	xc_restore_xc_entry_php_t(&processor, dst, src TSRMLS_CC);

	return dst;
}
/* }}} */
/* export: xc_entry_data_php_t *xc_processor_restore_xc_entry_data_php_t(const xc_entry_php_t *entry_php, xc_entry_data_php_t *dst, const xc_entry_data_php_t *src, zend_bool readonly_protection TSRMLS_DC); :export {{{ */
xc_entry_data_php_t *xc_processor_restore_xc_entry_data_php_t(const xc_entry_php_t *entry_php, xc_entry_data_php_t *dst, const xc_entry_data_php_t *src, zend_bool readonly_protection TSRMLS_DC) {
	xc_processor_t processor;

	memset(&processor, 0, sizeof(processor));
	processor.readonly_protection = readonly_protection;
	/* this function is used for php data only */
	if (src->have_references) {
		processor.reference = 1;
	}
	processor.entry_php_src = entry_php;

	if (processor.reference) {
		zend_hash_init(&processor.zvalptrs, 0, NULL, NULL, 0);
	}
	xc_restore_xc_entry_data_php_t(&processor, dst, src TSRMLS_CC);
	if (processor.reference) {
		zend_hash_destroy(&processor.zvalptrs);
	}
	return dst;
}
/* }}} */
/* export: xc_entry_var_t *xc_processor_restore_xc_entry_var_t(xc_entry_var_t *dst, const xc_entry_var_t *src TSRMLS_DC); :export {{{ */
xc_entry_var_t *xc_processor_restore_xc_entry_var_t(xc_entry_var_t *dst, const xc_entry_var_t *src TSRMLS_DC) {
	xc_processor_t processor;

	memset(&processor, 0, sizeof(processor));
	xc_restore_xc_entry_var_t(&processor, dst, src TSRMLS_CC);

	return dst;
}
/* }}} */
/* export: zval *xc_processor_restore_zval(zval *dst, const zval *src, zend_bool have_references TSRMLS_DC); :export {{{ */
zval *xc_processor_restore_zval(zval *dst, const zval *src, zend_bool have_references TSRMLS_DC) {
	xc_processor_t processor;

	memset(&processor, 0, sizeof(processor));
	processor.reference = have_references;

	if (processor.reference) {
		zend_hash_init(&processor.zvalptrs, 0, NULL, NULL, 0);
				zend_hash_add(&processor.zvalptrs, (char *)src, sizeof(src), (void*)&dst, sizeof(dst), NULL);
	}
	xc_restore_zval(&processor, dst, src TSRMLS_CC);
	if (processor.reference) {
		zend_hash_destroy(&processor.zvalptrs);
	}

	return dst;
}
/* }}} */
/* export: void xc_dprint(xc_entry_php_t *src, int indent TSRMLS_DC); :export {{{ */
#ifdef HAVE_XCACHE_DPRINT
void xc_dprint(xc_entry_php_t *src, int indent TSRMLS_DC) {
	
	xc_dprint_xc_entry_php_t(src, indent TSRMLS_CC);
}
#endif
/* }}} */



  /* {{{ Pre-declare */
 	  	 	void 	xc_calc_zval( 		 		xc_processor_t *processor, const zval * const src 		 		 		 		TSRMLS_DC 	); 	
 	  	 	static void inline 	xc_calc_zval_ptr( 		 		xc_processor_t *processor, const zval_ptr * const src 		 		 		 		TSRMLS_DC 	); 	
 	  	 	static void inline 	xc_calc_zval_ptr_nullable( 		 		xc_processor_t *processor, const zval_ptr_nullable * const src 		 		 		 		TSRMLS_DC 	); 	
 	  	 	void 	xc_calc_zend_op_array( 		 		xc_processor_t *processor, const zend_op_array * const src 		 		 		 		TSRMLS_DC 	); 	
 	  	 	void 	xc_calc_zend_class_entry( 		 		xc_processor_t *processor, const zend_class_entry * const src 		 		 		 		TSRMLS_DC 	); 	
#ifdef HAVE_XCACHE_CONSTANT
 	  	 	static void inline 	xc_calc_zend_constant( 		 		xc_processor_t *processor, const zend_constant * const src 		 		 		 		TSRMLS_DC 	); 	
#endif
 	  	 	void 	xc_calc_zend_function( 		 		xc_processor_t *processor, const zend_function * const src 		 		 		 		TSRMLS_DC 	); 	
 	  	 	void 	xc_calc_xc_entry_var_t( 		 		xc_processor_t *processor, const xc_entry_var_t * const src 		 		 		 		TSRMLS_DC 	); 	
 	  	 	void 	xc_calc_xc_entry_php_t( 		 		xc_processor_t *processor, const xc_entry_php_t * const src 		 		 		 		TSRMLS_DC 	); 	
#ifdef ZEND_ENGINE_2
 	  	 	static void inline 	xc_calc_zend_property_info( 		 		xc_processor_t *processor, const zend_property_info * const src 		 		 		 		TSRMLS_DC 	); 	
#endif
/* }}} */
#ifdef IS_CV

	
	

/* {{{ xc_calc_zend_compiled_variable */
	
 	  	 	static void inline 	xc_calc_zend_compiled_variable( 		 		xc_processor_t *processor, const zend_compiled_variable * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(int) == sizeof(src->name_len));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_UNICODE, src->name, src->name_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->name, src->name_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->name, src->name_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

		
	assert(sizeof(ulong) == sizeof(src->hash_value));
		
	
	
	
	
	
	


	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_calc_zend_uint */
	
 	  	 	static void inline 	xc_calc_zend_uint( 		 		xc_processor_t *processor, const zend_uint * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
	
	

		
		
		
		
		
		
	}
/* }}} */
	

#ifndef ZEND_ENGINE_2

	
	

/* {{{ xc_calc_int */
	
 	  	 	static void inline 	xc_calc_int( 		 		xc_processor_t *processor, const int * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
	
	

		
		
		
		
		
		
	}
/* }}} */
	

#endif
#ifdef ZEND_ENGINE_2

	
	

/* {{{ xc_calc_zend_try_catch_element */
	
 	  	 	static void inline 	xc_calc_zend_try_catch_element( 		 		xc_processor_t *processor, const zend_try_catch_element * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(zend_uint) == sizeof(src->try_op));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->catch_op));
		
	
	
	
	
	
	


	

#ifdef ZEND_ENGINE_2_5
		
	assert(sizeof(zend_uint) == sizeof(src->finally_op));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->finally_end));
		
	
	
	
	
	
	


	

#endif

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_calc_zend_brk_cont_element */
	
 	  	 	static void inline 	xc_calc_zend_brk_cont_element( 		 		xc_processor_t *processor, const zend_brk_cont_element * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
		#ifdef ZEND_ENGINE_2_2
		
	assert(sizeof(int) == sizeof(src->start));
		
	
	
	
	
	
	


	

#endif
		
	assert(sizeof(int) == sizeof(src->cont));
		
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->brk));
		
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->parent));
		
	
	
	
	
	
	


	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	
	

/* {{{ xc_calc_HashTable_zval_ptr */
	
 	  	 	static void inline 	xc_calc_HashTable_zval_ptr( 		 		xc_processor_t *processor, const HashTable * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
		
		
	

				
				Bucket *srcBucket;
		
		zend_bool first = 1;
				
		size_t bucketsize;

#if defined(HARDENING_PATCH_HASH_PROTECT) && HARDENING_PATCH_HASH_PROTECT
		
							
	assert(sizeof(unsigned int) == sizeof(src->canary));
		
	
	
	
	
	
	


	

		
#endif
			
	assert(sizeof(uint) == sizeof(src->nTableSize));
		
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nTableMask));
		
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nNumOfElements));
		
	
	
	
	
	
	


	

			
	assert(sizeof(ulong) == sizeof(src->nNextFreeElement));
		
	
	
	
	
	
	


	

		 
	
	
	

		 
	
	
	

#ifdef ZEND_ENGINE_2_4
	if (src->nTableMask) {
#endif
		
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(Bucket*) * src->nTableSize;
	
	
	
	
	
	

		
	
	
	

		
	
	

		for (srcBucket = src->pListHead; srcBucket != NULL; srcBucket = srcBucket->pListNext) {
			

			bucketsize = BUCKET_SIZE(srcBucket);
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(char) * bucketsize;
	
	
	
	
	
	

			
			
			if (sizeof(void *) == sizeof(zval_ptr)) {
				
								
	
	
	

	
	assert(sizeof(zval_ptr) == sizeof((  ((zval_ptr*)srcBucket->pData))[0]));
	

	

	xc_calc_zval_ptr(
		
		processor,   ((zval_ptr*)srcBucket->pData)
		
		
		
		TSRMLS_CC
	);

	

	
	

			}
			else {
				
	
	
	

	
	assert(sizeof(zval_ptr) == sizeof(( ((zval_ptr*)srcBucket->pData))[0]));
	
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zval_ptr);
	
	
	
	
	
	


	

	xc_calc_zval_ptr(
		
		processor,  ((zval_ptr*)srcBucket->pData)
		
		
		
		TSRMLS_CC
	);

	

	
	

				
			}

			if (first) {
				
				first = 0;
			}

			
		}
		
	
	

				
#ifdef ZEND_ENGINE_2_4
	}
	else { /* if (src->nTableMask) */
		
	
	
	

	}
#endif
		 
	
	
	

		 
	
	
	

			
	assert(sizeof(zend_bool) == sizeof(src->persistent));
		
	
	
	
	
	
	


	

#ifdef IS_UNICODE
			
	assert(sizeof(zend_bool) == sizeof(src->unicode));
		
	
	
	
	
	
	


	

#endif
			
	assert(sizeof(unsigned char) == sizeof(src->nApplyCount));
		
	
	
	
	
	
	


	

			
	assert(sizeof(zend_bool) == sizeof(src->bApplyProtection));
		
	
	
	
	
	
	


	

#if ZEND_DEBUG
			
	assert(sizeof(int) == sizeof(src->inconsistent));
		
	
	
	
	
	
	


	

#endif
				
	
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	



	
	
	

/* {{{ xc_calc_HashTable_zend_function */
	
		/* export:  	  	 	void 	xc_calc_HashTable_zend_function( 		 		xc_processor_t *processor, const HashTable * const src 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_calc_HashTable_zend_function( 		 		xc_processor_t *processor, const HashTable * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
		
		
	

				
				Bucket *srcBucket;
		
		zend_bool first = 1;
				
		size_t bucketsize;

#if defined(HARDENING_PATCH_HASH_PROTECT) && HARDENING_PATCH_HASH_PROTECT
		
							
	assert(sizeof(unsigned int) == sizeof(src->canary));
		
	
	
	
	
	
	


	

		
#endif
			
	assert(sizeof(uint) == sizeof(src->nTableSize));
		
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nTableMask));
		
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nNumOfElements));
		
	
	
	
	
	
	


	

			
	assert(sizeof(ulong) == sizeof(src->nNextFreeElement));
		
	
	
	
	
	
	


	

		 
	
	
	

		 
	
	
	

#ifdef ZEND_ENGINE_2_4
	if (src->nTableMask) {
#endif
		
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(Bucket*) * src->nTableSize;
	
	
	
	
	
	

		
	
	
	

		
	
	

		for (srcBucket = src->pListHead; srcBucket != NULL; srcBucket = srcBucket->pListNext) {
			

			bucketsize = BUCKET_SIZE(srcBucket);
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(char) * bucketsize;
	
	
	
	
	
	

			
			
			if (sizeof(void *) == sizeof(zend_function)) {
				
								
	
	
	

	
	assert(sizeof(zend_function) == sizeof((  ((zend_function*)srcBucket->pData))[0]));
	

	

	xc_calc_zend_function(
		
		processor,   ((zend_function*)srcBucket->pData)
		
		
		
		TSRMLS_CC
	);

	

	
	

			}
			else {
				
	
	
	

	
	assert(sizeof(zend_function) == sizeof(( ((zend_function*)srcBucket->pData))[0]));
	
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zend_function);
	
	
	
	
	
	


	

	xc_calc_zend_function(
		
		processor,  ((zend_function*)srcBucket->pData)
		
		
		
		TSRMLS_CC
	);

	

	
	

				
			}

			if (first) {
				
				first = 0;
			}

			
		}
		
	
	

				
#ifdef ZEND_ENGINE_2_4
	}
	else { /* if (src->nTableMask) */
		
	
	
	

	}
#endif
		 
	
	
	

		 
	
	
	

			
	assert(sizeof(zend_bool) == sizeof(src->persistent));
		
	
	
	
	
	
	


	

#ifdef IS_UNICODE
			
	assert(sizeof(zend_bool) == sizeof(src->unicode));
		
	
	
	
	
	
	


	

#endif
			
	assert(sizeof(unsigned char) == sizeof(src->nApplyCount));
		
	
	
	
	
	
	


	

			
	assert(sizeof(zend_bool) == sizeof(src->bApplyProtection));
		
	
	
	
	
	
	


	

#if ZEND_DEBUG
			
	assert(sizeof(int) == sizeof(src->inconsistent));
		
	
	
	
	
	
	


	

#endif
				
	
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


#ifdef ZEND_ENGINE_2

	
	
	

/* {{{ xc_calc_HashTable_zend_property_info */
	
 	  	 	static void inline 	xc_calc_HashTable_zend_property_info( 		 		xc_processor_t *processor, const HashTable * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
		
		
	

				
				Bucket *srcBucket;
		
		zend_bool first = 1;
				
		size_t bucketsize;

#if defined(HARDENING_PATCH_HASH_PROTECT) && HARDENING_PATCH_HASH_PROTECT
		
							
	assert(sizeof(unsigned int) == sizeof(src->canary));
		
	
	
	
	
	
	


	

		
#endif
			
	assert(sizeof(uint) == sizeof(src->nTableSize));
		
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nTableMask));
		
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nNumOfElements));
		
	
	
	
	
	
	


	

			
	assert(sizeof(ulong) == sizeof(src->nNextFreeElement));
		
	
	
	
	
	
	


	

		 
	
	
	

		 
	
	
	

#ifdef ZEND_ENGINE_2_4
	if (src->nTableMask) {
#endif
		
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(Bucket*) * src->nTableSize;
	
	
	
	
	
	

		
	
	
	

		
	
	

		for (srcBucket = src->pListHead; srcBucket != NULL; srcBucket = srcBucket->pListNext) {
			

			bucketsize = BUCKET_SIZE(srcBucket);
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(char) * bucketsize;
	
	
	
	
	
	

			
			
			if (sizeof(void *) == sizeof(zend_property_info)) {
				
								
	
	
	

	
	assert(sizeof(zend_property_info) == sizeof((  ((zend_property_info*)srcBucket->pData))[0]));
	

	

	xc_calc_zend_property_info(
		
		processor,   ((zend_property_info*)srcBucket->pData)
		
		
		
		TSRMLS_CC
	);

	

	
	

			}
			else {
				
	
	
	

	
	assert(sizeof(zend_property_info) == sizeof(( ((zend_property_info*)srcBucket->pData))[0]));
	
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zend_property_info);
	
	
	
	
	
	


	

	xc_calc_zend_property_info(
		
		processor,  ((zend_property_info*)srcBucket->pData)
		
		
		
		TSRMLS_CC
	);

	

	
	

				
			}

			if (first) {
				
				first = 0;
			}

			
		}
		
	
	

				
#ifdef ZEND_ENGINE_2_4
	}
	else { /* if (src->nTableMask) */
		
	
	
	

	}
#endif
		 
	
	
	

		 
	
	
	

			
	assert(sizeof(zend_bool) == sizeof(src->persistent));
		
	
	
	
	
	
	


	

#ifdef IS_UNICODE
			
	assert(sizeof(zend_bool) == sizeof(src->unicode));
		
	
	
	
	
	
	


	

#endif
			
	assert(sizeof(unsigned char) == sizeof(src->nApplyCount));
		
	
	
	
	
	
	


	

			
	assert(sizeof(zend_bool) == sizeof(src->bApplyProtection));
		
	
	
	
	
	
	


	

#if ZEND_DEBUG
			
	assert(sizeof(int) == sizeof(src->inconsistent));
		
	
	
	
	
	
	


	

#endif
				
	
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


#endif
#ifdef IS_CONSTANT_AST


	
	

/* {{{ xc_calc_zend_ast */
	
		/* export:  	  	 	void 	xc_calc_zend_ast( 		 		xc_processor_t *processor, const zend_ast * const src 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_calc_zend_ast( 		 		xc_processor_t *processor, const zend_ast * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				zend_ushort i;
			
	assert(sizeof(zend_ushort) == sizeof(src->kind));
		
	
	
	
	
	
	


	

			
	assert(sizeof(zend_ushort) == sizeof(src->children));
		
	
	
	
	
	
	


	

		
	
	
	

		
	
	

			if (src->kind == ZEND_CONST) {
				assert(src->u.val);
				
				
	
	
	

	
	assert(sizeof(zval) == sizeof((  src->u.val)[0]));
	

	

	xc_calc_zval(
		
		processor,   src->u.val
		
		
		
		TSRMLS_CC
	);

	

	
	

				
			}
			else {
				for (i = 0; i < src->children; ++i) {
					zend_ast *src_ast = (&src->u.child)[i];
					if (src_ast) {
						{
	
		size_t zend_ast_size = (src_ast->kind == ZEND_CONST)
		 ? sizeof(zend_ast) + sizeof(zval)
		 : sizeof(zend_ast) + sizeof(zend_ast *) * (src_ast->children - 1);
	

	
	
							
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += zend_ast_size;
	
	
	
	
	
	

							
	
	
	

	
	assert(sizeof(zend_ast) == sizeof((  src_ast)[0]));
	

	

	xc_calc_zend_ast(
		
		processor,   src_ast
		
		
		
		TSRMLS_CC
	);

	

	
	

						
	
}

						
					}
					else {
						
					}
				}
			}
		
	
	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_calc_zval */
	
		/* export:  	  	 	void 	xc_calc_zval( 		 		xc_processor_t *processor, const zval * const src 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_calc_zval( 		 		xc_processor_t *processor, const zval * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
				/* Variable information */
		
	
	

		switch ((Z_TYPE_P(src) & IS_CONSTANT_TYPE_MASK)) {
			case IS_LONG:
			case IS_RESOURCE:
			case IS_BOOL:
					
	assert(sizeof(long) == sizeof(src->value.lval));
		
	
	
	

	

				break;
			case IS_DOUBLE:
					
	assert(sizeof(double) == sizeof(src->value.dval));
		
	
	
	

	

				break;
			case IS_NULL:
				
				break;

			case IS_CONSTANT:
#ifdef IS_UNICODE
				if (UG(unicode)) {
					goto proc_unicode;
				}
#endif
			case IS_STRING:
#ifdef FLAG_IS_BC
			case FLAG_IS_BC:
#endif
					
	assert(sizeof(int) == sizeof(src->value.str.len));
		
	
	
	

	

				  
	
	
	
	
	
	
	
	
	if (src->value.str.val == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, ZSTR(src->value.str.val), src->value.str.len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

				break;
#ifdef IS_UNICODE
			case IS_UNICODE:
proc_unicode:
					
	assert(sizeof(int32_t) == sizeof(src->value.uni.len));
		
	
	
	

	

				 
	
#ifdef IS_UNICODE
	
	
	
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->value.uni.val) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_UNICODE, src->value.uni.val, src->value.uni.len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

	
#else
	
	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->value.uni.val) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->value.uni.val, src->value.uni.len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

				break;
#endif

			case IS_ARRAY:
#ifdef IS_CONSTANT_ARRAY
			case IS_CONSTANT_ARRAY:
#endif
				assert(src->value.ht);
				
	
	if (src->value.ht) {
		
		
	
	
	

	
	assert(sizeof(HashTable) == sizeof(( src->value.ht)[0]));
	
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(HashTable);
	
	
	
	
	
	


	

	xc_calc_HashTable_zval_ptr(
		
		processor,  src->value.ht
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->value.ht == NULL);

	}
	

				break;

#ifdef IS_CONSTANT_AST
			case IS_CONSTANT_AST:
				assert(src->value.ast);
				{
	
		size_t zend_ast_size = (src->value.ast->kind == ZEND_CONST)
		 ? sizeof(zend_ast) + sizeof(zval)
		 : sizeof(zend_ast) + sizeof(zend_ast *) * (src->value.ast->children - 1);
	

	
	
	
	if (src->value.ast) {
		
		
	
	
	

	
	assert(sizeof(zend_ast) == sizeof(( src->value.ast)[0]));
	
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += zend_ast_size;
	
	
	
	
	
	


	

	xc_calc_zend_ast(
		
		processor,  src->value.ast
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->value.ast == NULL);

	}
	

	
}

				break;
#endif

			case IS_OBJECT:
				
				#ifndef ZEND_ENGINE_2
				
	
	if (src->value.obj.ce) {
		
		
	
	
	

	
	assert(sizeof(zend_class_entry) == sizeof(( src->value.obj.ce)[0]));
	
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zend_class_entry);
	
	
	
	
	
	


	

	xc_calc_zend_class_entry(
		
		processor,  src->value.obj.ce
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->value.obj.ce == NULL);

	}
	

				
	
	if (src->value.obj.properties) {
		
		
	
	
	

	
	assert(sizeof(HashTable) == sizeof(( src->value.obj.properties)[0]));
	
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(HashTable);
	
	
	
	
	
	


	

	xc_calc_HashTable_zval_ptr(
		
		processor,  src->value.obj.properties
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->value.obj.properties == NULL);

	}
	

#endif
				break;

			default:
				assert(0);
		}
		
	
	

		
	
	
	

			
	assert(sizeof(xc_zval_type_t) == sizeof(src->type));
			
	
	
	
	
	
	



	

#ifdef ZEND_ENGINE_2_3
			
	assert(sizeof(zend_uchar) == sizeof(src->is_ref__gc));
		
	
	
	
	
	
	


	

#else
			
	assert(sizeof(zend_uchar) == sizeof(src->is_ref));
		
	
	
	
	
	
	


	

#endif

#ifdef ZEND_ENGINE_2_3
			
	assert(sizeof(zend_uint) == sizeof(src->refcount__gc));
		
	
	
	
	
	
	


	

#elif defined(ZEND_ENGINE_2)
			
	assert(sizeof(zend_uint) == sizeof(src->refcount));
		
	
	
	
	
	
	


	

#else
			
	assert(sizeof(zend_ushort) == sizeof(src->refcount));
		
	
	
	
	
	
	


	

#endif
	
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_calc_zval_ptr */
	
 	  	 	static void inline 	xc_calc_zval_ptr( 		 		xc_processor_t *processor, const zval_ptr * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
		do {
			
				if (processor->reference) {
					zval_ptr *ppzv;
					if (zend_hash_find(&processor->zvalptrs, (char *) &src[0], sizeof(src[0]), (void **) &ppzv) == SUCCESS) {
						
						processor->have_references = 1;
						
						
						break;
					}
				}
			
			
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zval);
	
	
	
	
	
	

			
				if (processor->reference) {
					
						/* make dummy */
						zval_ptr pzv = (zval_ptr)-1;
					
					if (zend_hash_add(&processor->zvalptrs, (char *) &src[0], sizeof(src[0]), (void *) &pzv, sizeof(pzv), NULL) == SUCCESS) {
						/* first add, go on */
											}
					else {
						assert(0);
					}
				}
			
			
			
			
	
	
	

	
	assert(sizeof(zval) == sizeof((  src[0])[0]));
	

	

	xc_calc_zval(
		
		processor,   src[0]
		
		
		
		TSRMLS_CC
	);

	

	
	

			
		} while (0);
	
	

		
		
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_calc_zval_ptr_nullable */
	
 	  	 	static void inline 	xc_calc_zval_ptr_nullable( 		 		xc_processor_t *processor, const zval_ptr_nullable * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			if (src[0]) {
		
		
	
	
	

	
	assert(sizeof(zval_ptr) == sizeof((  src)[0]));
	

	xc_calc_zval_ptr(
		
		processor,   src
		
		
		
		TSRMLS_CC
	);

	
	

		
	}
	else {
		
	}
	

		
		
		
		
		
		
	}
/* }}} */
	

#ifdef ZEND_ENGINE_2

	
	

/* {{{ xc_calc_zend_arg_info */
	
 	  	 	static void inline 	xc_calc_zend_arg_info( 		 		xc_processor_t *processor, const zend_arg_info * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(zend_uint) == sizeof(src->name_len));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_UNICODE, src->name, src->name_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->name, src->name_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->name, src->name_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

		
	assert(sizeof(zend_uint) == sizeof(src->class_name_len));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->class_name) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_UNICODE, src->class_name, src->class_name_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->class_name) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->class_name, src->class_name_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->class_name) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->class_name, src->class_name_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

#ifdef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uchar) == sizeof(src->type_hint));
		
	
	
	
	
	
	


	

#elif defined(ZEND_ENGINE_2_1)
		
	assert(sizeof(zend_bool) == sizeof(src->array_type_hint));
		
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_6
		
	assert(sizeof(zend_uchar) == sizeof(src->pass_by_reference));
		
	
	
	
	
	
	


	

#endif
		
	assert(sizeof(zend_bool) == sizeof(src->allow_null));
		
	
	
	
	
	
	


	


#ifdef ZEND_ENGINE_2_6
		
	assert(sizeof(zend_bool) == sizeof(src->is_variadic));
		
	
	
	
	
	
	


	

#else
		
	assert(sizeof(zend_bool) == sizeof(src->pass_by_reference));
		
	
	
	
	
	
	


	

#endif

#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_bool) == sizeof(src->return_reference));
		
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->required_num_args));
		
	
	
	
	
	
	


	

#endif

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif
#ifdef HAVE_XCACHE_CONSTANT

	
	

/* {{{ xc_calc_zend_constant */
	
 	  	 	static void inline 	xc_calc_zend_constant( 		 		xc_processor_t *processor, const zend_constant * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
	
	assert(sizeof(zval) == sizeof(src->value));
	
	
	
	
	

	
	assert(sizeof(zval) == sizeof((& src->value)[0]));
	

	

	xc_calc_zval(
		
		processor, & src->value
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


		
	assert(sizeof(int) == sizeof(src->flags));
		
	
	
	
	
	
	


	

		
	assert(sizeof(uint) == sizeof(src->name_len));
		
	
	
	
	
	
	


	

	
	
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_UNICODE, src->name, src->name_len C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->name, src->name_len C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->name, src->name_len C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	assert(sizeof(int) == sizeof(src->module_number));
		
	
	
	
	
	
	


	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_calc_zend_function */
	
		/* export:  	  	 	void 	xc_calc_zend_function( 		 		xc_processor_t *processor, const zend_function * const src 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_calc_zend_function( 		 		xc_processor_t *processor, const zend_function * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
	
	

	switch (src->type) {
	case ZEND_INTERNAL_FUNCTION:
	case ZEND_OVERLOADED_FUNCTION:
		
		break;

	case ZEND_USER_FUNCTION:
	case ZEND_EVAL_CODE:
		
		
	
	assert(sizeof(zend_op_array) == sizeof(src->op_array));
	
	
	
	
	

	
	assert(sizeof(zend_op_array) == sizeof((& src->op_array)[0]));
	

	

	xc_calc_zend_op_array(
		
		processor, & src->op_array
		
		
		
		TSRMLS_CC
	);

	

	
	

	

		break;

	default:
		assert(0);
	}
	
	
	

	

		
		
		
		
		
		
	}
/* }}} */
	

#ifdef ZEND_ENGINE_2

	
	

/* {{{ xc_calc_zend_property_info */
	
 	  	 	static void inline 	xc_calc_zend_property_info( 		 		xc_processor_t *processor, const zend_property_info * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(zend_uint) == sizeof(src->flags));
		
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->name_length));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_UNICODE, src->name, src->name_length + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->name, src->name_length + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->name, src->name_length + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
	
	
	
	
	


	

#ifdef ZEND_ENGINE_2_4
		
	assert(sizeof(int) == sizeof(src->offset));
		
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_1
		
	assert(sizeof(int) == sizeof(src->doc_comment_len));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_UNICODE, src->doc_comment, src->doc_comment_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->doc_comment, src->doc_comment_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->doc_comment, src->doc_comment_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

#endif
	#if defined(ZEND_ENGINE_2_2)
	
	if (src->ce) {
		
		
#ifdef IS_UNICODE
		
#else
		
#endif
	}
	else {
		
	
	
	assert(src->ce == NULL);

	}

	
	
	

#endif

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif
#ifdef ZEND_ENGINE_2_4

	
	

/* {{{ xc_calc_zend_trait_method_reference */
	
 	  	 	static void inline 	xc_calc_zend_trait_method_reference( 		 		xc_processor_t *processor, const zend_trait_method_reference * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(unsigned int) == sizeof(src->mname_len));
		
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->method_name == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, ZSTR(src->method_name), src->mname_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

	
	
	
	
	assert(src->ce == NULL);

	
	
	


		
	assert(sizeof(unsigned int) == sizeof(src->cname_len));
		
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->class_name == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, ZSTR(src->class_name), src->cname_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_calc_zend_trait_alias */
	
 	  	 	static void inline 	xc_calc_zend_trait_alias( 		 		xc_processor_t *processor, const zend_trait_alias * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
	
	if (src->trait_method) {
		
		
	
	
	

	
	assert(sizeof(zend_trait_method_reference) == sizeof(( src->trait_method)[0]));
	
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zend_trait_method_reference);
	
	
	
	
	
	


	

	xc_calc_zend_trait_method_reference(
		
		processor,  src->trait_method
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->trait_method == NULL);

	}
	
	
	
	


		
	assert(sizeof(unsigned int) == sizeof(src->alias_len));
		
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->alias == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, ZSTR(src->alias), src->alias_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		
	assert(sizeof(zend_uint) == sizeof(src->modifiers));
		
	
	
	
	
	
	


	

#ifndef ZEND_ENGINE_2_5
	
	
	
	
	assert(src->function == NULL);

	
	
	


#endif

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_calc_zend_trait_precedence */
	
 	  	 	static void inline 	xc_calc_zend_trait_precedence( 		 		xc_processor_t *processor, const zend_trait_precedence * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
	
	if (src->trait_method) {
		
		
	
	
	

	
	assert(sizeof(zend_trait_method_reference) == sizeof(( src->trait_method)[0]));
	
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zend_trait_method_reference);
	
	
	
	
	
	


	

	xc_calc_zend_trait_method_reference(
		
		processor,  src->trait_method
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->trait_method == NULL);

	}
	
	
	
	


		if (src->exclude_from_classes) {
		int LOOPCOUNTER;
		
						
				size_t count = 0;
				while (src->exclude_from_classes[count]) {
					++count;
				}
				++count;
				
			
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(xc_ztstring) * count;
	
	
	
	
	
	

			

			for (LOOPCOUNTER = 0;
					src->exclude_from_classes[LOOPCOUNTER];
					++LOOPCOUNTER) {
				
	
	

					
					
						
	assert(sizeof(xc_ztstring) == sizeof((xc_ztstring) src->exclude_from_classes [LOOPCOUNTER]));
		
	 
	
	
	
	
	
	
	
	
	if ((xc_ztstring) src->exclude_from_classes [LOOPCOUNTER] == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, ZSTR((xc_ztstring) src->exclude_from_classes [LOOPCOUNTER]), strlen((xc_ztstring) src->exclude_from_classes [LOOPCOUNTER]) + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

	

	

					
					
				
	
	

			}
						
				
	
	
	

	}
	else {
		
	
	
	
	assert(src->exclude_from_classes == NULL);

	
	
	


	}

#ifndef ZEND_ENGINE_2_5
	
	
	
	
	assert(src->function == NULL);

	
	
	


#endif

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_calc_zend_trait_alias_ptr */
	
 	  	 	static void inline 	xc_calc_zend_trait_alias_ptr( 		 		xc_processor_t *processor, const zend_trait_alias_ptr * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
		
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zend_trait_alias);
	
	
	
	
	
	

		
	
	
	

	
	assert(sizeof(zend_trait_alias) == sizeof((  src[0])[0]));
	

	

	xc_calc_zend_trait_alias(
		
		processor,   src[0]
		
		
		
		TSRMLS_CC
	);

	

	
	

		
	
	

		
		
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_calc_zend_trait_precedence_ptr */
	
 	  	 	static void inline 	xc_calc_zend_trait_precedence_ptr( 		 		xc_processor_t *processor, const zend_trait_precedence_ptr * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
		
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zend_trait_precedence);
	
	
	
	
	
	

		
	
	
	

	
	assert(sizeof(zend_trait_precedence) == sizeof((  src[0])[0]));
	

	

	xc_calc_zend_trait_precedence(
		
		processor,   src[0]
		
		
		
		TSRMLS_CC
	);

	

	
	

		
	
	

		
		
		
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_calc_zend_class_entry */
	
		/* export:  	  	 	void 	xc_calc_zend_class_entry( 		 		xc_processor_t *processor, const zend_class_entry * const src 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_calc_zend_class_entry( 		 		xc_processor_t *processor, const zend_class_entry * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
		processor->active_class_entry_src = src;
		
	
		
	assert(sizeof(char) == sizeof(src->type));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->name_length));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_UNICODE, src->name, src->name_length + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->name, src->name_length + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->name, src->name_length + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	if (src->parent) {
		
		
#ifdef IS_UNICODE
		
#else
		
#endif
	}
	else {
		
	
	
	assert(src->parent == NULL);

	}

	
	
	

	
#ifdef ZEND_ENGINE_2
		
	assert(sizeof(int) == sizeof(src->refcount));
		
	
	
	
	
	
	


	

#else
	
	
	if (src->refcount) {
		
		
	
	
	

	
	assert(sizeof(int) == sizeof(( src->refcount)[0]));
	
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(int);
	
	
	
	
	
	


	

	xc_calc_int(
		
		processor,  src->refcount
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->refcount == NULL);

	}
	
	
	
	


#endif
#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_bool) == sizeof(src->constants_updated));
		
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2
		
	assert(sizeof(zend_uint) == sizeof(src->ce_flags));
		
	
	
	
	
	
	


	

#endif

#ifdef ZEND_ENGINE_2
	
	
	assert(sizeof(HashTable) == sizeof(src->properties_info));
	
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->properties_info)[0]));
	

	

	xc_calc_HashTable_zend_property_info(
		
		processor, & src->properties_info
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


#endif

#ifdef ZEND_ENGINE_2_4
	
	if (src->default_properties_table) {
		int i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zval_ptr_nullable) * src->default_properties_count;
	
	
	
	
	
	

			

			for (i = 0;
					i < src->default_properties_count;
					++i) {
				
	
	

					
	
	assert(sizeof(zval_ptr_nullable) == sizeof(src->default_properties_table[i]));
	
	
	
	
	

	
	assert(sizeof(zval_ptr_nullable) == sizeof((& src->default_properties_table[i])[0]));
	

	

	xc_calc_zval_ptr_nullable(
		
		processor, & src->default_properties_table[i]
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->default_properties_table == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->default_properties_count));
		
	
	
	
	
	
	


	

	
	if (src->default_static_members_table) {
		int i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zval_ptr_nullable) * src->default_static_members_count;
	
	
	
	
	
	

			

			for (i = 0;
					i < src->default_static_members_count;
					++i) {
				
	
	

					
	
	assert(sizeof(zval_ptr_nullable) == sizeof(src->default_static_members_table[i]));
	
	
	
	
	

	
	assert(sizeof(zval_ptr_nullable) == sizeof((& src->default_static_members_table[i])[0]));
	

	

	xc_calc_zval_ptr_nullable(
		
		processor, & src->default_static_members_table[i]
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->default_static_members_table == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->default_static_members_count));
		
	
	
	
	
	
	


	

	
	
	
	
	

#else
	
	
	
	
	

	
	
	assert(sizeof(HashTable) == sizeof(src->default_properties));
	
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->default_properties)[0]));
	

	

	xc_calc_HashTable_zval_ptr(
		
		processor, & src->default_properties
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


#	ifdef ZEND_ENGINE_2_1
	
	
	assert(sizeof(HashTable) == sizeof(src->default_static_members));
	
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->default_static_members)[0]));
	

	

	xc_calc_HashTable_zval_ptr(
		
		processor, & src->default_static_members
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
	
	
	

#	elif defined(ZEND_ENGINE_2)
	
	
	if (src->static_members) {
		
		
	
	
	

	
	assert(sizeof(HashTable) == sizeof(( src->static_members)[0]));
	
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(HashTable);
	
	
	
	
	
	


	

	xc_calc_HashTable_zval_ptr(
		
		processor,  src->static_members
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->static_members == NULL);

	}
	
	
	
	


#	endif
#endif /* ZEND_ENGINE_2_4 */

#ifdef ZEND_ENGINE_2
	
	
	assert(sizeof(HashTable) == sizeof(src->constants_table));
	
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->constants_table)[0]));
	

	

	xc_calc_HashTable_zval_ptr(
		
		processor, & src->constants_table
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	



#ifdef ZEND_ENGINE_2_2
		
	
	
	
	assert(src->interfaces == NULL);

	
	
	


	
	
	
	
	assert(src->num_interfaces == 0);

	
	
	



#	ifdef ZEND_ENGINE_2_4
		
	
	
	
	assert(src->traits == NULL);

	
	
	


	
	
	
	
	assert(src->num_traits == 0);

	
	
	


	
	if (src->trait_aliases) {
		size_t i; 
		
	

		
						
				size_t count = 0;
				while (src->trait_aliases[count]) {
					++count;
				}
				++count;
				
			
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zend_trait_alias_ptr) * count;
	
	
	
	
	
	

			

			for (i = 0;
					src->trait_aliases[i];
					++i) {
				
	
	

					
	
	assert(sizeof(zend_trait_alias_ptr) == sizeof(src->trait_aliases[i]));
	
	
	
	
	

	
	assert(sizeof(zend_trait_alias_ptr) == sizeof((& src->trait_aliases[i])[0]));
	

	

	xc_calc_zend_trait_alias_ptr(
		
		processor, & src->trait_aliases[i]
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->trait_aliases == NULL);

	
	
	


	}

	
	if (src->trait_precedences) {
		size_t i; 
		
	

		
						
				size_t count = 0;
				while (src->trait_precedences[count]) {
					++count;
				}
				++count;
				
			
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zend_trait_precedence_ptr) * count;
	
	
	
	
	
	

			

			for (i = 0;
					src->trait_precedences[i];
					++i) {
				
	
	

					
	
	assert(sizeof(zend_trait_precedence_ptr) == sizeof(src->trait_precedences[i]));
	
	
	
	
	

	
	assert(sizeof(zend_trait_precedence_ptr) == sizeof((& src->trait_precedences[i])[0]));
	

	

	xc_calc_zend_trait_precedence_ptr(
		
		processor, & src->trait_precedences[i]
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->trait_precedences == NULL);

	
	
	


	}

#	endif
#else
	
		
	
	
	

	
		
	assert(sizeof(zend_uint) == sizeof(src->num_interfaces));
		
	
	
	
	
	
	


	

#endif

#	ifdef ZEND_ENGINE_2_4
	
	
	

	 
	
	
	
	
	
	
	
	
	if (src->info.user.filename == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, ZSTR(src->info.user.filename), strlen(src->info.user.filename) + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		
	assert(sizeof(zend_uint) == sizeof(src->info.user.line_start));
		
	
	
	

	

		
	assert(sizeof(zend_uint) == sizeof(src->info.user.line_end));
		
	
	
	

	

		
	assert(sizeof(zend_uint) == sizeof(src->info.user.doc_comment_len));
		
	
	
	

	

	 
	
#ifdef IS_UNICODE
	
	
	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->info.user.doc_comment) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_UNICODE, src->info.user.doc_comment, src->info.user.doc_comment_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->info.user.doc_comment) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->info.user.doc_comment, src->info.user.doc_comment_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->info.user.doc_comment) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->info.user.doc_comment, src->info.user.doc_comment_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

	
	
	

	
	
	
	

#	else
	 
	
	
	

	
	
	
	
	
	
	
	
	if (src->filename == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, ZSTR(src->filename), strlen(src->filename) + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		
	assert(sizeof(zend_uint) == sizeof(src->line_start));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->line_end));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->doc_comment_len));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_UNICODE, src->doc_comment, src->doc_comment_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->doc_comment, src->doc_comment_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->doc_comment, src->doc_comment_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

#	endif

	/* # NOT DONE */
#	ifdef ZEND_ENGINE_2_1
	
	
	
	

	
	
	
	

#	endif
	
	
	
	

	
	
	
	

	
	
	
	

	
	
	
	

#	ifdef ZEND_ENGINE_2_3
	
	
	
	

#	endif
#	ifdef ZEND_ENGINE_2_1
	
	
	
	

	
	
	
	

#	endif
	/* deal with it inside xc_fix_method */
	
	
	
	

	
	
	
	

	
	
	
	

	
	
	
	

	
	
	
	

/* should be >5.1 */
#	ifdef ZEND_ENGINE_2_1
	
	
	
	

	
	
	
	

#	endif
	
	
	
	

#	ifdef ZEND_CALLSTATIC_FUNC_NAME
	
	
	
	

#	endif
# if defined(ZEND_ENGINE_2_2) || PHP_MAJOR_VERSION >= 6
	
	
	
	

# endif
# if defined(ZEND_ENGINE_2_6)
	
	
	
	

# endif
#	ifndef ZEND_ENGINE_2_4
	/* # NOT DONE */
	
	
	
	

#	endif
#else /* ZEND_ENGINE_2 */
	
	
	
	

	
	
	
	

	
	
	
	

#endif
		
	
	assert(sizeof(HashTable) == sizeof(src->function_table));
	
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->function_table)[0]));
	

	

	xc_calc_HashTable_zend_function(
		
		processor, & src->function_table
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
		processor->active_class_entry_src = NULL;
		
	

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#ifdef ZEND_ENGINE_2_4


#else

	
	

/* {{{ xc_calc_znode */
	
 	  	 	static void inline 	xc_calc_znode( 		 		xc_processor_t *processor, const znode * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(xc_op_type) == sizeof(src->op_type));
			
	
	
	
	
	
	



	


#ifdef IS_CV
#	define XCACHE_IS_CV IS_CV
#else
/* compatible with zend optimizer */
#	define XCACHE_IS_CV 16
#endif
	assert(src->op_type == IS_CONST ||
		src->op_type == IS_VAR ||
		src->op_type == XCACHE_IS_CV ||
		src->op_type == IS_TMP_VAR ||
		src->op_type == IS_UNUSED);
		
	
	

	switch (src->op_type) {
		case IS_CONST:
			
	
	assert(sizeof(zval) == sizeof(src->u.constant));
	
	
	
	
	

	
	assert(sizeof(zval) == sizeof((& src->u.constant)[0]));
	

	

	xc_calc_zval(
		
		processor, & src->u.constant
		
		
		
		TSRMLS_CC
	);

	

	
	

	

			break;
		
		case IS_VAR:
		case IS_TMP_VAR:
		case XCACHE_IS_CV:
				
	assert(sizeof(zend_uint) == sizeof(src->u.var));
		
	
	
	

	

				
	assert(sizeof(zend_uint) == sizeof(src->u.EA.type));
		
	
	
	

	

			break;
		case IS_UNUSED:
			
				
	assert(sizeof(zend_uint) == sizeof(src->u.opline_num));
		
	
	
	

	

#ifndef ZEND_ENGINE_2
				
	assert(sizeof(zend_uint) == sizeof(src->u.fetch_type));
		
	
	
	

	

#endif
				
	assert(sizeof(zend_uint) == sizeof(src->u.EA.type));
		
	
	
	

	

			break;
		
	}
	
	
	

	
	
	
	

#if 0
	
	
	
	

#endif
#undef XCACHE_IS_CV

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_calc_zend_op */
	
		/* export:  	  	 	void 	xc_calc_zend_op( 		 		xc_processor_t *processor, const zend_op * const src 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_calc_zend_op( 		 		xc_processor_t *processor, const zend_op * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(xc_opcode) == sizeof(src->opcode));
			
	
	
	
	
	
	



	

#ifdef ZEND_ENGINE_2_4
	
	switch (src->opcode) {
	case ZEND_BIND_TRAITS:
		((zend_op *) src)->op2_type = IS_UNUSED;
		break;
	}
	
	#ifndef NDEBUG
	switch ((src->result_type & ~EXT_TYPE_UNUSED)) {
	case IS_CONST:
	case IS_VAR:
	case IS_CV:
	case IS_TMP_VAR:
	case IS_UNUSED:
		break;

	default:
		assert(0);
	}
#endif

		
	
	

	switch ((src->result_type & ~EXT_TYPE_UNUSED)) {
		case IS_CONST:
			
					
	assert(sizeof(zend_uint) == sizeof(src->result.constant));
		
	
	
	

	

			
			break;
		
		case IS_VAR:
		case IS_TMP_VAR:
		case IS_CV:
				
	assert(sizeof(zend_uint) == sizeof(src->result.var));
		
	
	
	

	

			break;
		case IS_UNUSED:
			
				
	assert(sizeof(zend_uint) == sizeof(src->result.opline_num));
		
	
	
	

	

			break;
		
	}
	
	
	

	
	
	
	


	#ifndef NDEBUG
	switch ((src->op1_type )) {
	case IS_CONST:
	case IS_VAR:
	case IS_CV:
	case IS_TMP_VAR:
	case IS_UNUSED:
		break;

	default:
		assert(0);
	}
#endif

		
	
	

	switch ((src->op1_type )) {
		case IS_CONST:
			
				
					
							
	assert(sizeof(zend_uint) == sizeof(src->op1.constant));
		
	
	
	

	

					
				
			
			break;
		
		case IS_VAR:
		case IS_TMP_VAR:
		case IS_CV:
				
	assert(sizeof(zend_uint) == sizeof(src->op1.var));
		
	
	
	

	

			break;
		case IS_UNUSED:
			
				
	assert(sizeof(zend_uint) == sizeof(src->op1.opline_num));
		
	
	
	

	

			break;
		
	}
	
	
	

	
	
	
	


	#ifndef NDEBUG
	switch ((src->op2_type )) {
	case IS_CONST:
	case IS_VAR:
	case IS_CV:
	case IS_TMP_VAR:
	case IS_UNUSED:
		break;

	default:
		assert(0);
	}
#endif

		
	
	

	switch ((src->op2_type )) {
		case IS_CONST:
			
				
					
							
	assert(sizeof(zend_uint) == sizeof(src->op2.constant));
		
	
	
	

	

					
				
			
			break;
		
		case IS_VAR:
		case IS_TMP_VAR:
		case IS_CV:
				
	assert(sizeof(zend_uint) == sizeof(src->op2.var));
		
	
	
	

	

			break;
		case IS_UNUSED:
			
				
	assert(sizeof(zend_uint) == sizeof(src->op2.opline_num));
		
	
	
	

	

			break;
		
	}
	
	
	

	
	
	
	


#else
	
	
	assert(sizeof(znode) == sizeof(src->result));
	
	
	
	
	

	
	assert(sizeof(znode) == sizeof((& src->result)[0]));
	

	

	xc_calc_znode(
		
		processor, & src->result
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
	assert(sizeof(znode) == sizeof(src->op1));
	
	
	
	
	

	
	assert(sizeof(znode) == sizeof((& src->op1)[0]));
	

	

	xc_calc_znode(
		
		processor, & src->op1
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
	assert(sizeof(znode) == sizeof(src->op2));
	
	
	
	
	

	
	assert(sizeof(znode) == sizeof((& src->op2)[0]));
	

	

	xc_calc_znode(
		
		processor, & src->op2
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


#endif
		
	assert(sizeof(ulong) == sizeof(src->extended_value));
		
	
	
	
	
	
	


	

		
	assert(sizeof(uint) == sizeof(src->lineno));
		
	
	
	
	
	
	


	

#ifdef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uchar) == sizeof(src->op1_type));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uchar) == sizeof(src->op2_type));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uchar) == sizeof(src->result_type));
		
	
	
	
	
	
	


	

#endif
	
#ifdef ZEND_ENGINE_2
		
	assert(sizeof(opcode_handler_t) == sizeof(src->handler));
	/* is copying enough? */
	
	
	

	

#endif

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#ifdef ZEND_ENGINE_2_4

	
	

/* {{{ xc_calc_zend_literal */
	
 	  	 	static void inline 	xc_calc_zend_literal( 		 		xc_processor_t *processor, const zend_literal * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
	
	assert(sizeof(zval) == sizeof(src->constant));
	
	
	
	
	

	
	assert(sizeof(zval) == sizeof((& src->constant)[0]));
	

	

	xc_calc_zval(
		
		processor, & src->constant
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


		
	assert(sizeof(zend_ulong) == sizeof(src->hash_value));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->cache_slot));
		
	
	
	
	
	
	


	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_calc_zend_op_array */
	
		/* export:  	  	 	void 	xc_calc_zend_op_array( 		 		xc_processor_t *processor, const zend_op_array * const src 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_calc_zend_op_array( 		 		xc_processor_t *processor, const zend_op_array * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
	
	{
	
	do {
	
	/* Common elements */
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->function_name) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_UNICODE, src->function_name, xc_zstrlen_uchar (src->function_name) + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->function_name) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->function_name, xc_zstrlen_char (src->function_name) + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->function_name) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->function_name, xc_zstrlen_char (src->function_name) + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

#ifdef ZEND_ENGINE_2
		
	assert(sizeof(zend_uint) == sizeof(src->fn_flags));
		
	
	
	
	
	
	


	

	
	if (src->arg_info) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zend_arg_info) * src->num_args;
	
	
	
	
	
	

			

			for (i = 0;
					i < src->num_args;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_arg_info) == sizeof(src->arg_info[i]));
	
	
	
	
	

	
	assert(sizeof(zend_arg_info) == sizeof((& src->arg_info[i])[0]));
	

	

	xc_calc_zend_arg_info(
		
		processor, & src->arg_info[i]
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->arg_info == NULL);

	
	
	


	}

		
	assert(sizeof(zend_uint) == sizeof(src->num_args));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->required_num_args));
		
	
	
	
	
	
	


	

#	ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_bool) == sizeof(src->pass_rest_by_reference));
		
	
	
	
	
	
	


	

#	endif
#else
	if (src->arg_types) {
		
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zend_uchar) * src->arg_types[0] + 1;
	
	
	
	
	
	

		
		
		
	
	
	

	}
	else {
		
	
	
	
	assert(src->arg_types == NULL);

	
	
	


	}
#endif
#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(unsigned char) == sizeof(src->return_reference));
		
	
	
	
	
	
	


	

#endif
	/* END of common elements */
#ifdef IS_UNICODE
	#endif

	
	
	if (src->refcount) {
		
		
	
	
	

	
	assert(sizeof(zend_uint) == sizeof(( src->refcount)[0]));
	
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zend_uint);
	
	
	
	
	
	


	

	xc_calc_zend_uint(
		
		processor,  src->refcount
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->refcount == NULL);

	}
	
	
	
	


	

#ifdef ZEND_ENGINE_2_4
		
	if (src->literals) {
		int i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zend_literal) * src->last_literal;
	
	
	
	
	
	

			

			for (i = 0;
					i < src->last_literal;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_literal) == sizeof(src->literals[i]));
	
	
	
	
	

	
	assert(sizeof(zend_literal) == sizeof((& src->literals[i])[0]));
	

	

	xc_calc_zend_literal(
		
		processor, & src->literals[i]
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->literals == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->last_literal));
		
	
	
	
	
	
	


	

#endif

		
	if (src->opcodes) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zend_op) * src->last;
	
	
	
	
	
	

			

			for (i = 0;
					i < src->last;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_op) == sizeof(src->opcodes[i]));
	
	
	
	
	

	
	assert(sizeof(zend_op) == sizeof((& src->opcodes[i])[0]));
	

	

	xc_calc_zend_op(
		
		processor, & src->opcodes[i]
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->opcodes == NULL);

	
	
	


	}

		
	assert(sizeof(zend_uint) == sizeof(src->last));
		
	
	
	
	
	
	


	

#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uint) == sizeof(src->size));
		
	
	
	
	
	
	


	

#endif

#ifdef IS_CV
	
	if (src->vars) {
		int i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zend_compiled_variable) * src->last_var;
	
	
	
	
	
	

			

			for (i = 0;
					i < src->last_var;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_compiled_variable) == sizeof(src->vars[i]));
	
	
	
	
	

	
	assert(sizeof(zend_compiled_variable) == sizeof((& src->vars[i])[0]));
	

	

	xc_calc_zend_compiled_variable(
		
		processor, & src->vars[i]
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->vars == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->last_var));
		
	
	
	
	
	
	


	

#	ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uint) == sizeof(src->size_var));
		
	
	
	
	
	
	


	

#	endif
#else
		
#endif

		
	assert(sizeof(zend_uint) == sizeof(src->T));
		
	
	
	
	
	
	


	


#ifdef ZEND_ENGINE_2_5
		
	assert(sizeof(zend_uint) == sizeof(src->nested_calls));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->used_stack));
		
	
	
	
	
	
	


	

#endif

	
	if (src->brk_cont_array) {
		last_brk_cont_t i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zend_brk_cont_element) * src->last_brk_cont;
	
	
	
	
	
	

			

			for (i = 0;
					i < src->last_brk_cont;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_brk_cont_element) == sizeof(src->brk_cont_array[i]));
	
	
	
	
	

	
	assert(sizeof(zend_brk_cont_element) == sizeof((& src->brk_cont_array[i])[0]));
	

	

	xc_calc_zend_brk_cont_element(
		
		processor, & src->brk_cont_array[i]
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->brk_cont_array == NULL);

	
	
	


	}

		
	assert(sizeof(last_brk_cont_t) == sizeof(src->last_brk_cont));
		
	
	
	
	
	
	



	

#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uint) == sizeof(src->current_brk_cont));
		
	
	
	
	
	
	


	

#endif
#ifndef ZEND_ENGINE_2
		
	assert(sizeof(zend_bool) == sizeof(src->uses_globals));
		
	
	
	
	
	
	


	

#endif

#ifdef ZEND_ENGINE_2
	
	if (src->try_catch_array) {
		int i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zend_try_catch_element) * src->last_try_catch;
	
	
	
	
	
	

			

			for (i = 0;
					i < src->last_try_catch;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_try_catch_element) == sizeof(src->try_catch_array[i]));
	
	
	
	
	

	
	assert(sizeof(zend_try_catch_element) == sizeof((& src->try_catch_array[i])[0]));
	

	

	xc_calc_zend_try_catch_element(
		
		processor, & src->try_catch_array[i]
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->try_catch_array == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->last_try_catch));
		
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_5
		
	assert(sizeof(zend_bool) == sizeof(src->has_finally_block));
		
	
	
	
	
	
	


	

#endif

	
	
	if (src->static_variables) {
		
		
	
	
	

	
	assert(sizeof(HashTable) == sizeof(( src->static_variables)[0]));
	
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(HashTable);
	
	
	
	
	
	


	

	xc_calc_HashTable_zval_ptr(
		
		processor,  src->static_variables
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->static_variables == NULL);

	}
	
	
	
	



#ifndef ZEND_ENGINE_2_4
	
	
	
	

		
	assert(sizeof(int) == sizeof(src->backpatch_count));
		
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_3
		
	assert(sizeof(zend_uint) == sizeof(src->this_var));
		
	
	
	
	
	
	


	

#endif

#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_bool) == sizeof(src->done_pass_two));
		
	
	
	
	
	
	


	

#endif
	/* 5.0 <= ver < 5.3 */
#if defined(ZEND_ENGINE_2) && !defined(ZEND_ENGINE_2_3)
		
	assert(sizeof(zend_bool) == sizeof(src->uses_this));
		
	
	
	
	
	
	


	

#endif

	 
	
	
	

	
	
	
	
	
	
	
	
	if (src->filename == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, ZSTR(src->filename), strlen(src->filename) + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

#ifdef IS_UNICODE
	
		 
	
	
	

	
	
	
	
	
	
	
	
	if (src->script_encoding == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, ZSTR(src->script_encoding), strlen(src->script_encoding) + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

	
#endif
#ifdef ZEND_ENGINE_2
		
	assert(sizeof(zend_uint) == sizeof(src->line_start));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->line_end));
		
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->doc_comment_len));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_UNICODE, src->doc_comment, src->doc_comment_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->doc_comment, src->doc_comment_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->doc_comment, src->doc_comment_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

#endif
#ifdef ZEND_COMPILE_DELAYED_BINDING
		
	assert(sizeof(zend_uint) == sizeof(src->early_binding));
		
	
	
	
	
	
	


	
;
#endif

	/* reserved */
	
	
	
	

#if defined(HARDENING_PATCH) && HARDENING_PATCH
		
	assert(sizeof(zend_bool) == sizeof(src->created_by_eval));
		
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_4
	
	
	
	

		
	assert(sizeof(int) == sizeof(src->last_cache_slot));
		
	
	
	
	
	
	


	

#endif
	} while (0);
	

#ifdef ZEND_ENGINE_2
		
		
	
	
	
	
	


#endif

#ifdef ZEND_ENGINE_2
	
	if (src->scope) {
		
		
#ifdef IS_UNICODE
		
#else
		
#endif
	}
	else {
		
	
	
	assert(src->scope == NULL);

	}

	
	
	

	
#endif

	
	}
	
	

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


#ifdef HAVE_XCACHE_CONSTANT

	
	

/* {{{ xc_calc_xc_constinfo_t */
	
 	  	 	static void inline 	xc_calc_xc_constinfo_t( 		 		xc_processor_t *processor, const xc_constinfo_t * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(zend_uint) == sizeof(src->key_size));
		
	
	
	
	
	
	


	

#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
	
	
	
	
	


	

#endif
	
		
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (src->type == IS_UNICODE) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->key) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_UNICODE, src->key, src->key_size C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->key, src->key_size C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->key, src->key_size C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
	
	
	
	
	


	

	
	
	assert(sizeof(zend_constant) == sizeof(src->constant));
	
	
	
	
	

	
	assert(sizeof(zend_constant) == sizeof((& src->constant)[0]));
	

	

	xc_calc_zend_constant(
		
		processor, & src->constant
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	



		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif


	
	

/* {{{ xc_calc_xc_op_array_info_detail_t */
	
 	  	 	static void inline 	xc_calc_xc_op_array_info_detail_t( 		 		xc_processor_t *processor, const xc_op_array_info_detail_t * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(zend_uint) == sizeof(src->index));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->info));
		
	
	
	
	
	
	


	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_calc_xc_op_array_info_t */
	
 	  	 	static void inline 	xc_calc_xc_op_array_info_t( 		 		xc_processor_t *processor, const xc_op_array_info_t * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
		#ifdef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uint) == sizeof(src->literalinfo_cnt));
		
	
	
	
	
	
	


	

	
	if (src->literalinfos) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(xc_op_array_info_detail_t) * src->literalinfo_cnt;
	
	
	
	
	
	

			

			for (i = 0;
					i < src->literalinfo_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_op_array_info_detail_t) == sizeof(src->literalinfos[i]));
	
	
	
	
	

	
	assert(sizeof(xc_op_array_info_detail_t) == sizeof((& src->literalinfos[i])[0]));
	

	

	xc_calc_xc_op_array_info_detail_t(
		
		processor, & src->literalinfos[i]
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->literalinfos == NULL);

	
	
	


	}

#else
		
	assert(sizeof(zend_uint) == sizeof(src->oplineinfo_cnt));
		
	
	
	
	
	
	


	

	
	if (src->oplineinfos) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(xc_op_array_info_detail_t) * src->oplineinfo_cnt;
	
	
	
	
	
	

			

			for (i = 0;
					i < src->oplineinfo_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_op_array_info_detail_t) == sizeof(src->oplineinfos[i]));
	
	
	
	
	

	
	assert(sizeof(xc_op_array_info_detail_t) == sizeof((& src->oplineinfos[i])[0]));
	

	

	xc_calc_xc_op_array_info_detail_t(
		
		processor, & src->oplineinfos[i]
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->oplineinfos == NULL);

	
	
	


	}

#endif

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	



	
	

/* {{{ xc_calc_xc_funcinfo_t */
	
		/* export:  	  	 	void 	xc_calc_xc_funcinfo_t( 		 		xc_processor_t *processor, const xc_funcinfo_t * const src 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_calc_xc_funcinfo_t( 		 		xc_processor_t *processor, const xc_funcinfo_t * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(zend_uint) == sizeof(src->key_size));
		
	
	
	
	
	
	


	

#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
	
	
	
	
	


	

#endif
	
		
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (src->type == IS_UNICODE) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->key) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_UNICODE, src->key, src->key_size C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->key, src->key_size C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->key, src->key_size C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
	
	
	
	
	


	

	
		
	
	assert(sizeof(xc_op_array_info_t) == sizeof(src->op_array_info));
	
	
	
	
	

	
	assert(sizeof(xc_op_array_info_t) == sizeof((& src->op_array_info)[0]));
	

	

	xc_calc_xc_op_array_info_t(
		
		processor, & src->op_array_info
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
	
	
	assert(sizeof(zend_function) == sizeof(src->func));
	
	
	
	
	

	
	assert(sizeof(zend_function) == sizeof((& src->func)[0]));
	

	

	xc_calc_zend_function(
		
		processor, & src->func
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	



		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_calc_xc_classinfo_t */
	
		/* export:  	  	 	void 	xc_calc_xc_classinfo_t( 		 		xc_processor_t *processor, const xc_classinfo_t * const src 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_calc_xc_classinfo_t( 		 		xc_processor_t *processor, const xc_classinfo_t * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(zend_uint) == sizeof(src->key_size));
		
	
	
	
	
	
	


	

#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
	
	
	
	
	


	

#endif
	
		
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (src->type == IS_UNICODE) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->key) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_UNICODE, src->key, src->key_size C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->key, src->key_size C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->key, src->key_size C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->methodinfo_cnt));
		
	
	
	
	
	
	


	

	
		
	if (src->methodinfos) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(xc_op_array_info_t) * src->methodinfo_cnt;
	
	
	
	
	
	

			

			for (i = 0;
					i < src->methodinfo_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_op_array_info_t) == sizeof(src->methodinfos[i]));
	
	
	
	
	

	
	assert(sizeof(xc_op_array_info_t) == sizeof((& src->methodinfos[i])[0]));
	

	

	xc_calc_xc_op_array_info_t(
		
		processor, & src->methodinfos[i]
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->methodinfos == NULL);

	
	
	


	}

	
	
#ifdef ZEND_ENGINE_2
	
	
	if (src->cest) {
		
		
	
	
	

	
	assert(sizeof(zend_class_entry) == sizeof(( src->cest)[0]));
	
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zend_class_entry);
	
	
	
	
	
	


	

	xc_calc_zend_class_entry(
		
		processor,  src->cest
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->cest == NULL);

	}
	
	
	
	


#else
	
	
	assert(sizeof(zend_class_entry) == sizeof(src->cest));
	
	
	
	
	

	
	assert(sizeof(zend_class_entry) == sizeof((& src->cest)[0]));
	

	

	xc_calc_zend_class_entry(
		
		processor, & src->cest
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


#endif
#ifndef ZEND_COMPILE_DELAYED_BINDING
		
	assert(sizeof(int) == sizeof(src->oplineno));
		
	
	
	
	
	
	


	

#endif

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


#ifdef ZEND_ENGINE_2_1

	
	

/* {{{ xc_calc_xc_autoglobal_t */
	
 	  	 	static void inline 	xc_calc_xc_autoglobal_t( 		 		xc_processor_t *processor, const xc_autoglobal_t * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(zend_uint) == sizeof(src->key_len));
		
	
	
	
	
	
	


	

#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
	
	
	
	
	


	

#endif
	
		 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (src->type == IS_UNICODE) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->key) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_UNICODE, src->key, src->key_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->key, src->key_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->key, src->key_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
	
	
	
	
	


	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif


#ifdef XCACHE_ERROR_CACHING

	
	

/* {{{ xc_calc_xc_compilererror_t */
	
 	  	 	static void inline 	xc_calc_xc_compilererror_t( 		 		xc_processor_t *processor, const xc_compilererror_t * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(int) == sizeof(src->type));
		
	
	
	
	
	
	


	

		
	assert(sizeof(uint) == sizeof(src->lineno));
		
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->error_len));
		
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->error == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, ZSTR(src->error), src->error_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif


	
	

/* {{{ xc_calc_xc_entry_data_php_t */
	
		/* export:  	  	 	void 	xc_calc_xc_entry_data_php_t( 		 		xc_processor_t *processor, const xc_entry_data_php_t * const src 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_calc_xc_entry_data_php_t( 		 		xc_processor_t *processor, const xc_entry_data_php_t * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			

	/* skip */
	
	
	
	

		
	assert(sizeof(xc_hash_value_t) == sizeof(src->hvalue));
		
	
	
	
	
	
	


	

		
	assert(sizeof(xc_md5sum_t) == sizeof(src->md5));
	
	
	
	

	

		
	assert(sizeof(zend_ulong) == sizeof(src->refcount));
		
	
	
	
	
	
	


	


		
	assert(sizeof(zend_ulong) == sizeof(src->hits));
		
	
	
	
	
	
	


	

		
	assert(sizeof(size_t) == sizeof(src->size));
		
	
	
	
	
	
	


	


	
		
	
	assert(sizeof(xc_op_array_info_t) == sizeof(src->op_array_info));
	
	
	
	
	

	
	assert(sizeof(xc_op_array_info_t) == sizeof((& src->op_array_info)[0]));
	

	

	xc_calc_xc_op_array_info_t(
		
		processor, & src->op_array_info
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
	
	
	if (src->op_array) {
		
		
	
	
	

	
	assert(sizeof(zend_op_array) == sizeof(( src->op_array)[0]));
	
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(zend_op_array);
	
	
	
	
	
	


	

	xc_calc_zend_op_array(
		
		processor,  src->op_array
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->op_array == NULL);

	}
	
	
	
	



#ifdef HAVE_XCACHE_CONSTANT
		
	assert(sizeof(zend_uint) == sizeof(src->constinfo_cnt));
		
	
	
	
	
	
	


	

	
	if (src->constinfos) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(xc_constinfo_t) * src->constinfo_cnt;
	
	
	
	
	
	

			

			for (i = 0;
					i < src->constinfo_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_constinfo_t) == sizeof(src->constinfos[i]));
	
	
	
	
	

	
	assert(sizeof(xc_constinfo_t) == sizeof((& src->constinfos[i])[0]));
	

	

	xc_calc_xc_constinfo_t(
		
		processor, & src->constinfos[i]
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->constinfos == NULL);

	
	
	


	}

#endif

		
	assert(sizeof(zend_uint) == sizeof(src->funcinfo_cnt));
		
	
	
	
	
	
	


	

	
	if (src->funcinfos) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(xc_funcinfo_t) * src->funcinfo_cnt;
	
	
	
	
	
	

			

			for (i = 0;
					i < src->funcinfo_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_funcinfo_t) == sizeof(src->funcinfos[i]));
	
	
	
	
	

	
	assert(sizeof(xc_funcinfo_t) == sizeof((& src->funcinfos[i])[0]));
	

	

	xc_calc_xc_funcinfo_t(
		
		processor, & src->funcinfos[i]
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->funcinfos == NULL);

	
	
	


	}


		
	assert(sizeof(zend_uint) == sizeof(src->classinfo_cnt));
		
	
	
	
	
	
	


	

	
	if (src->classinfos) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(xc_classinfo_t) * src->classinfo_cnt;
	
	
	
	
	
	

			

			for (i = 0;
					i < src->classinfo_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_classinfo_t) == sizeof(src->classinfos[i]));
	
	
	
	
	

	
	assert(sizeof(xc_classinfo_t) == sizeof((& src->classinfos[i])[0]));
	

	

	xc_calc_xc_classinfo_t(
		
		processor, & src->classinfos[i]
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->classinfos == NULL);

	
	
	


	}

#ifdef ZEND_ENGINE_2_1
		
	assert(sizeof(zend_uint) == sizeof(src->autoglobal_cnt));
		
	
	
	
	
	
	


	

	
		
	if (src->autoglobals) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(xc_autoglobal_t) * src->autoglobal_cnt;
	
	
	
	
	
	

			

			for (i = 0;
					i < src->autoglobal_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_autoglobal_t) == sizeof(src->autoglobals[i]));
	
	
	
	
	

	
	assert(sizeof(xc_autoglobal_t) == sizeof((& src->autoglobals[i])[0]));
	

	

	xc_calc_xc_autoglobal_t(
		
		processor, & src->autoglobals[i]
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->autoglobals == NULL);

	
	
	


	}

	
#endif
#ifdef XCACHE_ERROR_CACHING
		
	assert(sizeof(zend_uint) == sizeof(src->compilererror_cnt));
		
	
	
	
	
	
	


	

	
		
	if (src->compilererrors) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
		
		processor->size = (size_t) ALIGN(processor->size);
		processor->size += sizeof(xc_compilererror_t) * src->compilererror_cnt;
	
	
	
	
	
	

			

			for (i = 0;
					i < src->compilererror_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_compilererror_t) == sizeof(src->compilererrors[i]));
	
	
	
	
	

	
	assert(sizeof(xc_compilererror_t) == sizeof((& src->compilererrors[i])[0]));
	

	

	xc_calc_xc_compilererror_t(
		
		processor, & src->compilererrors[i]
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->compilererrors == NULL);

	
	
	


	}

	
#endif
#ifndef ZEND_COMPILE_DELAYED_BINDING
		
	assert(sizeof(zend_bool) == sizeof(src->have_early_binding));
		
	
	
	
	
	
	


	

#endif
		
	assert(sizeof(zend_bool) == sizeof(src->have_references));
		
	
	
	
	
	
	


	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_calc_xc_entry_t */
	
 	  	 	static void inline 	xc_calc_xc_entry_t( 		 		xc_processor_t *processor, const xc_entry_t * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			/* skip */
	
	
	
	

		
	assert(sizeof(size_t) == sizeof(src->size));
		
	
	
	
	
	
	


	


		
	assert(sizeof(time_t) == sizeof(src->ctime));
		
	
	
	
	
	
	


	

		
	assert(sizeof(time_t) == sizeof(src->atime));
		
	
	
	
	
	
	


	

		
	assert(sizeof(time_t) == sizeof(src->dtime));
		
	
	
	
	
	
	


	

		
	assert(sizeof(long) == sizeof(src->ttl));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_ulong) == sizeof(src->hits));
		
	
	
	
	
	
	


	

	
	
	
	
 
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_calc_xc_entry_php_t */
	
		/* export:  	  	 	void 	xc_calc_xc_entry_php_t( 		 		xc_processor_t *processor, const xc_entry_php_t * const src 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_calc_xc_entry_php_t( 		 		xc_processor_t *processor, const xc_entry_php_t * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
	
	assert(sizeof(xc_entry_t) == sizeof(src->entry));
	
	
	
	
	

	
	assert(sizeof(xc_entry_t) == sizeof((& src->entry)[0]));
	

	

	xc_calc_xc_entry_t(
		
		processor, & src->entry
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
	

			
	assert(sizeof(int) == sizeof(src->entry.name.str.len));
		
	
	
	

	

		
			  
	
	
	
	
	
	
	
	
	if (src->entry.name.str.val == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, ZSTR(src->entry.name.str.val), src->entry.name.str.len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		
	
	
	


	
	
	
	


		
	assert(sizeof(long) == sizeof(src->refcount));
		
	
	
	
	
	
	


	

		
	assert(sizeof(time_t) == sizeof(src->file_mtime));
		
	
	
	
	
	
	


	

		
	assert(sizeof(size_t) == sizeof(src->file_size));
		
	
	
	
	
	
	


	

		
	assert(sizeof(size_t) == sizeof(src->file_device));
		
	
	
	
	
	
	


	

		
	assert(sizeof(size_t) == sizeof(src->file_inode));
		
	
	
	
	
	
	


	


		
	assert(sizeof(size_t) == sizeof(src->filepath_len));
		
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->filepath == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, ZSTR(src->filepath), src->filepath_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		
	assert(sizeof(size_t) == sizeof(src->dirpath_len));
		
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->dirpath == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, ZSTR(src->dirpath), src->dirpath_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

#ifdef IS_UNICODE
		
	assert(sizeof(int) == sizeof(src->ufilepath_len));
		
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->ufilepath == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_UNICODE, ZSTR(src->ufilepath), src->ufilepath_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		
	assert(sizeof(int) == sizeof(src->udirpath_len));
		
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->udirpath == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_UNICODE, ZSTR(src->udirpath), src->udirpath_len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

#endif

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_calc_xc_entry_var_t */
	
		/* export:  	  	 	void 	xc_calc_xc_entry_var_t( 		 		xc_processor_t *processor, const xc_entry_var_t * const src 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_calc_xc_entry_var_t( 		 		xc_processor_t *processor, const xc_entry_var_t * const src 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
	
	assert(sizeof(xc_entry_t) == sizeof(src->entry));
	
	
	
	
	

	
	assert(sizeof(xc_entry_t) == sizeof((& src->entry)[0]));
	

	

	xc_calc_xc_entry_t(
		
		processor, & src->entry
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	



#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->name_type));
		
	
	
	
	
	
	


	

#endif
		
	
	

#ifdef IS_UNICODE
		if (src->name_type == IS_UNICODE) {
				
	assert(sizeof(int32_t) == sizeof(src->entry.name.ustr.len));
		
	
	
	

	

		}
		else {
				
	assert(sizeof(int) == sizeof(src->entry.name.str.len));
		
	
	
	

	

		}
#else
			
	assert(sizeof(int) == sizeof(src->entry.name.str.len));
		
	
	
	

	

#endif
		
#ifdef IS_UNICODE
			 
	
#ifdef IS_UNICODE
	
	
	
		if (src->name_type == IS_UNICODE) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->entry.name.uni.val) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_UNICODE, src->entry.name.uni.val, src->entry.name.uni.len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->entry.name.uni.val) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->entry.name.uni.val, src->entry.name.uni.len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->entry.name.uni.val) == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, src->entry.name.uni.val, src->entry.name.uni.len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

#else
			  
	
	
	
	
	
	
	
	
	if (src->entry.name.str.val == NULL) {
		
		
	}
	else {
		
		xc_calc_string_n(processor, IS_STRING, ZSTR(src->entry.name.str.val), src->entry.name.str.len + 1 C_RELAYLINE);
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
		
	
	
	

	
	
	
	
	
	

	
	assert(sizeof(zval_ptr) == sizeof((& src->value)[0]));
	

	

	xc_calc_zval_ptr(
		
		processor, & src->value
		
		
		
		TSRMLS_CC
	);

	

	
	

		
	assert(sizeof(zend_bool) == sizeof(src->have_references));
		
	
	
	
	
	
	


	

	
	
	
	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	



  /* {{{ Pre-declare */
 	  	 	void 	xc_store_zval( 		 		 		xc_processor_t *processor, zval *dst, const zval * const src 		 		 		TSRMLS_DC 	); 	
 	  	 	static void inline 	xc_store_zval_ptr( 		 		 		xc_processor_t *processor, zval_ptr *dst, const zval_ptr * const src 		 		 		TSRMLS_DC 	); 	
 	  	 	static void inline 	xc_store_zval_ptr_nullable( 		 		 		xc_processor_t *processor, zval_ptr_nullable *dst, const zval_ptr_nullable * const src 		 		 		TSRMLS_DC 	); 	
 	  	 	void 	xc_store_zend_op_array( 		 		 		xc_processor_t *processor, zend_op_array *dst, const zend_op_array * const src 		 		 		TSRMLS_DC 	); 	
 	  	 	void 	xc_store_zend_class_entry( 		 		 		xc_processor_t *processor, zend_class_entry *dst, const zend_class_entry * const src 		 		 		TSRMLS_DC 	); 	
#ifdef HAVE_XCACHE_CONSTANT
 	  	 	static void inline 	xc_store_zend_constant( 		 		 		xc_processor_t *processor, zend_constant *dst, const zend_constant * const src 		 		 		TSRMLS_DC 	); 	
#endif
 	  	 	void 	xc_store_zend_function( 		 		 		xc_processor_t *processor, zend_function *dst, const zend_function * const src 		 		 		TSRMLS_DC 	); 	
 	  	 	void 	xc_store_xc_entry_var_t( 		 		 		xc_processor_t *processor, xc_entry_var_t *dst, const xc_entry_var_t * const src 		 		 		TSRMLS_DC 	); 	
 	  	 	void 	xc_store_xc_entry_php_t( 		 		 		xc_processor_t *processor, xc_entry_php_t *dst, const xc_entry_php_t * const src 		 		 		TSRMLS_DC 	); 	
#ifdef ZEND_ENGINE_2
 	  	 	static void inline 	xc_store_zend_property_info( 		 		 		xc_processor_t *processor, zend_property_info *dst, const zend_property_info * const src 		 		 		TSRMLS_DC 	); 	
#endif
/* }}} */
#ifdef IS_CV

	
	

/* {{{ xc_store_zend_compiled_variable */
	
 	  	 	static void inline 	xc_store_zend_compiled_variable( 		 		 		xc_processor_t *processor, zend_compiled_variable *dst, const zend_compiled_variable * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_compiled_variable));
			do {
		

		
				
	assert(sizeof(int) == sizeof(src->name_len));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_U(dst->name) =  ZSTR_U(xc_store_string_n(processor, IS_UNICODE, src->name, src->name_len + 1 C_RELAYLINE));
		
		
	ZSTR_U(dst->name) = (UChar *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_U(dst->name));

		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->name) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->name, src->name_len + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->name) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->name));

		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->name) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->name, src->name_len + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->name) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->name));

		
	}
	
	
	
	
	
	
	

#endif
	

		
	assert(sizeof(ulong) == sizeof(src->hash_value));
		
	
	
	
	
	
	


	


		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_store_zend_uint */
	
 	  	 	static void inline 	xc_store_zend_uint( 		 		 		xc_processor_t *processor, zend_uint *dst, const zend_uint * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_uint));
			do {
		

		
			dst[0] = src[0];
	
	

		
		
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#ifndef ZEND_ENGINE_2

	
	

/* {{{ xc_store_int */
	
 	  	 	static void inline 	xc_store_int( 		 		 		xc_processor_t *processor, int *dst, const int * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(int));
			do {
		

		
			*dst = *src;
	
	

		
		
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif
#ifdef ZEND_ENGINE_2

	
	

/* {{{ xc_store_zend_try_catch_element */
	
 	  	 	static void inline 	xc_store_zend_try_catch_element( 		 		 		xc_processor_t *processor, zend_try_catch_element *dst, const zend_try_catch_element * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_try_catch_element));
			do {
		

		
				
	assert(sizeof(zend_uint) == sizeof(src->try_op));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->catch_op));
		
	
	
	
	
	
	


	

#ifdef ZEND_ENGINE_2_5
		
	assert(sizeof(zend_uint) == sizeof(src->finally_op));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->finally_end));
		
	
	
	
	
	
	


	

#endif

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_store_zend_brk_cont_element */
	
 	  	 	static void inline 	xc_store_zend_brk_cont_element( 		 		 		xc_processor_t *processor, zend_brk_cont_element *dst, const zend_brk_cont_element * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_brk_cont_element));
			do {
		

		
		#ifdef ZEND_ENGINE_2_2
		
	assert(sizeof(int) == sizeof(src->start));
		
	
	
	
	
	
	


	

#endif
		
	assert(sizeof(int) == sizeof(src->cont));
		
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->brk));
		
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->parent));
		
	
	
	
	
	
	


	


		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	
	

/* {{{ xc_store_HashTable_zval_ptr */
	
 	  	 	static void inline 	xc_store_HashTable_zval_ptr( 		 		 		xc_processor_t *processor, HashTable *dst, const HashTable * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(HashTable));
			do {
		

		
		
		
	

				
				Bucket *srcBucket;
		Bucket *pnew = NULL, *prev = NULL;
		zend_bool first = 1;
				uint n;
		size_t bucketsize;

#if defined(HARDENING_PATCH_HASH_PROTECT) && HARDENING_PATCH_HASH_PROTECT
		
							
	assert(sizeof(unsigned int) == sizeof(src->canary));
		
	
	
	
	
	
	


	

		
#endif
			
	assert(sizeof(uint) == sizeof(src->nTableSize));
		
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nTableMask));
		
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nNumOfElements));
		
	
	
	
	
	
	


	

			
	assert(sizeof(ulong) == sizeof(src->nNextFreeElement));
		
	
	
	
	
	
	


	

		dst->pInternalPointer = NULL;	/* Used for element traversal */ 
	
	
	

		dst->pListHead = NULL; 
	
	
	

#ifdef ZEND_ENGINE_2_4
	if (src->nTableMask) {
#endif
		
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->arBuckets = (Bucket* *) (processor->p = (char *) ALIGN(processor->p));
		
				memset(dst->arBuckets, 0, sizeof(Bucket*) * src->nTableSize);
		
		processor->p += sizeof(Bucket*) * src->nTableSize;

		
	
	
	
	
	

		
	
	
	

		
	
	

		for (srcBucket = src->pListHead; srcBucket != NULL; srcBucket = srcBucket->pListNext) {
			

			bucketsize = BUCKET_SIZE(srcBucket);
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		pnew = (Bucket *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(char) * bucketsize;

		
	
	
	
	
	

			
#ifdef ZEND_ENGINE_2_4
				memcpy(pnew, srcBucket, BUCKET_HEAD_SIZE(Bucket));
				if (BUCKET_KEY_SIZE(srcBucket)) {
					memcpy((char *) (pnew + 1), srcBucket->arKey, BUCKET_KEY_SIZE(srcBucket));
					pnew->arKey = (const char *) (pnew + 1);
				}
				else {
					pnew->arKey = NULL;
				}
#else
				memcpy(pnew, srcBucket, bucketsize);
#endif
				n = srcBucket->h & src->nTableMask;
				/* pnew into hash node chain */
				pnew->pLast = NULL;
				pnew->pNext = dst->arBuckets[n];
				if (pnew->pNext) {
					pnew->pNext->pLast = pnew;
				}
				dst->arBuckets[n] = pnew;
			
			
			if (sizeof(void *) == sizeof(zval_ptr)) {
				pnew->pData = &pnew->pDataPtr;
								
	
	
	

	
	assert(sizeof(zval_ptr) == sizeof((  ((zval_ptr*)srcBucket->pData))[0]));
	

	

	xc_store_zval_ptr(
		
		
		processor,   pnew->pData,   ((zval_ptr*)srcBucket->pData)
		
		
		TSRMLS_CC
	);

	

	
	

			}
			else {
				
	
	
	

	
	assert(sizeof(zval_ptr) == sizeof(( ((zval_ptr*)srcBucket->pData))[0]));
	
	
	
		
	
	
	/* allocate */
	
	
		
		
		pnew->pData = (zval_ptr *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zval_ptr);

		
	
	
	
	
	


	

	xc_store_zval_ptr(
		
		
		processor,  pnew->pData,  ((zval_ptr*)srcBucket->pData)
		
		
		TSRMLS_CC
	);

	

	
	
	pnew->pData = (zval_ptr *) processor->shm->handlers->to_readonly(processor->shm, (void *)pnew->pData);


				pnew->pDataPtr = NULL;
			}

			if (first) {
				dst->pListHead = pnew;
				first = 0;
			}

			
				/* flat link */
				pnew->pListLast = prev;
				pnew->pListNext = NULL;
				if (prev) {
					prev->pListNext = pnew;
				}
				prev = pnew;
			
		}
		
	
	

				
	dst->arBuckets = (Bucket * *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->arBuckets);

#ifdef ZEND_ENGINE_2_4
	}
	else { /* if (src->nTableMask) */
		
	
	
	

	}
#endif
		dst->pListTail = pnew; 
	
	
	

		dst->pDestructor = src->pDestructor; 
	
	
	

			
	assert(sizeof(zend_bool) == sizeof(src->persistent));
		
	
	
	
	
	
	


	

#ifdef IS_UNICODE
			
	assert(sizeof(zend_bool) == sizeof(src->unicode));
		
	
	
	
	
	
	


	

#endif
			
	assert(sizeof(unsigned char) == sizeof(src->nApplyCount));
		
	
	
	
	
	
	


	

			
	assert(sizeof(zend_bool) == sizeof(src->bApplyProtection));
		
	
	
	
	
	
	


	

#if ZEND_DEBUG
			
	assert(sizeof(int) == sizeof(src->inconsistent));
		
	
	
	
	
	
	


	

#endif
				
	
		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	



	
	
	

/* {{{ xc_store_HashTable_zend_function */
	
		/* export:  	  	 	void 	xc_store_HashTable_zend_function( 		 		 		xc_processor_t *processor, HashTable *dst, const HashTable * const src 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_store_HashTable_zend_function( 		 		 		xc_processor_t *processor, HashTable *dst, const HashTable * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(HashTable));
			do {
		

		
		
		
	

				
				Bucket *srcBucket;
		Bucket *pnew = NULL, *prev = NULL;
		zend_bool first = 1;
				uint n;
		size_t bucketsize;

#if defined(HARDENING_PATCH_HASH_PROTECT) && HARDENING_PATCH_HASH_PROTECT
		
							
	assert(sizeof(unsigned int) == sizeof(src->canary));
		
	
	
	
	
	
	


	

		
#endif
			
	assert(sizeof(uint) == sizeof(src->nTableSize));
		
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nTableMask));
		
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nNumOfElements));
		
	
	
	
	
	
	


	

			
	assert(sizeof(ulong) == sizeof(src->nNextFreeElement));
		
	
	
	
	
	
	


	

		dst->pInternalPointer = NULL;	/* Used for element traversal */ 
	
	
	

		dst->pListHead = NULL; 
	
	
	

#ifdef ZEND_ENGINE_2_4
	if (src->nTableMask) {
#endif
		
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->arBuckets = (Bucket* *) (processor->p = (char *) ALIGN(processor->p));
		
				memset(dst->arBuckets, 0, sizeof(Bucket*) * src->nTableSize);
		
		processor->p += sizeof(Bucket*) * src->nTableSize;

		
	
	
	
	
	

		
	
	
	

		
	
	

		for (srcBucket = src->pListHead; srcBucket != NULL; srcBucket = srcBucket->pListNext) {
			

			bucketsize = BUCKET_SIZE(srcBucket);
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		pnew = (Bucket *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(char) * bucketsize;

		
	
	
	
	
	

			
#ifdef ZEND_ENGINE_2_4
				memcpy(pnew, srcBucket, BUCKET_HEAD_SIZE(Bucket));
				if (BUCKET_KEY_SIZE(srcBucket)) {
					memcpy((char *) (pnew + 1), srcBucket->arKey, BUCKET_KEY_SIZE(srcBucket));
					pnew->arKey = (const char *) (pnew + 1);
				}
				else {
					pnew->arKey = NULL;
				}
#else
				memcpy(pnew, srcBucket, bucketsize);
#endif
				n = srcBucket->h & src->nTableMask;
				/* pnew into hash node chain */
				pnew->pLast = NULL;
				pnew->pNext = dst->arBuckets[n];
				if (pnew->pNext) {
					pnew->pNext->pLast = pnew;
				}
				dst->arBuckets[n] = pnew;
			
			
			if (sizeof(void *) == sizeof(zend_function)) {
				pnew->pData = &pnew->pDataPtr;
								
	
	
	

	
	assert(sizeof(zend_function) == sizeof((  ((zend_function*)srcBucket->pData))[0]));
	

	

	xc_store_zend_function(
		
		
		processor,   pnew->pData,   ((zend_function*)srcBucket->pData)
		
		
		TSRMLS_CC
	);

	

	
	

			}
			else {
				
	
	
	

	
	assert(sizeof(zend_function) == sizeof(( ((zend_function*)srcBucket->pData))[0]));
	
	
	
		
	
	
	/* allocate */
	
	
		
		
		pnew->pData = (zend_function *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zend_function);

		
	
	
	
	
	


	

	xc_store_zend_function(
		
		
		processor,  pnew->pData,  ((zend_function*)srcBucket->pData)
		
		
		TSRMLS_CC
	);

	

	
	
	pnew->pData = (zend_function *) processor->shm->handlers->to_readonly(processor->shm, (void *)pnew->pData);


				pnew->pDataPtr = NULL;
			}

			if (first) {
				dst->pListHead = pnew;
				first = 0;
			}

			
				/* flat link */
				pnew->pListLast = prev;
				pnew->pListNext = NULL;
				if (prev) {
					prev->pListNext = pnew;
				}
				prev = pnew;
			
		}
		
	
	

				
	dst->arBuckets = (Bucket * *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->arBuckets);

#ifdef ZEND_ENGINE_2_4
	}
	else { /* if (src->nTableMask) */
		
	
	
	

	}
#endif
		dst->pListTail = pnew; 
	
	
	

		dst->pDestructor = src->pDestructor; 
	
	
	

			
	assert(sizeof(zend_bool) == sizeof(src->persistent));
		
	
	
	
	
	
	


	

#ifdef IS_UNICODE
			
	assert(sizeof(zend_bool) == sizeof(src->unicode));
		
	
	
	
	
	
	


	

#endif
			
	assert(sizeof(unsigned char) == sizeof(src->nApplyCount));
		
	
	
	
	
	
	


	

			
	assert(sizeof(zend_bool) == sizeof(src->bApplyProtection));
		
	
	
	
	
	
	


	

#if ZEND_DEBUG
			
	assert(sizeof(int) == sizeof(src->inconsistent));
		
	
	
	
	
	
	


	

#endif
				
	
		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


#ifdef ZEND_ENGINE_2

	
	
	

/* {{{ xc_store_HashTable_zend_property_info */
	
 	  	 	static void inline 	xc_store_HashTable_zend_property_info( 		 		 		xc_processor_t *processor, HashTable *dst, const HashTable * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(HashTable));
			do {
		

		
		
		
	

				
				Bucket *srcBucket;
		Bucket *pnew = NULL, *prev = NULL;
		zend_bool first = 1;
				uint n;
		size_t bucketsize;

#if defined(HARDENING_PATCH_HASH_PROTECT) && HARDENING_PATCH_HASH_PROTECT
		
							
	assert(sizeof(unsigned int) == sizeof(src->canary));
		
	
	
	
	
	
	


	

		
#endif
			
	assert(sizeof(uint) == sizeof(src->nTableSize));
		
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nTableMask));
		
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nNumOfElements));
		
	
	
	
	
	
	


	

			
	assert(sizeof(ulong) == sizeof(src->nNextFreeElement));
		
	
	
	
	
	
	


	

		dst->pInternalPointer = NULL;	/* Used for element traversal */ 
	
	
	

		dst->pListHead = NULL; 
	
	
	

#ifdef ZEND_ENGINE_2_4
	if (src->nTableMask) {
#endif
		
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->arBuckets = (Bucket* *) (processor->p = (char *) ALIGN(processor->p));
		
				memset(dst->arBuckets, 0, sizeof(Bucket*) * src->nTableSize);
		
		processor->p += sizeof(Bucket*) * src->nTableSize;

		
	
	
	
	
	

		
	
	
	

		
	
	

		for (srcBucket = src->pListHead; srcBucket != NULL; srcBucket = srcBucket->pListNext) {
			

			bucketsize = BUCKET_SIZE(srcBucket);
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		pnew = (Bucket *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(char) * bucketsize;

		
	
	
	
	
	

			
#ifdef ZEND_ENGINE_2_4
				memcpy(pnew, srcBucket, BUCKET_HEAD_SIZE(Bucket));
				if (BUCKET_KEY_SIZE(srcBucket)) {
					memcpy((char *) (pnew + 1), srcBucket->arKey, BUCKET_KEY_SIZE(srcBucket));
					pnew->arKey = (const char *) (pnew + 1);
				}
				else {
					pnew->arKey = NULL;
				}
#else
				memcpy(pnew, srcBucket, bucketsize);
#endif
				n = srcBucket->h & src->nTableMask;
				/* pnew into hash node chain */
				pnew->pLast = NULL;
				pnew->pNext = dst->arBuckets[n];
				if (pnew->pNext) {
					pnew->pNext->pLast = pnew;
				}
				dst->arBuckets[n] = pnew;
			
			
			if (sizeof(void *) == sizeof(zend_property_info)) {
				pnew->pData = &pnew->pDataPtr;
								
	
	
	

	
	assert(sizeof(zend_property_info) == sizeof((  ((zend_property_info*)srcBucket->pData))[0]));
	

	

	xc_store_zend_property_info(
		
		
		processor,   pnew->pData,   ((zend_property_info*)srcBucket->pData)
		
		
		TSRMLS_CC
	);

	

	
	

			}
			else {
				
	
	
	

	
	assert(sizeof(zend_property_info) == sizeof(( ((zend_property_info*)srcBucket->pData))[0]));
	
	
	
		
	
	
	/* allocate */
	
	
		
		
		pnew->pData = (zend_property_info *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zend_property_info);

		
	
	
	
	
	


	

	xc_store_zend_property_info(
		
		
		processor,  pnew->pData,  ((zend_property_info*)srcBucket->pData)
		
		
		TSRMLS_CC
	);

	

	
	
	pnew->pData = (zend_property_info *) processor->shm->handlers->to_readonly(processor->shm, (void *)pnew->pData);


				pnew->pDataPtr = NULL;
			}

			if (first) {
				dst->pListHead = pnew;
				first = 0;
			}

			
				/* flat link */
				pnew->pListLast = prev;
				pnew->pListNext = NULL;
				if (prev) {
					prev->pListNext = pnew;
				}
				prev = pnew;
			
		}
		
	
	

				
	dst->arBuckets = (Bucket * *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->arBuckets);

#ifdef ZEND_ENGINE_2_4
	}
	else { /* if (src->nTableMask) */
		
	
	
	

	}
#endif
		dst->pListTail = pnew; 
	
	
	

		dst->pDestructor = src->pDestructor; 
	
	
	

			
	assert(sizeof(zend_bool) == sizeof(src->persistent));
		
	
	
	
	
	
	


	

#ifdef IS_UNICODE
			
	assert(sizeof(zend_bool) == sizeof(src->unicode));
		
	
	
	
	
	
	


	

#endif
			
	assert(sizeof(unsigned char) == sizeof(src->nApplyCount));
		
	
	
	
	
	
	


	

			
	assert(sizeof(zend_bool) == sizeof(src->bApplyProtection));
		
	
	
	
	
	
	


	

#if ZEND_DEBUG
			
	assert(sizeof(int) == sizeof(src->inconsistent));
		
	
	
	
	
	
	


	

#endif
				
	
		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


#endif
#ifdef IS_CONSTANT_AST


	
	

/* {{{ xc_store_zend_ast */
	
		/* export:  	  	 	void 	xc_store_zend_ast( 		 		 		xc_processor_t *processor, zend_ast *dst, const zend_ast * const src 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_store_zend_ast( 		 		 		xc_processor_t *processor, zend_ast *dst, const zend_ast * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_ast));
			do {
		

		
				zend_ushort i;
			
	assert(sizeof(zend_ushort) == sizeof(src->kind));
		
	
	
	
	
	
	


	

			
	assert(sizeof(zend_ushort) == sizeof(src->children));
		
	
	
	
	
	
	


	

		
	
	
	

		
	
	

			if (src->kind == ZEND_CONST) {
				assert(src->u.val);
				
					dst->u.val = (zval *) (dst + 1);
					memcpy(dst->u.val, src->u.val, sizeof(zval));
				
				
	
	
	

	
	assert(sizeof(zval) == sizeof((  src->u.val)[0]));
	

	

	xc_store_zval(
		
		
		processor,   dst->u.val,   src->u.val
		
		
		TSRMLS_CC
	);

	

	
	

				
	dst->u.val = (zval *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->u.val);

			}
			else {
				for (i = 0; i < src->children; ++i) {
					zend_ast *src_ast = (&src->u.child)[i];
					if (src_ast) {
						{
	
		size_t zend_ast_size = (src_ast->kind == ZEND_CONST)
		 ? sizeof(zend_ast) + sizeof(zval)
		 : sizeof(zend_ast) + sizeof(zend_ast *) * (src_ast->children - 1);
	

	
	
							
	
	
		
	
	
	/* allocate */
	
	
		
		
		(&dst->u.child)[i] = (zend_ast *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += zend_ast_size;

		
	
	
	
	
	

							
	
	
	

	
	assert(sizeof(zend_ast) == sizeof((  src_ast)[0]));
	

	

	xc_store_zend_ast(
		
		
		processor,   (&dst->u.child)[i],   src_ast
		
		
		TSRMLS_CC
	);

	

	
	

						
	
}

						
	(&dst->u.child)[i] = (zend_ast *) processor->shm->handlers->to_readonly(processor->shm, (void *)(&dst->u.child)[i]);

					}
					else {
						(&dst->u.child)[i] = NULL;
					}
				}
			}
		
	
	


		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_store_zval */
	
		/* export:  	  	 	void 	xc_store_zval( 		 		 		xc_processor_t *processor, zval *dst, const zval * const src 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_store_zval( 		 		 		xc_processor_t *processor, zval *dst, const zval * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zval));
			do {
		

		
			
				/* Variable information */
		
	
	

		switch ((Z_TYPE_P(src) & IS_CONSTANT_TYPE_MASK)) {
			case IS_LONG:
			case IS_RESOURCE:
			case IS_BOOL:
					
	assert(sizeof(long) == sizeof(src->value.lval));
		
	
	
	

	

				break;
			case IS_DOUBLE:
					
	assert(sizeof(double) == sizeof(src->value.dval));
		
	
	
	

	

				break;
			case IS_NULL:
				
				break;

			case IS_CONSTANT:
#ifdef IS_UNICODE
				if (UG(unicode)) {
					goto proc_unicode;
				}
#endif
			case IS_STRING:
#ifdef FLAG_IS_BC
			case FLAG_IS_BC:
#endif
					
	assert(sizeof(int) == sizeof(src->value.str.len));
		
	
	
	

	

				  
	
	
	
	
	
	
	
	
	if (src->value.str.val == NULL) {
		
		
	}
	else {
		
		
		dst->value.str.val =  ZSTR_S(xc_store_string_n(processor, IS_STRING, ZSTR(src->value.str.val), src->value.str.len + 1 C_RELAYLINE));
		
		
	dst->value.str.val = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->value.str.val);

		
	}
	
	
	
	
	
	
	

				break;
#ifdef IS_UNICODE
			case IS_UNICODE:
proc_unicode:
					
	assert(sizeof(int32_t) == sizeof(src->value.uni.len));
		
	
	
	

	

				 
	
#ifdef IS_UNICODE
	
	
	
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->value.uni.val) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_U(dst->value.uni.val) =  ZSTR_U(xc_store_string_n(processor, IS_UNICODE, src->value.uni.val, src->value.uni.len + 1 C_RELAYLINE));
		
		
	ZSTR_U(dst->value.uni.val) = (UChar *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_U(dst->value.uni.val));

		
	}
	
	
	
	
	
	
	

	
#else
	
	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->value.uni.val) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->value.uni.val) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->value.uni.val, src->value.uni.len + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->value.uni.val) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->value.uni.val));

		
	}
	
	
	
	
	
	
	

#endif
	

				break;
#endif

			case IS_ARRAY:
#ifdef IS_CONSTANT_ARRAY
			case IS_CONSTANT_ARRAY:
#endif
				assert(src->value.ht);
				
	
	if (src->value.ht) {
		
		
	
	
	

	
	assert(sizeof(HashTable) == sizeof(( src->value.ht)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->value.ht = (HashTable *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(HashTable);

		
	
	
	
	
	


	

	xc_store_HashTable_zval_ptr(
		
		
		processor,  dst->value.ht,  src->value.ht
		
		
		TSRMLS_CC
	);

	

	
	
	dst->value.ht = (HashTable *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->value.ht);


	}
	else {
		
		
	
	
	assert(src->value.ht == NULL);

	}
	

				break;

#ifdef IS_CONSTANT_AST
			case IS_CONSTANT_AST:
				assert(src->value.ast);
				{
	
		size_t zend_ast_size = (src->value.ast->kind == ZEND_CONST)
		 ? sizeof(zend_ast) + sizeof(zval)
		 : sizeof(zend_ast) + sizeof(zend_ast *) * (src->value.ast->children - 1);
	

	
	
	
	if (src->value.ast) {
		
		
	
	
	

	
	assert(sizeof(zend_ast) == sizeof(( src->value.ast)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->value.ast = (zend_ast *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += zend_ast_size;

		
	
	
	
	
	


	

	xc_store_zend_ast(
		
		
		processor,  dst->value.ast,  src->value.ast
		
		
		TSRMLS_CC
	);

	

	
	
	dst->value.ast = (zend_ast *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->value.ast);


	}
	else {
		
		
	
	
	assert(src->value.ast == NULL);

	}
	

	
}

				break;
#endif

			case IS_OBJECT:
				
				#ifndef ZEND_ENGINE_2
				
	
	if (src->value.obj.ce) {
		
		
	
	
	

	
	assert(sizeof(zend_class_entry) == sizeof(( src->value.obj.ce)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->value.obj.ce = (zend_class_entry *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zend_class_entry);

		
	
	
	
	
	


	

	xc_store_zend_class_entry(
		
		
		processor,  dst->value.obj.ce,  src->value.obj.ce
		
		
		TSRMLS_CC
	);

	

	
	
	dst->value.obj.ce = (zend_class_entry *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->value.obj.ce);


	}
	else {
		
		
	
	
	assert(src->value.obj.ce == NULL);

	}
	

				
	
	if (src->value.obj.properties) {
		
		
	
	
	

	
	assert(sizeof(HashTable) == sizeof(( src->value.obj.properties)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->value.obj.properties = (HashTable *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(HashTable);

		
	
	
	
	
	


	

	xc_store_HashTable_zval_ptr(
		
		
		processor,  dst->value.obj.properties,  src->value.obj.properties
		
		
		TSRMLS_CC
	);

	

	
	
	dst->value.obj.properties = (HashTable *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->value.obj.properties);


	}
	else {
		
		
	
	
	assert(src->value.obj.properties == NULL);

	}
	

#endif
				break;

			default:
				assert(0);
		}
		
	
	

		
	
	
	

			
	assert(sizeof(xc_zval_type_t) == sizeof(src->type));
			
	
	
	
	
	
	



	

#ifdef ZEND_ENGINE_2_3
			
	assert(sizeof(zend_uchar) == sizeof(src->is_ref__gc));
		
	
	
	
	
	
	


	

#else
			
	assert(sizeof(zend_uchar) == sizeof(src->is_ref));
		
	
	
	
	
	
	


	

#endif

#ifdef ZEND_ENGINE_2_3
			
	assert(sizeof(zend_uint) == sizeof(src->refcount__gc));
		
	
	
	
	
	
	


	

#elif defined(ZEND_ENGINE_2)
			
	assert(sizeof(zend_uint) == sizeof(src->refcount));
		
	
	
	
	
	
	


	

#else
			
	assert(sizeof(zend_ushort) == sizeof(src->refcount));
		
	
	
	
	
	
	


	

#endif
	
		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_store_zval_ptr */
	
 	  	 	static void inline 	xc_store_zval_ptr( 		 		 		xc_processor_t *processor, zval_ptr *dst, const zval_ptr * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zval_ptr));
			do {
		

		
			
		do {
			
				if (processor->reference) {
					zval_ptr *ppzv;
					if (zend_hash_find(&processor->zvalptrs, (char *) &src[0], sizeof(src[0]), (void **) &ppzv) == SUCCESS) {
						
							dst[0] = *ppzv;
							/* *dst is updated */
													
						processor->have_references = 1;
						assert(xc_is_shm(dst[0]));
						
						break;
					}
				}
			
			
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst[0] = (zval *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zval);

		
	
	
	
	
	

			
				if (processor->reference) {
					
						zval_ptr pzv = dst[0];
						
	pzv = (zval *) processor->shm->handlers->to_readonly(processor->shm, (void *)pzv);

					
					if (zend_hash_add(&processor->zvalptrs, (char *) &src[0], sizeof(src[0]), (void *) &pzv, sizeof(pzv), NULL) == SUCCESS) {
						/* first add, go on */
											}
					else {
						assert(0);
					}
				}
			
			
							
			
			
	
	
	

	
	assert(sizeof(zval) == sizeof((  src[0])[0]));
	

	

	xc_store_zval(
		
		
		processor,   dst[0],   src[0]
		
		
		TSRMLS_CC
	);

	

	
	

			
	dst[0] = (zval *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst[0]);

		} while (0);
	
	

		
		
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_store_zval_ptr_nullable */
	
 	  	 	static void inline 	xc_store_zval_ptr_nullable( 		 		 		xc_processor_t *processor, zval_ptr_nullable *dst, const zval_ptr_nullable * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zval_ptr_nullable));
			do {
		

		
			if (src[0]) {
		
		
	
	
	

	
	assert(sizeof(zval_ptr) == sizeof((  src)[0]));
	

	xc_store_zval_ptr(
		
		
		processor,   dst,   src
		
		
		TSRMLS_CC
	);

	
	

		
	}
	else {
		
	
	
	assert(src[0] == NULL);

	}
	

		
		
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#ifdef ZEND_ENGINE_2

	
	

/* {{{ xc_store_zend_arg_info */
	
 	  	 	static void inline 	xc_store_zend_arg_info( 		 		 		xc_processor_t *processor, zend_arg_info *dst, const zend_arg_info * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_arg_info));
			do {
		

		
				
	assert(sizeof(zend_uint) == sizeof(src->name_len));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_U(dst->name) =  ZSTR_U(xc_store_string_n(processor, IS_UNICODE, src->name, src->name_len + 1 C_RELAYLINE));
		
		
	ZSTR_U(dst->name) = (UChar *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_U(dst->name));

		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->name) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->name, src->name_len + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->name) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->name));

		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->name) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->name, src->name_len + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->name) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->name));

		
	}
	
	
	
	
	
	
	

#endif
	

		
	assert(sizeof(zend_uint) == sizeof(src->class_name_len));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->class_name) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_U(dst->class_name) =  ZSTR_U(xc_store_string_n(processor, IS_UNICODE, src->class_name, src->class_name_len + 1 C_RELAYLINE));
		
		
	ZSTR_U(dst->class_name) = (UChar *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_U(dst->class_name));

		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->class_name) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->class_name) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->class_name, src->class_name_len + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->class_name) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->class_name));

		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->class_name) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->class_name) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->class_name, src->class_name_len + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->class_name) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->class_name));

		
	}
	
	
	
	
	
	
	

#endif
	

#ifdef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uchar) == sizeof(src->type_hint));
		
	
	
	
	
	
	


	

#elif defined(ZEND_ENGINE_2_1)
		
	assert(sizeof(zend_bool) == sizeof(src->array_type_hint));
		
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_6
		
	assert(sizeof(zend_uchar) == sizeof(src->pass_by_reference));
		
	
	
	
	
	
	


	

#endif
		
	assert(sizeof(zend_bool) == sizeof(src->allow_null));
		
	
	
	
	
	
	


	


#ifdef ZEND_ENGINE_2_6
		
	assert(sizeof(zend_bool) == sizeof(src->is_variadic));
		
	
	
	
	
	
	


	

#else
		
	assert(sizeof(zend_bool) == sizeof(src->pass_by_reference));
		
	
	
	
	
	
	


	

#endif

#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_bool) == sizeof(src->return_reference));
		
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->required_num_args));
		
	
	
	
	
	
	


	

#endif

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif
#ifdef HAVE_XCACHE_CONSTANT

	
	

/* {{{ xc_store_zend_constant */
	
 	  	 	static void inline 	xc_store_zend_constant( 		 		 		xc_processor_t *processor, zend_constant *dst, const zend_constant * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_constant));
			do {
		

		
			
	
	assert(sizeof(zval) == sizeof(src->value));
	
	
	
	
	

	
	assert(sizeof(zval) == sizeof((& src->value)[0]));
	

	

	xc_store_zval(
		
		
		processor, & dst->value, & src->value
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


		
	assert(sizeof(int) == sizeof(src->flags));
		
	
	
	
	
	
	


	

		
	assert(sizeof(uint) == sizeof(src->name_len));
		
	
	
	
	
	
	


	

	
	
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_U(dst->name) =  ZSTR_U(xc_store_string_n(processor, IS_UNICODE, src->name, src->name_len C_RELAYLINE));
		
		
	ZSTR_U(dst->name) = (UChar *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_U(dst->name));

		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->name) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->name, src->name_len C_RELAYLINE));
		
		
	ZSTR_S(dst->name) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->name));

		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->name) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->name, src->name_len C_RELAYLINE));
		
		
	ZSTR_S(dst->name) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->name));

		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	assert(sizeof(int) == sizeof(src->module_number));
		
	
	
	
	
	
	


	


		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_store_zend_function */
	
		/* export:  	  	 	void 	xc_store_zend_function( 		 		 		xc_processor_t *processor, zend_function *dst, const zend_function * const src 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_store_zend_function( 		 		 		xc_processor_t *processor, zend_function *dst, const zend_function * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_function));
			do {
		

		
			
	
	

	switch (src->type) {
	case ZEND_INTERNAL_FUNCTION:
	case ZEND_OVERLOADED_FUNCTION:
		
		break;

	case ZEND_USER_FUNCTION:
	case ZEND_EVAL_CODE:
		
		
	
	assert(sizeof(zend_op_array) == sizeof(src->op_array));
	
	
	
	
	

	
	assert(sizeof(zend_op_array) == sizeof((& src->op_array)[0]));
	

	

	xc_store_zend_op_array(
		
		
		processor, & dst->op_array, & src->op_array
		
		
		TSRMLS_CC
	);

	

	
	

	

		break;

	default:
		assert(0);
	}
	
	
	

	

		
		
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#ifdef ZEND_ENGINE_2

	
	

/* {{{ xc_store_zend_property_info */
	
 	  	 	static void inline 	xc_store_zend_property_info( 		 		 		xc_processor_t *processor, zend_property_info *dst, const zend_property_info * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_property_info));
			do {
		

		
				
	assert(sizeof(zend_uint) == sizeof(src->flags));
		
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->name_length));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_U(dst->name) =  ZSTR_U(xc_store_string_n(processor, IS_UNICODE, src->name, src->name_length + 1 C_RELAYLINE));
		
		
	ZSTR_U(dst->name) = (UChar *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_U(dst->name));

		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->name) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->name, src->name_length + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->name) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->name));

		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->name) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->name, src->name_length + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->name) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->name));

		
	}
	
	
	
	
	
	
	

#endif
	

		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
	
	
	
	
	


	

#ifdef ZEND_ENGINE_2_4
		
	assert(sizeof(int) == sizeof(src->offset));
		
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_1
		
	assert(sizeof(int) == sizeof(src->doc_comment_len));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_U(dst->doc_comment) =  ZSTR_U(xc_store_string_n(processor, IS_UNICODE, src->doc_comment, src->doc_comment_len + 1 C_RELAYLINE));
		
		
	ZSTR_U(dst->doc_comment) = (UChar *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_U(dst->doc_comment));

		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->doc_comment) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->doc_comment, src->doc_comment_len + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->doc_comment) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->doc_comment));

		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->doc_comment) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->doc_comment, src->doc_comment_len + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->doc_comment) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->doc_comment));

		
	}
	
	
	
	
	
	
	

#endif
	

#endif
	#if defined(ZEND_ENGINE_2_2)
	
	if (src->ce) {
		dst->ce = (zend_class_entry *) xc_get_class_num(processor,src->ce);
		
#ifdef IS_UNICODE
		
#else
		
#endif
	}
	else {
		
	
	
	assert(src->ce == NULL);

	}

	
	
	

#endif

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif
#ifdef ZEND_ENGINE_2_4

	
	

/* {{{ xc_store_zend_trait_method_reference */
	
 	  	 	static void inline 	xc_store_zend_trait_method_reference( 		 		 		xc_processor_t *processor, zend_trait_method_reference *dst, const zend_trait_method_reference * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_trait_method_reference));
			do {
		

		
				
	assert(sizeof(unsigned int) == sizeof(src->mname_len));
		
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->method_name == NULL) {
		
		
	}
	else {
		
		
		dst->method_name =  ZSTR_S(xc_store_string_n(processor, IS_STRING, ZSTR(src->method_name), src->mname_len + 1 C_RELAYLINE));
		
		
	dst->method_name = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->method_name);

		
	}
	
	
	
	
	
	
	

	
	
	
	
	assert(src->ce == NULL);

	
	
	


		
	assert(sizeof(unsigned int) == sizeof(src->cname_len));
		
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->class_name == NULL) {
		
		
	}
	else {
		
		
		dst->class_name =  ZSTR_S(xc_store_string_n(processor, IS_STRING, ZSTR(src->class_name), src->cname_len + 1 C_RELAYLINE));
		
		
	dst->class_name = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->class_name);

		
	}
	
	
	
	
	
	
	


		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_store_zend_trait_alias */
	
 	  	 	static void inline 	xc_store_zend_trait_alias( 		 		 		xc_processor_t *processor, zend_trait_alias *dst, const zend_trait_alias * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_trait_alias));
			do {
		

		
			
	
	if (src->trait_method) {
		
		
	
	
	

	
	assert(sizeof(zend_trait_method_reference) == sizeof(( src->trait_method)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->trait_method = (zend_trait_method_reference *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zend_trait_method_reference);

		
	
	
	
	
	


	

	xc_store_zend_trait_method_reference(
		
		
		processor,  dst->trait_method,  src->trait_method
		
		
		TSRMLS_CC
	);

	

	
	
	dst->trait_method = (zend_trait_method_reference *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->trait_method);


	}
	else {
		
		
	
	
	assert(src->trait_method == NULL);

	}
	
	
	
	


		
	assert(sizeof(unsigned int) == sizeof(src->alias_len));
		
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->alias == NULL) {
		
		
	}
	else {
		
		
		dst->alias =  ZSTR_S(xc_store_string_n(processor, IS_STRING, ZSTR(src->alias), src->alias_len + 1 C_RELAYLINE));
		
		
	dst->alias = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->alias);

		
	}
	
	
	
	
	
	
	

		
	assert(sizeof(zend_uint) == sizeof(src->modifiers));
		
	
	
	
	
	
	


	

#ifndef ZEND_ENGINE_2_5
	
	
	
	
	assert(src->function == NULL);

	
	
	


#endif

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_store_zend_trait_precedence */
	
 	  	 	static void inline 	xc_store_zend_trait_precedence( 		 		 		xc_processor_t *processor, zend_trait_precedence *dst, const zend_trait_precedence * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_trait_precedence));
			do {
		

		
			
	
	if (src->trait_method) {
		
		
	
	
	

	
	assert(sizeof(zend_trait_method_reference) == sizeof(( src->trait_method)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->trait_method = (zend_trait_method_reference *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zend_trait_method_reference);

		
	
	
	
	
	


	

	xc_store_zend_trait_method_reference(
		
		
		processor,  dst->trait_method,  src->trait_method
		
		
		TSRMLS_CC
	);

	

	
	
	dst->trait_method = (zend_trait_method_reference *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->trait_method);


	}
	else {
		
		
	
	
	assert(src->trait_method == NULL);

	}
	
	
	
	


		if (src->exclude_from_classes) {
		int LOOPCOUNTER;
		
						
				size_t count = 0;
				while (src->exclude_from_classes[count]) {
					++count;
				}
				++count;
				
			
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->exclude_from_classes = (zend_class_entry* *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(xc_ztstring) * count;

		
	
	
	
	
	

			

			for (LOOPCOUNTER = 0;
					src->exclude_from_classes[LOOPCOUNTER];
					++LOOPCOUNTER) {
				
	
	

					
					
						
	assert(sizeof(xc_ztstring) == sizeof((xc_ztstring) src->exclude_from_classes [LOOPCOUNTER]));
		
	 
	
	
	
	
	
	
	
	
	if ((xc_ztstring) src->exclude_from_classes [LOOPCOUNTER] == NULL) {
		
		
	}
	else {
		
		
		dst->exclude_from_classes [LOOPCOUNTER] = (zend_class_entry *) ZSTR_S(xc_store_string_n(processor, IS_STRING, ZSTR((xc_ztstring) src->exclude_from_classes [LOOPCOUNTER]), strlen((xc_ztstring) src->exclude_from_classes [LOOPCOUNTER]) + 1 C_RELAYLINE));
		
		
	dst->exclude_from_classes [LOOPCOUNTER] = (zend_class_entry *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->exclude_from_classes [LOOPCOUNTER]);

		
	}
	
	
	
	
	
	
	

	

	

					
					
				
	
	

			}
						dst->exclude_from_classes[LOOPCOUNTER] = NULL;
				
	
	
	

	}
	else {
		
	
	
	
	assert(src->exclude_from_classes == NULL);

	
	
	


	}

#ifndef ZEND_ENGINE_2_5
	
	
	
	
	assert(src->function == NULL);

	
	
	


#endif

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_store_zend_trait_alias_ptr */
	
 	  	 	static void inline 	xc_store_zend_trait_alias_ptr( 		 		 		xc_processor_t *processor, zend_trait_alias_ptr *dst, const zend_trait_alias_ptr * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_trait_alias_ptr));
			do {
		

		
			
		
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst[0] = (zend_trait_alias *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zend_trait_alias);

		
	
	
	
	
	

		
	
	
	

	
	assert(sizeof(zend_trait_alias) == sizeof((  src[0])[0]));
	

	

	xc_store_zend_trait_alias(
		
		
		processor,   dst[0],   src[0]
		
		
		TSRMLS_CC
	);

	

	
	

		
	dst[0] = (zend_trait_alias *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst[0]);

	
	

		
		
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_store_zend_trait_precedence_ptr */
	
 	  	 	static void inline 	xc_store_zend_trait_precedence_ptr( 		 		 		xc_processor_t *processor, zend_trait_precedence_ptr *dst, const zend_trait_precedence_ptr * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_trait_precedence_ptr));
			do {
		

		
			
		
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst[0] = (zend_trait_precedence *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zend_trait_precedence);

		
	
	
	
	
	

		
	
	
	

	
	assert(sizeof(zend_trait_precedence) == sizeof((  src[0])[0]));
	

	

	xc_store_zend_trait_precedence(
		
		
		processor,   dst[0],   src[0]
		
		
		TSRMLS_CC
	);

	

	
	

		
	dst[0] = (zend_trait_precedence *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst[0]);

	
	

		
		
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_store_zend_class_entry */
	
		/* export:  	  	 	void 	xc_store_zend_class_entry( 		 		 		xc_processor_t *processor, zend_class_entry *dst, const zend_class_entry * const src 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_store_zend_class_entry( 		 		 		xc_processor_t *processor, zend_class_entry *dst, const zend_class_entry * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_class_entry));
			do {
		

		
			
		processor->active_class_entry_src = src;
		processor->active_class_entry_dst = dst;
	
		
	assert(sizeof(char) == sizeof(src->type));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->name_length));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_U(dst->name) =  ZSTR_U(xc_store_string_n(processor, IS_UNICODE, src->name, src->name_length + 1 C_RELAYLINE));
		
		
	ZSTR_U(dst->name) = (UChar *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_U(dst->name));

		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->name) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->name, src->name_length + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->name) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->name));

		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->name) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->name, src->name_length + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->name) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->name));

		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	if (src->parent) {
		dst->parent = (zend_class_entry *) xc_get_class_num(processor,src->parent);
		
#ifdef IS_UNICODE
		
#else
		
#endif
	}
	else {
		
	
	
	assert(src->parent == NULL);

	}

	
	
	

	
#ifdef ZEND_ENGINE_2
		
	assert(sizeof(int) == sizeof(src->refcount));
		
	
	
	
	
	
	


	

#else
	
	
	if (src->refcount) {
		
		
	
	
	

	
	assert(sizeof(int) == sizeof(( src->refcount)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->refcount = (int *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(int);

		
	
	
	
	
	


	

	xc_store_int(
		
		
		processor,  dst->refcount,  src->refcount
		
		
		TSRMLS_CC
	);

	

	
	
	dst->refcount = (int *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->refcount);


	}
	else {
		
		
	
	
	assert(src->refcount == NULL);

	}
	
	
	
	


#endif
#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_bool) == sizeof(src->constants_updated));
		
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2
		
	assert(sizeof(zend_uint) == sizeof(src->ce_flags));
		
	
	
	
	
	
	


	

#endif

#ifdef ZEND_ENGINE_2
	
	
	assert(sizeof(HashTable) == sizeof(src->properties_info));
	
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->properties_info)[0]));
	

	

	xc_store_HashTable_zend_property_info(
		
		
		processor, & dst->properties_info, & src->properties_info
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


#endif

#ifdef ZEND_ENGINE_2_4
	
	if (src->default_properties_table) {
		int i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->default_properties_table = (zval_ptr_nullable *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zval_ptr_nullable) * src->default_properties_count;

		
	
	
	
	
	

			

			for (i = 0;
					i < src->default_properties_count;
					++i) {
				
	
	

					
	
	assert(sizeof(zval_ptr_nullable) == sizeof(src->default_properties_table[i]));
	
	
	
	
	

	
	assert(sizeof(zval_ptr_nullable) == sizeof((& src->default_properties_table[i])[0]));
	

	

	xc_store_zval_ptr_nullable(
		
		
		processor, & dst->default_properties_table[i], & src->default_properties_table[i]
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
	dst->default_properties_table = (zval_ptr_nullable *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->default_properties_table);

		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->default_properties_table == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->default_properties_count));
		
	
	
	
	
	
	


	

	
	if (src->default_static_members_table) {
		int i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->default_static_members_table = (zval_ptr_nullable *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zval_ptr_nullable) * src->default_static_members_count;

		
	
	
	
	
	

			

			for (i = 0;
					i < src->default_static_members_count;
					++i) {
				
	
	

					
	
	assert(sizeof(zval_ptr_nullable) == sizeof(src->default_static_members_table[i]));
	
	
	
	
	

	
	assert(sizeof(zval_ptr_nullable) == sizeof((& src->default_static_members_table[i])[0]));
	

	

	xc_store_zval_ptr_nullable(
		
		
		processor, & dst->default_static_members_table[i], & src->default_static_members_table[i]
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
	dst->default_static_members_table = (zval_ptr_nullable *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->default_static_members_table);

		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->default_static_members_table == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->default_static_members_count));
		
	
	
	
	
	
	


	

	dst->static_members_table = dst->default_static_members_table;
	
	
	
	

#else
	dst->builtin_functions = src->builtin_functions;
	
	
	
	

	
	
	assert(sizeof(HashTable) == sizeof(src->default_properties));
	
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->default_properties)[0]));
	

	

	xc_store_HashTable_zval_ptr(
		
		
		processor, & dst->default_properties, & src->default_properties
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


#	ifdef ZEND_ENGINE_2_1
	
	
	assert(sizeof(HashTable) == sizeof(src->default_static_members));
	
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->default_static_members)[0]));
	

	

	xc_store_HashTable_zval_ptr(
		
		
		processor, & dst->default_static_members, & src->default_static_members
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	dst->static_members = &dst->default_static_members;
	
	
	
	

#	elif defined(ZEND_ENGINE_2)
	
	
	if (src->static_members) {
		
		
	
	
	

	
	assert(sizeof(HashTable) == sizeof(( src->static_members)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->static_members = (HashTable *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(HashTable);

		
	
	
	
	
	


	

	xc_store_HashTable_zval_ptr(
		
		
		processor,  dst->static_members,  src->static_members
		
		
		TSRMLS_CC
	);

	

	
	
	dst->static_members = (HashTable *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->static_members);


	}
	else {
		
		
	
	
	assert(src->static_members == NULL);

	}
	
	
	
	


#	endif
#endif /* ZEND_ENGINE_2_4 */

#ifdef ZEND_ENGINE_2
	
	
	assert(sizeof(HashTable) == sizeof(src->constants_table));
	
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->constants_table)[0]));
	

	

	xc_store_HashTable_zval_ptr(
		
		
		processor, & dst->constants_table, & src->constants_table
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	



#ifdef ZEND_ENGINE_2_2
		
	
	
	
	assert(src->interfaces == NULL);

	
	
	


	
	
	
	
	assert(src->num_interfaces == 0);

	
	
	



#	ifdef ZEND_ENGINE_2_4
		
	
	
	
	assert(src->traits == NULL);

	
	
	


	
	
	
	
	assert(src->num_traits == 0);

	
	
	


	
	if (src->trait_aliases) {
		size_t i; 
		
	

		
						
				size_t count = 0;
				while (src->trait_aliases[count]) {
					++count;
				}
				++count;
				
			
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->trait_aliases = (zend_trait_alias_ptr *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zend_trait_alias_ptr) * count;

		
	
	
	
	
	

			

			for (i = 0;
					src->trait_aliases[i];
					++i) {
				
	
	

					
	
	assert(sizeof(zend_trait_alias_ptr) == sizeof(src->trait_aliases[i]));
	
	
	
	
	

	
	assert(sizeof(zend_trait_alias_ptr) == sizeof((& src->trait_aliases[i])[0]));
	

	

	xc_store_zend_trait_alias_ptr(
		
		
		processor, & dst->trait_aliases[i], & src->trait_aliases[i]
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						dst->trait_aliases[i] = NULL;
				
	dst->trait_aliases = (zend_trait_alias_ptr *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->trait_aliases);

		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->trait_aliases == NULL);

	
	
	


	}

	
	if (src->trait_precedences) {
		size_t i; 
		
	

		
						
				size_t count = 0;
				while (src->trait_precedences[count]) {
					++count;
				}
				++count;
				
			
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->trait_precedences = (zend_trait_precedence_ptr *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zend_trait_precedence_ptr) * count;

		
	
	
	
	
	

			

			for (i = 0;
					src->trait_precedences[i];
					++i) {
				
	
	

					
	
	assert(sizeof(zend_trait_precedence_ptr) == sizeof(src->trait_precedences[i]));
	
	
	
	
	

	
	assert(sizeof(zend_trait_precedence_ptr) == sizeof((& src->trait_precedences[i])[0]));
	

	

	xc_store_zend_trait_precedence_ptr(
		
		
		processor, & dst->trait_precedences[i], & src->trait_precedences[i]
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						dst->trait_precedences[i] = NULL;
				
	dst->trait_precedences = (zend_trait_precedence_ptr *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->trait_precedences);

		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->trait_precedences == NULL);

	
	
	


	}

#	endif
#else
	
		
	
	
	

	
		
	assert(sizeof(zend_uint) == sizeof(src->num_interfaces));
		
	
	
	
	
	
	


	

#endif

#	ifdef ZEND_ENGINE_2_4
	
	
	

	 
	
	
	
	
	
	
	
	
	if (src->info.user.filename == NULL) {
		
		
	}
	else {
		
		
		dst->info.user.filename =  ZSTR_S(xc_store_string_n(processor, IS_STRING, ZSTR(src->info.user.filename), strlen(src->info.user.filename) + 1 C_RELAYLINE));
		
		
	dst->info.user.filename = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->info.user.filename);

		
	}
	
	
	
	
	
	
	

		
	assert(sizeof(zend_uint) == sizeof(src->info.user.line_start));
		
	
	
	

	

		
	assert(sizeof(zend_uint) == sizeof(src->info.user.line_end));
		
	
	
	

	

		
	assert(sizeof(zend_uint) == sizeof(src->info.user.doc_comment_len));
		
	
	
	

	

	 
	
#ifdef IS_UNICODE
	
	
	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->info.user.doc_comment) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_U(dst->info.user.doc_comment) =  ZSTR_U(xc_store_string_n(processor, IS_UNICODE, src->info.user.doc_comment, src->info.user.doc_comment_len + 1 C_RELAYLINE));
		
		
	ZSTR_U(dst->info.user.doc_comment) = (UChar *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_U(dst->info.user.doc_comment));

		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->info.user.doc_comment) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->info.user.doc_comment) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->info.user.doc_comment, src->info.user.doc_comment_len + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->info.user.doc_comment) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->info.user.doc_comment));

		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->info.user.doc_comment) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->info.user.doc_comment) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->info.user.doc_comment, src->info.user.doc_comment_len + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->info.user.doc_comment) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->info.user.doc_comment));

		
	}
	
	
	
	
	
	
	

#endif
	

	
	
	

	
	
	
	

#	else
	 
	
	
	

	
	
	
	
	
	
	
	
	if (src->filename == NULL) {
		
		
	}
	else {
		
		
		dst->filename =  ZSTR_S(xc_store_string_n(processor, IS_STRING, ZSTR(src->filename), strlen(src->filename) + 1 C_RELAYLINE));
		
		
	dst->filename = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->filename);

		
	}
	
	
	
	
	
	
	

		
	assert(sizeof(zend_uint) == sizeof(src->line_start));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->line_end));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->doc_comment_len));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_U(dst->doc_comment) =  ZSTR_U(xc_store_string_n(processor, IS_UNICODE, src->doc_comment, src->doc_comment_len + 1 C_RELAYLINE));
		
		
	ZSTR_U(dst->doc_comment) = (UChar *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_U(dst->doc_comment));

		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->doc_comment) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->doc_comment, src->doc_comment_len + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->doc_comment) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->doc_comment));

		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->doc_comment) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->doc_comment, src->doc_comment_len + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->doc_comment) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->doc_comment));

		
	}
	
	
	
	
	
	
	

#endif
	

#	endif

	/* # NOT DONE */
#	ifdef ZEND_ENGINE_2_1
	
	
	
	

	
	
	
	

#	endif
	
	
	
	

	
	
	
	

	
	
	
	

	
	
	
	

#	ifdef ZEND_ENGINE_2_3
	
	
	
	

#	endif
#	ifdef ZEND_ENGINE_2_1
	
	
	
	

	
	
	
	

#	endif
	/* deal with it inside xc_fix_method */
	dst->constructor = NULL;
	
	
	

	
	
	
	

	
	
	
	

	
	
	
	

	
	
	
	

/* should be >5.1 */
#	ifdef ZEND_ENGINE_2_1
	
	
	
	

	
	
	
	

#	endif
	
	
	
	

#	ifdef ZEND_CALLSTATIC_FUNC_NAME
	
	
	
	

#	endif
# if defined(ZEND_ENGINE_2_2) || PHP_MAJOR_VERSION >= 6
	
	
	
	

# endif
# if defined(ZEND_ENGINE_2_6)
	
	
	
	

# endif
#	ifndef ZEND_ENGINE_2_4
	/* # NOT DONE */
	
	
	
	

#	endif
#else /* ZEND_ENGINE_2 */
	
	
	
	

	
	
	
	

	
	
	
	

#endif
		
	
	assert(sizeof(HashTable) == sizeof(src->function_table));
	
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->function_table)[0]));
	

	

	xc_store_HashTable_zend_function(
		
		
		processor, & dst->function_table, & src->function_table
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
		processor->active_class_entry_src = NULL;
		processor->active_class_entry_dst = NULL;
	

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#ifdef ZEND_ENGINE_2_4


#else

	
	

/* {{{ xc_store_znode */
	
 	  	 	static void inline 	xc_store_znode( 		 		 		xc_processor_t *processor, znode *dst, const znode * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(znode));
			do {
		

		
				
	assert(sizeof(xc_op_type) == sizeof(src->op_type));
			
	
	
	
	
	
	



	


#ifdef IS_CV
#	define XCACHE_IS_CV IS_CV
#else
/* compatible with zend optimizer */
#	define XCACHE_IS_CV 16
#endif
	assert(src->op_type == IS_CONST ||
		src->op_type == IS_VAR ||
		src->op_type == XCACHE_IS_CV ||
		src->op_type == IS_TMP_VAR ||
		src->op_type == IS_UNUSED);
		
	
	

	switch (src->op_type) {
		case IS_CONST:
			
	
	assert(sizeof(zval) == sizeof(src->u.constant));
	
	
	
	
	

	
	assert(sizeof(zval) == sizeof((& src->u.constant)[0]));
	

	

	xc_store_zval(
		
		
		processor, & dst->u.constant, & src->u.constant
		
		
		TSRMLS_CC
	);

	

	
	

	

			break;
		
			
		
	}
	
	
	

	
	
	
	

#if 0
	
	
	
	

#endif
#undef XCACHE_IS_CV

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_store_zend_op */
	
		/* export:  	  	 	void 	xc_store_zend_op( 		 		 		xc_processor_t *processor, zend_op *dst, const zend_op * const src 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_store_zend_op( 		 		 		xc_processor_t *processor, zend_op *dst, const zend_op * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_op));
			do {
		

		
				
	assert(sizeof(xc_opcode) == sizeof(src->opcode));
			
	
	
	
	
	
	



	

#ifdef ZEND_ENGINE_2_4
	
	switch (src->opcode) {
	case ZEND_BIND_TRAITS:
		((zend_op *) src)->op2_type = IS_UNUSED;
		break;
	}
	
	#ifndef NDEBUG
	switch ((src->result_type & ~EXT_TYPE_UNUSED)) {
	case IS_CONST:
	case IS_VAR:
	case IS_CV:
	case IS_TMP_VAR:
	case IS_UNUSED:
		break;

	default:
		assert(0);
	}
#endif

		
	
	

	switch ((src->result_type & ~EXT_TYPE_UNUSED)) {
		case IS_CONST:
			
					
	assert(sizeof(zend_uint) == sizeof(src->result.constant));
		
	
	
	

	

			
			break;
		
			
		
	}
	
	
	

	
	
	
	


	#ifndef NDEBUG
	switch ((src->op1_type )) {
	case IS_CONST:
	case IS_VAR:
	case IS_CV:
	case IS_TMP_VAR:
	case IS_UNUSED:
		break;

	default:
		assert(0);
	}
#endif

		
	
	

	switch ((src->op1_type )) {
		case IS_CONST:
			
				
					
						dst->op1 = src->op1;
					
				
			
			break;
		
			
		
	}
	
	
	

	
	
	
	


	#ifndef NDEBUG
	switch ((src->op2_type )) {
	case IS_CONST:
	case IS_VAR:
	case IS_CV:
	case IS_TMP_VAR:
	case IS_UNUSED:
		break;

	default:
		assert(0);
	}
#endif

		
	
	

	switch ((src->op2_type )) {
		case IS_CONST:
			
				
					
						dst->op2 = src->op2;
					
				
			
			break;
		
			
		
	}
	
	
	

	
	
	
	


#else
	
	
	assert(sizeof(znode) == sizeof(src->result));
	
	
	
	
	

	
	assert(sizeof(znode) == sizeof((& src->result)[0]));
	

	

	xc_store_znode(
		
		
		processor, & dst->result, & src->result
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
	assert(sizeof(znode) == sizeof(src->op1));
	
	
	
	
	

	
	assert(sizeof(znode) == sizeof((& src->op1)[0]));
	

	

	xc_store_znode(
		
		
		processor, & dst->op1, & src->op1
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
	assert(sizeof(znode) == sizeof(src->op2));
	
	
	
	
	

	
	assert(sizeof(znode) == sizeof((& src->op2)[0]));
	

	

	xc_store_znode(
		
		
		processor, & dst->op2, & src->op2
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


#endif
		
	assert(sizeof(ulong) == sizeof(src->extended_value));
		
	
	
	
	
	
	


	

		
	assert(sizeof(uint) == sizeof(src->lineno));
		
	
	
	
	
	
	


	

#ifdef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uchar) == sizeof(src->op1_type));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uchar) == sizeof(src->op2_type));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uchar) == sizeof(src->result_type));
		
	
	
	
	
	
	


	

#endif
	
		assert(processor->active_op_array_src);
		assert(processor->active_op_array_dst);
#ifdef ZEND_ENGINE_2_4
		
		
			if (src->op1_type == IS_CONST) {
				dst->op1.constant = src->op1.literal - processor->active_op_array_src->literals;
				dst->op1.literal = &processor->active_op_array_dst->literals[dst->op1.constant];
			}
		
		
			if (src->op2_type == IS_CONST) {
				dst->op2.constant = src->op2.literal - processor->active_op_array_src->literals;
				dst->op2.literal = &processor->active_op_array_dst->literals[dst->op2.constant];
			}
		
#endif
		
#ifdef ZEND_ENGINE_2
		switch (src->opcode) {
#	ifdef ZEND_GOTO
			case ZEND_GOTO:
#	endif
			case ZEND_JMP:
#	ifdef ZEND_FAST_CALL
			case ZEND_FAST_CALL:
#	endif
				assert(Z_OP(src->op1).jmp_addr >= processor->active_op_array_src->opcodes);
				assert(Z_OP(src->op1).jmp_addr - processor->active_op_array_src->opcodes < processor->active_op_array_src->last);
				Z_OP(dst->op1).jmp_addr = processor->active_op_array_dst->opcodes + (Z_OP(src->op1).jmp_addr - processor->active_op_array_src->opcodes);
				assert(Z_OP(dst->op1).jmp_addr >= processor->active_op_array_dst->opcodes);
				assert(Z_OP(dst->op1).jmp_addr - processor->active_op_array_dst->opcodes < processor->active_op_array_dst->last);
				
	Z_OP(dst->op1).jmp_addr = (zend_op *) processor->shm->handlers->to_readonly(processor->shm, (void *)Z_OP(dst->op1).jmp_addr);

				break;

			case ZEND_JMPZ:
			case ZEND_JMPNZ:
			case ZEND_JMPZ_EX:
			case ZEND_JMPNZ_EX:
#	ifdef ZEND_JMP_SET
			case ZEND_JMP_SET:
#	endif
#	ifdef ZEND_JMP_SET_VAR
			case ZEND_JMP_SET_VAR:
#	endif
				assert(Z_OP(src->op2).jmp_addr >= processor->active_op_array_src->opcodes);
				assert(Z_OP(src->op2).jmp_addr - processor->active_op_array_src->opcodes < processor->active_op_array_src->last);
				Z_OP(dst->op2).jmp_addr = processor->active_op_array_dst->opcodes + (Z_OP(src->op2).jmp_addr - processor->active_op_array_src->opcodes);
				assert(Z_OP(dst->op2).jmp_addr >= processor->active_op_array_dst->opcodes);
				assert(Z_OP(dst->op2).jmp_addr - processor->active_op_array_dst->opcodes < processor->active_op_array_dst->last);
				
	Z_OP(dst->op2).jmp_addr = (zend_op *) processor->shm->handlers->to_readonly(processor->shm, (void *)Z_OP(dst->op2).jmp_addr);

				break;

			default:
				break;
		}
#	endif
	
#ifdef ZEND_ENGINE_2
		
	assert(sizeof(opcode_handler_t) == sizeof(src->handler));
	/* is copying enough? */
	
	
	

	

#endif

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#ifdef ZEND_ENGINE_2_4

	
	

/* {{{ xc_store_zend_literal */
	
 	  	 	static void inline 	xc_store_zend_literal( 		 		 		xc_processor_t *processor, zend_literal *dst, const zend_literal * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_literal));
			do {
		

		
			
	
	assert(sizeof(zval) == sizeof(src->constant));
	
	
	
	
	

	
	assert(sizeof(zval) == sizeof((& src->constant)[0]));
	

	

	xc_store_zval(
		
		
		processor, & dst->constant, & src->constant
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


		
	assert(sizeof(zend_ulong) == sizeof(src->hash_value));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->cache_slot));
		
	
	
	
	
	
	


	


		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_store_zend_op_array */
	
		/* export:  	  	 	void 	xc_store_zend_op_array( 		 		 		xc_processor_t *processor, zend_op_array *dst, const zend_op_array * const src 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_store_zend_op_array( 		 		 		xc_processor_t *processor, zend_op_array *dst, const zend_op_array * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_op_array));
			do {
		

		
			
		processor->active_op_array_dst = dst;
		processor->active_op_array_src = src;
	
	
	{
	
	do {
	
	/* Common elements */
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->function_name) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_U(dst->function_name) =  ZSTR_U(xc_store_string_n(processor, IS_UNICODE, src->function_name, xc_zstrlen_uchar (src->function_name) + 1 C_RELAYLINE));
		
		
	ZSTR_U(dst->function_name) = (UChar *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_U(dst->function_name));

		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->function_name) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->function_name) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->function_name, xc_zstrlen_char (src->function_name) + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->function_name) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->function_name));

		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->function_name) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->function_name) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->function_name, xc_zstrlen_char (src->function_name) + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->function_name) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->function_name));

		
	}
	
	
	
	
	
	
	

#endif
	

#ifdef ZEND_ENGINE_2
		
	assert(sizeof(zend_uint) == sizeof(src->fn_flags));
		
	
	
	
	
	
	


	

	
	if (src->arg_info) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->arg_info = (zend_arg_info *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zend_arg_info) * src->num_args;

		
	
	
	
	
	

			

			for (i = 0;
					i < src->num_args;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_arg_info) == sizeof(src->arg_info[i]));
	
	
	
	
	

	
	assert(sizeof(zend_arg_info) == sizeof((& src->arg_info[i])[0]));
	

	

	xc_store_zend_arg_info(
		
		
		processor, & dst->arg_info[i], & src->arg_info[i]
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
	dst->arg_info = (zend_arg_info *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->arg_info);

		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->arg_info == NULL);

	
	
	


	}

		
	assert(sizeof(zend_uint) == sizeof(src->num_args));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->required_num_args));
		
	
	
	
	
	
	


	

#	ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_bool) == sizeof(src->pass_rest_by_reference));
		
	
	
	
	
	
	


	

#	endif
#else
	if (src->arg_types) {
		
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->arg_types = (zend_uchar *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zend_uchar) * src->arg_types[0] + 1;

		
	
	
	
	
	

		memcpy(dst->arg_types, src->arg_types, sizeof(src->arg_types[0]) * (src->arg_types[0]+1));
		
		
	
	
	

	}
	else {
		
	
	
	
	assert(src->arg_types == NULL);

	
	
	


	}
#endif
#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(unsigned char) == sizeof(src->return_reference));
		
	
	
	
	
	
	


	

#endif
	/* END of common elements */
#ifdef IS_UNICODE
	#endif

	
	
	if (src->refcount) {
		
		
	
	
	

	
	assert(sizeof(zend_uint) == sizeof(( src->refcount)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->refcount = (zend_uint *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zend_uint);

		
	
	
	
	
	


	

	xc_store_zend_uint(
		
		
		processor,  dst->refcount,  src->refcount
		
		
		TSRMLS_CC
	);

	

	
	
	dst->refcount = (zend_uint *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->refcount);


	}
	else {
		
		
	
	
	assert(src->refcount == NULL);

	}
	
	
	
	


	
		
	dst->refcount = (zend_uint *) processor->shm->handlers->to_readwrite(processor->shm, (void *)dst->refcount);

		dst->refcount[0] = 1;
		
	dst->refcount = (zend_uint *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->refcount);

	

#ifdef ZEND_ENGINE_2_4
		
	if (src->literals) {
		int i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->literals = (zend_literal *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zend_literal) * src->last_literal;

		
	
	
	
	
	

			

			for (i = 0;
					i < src->last_literal;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_literal) == sizeof(src->literals[i]));
	
	
	
	
	

	
	assert(sizeof(zend_literal) == sizeof((& src->literals[i])[0]));
	

	

	xc_store_zend_literal(
		
		
		processor, & dst->literals[i], & src->literals[i]
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
	dst->literals = (zend_literal *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->literals);

		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->literals == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->last_literal));
		
	
	
	
	
	
	


	

#endif

		
	if (src->opcodes) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->opcodes = (zend_op *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zend_op) * src->last;

		
	
	
	
	
	

			

			for (i = 0;
					i < src->last;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_op) == sizeof(src->opcodes[i]));
	
	
	
	
	

	
	assert(sizeof(zend_op) == sizeof((& src->opcodes[i])[0]));
	

	

	xc_store_zend_op(
		
		
		processor, & dst->opcodes[i], & src->opcodes[i]
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
	dst->opcodes = (zend_op *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->opcodes);

		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->opcodes == NULL);

	
	
	


	}

		
	assert(sizeof(zend_uint) == sizeof(src->last));
		
	
	
	
	
	
	


	

#ifndef ZEND_ENGINE_2_4
	dst->size = src->last;
	
	
	

#endif

#ifdef IS_CV
	
	if (src->vars) {
		int i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->vars = (zend_compiled_variable *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zend_compiled_variable) * src->last_var;

		
	
	
	
	
	

			

			for (i = 0;
					i < src->last_var;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_compiled_variable) == sizeof(src->vars[i]));
	
	
	
	
	

	
	assert(sizeof(zend_compiled_variable) == sizeof((& src->vars[i])[0]));
	

	

	xc_store_zend_compiled_variable(
		
		
		processor, & dst->vars[i], & src->vars[i]
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
	dst->vars = (zend_compiled_variable *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->vars);

		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->vars == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->last_var));
		
	
	
	
	
	
	


	

#	ifndef ZEND_ENGINE_2_4
	dst->size_var = src->last_var;
	
	
	

#	endif
#else
		
#endif

		
	assert(sizeof(zend_uint) == sizeof(src->T));
		
	
	
	
	
	
	


	


#ifdef ZEND_ENGINE_2_5
		
	assert(sizeof(zend_uint) == sizeof(src->nested_calls));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->used_stack));
		
	
	
	
	
	
	


	

#endif

	
	if (src->brk_cont_array) {
		last_brk_cont_t i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->brk_cont_array = (zend_brk_cont_element *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zend_brk_cont_element) * src->last_brk_cont;

		
	
	
	
	
	

			

			for (i = 0;
					i < src->last_brk_cont;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_brk_cont_element) == sizeof(src->brk_cont_array[i]));
	
	
	
	
	

	
	assert(sizeof(zend_brk_cont_element) == sizeof((& src->brk_cont_array[i])[0]));
	

	

	xc_store_zend_brk_cont_element(
		
		
		processor, & dst->brk_cont_array[i], & src->brk_cont_array[i]
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
	dst->brk_cont_array = (zend_brk_cont_element *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->brk_cont_array);

		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->brk_cont_array == NULL);

	
	
	


	}

		
	assert(sizeof(last_brk_cont_t) == sizeof(src->last_brk_cont));
		
	
	
	
	
	
	



	

#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uint) == sizeof(src->current_brk_cont));
		
	
	
	
	
	
	


	

#endif
#ifndef ZEND_ENGINE_2
		
	assert(sizeof(zend_bool) == sizeof(src->uses_globals));
		
	
	
	
	
	
	


	

#endif

#ifdef ZEND_ENGINE_2
	
	if (src->try_catch_array) {
		int i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->try_catch_array = (zend_try_catch_element *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zend_try_catch_element) * src->last_try_catch;

		
	
	
	
	
	

			

			for (i = 0;
					i < src->last_try_catch;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_try_catch_element) == sizeof(src->try_catch_array[i]));
	
	
	
	
	

	
	assert(sizeof(zend_try_catch_element) == sizeof((& src->try_catch_array[i])[0]));
	

	

	xc_store_zend_try_catch_element(
		
		
		processor, & dst->try_catch_array[i], & src->try_catch_array[i]
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
	dst->try_catch_array = (zend_try_catch_element *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->try_catch_array);

		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->try_catch_array == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->last_try_catch));
		
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_5
		
	assert(sizeof(zend_bool) == sizeof(src->has_finally_block));
		
	
	
	
	
	
	


	

#endif

	
	
	if (src->static_variables) {
		
		
	
	
	

	
	assert(sizeof(HashTable) == sizeof(( src->static_variables)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->static_variables = (HashTable *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(HashTable);

		
	
	
	
	
	


	

	xc_store_HashTable_zval_ptr(
		
		
		processor,  dst->static_variables,  src->static_variables
		
		
		TSRMLS_CC
	);

	

	
	
	dst->static_variables = (HashTable *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->static_variables);


	}
	else {
		
		
	
	
	assert(src->static_variables == NULL);

	}
	
	
	
	



#ifndef ZEND_ENGINE_2_4
	
	
	
	

		
	assert(sizeof(int) == sizeof(src->backpatch_count));
		
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_3
		
	assert(sizeof(zend_uint) == sizeof(src->this_var));
		
	
	
	
	
	
	


	

#endif

#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_bool) == sizeof(src->done_pass_two));
		
	
	
	
	
	
	


	

#endif
	/* 5.0 <= ver < 5.3 */
#if defined(ZEND_ENGINE_2) && !defined(ZEND_ENGINE_2_3)
		
	assert(sizeof(zend_bool) == sizeof(src->uses_this));
		
	
	
	
	
	
	


	

#endif

	 
	
	
	

	
	
	
	
	
	
	
	
	if (src->filename == NULL) {
		
		
	}
	else {
		
		
		dst->filename =  ZSTR_S(xc_store_string_n(processor, IS_STRING, ZSTR(src->filename), strlen(src->filename) + 1 C_RELAYLINE));
		
		
	dst->filename = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->filename);

		
	}
	
	
	
	
	
	
	

#ifdef IS_UNICODE
	
		 
	
	
	

	
	
	
	
	
	
	
	
	if (src->script_encoding == NULL) {
		
		
	}
	else {
		
		
		dst->script_encoding =  ZSTR_S(xc_store_string_n(processor, IS_STRING, ZSTR(src->script_encoding), strlen(src->script_encoding) + 1 C_RELAYLINE));
		
		
	dst->script_encoding = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->script_encoding);

		
	}
	
	
	
	
	
	
	

	
#endif
#ifdef ZEND_ENGINE_2
		
	assert(sizeof(zend_uint) == sizeof(src->line_start));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->line_end));
		
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->doc_comment_len));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_U(dst->doc_comment) =  ZSTR_U(xc_store_string_n(processor, IS_UNICODE, src->doc_comment, src->doc_comment_len + 1 C_RELAYLINE));
		
		
	ZSTR_U(dst->doc_comment) = (UChar *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_U(dst->doc_comment));

		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->doc_comment) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->doc_comment, src->doc_comment_len + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->doc_comment) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->doc_comment));

		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->doc_comment) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->doc_comment, src->doc_comment_len + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->doc_comment) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->doc_comment));

		
	}
	
	
	
	
	
	
	

#endif
	

#endif
#ifdef ZEND_COMPILE_DELAYED_BINDING
		
	assert(sizeof(zend_uint) == sizeof(src->early_binding));
		
	
	
	
	
	
	


	
;
#endif

	/* reserved */
	
	
	
	

#if defined(HARDENING_PATCH) && HARDENING_PATCH
		
	assert(sizeof(zend_bool) == sizeof(src->created_by_eval));
		
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_4
	dst->run_time_cache = NULL;
	
	
	

		
	assert(sizeof(int) == sizeof(src->last_cache_slot));
		
	
	
	
	
	
	


	

#endif
	} while (0);
	

#ifdef ZEND_ENGINE_2
		dst->prototype = (processor->active_class_entry_src && src->prototype) ? (zend_function *) -1 : NULL;
	
	
	
	


#endif

#ifdef ZEND_ENGINE_2
	
	if (src->scope) {
		dst->scope = (zend_class_entry *) xc_get_class_num(processor,src->scope);
		
#ifdef IS_UNICODE
		
#else
		
#endif
	}
	else {
		
	
	
	assert(src->scope == NULL);

	}

	
	
	

	
		if (src->scope) {
			xc_fix_method(processor, dst TSRMLS_CC);
		}
	
#endif

	
	}
	
		processor->active_op_array_dst = NULL;
		processor->active_op_array_src = NULL;
	
	

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


#ifdef HAVE_XCACHE_CONSTANT

	
	

/* {{{ xc_store_xc_constinfo_t */
	
 	  	 	static void inline 	xc_store_xc_constinfo_t( 		 		 		xc_processor_t *processor, xc_constinfo_t *dst, const xc_constinfo_t * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(xc_constinfo_t));
			do {
		

		
				
	assert(sizeof(zend_uint) == sizeof(src->key_size));
		
	
	
	
	
	
	


	

#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
	
	
	
	
	


	

#endif
	
		
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (src->type == IS_UNICODE) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->key) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_U(dst->key) =  ZSTR_U(xc_store_string_n(processor, IS_UNICODE, src->key, src->key_size C_RELAYLINE));
		
		
	ZSTR_U(dst->key) = (UChar *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_U(dst->key));

		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->key) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->key, src->key_size C_RELAYLINE));
		
		
	ZSTR_S(dst->key) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->key));

		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->key) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->key, src->key_size C_RELAYLINE));
		
		
	ZSTR_S(dst->key) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->key));

		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
	
	
	
	
	


	

	
	
	assert(sizeof(zend_constant) == sizeof(src->constant));
	
	
	
	
	

	
	assert(sizeof(zend_constant) == sizeof((& src->constant)[0]));
	

	

	xc_store_zend_constant(
		
		
		processor, & dst->constant, & src->constant
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	



		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif


	
	

/* {{{ xc_store_xc_op_array_info_detail_t */
	
 	  	 	static void inline 	xc_store_xc_op_array_info_detail_t( 		 		 		xc_processor_t *processor, xc_op_array_info_detail_t *dst, const xc_op_array_info_detail_t * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(xc_op_array_info_detail_t));
			do {
		

		
				
	assert(sizeof(zend_uint) == sizeof(src->index));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->info));
		
	
	
	
	
	
	


	


		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_store_xc_op_array_info_t */
	
 	  	 	static void inline 	xc_store_xc_op_array_info_t( 		 		 		xc_processor_t *processor, xc_op_array_info_t *dst, const xc_op_array_info_t * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(xc_op_array_info_t));
			do {
		

		
		#ifdef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uint) == sizeof(src->literalinfo_cnt));
		
	
	
	
	
	
	


	

	
	if (src->literalinfos) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->literalinfos = (xc_op_array_info_detail_t *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(xc_op_array_info_detail_t) * src->literalinfo_cnt;

		
	
	
	
	
	

			

			for (i = 0;
					i < src->literalinfo_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_op_array_info_detail_t) == sizeof(src->literalinfos[i]));
	
	
	
	
	

	
	assert(sizeof(xc_op_array_info_detail_t) == sizeof((& src->literalinfos[i])[0]));
	

	

	xc_store_xc_op_array_info_detail_t(
		
		
		processor, & dst->literalinfos[i], & src->literalinfos[i]
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
	dst->literalinfos = (xc_op_array_info_detail_t *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->literalinfos);

		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->literalinfos == NULL);

	
	
	


	}

#else
		
	assert(sizeof(zend_uint) == sizeof(src->oplineinfo_cnt));
		
	
	
	
	
	
	


	

	
	if (src->oplineinfos) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->oplineinfos = (xc_op_array_info_detail_t *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(xc_op_array_info_detail_t) * src->oplineinfo_cnt;

		
	
	
	
	
	

			

			for (i = 0;
					i < src->oplineinfo_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_op_array_info_detail_t) == sizeof(src->oplineinfos[i]));
	
	
	
	
	

	
	assert(sizeof(xc_op_array_info_detail_t) == sizeof((& src->oplineinfos[i])[0]));
	

	

	xc_store_xc_op_array_info_detail_t(
		
		
		processor, & dst->oplineinfos[i], & src->oplineinfos[i]
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
	dst->oplineinfos = (xc_op_array_info_detail_t *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->oplineinfos);

		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->oplineinfos == NULL);

	
	
	


	}

#endif

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	



	
	

/* {{{ xc_store_xc_funcinfo_t */
	
		/* export:  	  	 	void 	xc_store_xc_funcinfo_t( 		 		 		xc_processor_t *processor, xc_funcinfo_t *dst, const xc_funcinfo_t * const src 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_store_xc_funcinfo_t( 		 		 		xc_processor_t *processor, xc_funcinfo_t *dst, const xc_funcinfo_t * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(xc_funcinfo_t));
			do {
		

		
				
	assert(sizeof(zend_uint) == sizeof(src->key_size));
		
	
	
	
	
	
	


	

#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
	
	
	
	
	


	

#endif
	
		
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (src->type == IS_UNICODE) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->key) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_U(dst->key) =  ZSTR_U(xc_store_string_n(processor, IS_UNICODE, src->key, src->key_size C_RELAYLINE));
		
		
	ZSTR_U(dst->key) = (UChar *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_U(dst->key));

		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->key) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->key, src->key_size C_RELAYLINE));
		
		
	ZSTR_S(dst->key) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->key));

		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->key) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->key, src->key_size C_RELAYLINE));
		
		
	ZSTR_S(dst->key) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->key));

		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
	
	
	
	
	


	

	
		
	
	assert(sizeof(xc_op_array_info_t) == sizeof(src->op_array_info));
	
	
	
	
	

	
	assert(sizeof(xc_op_array_info_t) == sizeof((& src->op_array_info)[0]));
	

	

	xc_store_xc_op_array_info_t(
		
		
		processor, & dst->op_array_info, & src->op_array_info
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
	
	
	assert(sizeof(zend_function) == sizeof(src->func));
	
	
	
	
	

	
	assert(sizeof(zend_function) == sizeof((& src->func)[0]));
	

	

	xc_store_zend_function(
		
		
		processor, & dst->func, & src->func
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	



		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_store_xc_classinfo_t */
	
		/* export:  	  	 	void 	xc_store_xc_classinfo_t( 		 		 		xc_processor_t *processor, xc_classinfo_t *dst, const xc_classinfo_t * const src 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_store_xc_classinfo_t( 		 		 		xc_processor_t *processor, xc_classinfo_t *dst, const xc_classinfo_t * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(xc_classinfo_t));
			do {
		

		
				
	assert(sizeof(zend_uint) == sizeof(src->key_size));
		
	
	
	
	
	
	


	

#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
	
	
	
	
	


	

#endif
	
		
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (src->type == IS_UNICODE) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->key) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_U(dst->key) =  ZSTR_U(xc_store_string_n(processor, IS_UNICODE, src->key, src->key_size C_RELAYLINE));
		
		
	ZSTR_U(dst->key) = (UChar *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_U(dst->key));

		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->key) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->key, src->key_size C_RELAYLINE));
		
		
	ZSTR_S(dst->key) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->key));

		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->key) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->key, src->key_size C_RELAYLINE));
		
		
	ZSTR_S(dst->key) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->key));

		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->methodinfo_cnt));
		
	
	
	
	
	
	


	

	
		
	if (src->methodinfos) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->methodinfos = (xc_op_array_info_t *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(xc_op_array_info_t) * src->methodinfo_cnt;

		
	
	
	
	
	

			

			for (i = 0;
					i < src->methodinfo_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_op_array_info_t) == sizeof(src->methodinfos[i]));
	
	
	
	
	

	
	assert(sizeof(xc_op_array_info_t) == sizeof((& src->methodinfos[i])[0]));
	

	

	xc_store_xc_op_array_info_t(
		
		
		processor, & dst->methodinfos[i], & src->methodinfos[i]
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
	dst->methodinfos = (xc_op_array_info_t *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->methodinfos);

		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->methodinfos == NULL);

	
	
	


	}

	
	
#ifdef ZEND_ENGINE_2
	
	
	if (src->cest) {
		
		
	
	
	

	
	assert(sizeof(zend_class_entry) == sizeof(( src->cest)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->cest = (zend_class_entry *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zend_class_entry);

		
	
	
	
	
	


	

	xc_store_zend_class_entry(
		
		
		processor,  dst->cest,  src->cest
		
		
		TSRMLS_CC
	);

	

	
	
	dst->cest = (zend_class_entry *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->cest);


	}
	else {
		
		
	
	
	assert(src->cest == NULL);

	}
	
	
	
	


#else
	
	
	assert(sizeof(zend_class_entry) == sizeof(src->cest));
	
	
	
	
	

	
	assert(sizeof(zend_class_entry) == sizeof((& src->cest)[0]));
	

	

	xc_store_zend_class_entry(
		
		
		processor, & dst->cest, & src->cest
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


#endif
#ifndef ZEND_COMPILE_DELAYED_BINDING
		
	assert(sizeof(int) == sizeof(src->oplineno));
		
	
	
	
	
	
	


	

#endif

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


#ifdef ZEND_ENGINE_2_1

	
	

/* {{{ xc_store_xc_autoglobal_t */
	
 	  	 	static void inline 	xc_store_xc_autoglobal_t( 		 		 		xc_processor_t *processor, xc_autoglobal_t *dst, const xc_autoglobal_t * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(xc_autoglobal_t));
			do {
		

		
				
	assert(sizeof(zend_uint) == sizeof(src->key_len));
		
	
	
	
	
	
	


	

#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
	
	
	
	
	


	

#endif
	
		 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (src->type == IS_UNICODE) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->key) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_U(dst->key) =  ZSTR_U(xc_store_string_n(processor, IS_UNICODE, src->key, src->key_len + 1 C_RELAYLINE));
		
		
	ZSTR_U(dst->key) = (UChar *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_U(dst->key));

		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->key) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->key, src->key_len + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->key) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->key));

		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->key) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->key, src->key_len + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->key) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->key));

		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
	
	
	
	
	


	


		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif


#ifdef XCACHE_ERROR_CACHING

	
	

/* {{{ xc_store_xc_compilererror_t */
	
 	  	 	static void inline 	xc_store_xc_compilererror_t( 		 		 		xc_processor_t *processor, xc_compilererror_t *dst, const xc_compilererror_t * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(xc_compilererror_t));
			do {
		

		
				
	assert(sizeof(int) == sizeof(src->type));
		
	
	
	
	
	
	


	

		
	assert(sizeof(uint) == sizeof(src->lineno));
		
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->error_len));
		
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->error == NULL) {
		
		
	}
	else {
		
		
		dst->error =  ZSTR_S(xc_store_string_n(processor, IS_STRING, ZSTR(src->error), src->error_len + 1 C_RELAYLINE));
		
		
	dst->error = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->error);

		
	}
	
	
	
	
	
	
	


		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif


	
	

/* {{{ xc_store_xc_entry_data_php_t */
	
		/* export:  	  	 	void 	xc_store_xc_entry_data_php_t( 		 		 		xc_processor_t *processor, xc_entry_data_php_t *dst, const xc_entry_data_php_t * const src 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_store_xc_entry_data_php_t( 		 		 		xc_processor_t *processor, xc_entry_data_php_t *dst, const xc_entry_data_php_t * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(xc_entry_data_php_t));
			do {
		

		
			
		processor->php_dst = dst;
		processor->php_src = src;
	

	/* skip */
	
	
	
	

		
	assert(sizeof(xc_hash_value_t) == sizeof(src->hvalue));
		
	
	
	
	
	
	


	

		
	assert(sizeof(xc_md5sum_t) == sizeof(src->md5));
	
	
	
	

	

		
	assert(sizeof(zend_ulong) == sizeof(src->refcount));
		
	
	
	
	
	
	


	


		
	assert(sizeof(zend_ulong) == sizeof(src->hits));
		
	
	
	
	
	
	


	

		
	assert(sizeof(size_t) == sizeof(src->size));
		
	
	
	
	
	
	


	


	
		
	
	assert(sizeof(xc_op_array_info_t) == sizeof(src->op_array_info));
	
	
	
	
	

	
	assert(sizeof(xc_op_array_info_t) == sizeof((& src->op_array_info)[0]));
	

	

	xc_store_xc_op_array_info_t(
		
		
		processor, & dst->op_array_info, & src->op_array_info
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
	
	
	if (src->op_array) {
		
		
	
	
	

	
	assert(sizeof(zend_op_array) == sizeof(( src->op_array)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->op_array = (zend_op_array *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(zend_op_array);

		
	
	
	
	
	


	

	xc_store_zend_op_array(
		
		
		processor,  dst->op_array,  src->op_array
		
		
		TSRMLS_CC
	);

	

	
	
	dst->op_array = (zend_op_array *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->op_array);


	}
	else {
		
		
	
	
	assert(src->op_array == NULL);

	}
	
	
	
	



#ifdef HAVE_XCACHE_CONSTANT
		
	assert(sizeof(zend_uint) == sizeof(src->constinfo_cnt));
		
	
	
	
	
	
	


	

	
	if (src->constinfos) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->constinfos = (xc_constinfo_t *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(xc_constinfo_t) * src->constinfo_cnt;

		
	
	
	
	
	

			

			for (i = 0;
					i < src->constinfo_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_constinfo_t) == sizeof(src->constinfos[i]));
	
	
	
	
	

	
	assert(sizeof(xc_constinfo_t) == sizeof((& src->constinfos[i])[0]));
	

	

	xc_store_xc_constinfo_t(
		
		
		processor, & dst->constinfos[i], & src->constinfos[i]
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
	dst->constinfos = (xc_constinfo_t *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->constinfos);

		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->constinfos == NULL);

	
	
	


	}

#endif

		
	assert(sizeof(zend_uint) == sizeof(src->funcinfo_cnt));
		
	
	
	
	
	
	


	

	
	if (src->funcinfos) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->funcinfos = (xc_funcinfo_t *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(xc_funcinfo_t) * src->funcinfo_cnt;

		
	
	
	
	
	

			

			for (i = 0;
					i < src->funcinfo_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_funcinfo_t) == sizeof(src->funcinfos[i]));
	
	
	
	
	

	
	assert(sizeof(xc_funcinfo_t) == sizeof((& src->funcinfos[i])[0]));
	

	

	xc_store_xc_funcinfo_t(
		
		
		processor, & dst->funcinfos[i], & src->funcinfos[i]
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
	dst->funcinfos = (xc_funcinfo_t *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->funcinfos);

		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->funcinfos == NULL);

	
	
	


	}


		
	assert(sizeof(zend_uint) == sizeof(src->classinfo_cnt));
		
	
	
	
	
	
	


	

	
	if (src->classinfos) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->classinfos = (xc_classinfo_t *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(xc_classinfo_t) * src->classinfo_cnt;

		
	
	
	
	
	

			

			for (i = 0;
					i < src->classinfo_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_classinfo_t) == sizeof(src->classinfos[i]));
	
	
	
	
	

	
	assert(sizeof(xc_classinfo_t) == sizeof((& src->classinfos[i])[0]));
	

	

	xc_store_xc_classinfo_t(
		
		
		processor, & dst->classinfos[i], & src->classinfos[i]
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
	dst->classinfos = (xc_classinfo_t *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->classinfos);

		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->classinfos == NULL);

	
	
	


	}

#ifdef ZEND_ENGINE_2_1
		
	assert(sizeof(zend_uint) == sizeof(src->autoglobal_cnt));
		
	
	
	
	
	
	


	

	
		
	if (src->autoglobals) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->autoglobals = (xc_autoglobal_t *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(xc_autoglobal_t) * src->autoglobal_cnt;

		
	
	
	
	
	

			

			for (i = 0;
					i < src->autoglobal_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_autoglobal_t) == sizeof(src->autoglobals[i]));
	
	
	
	
	

	
	assert(sizeof(xc_autoglobal_t) == sizeof((& src->autoglobals[i])[0]));
	

	

	xc_store_xc_autoglobal_t(
		
		
		processor, & dst->autoglobals[i], & src->autoglobals[i]
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
	dst->autoglobals = (xc_autoglobal_t *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->autoglobals);

		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->autoglobals == NULL);

	
	
	


	}

	
#endif
#ifdef XCACHE_ERROR_CACHING
		
	assert(sizeof(zend_uint) == sizeof(src->compilererror_cnt));
		
	
	
	
	
	
	


	

	
		
	if (src->compilererrors) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
		
		
		dst->compilererrors = (xc_compilererror_t *) (processor->p = (char *) ALIGN(processor->p));
		
				
			
		processor->p += sizeof(xc_compilererror_t) * src->compilererror_cnt;

		
	
	
	
	
	

			

			for (i = 0;
					i < src->compilererror_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_compilererror_t) == sizeof(src->compilererrors[i]));
	
	
	
	
	

	
	assert(sizeof(xc_compilererror_t) == sizeof((& src->compilererrors[i])[0]));
	

	

	xc_store_xc_compilererror_t(
		
		
		processor, & dst->compilererrors[i], & src->compilererrors[i]
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
	dst->compilererrors = (xc_compilererror_t *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->compilererrors);

		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->compilererrors == NULL);

	
	
	


	}

	
#endif
#ifndef ZEND_COMPILE_DELAYED_BINDING
		
	assert(sizeof(zend_bool) == sizeof(src->have_early_binding));
		
	
	
	
	
	
	


	

#endif
		
	assert(sizeof(zend_bool) == sizeof(src->have_references));
		
	
	
	
	
	
	


	


		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_store_xc_entry_t */
	
 	  	 	static void inline 	xc_store_xc_entry_t( 		 		 		xc_processor_t *processor, xc_entry_t *dst, const xc_entry_t * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(xc_entry_t));
			do {
		

		
			/* skip */
	
	
	
	

		
	assert(sizeof(size_t) == sizeof(src->size));
		
	
	
	
	
	
	


	


		
	assert(sizeof(time_t) == sizeof(src->ctime));
		
	
	
	
	
	
	


	

		
	assert(sizeof(time_t) == sizeof(src->atime));
		
	
	
	
	
	
	


	

		
	assert(sizeof(time_t) == sizeof(src->dtime));
		
	
	
	
	
	
	


	

		
	assert(sizeof(long) == sizeof(src->ttl));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_ulong) == sizeof(src->hits));
		
	
	
	
	
	
	


	

	
	
	
	
 
		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_store_xc_entry_php_t */
	
		/* export:  	  	 	void 	xc_store_xc_entry_php_t( 		 		 		xc_processor_t *processor, xc_entry_php_t *dst, const xc_entry_php_t * const src 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_store_xc_entry_php_t( 		 		 		xc_processor_t *processor, xc_entry_php_t *dst, const xc_entry_php_t * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(xc_entry_php_t));
			do {
		

		
			
	
	assert(sizeof(xc_entry_t) == sizeof(src->entry));
	
	
	
	
	

	
	assert(sizeof(xc_entry_t) == sizeof((& src->entry)[0]));
	

	

	xc_store_xc_entry_t(
		
		
		processor, & dst->entry, & src->entry
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
	

			
	assert(sizeof(int) == sizeof(src->entry.name.str.len));
		
	
	
	

	

		
			  
	
	
	
	
	
	
	
	
	if (src->entry.name.str.val == NULL) {
		
		
	}
	else {
		
		
		dst->entry.name.str.val =  ZSTR_S(xc_store_string_n(processor, IS_STRING, ZSTR(src->entry.name.str.val), src->entry.name.str.len + 1 C_RELAYLINE));
		
		
	dst->entry.name.str.val = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->entry.name.str.val);

		
	}
	
	
	
	
	
	
	

		
	
	
	


	
	
	
	


	dst->refcount = 0; 
	
	
	

		
	assert(sizeof(time_t) == sizeof(src->file_mtime));
		
	
	
	
	
	
	


	

		
	assert(sizeof(size_t) == sizeof(src->file_size));
		
	
	
	
	
	
	


	

		
	assert(sizeof(size_t) == sizeof(src->file_device));
		
	
	
	
	
	
	


	

		
	assert(sizeof(size_t) == sizeof(src->file_inode));
		
	
	
	
	
	
	


	


		
	assert(sizeof(size_t) == sizeof(src->filepath_len));
		
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->filepath == NULL) {
		
		
	}
	else {
		
		
		dst->filepath =  ZSTR_S(xc_store_string_n(processor, IS_STRING, ZSTR(src->filepath), src->filepath_len + 1 C_RELAYLINE));
		
		
	dst->filepath = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->filepath);

		
	}
	
	
	
	
	
	
	

		
	assert(sizeof(size_t) == sizeof(src->dirpath_len));
		
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->dirpath == NULL) {
		
		
	}
	else {
		
		
		dst->dirpath =  ZSTR_S(xc_store_string_n(processor, IS_STRING, ZSTR(src->dirpath), src->dirpath_len + 1 C_RELAYLINE));
		
		
	dst->dirpath = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->dirpath);

		
	}
	
	
	
	
	
	
	

#ifdef IS_UNICODE
		
	assert(sizeof(int) == sizeof(src->ufilepath_len));
		
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->ufilepath == NULL) {
		
		
	}
	else {
		
		
		dst->ufilepath =  ZSTR_U(xc_store_string_n(processor, IS_UNICODE, ZSTR(src->ufilepath), src->ufilepath_len + 1 C_RELAYLINE));
		
		
	dst->ufilepath = (UChar *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->ufilepath);

		
	}
	
	
	
	
	
	
	

		
	assert(sizeof(int) == sizeof(src->udirpath_len));
		
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->udirpath == NULL) {
		
		
	}
	else {
		
		
		dst->udirpath =  ZSTR_U(xc_store_string_n(processor, IS_UNICODE, ZSTR(src->udirpath), src->udirpath_len + 1 C_RELAYLINE));
		
		
	dst->udirpath = (UChar *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->udirpath);

		
	}
	
	
	
	
	
	
	

#endif

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_store_xc_entry_var_t */
	
		/* export:  	  	 	void 	xc_store_xc_entry_var_t( 		 		 		xc_processor_t *processor, xc_entry_var_t *dst, const xc_entry_var_t * const src 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_store_xc_entry_var_t( 		 		 		xc_processor_t *processor, xc_entry_var_t *dst, const xc_entry_var_t * const src 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(xc_entry_var_t));
			do {
		

		
			
	
	assert(sizeof(xc_entry_t) == sizeof(src->entry));
	
	
	
	
	

	
	assert(sizeof(xc_entry_t) == sizeof((& src->entry)[0]));
	

	

	xc_store_xc_entry_t(
		
		
		processor, & dst->entry, & src->entry
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	



#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->name_type));
		
	
	
	
	
	
	


	

#endif
		
	
	

#ifdef IS_UNICODE
		if (src->name_type == IS_UNICODE) {
				
	assert(sizeof(int32_t) == sizeof(src->entry.name.ustr.len));
		
	
	
	

	

		}
		else {
				
	assert(sizeof(int) == sizeof(src->entry.name.str.len));
		
	
	
	

	

		}
#else
			
	assert(sizeof(int) == sizeof(src->entry.name.str.len));
		
	
	
	

	

#endif
		
#ifdef IS_UNICODE
			 
	
#ifdef IS_UNICODE
	
	
	
		if (src->name_type == IS_UNICODE) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->entry.name.uni.val) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_U(dst->entry.name.uni.val) =  ZSTR_U(xc_store_string_n(processor, IS_UNICODE, src->entry.name.uni.val, src->entry.name.uni.len + 1 C_RELAYLINE));
		
		
	ZSTR_U(dst->entry.name.uni.val) = (UChar *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_U(dst->entry.name.uni.val));

		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->entry.name.uni.val) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->entry.name.uni.val) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->entry.name.uni.val, src->entry.name.uni.len + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->entry.name.uni.val) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->entry.name.uni.val));

		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->entry.name.uni.val) == NULL) {
		
		
	}
	else {
		
		
		ZSTR_S(dst->entry.name.uni.val) =  ZSTR_S(xc_store_string_n(processor, IS_STRING, src->entry.name.uni.val, src->entry.name.uni.len + 1 C_RELAYLINE));
		
		
	ZSTR_S(dst->entry.name.uni.val) = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)ZSTR_S(dst->entry.name.uni.val));

		
	}
	
	
	
	
	
	
	

#endif
	

#else
			  
	
	
	
	
	
	
	
	
	if (src->entry.name.str.val == NULL) {
		
		
	}
	else {
		
		
		dst->entry.name.str.val =  ZSTR_S(xc_store_string_n(processor, IS_STRING, ZSTR(src->entry.name.str.val), src->entry.name.str.len + 1 C_RELAYLINE));
		
		
	dst->entry.name.str.val = (char *) processor->shm->handlers->to_readonly(processor->shm, (void *)dst->entry.name.str.val);

		
	}
	
	
	
	
	
	
	

#endif
		
	
	
	

	
	
	
	
	
	

	
	assert(sizeof(zval_ptr) == sizeof((& src->value)[0]));
	

	

	xc_store_zval_ptr(
		
		
		processor, & dst->value, & src->value
		
		
		TSRMLS_CC
	);

	

	
	

		
	assert(sizeof(zend_bool) == sizeof(src->have_references));
		
	
	
	
	
	
	


	

	
	
	
	


		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	




  /* {{{ Pre-declare */
 	  	 	void 	xc_restore_zval( 		 		 		 		xc_processor_t *processor, zval *dst, const zval * const src 		 		TSRMLS_DC 	); 	
 	  	 	static void inline 	xc_restore_zval_ptr( 		 		 		 		xc_processor_t *processor, zval_ptr *dst, const zval_ptr * const src 		 		TSRMLS_DC 	); 	
 	  	 	static void inline 	xc_restore_zval_ptr_nullable( 		 		 		 		xc_processor_t *processor, zval_ptr_nullable *dst, const zval_ptr_nullable * const src 		 		TSRMLS_DC 	); 	
 	  	 	void 	xc_restore_zend_op_array( 		 		 		 		xc_processor_t *processor, zend_op_array *dst, const zend_op_array * const src 		 		TSRMLS_DC 	); 	
 	  	 	void 	xc_restore_zend_class_entry( 		 		 		 		xc_processor_t *processor, zend_class_entry *dst, const zend_class_entry * const src 		 		TSRMLS_DC 	); 	
#ifdef HAVE_XCACHE_CONSTANT
 	  	 	static void inline 	xc_restore_zend_constant( 		 		 		 		xc_processor_t *processor, zend_constant *dst, const zend_constant * const src 		 		TSRMLS_DC 	); 	
#endif
 	  	 	void 	xc_restore_zend_function( 		 		 		 		xc_processor_t *processor, zend_function *dst, const zend_function * const src 		 		TSRMLS_DC 	); 	
 	  	 	void 	xc_restore_xc_entry_var_t( 		 		 		 		xc_processor_t *processor, xc_entry_var_t *dst, const xc_entry_var_t * const src 		 		TSRMLS_DC 	); 	
 	  	 	void 	xc_restore_xc_entry_php_t( 		 		 		 		xc_processor_t *processor, xc_entry_php_t *dst, const xc_entry_php_t * const src 		 		TSRMLS_DC 	); 	
#ifdef ZEND_ENGINE_2
 	  	 	static void inline 	xc_restore_zend_property_info( 		 		 		 		xc_processor_t *processor, zend_property_info *dst, const zend_property_info * const src 		 		TSRMLS_DC 	); 	
#endif
/* }}} */
#ifdef IS_CV

	
	

/* {{{ xc_restore_zend_compiled_variable */
	
 	  	 	static void inline 	xc_restore_zend_compiled_variable( 		 		 		 		xc_processor_t *processor, zend_compiled_variable *dst, const zend_compiled_variable * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_compiled_variable));
			do {
		

		
				
	assert(sizeof(int) == sizeof(src->name_len));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_U(dst->name) =  eustrndup (ZSTR_U(src->name), (src->name_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->name) =  estrndup (ZSTR_S(src->name), (src->name_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->name) =  estrndup (ZSTR_S(src->name), (src->name_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

		
	assert(sizeof(ulong) == sizeof(src->hash_value));
		
	
	
	
	
	
	


	


		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_restore_zend_uint */
	
 	  	 	static void inline 	xc_restore_zend_uint( 		 		 		 		xc_processor_t *processor, zend_uint *dst, const zend_uint * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_uint));
			do {
		

		
			dst[0] = src[0];
	
	

		
		
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#ifndef ZEND_ENGINE_2

	
	

/* {{{ xc_restore_int */
	
 	  	 	static void inline 	xc_restore_int( 		 		 		 		xc_processor_t *processor, int *dst, const int * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(int));
			do {
		

		
			*dst = *src;
	
	

		
		
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif
#ifdef ZEND_ENGINE_2

	
	

/* {{{ xc_restore_zend_try_catch_element */
	
 	  	 	static void inline 	xc_restore_zend_try_catch_element( 		 		 		 		xc_processor_t *processor, zend_try_catch_element *dst, const zend_try_catch_element * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_try_catch_element));
			do {
		

		
				
	assert(sizeof(zend_uint) == sizeof(src->try_op));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->catch_op));
		
	
	
	
	
	
	


	

#ifdef ZEND_ENGINE_2_5
		
	assert(sizeof(zend_uint) == sizeof(src->finally_op));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->finally_end));
		
	
	
	
	
	
	


	

#endif

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_restore_zend_brk_cont_element */
	
 	  	 	static void inline 	xc_restore_zend_brk_cont_element( 		 		 		 		xc_processor_t *processor, zend_brk_cont_element *dst, const zend_brk_cont_element * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_brk_cont_element));
			do {
		

		
		#ifdef ZEND_ENGINE_2_2
		
	assert(sizeof(int) == sizeof(src->start));
		
	
	
	
	
	
	


	

#endif
		
	assert(sizeof(int) == sizeof(src->cont));
		
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->brk));
		
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->parent));
		
	
	
	
	
	
	


	


		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	
	

/* {{{ xc_restore_HashTable_zval_ptr */
	
 	  	 	static void inline 	xc_restore_HashTable_zval_ptr( 		 		 		 		xc_processor_t *processor, HashTable *dst, const HashTable * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(HashTable));
			do {
		

		
		
		
	

				
				Bucket *srcBucket;
		Bucket *pnew = NULL, *prev = NULL;
		zend_bool first = 1;
				uint n;
		size_t bucketsize;

#if defined(HARDENING_PATCH_HASH_PROTECT) && HARDENING_PATCH_HASH_PROTECT
		dst->canary = zend_hash_canary; 
	
	
	

#endif
			
	assert(sizeof(uint) == sizeof(src->nTableSize));
		
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nTableMask));
		
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nNumOfElements));
		
	
	
	
	
	
	


	

			
	assert(sizeof(ulong) == sizeof(src->nNextFreeElement));
		
	
	
	
	
	
	


	

		dst->pInternalPointer = NULL;	/* Used for element traversal */ 
	
	
	

		dst->pListHead = NULL; 
	
	
	

#ifdef ZEND_ENGINE_2_4
	if (src->nTableMask) {
#endif
		
	
	
		
	
	
	/* allocate */
	
	
	
			dst->arBuckets = (Bucket* *) ecalloc(src->nTableSize, sizeof(Bucket*));
		
	
	
	
	

		
	
	
	

		
	
	

		for (srcBucket = src->pListHead; srcBucket != NULL; srcBucket = srcBucket->pListNext) {
			

			bucketsize = BUCKET_SIZE(srcBucket);
			
	
	
		
	
	
	/* allocate */
	
	
	
			pnew = (Bucket *) emalloc(sizeof(char) * bucketsize);
			
		
	
	
	
	

			
#ifdef ZEND_ENGINE_2_4
				memcpy(pnew, srcBucket, BUCKET_HEAD_SIZE(Bucket));
				if (BUCKET_KEY_SIZE(srcBucket)) {
					memcpy((char *) (pnew + 1), srcBucket->arKey, BUCKET_KEY_SIZE(srcBucket));
					pnew->arKey = (const char *) (pnew + 1);
				}
				else {
					pnew->arKey = NULL;
				}
#else
				memcpy(pnew, srcBucket, bucketsize);
#endif
				n = srcBucket->h & src->nTableMask;
				/* pnew into hash node chain */
				pnew->pLast = NULL;
				pnew->pNext = dst->arBuckets[n];
				if (pnew->pNext) {
					pnew->pNext->pLast = pnew;
				}
				dst->arBuckets[n] = pnew;
			
			
			if (sizeof(void *) == sizeof(zval_ptr)) {
				pnew->pData = &pnew->pDataPtr;
								
	
	
	

	
	assert(sizeof(zval_ptr) == sizeof((  ((zval_ptr*)srcBucket->pData))[0]));
	

	

	xc_restore_zval_ptr(
		
		
		
		processor,   pnew->pData,   ((zval_ptr*)srcBucket->pData)
		
		TSRMLS_CC
	);

	

	
	

			}
			else {
				
	
	
	

	
	assert(sizeof(zval_ptr) == sizeof(( ((zval_ptr*)srcBucket->pData))[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
			pnew->pData = (zval_ptr *) emalloc(sizeof(zval_ptr));
			
		
	
	
	
	


	

	xc_restore_zval_ptr(
		
		
		
		processor,  pnew->pData,  ((zval_ptr*)srcBucket->pData)
		
		TSRMLS_CC
	);

	

	
	

				pnew->pDataPtr = NULL;
			}

			if (first) {
				dst->pListHead = pnew;
				first = 0;
			}

			
				/* flat link */
				pnew->pListLast = prev;
				pnew->pListNext = NULL;
				if (prev) {
					prev->pListNext = pnew;
				}
				prev = pnew;
			
		}
		
	
	

				
#ifdef ZEND_ENGINE_2_4
	}
	else { /* if (src->nTableMask) */
		
	
	
	

	}
#endif
		dst->pListTail = pnew; 
	
	
	

		dst->pDestructor = src->pDestructor; 
	
	
	

			
	assert(sizeof(zend_bool) == sizeof(src->persistent));
		
	
	
	
	
	
	


	

#ifdef IS_UNICODE
			
	assert(sizeof(zend_bool) == sizeof(src->unicode));
		
	
	
	
	
	
	


	

#endif
			
	assert(sizeof(unsigned char) == sizeof(src->nApplyCount));
		
	
	
	
	
	
	


	

			
	assert(sizeof(zend_bool) == sizeof(src->bApplyProtection));
		
	
	
	
	
	
	


	

#if ZEND_DEBUG
			
	assert(sizeof(int) == sizeof(src->inconsistent));
		
	
	
	
	
	
	


	

#endif
				
	
		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	



	
	
	

/* {{{ xc_restore_HashTable_zend_function */
	
		/* export:  	  	 	void 	xc_restore_HashTable_zend_function( 		 		 		 		xc_processor_t *processor, HashTable *dst, const HashTable * const src 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_restore_HashTable_zend_function( 		 		 		 		xc_processor_t *processor, HashTable *dst, const HashTable * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(HashTable));
			do {
		

		
		
		
	

				
				Bucket *srcBucket;
		Bucket *pnew = NULL, *prev = NULL;
		zend_bool first = 1;
				uint n;
		size_t bucketsize;

#if defined(HARDENING_PATCH_HASH_PROTECT) && HARDENING_PATCH_HASH_PROTECT
		dst->canary = zend_hash_canary; 
	
	
	

#endif
			
	assert(sizeof(uint) == sizeof(src->nTableSize));
		
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nTableMask));
		
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nNumOfElements));
		
	
	
	
	
	
	


	

			
	assert(sizeof(ulong) == sizeof(src->nNextFreeElement));
		
	
	
	
	
	
	


	

		dst->pInternalPointer = NULL;	/* Used for element traversal */ 
	
	
	

		dst->pListHead = NULL; 
	
	
	

#ifdef ZEND_ENGINE_2_4
	if (src->nTableMask) {
#endif
		
	
	
		
	
	
	/* allocate */
	
	
	
			dst->arBuckets = (Bucket* *) ecalloc(src->nTableSize, sizeof(Bucket*));
		
	
	
	
	

		
	
	
	

		
	
	

		for (srcBucket = src->pListHead; srcBucket != NULL; srcBucket = srcBucket->pListNext) {
			

			bucketsize = BUCKET_SIZE(srcBucket);
			
	
	
		
	
	
	/* allocate */
	
	
	
			pnew = (Bucket *) emalloc(sizeof(char) * bucketsize);
			
		
	
	
	
	

			
#ifdef ZEND_ENGINE_2_4
				memcpy(pnew, srcBucket, BUCKET_HEAD_SIZE(Bucket));
				if (BUCKET_KEY_SIZE(srcBucket)) {
					memcpy((char *) (pnew + 1), srcBucket->arKey, BUCKET_KEY_SIZE(srcBucket));
					pnew->arKey = (const char *) (pnew + 1);
				}
				else {
					pnew->arKey = NULL;
				}
#else
				memcpy(pnew, srcBucket, bucketsize);
#endif
				n = srcBucket->h & src->nTableMask;
				/* pnew into hash node chain */
				pnew->pLast = NULL;
				pnew->pNext = dst->arBuckets[n];
				if (pnew->pNext) {
					pnew->pNext->pLast = pnew;
				}
				dst->arBuckets[n] = pnew;
			
			
			if (sizeof(void *) == sizeof(zend_function)) {
				pnew->pData = &pnew->pDataPtr;
								
	
	
	

	
	assert(sizeof(zend_function) == sizeof((  ((zend_function*)srcBucket->pData))[0]));
	

	

	xc_restore_zend_function(
		
		
		
		processor,   pnew->pData,   ((zend_function*)srcBucket->pData)
		
		TSRMLS_CC
	);

	

	
	

			}
			else {
				
	
	
	

	
	assert(sizeof(zend_function) == sizeof(( ((zend_function*)srcBucket->pData))[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
			pnew->pData = (zend_function *) emalloc(sizeof(zend_function));
			
		
	
	
	
	


	

	xc_restore_zend_function(
		
		
		
		processor,  pnew->pData,  ((zend_function*)srcBucket->pData)
		
		TSRMLS_CC
	);

	

	
	

				pnew->pDataPtr = NULL;
			}

			if (first) {
				dst->pListHead = pnew;
				first = 0;
			}

			
				/* flat link */
				pnew->pListLast = prev;
				pnew->pListNext = NULL;
				if (prev) {
					prev->pListNext = pnew;
				}
				prev = pnew;
			
		}
		
	
	

				
#ifdef ZEND_ENGINE_2_4
	}
	else { /* if (src->nTableMask) */
		
	
	
	

	}
#endif
		dst->pListTail = pnew; 
	
	
	

		dst->pDestructor = src->pDestructor; 
	
	
	

			
	assert(sizeof(zend_bool) == sizeof(src->persistent));
		
	
	
	
	
	
	


	

#ifdef IS_UNICODE
			
	assert(sizeof(zend_bool) == sizeof(src->unicode));
		
	
	
	
	
	
	


	

#endif
			
	assert(sizeof(unsigned char) == sizeof(src->nApplyCount));
		
	
	
	
	
	
	


	

			
	assert(sizeof(zend_bool) == sizeof(src->bApplyProtection));
		
	
	
	
	
	
	


	

#if ZEND_DEBUG
			
	assert(sizeof(int) == sizeof(src->inconsistent));
		
	
	
	
	
	
	


	

#endif
				
	
		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


#ifdef ZEND_ENGINE_2

	
	
	

/* {{{ xc_restore_HashTable_zend_property_info */
	
 	  	 	static void inline 	xc_restore_HashTable_zend_property_info( 		 		 		 		xc_processor_t *processor, HashTable *dst, const HashTable * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(HashTable));
			do {
		

		
		
		
	

				
				Bucket *srcBucket;
		Bucket *pnew = NULL, *prev = NULL;
		zend_bool first = 1;
				uint n;
		size_t bucketsize;

#if defined(HARDENING_PATCH_HASH_PROTECT) && HARDENING_PATCH_HASH_PROTECT
		dst->canary = zend_hash_canary; 
	
	
	

#endif
			
	assert(sizeof(uint) == sizeof(src->nTableSize));
		
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nTableMask));
		
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nNumOfElements));
		
	
	
	
	
	
	


	

			
	assert(sizeof(ulong) == sizeof(src->nNextFreeElement));
		
	
	
	
	
	
	


	

		dst->pInternalPointer = NULL;	/* Used for element traversal */ 
	
	
	

		dst->pListHead = NULL; 
	
	
	

#ifdef ZEND_ENGINE_2_4
	if (src->nTableMask) {
#endif
		
	
	
		
	
	
	/* allocate */
	
	
	
			dst->arBuckets = (Bucket* *) ecalloc(src->nTableSize, sizeof(Bucket*));
		
	
	
	
	

		
	
	
	

		
	
	

		for (srcBucket = src->pListHead; srcBucket != NULL; srcBucket = srcBucket->pListNext) {
			

			bucketsize = BUCKET_SIZE(srcBucket);
			
	
	
		
	
	
	/* allocate */
	
	
	
			pnew = (Bucket *) emalloc(sizeof(char) * bucketsize);
			
		
	
	
	
	

			
#ifdef ZEND_ENGINE_2_4
				memcpy(pnew, srcBucket, BUCKET_HEAD_SIZE(Bucket));
				if (BUCKET_KEY_SIZE(srcBucket)) {
					memcpy((char *) (pnew + 1), srcBucket->arKey, BUCKET_KEY_SIZE(srcBucket));
					pnew->arKey = (const char *) (pnew + 1);
				}
				else {
					pnew->arKey = NULL;
				}
#else
				memcpy(pnew, srcBucket, bucketsize);
#endif
				n = srcBucket->h & src->nTableMask;
				/* pnew into hash node chain */
				pnew->pLast = NULL;
				pnew->pNext = dst->arBuckets[n];
				if (pnew->pNext) {
					pnew->pNext->pLast = pnew;
				}
				dst->arBuckets[n] = pnew;
			
			
			if (sizeof(void *) == sizeof(zend_property_info)) {
				pnew->pData = &pnew->pDataPtr;
								
	
	
	

	
	assert(sizeof(zend_property_info) == sizeof((  ((zend_property_info*)srcBucket->pData))[0]));
	

	

	xc_restore_zend_property_info(
		
		
		
		processor,   pnew->pData,   ((zend_property_info*)srcBucket->pData)
		
		TSRMLS_CC
	);

	

	
	

			}
			else {
				
	
	
	

	
	assert(sizeof(zend_property_info) == sizeof(( ((zend_property_info*)srcBucket->pData))[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
			pnew->pData = (zend_property_info *) emalloc(sizeof(zend_property_info));
			
		
	
	
	
	


	

	xc_restore_zend_property_info(
		
		
		
		processor,  pnew->pData,  ((zend_property_info*)srcBucket->pData)
		
		TSRMLS_CC
	);

	

	
	

				pnew->pDataPtr = NULL;
			}

			if (first) {
				dst->pListHead = pnew;
				first = 0;
			}

			
				/* flat link */
				pnew->pListLast = prev;
				pnew->pListNext = NULL;
				if (prev) {
					prev->pListNext = pnew;
				}
				prev = pnew;
			
		}
		
	
	

				
#ifdef ZEND_ENGINE_2_4
	}
	else { /* if (src->nTableMask) */
		
	
	
	

	}
#endif
		dst->pListTail = pnew; 
	
	
	

		dst->pDestructor = src->pDestructor; 
	
	
	

			
	assert(sizeof(zend_bool) == sizeof(src->persistent));
		
	
	
	
	
	
	


	

#ifdef IS_UNICODE
			
	assert(sizeof(zend_bool) == sizeof(src->unicode));
		
	
	
	
	
	
	


	

#endif
			
	assert(sizeof(unsigned char) == sizeof(src->nApplyCount));
		
	
	
	
	
	
	


	

			
	assert(sizeof(zend_bool) == sizeof(src->bApplyProtection));
		
	
	
	
	
	
	


	

#if ZEND_DEBUG
			
	assert(sizeof(int) == sizeof(src->inconsistent));
		
	
	
	
	
	
	


	

#endif
				
	
		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


#endif
#ifdef IS_CONSTANT_AST


	
	

/* {{{ xc_restore_zend_ast */
	
		/* export:  	  	 	void 	xc_restore_zend_ast( 		 		 		 		xc_processor_t *processor, zend_ast *dst, const zend_ast * const src 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_restore_zend_ast( 		 		 		 		xc_processor_t *processor, zend_ast *dst, const zend_ast * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_ast));
			do {
		

		
				zend_ushort i;
			
	assert(sizeof(zend_ushort) == sizeof(src->kind));
		
	
	
	
	
	
	


	

			
	assert(sizeof(zend_ushort) == sizeof(src->children));
		
	
	
	
	
	
	


	

		
	
	
	

		
	
	

			if (src->kind == ZEND_CONST) {
				assert(src->u.val);
				
					dst->u.val = (zval *) (dst + 1);
					memcpy(dst->u.val, src->u.val, sizeof(zval));
				
				
	
	
	

	
	assert(sizeof(zval) == sizeof((  src->u.val)[0]));
	

	

	xc_restore_zval(
		
		
		
		processor,   dst->u.val,   src->u.val
		
		TSRMLS_CC
	);

	

	
	

				
			}
			else {
				for (i = 0; i < src->children; ++i) {
					zend_ast *src_ast = (&src->u.child)[i];
					if (src_ast) {
						{
	
		size_t zend_ast_size = (src_ast->kind == ZEND_CONST)
		 ? sizeof(zend_ast) + sizeof(zval)
		 : sizeof(zend_ast) + sizeof(zend_ast *) * (src_ast->children - 1);
	

	
	
							
	
	
		
	
	
	/* allocate */
	
	
	
			(&dst->u.child)[i] = (zend_ast *) emalloc(zend_ast_size);
			
		
	
	
	
	

							
	
	
	

	
	assert(sizeof(zend_ast) == sizeof((  src_ast)[0]));
	

	

	xc_restore_zend_ast(
		
		
		
		processor,   (&dst->u.child)[i],   src_ast
		
		TSRMLS_CC
	);

	

	
	

						
	
}

						
					}
					else {
						(&dst->u.child)[i] = NULL;
					}
				}
			}
		
	
	


		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_restore_zval */
	
		/* export:  	  	 	void 	xc_restore_zval( 		 		 		 		xc_processor_t *processor, zval *dst, const zval * const src 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_restore_zval( 		 		 		 		xc_processor_t *processor, zval *dst, const zval * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zval));
			do {
		

		
			
				/* Variable information */
		
	
	

		switch ((Z_TYPE_P(src) & IS_CONSTANT_TYPE_MASK)) {
			case IS_LONG:
			case IS_RESOURCE:
			case IS_BOOL:
					
	assert(sizeof(long) == sizeof(src->value.lval));
		
	
	
	

	

				break;
			case IS_DOUBLE:
					
	assert(sizeof(double) == sizeof(src->value.dval));
		
	
	
	

	

				break;
			case IS_NULL:
				
				break;

			case IS_CONSTANT:
#ifdef IS_UNICODE
				if (UG(unicode)) {
					goto proc_unicode;
				}
#endif
			case IS_STRING:
#ifdef FLAG_IS_BC
			case FLAG_IS_BC:
#endif
					
	assert(sizeof(int) == sizeof(src->value.str.len));
		
	
	
	

	

				  
	
	
	
	
	
	
	
	
	if (src->value.str.val == NULL) {
		
		
	}
	else {
		
		
		
		
			dst->value.str.val =  estrndup (src->value.str.val, (src->value.str.len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

				break;
#ifdef IS_UNICODE
			case IS_UNICODE:
proc_unicode:
					
	assert(sizeof(int32_t) == sizeof(src->value.uni.len));
		
	
	
	

	

				 
	
#ifdef IS_UNICODE
	
	
	
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->value.uni.val) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_U(dst->value.uni.val) =  eustrndup (ZSTR_U(src->value.uni.val), (src->value.uni.len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

	
#else
	
	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->value.uni.val) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->value.uni.val) =  estrndup (ZSTR_S(src->value.uni.val), (src->value.uni.len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

				break;
#endif

			case IS_ARRAY:
#ifdef IS_CONSTANT_ARRAY
			case IS_CONSTANT_ARRAY:
#endif
				assert(src->value.ht);
				
	
	if (src->value.ht) {
		
		
	
	
	

	
	assert(sizeof(HashTable) == sizeof(( src->value.ht)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
			ALLOC_HASHTABLE(dst->value.ht);
			
		
	
	
	
	


	

	xc_restore_HashTable_zval_ptr(
		
		
		
		processor,  dst->value.ht,  src->value.ht
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->value.ht == NULL);

	}
	

				break;

#ifdef IS_CONSTANT_AST
			case IS_CONSTANT_AST:
				assert(src->value.ast);
				{
	
		size_t zend_ast_size = (src->value.ast->kind == ZEND_CONST)
		 ? sizeof(zend_ast) + sizeof(zval)
		 : sizeof(zend_ast) + sizeof(zend_ast *) * (src->value.ast->children - 1);
	

	
	
	
	if (src->value.ast) {
		
		
	
	
	

	
	assert(sizeof(zend_ast) == sizeof(( src->value.ast)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
			dst->value.ast = (zend_ast *) emalloc(zend_ast_size);
			
		
	
	
	
	


	

	xc_restore_zend_ast(
		
		
		
		processor,  dst->value.ast,  src->value.ast
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->value.ast == NULL);

	}
	

	
}

				break;
#endif

			case IS_OBJECT:
				
				#ifndef ZEND_ENGINE_2
				
	
	if (src->value.obj.ce) {
		
		
	
	
	

	
	assert(sizeof(zend_class_entry) == sizeof(( src->value.obj.ce)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
			dst->value.obj.ce = (zend_class_entry *) emalloc(sizeof(zend_class_entry));
			
		
	
	
	
	


	

	xc_restore_zend_class_entry(
		
		
		
		processor,  dst->value.obj.ce,  src->value.obj.ce
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->value.obj.ce == NULL);

	}
	

				
	
	if (src->value.obj.properties) {
		
		
	
	
	

	
	assert(sizeof(HashTable) == sizeof(( src->value.obj.properties)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
			ALLOC_HASHTABLE(dst->value.obj.properties);
			
		
	
	
	
	


	

	xc_restore_HashTable_zval_ptr(
		
		
		
		processor,  dst->value.obj.properties,  src->value.obj.properties
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->value.obj.properties == NULL);

	}
	

#endif
				break;

			default:
				assert(0);
		}
		
	
	

		
	
	
	

			
	assert(sizeof(xc_zval_type_t) == sizeof(src->type));
			
	
	
	
	
	
	



	

#ifdef ZEND_ENGINE_2_3
			
	assert(sizeof(zend_uchar) == sizeof(src->is_ref__gc));
		
	
	
	
	
	
	


	

#else
			
	assert(sizeof(zend_uchar) == sizeof(src->is_ref));
		
	
	
	
	
	
	


	

#endif

#ifdef ZEND_ENGINE_2_3
			
	assert(sizeof(zend_uint) == sizeof(src->refcount__gc));
		
	
	
	
	
	
	


	

#elif defined(ZEND_ENGINE_2)
			
	assert(sizeof(zend_uint) == sizeof(src->refcount));
		
	
	
	
	
	
	


	

#else
			
	assert(sizeof(zend_ushort) == sizeof(src->refcount));
		
	
	
	
	
	
	


	

#endif
	
		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_restore_zval_ptr */
	
 	  	 	static void inline 	xc_restore_zval_ptr( 		 		 		 		xc_processor_t *processor, zval_ptr *dst, const zval_ptr * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zval_ptr));
			do {
		

		
			
		do {
			
				if (processor->reference) {
					zval_ptr *ppzv;
					if (zend_hash_find(&processor->zvalptrs, (char *) &src[0], sizeof(src[0]), (void **) &ppzv) == SUCCESS) {
						
							dst[0] = *ppzv;
							/* *dst is updated */
													
						
						
						assert(!xc_is_shm(dst[0]));
						break;
					}
				}
			
			
			
	
	
		
	
	
	/* allocate */
	
	
	
			ALLOC_ZVAL(dst[0]);
			
		
	
	
	
	

			
				if (processor->reference) {
					
						zval_ptr pzv = dst[0];
						
					
					if (zend_hash_add(&processor->zvalptrs, (char *) &src[0], sizeof(src[0]), (void *) &pzv, sizeof(pzv), NULL) == SUCCESS) {
						/* first add, go on */
											}
					else {
						assert(0);
					}
				}
			
			
							
			
			
	
	
	

	
	assert(sizeof(zval) == sizeof((  src[0])[0]));
	

	

	xc_restore_zval(
		
		
		
		processor,   dst[0],   src[0]
		
		TSRMLS_CC
	);

	

	
	

			
		} while (0);
	
	

		
		
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_restore_zval_ptr_nullable */
	
 	  	 	static void inline 	xc_restore_zval_ptr_nullable( 		 		 		 		xc_processor_t *processor, zval_ptr_nullable *dst, const zval_ptr_nullable * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zval_ptr_nullable));
			do {
		

		
			if (src[0]) {
		
		
	
	
	

	
	assert(sizeof(zval_ptr) == sizeof((  src)[0]));
	

	xc_restore_zval_ptr(
		
		
		
		processor,   dst,   src
		
		TSRMLS_CC
	);

	
	

		
	}
	else {
		
	
	
	assert(src[0] == NULL);

	}
	

		
		
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#ifdef ZEND_ENGINE_2

	
	

/* {{{ xc_restore_zend_arg_info */
	
 	  	 	static void inline 	xc_restore_zend_arg_info( 		 		 		 		xc_processor_t *processor, zend_arg_info *dst, const zend_arg_info * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_arg_info));
			do {
		

		
				
	assert(sizeof(zend_uint) == sizeof(src->name_len));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_U(dst->name) =  eustrndup (ZSTR_U(src->name), (src->name_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->name) =  estrndup (ZSTR_S(src->name), (src->name_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->name) =  estrndup (ZSTR_S(src->name), (src->name_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

		
	assert(sizeof(zend_uint) == sizeof(src->class_name_len));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->class_name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_U(dst->class_name) =  eustrndup (ZSTR_U(src->class_name), (src->class_name_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->class_name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->class_name) =  estrndup (ZSTR_S(src->class_name), (src->class_name_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->class_name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->class_name) =  estrndup (ZSTR_S(src->class_name), (src->class_name_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

#ifdef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uchar) == sizeof(src->type_hint));
		
	
	
	
	
	
	


	

#elif defined(ZEND_ENGINE_2_1)
		
	assert(sizeof(zend_bool) == sizeof(src->array_type_hint));
		
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_6
		
	assert(sizeof(zend_uchar) == sizeof(src->pass_by_reference));
		
	
	
	
	
	
	


	

#endif
		
	assert(sizeof(zend_bool) == sizeof(src->allow_null));
		
	
	
	
	
	
	


	


#ifdef ZEND_ENGINE_2_6
		
	assert(sizeof(zend_bool) == sizeof(src->is_variadic));
		
	
	
	
	
	
	


	

#else
		
	assert(sizeof(zend_bool) == sizeof(src->pass_by_reference));
		
	
	
	
	
	
	


	

#endif

#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_bool) == sizeof(src->return_reference));
		
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->required_num_args));
		
	
	
	
	
	
	


	

#endif

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif
#ifdef HAVE_XCACHE_CONSTANT

	
	

/* {{{ xc_restore_zend_constant */
	
 	  	 	static void inline 	xc_restore_zend_constant( 		 		 		 		xc_processor_t *processor, zend_constant *dst, const zend_constant * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_constant));
			do {
		

		
			
	
	assert(sizeof(zval) == sizeof(src->value));
	
	
	
	
	

	
	assert(sizeof(zval) == sizeof((& src->value)[0]));
	

	

	xc_restore_zval(
		
		
		
		processor, & dst->value, & src->value
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


		
	assert(sizeof(int) == sizeof(src->flags));
		
	
	
	
	
	
	


	

		
	assert(sizeof(uint) == sizeof(src->name_len));
		
	
	
	
	
	
	


	

	
	
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_U(dst->name) =  eustrndup (ZSTR_U(src->name), (src->name_len) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->name) =  zend_strndup (ZSTR_S(src->name), (src->name_len) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->name) =  zend_strndup (ZSTR_S(src->name), (src->name_len) - 1);
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	assert(sizeof(int) == sizeof(src->module_number));
		
	
	
	
	
	
	


	


		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_restore_zend_function */
	
		/* export:  	  	 	void 	xc_restore_zend_function( 		 		 		 		xc_processor_t *processor, zend_function *dst, const zend_function * const src 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_restore_zend_function( 		 		 		 		xc_processor_t *processor, zend_function *dst, const zend_function * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_function));
			do {
		

		
			
	
	

	switch (src->type) {
	case ZEND_INTERNAL_FUNCTION:
	case ZEND_OVERLOADED_FUNCTION:
		
		break;

	case ZEND_USER_FUNCTION:
	case ZEND_EVAL_CODE:
		
		
	
	assert(sizeof(zend_op_array) == sizeof(src->op_array));
	
	
	
	
	

	
	assert(sizeof(zend_op_array) == sizeof((& src->op_array)[0]));
	

	

	xc_restore_zend_op_array(
		
		
		
		processor, & dst->op_array, & src->op_array
		
		TSRMLS_CC
	);

	

	
	

	

		break;

	default:
		assert(0);
	}
	
	
	

	

		
		
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#ifdef ZEND_ENGINE_2

	
	

/* {{{ xc_restore_zend_property_info */
	
 	  	 	static void inline 	xc_restore_zend_property_info( 		 		 		 		xc_processor_t *processor, zend_property_info *dst, const zend_property_info * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_property_info));
			do {
		

		
				
	assert(sizeof(zend_uint) == sizeof(src->flags));
		
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->name_length));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_U(dst->name) =  eustrndup (ZSTR_U(src->name), (src->name_length + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->name) =  estrndup (ZSTR_S(src->name), (src->name_length + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->name) =  estrndup (ZSTR_S(src->name), (src->name_length + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
	
	
	
	
	


	

#ifdef ZEND_ENGINE_2_4
		
	assert(sizeof(int) == sizeof(src->offset));
		
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_1
		
	assert(sizeof(int) == sizeof(src->doc_comment_len));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_U(dst->doc_comment) =  eustrndup (ZSTR_U(src->doc_comment), (src->doc_comment_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->doc_comment) =  estrndup (ZSTR_S(src->doc_comment), (src->doc_comment_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->doc_comment) =  estrndup (ZSTR_S(src->doc_comment), (src->doc_comment_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

#endif
	#if defined(ZEND_ENGINE_2_2)
	
	if (src->ce) {
		
		dst->ce = xc_get_class(processor,(zend_ulong) src->ce);
#ifdef IS_UNICODE
		
#else
		
#endif
	}
	else {
		
	
	
	assert(src->ce == NULL);

	}

	
	
	

#endif

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif
#ifdef ZEND_ENGINE_2_4

	
	

/* {{{ xc_restore_zend_trait_method_reference */
	
 	  	 	static void inline 	xc_restore_zend_trait_method_reference( 		 		 		 		xc_processor_t *processor, zend_trait_method_reference *dst, const zend_trait_method_reference * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_trait_method_reference));
			do {
		

		
				
	assert(sizeof(unsigned int) == sizeof(src->mname_len));
		
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->method_name == NULL) {
		
		
	}
	else {
		
		
		
		
			dst->method_name =  estrndup (src->method_name, (src->mname_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

	
	
	
	
	assert(src->ce == NULL);

	
	
	


		
	assert(sizeof(unsigned int) == sizeof(src->cname_len));
		
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->class_name == NULL) {
		
		
	}
	else {
		
		
		
		
			dst->class_name =  estrndup (src->class_name, (src->cname_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	


		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_restore_zend_trait_alias */
	
 	  	 	static void inline 	xc_restore_zend_trait_alias( 		 		 		 		xc_processor_t *processor, zend_trait_alias *dst, const zend_trait_alias * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_trait_alias));
			do {
		

		
			
	
	if (src->trait_method) {
		
		
	
	
	

	
	assert(sizeof(zend_trait_method_reference) == sizeof(( src->trait_method)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
			dst->trait_method = (zend_trait_method_reference *) emalloc(sizeof(zend_trait_method_reference));
			
		
	
	
	
	


	

	xc_restore_zend_trait_method_reference(
		
		
		
		processor,  dst->trait_method,  src->trait_method
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->trait_method == NULL);

	}
	
	
	
	


		
	assert(sizeof(unsigned int) == sizeof(src->alias_len));
		
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->alias == NULL) {
		
		
	}
	else {
		
		
		
		
			dst->alias =  estrndup (src->alias, (src->alias_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		
	assert(sizeof(zend_uint) == sizeof(src->modifiers));
		
	
	
	
	
	
	


	

#ifndef ZEND_ENGINE_2_5
	
	
	
	
	assert(src->function == NULL);

	
	
	


#endif

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_restore_zend_trait_precedence */
	
 	  	 	static void inline 	xc_restore_zend_trait_precedence( 		 		 		 		xc_processor_t *processor, zend_trait_precedence *dst, const zend_trait_precedence * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_trait_precedence));
			do {
		

		
			
	
	if (src->trait_method) {
		
		
	
	
	

	
	assert(sizeof(zend_trait_method_reference) == sizeof(( src->trait_method)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
			dst->trait_method = (zend_trait_method_reference *) emalloc(sizeof(zend_trait_method_reference));
			
		
	
	
	
	


	

	xc_restore_zend_trait_method_reference(
		
		
		
		processor,  dst->trait_method,  src->trait_method
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->trait_method == NULL);

	}
	
	
	
	


		if (src->exclude_from_classes) {
		int LOOPCOUNTER;
		
						
				size_t count = 0;
				while (src->exclude_from_classes[count]) {
					++count;
				}
				++count;
				
			
			
	
	
		
	
	
	/* allocate */
	
	
	
			dst->exclude_from_classes = (zend_class_entry* *) emalloc(sizeof(xc_ztstring) * count);
			
		
	
	
	
	

			

			for (LOOPCOUNTER = 0;
					src->exclude_from_classes[LOOPCOUNTER];
					++LOOPCOUNTER) {
				
	
	

					
					
						
	assert(sizeof(xc_ztstring) == sizeof((xc_ztstring) src->exclude_from_classes [LOOPCOUNTER]));
		
	 
	
	
	
	
	
	
	
	
	if ((xc_ztstring) src->exclude_from_classes [LOOPCOUNTER] == NULL) {
		
		
	}
	else {
		
		
		
		
			dst->exclude_from_classes [LOOPCOUNTER] = (zend_class_entry *) estrndup ((xc_ztstring) src->exclude_from_classes [LOOPCOUNTER], (strlen((xc_ztstring) src->exclude_from_classes [LOOPCOUNTER]) + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

	

	

					
					
				
	
	

			}
						dst->exclude_from_classes[LOOPCOUNTER] = NULL;
				
	
	
	

	}
	else {
		
	
	
	
	assert(src->exclude_from_classes == NULL);

	
	
	


	}

#ifndef ZEND_ENGINE_2_5
	
	
	
	
	assert(src->function == NULL);

	
	
	


#endif

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_restore_zend_trait_alias_ptr */
	
 	  	 	static void inline 	xc_restore_zend_trait_alias_ptr( 		 		 		 		xc_processor_t *processor, zend_trait_alias_ptr *dst, const zend_trait_alias_ptr * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_trait_alias_ptr));
			do {
		

		
			
		
	
	
		
	
	
	/* allocate */
	
	
	
			dst[0] = (zend_trait_alias *) emalloc(sizeof(zend_trait_alias));
			
		
	
	
	
	

		
	
	
	

	
	assert(sizeof(zend_trait_alias) == sizeof((  src[0])[0]));
	

	

	xc_restore_zend_trait_alias(
		
		
		
		processor,   dst[0],   src[0]
		
		TSRMLS_CC
	);

	

	
	

		
	
	

		
		
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_restore_zend_trait_precedence_ptr */
	
 	  	 	static void inline 	xc_restore_zend_trait_precedence_ptr( 		 		 		 		xc_processor_t *processor, zend_trait_precedence_ptr *dst, const zend_trait_precedence_ptr * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_trait_precedence_ptr));
			do {
		

		
			
		
	
	
		
	
	
	/* allocate */
	
	
	
			dst[0] = (zend_trait_precedence *) emalloc(sizeof(zend_trait_precedence));
			
		
	
	
	
	

		
	
	
	

	
	assert(sizeof(zend_trait_precedence) == sizeof((  src[0])[0]));
	

	

	xc_restore_zend_trait_precedence(
		
		
		
		processor,   dst[0],   src[0]
		
		TSRMLS_CC
	);

	

	
	

		
	
	

		
		
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_restore_zend_class_entry */
	
		/* export:  	  	 	void 	xc_restore_zend_class_entry( 		 		 		 		xc_processor_t *processor, zend_class_entry *dst, const zend_class_entry * const src 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_restore_zend_class_entry( 		 		 		 		xc_processor_t *processor, zend_class_entry *dst, const zend_class_entry * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_class_entry));
			do {
		

		
			
		processor->active_class_entry_src = src;
		processor->active_class_entry_dst = dst;
	
		
	assert(sizeof(char) == sizeof(src->type));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->name_length));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_U(dst->name) =  eustrndup (ZSTR_U(src->name), (src->name_length + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->name) =  estrndup (ZSTR_S(src->name), (src->name_length + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->name) =  estrndup (ZSTR_S(src->name), (src->name_length + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

	
#ifndef ZEND_ENGINE_2
		/* just copy parent and resolve on install_class */
		
	
	
	

#else
		
	if (src->parent) {
		
		dst->parent = xc_get_class(processor,(zend_ulong) src->parent);
#ifdef IS_UNICODE
		
#else
		
#endif
	}
	else {
		
	
	
	assert(src->parent == NULL);

	}

	
	
	

#endif
	
#ifdef ZEND_ENGINE_2
		
	assert(sizeof(int) == sizeof(src->refcount));
		
	
	
	
	
	
	


	

#else
	
	
	if (src->refcount) {
		
		
	
	
	

	
	assert(sizeof(int) == sizeof(( src->refcount)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
			dst->refcount = (int *) emalloc(sizeof(int));
			
		
	
	
	
	


	

	xc_restore_int(
		
		
		
		processor,  dst->refcount,  src->refcount
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->refcount == NULL);

	}
	
	
	
	


#endif
#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_bool) == sizeof(src->constants_updated));
		
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2
		
	assert(sizeof(zend_uint) == sizeof(src->ce_flags));
		
	
	
	
	
	
	


	

#endif

#ifdef ZEND_ENGINE_2
	
	
	assert(sizeof(HashTable) == sizeof(src->properties_info));
	
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->properties_info)[0]));
	

	

	xc_restore_HashTable_zend_property_info(
		
		
		
		processor, & dst->properties_info, & src->properties_info
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


#endif

#ifdef ZEND_ENGINE_2_4
	
	if (src->default_properties_table) {
		int i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
			dst->default_properties_table = (zval_ptr_nullable *) emalloc(sizeof(zval_ptr_nullable) * src->default_properties_count);
			
		
	
	
	
	

			

			for (i = 0;
					i < src->default_properties_count;
					++i) {
				
	
	

					
	
	assert(sizeof(zval_ptr_nullable) == sizeof(src->default_properties_table[i]));
	
	
	
	
	

	
	assert(sizeof(zval_ptr_nullable) == sizeof((& src->default_properties_table[i])[0]));
	

	

	xc_restore_zval_ptr_nullable(
		
		
		
		processor, & dst->default_properties_table[i], & src->default_properties_table[i]
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->default_properties_table == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->default_properties_count));
		
	
	
	
	
	
	


	

	
	if (src->default_static_members_table) {
		int i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
			dst->default_static_members_table = (zval_ptr_nullable *) emalloc(sizeof(zval_ptr_nullable) * src->default_static_members_count);
			
		
	
	
	
	

			

			for (i = 0;
					i < src->default_static_members_count;
					++i) {
				
	
	

					
	
	assert(sizeof(zval_ptr_nullable) == sizeof(src->default_static_members_table[i]));
	
	
	
	
	

	
	assert(sizeof(zval_ptr_nullable) == sizeof((& src->default_static_members_table[i])[0]));
	

	

	xc_restore_zval_ptr_nullable(
		
		
		
		processor, & dst->default_static_members_table[i], & src->default_static_members_table[i]
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->default_static_members_table == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->default_static_members_count));
		
	
	
	
	
	
	


	

	dst->static_members_table = dst->default_static_members_table;
	
	
	
	

#else
	dst->builtin_functions = src->builtin_functions;
	
	
	
	

	
	
	assert(sizeof(HashTable) == sizeof(src->default_properties));
	
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->default_properties)[0]));
	

	

	xc_restore_HashTable_zval_ptr(
		
		
		
		processor, & dst->default_properties, & src->default_properties
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


#	ifdef ZEND_ENGINE_2_1
	
	
	assert(sizeof(HashTable) == sizeof(src->default_static_members));
	
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->default_static_members)[0]));
	

	

	xc_restore_HashTable_zval_ptr(
		
		
		
		processor, & dst->default_static_members, & src->default_static_members
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	dst->static_members = &dst->default_static_members;
	
	
	
	

#	elif defined(ZEND_ENGINE_2)
	
	
	if (src->static_members) {
		
		
	
	
	

	
	assert(sizeof(HashTable) == sizeof(( src->static_members)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
			ALLOC_HASHTABLE(dst->static_members);
			
		
	
	
	
	


	

	xc_restore_HashTable_zval_ptr(
		
		
		
		processor,  dst->static_members,  src->static_members
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->static_members == NULL);

	}
	
	
	
	


#	endif
#endif /* ZEND_ENGINE_2_4 */

#ifdef ZEND_ENGINE_2
	
	
	assert(sizeof(HashTable) == sizeof(src->constants_table));
	
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->constants_table)[0]));
	

	

	xc_restore_HashTable_zval_ptr(
		
		
		
		processor, & dst->constants_table, & src->constants_table
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	



#ifdef ZEND_ENGINE_2_2
		
	
	
	
	assert(src->interfaces == NULL);

	
	
	


	
	
	
	
	assert(src->num_interfaces == 0);

	
	
	



#	ifdef ZEND_ENGINE_2_4
		
	
	
	
	assert(src->traits == NULL);

	
	
	


	
	
	
	
	assert(src->num_traits == 0);

	
	
	


	
	if (src->trait_aliases) {
		size_t i; 
		
	

		
						
				size_t count = 0;
				while (src->trait_aliases[count]) {
					++count;
				}
				++count;
				
			
			
	
	
		
	
	
	/* allocate */
	
	
	
			dst->trait_aliases = (zend_trait_alias_ptr *) emalloc(sizeof(zend_trait_alias_ptr) * count);
			
		
	
	
	
	

			

			for (i = 0;
					src->trait_aliases[i];
					++i) {
				
	
	

					
	
	assert(sizeof(zend_trait_alias_ptr) == sizeof(src->trait_aliases[i]));
	
	
	
	
	

	
	assert(sizeof(zend_trait_alias_ptr) == sizeof((& src->trait_aliases[i])[0]));
	

	

	xc_restore_zend_trait_alias_ptr(
		
		
		
		processor, & dst->trait_aliases[i], & src->trait_aliases[i]
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						dst->trait_aliases[i] = NULL;
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->trait_aliases == NULL);

	
	
	


	}

	
	if (src->trait_precedences) {
		size_t i; 
		
	

		
						
				size_t count = 0;
				while (src->trait_precedences[count]) {
					++count;
				}
				++count;
				
			
			
	
	
		
	
	
	/* allocate */
	
	
	
			dst->trait_precedences = (zend_trait_precedence_ptr *) emalloc(sizeof(zend_trait_precedence_ptr) * count);
			
		
	
	
	
	

			

			for (i = 0;
					src->trait_precedences[i];
					++i) {
				
	
	

					
	
	assert(sizeof(zend_trait_precedence_ptr) == sizeof(src->trait_precedences[i]));
	
	
	
	
	

	
	assert(sizeof(zend_trait_precedence_ptr) == sizeof((& src->trait_precedences[i])[0]));
	

	

	xc_restore_zend_trait_precedence_ptr(
		
		
		
		processor, & dst->trait_precedences[i], & src->trait_precedences[i]
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						dst->trait_precedences[i] = NULL;
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->trait_precedences == NULL);

	
	
	


	}

#	endif
#else
	
		if (src->num_interfaces) {
			
	
	
		
	
	
	/* allocate */
	
	
	
			dst->interfaces = (zend_class_entry* *) ecalloc(src->num_interfaces, sizeof(zend_class_entry*));
		
	
	
	
	

			
	
	
	

		}
		else {
			
	
	
	
	assert(src->interfaces == NULL);

	
	
	


		}
	
		
	assert(sizeof(zend_uint) == sizeof(src->num_interfaces));
		
	
	
	
	
	
	


	

#endif

#	ifdef ZEND_ENGINE_2_4
	
	
	

	dst->info.user.filename = processor->entry_php_src->filepath;
		
	assert(sizeof(zend_uint) == sizeof(src->info.user.line_start));
		
	
	
	

	

		
	assert(sizeof(zend_uint) == sizeof(src->info.user.line_end));
		
	
	
	

	

		
	assert(sizeof(zend_uint) == sizeof(src->info.user.doc_comment_len));
		
	
	
	

	

	 
	
#ifdef IS_UNICODE
	
	
	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->info.user.doc_comment) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_U(dst->info.user.doc_comment) =  eustrndup (ZSTR_U(src->info.user.doc_comment), (src->info.user.doc_comment_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->info.user.doc_comment) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->info.user.doc_comment) =  estrndup (ZSTR_S(src->info.user.doc_comment), (src->info.user.doc_comment_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->info.user.doc_comment) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->info.user.doc_comment) =  estrndup (ZSTR_S(src->info.user.doc_comment), (src->info.user.doc_comment_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

	
	
	

	
	
	
	

#	else
	dst->filename = processor->entry_php_src->filepath;
	
	
	

		
	assert(sizeof(zend_uint) == sizeof(src->line_start));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->line_end));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->doc_comment_len));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_U(dst->doc_comment) =  eustrndup (ZSTR_U(src->doc_comment), (src->doc_comment_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->doc_comment) =  estrndup (ZSTR_S(src->doc_comment), (src->doc_comment_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->doc_comment) =  estrndup (ZSTR_S(src->doc_comment), (src->doc_comment_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

#	endif

	/* # NOT DONE */
#	ifdef ZEND_ENGINE_2_1
	
	
	
	

	
	
	
	

#	endif
	
	
	
	

	
	
	
	

	
	
	
	

	
	
	
	

#	ifdef ZEND_ENGINE_2_3
	
	
	
	

#	endif
#	ifdef ZEND_ENGINE_2_1
	
	
	
	

	
	
	
	

#	endif
	/* deal with it inside xc_fix_method */
	dst->constructor = NULL;
	
	
	

	
	
	
	

	
	
	
	

	
	
	
	

	
	
	
	

/* should be >5.1 */
#	ifdef ZEND_ENGINE_2_1
	
	
	
	

	
	
	
	

#	endif
	
	
	
	

#	ifdef ZEND_CALLSTATIC_FUNC_NAME
	
	
	
	

#	endif
# if defined(ZEND_ENGINE_2_2) || PHP_MAJOR_VERSION >= 6
	
	
	
	

# endif
# if defined(ZEND_ENGINE_2_6)
	
	
	
	

# endif
#	ifndef ZEND_ENGINE_2_4
	/* # NOT DONE */
	
	
	
	

#	endif
#else /* ZEND_ENGINE_2 */
	
	
	
	

	
	
	
	

	
	
	
	

#endif
		
	
	assert(sizeof(HashTable) == sizeof(src->function_table));
	
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->function_table)[0]));
	

	

	xc_restore_HashTable_zend_function(
		
		
		
		processor, & dst->function_table, & src->function_table
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	dst->function_table.pDestructor = ZEND_FUNCTION_DTOR;
	
		processor->active_class_entry_src = NULL;
		processor->active_class_entry_dst = NULL;
	

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#ifdef ZEND_ENGINE_2_4


#else

	
	

/* {{{ xc_restore_znode */
	
 	  	 	static void inline 	xc_restore_znode( 		 		 		 		xc_processor_t *processor, znode *dst, const znode * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(znode));
			do {
		

		
				
	assert(sizeof(xc_op_type) == sizeof(src->op_type));
			
	
	
	
	
	
	



	


#ifdef IS_CV
#	define XCACHE_IS_CV IS_CV
#else
/* compatible with zend optimizer */
#	define XCACHE_IS_CV 16
#endif
	assert(src->op_type == IS_CONST ||
		src->op_type == IS_VAR ||
		src->op_type == XCACHE_IS_CV ||
		src->op_type == IS_TMP_VAR ||
		src->op_type == IS_UNUSED);
		
	
	

	switch (src->op_type) {
		case IS_CONST:
			
	
	assert(sizeof(zval) == sizeof(src->u.constant));
	
	
	
	
	

	
	assert(sizeof(zval) == sizeof((& src->u.constant)[0]));
	

	

	xc_restore_zval(
		
		
		
		processor, & dst->u.constant, & src->u.constant
		
		TSRMLS_CC
	);

	

	
	

	

			break;
		
			
		
	}
	
	
	

	
	
	
	

#if 0
	
	
	
	

#endif
#undef XCACHE_IS_CV

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_restore_zend_op */
	
		/* export:  	  	 	void 	xc_restore_zend_op( 		 		 		 		xc_processor_t *processor, zend_op *dst, const zend_op * const src 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_restore_zend_op( 		 		 		 		xc_processor_t *processor, zend_op *dst, const zend_op * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_op));
			do {
		

		
				
	assert(sizeof(xc_opcode) == sizeof(src->opcode));
			
	
	
	
	
	
	



	

#ifdef ZEND_ENGINE_2_4
	
	#ifndef NDEBUG
	switch ((src->result_type & ~EXT_TYPE_UNUSED)) {
	case IS_CONST:
	case IS_VAR:
	case IS_CV:
	case IS_TMP_VAR:
	case IS_UNUSED:
		break;

	default:
		assert(0);
	}
#endif

		
	
	

	switch ((src->result_type & ~EXT_TYPE_UNUSED)) {
		case IS_CONST:
			
					
	assert(sizeof(zend_uint) == sizeof(src->result.constant));
		
	
	
	

	

			
			break;
		
			
		
	}
	
	
	

	
	
	
	


	#ifndef NDEBUG
	switch ((src->op1_type )) {
	case IS_CONST:
	case IS_VAR:
	case IS_CV:
	case IS_TMP_VAR:
	case IS_UNUSED:
		break;

	default:
		assert(0);
	}
#endif

		
	
	

	switch ((src->op1_type )) {
		case IS_CONST:
			
				
					
						dst->op1 = src->op1;
					
				
			
			break;
		
			
		
	}
	
	
	

	
	
	
	


	#ifndef NDEBUG
	switch ((src->op2_type )) {
	case IS_CONST:
	case IS_VAR:
	case IS_CV:
	case IS_TMP_VAR:
	case IS_UNUSED:
		break;

	default:
		assert(0);
	}
#endif

		
	
	

	switch ((src->op2_type )) {
		case IS_CONST:
			
				
					
						dst->op2 = src->op2;
					
				
			
			break;
		
			
		
	}
	
	
	

	
	
	
	


#else
	
	
	assert(sizeof(znode) == sizeof(src->result));
	
	
	
	
	

	
	assert(sizeof(znode) == sizeof((& src->result)[0]));
	

	

	xc_restore_znode(
		
		
		
		processor, & dst->result, & src->result
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
	assert(sizeof(znode) == sizeof(src->op1));
	
	
	
	
	

	
	assert(sizeof(znode) == sizeof((& src->op1)[0]));
	

	

	xc_restore_znode(
		
		
		
		processor, & dst->op1, & src->op1
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
	assert(sizeof(znode) == sizeof(src->op2));
	
	
	
	
	

	
	assert(sizeof(znode) == sizeof((& src->op2)[0]));
	

	

	xc_restore_znode(
		
		
		
		processor, & dst->op2, & src->op2
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


#endif
		
	assert(sizeof(ulong) == sizeof(src->extended_value));
		
	
	
	
	
	
	


	

		
	assert(sizeof(uint) == sizeof(src->lineno));
		
	
	
	
	
	
	


	

#ifdef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uchar) == sizeof(src->op1_type));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uchar) == sizeof(src->op2_type));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uchar) == sizeof(src->result_type));
		
	
	
	
	
	
	


	

#endif
	
		assert(processor->active_op_array_src);
		assert(processor->active_op_array_dst);
#ifdef ZEND_ENGINE_2_4
		
		
			if (src->op1_type == IS_CONST) {
				dst->op1.constant = src->op1.literal - processor->active_op_array_src->literals;
				dst->op1.literal = &processor->active_op_array_dst->literals[dst->op1.constant];
			}
		
		
			if (src->op2_type == IS_CONST) {
				dst->op2.constant = src->op2.literal - processor->active_op_array_src->literals;
				dst->op2.literal = &processor->active_op_array_dst->literals[dst->op2.constant];
			}
		
#endif
		
#ifdef ZEND_ENGINE_2
		switch (src->opcode) {
#	ifdef ZEND_GOTO
			case ZEND_GOTO:
#	endif
			case ZEND_JMP:
#	ifdef ZEND_FAST_CALL
			case ZEND_FAST_CALL:
#	endif
				assert(Z_OP(src->op1).jmp_addr >= processor->active_op_array_src->opcodes);
				assert(Z_OP(src->op1).jmp_addr - processor->active_op_array_src->opcodes < processor->active_op_array_src->last);
				Z_OP(dst->op1).jmp_addr = processor->active_op_array_dst->opcodes + (Z_OP(src->op1).jmp_addr - processor->active_op_array_src->opcodes);
				assert(Z_OP(dst->op1).jmp_addr >= processor->active_op_array_dst->opcodes);
				assert(Z_OP(dst->op1).jmp_addr - processor->active_op_array_dst->opcodes < processor->active_op_array_dst->last);
				
				break;

			case ZEND_JMPZ:
			case ZEND_JMPNZ:
			case ZEND_JMPZ_EX:
			case ZEND_JMPNZ_EX:
#	ifdef ZEND_JMP_SET
			case ZEND_JMP_SET:
#	endif
#	ifdef ZEND_JMP_SET_VAR
			case ZEND_JMP_SET_VAR:
#	endif
				assert(Z_OP(src->op2).jmp_addr >= processor->active_op_array_src->opcodes);
				assert(Z_OP(src->op2).jmp_addr - processor->active_op_array_src->opcodes < processor->active_op_array_src->last);
				Z_OP(dst->op2).jmp_addr = processor->active_op_array_dst->opcodes + (Z_OP(src->op2).jmp_addr - processor->active_op_array_src->opcodes);
				assert(Z_OP(dst->op2).jmp_addr >= processor->active_op_array_dst->opcodes);
				assert(Z_OP(dst->op2).jmp_addr - processor->active_op_array_dst->opcodes < processor->active_op_array_dst->last);
				
				break;

			default:
				break;
		}
#	endif
	
#ifdef ZEND_ENGINE_2
		
	assert(sizeof(opcode_handler_t) == sizeof(src->handler));
	/* is copying enough? */
	
	
	

	

#endif

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#ifdef ZEND_ENGINE_2_4

	
	

/* {{{ xc_restore_zend_literal */
	
 	  	 	static void inline 	xc_restore_zend_literal( 		 		 		 		xc_processor_t *processor, zend_literal *dst, const zend_literal * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_literal));
			do {
		

		
			
	
	assert(sizeof(zval) == sizeof(src->constant));
	
	
	
	
	

	
	assert(sizeof(zval) == sizeof((& src->constant)[0]));
	

	

	xc_restore_zval(
		
		
		
		processor, & dst->constant, & src->constant
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


		
	assert(sizeof(zend_ulong) == sizeof(src->hash_value));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->cache_slot));
		
	
	
	
	
	
	


	


		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_restore_zend_op_array */
	
		/* export:  	  	 	void 	xc_restore_zend_op_array( 		 		 		 		xc_processor_t *processor, zend_op_array *dst, const zend_op_array * const src 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_restore_zend_op_array( 		 		 		 		xc_processor_t *processor, zend_op_array *dst, const zend_op_array * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(zend_op_array));
			do {
		

		
			
		processor->active_op_array_dst = dst;
		processor->active_op_array_src = src;
	
	
	{
	
	const xc_op_array_info_t *op_array_info = &processor->active_op_array_infos_src[processor->active_op_array_index++];
			#ifdef ZEND_COMPILE_DELAYED_BINDING
	zend_bool need_early_binding = 0;
#else
	zend_bool need_early_binding = processor->php_src->have_early_binding;
#endif
	zend_bool shallow_copy = !processor->readonly_protection && !(src == processor->php_src->op_array && need_early_binding);
	if (shallow_copy) {
		zend_bool gc_arg_info = 0;
		zend_bool gc_opcodes  = 0;
#ifdef ZEND_ENGINE_2_4
		zend_bool gc_literals = 0;
#endif
		/* really fast shallow copy */
		memcpy(dst, src, sizeof(src[0]));
		dst->refcount = &XG(op_array_dummy_refcount_holder);
		XG(op_array_dummy_refcount_holder) = ((zend_uint) -1) / 2;
#ifdef ZEND_ACC_ALIAS
		if ((processor->active_class_entry_src && (processor->active_class_entry_src->ce_flags & ZEND_ACC_TRAIT))) {
			 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->function_name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_U(dst->function_name) =  eustrndup (ZSTR_U(src->function_name), (xc_zstrlen_uchar (src->function_name) + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->function_name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->function_name) =  estrndup (ZSTR_S(src->function_name), (xc_zstrlen_char (src->function_name) + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->function_name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->function_name) =  estrndup (ZSTR_S(src->function_name), (xc_zstrlen_char (src->function_name) + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

		}
#endif
		/* deep */
		
	
	if (src->static_variables) {
		
		
	
	
	

	
	assert(sizeof(HashTable) == sizeof(( src->static_variables)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
			ALLOC_HASHTABLE(dst->static_variables);
			
		
	
	
	
	


	

	xc_restore_HashTable_zval_ptr(
		
		
		
		processor,  dst->static_variables,  src->static_variables
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->static_variables == NULL);

	}
	
	
	
	


#ifdef ZEND_ENGINE_2
		if (src->arg_info) {
			gc_arg_info = 1;
			
	if (src->arg_info) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
			dst->arg_info = (zend_arg_info *) emalloc(sizeof(zend_arg_info) * src->num_args);
			
		
	
	
	
	

			

			for (i = 0;
					i < src->num_args;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_arg_info) == sizeof(src->arg_info[i]));
	
	
	
	
	

	
	assert(sizeof(zend_arg_info) == sizeof((& src->arg_info[i])[0]));
	

	

	xc_restore_zend_arg_info(
		
		
		
		processor, & dst->arg_info[i], & src->arg_info[i]
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->arg_info == NULL);

	
	
	


	}

		}
#endif
		dst->filename = processor->entry_php_src->filepath;

#ifdef ZEND_ENGINE_2_4
		if (src->literals && op_array_info->literalinfo_cnt) {
			gc_opcodes = 1;
			gc_literals = 1;
		}
#else
		if (op_array_info->oplineinfo_cnt) {
			gc_opcodes = 1;
		}
#endif

#ifdef ZEND_ENGINE_2_4
		if (gc_literals) {
						
	
	
	
		
	
	
	/* allocate */
	
	
	
			dst->literals = (zend_literal *) emalloc(sizeof(zend_literal) * src->last_literal);
			
		
	
	
	
	

	
		memcpy(dst->literals, src->literals, sizeof(dst->literals[0]) * src->last_literal);
		

		}
#endif
		if (gc_opcodes) {
			zend_op *opline, *end;
			
	
	
	
		
	
	
	/* allocate */
	
	
	
			dst->opcodes = (zend_op *) emalloc(sizeof(zend_op) * src->last);
			
		
	
	
	
	

	
		memcpy(dst->opcodes, src->opcodes, sizeof(dst->opcodes[0]) * src->last);
		


			for (opline = dst->opcodes, end = opline + src->last; opline < end; ++opline) {
#ifdef ZEND_ENGINE_2_4
				
				
					if (opline->op1_type == IS_CONST) {
						opline->op1.literal = &dst->literals[opline->op1.literal - src->literals];
					}
				
				
					if (opline->op2_type == IS_CONST) {
						opline->op2.literal = &dst->literals[opline->op2.literal - src->literals];
					}
				
				
#endif

				switch (opline->opcode) {
#ifdef ZEND_GOTO
					case ZEND_GOTO:
#endif
					case ZEND_JMP:
#ifdef ZEND_FAST_CALL
					case ZEND_FAST_CALL:
#endif
#ifdef ZEND_ENGINE_2
						Z_OP(opline->op1).jmp_addr = &dst->opcodes[Z_OP(opline->op1).jmp_addr - src->opcodes];
#endif
						break;

					case ZEND_JMPZ:
					case ZEND_JMPNZ:
					case ZEND_JMPZ_EX:
					case ZEND_JMPNZ_EX:
#ifdef ZEND_JMP_SET
					case ZEND_JMP_SET:
#endif
#ifdef ZEND_JMP_SET_VAR
			case ZEND_JMP_SET_VAR:
#endif
#ifdef ZEND_ENGINE_2
						Z_OP(opline->op2).jmp_addr = &dst->opcodes[Z_OP(opline->op2).jmp_addr - src->opcodes];
#endif
						break;

					default:
						break;
				}
			}
		}
		if (gc_arg_info || gc_opcodes
#ifdef ZEND_ENGINE_2_4
		 || gc_literals
#endif
		) {
			xc_gc_op_array_t gc_op_array;
#ifdef ZEND_ENGINE_2
			gc_op_array.num_args = gc_arg_info ? dst->num_args : 0;
			gc_op_array.arg_info = gc_arg_info ? dst->arg_info : NULL;
#endif
			gc_op_array.opcodes  = gc_opcodes ? dst->opcodes : NULL;
#ifdef ZEND_ENGINE_2_4
			gc_op_array.literals = gc_literals ? dst->literals : NULL;
#endif
			xc_gc_add_op_array(&gc_op_array TSRMLS_CC);
		}
		
	}
	else
	
	do {
	
	/* Common elements */
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->function_name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_U(dst->function_name) =  eustrndup (ZSTR_U(src->function_name), (xc_zstrlen_uchar (src->function_name) + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->function_name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->function_name) =  estrndup (ZSTR_S(src->function_name), (xc_zstrlen_char (src->function_name) + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->function_name) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->function_name) =  estrndup (ZSTR_S(src->function_name), (xc_zstrlen_char (src->function_name) + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

#ifdef ZEND_ENGINE_2
		
	assert(sizeof(zend_uint) == sizeof(src->fn_flags));
		
	
	
	
	
	
	


	

	
	if (src->arg_info) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
			dst->arg_info = (zend_arg_info *) emalloc(sizeof(zend_arg_info) * src->num_args);
			
		
	
	
	
	

			

			for (i = 0;
					i < src->num_args;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_arg_info) == sizeof(src->arg_info[i]));
	
	
	
	
	

	
	assert(sizeof(zend_arg_info) == sizeof((& src->arg_info[i])[0]));
	

	

	xc_restore_zend_arg_info(
		
		
		
		processor, & dst->arg_info[i], & src->arg_info[i]
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->arg_info == NULL);

	
	
	


	}

		
	assert(sizeof(zend_uint) == sizeof(src->num_args));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->required_num_args));
		
	
	
	
	
	
	


	

#	ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_bool) == sizeof(src->pass_rest_by_reference));
		
	
	
	
	
	
	


	

#	endif
#else
	if (src->arg_types) {
		
	
	
		
	
	
	/* allocate */
	
	
	
			dst->arg_types = (zend_uchar *) emalloc(sizeof(zend_uchar) * src->arg_types[0] + 1);
			
		
	
	
	
	

		memcpy(dst->arg_types, src->arg_types, sizeof(src->arg_types[0]) * (src->arg_types[0]+1));
		
		
	
	
	

	}
	else {
		
	
	
	
	assert(src->arg_types == NULL);

	
	
	


	}
#endif
#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(unsigned char) == sizeof(src->return_reference));
		
	
	
	
	
	
	


	

#endif
	/* END of common elements */
#ifdef IS_UNICODE
	#endif

	
	
	if (src->refcount) {
		
		
	
	
	

	
	assert(sizeof(zend_uint) == sizeof(( src->refcount)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
			dst->refcount = (zend_uint *) emalloc(sizeof(zend_uint));
			
		
	
	
	
	


	

	xc_restore_zend_uint(
		
		
		
		processor,  dst->refcount,  src->refcount
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->refcount == NULL);

	}
	
	
	
	


	

#ifdef ZEND_ENGINE_2_4
		
	if (src->literals) {
		int i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
			dst->literals = (zend_literal *) emalloc(sizeof(zend_literal) * src->last_literal);
			
		
	
	
	
	

			

			for (i = 0;
					i < src->last_literal;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_literal) == sizeof(src->literals[i]));
	
	
	
	
	

	
	assert(sizeof(zend_literal) == sizeof((& src->literals[i])[0]));
	

	

	xc_restore_zend_literal(
		
		
		
		processor, & dst->literals[i], & src->literals[i]
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->literals == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->last_literal));
		
	
	
	
	
	
	


	

#endif

		
	if (src->opcodes) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
			dst->opcodes = (zend_op *) emalloc(sizeof(zend_op) * src->last);
			
		
	
	
	
	

			

			for (i = 0;
					i < src->last;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_op) == sizeof(src->opcodes[i]));
	
	
	
	
	

	
	assert(sizeof(zend_op) == sizeof((& src->opcodes[i])[0]));
	

	

	xc_restore_zend_op(
		
		
		
		processor, & dst->opcodes[i], & src->opcodes[i]
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->opcodes == NULL);

	
	
	


	}

		
	assert(sizeof(zend_uint) == sizeof(src->last));
		
	
	
	
	
	
	


	

#ifndef ZEND_ENGINE_2_4
	dst->size = src->last;
	
	
	

#endif

#ifdef IS_CV
	
	if (src->vars) {
		int i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
			dst->vars = (zend_compiled_variable *) emalloc(sizeof(zend_compiled_variable) * src->last_var);
			
		
	
	
	
	

			

			for (i = 0;
					i < src->last_var;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_compiled_variable) == sizeof(src->vars[i]));
	
	
	
	
	

	
	assert(sizeof(zend_compiled_variable) == sizeof((& src->vars[i])[0]));
	

	

	xc_restore_zend_compiled_variable(
		
		
		
		processor, & dst->vars[i], & src->vars[i]
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->vars == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->last_var));
		
	
	
	
	
	
	


	

#	ifndef ZEND_ENGINE_2_4
	dst->size_var = src->last_var;
	
	
	

#	endif
#else
		
#endif

		
	assert(sizeof(zend_uint) == sizeof(src->T));
		
	
	
	
	
	
	


	


#ifdef ZEND_ENGINE_2_5
		
	assert(sizeof(zend_uint) == sizeof(src->nested_calls));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->used_stack));
		
	
	
	
	
	
	


	

#endif

	
	if (src->brk_cont_array) {
		last_brk_cont_t i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
			dst->brk_cont_array = (zend_brk_cont_element *) emalloc(sizeof(zend_brk_cont_element) * src->last_brk_cont);
			
		
	
	
	
	

			

			for (i = 0;
					i < src->last_brk_cont;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_brk_cont_element) == sizeof(src->brk_cont_array[i]));
	
	
	
	
	

	
	assert(sizeof(zend_brk_cont_element) == sizeof((& src->brk_cont_array[i])[0]));
	

	

	xc_restore_zend_brk_cont_element(
		
		
		
		processor, & dst->brk_cont_array[i], & src->brk_cont_array[i]
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->brk_cont_array == NULL);

	
	
	


	}

		
	assert(sizeof(last_brk_cont_t) == sizeof(src->last_brk_cont));
		
	
	
	
	
	
	



	

#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uint) == sizeof(src->current_brk_cont));
		
	
	
	
	
	
	


	

#endif
#ifndef ZEND_ENGINE_2
		
	assert(sizeof(zend_bool) == sizeof(src->uses_globals));
		
	
	
	
	
	
	


	

#endif

#ifdef ZEND_ENGINE_2
	
	if (src->try_catch_array) {
		int i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
			dst->try_catch_array = (zend_try_catch_element *) emalloc(sizeof(zend_try_catch_element) * src->last_try_catch);
			
		
	
	
	
	

			

			for (i = 0;
					i < src->last_try_catch;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_try_catch_element) == sizeof(src->try_catch_array[i]));
	
	
	
	
	

	
	assert(sizeof(zend_try_catch_element) == sizeof((& src->try_catch_array[i])[0]));
	

	

	xc_restore_zend_try_catch_element(
		
		
		
		processor, & dst->try_catch_array[i], & src->try_catch_array[i]
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->try_catch_array == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->last_try_catch));
		
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_5
		
	assert(sizeof(zend_bool) == sizeof(src->has_finally_block));
		
	
	
	
	
	
	


	

#endif

	
	
	if (src->static_variables) {
		
		
	
	
	

	
	assert(sizeof(HashTable) == sizeof(( src->static_variables)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
			ALLOC_HASHTABLE(dst->static_variables);
			
		
	
	
	
	


	

	xc_restore_HashTable_zval_ptr(
		
		
		
		processor,  dst->static_variables,  src->static_variables
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->static_variables == NULL);

	}
	
	
	
	



#ifndef ZEND_ENGINE_2_4
	
	
	
	

		
	assert(sizeof(int) == sizeof(src->backpatch_count));
		
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_3
		
	assert(sizeof(zend_uint) == sizeof(src->this_var));
		
	
	
	
	
	
	


	

#endif

#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_bool) == sizeof(src->done_pass_two));
		
	
	
	
	
	
	


	

#endif
	/* 5.0 <= ver < 5.3 */
#if defined(ZEND_ENGINE_2) && !defined(ZEND_ENGINE_2_3)
		
	assert(sizeof(zend_bool) == sizeof(src->uses_this));
		
	
	
	
	
	
	


	

#endif

	dst->filename = processor->entry_php_src->filepath;
	
	
	

#ifdef IS_UNICODE
	
		
	
	
	

	
#endif
#ifdef ZEND_ENGINE_2
		
	assert(sizeof(zend_uint) == sizeof(src->line_start));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->line_end));
		
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->doc_comment_len));
		
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_U(dst->doc_comment) =  eustrndup (ZSTR_U(src->doc_comment), (src->doc_comment_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->doc_comment) =  estrndup (ZSTR_S(src->doc_comment), (src->doc_comment_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		
		
		
		
			ZSTR_S(dst->doc_comment) =  estrndup (ZSTR_S(src->doc_comment), (src->doc_comment_len + 1) - 1);
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

#endif
#ifdef ZEND_COMPILE_DELAYED_BINDING
		
	assert(sizeof(zend_uint) == sizeof(src->early_binding));
		
	
	
	
	
	
	


	
;
#endif

	/* reserved */
	
	
	
	

#if defined(HARDENING_PATCH) && HARDENING_PATCH
		
	assert(sizeof(zend_bool) == sizeof(src->created_by_eval));
		
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_4
	dst->run_time_cache = NULL;
	
	
	

		
	assert(sizeof(int) == sizeof(src->last_cache_slot));
		
	
	
	
	
	
	


	

#endif
	} while (0);
	xc_fix_op_array_info(processor->entry_php_src, processor->php_src, dst, shallow_copy, op_array_info TSRMLS_CC);

#ifdef ZEND_ENGINE_2
		
		do {
			zend_function *parent;
			if (src->prototype != NULL
			 && zend_u_hash_find(&(processor->active_class_entry_dst->parent->function_table),
					UG(unicode) ? IS_UNICODE : IS_STRING,
					src->function_name, xc_zstrlen(UG(unicode) ? IS_UNICODE : IS_STRING, src->function_name) + 1,
					(void **) &parent) == SUCCESS) {
				/* see do_inherit_method_check() */
				if ((parent->common.fn_flags & ZEND_ACC_ABSTRACT)) {
					dst->prototype = parent;
				} else if (!(parent->common.fn_flags & ZEND_ACC_CTOR) || (parent->common.prototype && (parent->common.prototype->common.scope->ce_flags & ZEND_ACC_INTERFACE))) {
					/* ctors only have a prototype if it comes from an interface */
					dst->prototype = parent->common.prototype ? parent->common.prototype : parent;
				}
				else {
					dst->prototype = NULL;
				}
			}
			else {
				dst->prototype = NULL;
			}
		} while (0);
		
	
	
	
	
	


#endif

#ifdef ZEND_ENGINE_2
	
	if (src->scope) {
		
		dst->scope = xc_get_class(processor,(zend_ulong) src->scope);
#ifdef IS_UNICODE
		
#else
		
#endif
	}
	else {
		
	
	
	assert(src->scope == NULL);

	}

	
	
	

	
		if (src->scope) {
			xc_fix_method(processor, dst TSRMLS_CC);
		}
	
#endif

	
		if (xc_have_op_array_ctor) {
			zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) xc_zend_extension_op_array_ctor_handler, dst TSRMLS_CC);
		}
	
	}
	
		processor->active_op_array_dst = NULL;
		processor->active_op_array_src = NULL;
	
	

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


#ifdef HAVE_XCACHE_CONSTANT

	
	

/* {{{ xc_restore_xc_constinfo_t */
	
 	  	 	static void inline 	xc_restore_xc_constinfo_t( 		 		 		 		xc_processor_t *processor, xc_constinfo_t *dst, const xc_constinfo_t * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(xc_constinfo_t));
			do {
		

		
				
	assert(sizeof(zend_uint) == sizeof(src->key_size));
		
	
	
	
	
	
	


	

#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
	
	
	
	
	


	

#endif
	
	
	
	

		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
	
	
	
	
	


	

	
	
	assert(sizeof(zend_constant) == sizeof(src->constant));
	
	
	
	
	

	
	assert(sizeof(zend_constant) == sizeof((& src->constant)[0]));
	

	

	xc_restore_zend_constant(
		
		
		
		processor, & dst->constant, & src->constant
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	



		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	

#endif


	
	

/* {{{ xc_restore_xc_funcinfo_t */
	
		/* export:  	  	 	void 	xc_restore_xc_funcinfo_t( 		 		 		 		xc_processor_t *processor, xc_funcinfo_t *dst, const xc_funcinfo_t * const src 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_restore_xc_funcinfo_t( 		 		 		 		xc_processor_t *processor, xc_funcinfo_t *dst, const xc_funcinfo_t * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(xc_funcinfo_t));
			do {
		

		
				
	assert(sizeof(zend_uint) == sizeof(src->key_size));
		
	
	
	
	
	
	


	

#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
	
	
	
	
	


	

#endif
	
	
	
	

		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
	
	
	
	
	


	

	
	
	
	

	
		processor->active_op_array_infos_src = &src->op_array_info;
		processor->active_op_array_index = 0;
	
	
	
	assert(sizeof(zend_function) == sizeof(src->func));
	
	
	
	
	

	
	assert(sizeof(zend_function) == sizeof((& src->func)[0]));
	

	

	xc_restore_zend_function(
		
		
		
		processor, & dst->func, & src->func
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	



		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_restore_xc_classinfo_t */
	
		/* export:  	  	 	void 	xc_restore_xc_classinfo_t( 		 		 		 		xc_processor_t *processor, xc_classinfo_t *dst, const xc_classinfo_t * const src 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_restore_xc_classinfo_t( 		 		 		 		xc_processor_t *processor, xc_classinfo_t *dst, const xc_classinfo_t * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(xc_classinfo_t));
			do {
		

		
				
	assert(sizeof(zend_uint) == sizeof(src->key_size));
		
	
	
	
	
	
	


	

#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
	
	
	
	
	


	

#endif
	
	
	
	

		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->methodinfo_cnt));
		
	
	
	
	
	
	


	

	
	
	
	

	
		processor->active_op_array_infos_src = src->methodinfos;
		processor->active_op_array_index = 0;
	
#ifdef ZEND_ENGINE_2
	
	
	if (src->cest) {
		
		
	
	
	

	
	assert(sizeof(zend_class_entry) == sizeof(( src->cest)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
			dst->cest = (zend_class_entry *) emalloc(sizeof(zend_class_entry));
			
		
	
	
	
	


	

	xc_restore_zend_class_entry(
		
		
		
		processor,  dst->cest,  src->cest
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->cest == NULL);

	}
	
	
	
	


#else
	
	
	assert(sizeof(zend_class_entry) == sizeof(src->cest));
	
	
	
	
	

	
	assert(sizeof(zend_class_entry) == sizeof((& src->cest)[0]));
	

	

	xc_restore_zend_class_entry(
		
		
		
		processor, & dst->cest, & src->cest
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


#endif
#ifndef ZEND_COMPILE_DELAYED_BINDING
		
	assert(sizeof(int) == sizeof(src->oplineno));
		
	
	
	
	
	
	


	

#endif

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	




	
	

/* {{{ xc_restore_xc_entry_data_php_t */
	
		/* export:  	  	 	void 	xc_restore_xc_entry_data_php_t( 		 		 		 		xc_processor_t *processor, xc_entry_data_php_t *dst, const xc_entry_data_php_t * const src 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_restore_xc_entry_data_php_t( 		 		 		 		xc_processor_t *processor, xc_entry_data_php_t *dst, const xc_entry_data_php_t * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(xc_entry_data_php_t));
			do {
		

		
			
		processor->php_dst = dst;
		processor->php_src = src;
	

	/* skip */
	
	
	
	

		
	assert(sizeof(xc_hash_value_t) == sizeof(src->hvalue));
		
	
	
	
	
	
	


	

		
	assert(sizeof(xc_md5sum_t) == sizeof(src->md5));
	
	
	
	

	

		
	assert(sizeof(zend_ulong) == sizeof(src->refcount));
		
	
	
	
	
	
	


	


		
	assert(sizeof(zend_ulong) == sizeof(src->hits));
		
	
	
	
	
	
	


	

		
	assert(sizeof(size_t) == sizeof(src->size));
		
	
	
	
	
	
	


	


	
	
	
	

	
		processor->active_op_array_infos_src = &dst->op_array_info;
		processor->active_op_array_index = 0;
	
	
	
	if (src->op_array) {
		
		
	
	
	

	
	assert(sizeof(zend_op_array) == sizeof(( src->op_array)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
			dst->op_array = (zend_op_array *) emalloc(sizeof(zend_op_array));
			
		
	
	
	
	


	

	xc_restore_zend_op_array(
		
		
		
		processor,  dst->op_array,  src->op_array
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		
		
	
	
	assert(src->op_array == NULL);

	}
	
	
	
	



#ifdef HAVE_XCACHE_CONSTANT
		
	assert(sizeof(zend_uint) == sizeof(src->constinfo_cnt));
		
	
	
	
	
	
	


	

	
	if (src->constinfos) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
			dst->constinfos = (xc_constinfo_t *) emalloc(sizeof(xc_constinfo_t) * src->constinfo_cnt);
			
		
	
	
	
	

			

			for (i = 0;
					i < src->constinfo_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_constinfo_t) == sizeof(src->constinfos[i]));
	
	
	
	
	

	
	assert(sizeof(xc_constinfo_t) == sizeof((& src->constinfos[i])[0]));
	

	

	xc_restore_xc_constinfo_t(
		
		
		
		processor, & dst->constinfos[i], & src->constinfos[i]
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->constinfos == NULL);

	
	
	


	}

#endif

		
	assert(sizeof(zend_uint) == sizeof(src->funcinfo_cnt));
		
	
	
	
	
	
	


	

	
	if (src->funcinfos) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
			dst->funcinfos = (xc_funcinfo_t *) emalloc(sizeof(xc_funcinfo_t) * src->funcinfo_cnt);
			
		
	
	
	
	

			

			for (i = 0;
					i < src->funcinfo_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_funcinfo_t) == sizeof(src->funcinfos[i]));
	
	
	
	
	

	
	assert(sizeof(xc_funcinfo_t) == sizeof((& src->funcinfos[i])[0]));
	

	

	xc_restore_xc_funcinfo_t(
		
		
		
		processor, & dst->funcinfos[i], & src->funcinfos[i]
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->funcinfos == NULL);

	
	
	


	}


		
	assert(sizeof(zend_uint) == sizeof(src->classinfo_cnt));
		
	
	
	
	
	
	


	

	
	if (src->classinfos) {
		
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
			dst->classinfos = (xc_classinfo_t *) emalloc(sizeof(xc_classinfo_t) * src->classinfo_cnt);
			
		
	
	
	
	

			

			for (processor->active_class_index = 0;
					processor->active_class_index < src->classinfo_cnt;
					++processor->active_class_index) {
				
	
	

					
	
	assert(sizeof(xc_classinfo_t) == sizeof(src->classinfos[processor->active_class_index]));
	
	
	
	
	

	
	assert(sizeof(xc_classinfo_t) == sizeof((& src->classinfos[processor->active_class_index])[0]));
	

	

	xc_restore_xc_classinfo_t(
		
		
		
		processor, & dst->classinfos[processor->active_class_index], & src->classinfos[processor->active_class_index]
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->classinfos == NULL);

	
	
	


	}

#ifdef ZEND_ENGINE_2_1
		
	assert(sizeof(zend_uint) == sizeof(src->autoglobal_cnt));
		
	
	
	
	
	
	


	

	
		
	
	
	

	
#endif
#ifdef XCACHE_ERROR_CACHING
		
	assert(sizeof(zend_uint) == sizeof(src->compilererror_cnt));
		
	
	
	
	
	
	


	

	
		
	
	
	

	
#endif
#ifndef ZEND_COMPILE_DELAYED_BINDING
		
	assert(sizeof(zend_bool) == sizeof(src->have_early_binding));
		
	
	
	
	
	
	


	

#endif
		
	assert(sizeof(zend_bool) == sizeof(src->have_references));
		
	
	
	
	
	
	


	


		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_restore_xc_entry_t */
	
 	  	 	static void inline 	xc_restore_xc_entry_t( 		 		 		 		xc_processor_t *processor, xc_entry_t *dst, const xc_entry_t * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(xc_entry_t));
			do {
		

		
			/* skip */
	
	
	
	

		
	assert(sizeof(size_t) == sizeof(src->size));
		
	
	
	
	
	
	


	


		
	assert(sizeof(time_t) == sizeof(src->ctime));
		
	
	
	
	
	
	


	

		
	assert(sizeof(time_t) == sizeof(src->atime));
		
	
	
	
	
	
	


	

		
	assert(sizeof(time_t) == sizeof(src->dtime));
		
	
	
	
	
	
	


	

		
	assert(sizeof(long) == sizeof(src->ttl));
		
	
	
	
	
	
	


	

		
	assert(sizeof(zend_ulong) == sizeof(src->hits));
		
	
	
	
	
	
	


	

	
	
	
	
 
		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_restore_xc_entry_php_t */
	
		/* export:  	  	 	void 	xc_restore_xc_entry_php_t( 		 		 		 		xc_processor_t *processor, xc_entry_php_t *dst, const xc_entry_php_t * const src 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_restore_xc_entry_php_t( 		 		 		 		xc_processor_t *processor, xc_entry_php_t *dst, const xc_entry_php_t * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(xc_entry_php_t));
			do {
		

		
			
	
	assert(sizeof(xc_entry_t) == sizeof(src->entry));
	
	
	
	
	

	
	assert(sizeof(xc_entry_t) == sizeof((& src->entry)[0]));
	

	

	xc_restore_xc_entry_t(
		
		
		
		processor, & dst->entry, & src->entry
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
	

			
	assert(sizeof(int) == sizeof(src->entry.name.str.len));
		
	
	
	

	

		
	
	
	


	
	
	
	


		
	assert(sizeof(long) == sizeof(src->refcount));
		
	
	
	
	
	
	


	

		
	assert(sizeof(time_t) == sizeof(src->file_mtime));
		
	
	
	
	
	
	


	

		
	assert(sizeof(size_t) == sizeof(src->file_size));
		
	
	
	
	
	
	


	

		
	assert(sizeof(size_t) == sizeof(src->file_device));
		
	
	
	
	
	
	


	

		
	assert(sizeof(size_t) == sizeof(src->file_inode));
		
	
	
	
	
	
	


	


		
	assert(sizeof(size_t) == sizeof(src->filepath_len));
		
	
	
	
	
	
	


	

	
	
	
	

		
	assert(sizeof(size_t) == sizeof(src->dirpath_len));
		
	
	
	
	
	
	


	

	
	
	
	

#ifdef IS_UNICODE
		
	assert(sizeof(int) == sizeof(src->ufilepath_len));
		
	
	
	
	
	
	


	

	
	
	
	

		
	assert(sizeof(int) == sizeof(src->udirpath_len));
		
	
	
	
	
	
	


	

	
	
	
	

#endif

		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_restore_xc_entry_var_t */
	
		/* export:  	  	 	void 	xc_restore_xc_entry_var_t( 		 		 		 		xc_processor_t *processor, xc_entry_var_t *dst, const xc_entry_var_t * const src 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_restore_xc_entry_var_t( 		 		 		 		xc_processor_t *processor, xc_entry_var_t *dst, const xc_entry_var_t * const src 		 		TSRMLS_DC 	) 	
	{
		
		
		

		
			memcpy(dst, src, sizeof(xc_entry_var_t));
			do {
		

		
			
	
	assert(sizeof(xc_entry_t) == sizeof(src->entry));
	
	
	
	
	

	
	assert(sizeof(xc_entry_t) == sizeof((& src->entry)[0]));
	

	

	xc_restore_xc_entry_t(
		
		
		
		processor, & dst->entry, & src->entry
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	



#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->name_type));
		
	
	
	
	
	
	


	

#endif
		
	
	

#ifdef IS_UNICODE
		if (src->name_type == IS_UNICODE) {
				
	assert(sizeof(int32_t) == sizeof(src->entry.name.ustr.len));
		
	
	
	

	

		}
		else {
				
	assert(sizeof(int) == sizeof(src->entry.name.str.len));
		
	
	
	

	

		}
#else
			
	assert(sizeof(int) == sizeof(src->entry.name.str.len));
		
	
	
	

	

#endif
		
	
	
	

	
	
	
	
	
	

	
	assert(sizeof(zval_ptr) == sizeof((& src->value)[0]));
	

	

	xc_restore_zval_ptr(
		
		
		
		processor, & dst->value, & src->value
		
		TSRMLS_CC
	);

	

	
	

		
	assert(sizeof(zend_bool) == sizeof(src->have_references));
		
	
	
	
	
	
	


	

	
	
	
	


		
		
		
			
			
			
		
		
			} while (0);
		
		
		
	}
/* }}} */
	




 
#ifdef HAVE_XCACHE_DPRINT
  /* {{{ Pre-declare */
 	  	 	void 	xc_dprint_zval( 		const zval * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	
 	  	 	static void inline 	xc_dprint_zval_ptr( 		const zval_ptr * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	
 	  	 	static void inline 	xc_dprint_zval_ptr_nullable( 		const zval_ptr_nullable * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	
 	  	 	void 	xc_dprint_zend_op_array( 		const zend_op_array * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	
 	  	 	void 	xc_dprint_zend_class_entry( 		const zend_class_entry * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	
#ifdef HAVE_XCACHE_CONSTANT
 	  	 	static void inline 	xc_dprint_zend_constant( 		const zend_constant * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	
#endif
 	  	 	void 	xc_dprint_zend_function( 		const zend_function * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	
 	  	 	void 	xc_dprint_xc_entry_var_t( 		const xc_entry_var_t * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	
 	  	 	void 	xc_dprint_xc_entry_php_t( 		const xc_entry_php_t * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	
#ifdef ZEND_ENGINE_2
 	  	 	static void inline 	xc_dprint_zend_property_info( 		const zend_property_info * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	
#endif
/* }}} */
#ifdef IS_CV

	
	

/* {{{ xc_dprint_zend_compiled_variable */
	
 	  	 	static void inline 	xc_dprint_zend_compiled_variable( 		const zend_compiled_variable * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
				
	assert(sizeof(int) == sizeof(src->name_len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:name_len:\t%d\n", src->name_len);
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
#ifdef IS_UNICODE
			do {
				zval zv;
				zval reszv;
				int usecopy;

				INIT_ZVAL(zv);
				ZVAL_UNICODEL(&zv, ZSTR_U(src->name), src->name_len + 1 - 1, 1);
				zend_make_printable_zval(&zv, &reszv, &usecopy);
				fprintf(stderr, "string:%s:\t\"", "dst->name");
				xc_dprint_str_len(Z_STRVAL(reszv), Z_STRLEN(reszv));
				fprintf(stderr, "\" len=%lu\n", (unsigned long) src->name_len + 1 - 1);
				if (usecopy) {
					zval_dtor(&reszv);
				}
				zval_dtor(&zv);
			} while (0);
#endif
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->name");
			xc_dprint_str_len(ZSTR_S(src->name), src->name_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->name_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->name");
			xc_dprint_str_len(ZSTR_S(src->name), src->name_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->name_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

		
	assert(sizeof(ulong) == sizeof(src->hash_value));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "ulong:hash_value:\t%lu\n", src->hash_value);
	
	
	
	
	
	


	


		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_dprint_zend_uint */
	
 	  	 	static void inline 	xc_dprint_zend_uint( 		const zend_uint * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
			
	
		xc_dprint_indent(indent);
		fprintf(stderr, "%u\n", src[0]);
	
	

		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
		
		
		
	}
/* }}} */
	

#ifndef ZEND_ENGINE_2

	
	

/* {{{ xc_dprint_int */
	
 	  	 	static void inline 	xc_dprint_int( 		const int * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
			
	
		xc_dprint_indent(indent);
		fprintf(stderr, "%d\n", src[0]);
	
	

		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
		
		
		
	}
/* }}} */
	

#endif
#ifdef ZEND_ENGINE_2

	
	

/* {{{ xc_dprint_zend_try_catch_element */
	
 	  	 	static void inline 	xc_dprint_zend_try_catch_element( 		const zend_try_catch_element * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
				
	assert(sizeof(zend_uint) == sizeof(src->try_op));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:try_op:\t%u\n", src->try_op);
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->catch_op));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:catch_op:\t%u\n", src->catch_op);
	
	
	
	
	
	


	

#ifdef ZEND_ENGINE_2_5
		
	assert(sizeof(zend_uint) == sizeof(src->finally_op));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:finally_op:\t%u\n", src->finally_op);
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->finally_end));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:finally_end:\t%u\n", src->finally_end);
	
	
	
	
	
	


	

#endif

		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_dprint_zend_brk_cont_element */
	
 	  	 	static void inline 	xc_dprint_zend_brk_cont_element( 		const zend_brk_cont_element * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
		#ifdef ZEND_ENGINE_2_2
		
	assert(sizeof(int) == sizeof(src->start));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:start:\t%d\n", src->start);
	
	
	
	
	
	


	

#endif
		
	assert(sizeof(int) == sizeof(src->cont));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:cont:\t%d\n", src->cont);
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->brk));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:brk:\t%d\n", src->brk);
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->parent));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:parent:\t%d\n", src->parent);
	
	
	
	
	
	


	


		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	
	

/* {{{ xc_dprint_HashTable_zval_ptr */
	
 	  	 	static void inline 	xc_dprint_HashTable_zval_ptr( 		const HashTable * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
		
		
	

				
				Bucket *srcBucket;
		
		zend_bool first = 1;
				
		

#if defined(HARDENING_PATCH_HASH_PROTECT) && HARDENING_PATCH_HASH_PROTECT
		
							
	assert(sizeof(unsigned int) == sizeof(src->canary));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "unsigned int:canary:\t%u\n", src->canary);
	
	
	
	
	
	


	

		
#endif
			
	assert(sizeof(uint) == sizeof(src->nTableSize));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "uint:nTableSize:\t%u\n", src->nTableSize);
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nTableMask));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "uint:nTableMask:\t%u\n", src->nTableMask);
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nNumOfElements));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "uint:nNumOfElements:\t%u\n", src->nNumOfElements);
	
	
	
	
	
	


	

			
	assert(sizeof(ulong) == sizeof(src->nNextFreeElement));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "ulong:nNextFreeElement:\t%lu\n", src->nNextFreeElement);
	
	
	
	
	
	


	

		 
	
	
	

		 
	
	
	

#ifdef ZEND_ENGINE_2_4
	if (src->nTableMask) {
#endif
		
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

		
	
	
	

		
	
	

		for (srcBucket = src->pListHead; srcBucket != NULL; srcBucket = srcBucket->pListNext) {
			

			
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			
			
				xc_dprint_indent(indent);
				fprintf(stderr, "zval_ptr:\"");
				xc_dprint_str_len(BUCKET_KEY_S(srcBucket), BUCKET_KEY_SIZE(srcBucket));
				fprintf(stderr, "\" %d:h=%lu ", BUCKET_KEY_SIZE(srcBucket), srcBucket->h);
			
			if (sizeof(void *) == sizeof(zval_ptr)) {
				
								
	
	
	

	
	assert(sizeof(zval_ptr) == sizeof((  ((zval_ptr*)srcBucket->pData))[0]));
	

	

	xc_dprint_zval_ptr(
		             ((zval_ptr*)srcBucket->pData), indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

			}
			else {
				
	
	
	

	
	assert(sizeof(zval_ptr) == sizeof(( ((zval_ptr*)srcBucket->pData))[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	

	xc_dprint_zval_ptr(
		            ((zval_ptr*)srcBucket->pData), indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

				
			}

			if (first) {
				
				first = 0;
			}

			
		}
		
	
	

				
#ifdef ZEND_ENGINE_2_4
	}
	else { /* if (src->nTableMask) */
		
	
	
	

	}
#endif
		 
	
	
	

		 
	
	
	

			
	assert(sizeof(zend_bool) == sizeof(src->persistent));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:persistent:\t%u\n", src->persistent);
	
	
	
	
	
	


	

#ifdef IS_UNICODE
			
	assert(sizeof(zend_bool) == sizeof(src->unicode));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:unicode:\t%u\n", src->unicode);
	
	
	
	
	
	


	

#endif
			
	assert(sizeof(unsigned char) == sizeof(src->nApplyCount));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "unsigned char:nApplyCount:\t%u\n", src->nApplyCount);
	
	
	
	
	
	


	

			
	assert(sizeof(zend_bool) == sizeof(src->bApplyProtection));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:bApplyProtection:\t%u\n", src->bApplyProtection);
	
	
	
	
	
	


	

#if ZEND_DEBUG
			
	assert(sizeof(int) == sizeof(src->inconsistent));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:inconsistent:\t%d\n", src->inconsistent);
	
	
	
	
	
	


	

#endif
				
	
		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	



	
	
	

/* {{{ xc_dprint_HashTable_zend_function */
	
		/* export:  	  	 	void 	xc_dprint_HashTable_zend_function( 		const HashTable * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dprint_HashTable_zend_function( 		const HashTable * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
		
		
	

				
				Bucket *srcBucket;
		
		zend_bool first = 1;
				
		

#if defined(HARDENING_PATCH_HASH_PROTECT) && HARDENING_PATCH_HASH_PROTECT
		
							
	assert(sizeof(unsigned int) == sizeof(src->canary));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "unsigned int:canary:\t%u\n", src->canary);
	
	
	
	
	
	


	

		
#endif
			
	assert(sizeof(uint) == sizeof(src->nTableSize));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "uint:nTableSize:\t%u\n", src->nTableSize);
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nTableMask));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "uint:nTableMask:\t%u\n", src->nTableMask);
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nNumOfElements));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "uint:nNumOfElements:\t%u\n", src->nNumOfElements);
	
	
	
	
	
	


	

			
	assert(sizeof(ulong) == sizeof(src->nNextFreeElement));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "ulong:nNextFreeElement:\t%lu\n", src->nNextFreeElement);
	
	
	
	
	
	


	

		 
	
	
	

		 
	
	
	

#ifdef ZEND_ENGINE_2_4
	if (src->nTableMask) {
#endif
		
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

		
	
	
	

		
	
	

		for (srcBucket = src->pListHead; srcBucket != NULL; srcBucket = srcBucket->pListNext) {
			

			
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			
			
				xc_dprint_indent(indent);
				fprintf(stderr, "zend_function:\"");
				xc_dprint_str_len(BUCKET_KEY_S(srcBucket), BUCKET_KEY_SIZE(srcBucket));
				fprintf(stderr, "\" %d:h=%lu ", BUCKET_KEY_SIZE(srcBucket), srcBucket->h);
			
			if (sizeof(void *) == sizeof(zend_function)) {
				
								
	
	
	

	
	assert(sizeof(zend_function) == sizeof((  ((zend_function*)srcBucket->pData))[0]));
	

	

	xc_dprint_zend_function(
		             ((zend_function*)srcBucket->pData), indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

			}
			else {
				
	
	
	

	
	assert(sizeof(zend_function) == sizeof(( ((zend_function*)srcBucket->pData))[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	

	xc_dprint_zend_function(
		            ((zend_function*)srcBucket->pData), indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

				
			}

			if (first) {
				
				first = 0;
			}

			
		}
		
	
	

				
#ifdef ZEND_ENGINE_2_4
	}
	else { /* if (src->nTableMask) */
		
	
	
	

	}
#endif
		 
	
	
	

		 
	
	
	

			
	assert(sizeof(zend_bool) == sizeof(src->persistent));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:persistent:\t%u\n", src->persistent);
	
	
	
	
	
	


	

#ifdef IS_UNICODE
			
	assert(sizeof(zend_bool) == sizeof(src->unicode));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:unicode:\t%u\n", src->unicode);
	
	
	
	
	
	


	

#endif
			
	assert(sizeof(unsigned char) == sizeof(src->nApplyCount));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "unsigned char:nApplyCount:\t%u\n", src->nApplyCount);
	
	
	
	
	
	


	

			
	assert(sizeof(zend_bool) == sizeof(src->bApplyProtection));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:bApplyProtection:\t%u\n", src->bApplyProtection);
	
	
	
	
	
	


	

#if ZEND_DEBUG
			
	assert(sizeof(int) == sizeof(src->inconsistent));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:inconsistent:\t%d\n", src->inconsistent);
	
	
	
	
	
	


	

#endif
				
	
		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


#ifdef ZEND_ENGINE_2

	
	
	

/* {{{ xc_dprint_HashTable_zend_property_info */
	
 	  	 	static void inline 	xc_dprint_HashTable_zend_property_info( 		const HashTable * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
		
		
	

				
				Bucket *srcBucket;
		
		zend_bool first = 1;
				
		

#if defined(HARDENING_PATCH_HASH_PROTECT) && HARDENING_PATCH_HASH_PROTECT
		
							
	assert(sizeof(unsigned int) == sizeof(src->canary));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "unsigned int:canary:\t%u\n", src->canary);
	
	
	
	
	
	


	

		
#endif
			
	assert(sizeof(uint) == sizeof(src->nTableSize));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "uint:nTableSize:\t%u\n", src->nTableSize);
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nTableMask));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "uint:nTableMask:\t%u\n", src->nTableMask);
	
	
	
	
	
	


	

			
	assert(sizeof(uint) == sizeof(src->nNumOfElements));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "uint:nNumOfElements:\t%u\n", src->nNumOfElements);
	
	
	
	
	
	


	

			
	assert(sizeof(ulong) == sizeof(src->nNextFreeElement));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "ulong:nNextFreeElement:\t%lu\n", src->nNextFreeElement);
	
	
	
	
	
	


	

		 
	
	
	

		 
	
	
	

#ifdef ZEND_ENGINE_2_4
	if (src->nTableMask) {
#endif
		
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

		
	
	
	

		
	
	

		for (srcBucket = src->pListHead; srcBucket != NULL; srcBucket = srcBucket->pListNext) {
			

			
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			
			
				xc_dprint_indent(indent);
				fprintf(stderr, "zend_property_info:\"");
				xc_dprint_str_len(BUCKET_KEY_S(srcBucket), BUCKET_KEY_SIZE(srcBucket));
				fprintf(stderr, "\" %d:h=%lu ", BUCKET_KEY_SIZE(srcBucket), srcBucket->h);
			
			if (sizeof(void *) == sizeof(zend_property_info)) {
				
								
	
	
	

	
	assert(sizeof(zend_property_info) == sizeof((  ((zend_property_info*)srcBucket->pData))[0]));
	

	

	xc_dprint_zend_property_info(
		             ((zend_property_info*)srcBucket->pData), indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

			}
			else {
				
	
	
	

	
	assert(sizeof(zend_property_info) == sizeof(( ((zend_property_info*)srcBucket->pData))[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	

	xc_dprint_zend_property_info(
		            ((zend_property_info*)srcBucket->pData), indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

				
			}

			if (first) {
				
				first = 0;
			}

			
		}
		
	
	

				
#ifdef ZEND_ENGINE_2_4
	}
	else { /* if (src->nTableMask) */
		
	
	
	

	}
#endif
		 
	
	
	

		 
	
	
	

			
	assert(sizeof(zend_bool) == sizeof(src->persistent));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:persistent:\t%u\n", src->persistent);
	
	
	
	
	
	


	

#ifdef IS_UNICODE
			
	assert(sizeof(zend_bool) == sizeof(src->unicode));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:unicode:\t%u\n", src->unicode);
	
	
	
	
	
	


	

#endif
			
	assert(sizeof(unsigned char) == sizeof(src->nApplyCount));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "unsigned char:nApplyCount:\t%u\n", src->nApplyCount);
	
	
	
	
	
	


	

			
	assert(sizeof(zend_bool) == sizeof(src->bApplyProtection));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:bApplyProtection:\t%u\n", src->bApplyProtection);
	
	
	
	
	
	


	

#if ZEND_DEBUG
			
	assert(sizeof(int) == sizeof(src->inconsistent));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:inconsistent:\t%d\n", src->inconsistent);
	
	
	
	
	
	


	

#endif
				
	
		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


#endif
#ifdef IS_CONSTANT_AST


	
	

/* {{{ xc_dprint_zend_ast */
	
		/* export:  	  	 	void 	xc_dprint_zend_ast( 		const zend_ast * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dprint_zend_ast( 		const zend_ast * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
				zend_ushort i;
			
	assert(sizeof(zend_ushort) == sizeof(src->kind));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_ushort:kind:\t%hu\n", src->kind);
	
	
	
	
	
	


	

			
	assert(sizeof(zend_ushort) == sizeof(src->children));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_ushort:children:\t%hu\n", src->children);
	
	
	
	
	
	


	

		
	
	
	

		
	
	

			if (src->kind == ZEND_CONST) {
				assert(src->u.val);
				
				
	
	
	

	
	assert(sizeof(zval) == sizeof((  src->u.val)[0]));
	

	

	xc_dprint_zval(
		             src->u.val, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

				
			}
			else {
				for (i = 0; i < src->children; ++i) {
					zend_ast *src_ast = (&src->u.child)[i];
					if (src_ast) {
						{
	

	
	
							
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

							
	
	
	

	
	assert(sizeof(zend_ast) == sizeof((  src_ast)[0]));
	

	

	xc_dprint_zend_ast(
		             src_ast, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

						
	
}

						
					}
					else {
						
					}
				}
			}
		
	
	


		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_dprint_zval */
	
		/* export:  	  	 	void 	xc_dprint_zval( 		const zval * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dprint_zval( 		const zval * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
			
				/* Variable information */
		
	
	

		switch ((Z_TYPE_P(src) & IS_CONSTANT_TYPE_MASK)) {
			case IS_LONG:
			case IS_RESOURCE:
			case IS_BOOL:
					
	assert(sizeof(long) == sizeof(src->value.lval));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "long:value.lval:\t%ld\n", src->value.lval);
	
	
	

	

				break;
			case IS_DOUBLE:
					
	assert(sizeof(double) == sizeof(src->value.dval));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "double:value.dval:\t%f\n", src->value.dval);
	
	
	

	

				break;
			case IS_NULL:
				xc_dprint_indent(indent);fprintf(stderr, "\tNULL\n");
				break;

			case IS_CONSTANT:
#ifdef IS_UNICODE
				if (UG(unicode)) {
					goto proc_unicode;
				}
#endif
			case IS_STRING:
#ifdef FLAG_IS_BC
			case FLAG_IS_BC:
#endif
					
	assert(sizeof(int) == sizeof(src->value.str.len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:value.str.len:\t%d\n", src->value.str.len);
	
	
	

	

				  
	
	
	
	
	
	
	
	
	if (src->value.str.val == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->value.str.val");
			xc_dprint_str_len(src->value.str.val, src->value.str.len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->value.str.len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

				break;
#ifdef IS_UNICODE
			case IS_UNICODE:
proc_unicode:
					
	assert(sizeof(int32_t) == sizeof(src->value.uni.len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int32_t:value.uni.len:\t%d\n", src->value.uni.len);
	
	
	

	

				 
	
#ifdef IS_UNICODE
	
	
	
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->value.uni.val) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
#ifdef IS_UNICODE
			do {
				zval zv;
				zval reszv;
				int usecopy;

				INIT_ZVAL(zv);
				ZVAL_UNICODEL(&zv, ZSTR_U(src->value.uni.val), src->value.uni.len + 1 - 1, 1);
				zend_make_printable_zval(&zv, &reszv, &usecopy);
				fprintf(stderr, "string:%s:\t\"", "dst->value.uni.val");
				xc_dprint_str_len(Z_STRVAL(reszv), Z_STRLEN(reszv));
				fprintf(stderr, "\" len=%lu\n", (unsigned long) src->value.uni.len + 1 - 1);
				if (usecopy) {
					zval_dtor(&reszv);
				}
				zval_dtor(&zv);
			} while (0);
#endif
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

	
#else
	
	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->value.uni.val) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->value.uni.val");
			xc_dprint_str_len(ZSTR_S(src->value.uni.val), src->value.uni.len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->value.uni.len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

				break;
#endif

			case IS_ARRAY:
#ifdef IS_CONSTANT_ARRAY
			case IS_CONSTANT_ARRAY:
#endif
				assert(src->value.ht);
				
	
	if (src->value.ht) {
		xc_dprint_indent(indent);fprintf(stderr, "HashTable:value.ht ");
		
	
	
	

	
	assert(sizeof(HashTable) == sizeof(( src->value.ht)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	

	xc_dprint_HashTable_zval_ptr(
		            src->value.ht, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		xc_dprint_indent(indent);fprintf(stderr, "HashTable:value.ht:\tNULL\n");
		
	
	
	assert(src->value.ht == NULL);

	}
	

				break;

#ifdef IS_CONSTANT_AST
			case IS_CONSTANT_AST:
				assert(src->value.ast);
				{
	

	
	
	
	if (src->value.ast) {
		xc_dprint_indent(indent);fprintf(stderr, "zend_ast:value.ast ");
		
	
	
	

	
	assert(sizeof(zend_ast) == sizeof(( src->value.ast)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	

	xc_dprint_zend_ast(
		            src->value.ast, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		xc_dprint_indent(indent);fprintf(stderr, "zend_ast:value.ast:\tNULL\n");
		
	
	
	assert(src->value.ast == NULL);

	}
	

	
}

				break;
#endif

			case IS_OBJECT:
				
				#ifndef ZEND_ENGINE_2
				
	
	if (src->value.obj.ce) {
		xc_dprint_indent(indent);fprintf(stderr, "zend_class_entry:value.obj.ce ");
		
	
	
	

	
	assert(sizeof(zend_class_entry) == sizeof(( src->value.obj.ce)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	

	xc_dprint_zend_class_entry(
		            src->value.obj.ce, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		xc_dprint_indent(indent);fprintf(stderr, "zend_class_entry:value.obj.ce:\tNULL\n");
		
	
	
	assert(src->value.obj.ce == NULL);

	}
	

				
	
	if (src->value.obj.properties) {
		xc_dprint_indent(indent);fprintf(stderr, "HashTable:value.obj.properties ");
		
	
	
	

	
	assert(sizeof(HashTable) == sizeof(( src->value.obj.properties)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	

	xc_dprint_HashTable_zval_ptr(
		            src->value.obj.properties, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		xc_dprint_indent(indent);fprintf(stderr, "HashTable:value.obj.properties:\tNULL\n");
		
	
	
	assert(src->value.obj.properties == NULL);

	}
	

#endif
				break;

			default:
				assert(0);
		}
		
	
	

		
	
	
	

			
	assert(sizeof(xc_zval_type_t) == sizeof(src->type));
		
		xc_dprint_indent(indent);
		fprintf(stderr, ":type:\t%d %s\n", src->type, xc_get_data_type(src->type));
		
	
	
	

	

	

#ifdef ZEND_ENGINE_2_3
			
	assert(sizeof(zend_uchar) == sizeof(src->is_ref__gc));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uchar:is_ref__gc:\t%u\n", src->is_ref__gc);
	
	
	
	
	
	


	

#else
			
	assert(sizeof(zend_uchar) == sizeof(src->is_ref));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uchar:is_ref:\t%u\n", src->is_ref);
	
	
	
	
	
	


	

#endif

#ifdef ZEND_ENGINE_2_3
			
	assert(sizeof(zend_uint) == sizeof(src->refcount__gc));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:refcount__gc:\t%u\n", src->refcount__gc);
	
	
	
	
	
	


	

#elif defined(ZEND_ENGINE_2)
			
	assert(sizeof(zend_uint) == sizeof(src->refcount));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:refcount:\t%u\n", src->refcount);
	
	
	
	
	
	


	

#else
			
	assert(sizeof(zend_ushort) == sizeof(src->refcount));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_ushort:refcount:\t%hu\n", src->refcount);
	
	
	
	
	
	


	

#endif
	
		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dprint_zval_ptr */
	
 	  	 	static void inline 	xc_dprint_zval_ptr( 		const zval_ptr * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
			
		do {
			
			
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			
			
			xc_dprint_indent(indent);fprintf(stderr, "[%p] ", (void *) src[0]);
			
	
	
	

	
	assert(sizeof(zval) == sizeof((  src[0])[0]));
	

	

	xc_dprint_zval(
		             src[0], indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

			
		} while (0);
	
	

		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dprint_zval_ptr_nullable */
	
 	  	 	static void inline 	xc_dprint_zval_ptr_nullable( 		const zval_ptr_nullable * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
			if (src[0]) {
		
		
	
	
	

	
	assert(sizeof(zval_ptr) == sizeof((  src)[0]));
	

	xc_dprint_zval_ptr(
		             src, indent
		
		
		
		
		TSRMLS_CC
	);

	
	

		
	}
	else {
		
	}
	

		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
		
		
		
	}
/* }}} */
	

#ifdef ZEND_ENGINE_2

	
	

/* {{{ xc_dprint_zend_arg_info */
	
 	  	 	static void inline 	xc_dprint_zend_arg_info( 		const zend_arg_info * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
				
	assert(sizeof(zend_uint) == sizeof(src->name_len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:name_len:\t%u\n", src->name_len);
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
#ifdef IS_UNICODE
			do {
				zval zv;
				zval reszv;
				int usecopy;

				INIT_ZVAL(zv);
				ZVAL_UNICODEL(&zv, ZSTR_U(src->name), src->name_len + 1 - 1, 1);
				zend_make_printable_zval(&zv, &reszv, &usecopy);
				fprintf(stderr, "string:%s:\t\"", "dst->name");
				xc_dprint_str_len(Z_STRVAL(reszv), Z_STRLEN(reszv));
				fprintf(stderr, "\" len=%lu\n", (unsigned long) src->name_len + 1 - 1);
				if (usecopy) {
					zval_dtor(&reszv);
				}
				zval_dtor(&zv);
			} while (0);
#endif
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->name");
			xc_dprint_str_len(ZSTR_S(src->name), src->name_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->name_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->name");
			xc_dprint_str_len(ZSTR_S(src->name), src->name_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->name_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

		
	assert(sizeof(zend_uint) == sizeof(src->class_name_len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:class_name_len:\t%u\n", src->class_name_len);
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->class_name) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
#ifdef IS_UNICODE
			do {
				zval zv;
				zval reszv;
				int usecopy;

				INIT_ZVAL(zv);
				ZVAL_UNICODEL(&zv, ZSTR_U(src->class_name), src->class_name_len + 1 - 1, 1);
				zend_make_printable_zval(&zv, &reszv, &usecopy);
				fprintf(stderr, "string:%s:\t\"", "dst->class_name");
				xc_dprint_str_len(Z_STRVAL(reszv), Z_STRLEN(reszv));
				fprintf(stderr, "\" len=%lu\n", (unsigned long) src->class_name_len + 1 - 1);
				if (usecopy) {
					zval_dtor(&reszv);
				}
				zval_dtor(&zv);
			} while (0);
#endif
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->class_name) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->class_name");
			xc_dprint_str_len(ZSTR_S(src->class_name), src->class_name_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->class_name_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->class_name) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->class_name");
			xc_dprint_str_len(ZSTR_S(src->class_name), src->class_name_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->class_name_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

#ifdef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uchar) == sizeof(src->type_hint));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uchar:type_hint:\t%u\n", src->type_hint);
	
	
	
	
	
	


	

#elif defined(ZEND_ENGINE_2_1)
		
	assert(sizeof(zend_bool) == sizeof(src->array_type_hint));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:array_type_hint:\t%u\n", src->array_type_hint);
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_6
		
	assert(sizeof(zend_uchar) == sizeof(src->pass_by_reference));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uchar:pass_by_reference:\t%u\n", src->pass_by_reference);
	
	
	
	
	
	


	

#endif
		
	assert(sizeof(zend_bool) == sizeof(src->allow_null));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:allow_null:\t%u\n", src->allow_null);
	
	
	
	
	
	


	


#ifdef ZEND_ENGINE_2_6
		
	assert(sizeof(zend_bool) == sizeof(src->is_variadic));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:is_variadic:\t%u\n", src->is_variadic);
	
	
	
	
	
	


	

#else
		
	assert(sizeof(zend_bool) == sizeof(src->pass_by_reference));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:pass_by_reference:\t%u\n", src->pass_by_reference);
	
	
	
	
	
	


	

#endif

#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_bool) == sizeof(src->return_reference));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:return_reference:\t%u\n", src->return_reference);
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->required_num_args));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:required_num_args:\t%d\n", src->required_num_args);
	
	
	
	
	
	


	

#endif

		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif
#ifdef HAVE_XCACHE_CONSTANT

	
	

/* {{{ xc_dprint_zend_constant */
	
 	  	 	static void inline 	xc_dprint_zend_constant( 		const zend_constant * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
			
	
	assert(sizeof(zval) == sizeof(src->value));
	xc_dprint_indent(indent);fprintf(stderr, "zval:value ");
	
	
	
	

	
	assert(sizeof(zval) == sizeof((& src->value)[0]));
	

	

	xc_dprint_zval(
		           & src->value, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


		
	assert(sizeof(int) == sizeof(src->flags));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:flags:\t%d\n", src->flags);
	
	
	
	
	
	


	

		
	assert(sizeof(uint) == sizeof(src->name_len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "uint:name_len:\t%u\n", src->name_len);
	
	
	
	
	
	


	

	
	
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
#ifdef IS_UNICODE
			do {
				zval zv;
				zval reszv;
				int usecopy;

				INIT_ZVAL(zv);
				ZVAL_UNICODEL(&zv, ZSTR_U(src->name), src->name_len - 1, 1);
				zend_make_printable_zval(&zv, &reszv, &usecopy);
				fprintf(stderr, "string:%s:\t\"", "dst->name");
				xc_dprint_str_len(Z_STRVAL(reszv), Z_STRLEN(reszv));
				fprintf(stderr, "\" len=%lu\n", (unsigned long) src->name_len - 1);
				if (usecopy) {
					zval_dtor(&reszv);
				}
				zval_dtor(&zv);
			} while (0);
#endif
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->name");
			xc_dprint_str_len(ZSTR_S(src->name), src->name_len - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->name_len - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->name");
			xc_dprint_str_len(ZSTR_S(src->name), src->name_len - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->name_len - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	assert(sizeof(int) == sizeof(src->module_number));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:module_number:\t%d\n", src->module_number);
	
	
	
	
	
	


	


		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_dprint_zend_function */
	
		/* export:  	  	 	void 	xc_dprint_zend_function( 		const zend_function * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dprint_zend_function( 		const zend_function * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
			
	
	

	switch (src->type) {
	case ZEND_INTERNAL_FUNCTION:
	case ZEND_OVERLOADED_FUNCTION:
		
		break;

	case ZEND_USER_FUNCTION:
	case ZEND_EVAL_CODE:
		
		
	
	assert(sizeof(zend_op_array) == sizeof(src->op_array));
	xc_dprint_indent(indent);fprintf(stderr, "zend_op_array:op_array ");
	
	
	
	

	
	assert(sizeof(zend_op_array) == sizeof((& src->op_array)[0]));
	

	

	xc_dprint_zend_op_array(
		           & src->op_array, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	

		break;

	default:
		assert(0);
	}
	
	
	

	

		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
		
		
		
	}
/* }}} */
	

#ifdef ZEND_ENGINE_2

	
	

/* {{{ xc_dprint_zend_property_info */
	
 	  	 	static void inline 	xc_dprint_zend_property_info( 		const zend_property_info * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
				
	assert(sizeof(zend_uint) == sizeof(src->flags));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:flags:\t%u\n", src->flags);
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->name_length));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:name_length:\t%d\n", src->name_length);
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
#ifdef IS_UNICODE
			do {
				zval zv;
				zval reszv;
				int usecopy;

				INIT_ZVAL(zv);
				ZVAL_UNICODEL(&zv, ZSTR_U(src->name), src->name_length + 1 - 1, 1);
				zend_make_printable_zval(&zv, &reszv, &usecopy);
				fprintf(stderr, "string:%s:\t\"", "dst->name");
				xc_dprint_str_len(Z_STRVAL(reszv), Z_STRLEN(reszv));
				fprintf(stderr, "\" len=%lu\n", (unsigned long) src->name_length + 1 - 1);
				if (usecopy) {
					zval_dtor(&reszv);
				}
				zval_dtor(&zv);
			} while (0);
#endif
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->name");
			xc_dprint_str_len(ZSTR_S(src->name), src->name_length + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->name_length + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->name");
			xc_dprint_str_len(ZSTR_S(src->name), src->name_length + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->name_length + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "ulong:h:\t%lu\n", src->h);
	
	
	
	
	
	


	

#ifdef ZEND_ENGINE_2_4
		
	assert(sizeof(int) == sizeof(src->offset));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:offset:\t%d\n", src->offset);
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_1
		
	assert(sizeof(int) == sizeof(src->doc_comment_len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:doc_comment_len:\t%d\n", src->doc_comment_len);
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->doc_comment) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
#ifdef IS_UNICODE
			do {
				zval zv;
				zval reszv;
				int usecopy;

				INIT_ZVAL(zv);
				ZVAL_UNICODEL(&zv, ZSTR_U(src->doc_comment), src->doc_comment_len + 1 - 1, 1);
				zend_make_printable_zval(&zv, &reszv, &usecopy);
				fprintf(stderr, "string:%s:\t\"", "dst->doc_comment");
				xc_dprint_str_len(Z_STRVAL(reszv), Z_STRLEN(reszv));
				fprintf(stderr, "\" len=%lu\n", (unsigned long) src->doc_comment_len + 1 - 1);
				if (usecopy) {
					zval_dtor(&reszv);
				}
				zval_dtor(&zv);
			} while (0);
#endif
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->doc_comment");
			xc_dprint_str_len(ZSTR_S(src->doc_comment), src->doc_comment_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->doc_comment_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->doc_comment");
			xc_dprint_str_len(ZSTR_S(src->doc_comment), src->doc_comment_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->doc_comment_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

#endif
	#if defined(ZEND_ENGINE_2_2)
	
	if (src->ce) {
		
		
#ifdef IS_UNICODE
		
#else
		
#endif
	}
	else {
		
	
	
	assert(src->ce == NULL);

	}

	
	
	

#endif

		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif
#ifdef ZEND_ENGINE_2_4

	
	

/* {{{ xc_dprint_zend_trait_method_reference */
	
 	  	 	static void inline 	xc_dprint_zend_trait_method_reference( 		const zend_trait_method_reference * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
				
	assert(sizeof(unsigned int) == sizeof(src->mname_len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "unsigned int:mname_len:\t%u\n", src->mname_len);
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->method_name == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->method_name");
			xc_dprint_str_len(src->method_name, src->mname_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->mname_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

	
	
	
	
	assert(src->ce == NULL);

	
	
	


		
	assert(sizeof(unsigned int) == sizeof(src->cname_len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "unsigned int:cname_len:\t%u\n", src->cname_len);
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->class_name == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->class_name");
			xc_dprint_str_len(src->class_name, src->cname_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->cname_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	


		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dprint_zend_trait_alias */
	
 	  	 	static void inline 	xc_dprint_zend_trait_alias( 		const zend_trait_alias * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
			
	
	if (src->trait_method) {
		xc_dprint_indent(indent);fprintf(stderr, "zend_trait_method_reference:trait_method ");
		
	
	
	

	
	assert(sizeof(zend_trait_method_reference) == sizeof(( src->trait_method)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	

	xc_dprint_zend_trait_method_reference(
		            src->trait_method, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		xc_dprint_indent(indent);fprintf(stderr, "zend_trait_method_reference:trait_method:\tNULL\n");
		
	
	
	assert(src->trait_method == NULL);

	}
	
	
	
	


		
	assert(sizeof(unsigned int) == sizeof(src->alias_len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "unsigned int:alias_len:\t%u\n", src->alias_len);
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->alias == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->alias");
			xc_dprint_str_len(src->alias, src->alias_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->alias_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		
	assert(sizeof(zend_uint) == sizeof(src->modifiers));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:modifiers:\t%u\n", src->modifiers);
	
	
	
	
	
	


	

#ifndef ZEND_ENGINE_2_5
	
	
	
	
	assert(src->function == NULL);

	
	
	


#endif

		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dprint_zend_trait_precedence */
	
 	  	 	static void inline 	xc_dprint_zend_trait_precedence( 		const zend_trait_precedence * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
			
	
	if (src->trait_method) {
		xc_dprint_indent(indent);fprintf(stderr, "zend_trait_method_reference:trait_method ");
		
	
	
	

	
	assert(sizeof(zend_trait_method_reference) == sizeof(( src->trait_method)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	

	xc_dprint_zend_trait_method_reference(
		            src->trait_method, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		xc_dprint_indent(indent);fprintf(stderr, "zend_trait_method_reference:trait_method:\tNULL\n");
		
	
	
	assert(src->trait_method == NULL);

	}
	
	
	
	


		if (src->exclude_from_classes) {
		int LOOPCOUNTER;
		
						
				size_t count = 0;
				while (src->exclude_from_classes[count]) {
					++count;
				}
				++count;
				
			
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			

			for (LOOPCOUNTER = 0;
					src->exclude_from_classes[LOOPCOUNTER];
					++LOOPCOUNTER) {
				
	
	

					
					
						
	assert(sizeof(xc_ztstring) == sizeof((xc_ztstring) src->exclude_from_classes [LOOPCOUNTER]));
		
	 
	
	
	
	
	
	
	
	
	if ((xc_ztstring) src->exclude_from_classes [LOOPCOUNTER] == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->exclude_from_classes [LOOPCOUNTER]");
			xc_dprint_str_len((xc_ztstring) src->exclude_from_classes [LOOPCOUNTER], strlen((xc_ztstring) src->exclude_from_classes [LOOPCOUNTER]) + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) strlen((xc_ztstring) src->exclude_from_classes [LOOPCOUNTER]) + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

	

	

					
					
				
	
	

			}
						
				
	
	
	

	}
	else {
		
	
	
	
	assert(src->exclude_from_classes == NULL);

	
	
	


	}

#ifndef ZEND_ENGINE_2_5
	
	
	
	
	assert(src->function == NULL);

	
	
	


#endif

		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dprint_zend_trait_alias_ptr */
	
 	  	 	static void inline 	xc_dprint_zend_trait_alias_ptr( 		const zend_trait_alias_ptr * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
			
		
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

		
	
	
	

	
	assert(sizeof(zend_trait_alias) == sizeof((  src[0])[0]));
	

	

	xc_dprint_zend_trait_alias(
		             src[0], indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

		
	
	

		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dprint_zend_trait_precedence_ptr */
	
 	  	 	static void inline 	xc_dprint_zend_trait_precedence_ptr( 		const zend_trait_precedence_ptr * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
			
		
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

		
	
	
	

	
	assert(sizeof(zend_trait_precedence) == sizeof((  src[0])[0]));
	

	

	xc_dprint_zend_trait_precedence(
		             src[0], indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

		
	
	

		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_dprint_zend_class_entry */
	
		/* export:  	  	 	void 	xc_dprint_zend_class_entry( 		const zend_class_entry * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dprint_zend_class_entry( 		const zend_class_entry * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
			
		
	assert(sizeof(char) == sizeof(src->type));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "char:type:\t%d\n", src->type);
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->name_length));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:name_length:\t%u\n", src->name_length);
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
#ifdef IS_UNICODE
			do {
				zval zv;
				zval reszv;
				int usecopy;

				INIT_ZVAL(zv);
				ZVAL_UNICODEL(&zv, ZSTR_U(src->name), src->name_length + 1 - 1, 1);
				zend_make_printable_zval(&zv, &reszv, &usecopy);
				fprintf(stderr, "string:%s:\t\"", "dst->name");
				xc_dprint_str_len(Z_STRVAL(reszv), Z_STRLEN(reszv));
				fprintf(stderr, "\" len=%lu\n", (unsigned long) src->name_length + 1 - 1);
				if (usecopy) {
					zval_dtor(&reszv);
				}
				zval_dtor(&zv);
			} while (0);
#endif
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->name");
			xc_dprint_str_len(ZSTR_S(src->name), src->name_length + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->name_length + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->name");
			xc_dprint_str_len(ZSTR_S(src->name), src->name_length + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->name_length + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	if (src->parent) {
		
		
#ifdef IS_UNICODE
		
#else
		
#endif
	}
	else {
		
	
	
	assert(src->parent == NULL);

	}

	
	
	

	
#ifdef ZEND_ENGINE_2
		
	assert(sizeof(int) == sizeof(src->refcount));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:refcount:\t%d\n", src->refcount);
	
	
	
	
	
	


	

#else
	
	
	if (src->refcount) {
		xc_dprint_indent(indent);fprintf(stderr, "int:refcount ");
		
	
	
	

	
	assert(sizeof(int) == sizeof(( src->refcount)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	

	xc_dprint_int(
		            src->refcount, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		xc_dprint_indent(indent);fprintf(stderr, "int:refcount:\tNULL\n");
		
	
	
	assert(src->refcount == NULL);

	}
	
	
	
	


#endif
#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_bool) == sizeof(src->constants_updated));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:constants_updated:\t%u\n", src->constants_updated);
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2
		
	assert(sizeof(zend_uint) == sizeof(src->ce_flags));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:ce_flags:\t%u\n", src->ce_flags);
	
	
	
	
	
	


	

#endif

#ifdef ZEND_ENGINE_2
	
	
	assert(sizeof(HashTable) == sizeof(src->properties_info));
	xc_dprint_indent(indent);fprintf(stderr, "HashTable:properties_info ");
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->properties_info)[0]));
	

	

	xc_dprint_HashTable_zend_property_info(
		           & src->properties_info, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


#endif

#ifdef ZEND_ENGINE_2_4
	
	if (src->default_properties_table) {
		int i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			

			for (i = 0;
					i < src->default_properties_count;
					++i) {
				
	
	

					
	
	assert(sizeof(zval_ptr_nullable) == sizeof(src->default_properties_table[i]));
	xc_dprint_indent(indent);fprintf(stderr, "zval_ptr_nullable:default_properties_table[i] ");
	
	
	
	

	
	assert(sizeof(zval_ptr_nullable) == sizeof((& src->default_properties_table[i])[0]));
	

	

	xc_dprint_zval_ptr_nullable(
		           & src->default_properties_table[i], indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->default_properties_table == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->default_properties_count));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:default_properties_count:\t%d\n", src->default_properties_count);
	
	
	
	
	
	


	

	
	if (src->default_static_members_table) {
		int i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			

			for (i = 0;
					i < src->default_static_members_count;
					++i) {
				
	
	

					
	
	assert(sizeof(zval_ptr_nullable) == sizeof(src->default_static_members_table[i]));
	xc_dprint_indent(indent);fprintf(stderr, "zval_ptr_nullable:default_static_members_table[i] ");
	
	
	
	

	
	assert(sizeof(zval_ptr_nullable) == sizeof((& src->default_static_members_table[i])[0]));
	

	

	xc_dprint_zval_ptr_nullable(
		           & src->default_static_members_table[i], indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->default_static_members_table == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->default_static_members_count));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:default_static_members_count:\t%d\n", src->default_static_members_count);
	
	
	
	
	
	


	

	
	
	
	
	

#else
	
	
	
	
	

	
	
	assert(sizeof(HashTable) == sizeof(src->default_properties));
	xc_dprint_indent(indent);fprintf(stderr, "HashTable:default_properties ");
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->default_properties)[0]));
	

	

	xc_dprint_HashTable_zval_ptr(
		           & src->default_properties, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


#	ifdef ZEND_ENGINE_2_1
	
	
	assert(sizeof(HashTable) == sizeof(src->default_static_members));
	xc_dprint_indent(indent);fprintf(stderr, "HashTable:default_static_members ");
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->default_static_members)[0]));
	

	

	xc_dprint_HashTable_zval_ptr(
		           & src->default_static_members, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
	
	
	

#	elif defined(ZEND_ENGINE_2)
	
	
	if (src->static_members) {
		xc_dprint_indent(indent);fprintf(stderr, "HashTable:static_members ");
		
	
	
	

	
	assert(sizeof(HashTable) == sizeof(( src->static_members)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	

	xc_dprint_HashTable_zval_ptr(
		            src->static_members, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		xc_dprint_indent(indent);fprintf(stderr, "HashTable:static_members:\tNULL\n");
		
	
	
	assert(src->static_members == NULL);

	}
	
	
	
	


#	endif
#endif /* ZEND_ENGINE_2_4 */

#ifdef ZEND_ENGINE_2
	
	
	assert(sizeof(HashTable) == sizeof(src->constants_table));
	xc_dprint_indent(indent);fprintf(stderr, "HashTable:constants_table ");
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->constants_table)[0]));
	

	

	xc_dprint_HashTable_zval_ptr(
		           & src->constants_table, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	



#ifdef ZEND_ENGINE_2_2
		
	
	
	
	assert(src->interfaces == NULL);

	
	
	


	
	
	
	
	assert(src->num_interfaces == 0);

	
	
	



#	ifdef ZEND_ENGINE_2_4
		
	
	
	
	assert(src->traits == NULL);

	
	
	


	
	
	
	
	assert(src->num_traits == 0);

	
	
	


	
	if (src->trait_aliases) {
		size_t i; 
		
	

		
						
				size_t count = 0;
				while (src->trait_aliases[count]) {
					++count;
				}
				++count;
				
			
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			

			for (i = 0;
					src->trait_aliases[i];
					++i) {
				
	
	

					
	
	assert(sizeof(zend_trait_alias_ptr) == sizeof(src->trait_aliases[i]));
	xc_dprint_indent(indent);fprintf(stderr, "zend_trait_alias_ptr:trait_aliases[i] ");
	
	
	
	

	
	assert(sizeof(zend_trait_alias_ptr) == sizeof((& src->trait_aliases[i])[0]));
	

	

	xc_dprint_zend_trait_alias_ptr(
		           & src->trait_aliases[i], indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->trait_aliases == NULL);

	
	
	


	}

	
	if (src->trait_precedences) {
		size_t i; 
		
	

		
						
				size_t count = 0;
				while (src->trait_precedences[count]) {
					++count;
				}
				++count;
				
			
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			

			for (i = 0;
					src->trait_precedences[i];
					++i) {
				
	
	

					
	
	assert(sizeof(zend_trait_precedence_ptr) == sizeof(src->trait_precedences[i]));
	xc_dprint_indent(indent);fprintf(stderr, "zend_trait_precedence_ptr:trait_precedences[i] ");
	
	
	
	

	
	assert(sizeof(zend_trait_precedence_ptr) == sizeof((& src->trait_precedences[i])[0]));
	

	

	xc_dprint_zend_trait_precedence_ptr(
		           & src->trait_precedences[i], indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->trait_precedences == NULL);

	
	
	


	}

#	endif
#else
	
		
	
	
	

	
		
	assert(sizeof(zend_uint) == sizeof(src->num_interfaces));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:num_interfaces:\t%u\n", src->num_interfaces);
	
	
	
	
	
	


	

#endif

#	ifdef ZEND_ENGINE_2_4
	
	
	

	 
	
	
	
	
	
	
	
	
	if (src->info.user.filename == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->info.user.filename");
			xc_dprint_str_len(src->info.user.filename, strlen(src->info.user.filename) + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) strlen(src->info.user.filename) + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		
	assert(sizeof(zend_uint) == sizeof(src->info.user.line_start));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:info.user.line_start:\t%u\n", src->info.user.line_start);
	
	
	

	

		
	assert(sizeof(zend_uint) == sizeof(src->info.user.line_end));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:info.user.line_end:\t%u\n", src->info.user.line_end);
	
	
	

	

		
	assert(sizeof(zend_uint) == sizeof(src->info.user.doc_comment_len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:info.user.doc_comment_len:\t%u\n", src->info.user.doc_comment_len);
	
	
	

	

	 
	
#ifdef IS_UNICODE
	
	
	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->info.user.doc_comment) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
#ifdef IS_UNICODE
			do {
				zval zv;
				zval reszv;
				int usecopy;

				INIT_ZVAL(zv);
				ZVAL_UNICODEL(&zv, ZSTR_U(src->info.user.doc_comment), src->info.user.doc_comment_len + 1 - 1, 1);
				zend_make_printable_zval(&zv, &reszv, &usecopy);
				fprintf(stderr, "string:%s:\t\"", "dst->info.user.doc_comment");
				xc_dprint_str_len(Z_STRVAL(reszv), Z_STRLEN(reszv));
				fprintf(stderr, "\" len=%lu\n", (unsigned long) src->info.user.doc_comment_len + 1 - 1);
				if (usecopy) {
					zval_dtor(&reszv);
				}
				zval_dtor(&zv);
			} while (0);
#endif
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->info.user.doc_comment) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->info.user.doc_comment");
			xc_dprint_str_len(ZSTR_S(src->info.user.doc_comment), src->info.user.doc_comment_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->info.user.doc_comment_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->info.user.doc_comment) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->info.user.doc_comment");
			xc_dprint_str_len(ZSTR_S(src->info.user.doc_comment), src->info.user.doc_comment_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->info.user.doc_comment_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

	
	
	

	
	
	
	

#	else
	 
	
	
	

	
	
	
	
	
	
	
	
	if (src->filename == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->filename");
			xc_dprint_str_len(src->filename, strlen(src->filename) + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) strlen(src->filename) + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		
	assert(sizeof(zend_uint) == sizeof(src->line_start));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:line_start:\t%u\n", src->line_start);
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->line_end));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:line_end:\t%u\n", src->line_end);
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->doc_comment_len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:doc_comment_len:\t%u\n", src->doc_comment_len);
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->doc_comment) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
#ifdef IS_UNICODE
			do {
				zval zv;
				zval reszv;
				int usecopy;

				INIT_ZVAL(zv);
				ZVAL_UNICODEL(&zv, ZSTR_U(src->doc_comment), src->doc_comment_len + 1 - 1, 1);
				zend_make_printable_zval(&zv, &reszv, &usecopy);
				fprintf(stderr, "string:%s:\t\"", "dst->doc_comment");
				xc_dprint_str_len(Z_STRVAL(reszv), Z_STRLEN(reszv));
				fprintf(stderr, "\" len=%lu\n", (unsigned long) src->doc_comment_len + 1 - 1);
				if (usecopy) {
					zval_dtor(&reszv);
				}
				zval_dtor(&zv);
			} while (0);
#endif
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->doc_comment");
			xc_dprint_str_len(ZSTR_S(src->doc_comment), src->doc_comment_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->doc_comment_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->doc_comment");
			xc_dprint_str_len(ZSTR_S(src->doc_comment), src->doc_comment_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->doc_comment_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

#	endif

	/* # NOT DONE */
#	ifdef ZEND_ENGINE_2_1
	
	
	
	

	
	
	
	

#	endif
	
	
	
	

	
	
	
	

	
	
	
	

	
	
	
	

#	ifdef ZEND_ENGINE_2_3
	
	
	
	

#	endif
#	ifdef ZEND_ENGINE_2_1
	
	
	
	

	
	
	
	

#	endif
	/* deal with it inside xc_fix_method */
	
	
	
	

	
	
	
	

	
	
	
	

	
	
	
	

	
	
	
	

/* should be >5.1 */
#	ifdef ZEND_ENGINE_2_1
	
	
	
	

	
	
	
	

#	endif
	
	
	
	

#	ifdef ZEND_CALLSTATIC_FUNC_NAME
	
	
	
	

#	endif
# if defined(ZEND_ENGINE_2_2) || PHP_MAJOR_VERSION >= 6
	
	
	
	

# endif
# if defined(ZEND_ENGINE_2_6)
	
	
	
	

# endif
#	ifndef ZEND_ENGINE_2_4
	/* # NOT DONE */
	
	
	
	

#	endif
#else /* ZEND_ENGINE_2 */
	
	
	
	

	
	
	
	

	
	
	
	

#endif
		
	
	assert(sizeof(HashTable) == sizeof(src->function_table));
	xc_dprint_indent(indent);fprintf(stderr, "HashTable:function_table ");
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->function_table)[0]));
	

	

	xc_dprint_HashTable_zend_function(
		           & src->function_table, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	

		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#ifdef ZEND_ENGINE_2_4


#else

	
	

/* {{{ xc_dprint_znode */
	
 	  	 	static void inline 	xc_dprint_znode( 		const znode * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
				
	assert(sizeof(xc_op_type) == sizeof(src->op_type));
		
		xc_dprint_indent(indent);
		fprintf(stderr, ":op_type:\t%d %s\n", src->op_type, xc_get_op_type(src->op_type));
		
	
	
	

	

	


#ifdef IS_CV
#	define XCACHE_IS_CV IS_CV
#else
/* compatible with zend optimizer */
#	define XCACHE_IS_CV 16
#endif
	assert(src->op_type == IS_CONST ||
		src->op_type == IS_VAR ||
		src->op_type == XCACHE_IS_CV ||
		src->op_type == IS_TMP_VAR ||
		src->op_type == IS_UNUSED);
		
	
	

	switch (src->op_type) {
		case IS_CONST:
			
	
	assert(sizeof(zval) == sizeof(src->u.constant));
	xc_dprint_indent(indent);fprintf(stderr, "zval:u.constant ");
	
	
	
	

	
	assert(sizeof(zval) == sizeof((& src->u.constant)[0]));
	

	

	xc_dprint_zval(
		           & src->u.constant, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	

			break;
		
		case IS_VAR:
		case IS_TMP_VAR:
		case XCACHE_IS_CV:
				
	assert(sizeof(zend_uint) == sizeof(src->u.var));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:u.var:\t%u\n", src->u.var);
	
	
	

	

				
	assert(sizeof(zend_uint) == sizeof(src->u.EA.type));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:u.EA.type:\t%u\n", src->u.EA.type);
	
	
	

	

			break;
		case IS_UNUSED:
			
				
	assert(sizeof(zend_uint) == sizeof(src->u.opline_num));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:u.opline_num:\t%u\n", src->u.opline_num);
	
	
	

	

#ifndef ZEND_ENGINE_2
				
	assert(sizeof(zend_uint) == sizeof(src->u.fetch_type));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:u.fetch_type:\t%u\n", src->u.fetch_type);
	
	
	

	

#endif
				
	assert(sizeof(zend_uint) == sizeof(src->u.EA.type));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:u.EA.type:\t%u\n", src->u.EA.type);
	
	
	

	

			break;
		
	}
	
	
	

	
	
	
	

#if 0
	
	
	
	

#endif
#undef XCACHE_IS_CV

		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_dprint_zend_op */
	
		/* export:  	  	 	void 	xc_dprint_zend_op( 		const zend_op * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dprint_zend_op( 		const zend_op * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
				
	assert(sizeof(xc_opcode) == sizeof(src->opcode));
		
		xc_dprint_indent(indent);
		fprintf(stderr, ":opcode:\t%u %s\n", src->opcode, xc_get_opcode(src->opcode));
		
	
	
	

	

	

#ifdef ZEND_ENGINE_2_4
	
	switch (src->opcode) {
	case ZEND_BIND_TRAITS:
		((zend_op *) src)->op2_type = IS_UNUSED;
		break;
	}
	
	#ifndef NDEBUG
	switch ((src->result_type & ~EXT_TYPE_UNUSED)) {
	case IS_CONST:
	case IS_VAR:
	case IS_CV:
	case IS_TMP_VAR:
	case IS_UNUSED:
		break;

	default:
		assert(0);
	}
#endif

		
	
	

	switch ((src->result_type & ~EXT_TYPE_UNUSED)) {
		case IS_CONST:
			
					
	assert(sizeof(zend_uint) == sizeof(src->result.constant));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:result.constant:\t%u\n", src->result.constant);
	
	
	

	

			
			break;
		
		case IS_VAR:
		case IS_TMP_VAR:
		case IS_CV:
				
	assert(sizeof(zend_uint) == sizeof(src->result.var));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:result.var:\t%u\n", src->result.var);
	
	
	

	

			break;
		case IS_UNUSED:
			
				
	assert(sizeof(zend_uint) == sizeof(src->result.opline_num));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:result.opline_num:\t%u\n", src->result.opline_num);
	
	
	

	

			break;
		
	}
	
	
	

	
	
	
	


	#ifndef NDEBUG
	switch ((src->op1_type )) {
	case IS_CONST:
	case IS_VAR:
	case IS_CV:
	case IS_TMP_VAR:
	case IS_UNUSED:
		break;

	default:
		assert(0);
	}
#endif

		
	
	

	switch ((src->op1_type )) {
		case IS_CONST:
			
				
					
							
	assert(sizeof(zend_uint) == sizeof(src->op1.constant));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:op1.constant:\t%u\n", src->op1.constant);
	
	
	

	

					
				
			
			break;
		
		case IS_VAR:
		case IS_TMP_VAR:
		case IS_CV:
				
	assert(sizeof(zend_uint) == sizeof(src->op1.var));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:op1.var:\t%u\n", src->op1.var);
	
	
	

	

			break;
		case IS_UNUSED:
			
				
	assert(sizeof(zend_uint) == sizeof(src->op1.opline_num));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:op1.opline_num:\t%u\n", src->op1.opline_num);
	
	
	

	

			break;
		
	}
	
	
	

	
	
	
	


	#ifndef NDEBUG
	switch ((src->op2_type )) {
	case IS_CONST:
	case IS_VAR:
	case IS_CV:
	case IS_TMP_VAR:
	case IS_UNUSED:
		break;

	default:
		assert(0);
	}
#endif

		
	
	

	switch ((src->op2_type )) {
		case IS_CONST:
			
				
					
							
	assert(sizeof(zend_uint) == sizeof(src->op2.constant));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:op2.constant:\t%u\n", src->op2.constant);
	
	
	

	

					
				
			
			break;
		
		case IS_VAR:
		case IS_TMP_VAR:
		case IS_CV:
				
	assert(sizeof(zend_uint) == sizeof(src->op2.var));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:op2.var:\t%u\n", src->op2.var);
	
	
	

	

			break;
		case IS_UNUSED:
			
				
	assert(sizeof(zend_uint) == sizeof(src->op2.opline_num));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:op2.opline_num:\t%u\n", src->op2.opline_num);
	
	
	

	

			break;
		
	}
	
	
	

	
	
	
	


#else
	
	
	assert(sizeof(znode) == sizeof(src->result));
	xc_dprint_indent(indent);fprintf(stderr, "znode:result ");
	
	
	
	

	
	assert(sizeof(znode) == sizeof((& src->result)[0]));
	

	

	xc_dprint_znode(
		           & src->result, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
	assert(sizeof(znode) == sizeof(src->op1));
	xc_dprint_indent(indent);fprintf(stderr, "znode:op1 ");
	
	
	
	

	
	assert(sizeof(znode) == sizeof((& src->op1)[0]));
	

	

	xc_dprint_znode(
		           & src->op1, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
	assert(sizeof(znode) == sizeof(src->op2));
	xc_dprint_indent(indent);fprintf(stderr, "znode:op2 ");
	
	
	
	

	
	assert(sizeof(znode) == sizeof((& src->op2)[0]));
	

	

	xc_dprint_znode(
		           & src->op2, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


#endif
		
	assert(sizeof(ulong) == sizeof(src->extended_value));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "ulong:extended_value:\t%lu\n", src->extended_value);
	
	
	
	
	
	


	

		
	assert(sizeof(uint) == sizeof(src->lineno));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "uint:lineno:\t%u\n", src->lineno);
	
	
	
	
	
	


	

#ifdef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uchar) == sizeof(src->op1_type));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uchar:op1_type:\t%u\n", src->op1_type);
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uchar) == sizeof(src->op2_type));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uchar:op2_type:\t%u\n", src->op2_type);
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uchar) == sizeof(src->result_type));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uchar:result_type:\t%u\n", src->result_type);
	
	
	
	
	
	


	

#endif
	
#ifdef ZEND_ENGINE_2
		
	assert(sizeof(opcode_handler_t) == sizeof(src->handler));
	/* is copying enough? */
	
	
	

	

#endif

		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#ifdef ZEND_ENGINE_2_4

	
	

/* {{{ xc_dprint_zend_literal */
	
 	  	 	static void inline 	xc_dprint_zend_literal( 		const zend_literal * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
			
	
	assert(sizeof(zval) == sizeof(src->constant));
	xc_dprint_indent(indent);fprintf(stderr, "zval:constant ");
	
	
	
	

	
	assert(sizeof(zval) == sizeof((& src->constant)[0]));
	

	

	xc_dprint_zval(
		           & src->constant, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


		
	assert(sizeof(zend_ulong) == sizeof(src->hash_value));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_ulong:hash_value:\t%lu\n", src->hash_value);
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->cache_slot));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:cache_slot:\t%u\n", src->cache_slot);
	
	
	
	
	
	


	


		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_dprint_zend_op_array */
	
		/* export:  	  	 	void 	xc_dprint_zend_op_array( 		const zend_op_array * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dprint_zend_op_array( 		const zend_op_array * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
			
	
	{
	
	do {
	
	/* Common elements */
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uchar:type:\t%u\n", src->type);
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->function_name) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
#ifdef IS_UNICODE
			do {
				zval zv;
				zval reszv;
				int usecopy;

				INIT_ZVAL(zv);
				ZVAL_UNICODEL(&zv, ZSTR_U(src->function_name), xc_zstrlen_uchar (src->function_name) + 1 - 1, 1);
				zend_make_printable_zval(&zv, &reszv, &usecopy);
				fprintf(stderr, "string:%s:\t\"", "dst->function_name");
				xc_dprint_str_len(Z_STRVAL(reszv), Z_STRLEN(reszv));
				fprintf(stderr, "\" len=%lu\n", (unsigned long) xc_zstrlen_uchar (src->function_name) + 1 - 1);
				if (usecopy) {
					zval_dtor(&reszv);
				}
				zval_dtor(&zv);
			} while (0);
#endif
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->function_name) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->function_name");
			xc_dprint_str_len(ZSTR_S(src->function_name), xc_zstrlen_char (src->function_name) + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) xc_zstrlen_char (src->function_name) + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->function_name) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->function_name");
			xc_dprint_str_len(ZSTR_S(src->function_name), xc_zstrlen_char (src->function_name) + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) xc_zstrlen_char (src->function_name) + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

#ifdef ZEND_ENGINE_2
		
	assert(sizeof(zend_uint) == sizeof(src->fn_flags));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:fn_flags:\t%u\n", src->fn_flags);
	
	
	
	
	
	


	

	
	if (src->arg_info) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			

			for (i = 0;
					i < src->num_args;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_arg_info) == sizeof(src->arg_info[i]));
	xc_dprint_indent(indent);fprintf(stderr, "zend_arg_info:arg_info[i] ");
	
	
	
	

	
	assert(sizeof(zend_arg_info) == sizeof((& src->arg_info[i])[0]));
	

	

	xc_dprint_zend_arg_info(
		           & src->arg_info[i], indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->arg_info == NULL);

	
	
	


	}

		
	assert(sizeof(zend_uint) == sizeof(src->num_args));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:num_args:\t%u\n", src->num_args);
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->required_num_args));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:required_num_args:\t%u\n", src->required_num_args);
	
	
	
	
	
	


	

#	ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_bool) == sizeof(src->pass_rest_by_reference));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:pass_rest_by_reference:\t%u\n", src->pass_rest_by_reference);
	
	
	
	
	
	


	

#	endif
#else
	if (src->arg_types) {
		
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

		
		
		
	
	
	

	}
	else {
		
	
	
	
	assert(src->arg_types == NULL);

	
	
	


	}
#endif
#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(unsigned char) == sizeof(src->return_reference));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "unsigned char:return_reference:\t%u\n", src->return_reference);
	
	
	
	
	
	


	

#endif
	/* END of common elements */
#ifdef IS_UNICODE
	#endif

	
	
	if (src->refcount) {
		xc_dprint_indent(indent);fprintf(stderr, "zend_uint:refcount ");
		
	
	
	

	
	assert(sizeof(zend_uint) == sizeof(( src->refcount)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	

	xc_dprint_zend_uint(
		            src->refcount, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		xc_dprint_indent(indent);fprintf(stderr, "zend_uint:refcount:\tNULL\n");
		
	
	
	assert(src->refcount == NULL);

	}
	
	
	
	


	

#ifdef ZEND_ENGINE_2_4
		
	if (src->literals) {
		int i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			

			for (i = 0;
					i < src->last_literal;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_literal) == sizeof(src->literals[i]));
	xc_dprint_indent(indent);fprintf(stderr, "zend_literal:literals[i] ");
	
	
	
	

	
	assert(sizeof(zend_literal) == sizeof((& src->literals[i])[0]));
	

	

	xc_dprint_zend_literal(
		           & src->literals[i], indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->literals == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->last_literal));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:last_literal:\t%d\n", src->last_literal);
	
	
	
	
	
	


	

#endif

		
	if (src->opcodes) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			

			for (i = 0;
					i < src->last;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_op) == sizeof(src->opcodes[i]));
	xc_dprint_indent(indent);fprintf(stderr, "zend_op:opcodes[i] ");
	
	
	
	

	
	assert(sizeof(zend_op) == sizeof((& src->opcodes[i])[0]));
	

	

	xc_dprint_zend_op(
		           & src->opcodes[i], indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->opcodes == NULL);

	
	
	


	}

		
	assert(sizeof(zend_uint) == sizeof(src->last));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:last:\t%u\n", src->last);
	
	
	
	
	
	


	

#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uint) == sizeof(src->size));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:size:\t%u\n", src->size);
	
	
	
	
	
	


	

#endif

#ifdef IS_CV
	
	if (src->vars) {
		int i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			

			for (i = 0;
					i < src->last_var;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_compiled_variable) == sizeof(src->vars[i]));
	xc_dprint_indent(indent);fprintf(stderr, "zend_compiled_variable:vars[i] ");
	
	
	
	

	
	assert(sizeof(zend_compiled_variable) == sizeof((& src->vars[i])[0]));
	

	

	xc_dprint_zend_compiled_variable(
		           & src->vars[i], indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->vars == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->last_var));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:last_var:\t%d\n", src->last_var);
	
	
	
	
	
	


	

#	ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uint) == sizeof(src->size_var));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:size_var:\t%u\n", src->size_var);
	
	
	
	
	
	


	

#	endif
#else
		
#endif

		
	assert(sizeof(zend_uint) == sizeof(src->T));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:T:\t%u\n", src->T);
	
	
	
	
	
	


	


#ifdef ZEND_ENGINE_2_5
		
	assert(sizeof(zend_uint) == sizeof(src->nested_calls));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:nested_calls:\t%u\n", src->nested_calls);
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->used_stack));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:used_stack:\t%u\n", src->used_stack);
	
	
	
	
	
	


	

#endif

	
	if (src->brk_cont_array) {
		last_brk_cont_t i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			

			for (i = 0;
					i < src->last_brk_cont;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_brk_cont_element) == sizeof(src->brk_cont_array[i]));
	xc_dprint_indent(indent);fprintf(stderr, "zend_brk_cont_element:brk_cont_array[i] ");
	
	
	
	

	
	assert(sizeof(zend_brk_cont_element) == sizeof((& src->brk_cont_array[i])[0]));
	

	

	xc_dprint_zend_brk_cont_element(
		           & src->brk_cont_array[i], indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->brk_cont_array == NULL);

	
	
	


	}

		
	assert(sizeof(last_brk_cont_t) == sizeof(src->last_brk_cont));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "last_brk_cont_t:last_brk_cont:\t%d\n", src->last_brk_cont);
	
	
	
	
	
	



	

#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uint) == sizeof(src->current_brk_cont));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:current_brk_cont:\t%u\n", src->current_brk_cont);
	
	
	
	
	
	


	

#endif
#ifndef ZEND_ENGINE_2
		
	assert(sizeof(zend_bool) == sizeof(src->uses_globals));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:uses_globals:\t%u\n", src->uses_globals);
	
	
	
	
	
	


	

#endif

#ifdef ZEND_ENGINE_2
	
	if (src->try_catch_array) {
		int i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			

			for (i = 0;
					i < src->last_try_catch;
					++i) {
				
	
	

					
	
	assert(sizeof(zend_try_catch_element) == sizeof(src->try_catch_array[i]));
	xc_dprint_indent(indent);fprintf(stderr, "zend_try_catch_element:try_catch_array[i] ");
	
	
	
	

	
	assert(sizeof(zend_try_catch_element) == sizeof((& src->try_catch_array[i])[0]));
	

	

	xc_dprint_zend_try_catch_element(
		           & src->try_catch_array[i], indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->try_catch_array == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->last_try_catch));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:last_try_catch:\t%d\n", src->last_try_catch);
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_5
		
	assert(sizeof(zend_bool) == sizeof(src->has_finally_block));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:has_finally_block:\t%u\n", src->has_finally_block);
	
	
	
	
	
	


	

#endif

	
	
	if (src->static_variables) {
		xc_dprint_indent(indent);fprintf(stderr, "HashTable:static_variables ");
		
	
	
	

	
	assert(sizeof(HashTable) == sizeof(( src->static_variables)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	

	xc_dprint_HashTable_zval_ptr(
		            src->static_variables, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		xc_dprint_indent(indent);fprintf(stderr, "HashTable:static_variables:\tNULL\n");
		
	
	
	assert(src->static_variables == NULL);

	}
	
	
	
	



#ifndef ZEND_ENGINE_2_4
	
	
	
	

		
	assert(sizeof(int) == sizeof(src->backpatch_count));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:backpatch_count:\t%d\n", src->backpatch_count);
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_3
		
	assert(sizeof(zend_uint) == sizeof(src->this_var));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:this_var:\t%u\n", src->this_var);
	
	
	
	
	
	


	

#endif

#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_bool) == sizeof(src->done_pass_two));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:done_pass_two:\t%u\n", src->done_pass_two);
	
	
	
	
	
	


	

#endif
	/* 5.0 <= ver < 5.3 */
#if defined(ZEND_ENGINE_2) && !defined(ZEND_ENGINE_2_3)
		
	assert(sizeof(zend_bool) == sizeof(src->uses_this));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:uses_this:\t%u\n", src->uses_this);
	
	
	
	
	
	


	

#endif

	 
	
	
	

	
	
	
	
	
	
	
	
	if (src->filename == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->filename");
			xc_dprint_str_len(src->filename, strlen(src->filename) + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) strlen(src->filename) + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

#ifdef IS_UNICODE
	
		 
	
	
	

	
	
	
	
	
	
	
	
	if (src->script_encoding == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->script_encoding");
			xc_dprint_str_len(src->script_encoding, strlen(src->script_encoding) + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) strlen(src->script_encoding) + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

	
#endif
#ifdef ZEND_ENGINE_2
		
	assert(sizeof(zend_uint) == sizeof(src->line_start));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:line_start:\t%u\n", src->line_start);
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->line_end));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:line_end:\t%u\n", src->line_end);
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->doc_comment_len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:doc_comment_len:\t%d\n", src->doc_comment_len);
	
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->doc_comment) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
#ifdef IS_UNICODE
			do {
				zval zv;
				zval reszv;
				int usecopy;

				INIT_ZVAL(zv);
				ZVAL_UNICODEL(&zv, ZSTR_U(src->doc_comment), src->doc_comment_len + 1 - 1, 1);
				zend_make_printable_zval(&zv, &reszv, &usecopy);
				fprintf(stderr, "string:%s:\t\"", "dst->doc_comment");
				xc_dprint_str_len(Z_STRVAL(reszv), Z_STRLEN(reszv));
				fprintf(stderr, "\" len=%lu\n", (unsigned long) src->doc_comment_len + 1 - 1);
				if (usecopy) {
					zval_dtor(&reszv);
				}
				zval_dtor(&zv);
			} while (0);
#endif
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->doc_comment");
			xc_dprint_str_len(ZSTR_S(src->doc_comment), src->doc_comment_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->doc_comment_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->doc_comment");
			xc_dprint_str_len(ZSTR_S(src->doc_comment), src->doc_comment_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->doc_comment_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

#endif
#ifdef ZEND_COMPILE_DELAYED_BINDING
		
	assert(sizeof(zend_uint) == sizeof(src->early_binding));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:early_binding:\t%u\n", src->early_binding);
	
	
	
	
	
	


	
;
#endif

	/* reserved */
	
	
	
	

#if defined(HARDENING_PATCH) && HARDENING_PATCH
		
	assert(sizeof(zend_bool) == sizeof(src->created_by_eval));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:created_by_eval:\t%u\n", src->created_by_eval);
	
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_4
	
	
	
	

		
	assert(sizeof(int) == sizeof(src->last_cache_slot));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:last_cache_slot:\t%d\n", src->last_cache_slot);
	
	
	
	
	
	


	

#endif
	} while (0);
	

#ifdef ZEND_ENGINE_2
		
		
	
	
	
	
	


#endif

#ifdef ZEND_ENGINE_2
	
	if (src->scope) {
		
		
#ifdef IS_UNICODE
		
#else
		
#endif
	}
	else {
		
	
	
	assert(src->scope == NULL);

	}

	
	
	

	
#endif

	
	}
	
	

		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


#ifdef HAVE_XCACHE_CONSTANT

	
	

/* {{{ xc_dprint_xc_constinfo_t */
	
 	  	 	static void inline 	xc_dprint_xc_constinfo_t( 		const xc_constinfo_t * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
				
	assert(sizeof(zend_uint) == sizeof(src->key_size));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:key_size:\t%u\n", src->key_size);
	
	
	
	
	
	


	

#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uchar:type:\t%u\n", src->type);
	
	
	
	
	
	


	

#endif
	
		
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (src->type == IS_UNICODE) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->key) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
#ifdef IS_UNICODE
			do {
				zval zv;
				zval reszv;
				int usecopy;

				INIT_ZVAL(zv);
				ZVAL_UNICODEL(&zv, ZSTR_U(src->key), src->key_size - 1, 1);
				zend_make_printable_zval(&zv, &reszv, &usecopy);
				fprintf(stderr, "string:%s:\t\"", "dst->key");
				xc_dprint_str_len(Z_STRVAL(reszv), Z_STRLEN(reszv));
				fprintf(stderr, "\" len=%lu\n", (unsigned long) src->key_size - 1);
				if (usecopy) {
					zval_dtor(&reszv);
				}
				zval_dtor(&zv);
			} while (0);
#endif
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->key");
			xc_dprint_str_len(ZSTR_S(src->key), src->key_size - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->key_size - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->key");
			xc_dprint_str_len(ZSTR_S(src->key), src->key_size - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->key_size - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "ulong:h:\t%lu\n", src->h);
	
	
	
	
	
	


	

	
	
	assert(sizeof(zend_constant) == sizeof(src->constant));
	xc_dprint_indent(indent);fprintf(stderr, "zend_constant:constant ");
	
	
	
	

	
	assert(sizeof(zend_constant) == sizeof((& src->constant)[0]));
	

	

	xc_dprint_zend_constant(
		           & src->constant, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	



		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif


	
	

/* {{{ xc_dprint_xc_op_array_info_detail_t */
	
 	  	 	static void inline 	xc_dprint_xc_op_array_info_detail_t( 		const xc_op_array_info_detail_t * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
				
	assert(sizeof(zend_uint) == sizeof(src->index));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:index:\t%u\n", src->index);
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->info));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:info:\t%u\n", src->info);
	
	
	
	
	
	


	


		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dprint_xc_op_array_info_t */
	
 	  	 	static void inline 	xc_dprint_xc_op_array_info_t( 		const xc_op_array_info_t * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
		#ifdef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uint) == sizeof(src->literalinfo_cnt));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:literalinfo_cnt:\t%u\n", src->literalinfo_cnt);
	
	
	
	
	
	


	

	
	if (src->literalinfos) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			

			for (i = 0;
					i < src->literalinfo_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_op_array_info_detail_t) == sizeof(src->literalinfos[i]));
	xc_dprint_indent(indent);fprintf(stderr, "xc_op_array_info_detail_t:literalinfos[i] ");
	
	
	
	

	
	assert(sizeof(xc_op_array_info_detail_t) == sizeof((& src->literalinfos[i])[0]));
	

	

	xc_dprint_xc_op_array_info_detail_t(
		           & src->literalinfos[i], indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->literalinfos == NULL);

	
	
	


	}

#else
		
	assert(sizeof(zend_uint) == sizeof(src->oplineinfo_cnt));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:oplineinfo_cnt:\t%u\n", src->oplineinfo_cnt);
	
	
	
	
	
	


	

	
	if (src->oplineinfos) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			

			for (i = 0;
					i < src->oplineinfo_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_op_array_info_detail_t) == sizeof(src->oplineinfos[i]));
	xc_dprint_indent(indent);fprintf(stderr, "xc_op_array_info_detail_t:oplineinfos[i] ");
	
	
	
	

	
	assert(sizeof(xc_op_array_info_detail_t) == sizeof((& src->oplineinfos[i])[0]));
	

	

	xc_dprint_xc_op_array_info_detail_t(
		           & src->oplineinfos[i], indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->oplineinfos == NULL);

	
	
	


	}

#endif

		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	



	
	

/* {{{ xc_dprint_xc_funcinfo_t */
	
		/* export:  	  	 	void 	xc_dprint_xc_funcinfo_t( 		const xc_funcinfo_t * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dprint_xc_funcinfo_t( 		const xc_funcinfo_t * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
				
	assert(sizeof(zend_uint) == sizeof(src->key_size));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:key_size:\t%u\n", src->key_size);
	
	
	
	
	
	


	

#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uchar:type:\t%u\n", src->type);
	
	
	
	
	
	


	

#endif
	
		
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (src->type == IS_UNICODE) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->key) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
#ifdef IS_UNICODE
			do {
				zval zv;
				zval reszv;
				int usecopy;

				INIT_ZVAL(zv);
				ZVAL_UNICODEL(&zv, ZSTR_U(src->key), src->key_size - 1, 1);
				zend_make_printable_zval(&zv, &reszv, &usecopy);
				fprintf(stderr, "string:%s:\t\"", "dst->key");
				xc_dprint_str_len(Z_STRVAL(reszv), Z_STRLEN(reszv));
				fprintf(stderr, "\" len=%lu\n", (unsigned long) src->key_size - 1);
				if (usecopy) {
					zval_dtor(&reszv);
				}
				zval_dtor(&zv);
			} while (0);
#endif
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->key");
			xc_dprint_str_len(ZSTR_S(src->key), src->key_size - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->key_size - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->key");
			xc_dprint_str_len(ZSTR_S(src->key), src->key_size - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->key_size - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "ulong:h:\t%lu\n", src->h);
	
	
	
	
	
	


	

	
		
	
	assert(sizeof(xc_op_array_info_t) == sizeof(src->op_array_info));
	xc_dprint_indent(indent);fprintf(stderr, "xc_op_array_info_t:op_array_info ");
	
	
	
	

	
	assert(sizeof(xc_op_array_info_t) == sizeof((& src->op_array_info)[0]));
	

	

	xc_dprint_xc_op_array_info_t(
		           & src->op_array_info, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
	
	
	assert(sizeof(zend_function) == sizeof(src->func));
	xc_dprint_indent(indent);fprintf(stderr, "zend_function:func ");
	
	
	
	

	
	assert(sizeof(zend_function) == sizeof((& src->func)[0]));
	

	

	xc_dprint_zend_function(
		           & src->func, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	



		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dprint_xc_classinfo_t */
	
		/* export:  	  	 	void 	xc_dprint_xc_classinfo_t( 		const xc_classinfo_t * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dprint_xc_classinfo_t( 		const xc_classinfo_t * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
				
	assert(sizeof(zend_uint) == sizeof(src->key_size));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:key_size:\t%u\n", src->key_size);
	
	
	
	
	
	


	

#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uchar:type:\t%u\n", src->type);
	
	
	
	
	
	


	

#endif
	
		
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (src->type == IS_UNICODE) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->key) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
#ifdef IS_UNICODE
			do {
				zval zv;
				zval reszv;
				int usecopy;

				INIT_ZVAL(zv);
				ZVAL_UNICODEL(&zv, ZSTR_U(src->key), src->key_size - 1, 1);
				zend_make_printable_zval(&zv, &reszv, &usecopy);
				fprintf(stderr, "string:%s:\t\"", "dst->key");
				xc_dprint_str_len(Z_STRVAL(reszv), Z_STRLEN(reszv));
				fprintf(stderr, "\" len=%lu\n", (unsigned long) src->key_size - 1);
				if (usecopy) {
					zval_dtor(&reszv);
				}
				zval_dtor(&zv);
			} while (0);
#endif
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->key");
			xc_dprint_str_len(ZSTR_S(src->key), src->key_size - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->key_size - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->key");
			xc_dprint_str_len(ZSTR_S(src->key), src->key_size - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->key_size - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "ulong:h:\t%lu\n", src->h);
	
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->methodinfo_cnt));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:methodinfo_cnt:\t%u\n", src->methodinfo_cnt);
	
	
	
	
	
	


	

	
		
	if (src->methodinfos) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			

			for (i = 0;
					i < src->methodinfo_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_op_array_info_t) == sizeof(src->methodinfos[i]));
	xc_dprint_indent(indent);fprintf(stderr, "xc_op_array_info_t:methodinfos[i] ");
	
	
	
	

	
	assert(sizeof(xc_op_array_info_t) == sizeof((& src->methodinfos[i])[0]));
	

	

	xc_dprint_xc_op_array_info_t(
		           & src->methodinfos[i], indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->methodinfos == NULL);

	
	
	


	}

	
	
#ifdef ZEND_ENGINE_2
	
	
	if (src->cest) {
		xc_dprint_indent(indent);fprintf(stderr, "zend_class_entry:cest ");
		
	
	
	

	
	assert(sizeof(zend_class_entry) == sizeof(( src->cest)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	

	xc_dprint_zend_class_entry(
		            src->cest, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		xc_dprint_indent(indent);fprintf(stderr, "zend_class_entry:cest:\tNULL\n");
		
	
	
	assert(src->cest == NULL);

	}
	
	
	
	


#else
	
	
	assert(sizeof(zend_class_entry) == sizeof(src->cest));
	xc_dprint_indent(indent);fprintf(stderr, "zend_class_entry:cest ");
	
	
	
	

	
	assert(sizeof(zend_class_entry) == sizeof((& src->cest)[0]));
	

	

	xc_dprint_zend_class_entry(
		           & src->cest, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


#endif
#ifndef ZEND_COMPILE_DELAYED_BINDING
		
	assert(sizeof(int) == sizeof(src->oplineno));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:oplineno:\t%d\n", src->oplineno);
	
	
	
	
	
	


	

#endif

		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


#ifdef ZEND_ENGINE_2_1

	
	

/* {{{ xc_dprint_xc_autoglobal_t */
	
 	  	 	static void inline 	xc_dprint_xc_autoglobal_t( 		const xc_autoglobal_t * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
				
	assert(sizeof(zend_uint) == sizeof(src->key_len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:key_len:\t%u\n", src->key_len);
	
	
	
	
	
	


	

#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uchar:type:\t%u\n", src->type);
	
	
	
	
	
	


	

#endif
	
		 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (src->type == IS_UNICODE) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->key) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
#ifdef IS_UNICODE
			do {
				zval zv;
				zval reszv;
				int usecopy;

				INIT_ZVAL(zv);
				ZVAL_UNICODEL(&zv, ZSTR_U(src->key), src->key_len + 1 - 1, 1);
				zend_make_printable_zval(&zv, &reszv, &usecopy);
				fprintf(stderr, "string:%s:\t\"", "dst->key");
				xc_dprint_str_len(Z_STRVAL(reszv), Z_STRLEN(reszv));
				fprintf(stderr, "\" len=%lu\n", (unsigned long) src->key_len + 1 - 1);
				if (usecopy) {
					zval_dtor(&reszv);
				}
				zval_dtor(&zv);
			} while (0);
#endif
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->key");
			xc_dprint_str_len(ZSTR_S(src->key), src->key_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->key_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->key");
			xc_dprint_str_len(ZSTR_S(src->key), src->key_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->key_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "ulong:h:\t%lu\n", src->h);
	
	
	
	
	
	


	


		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif


#ifdef XCACHE_ERROR_CACHING

	
	

/* {{{ xc_dprint_xc_compilererror_t */
	
 	  	 	static void inline 	xc_dprint_xc_compilererror_t( 		const xc_compilererror_t * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
				
	assert(sizeof(int) == sizeof(src->type));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:type:\t%d\n", src->type);
	
	
	
	
	
	


	

		
	assert(sizeof(uint) == sizeof(src->lineno));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "uint:lineno:\t%u\n", src->lineno);
	
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->error_len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:error_len:\t%d\n", src->error_len);
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->error == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->error");
			xc_dprint_str_len(src->error, src->error_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->error_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	


		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif


	
	

/* {{{ xc_dprint_xc_entry_data_php_t */
	
		/* export:  	  	 	void 	xc_dprint_xc_entry_data_php_t( 		const xc_entry_data_php_t * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dprint_xc_entry_data_php_t( 		const xc_entry_data_php_t * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
			

	/* skip */
	
	
	
	

		
	assert(sizeof(xc_hash_value_t) == sizeof(src->hvalue));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "xc_hash_value_t:hvalue:\t%lu\n", src->hvalue);
	
	
	
	
	
	


	

		
	assert(sizeof(xc_md5sum_t) == sizeof(src->md5));
	
	
	
	

	

		
	assert(sizeof(zend_ulong) == sizeof(src->refcount));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_ulong:refcount:\t%lu\n", src->refcount);
	
	
	
	
	
	


	


		
	assert(sizeof(zend_ulong) == sizeof(src->hits));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_ulong:hits:\t%lu\n", src->hits);
	
	
	
	
	
	


	

		
	assert(sizeof(size_t) == sizeof(src->size));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "size_t:size:\t%lu\n", src->size);
	
	
	
	
	
	


	


	
		
	
	assert(sizeof(xc_op_array_info_t) == sizeof(src->op_array_info));
	xc_dprint_indent(indent);fprintf(stderr, "xc_op_array_info_t:op_array_info ");
	
	
	
	

	
	assert(sizeof(xc_op_array_info_t) == sizeof((& src->op_array_info)[0]));
	

	

	xc_dprint_xc_op_array_info_t(
		           & src->op_array_info, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
	
	
	if (src->op_array) {
		xc_dprint_indent(indent);fprintf(stderr, "zend_op_array:op_array ");
		
	
	
	

	
	assert(sizeof(zend_op_array) == sizeof(( src->op_array)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	

	xc_dprint_zend_op_array(
		            src->op_array, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		xc_dprint_indent(indent);fprintf(stderr, "zend_op_array:op_array:\tNULL\n");
		
	
	
	assert(src->op_array == NULL);

	}
	
	
	
	



#ifdef HAVE_XCACHE_CONSTANT
		
	assert(sizeof(zend_uint) == sizeof(src->constinfo_cnt));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:constinfo_cnt:\t%u\n", src->constinfo_cnt);
	
	
	
	
	
	


	

	
	if (src->constinfos) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			

			for (i = 0;
					i < src->constinfo_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_constinfo_t) == sizeof(src->constinfos[i]));
	xc_dprint_indent(indent);fprintf(stderr, "xc_constinfo_t:constinfos[i] ");
	
	
	
	

	
	assert(sizeof(xc_constinfo_t) == sizeof((& src->constinfos[i])[0]));
	

	

	xc_dprint_xc_constinfo_t(
		           & src->constinfos[i], indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->constinfos == NULL);

	
	
	


	}

#endif

		
	assert(sizeof(zend_uint) == sizeof(src->funcinfo_cnt));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:funcinfo_cnt:\t%u\n", src->funcinfo_cnt);
	
	
	
	
	
	


	

	
	if (src->funcinfos) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			

			for (i = 0;
					i < src->funcinfo_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_funcinfo_t) == sizeof(src->funcinfos[i]));
	xc_dprint_indent(indent);fprintf(stderr, "xc_funcinfo_t:funcinfos[i] ");
	
	
	
	

	
	assert(sizeof(xc_funcinfo_t) == sizeof((& src->funcinfos[i])[0]));
	

	

	xc_dprint_xc_funcinfo_t(
		           & src->funcinfos[i], indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->funcinfos == NULL);

	
	
	


	}


		
	assert(sizeof(zend_uint) == sizeof(src->classinfo_cnt));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:classinfo_cnt:\t%u\n", src->classinfo_cnt);
	
	
	
	
	
	


	

	
	if (src->classinfos) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			

			for (i = 0;
					i < src->classinfo_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_classinfo_t) == sizeof(src->classinfos[i]));
	xc_dprint_indent(indent);fprintf(stderr, "xc_classinfo_t:classinfos[i] ");
	
	
	
	

	
	assert(sizeof(xc_classinfo_t) == sizeof((& src->classinfos[i])[0]));
	

	

	xc_dprint_xc_classinfo_t(
		           & src->classinfos[i], indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->classinfos == NULL);

	
	
	


	}

#ifdef ZEND_ENGINE_2_1
		
	assert(sizeof(zend_uint) == sizeof(src->autoglobal_cnt));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:autoglobal_cnt:\t%u\n", src->autoglobal_cnt);
	
	
	
	
	
	


	

	
		
	if (src->autoglobals) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			

			for (i = 0;
					i < src->autoglobal_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_autoglobal_t) == sizeof(src->autoglobals[i]));
	xc_dprint_indent(indent);fprintf(stderr, "xc_autoglobal_t:autoglobals[i] ");
	
	
	
	

	
	assert(sizeof(xc_autoglobal_t) == sizeof((& src->autoglobals[i])[0]));
	

	

	xc_dprint_xc_autoglobal_t(
		           & src->autoglobals[i], indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->autoglobals == NULL);

	
	
	


	}

	
#endif
#ifdef XCACHE_ERROR_CACHING
		
	assert(sizeof(zend_uint) == sizeof(src->compilererror_cnt));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uint:compilererror_cnt:\t%u\n", src->compilererror_cnt);
	
	
	
	
	
	


	

	
		
	if (src->compilererrors) {
		zend_uint i; 
		
	

		
						
			
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

			

			for (i = 0;
					i < src->compilererror_cnt;
					++i) {
				
	
	

					
	
	assert(sizeof(xc_compilererror_t) == sizeof(src->compilererrors[i]));
	xc_dprint_indent(indent);fprintf(stderr, "xc_compilererror_t:compilererrors[i] ");
	
	
	
	

	
	assert(sizeof(xc_compilererror_t) == sizeof((& src->compilererrors[i])[0]));
	

	

	xc_dprint_xc_compilererror_t(
		           & src->compilererrors[i], indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	

				
	
	

			}
						
				
		
	
	
	

		
		
	}
	else {
		
	
	
	
	assert(src->compilererrors == NULL);

	
	
	


	}

	
#endif
#ifndef ZEND_COMPILE_DELAYED_BINDING
		
	assert(sizeof(zend_bool) == sizeof(src->have_early_binding));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:have_early_binding:\t%u\n", src->have_early_binding);
	
	
	
	
	
	


	

#endif
		
	assert(sizeof(zend_bool) == sizeof(src->have_references));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:have_references:\t%u\n", src->have_references);
	
	
	
	
	
	


	


		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dprint_xc_entry_t */
	
 	  	 	static void inline 	xc_dprint_xc_entry_t( 		const xc_entry_t * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
			/* skip */
	
	
	
	

		
	assert(sizeof(size_t) == sizeof(src->size));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "size_t:size:\t%lu\n", src->size);
	
	
	
	
	
	


	


		
	assert(sizeof(time_t) == sizeof(src->ctime));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "time_t:ctime:\t%ld\n", src->ctime);
	
	
	
	
	
	


	

		
	assert(sizeof(time_t) == sizeof(src->atime));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "time_t:atime:\t%ld\n", src->atime);
	
	
	
	
	
	


	

		
	assert(sizeof(time_t) == sizeof(src->dtime));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "time_t:dtime:\t%ld\n", src->dtime);
	
	
	
	
	
	


	

		
	assert(sizeof(long) == sizeof(src->ttl));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "long:ttl:\t%ld\n", src->ttl);
	
	
	
	
	
	


	

		
	assert(sizeof(zend_ulong) == sizeof(src->hits));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_ulong:hits:\t%lu\n", src->hits);
	
	
	
	
	
	


	

	
	
	
	
 
		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dprint_xc_entry_php_t */
	
		/* export:  	  	 	void 	xc_dprint_xc_entry_php_t( 		const xc_entry_php_t * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dprint_xc_entry_php_t( 		const xc_entry_php_t * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
			
	
	assert(sizeof(xc_entry_t) == sizeof(src->entry));
	xc_dprint_indent(indent);fprintf(stderr, "xc_entry_t:entry ");
	
	
	
	

	
	assert(sizeof(xc_entry_t) == sizeof((& src->entry)[0]));
	

	

	xc_dprint_xc_entry_t(
		           & src->entry, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	


	
	
	

			
	assert(sizeof(int) == sizeof(src->entry.name.str.len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:entry.name.str.len:\t%d\n", src->entry.name.str.len);
	
	
	

	

		
			  
	
	
	
	
	
	
	
	
	if (src->entry.name.str.val == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->entry.name.str.val");
			xc_dprint_str_len(src->entry.name.str.val, src->entry.name.str.len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->entry.name.str.len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		
	
	
	


	
	
	if (src->php) {
		xc_dprint_indent(indent);fprintf(stderr, "xc_entry_data_php_t:php ");
		
	
	
	

	
	assert(sizeof(xc_entry_data_php_t) == sizeof(( src->php)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	

	xc_dprint_xc_entry_data_php_t(
		            src->php, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	}
	else {
		xc_dprint_indent(indent);fprintf(stderr, "xc_entry_data_php_t:php:\tNULL\n");
		
	
	
	assert(src->php == NULL);

	}
	
	
	
	



		
	assert(sizeof(long) == sizeof(src->refcount));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "long:refcount:\t%ld\n", src->refcount);
	
	
	
	
	
	


	

		
	assert(sizeof(time_t) == sizeof(src->file_mtime));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "time_t:file_mtime:\t%ld\n", src->file_mtime);
	
	
	
	
	
	


	

		
	assert(sizeof(size_t) == sizeof(src->file_size));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "size_t:file_size:\t%lu\n", src->file_size);
	
	
	
	
	
	


	

		
	assert(sizeof(size_t) == sizeof(src->file_device));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "size_t:file_device:\t%lu\n", src->file_device);
	
	
	
	
	
	


	

		
	assert(sizeof(size_t) == sizeof(src->file_inode));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "size_t:file_inode:\t%lu\n", src->file_inode);
	
	
	
	
	
	


	


		
	assert(sizeof(size_t) == sizeof(src->filepath_len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "size_t:filepath_len:\t%lu\n", src->filepath_len);
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->filepath == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->filepath");
			xc_dprint_str_len(src->filepath, src->filepath_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->filepath_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		
	assert(sizeof(size_t) == sizeof(src->dirpath_len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "size_t:dirpath_len:\t%lu\n", src->dirpath_len);
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->dirpath == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->dirpath");
			xc_dprint_str_len(src->dirpath, src->dirpath_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->dirpath_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

#ifdef IS_UNICODE
		
	assert(sizeof(int) == sizeof(src->ufilepath_len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:ufilepath_len:\t%d\n", src->ufilepath_len);
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->ufilepath == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->ufilepath");
			xc_dprint_str_len(src->ufilepath, src->ufilepath_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->ufilepath_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		
	assert(sizeof(int) == sizeof(src->udirpath_len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:udirpath_len:\t%d\n", src->udirpath_len);
	
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->udirpath == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->udirpath");
			xc_dprint_str_len(src->udirpath, src->udirpath_len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->udirpath_len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

#endif

		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dprint_xc_entry_var_t */
	
		/* export:  	  	 	void 	xc_dprint_xc_entry_var_t( 		const xc_entry_var_t * const src, int indent 		 		 		 		 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dprint_xc_entry_var_t( 		const xc_entry_var_t * const src, int indent 		 		 		 		 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			fprintf(stderr, "%s", "{\n");
			indent ++;
			{
		
			
	
	assert(sizeof(xc_entry_t) == sizeof(src->entry));
	xc_dprint_indent(indent);fprintf(stderr, "xc_entry_t:entry ");
	
	
	
	

	
	assert(sizeof(xc_entry_t) == sizeof((& src->entry)[0]));
	

	

	xc_dprint_xc_entry_t(
		           & src->entry, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

	
	
	
	



#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->name_type));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_uchar:name_type:\t%u\n", src->name_type);
	
	
	
	
	
	


	

#endif
		
	
	

#ifdef IS_UNICODE
		if (src->name_type == IS_UNICODE) {
				
	assert(sizeof(int32_t) == sizeof(src->entry.name.ustr.len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int32_t:entry.name.ustr.len:\t%d\n", src->entry.name.ustr.len);
	
	
	

	

		}
		else {
				
	assert(sizeof(int) == sizeof(src->entry.name.str.len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:entry.name.str.len:\t%d\n", src->entry.name.str.len);
	
	
	

	

		}
#else
			
	assert(sizeof(int) == sizeof(src->entry.name.str.len));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "int:entry.name.str.len:\t%d\n", src->entry.name.str.len);
	
	
	

	

#endif
		
#ifdef IS_UNICODE
			 
	
#ifdef IS_UNICODE
	
	
	
		if (src->name_type == IS_UNICODE) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->entry.name.uni.val) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
#ifdef IS_UNICODE
			do {
				zval zv;
				zval reszv;
				int usecopy;

				INIT_ZVAL(zv);
				ZVAL_UNICODEL(&zv, ZSTR_U(src->entry.name.uni.val), src->entry.name.uni.len + 1 - 1, 1);
				zend_make_printable_zval(&zv, &reszv, &usecopy);
				fprintf(stderr, "string:%s:\t\"", "dst->entry.name.uni.val");
				xc_dprint_str_len(Z_STRVAL(reszv), Z_STRLEN(reszv));
				fprintf(stderr, "\" len=%lu\n", (unsigned long) src->entry.name.uni.len + 1 - 1);
				if (usecopy) {
					zval_dtor(&reszv);
				}
				zval_dtor(&zv);
			} while (0);
#endif
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->entry.name.uni.val) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->entry.name.uni.val");
			xc_dprint_str_len(ZSTR_S(src->entry.name.uni.val), src->entry.name.uni.len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->entry.name.uni.len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->entry.name.uni.val) == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->entry.name.uni.val");
			xc_dprint_str_len(ZSTR_S(src->entry.name.uni.val), src->entry.name.uni.len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->entry.name.uni.len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
	

#else
			  
	
	
	
	
	
	
	
	
	if (src->entry.name.str.val == NULL) {
		
		
	}
	else {
		xc_dprint_indent(indent);
			
			fprintf(stderr, "string:%s:\t\"", "dst->entry.name.str.val");
			xc_dprint_str_len(src->entry.name.str.val, src->entry.name.str.len + 1 - 1);
			fprintf(stderr, "\" len=%lu\n", (unsigned long) src->entry.name.str.len + 1 - 1);
			
		
		
		
		
		
		
	}
	
	
	
	
	
	
	

#endif
		
	
	
	

	
	xc_dprint_indent(indent);fprintf(stderr, "zval:value");
	
	
	
	

	
	assert(sizeof(zval_ptr) == sizeof((& src->value)[0]));
	

	

	xc_dprint_zval_ptr(
		           & src->value, indent
		
		
		
		
		TSRMLS_CC
	);

	

	
	

		
	assert(sizeof(zend_bool) == sizeof(src->have_references));
		
	
		xc_dprint_indent(indent);
		fprintf(stderr, "zend_bool:have_references:\t%u\n", src->have_references);
	
	
	
	
	
	


	

	
	
	
	


		
			}
			indent --;
			xc_dprint_indent(indent);fprintf(stderr, "}\n");
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


#endif /* HAVE_XCACHE_DPRINT */
#ifdef HAVE_XCACHE_DISASSEMBLER
  /* {{{ Pre-declare */
 	  	 	void 	xc_dasm_zval( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zval * const src 		TSRMLS_DC 	); 	
 	  	 	static void inline 	xc_dasm_zval_ptr( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zval_ptr * const src 		TSRMLS_DC 	); 	
 	  	 	static void inline 	xc_dasm_zval_ptr_nullable( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zval_ptr_nullable * const src 		TSRMLS_DC 	); 	
 	  	 	void 	xc_dasm_zend_op_array( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_op_array * const src 		TSRMLS_DC 	); 	
 	  	 	void 	xc_dasm_zend_class_entry( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_class_entry * const src 		TSRMLS_DC 	); 	
#ifdef HAVE_XCACHE_CONSTANT
 	  	 	static void inline 	xc_dasm_zend_constant( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_constant * const src 		TSRMLS_DC 	); 	
#endif
 	  	 	void 	xc_dasm_zend_function( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_function * const src 		TSRMLS_DC 	); 	
 	  	 	void 	xc_dasm_xc_entry_var_t( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const xc_entry_var_t * const src 		TSRMLS_DC 	); 	
 	  	 	void 	xc_dasm_xc_entry_php_t( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const xc_entry_php_t * const src 		TSRMLS_DC 	); 	
#ifdef ZEND_ENGINE_2
 	  	 	static void inline 	xc_dasm_zend_property_info( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_property_info * const src 		TSRMLS_DC 	); 	
#endif
/* }}} */
#ifdef IS_CV

	
	

/* {{{ xc_dasm_zend_compiled_variable */
	
 	  	 	static void inline 	xc_dasm_zend_compiled_variable( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_compiled_variable * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(int) == sizeof(src->name_len));
		
	
	
		add_assoc_long_ex(dst, ("name_len"), (sizeof("name_len")), src->name_len);
		
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
			add_assoc_null_ex(dst, ("name"), (sizeof("name")));
		
	}
	else {
		
		
		
		
		
		
			
				add_assoc_unicodel_ex(dst, ("name"), (sizeof("name")), ZSTR_U(src->name), src->name_len + 1-1, 1);
				
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
			add_assoc_null_ex(dst, ("name"), (sizeof("name")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("name"), (sizeof("name")), (char *) ZSTR_S(src->name), src->name_len + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
			add_assoc_null_ex(dst, ("name"), (sizeof("name")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("name"), (sizeof("name")), (char *) ZSTR_S(src->name), src->name_len + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

#endif
	

		
	assert(sizeof(ulong) == sizeof(src->hash_value));
		
	
	
		add_assoc_long_ex(dst, ("hash_value"), (sizeof("hash_value")), src->hash_value);
		
	
	
	
	
	


	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_dasm_zend_uint */
	
 	  	 	static void inline 	xc_dasm_zend_uint( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_uint * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
	
	

		
		
		
		
		
		
	}
/* }}} */
	

#ifndef ZEND_ENGINE_2

	
	

/* {{{ xc_dasm_int */
	
 	  	 	static void inline 	xc_dasm_int( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const int * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
	
	

		
		
		
		
		
		
	}
/* }}} */
	

#endif
#ifdef ZEND_ENGINE_2

	
	

/* {{{ xc_dasm_zend_try_catch_element */
	
 	  	 	static void inline 	xc_dasm_zend_try_catch_element( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_try_catch_element * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(zend_uint) == sizeof(src->try_op));
		
	
	
		add_assoc_long_ex(dst, ("try_op"), (sizeof("try_op")), src->try_op);
		
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->catch_op));
		
	
	
		add_assoc_long_ex(dst, ("catch_op"), (sizeof("catch_op")), src->catch_op);
		
	
	
	
	
	


	

#ifdef ZEND_ENGINE_2_5
		
	assert(sizeof(zend_uint) == sizeof(src->finally_op));
		
	
	
		add_assoc_long_ex(dst, ("finally_op"), (sizeof("finally_op")), src->finally_op);
		
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->finally_end));
		
	
	
		add_assoc_long_ex(dst, ("finally_end"), (sizeof("finally_end")), src->finally_end);
		
	
	
	
	
	


	

#endif

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_dasm_zend_brk_cont_element */
	
 	  	 	static void inline 	xc_dasm_zend_brk_cont_element( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_brk_cont_element * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
		#ifdef ZEND_ENGINE_2_2
		
	assert(sizeof(int) == sizeof(src->start));
		
	
	
		add_assoc_long_ex(dst, ("start"), (sizeof("start")), src->start);
		
	
	
	
	
	


	

#endif
		
	assert(sizeof(int) == sizeof(src->cont));
		
	
	
		add_assoc_long_ex(dst, ("cont"), (sizeof("cont")), src->cont);
		
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->brk));
		
	
	
		add_assoc_long_ex(dst, ("brk"), (sizeof("brk")), src->brk);
		
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->parent));
		
	
	
		add_assoc_long_ex(dst, ("parent"), (sizeof("parent")), src->parent);
		
	
	
	
	
	


	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	
	

/* {{{ xc_dasm_HashTable_zval_ptr */
	
 	  	 	static void inline 	xc_dasm_HashTable_zval_ptr( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const HashTable * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
		
		
	

				
			const Bucket *srcBucket;
			zval *zv;
			int bufsize = 2;
			char *buf = emalloc(bufsize);
			int keysize;

#if defined(HARDENING_PATCH_HASH_PROTECT) && HARDENING_PATCH_HASH_PROTECT
			
	
	
	

#endif
			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

#if ZEND_DEBUG
			
	
	
	

#endif
#ifdef IS_UNICODE
			
	
	
	

#endif

			
	
	

			for (srcBucket = src->pListHead; srcBucket != NULL; srcBucket = srcBucket->pListNext) {
				ALLOC_INIT_ZVAL(zv);
				array_init(zv);
				xc_dasm_zval_ptr (dasm, zv, ((zval_ptr*)srcBucket->pData) TSRMLS_CC);
				keysize = BUCKET_KEY_SIZE(srcBucket) + 2;
				if (keysize > bufsize) {
					do {
						bufsize *= 2;
					} while (keysize > bufsize);
					buf = erealloc(buf, bufsize);
				}
				memcpy(buf, BUCKET_KEY_S(srcBucket), keysize);
				buf[keysize - 2] = buf[keysize - 1] = ""[0];
				keysize = srcBucket->nKeyLength;
#ifdef IS_UNICODE
				if (BUCKET_KEY_TYPE(srcBucket) == IS_UNICODE) {
					if (buf[0] == ""[0] && buf[1] == ""[0]) {
						keysize ++;
					}
				} else
#endif
				{
					if (buf[0] == ""[0]) {
						keysize ++;
					}
				}
				add_u_assoc_zval_ex(dst, BUCKET_KEY_TYPE(srcBucket), ZSTR(buf), keysize, zv);
			}
			
	
	


			efree(buf);
				
	
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	



	
	
	

/* {{{ xc_dasm_HashTable_zend_function */
	
		/* export:  	  	 	void 	xc_dasm_HashTable_zend_function( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const HashTable * const src 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dasm_HashTable_zend_function( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const HashTable * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
		
		
	

				
			const Bucket *srcBucket;
			zval *zv;
			int bufsize = 2;
			char *buf = emalloc(bufsize);
			int keysize;

#if defined(HARDENING_PATCH_HASH_PROTECT) && HARDENING_PATCH_HASH_PROTECT
			
	
	
	

#endif
			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

#if ZEND_DEBUG
			
	
	
	

#endif
#ifdef IS_UNICODE
			
	
	
	

#endif

			
	
	

			for (srcBucket = src->pListHead; srcBucket != NULL; srcBucket = srcBucket->pListNext) {
				ALLOC_INIT_ZVAL(zv);
				array_init(zv);
				xc_dasm_zend_function (dasm, zv, ((zend_function*)srcBucket->pData) TSRMLS_CC);
				keysize = BUCKET_KEY_SIZE(srcBucket) + 2;
				if (keysize > bufsize) {
					do {
						bufsize *= 2;
					} while (keysize > bufsize);
					buf = erealloc(buf, bufsize);
				}
				memcpy(buf, BUCKET_KEY_S(srcBucket), keysize);
				buf[keysize - 2] = buf[keysize - 1] = ""[0];
				keysize = srcBucket->nKeyLength;
#ifdef IS_UNICODE
				if (BUCKET_KEY_TYPE(srcBucket) == IS_UNICODE) {
					if (buf[0] == ""[0] && buf[1] == ""[0]) {
						keysize ++;
					}
				} else
#endif
				{
					if (buf[0] == ""[0]) {
						keysize ++;
					}
				}
				add_u_assoc_zval_ex(dst, BUCKET_KEY_TYPE(srcBucket), ZSTR(buf), keysize, zv);
			}
			
	
	


			efree(buf);
				
	
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


#ifdef ZEND_ENGINE_2

	
	
	

/* {{{ xc_dasm_HashTable_zend_property_info */
	
 	  	 	static void inline 	xc_dasm_HashTable_zend_property_info( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const HashTable * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
		
		
	

				
			const Bucket *srcBucket;
			zval *zv;
			int bufsize = 2;
			char *buf = emalloc(bufsize);
			int keysize;

#if defined(HARDENING_PATCH_HASH_PROTECT) && HARDENING_PATCH_HASH_PROTECT
			
	
	
	

#endif
			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

			
	
	
	

#if ZEND_DEBUG
			
	
	
	

#endif
#ifdef IS_UNICODE
			
	
	
	

#endif

			
	
	

			for (srcBucket = src->pListHead; srcBucket != NULL; srcBucket = srcBucket->pListNext) {
				ALLOC_INIT_ZVAL(zv);
				array_init(zv);
				xc_dasm_zend_property_info (dasm, zv, ((zend_property_info*)srcBucket->pData) TSRMLS_CC);
				keysize = BUCKET_KEY_SIZE(srcBucket) + 2;
				if (keysize > bufsize) {
					do {
						bufsize *= 2;
					} while (keysize > bufsize);
					buf = erealloc(buf, bufsize);
				}
				memcpy(buf, BUCKET_KEY_S(srcBucket), keysize);
				buf[keysize - 2] = buf[keysize - 1] = ""[0];
				keysize = srcBucket->nKeyLength;
#ifdef IS_UNICODE
				if (BUCKET_KEY_TYPE(srcBucket) == IS_UNICODE) {
					if (buf[0] == ""[0] && buf[1] == ""[0]) {
						keysize ++;
					}
				} else
#endif
				{
					if (buf[0] == ""[0]) {
						keysize ++;
					}
				}
				add_u_assoc_zval_ex(dst, BUCKET_KEY_TYPE(srcBucket), ZSTR(buf), keysize, zv);
			}
			
	
	


			efree(buf);
				
	
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


#endif
#ifdef IS_CONSTANT_AST


	
	

/* {{{ xc_dasm_zend_ast */
	
		/* export:  	  	 	void 	xc_dasm_zend_ast( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_ast * const src 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dasm_zend_ast( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_ast * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				zend_ushort i;
			
	assert(sizeof(zend_ushort) == sizeof(src->kind));
		
	
	
		add_assoc_long_ex(dst, ("kind"), (sizeof("kind")), src->kind);
		
	
	
	
	
	


	

			
	assert(sizeof(zend_ushort) == sizeof(src->children));
		
	
	
		add_assoc_long_ex(dst, ("children"), (sizeof("children")), src->children);
		
	
	
	
	
	


	

		
	
	
	

		
	
	

			if (src->kind == ZEND_CONST) {
				assert(src->u.val);
				
				
	
	
	

	
	assert(sizeof(zval) == sizeof((  src->u.val)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_zval(
		
		
		
		
		dasm, zv,   src->u.val
		TSRMLS_CC
	);

	
		
			add_next_index_zval(dst, zv);
		
	} while (0);
	

	
	

				
			}
			else {
				for (i = 0; i < src->children; ++i) {
					zend_ast *src_ast = (&src->u.child)[i];
					if (src_ast) {
						{
	

	
	
							
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

							
	
	
	

	
	assert(sizeof(zend_ast) == sizeof((  src_ast)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_zend_ast(
		
		
		
		
		dasm, zv,   src_ast
		TSRMLS_CC
	);

	
		
			add_next_index_zval(dst, zv);
		
	} while (0);
	

	
	

						
	
}

						
					}
					else {
						
					}
				}
			}
		
	
	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_dasm_zval */
	
		/* export:  	  	 	void 	xc_dasm_zval( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zval * const src 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dasm_zval( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zval * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			do {
		zval_dtor(dst);
		*dst = *src;
		zval_copy_ctor(dst);
		Z_SET_REFCOUNT(*dst, 1);
		
	
	
	

		
	
	
	

#ifdef ZEND_ENGINE_2_3
		
	
	
	

		
	
	
	

#else
		
	
	
	

		
	
	
	

#endif
	} while(0);
	
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dasm_zval_ptr */
	
 	  	 	static void inline 	xc_dasm_zval_ptr( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zval_ptr * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
		
	

		xc_dasm_zval (dasm, dst, src[0] TSRMLS_CC);
		
	
	

		
		
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dasm_zval_ptr_nullable */
	
 	  	 	static void inline 	xc_dasm_zval_ptr_nullable( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zval_ptr_nullable * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			if (src[0]) {
		
		
	
	
	

	
	assert(sizeof(zval_ptr) == sizeof((  src)[0]));
	

	xc_dasm_zval_ptr(
		
		
		
		
		dasm, dst,   src
		TSRMLS_CC
	);

	
	

		
	}
	else {
		
	}
	

		
		
		
		
		
		
	}
/* }}} */
	

#ifdef ZEND_ENGINE_2

	
	

/* {{{ xc_dasm_zend_arg_info */
	
 	  	 	static void inline 	xc_dasm_zend_arg_info( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_arg_info * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(zend_uint) == sizeof(src->name_len));
		
	
	
		add_assoc_long_ex(dst, ("name_len"), (sizeof("name_len")), src->name_len);
		
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
			add_assoc_null_ex(dst, ("name"), (sizeof("name")));
		
	}
	else {
		
		
		
		
		
		
			
				add_assoc_unicodel_ex(dst, ("name"), (sizeof("name")), ZSTR_U(src->name), src->name_len + 1-1, 1);
				
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
			add_assoc_null_ex(dst, ("name"), (sizeof("name")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("name"), (sizeof("name")), (char *) ZSTR_S(src->name), src->name_len + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
			add_assoc_null_ex(dst, ("name"), (sizeof("name")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("name"), (sizeof("name")), (char *) ZSTR_S(src->name), src->name_len + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

#endif
	

		
	assert(sizeof(zend_uint) == sizeof(src->class_name_len));
		
	
	
		add_assoc_long_ex(dst, ("class_name_len"), (sizeof("class_name_len")), src->class_name_len);
		
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->class_name) == NULL) {
		
		
			add_assoc_null_ex(dst, ("class_name"), (sizeof("class_name")));
		
	}
	else {
		
		
		
		
		
		
			
				add_assoc_unicodel_ex(dst, ("class_name"), (sizeof("class_name")), ZSTR_U(src->class_name), src->class_name_len + 1-1, 1);
				
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->class_name) == NULL) {
		
		
			add_assoc_null_ex(dst, ("class_name"), (sizeof("class_name")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("class_name"), (sizeof("class_name")), (char *) ZSTR_S(src->class_name), src->class_name_len + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->class_name) == NULL) {
		
		
			add_assoc_null_ex(dst, ("class_name"), (sizeof("class_name")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("class_name"), (sizeof("class_name")), (char *) ZSTR_S(src->class_name), src->class_name_len + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

#endif
	

#ifdef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uchar) == sizeof(src->type_hint));
		
	
	
		add_assoc_long_ex(dst, ("type_hint"), (sizeof("type_hint")), src->type_hint);
		
	
	
	
	
	


	

#elif defined(ZEND_ENGINE_2_1)
		
	assert(sizeof(zend_bool) == sizeof(src->array_type_hint));
		
	
	
		add_assoc_bool_ex(dst, ("array_type_hint"), (sizeof("array_type_hint")), src->array_type_hint ? 1 : 0);
		
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_6
		
	assert(sizeof(zend_uchar) == sizeof(src->pass_by_reference));
		
	
	
		add_assoc_long_ex(dst, ("pass_by_reference"), (sizeof("pass_by_reference")), src->pass_by_reference);
		
	
	
	
	
	


	

#endif
		
	assert(sizeof(zend_bool) == sizeof(src->allow_null));
		
	
	
		add_assoc_bool_ex(dst, ("allow_null"), (sizeof("allow_null")), src->allow_null ? 1 : 0);
		
	
	
	
	
	


	


#ifdef ZEND_ENGINE_2_6
		
	assert(sizeof(zend_bool) == sizeof(src->is_variadic));
		
	
	
		add_assoc_bool_ex(dst, ("is_variadic"), (sizeof("is_variadic")), src->is_variadic ? 1 : 0);
		
	
	
	
	
	


	

#else
		
	assert(sizeof(zend_bool) == sizeof(src->pass_by_reference));
		
	
	
		add_assoc_bool_ex(dst, ("pass_by_reference"), (sizeof("pass_by_reference")), src->pass_by_reference ? 1 : 0);
		
	
	
	
	
	


	

#endif

#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_bool) == sizeof(src->return_reference));
		
	
	
		add_assoc_bool_ex(dst, ("return_reference"), (sizeof("return_reference")), src->return_reference ? 1 : 0);
		
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->required_num_args));
		
	
	
		add_assoc_long_ex(dst, ("required_num_args"), (sizeof("required_num_args")), src->required_num_args);
		
	
	
	
	
	


	

#endif

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif
#ifdef HAVE_XCACHE_CONSTANT

	
	

/* {{{ xc_dasm_zend_constant */
	
 	  	 	static void inline 	xc_dasm_zend_constant( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_constant * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
	
	assert(sizeof(zval) == sizeof(src->value));
	
	
	
	
	

	
	assert(sizeof(zval) == sizeof((& src->value)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_zval(
		
		
		
		
		dasm, zv, & src->value
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("value"), (sizeof("value")), zv);
		
	} while (0);
	

	
	

	
	
	
	


		
	assert(sizeof(int) == sizeof(src->flags));
		
	
	
		add_assoc_long_ex(dst, ("flags"), (sizeof("flags")), src->flags);
		
	
	
	
	
	


	

		
	assert(sizeof(uint) == sizeof(src->name_len));
		
	
	
		add_assoc_long_ex(dst, ("name_len"), (sizeof("name_len")), src->name_len);
		
	
	
	
	
	


	

	
	
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
			add_assoc_null_ex(dst, ("name"), (sizeof("name")));
		
	}
	else {
		
		
		
		
		
		
			
				add_assoc_unicodel_ex(dst, ("name"), (sizeof("name")), ZSTR_U(src->name), src->name_len-1, 1);
				
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
			add_assoc_null_ex(dst, ("name"), (sizeof("name")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("name"), (sizeof("name")), (char *) ZSTR_S(src->name), src->name_len-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
			add_assoc_null_ex(dst, ("name"), (sizeof("name")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("name"), (sizeof("name")), (char *) ZSTR_S(src->name), src->name_len-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	assert(sizeof(int) == sizeof(src->module_number));
		
	
	
		add_assoc_long_ex(dst, ("module_number"), (sizeof("module_number")), src->module_number);
		
	
	
	
	
	


	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_dasm_zend_function */
	
		/* export:  	  	 	void 	xc_dasm_zend_function( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_function * const src 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dasm_zend_function( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_function * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
	
	

	switch (src->type) {
	case ZEND_INTERNAL_FUNCTION:
	case ZEND_OVERLOADED_FUNCTION:
		
		break;

	case ZEND_USER_FUNCTION:
	case ZEND_EVAL_CODE:
		
		
	
	assert(sizeof(zend_op_array) == sizeof(src->op_array));
	
	
	
	
	

	
	assert(sizeof(zend_op_array) == sizeof((& src->op_array)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_zend_op_array(
		
		
		
		
		dasm, zv, & src->op_array
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("op_array"), (sizeof("op_array")), zv);
		
	} while (0);
	

	
	

	

		break;

	default:
		assert(0);
	}
	
	
	

	

		
		
		
		
		
		
	}
/* }}} */
	

#ifdef ZEND_ENGINE_2

	
	

/* {{{ xc_dasm_zend_property_info */
	
 	  	 	static void inline 	xc_dasm_zend_property_info( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_property_info * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(zend_uint) == sizeof(src->flags));
		
	
	
		add_assoc_long_ex(dst, ("flags"), (sizeof("flags")), src->flags);
		
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->name_length));
		
	
	
		add_assoc_long_ex(dst, ("name_length"), (sizeof("name_length")), src->name_length);
		
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
			add_assoc_null_ex(dst, ("name"), (sizeof("name")));
		
	}
	else {
		
		
		
		
		
		
			
				add_assoc_unicodel_ex(dst, ("name"), (sizeof("name")), ZSTR_U(src->name), src->name_length + 1-1, 1);
				
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
			add_assoc_null_ex(dst, ("name"), (sizeof("name")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("name"), (sizeof("name")), (char *) ZSTR_S(src->name), src->name_length + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
			add_assoc_null_ex(dst, ("name"), (sizeof("name")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("name"), (sizeof("name")), (char *) ZSTR_S(src->name), src->name_length + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

#endif
	

		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
	
		add_assoc_long_ex(dst, ("h"), (sizeof("h")), src->h);
		
	
	
	
	
	


	

#ifdef ZEND_ENGINE_2_4
		
	assert(sizeof(int) == sizeof(src->offset));
		
	
	
		add_assoc_long_ex(dst, ("offset"), (sizeof("offset")), src->offset);
		
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_1
		
	assert(sizeof(int) == sizeof(src->doc_comment_len));
		
	
	
		add_assoc_long_ex(dst, ("doc_comment_len"), (sizeof("doc_comment_len")), src->doc_comment_len);
		
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->doc_comment) == NULL) {
		
		
			add_assoc_null_ex(dst, ("doc_comment"), (sizeof("doc_comment")));
		
	}
	else {
		
		
		
		
		
		
			
				add_assoc_unicodel_ex(dst, ("doc_comment"), (sizeof("doc_comment")), ZSTR_U(src->doc_comment), src->doc_comment_len + 1-1, 1);
				
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
			add_assoc_null_ex(dst, ("doc_comment"), (sizeof("doc_comment")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("doc_comment"), (sizeof("doc_comment")), (char *) ZSTR_S(src->doc_comment), src->doc_comment_len + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
			add_assoc_null_ex(dst, ("doc_comment"), (sizeof("doc_comment")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("doc_comment"), (sizeof("doc_comment")), (char *) ZSTR_S(src->doc_comment), src->doc_comment_len + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

#endif
	

#endif
	#if defined(ZEND_ENGINE_2_2)
	
	if (src->ce) {
		
		
#ifdef IS_UNICODE
		add_assoc_unicodel_ex(dst, ("ce"), (sizeof("ce")), ZSTR_U(src->ce->name), src->ce->name_length, 1);
#else
		add_assoc_stringl_ex(dst, ("ce"), (sizeof("ce")), (char *) src->ce->name, src->ce->name_length, 1);
#endif
	}
	else {
		
	add_assoc_null_ex(dst, ("ce"), (sizeof("ce")));
	
	assert(src->ce == NULL);

	}

	
	
	

#endif

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif
#ifdef ZEND_ENGINE_2_4

	
	

/* {{{ xc_dasm_zend_trait_method_reference */
	
 	  	 	static void inline 	xc_dasm_zend_trait_method_reference( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_trait_method_reference * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(unsigned int) == sizeof(src->mname_len));
		
	
	
		add_assoc_long_ex(dst, ("mname_len"), (sizeof("mname_len")), src->mname_len);
		
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->method_name == NULL) {
		
		
			add_assoc_null_ex(dst, ("method_name"), (sizeof("method_name")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("method_name"), (sizeof("method_name")), (char *) src->method_name, src->mname_len + 1-1, 1);
				
			
		
	}
	
	
	
	
	
	
	

	
	
	add_assoc_null_ex(dst, ("ce"), (sizeof("ce")));
	
	assert(src->ce == NULL);

	
	
	


		
	assert(sizeof(unsigned int) == sizeof(src->cname_len));
		
	
	
		add_assoc_long_ex(dst, ("cname_len"), (sizeof("cname_len")), src->cname_len);
		
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->class_name == NULL) {
		
		
			add_assoc_null_ex(dst, ("class_name"), (sizeof("class_name")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("class_name"), (sizeof("class_name")), (char *) src->class_name, src->cname_len + 1-1, 1);
				
			
		
	}
	
	
	
	
	
	
	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dasm_zend_trait_alias */
	
 	  	 	static void inline 	xc_dasm_zend_trait_alias( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_trait_alias * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
	
	if (src->trait_method) {
		
		
	
	
	

	
	assert(sizeof(zend_trait_method_reference) == sizeof(( src->trait_method)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_zend_trait_method_reference(
		
		
		
		
		dasm, zv,  src->trait_method
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("trait_method"), (sizeof("trait_method")), zv);
		
	} while (0);
	

	
	

	}
	else {
		
		
	add_assoc_null_ex(dst, ("trait_method"), (sizeof("trait_method")));
	
	assert(src->trait_method == NULL);

	}
	
	
	
	


		
	assert(sizeof(unsigned int) == sizeof(src->alias_len));
		
	
	
		add_assoc_long_ex(dst, ("alias_len"), (sizeof("alias_len")), src->alias_len);
		
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->alias == NULL) {
		
		
			add_assoc_null_ex(dst, ("alias"), (sizeof("alias")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("alias"), (sizeof("alias")), (char *) src->alias, src->alias_len + 1-1, 1);
				
			
		
	}
	
	
	
	
	
	
	

		
	assert(sizeof(zend_uint) == sizeof(src->modifiers));
		
	
	
		add_assoc_long_ex(dst, ("modifiers"), (sizeof("modifiers")), src->modifiers);
		
	
	
	
	
	


	

#ifndef ZEND_ENGINE_2_5
	
	
	add_assoc_null_ex(dst, ("function"), (sizeof("function")));
	
	assert(src->function == NULL);

	
	
	


#endif

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dasm_zend_trait_precedence */
	
 	  	 	static void inline 	xc_dasm_zend_trait_precedence( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_trait_precedence * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
	
	if (src->trait_method) {
		
		
	
	
	

	
	assert(sizeof(zend_trait_method_reference) == sizeof(( src->trait_method)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_zend_trait_method_reference(
		
		
		
		
		dasm, zv,  src->trait_method
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("trait_method"), (sizeof("trait_method")), zv);
		
	} while (0);
	

	
	

	}
	else {
		
		
	add_assoc_null_ex(dst, ("trait_method"), (sizeof("trait_method")));
	
	assert(src->trait_method == NULL);

	}
	
	
	
	


		if (src->exclude_from_classes) {
		int LOOPCOUNTER;
		
			zval *arr;
			ALLOC_INIT_ZVAL(arr);
			array_init(arr);

			for (LOOPCOUNTER = 0;
					src->exclude_from_classes[LOOPCOUNTER];
					++LOOPCOUNTER) {
				
				
				
				
				
				
				
	
	

						
	assert(sizeof(xc_ztstring) == sizeof((xc_ztstring) src->exclude_from_classes [LOOPCOUNTER]));
		
	 
	
	
	
	
	
	
	
	
	if ((xc_ztstring) src->exclude_from_classes [LOOPCOUNTER] == NULL) {
		
		
			add_assoc_null_ex(arr, ("exclude_from_classes"), (sizeof("exclude_from_classes")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(arr, ("exclude_from_classes"), (sizeof("exclude_from_classes")), (char *) (xc_ztstring) src->exclude_from_classes [LOOPCOUNTER], strlen((xc_ztstring) src->exclude_from_classes [LOOPCOUNTER]) + 1-1, 1);
				
			
		
	}
	
	
	
	
	
	
	

	

	

				
	
	

				
				
				
				
				
				
			}
			add_assoc_zval_ex(dst, ("exclude_from_classes"), (sizeof("exclude_from_classes")), arr);
				
	
	
	

	}
	else {
		
	
	add_assoc_null_ex(dst, ("exclude_from_classes"), (sizeof("exclude_from_classes")));
	
	assert(src->exclude_from_classes == NULL);

	
	
	


	}

#ifndef ZEND_ENGINE_2_5
	
	
	add_assoc_null_ex(dst, ("function"), (sizeof("function")));
	
	assert(src->function == NULL);

	
	
	


#endif

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dasm_zend_trait_alias_ptr */
	
 	  	 	static void inline 	xc_dasm_zend_trait_alias_ptr( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_trait_alias_ptr * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
		
	

		xc_dasm_zend_trait_alias (dasm, dst, src[0] TSRMLS_CC);
		
	
	

		
		
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dasm_zend_trait_precedence_ptr */
	
 	  	 	static void inline 	xc_dasm_zend_trait_precedence_ptr( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_trait_precedence_ptr * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
		
	

		xc_dasm_zend_trait_precedence (dasm, dst, src[0] TSRMLS_CC);
		
	
	

		
		
		
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_dasm_zend_class_entry */
	
		/* export:  	  	 	void 	xc_dasm_zend_class_entry( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_class_entry * const src 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dasm_zend_class_entry( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_class_entry * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
		
	assert(sizeof(char) == sizeof(src->type));
		
	
	
		add_assoc_long_ex(dst, ("type"), (sizeof("type")), src->type);
		
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->name_length));
		
	
	
		add_assoc_long_ex(dst, ("name_length"), (sizeof("name_length")), src->name_length);
		
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->name) == NULL) {
		
		
			add_assoc_null_ex(dst, ("name"), (sizeof("name")));
		
	}
	else {
		
		
		
		
		
		
			
				add_assoc_unicodel_ex(dst, ("name"), (sizeof("name")), ZSTR_U(src->name), src->name_length + 1-1, 1);
				
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
			add_assoc_null_ex(dst, ("name"), (sizeof("name")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("name"), (sizeof("name")), (char *) ZSTR_S(src->name), src->name_length + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->name) == NULL) {
		
		
			add_assoc_null_ex(dst, ("name"), (sizeof("name")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("name"), (sizeof("name")), (char *) ZSTR_S(src->name), src->name_length + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	if (src->parent) {
		
		
#ifdef IS_UNICODE
		add_assoc_unicodel_ex(dst, ("parent"), (sizeof("parent")), ZSTR_U(src->parent->name), src->parent->name_length, 1);
#else
		add_assoc_stringl_ex(dst, ("parent"), (sizeof("parent")), (char *) src->parent->name, src->parent->name_length, 1);
#endif
	}
	else {
		
	add_assoc_null_ex(dst, ("parent"), (sizeof("parent")));
	
	assert(src->parent == NULL);

	}

	
	
	

	
#ifdef ZEND_ENGINE_2
		
	assert(sizeof(int) == sizeof(src->refcount));
		
	
	
		add_assoc_long_ex(dst, ("refcount"), (sizeof("refcount")), src->refcount);
		
	
	
	
	
	


	

#else
	
	
	if (src->refcount) {
		
		
	
	
	

	
	assert(sizeof(int) == sizeof(( src->refcount)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_int(
		
		
		
		
		dasm, zv,  src->refcount
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("refcount"), (sizeof("refcount")), zv);
		
	} while (0);
	

	
	

	}
	else {
		
		
	add_assoc_null_ex(dst, ("refcount"), (sizeof("refcount")));
	
	assert(src->refcount == NULL);

	}
	
	
	
	


#endif
#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_bool) == sizeof(src->constants_updated));
		
	
	
		add_assoc_bool_ex(dst, ("constants_updated"), (sizeof("constants_updated")), src->constants_updated ? 1 : 0);
		
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2
		
	assert(sizeof(zend_uint) == sizeof(src->ce_flags));
		
	
	
		add_assoc_long_ex(dst, ("ce_flags"), (sizeof("ce_flags")), src->ce_flags);
		
	
	
	
	
	


	

#endif

#ifdef ZEND_ENGINE_2
	
	
	assert(sizeof(HashTable) == sizeof(src->properties_info));
	
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->properties_info)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_HashTable_zend_property_info(
		
		
		
		
		dasm, zv, & src->properties_info
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("properties_info"), (sizeof("properties_info")), zv);
		
	} while (0);
	

	
	

	
	
	
	


#endif

#ifdef ZEND_ENGINE_2_4
	
	if (src->default_properties_table) {
		int i; 
		
	

		
			zval *arr;
			ALLOC_INIT_ZVAL(arr);
			array_init(arr);

			for (i = 0;
					i < src->default_properties_count;
					++i) {
				zval *zv;

				ALLOC_INIT_ZVAL(zv);
				array_init(zv);
				xc_dasm_zval_ptr_nullable (dasm, zv, &(src->default_properties_table[i]) TSRMLS_CC);
				add_next_index_zval(arr, zv);
			}
			add_assoc_zval_ex(dst, ("default_properties_table"), (sizeof("default_properties_table")), arr);
				
		
	
	
	

		
		
	}
	else {
		
	
	add_assoc_null_ex(dst, ("default_properties_table"), (sizeof("default_properties_table")));
	
	assert(src->default_properties_table == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->default_properties_count));
		
	
	
		add_assoc_long_ex(dst, ("default_properties_count"), (sizeof("default_properties_count")), src->default_properties_count);
		
	
	
	
	
	


	

	
	if (src->default_static_members_table) {
		int i; 
		
	

		
			zval *arr;
			ALLOC_INIT_ZVAL(arr);
			array_init(arr);

			for (i = 0;
					i < src->default_static_members_count;
					++i) {
				zval *zv;

				ALLOC_INIT_ZVAL(zv);
				array_init(zv);
				xc_dasm_zval_ptr_nullable (dasm, zv, &(src->default_static_members_table[i]) TSRMLS_CC);
				add_next_index_zval(arr, zv);
			}
			add_assoc_zval_ex(dst, ("default_static_members_table"), (sizeof("default_static_members_table")), arr);
				
		
	
	
	

		
		
	}
	else {
		
	
	add_assoc_null_ex(dst, ("default_static_members_table"), (sizeof("default_static_members_table")));
	
	assert(src->default_static_members_table == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->default_static_members_count));
		
	
	
		add_assoc_long_ex(dst, ("default_static_members_count"), (sizeof("default_static_members_count")), src->default_static_members_count);
		
	
	
	
	
	


	

	
	
	
	
	

#else
	
	
	
	
	

	
	
	assert(sizeof(HashTable) == sizeof(src->default_properties));
	
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->default_properties)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_HashTable_zval_ptr(
		
		
		
		
		dasm, zv, & src->default_properties
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("default_properties"), (sizeof("default_properties")), zv);
		
	} while (0);
	

	
	

	
	
	
	


#	ifdef ZEND_ENGINE_2_1
	
	
	assert(sizeof(HashTable) == sizeof(src->default_static_members));
	
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->default_static_members)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_HashTable_zval_ptr(
		
		
		
		
		dasm, zv, & src->default_static_members
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("default_static_members"), (sizeof("default_static_members")), zv);
		
	} while (0);
	

	
	

	
	
	
	


	
	
	
	
	

#	elif defined(ZEND_ENGINE_2)
	
	
	if (src->static_members) {
		
		
	
	
	

	
	assert(sizeof(HashTable) == sizeof(( src->static_members)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_HashTable_zval_ptr(
		
		
		
		
		dasm, zv,  src->static_members
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("static_members"), (sizeof("static_members")), zv);
		
	} while (0);
	

	
	

	}
	else {
		
		
	add_assoc_null_ex(dst, ("static_members"), (sizeof("static_members")));
	
	assert(src->static_members == NULL);

	}
	
	
	
	


#	endif
#endif /* ZEND_ENGINE_2_4 */

#ifdef ZEND_ENGINE_2
	
	
	assert(sizeof(HashTable) == sizeof(src->constants_table));
	
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->constants_table)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_HashTable_zval_ptr(
		
		
		
		
		dasm, zv, & src->constants_table
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("constants_table"), (sizeof("constants_table")), zv);
		
	} while (0);
	

	
	

	
	
	
	



#ifdef ZEND_ENGINE_2_2
		
	
	add_assoc_null_ex(dst, ("interfaces"), (sizeof("interfaces")));
	
	assert(src->interfaces == NULL);

	
	
	


	
	
	add_assoc_long_ex(dst, ("num_interfaces"), (sizeof("num_interfaces")), 0);
	
	assert(src->num_interfaces == 0);

	
	
	



#	ifdef ZEND_ENGINE_2_4
		
	
	add_assoc_null_ex(dst, ("traits"), (sizeof("traits")));
	
	assert(src->traits == NULL);

	
	
	


	
	
	add_assoc_long_ex(dst, ("num_traits"), (sizeof("num_traits")), 0);
	
	assert(src->num_traits == 0);

	
	
	


	
	if (src->trait_aliases) {
		size_t i; 
		
	

		
			zval *arr;
			ALLOC_INIT_ZVAL(arr);
			array_init(arr);

			for (i = 0;
					src->trait_aliases[i];
					++i) {
				zval *zv;

				ALLOC_INIT_ZVAL(zv);
				array_init(zv);
				xc_dasm_zend_trait_alias_ptr (dasm, zv, &(src->trait_aliases[i]) TSRMLS_CC);
				add_next_index_zval(arr, zv);
			}
			add_assoc_zval_ex(dst, ("trait_aliases"), (sizeof("trait_aliases")), arr);
				
		
	
	
	

		
		
	}
	else {
		
	
	add_assoc_null_ex(dst, ("trait_aliases"), (sizeof("trait_aliases")));
	
	assert(src->trait_aliases == NULL);

	
	
	


	}

	
	if (src->trait_precedences) {
		size_t i; 
		
	

		
			zval *arr;
			ALLOC_INIT_ZVAL(arr);
			array_init(arr);

			for (i = 0;
					src->trait_precedences[i];
					++i) {
				zval *zv;

				ALLOC_INIT_ZVAL(zv);
				array_init(zv);
				xc_dasm_zend_trait_precedence_ptr (dasm, zv, &(src->trait_precedences[i]) TSRMLS_CC);
				add_next_index_zval(arr, zv);
			}
			add_assoc_zval_ex(dst, ("trait_precedences"), (sizeof("trait_precedences")), arr);
				
		
	
	
	

		
		
	}
	else {
		
	
	add_assoc_null_ex(dst, ("trait_precedences"), (sizeof("trait_precedences")));
	
	assert(src->trait_precedences == NULL);

	
	
	


	}

#	endif
#else
	
		
	
	
	

	
		
	assert(sizeof(zend_uint) == sizeof(src->num_interfaces));
		
	
	
		add_assoc_long_ex(dst, ("num_interfaces"), (sizeof("num_interfaces")), src->num_interfaces);
		
	
	
	
	
	


	

#endif

#	ifdef ZEND_ENGINE_2_4
	
	
	

	 
	
	
	
	
	
	
	
	
	if (src->info.user.filename == NULL) {
		
		
			add_assoc_null_ex(dst, ("info.user.filename"), (sizeof("info.user.filename")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("info.user.filename"), (sizeof("info.user.filename")), (char *) src->info.user.filename, strlen(src->info.user.filename) + 1-1, 1);
				
			
		
	}
	
	
	
	
	
	
	

		
	assert(sizeof(zend_uint) == sizeof(src->info.user.line_start));
		
	
	
		add_assoc_long_ex(dst, ("info.user.line_start"), (sizeof("info.user.line_start")), src->info.user.line_start);
		
	
	

	

		
	assert(sizeof(zend_uint) == sizeof(src->info.user.line_end));
		
	
	
		add_assoc_long_ex(dst, ("info.user.line_end"), (sizeof("info.user.line_end")), src->info.user.line_end);
		
	
	

	

		
	assert(sizeof(zend_uint) == sizeof(src->info.user.doc_comment_len));
		
	
	
		add_assoc_long_ex(dst, ("info.user.doc_comment_len"), (sizeof("info.user.doc_comment_len")), src->info.user.doc_comment_len);
		
	
	

	

	 
	
#ifdef IS_UNICODE
	
	
	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->info.user.doc_comment) == NULL) {
		
		
			add_assoc_null_ex(dst, ("info.user.doc_comment"), (sizeof("info.user.doc_comment")));
		
	}
	else {
		
		
		
		
		
		
			
				add_assoc_unicodel_ex(dst, ("info.user.doc_comment"), (sizeof("info.user.doc_comment")), ZSTR_U(src->info.user.doc_comment), src->info.user.doc_comment_len + 1-1, 1);
				
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->info.user.doc_comment) == NULL) {
		
		
			add_assoc_null_ex(dst, ("info.user.doc_comment"), (sizeof("info.user.doc_comment")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("info.user.doc_comment"), (sizeof("info.user.doc_comment")), (char *) ZSTR_S(src->info.user.doc_comment), src->info.user.doc_comment_len + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->info.user.doc_comment) == NULL) {
		
		
			add_assoc_null_ex(dst, ("info.user.doc_comment"), (sizeof("info.user.doc_comment")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("info.user.doc_comment"), (sizeof("info.user.doc_comment")), (char *) ZSTR_S(src->info.user.doc_comment), src->info.user.doc_comment_len + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

#endif
	

	
	
	

	
	
	
	

#	else
	 
	
	
	

	
	
	
	
	
	
	
	
	if (src->filename == NULL) {
		
		
			add_assoc_null_ex(dst, ("filename"), (sizeof("filename")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("filename"), (sizeof("filename")), (char *) src->filename, strlen(src->filename) + 1-1, 1);
				
			
		
	}
	
	
	
	
	
	
	

		
	assert(sizeof(zend_uint) == sizeof(src->line_start));
		
	
	
		add_assoc_long_ex(dst, ("line_start"), (sizeof("line_start")), src->line_start);
		
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->line_end));
		
	
	
		add_assoc_long_ex(dst, ("line_end"), (sizeof("line_end")), src->line_end);
		
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->doc_comment_len));
		
	
	
		add_assoc_long_ex(dst, ("doc_comment_len"), (sizeof("doc_comment_len")), src->doc_comment_len);
		
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->doc_comment) == NULL) {
		
		
			add_assoc_null_ex(dst, ("doc_comment"), (sizeof("doc_comment")));
		
	}
	else {
		
		
		
		
		
		
			
				add_assoc_unicodel_ex(dst, ("doc_comment"), (sizeof("doc_comment")), ZSTR_U(src->doc_comment), src->doc_comment_len + 1-1, 1);
				
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
			add_assoc_null_ex(dst, ("doc_comment"), (sizeof("doc_comment")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("doc_comment"), (sizeof("doc_comment")), (char *) ZSTR_S(src->doc_comment), src->doc_comment_len + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
			add_assoc_null_ex(dst, ("doc_comment"), (sizeof("doc_comment")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("doc_comment"), (sizeof("doc_comment")), (char *) ZSTR_S(src->doc_comment), src->doc_comment_len + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

#endif
	

#	endif

	/* # NOT DONE */
#	ifdef ZEND_ENGINE_2_1
	
	
	
	

	
	
	
	

#	endif
	
	
	
	

	
	
	
	

	
	
	
	

	
	
	
	

#	ifdef ZEND_ENGINE_2_3
	
	
	
	

#	endif
#	ifdef ZEND_ENGINE_2_1
	
	
	
	

	
	
	
	

#	endif
	/* deal with it inside xc_fix_method */
	
	
	
	

	
	
	
	

	
	
	
	

	
	
	
	

	
	
	
	

/* should be >5.1 */
#	ifdef ZEND_ENGINE_2_1
	
	
	
	

	
	
	
	

#	endif
	
	
	
	

#	ifdef ZEND_CALLSTATIC_FUNC_NAME
	
	
	
	

#	endif
# if defined(ZEND_ENGINE_2_2) || PHP_MAJOR_VERSION >= 6
	
	
	
	

# endif
# if defined(ZEND_ENGINE_2_6)
	
	
	
	

# endif
#	ifndef ZEND_ENGINE_2_4
	/* # NOT DONE */
	
	
	
	

#	endif
#else /* ZEND_ENGINE_2 */
	
	
	
	

	
	
	
	

	
	
	
	

#endif
		
	
	assert(sizeof(HashTable) == sizeof(src->function_table));
	
	
	
	
	

	
	assert(sizeof(HashTable) == sizeof((& src->function_table)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_HashTable_zend_function(
		
		
		
		
		dasm, zv, & src->function_table
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("function_table"), (sizeof("function_table")), zv);
		
	} while (0);
	

	
	

	
	
	
	


	
	

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#ifdef ZEND_ENGINE_2_4


#else

	
	

/* {{{ xc_dasm_znode */
	
 	  	 	static void inline 	xc_dasm_znode( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const znode * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(xc_op_type) == sizeof(src->op_type));
			
	
	
		add_assoc_long_ex(dst, ("op_type"), (sizeof("op_type")), src->op_type);
		
	
	
	
	
	



	


#ifdef IS_CV
#	define XCACHE_IS_CV IS_CV
#else
/* compatible with zend optimizer */
#	define XCACHE_IS_CV 16
#endif
	assert(src->op_type == IS_CONST ||
		src->op_type == IS_VAR ||
		src->op_type == XCACHE_IS_CV ||
		src->op_type == IS_TMP_VAR ||
		src->op_type == IS_UNUSED);
		
	
	

	switch (src->op_type) {
		case IS_CONST:
			
	
	assert(sizeof(zval) == sizeof(src->u.constant));
	
	
	
	
	

	
	assert(sizeof(zval) == sizeof((& src->u.constant)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_zval(
		
		
		
		
		dasm, zv, & src->u.constant
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("u.constant"), (sizeof("u.constant")), zv);
		
	} while (0);
	

	
	

	

			break;
		
		case IS_VAR:
		case IS_TMP_VAR:
		case XCACHE_IS_CV:
				
	assert(sizeof(zend_uint) == sizeof(src->u.var));
		
	
	
		add_assoc_long_ex(dst, ("u.var"), (sizeof("u.var")), src->u.var);
		
	
	

	

				
	assert(sizeof(zend_uint) == sizeof(src->u.EA.type));
		
	
	
		add_assoc_long_ex(dst, ("u.EA.type"), (sizeof("u.EA.type")), src->u.EA.type);
		
	
	

	

			break;
		case IS_UNUSED:
				
	assert(sizeof(zend_uint) == sizeof(src->u.var));
		
	
	
		add_assoc_long_ex(dst, ("u.var"), (sizeof("u.var")), src->u.var);
		
	
	

	

				
	assert(sizeof(zend_uint) == sizeof(src->u.opline_num));
		
	
	
		add_assoc_long_ex(dst, ("u.opline_num"), (sizeof("u.opline_num")), src->u.opline_num);
		
	
	

	

#ifndef ZEND_ENGINE_2
				
	assert(sizeof(zend_uint) == sizeof(src->u.fetch_type));
		
	
	
		add_assoc_long_ex(dst, ("u.fetch_type"), (sizeof("u.fetch_type")), src->u.fetch_type);
		
	
	

	

#endif
				
	assert(sizeof(zend_uint) == sizeof(src->u.EA.type));
		
	
	
		add_assoc_long_ex(dst, ("u.EA.type"), (sizeof("u.EA.type")), src->u.EA.type);
		
	
	

	

			break;
		
	}
	
	
	

	
	
	
	

#if 0
	
	
	
	

#endif
#undef XCACHE_IS_CV

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_dasm_zend_op */
	
		/* export:  	  	 	void 	xc_dasm_zend_op( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_op * const src 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dasm_zend_op( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_op * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(xc_opcode) == sizeof(src->opcode));
			
	
	
		add_assoc_long_ex(dst, ("opcode"), (sizeof("opcode")), src->opcode);
		
	
	
	
	
	



	

#ifdef ZEND_ENGINE_2_4
	
	switch (src->opcode) {
	case ZEND_BIND_TRAITS:
		((zend_op *) src)->op2_type = IS_UNUSED;
		break;
	}
	
	#ifndef NDEBUG
	switch ((src->result_type & ~EXT_TYPE_UNUSED)) {
	case IS_CONST:
	case IS_VAR:
	case IS_CV:
	case IS_TMP_VAR:
	case IS_UNUSED:
		break;

	default:
		assert(0);
	}
#endif

		
	
	

	switch ((src->result_type & ~EXT_TYPE_UNUSED)) {
		case IS_CONST:
			
					
	assert(sizeof(zend_uint) == sizeof(src->result.constant));
		
	
	
		add_assoc_long_ex(dst, ("result.constant"), (sizeof("result.constant")), src->result.constant);
		
	
	

	

			
			break;
		
		case IS_VAR:
		case IS_TMP_VAR:
		case IS_CV:
				
	assert(sizeof(zend_uint) == sizeof(src->result.var));
		
	
	
		add_assoc_long_ex(dst, ("result.var"), (sizeof("result.var")), src->result.var);
		
	
	

	

			break;
		case IS_UNUSED:
				
	assert(sizeof(zend_uint) == sizeof(src->result.var));
		
	
	
		add_assoc_long_ex(dst, ("result.var"), (sizeof("result.var")), src->result.var);
		
	
	

	

				
	assert(sizeof(zend_uint) == sizeof(src->result.opline_num));
		
	
	
		add_assoc_long_ex(dst, ("result.opline_num"), (sizeof("result.opline_num")), src->result.opline_num);
		
	
	

	

			break;
		
	}
	
	
	

	
	
	
	


	#ifndef NDEBUG
	switch ((src->op1_type )) {
	case IS_CONST:
	case IS_VAR:
	case IS_CV:
	case IS_TMP_VAR:
	case IS_UNUSED:
		break;

	default:
		assert(0);
	}
#endif

		
	
	

	switch ((src->op1_type )) {
		case IS_CONST:
			
				{
					zval *zv;
					zval *srczv = &dasm->active_op_array_src->literals[src->op1.constant].constant;
					ALLOC_ZVAL(zv);
					MAKE_COPY_ZVAL(&srczv, zv);
					add_assoc_zval_ex(dst, ("op1.constant"), (sizeof("op1.constant")), zv);
				}
				
			
			break;
		
		case IS_VAR:
		case IS_TMP_VAR:
		case IS_CV:
				
	assert(sizeof(zend_uint) == sizeof(src->op1.var));
		
	
	
		add_assoc_long_ex(dst, ("op1.var"), (sizeof("op1.var")), src->op1.var);
		
	
	

	

			break;
		case IS_UNUSED:
				
	assert(sizeof(zend_uint) == sizeof(src->op1.var));
		
	
	
		add_assoc_long_ex(dst, ("op1.var"), (sizeof("op1.var")), src->op1.var);
		
	
	

	

				
	assert(sizeof(zend_uint) == sizeof(src->op1.opline_num));
		
	
	
		add_assoc_long_ex(dst, ("op1.opline_num"), (sizeof("op1.opline_num")), src->op1.opline_num);
		
	
	

	

			break;
		
	}
	
	
	

	
	
	
	


	#ifndef NDEBUG
	switch ((src->op2_type )) {
	case IS_CONST:
	case IS_VAR:
	case IS_CV:
	case IS_TMP_VAR:
	case IS_UNUSED:
		break;

	default:
		assert(0);
	}
#endif

		
	
	

	switch ((src->op2_type )) {
		case IS_CONST:
			
				{
					zval *zv;
					zval *srczv = &dasm->active_op_array_src->literals[src->op2.constant].constant;
					ALLOC_ZVAL(zv);
					MAKE_COPY_ZVAL(&srczv, zv);
					add_assoc_zval_ex(dst, ("op2.constant"), (sizeof("op2.constant")), zv);
				}
				
			
			break;
		
		case IS_VAR:
		case IS_TMP_VAR:
		case IS_CV:
				
	assert(sizeof(zend_uint) == sizeof(src->op2.var));
		
	
	
		add_assoc_long_ex(dst, ("op2.var"), (sizeof("op2.var")), src->op2.var);
		
	
	

	

			break;
		case IS_UNUSED:
				
	assert(sizeof(zend_uint) == sizeof(src->op2.var));
		
	
	
		add_assoc_long_ex(dst, ("op2.var"), (sizeof("op2.var")), src->op2.var);
		
	
	

	

				
	assert(sizeof(zend_uint) == sizeof(src->op2.opline_num));
		
	
	
		add_assoc_long_ex(dst, ("op2.opline_num"), (sizeof("op2.opline_num")), src->op2.opline_num);
		
	
	

	

			break;
		
	}
	
	
	

	
	
	
	


#else
	
	
	assert(sizeof(znode) == sizeof(src->result));
	
	
	
	
	

	
	assert(sizeof(znode) == sizeof((& src->result)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_znode(
		
		
		
		
		dasm, zv, & src->result
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("result"), (sizeof("result")), zv);
		
	} while (0);
	

	
	

	
	
	
	


	
	
	assert(sizeof(znode) == sizeof(src->op1));
	
	
	
	
	

	
	assert(sizeof(znode) == sizeof((& src->op1)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_znode(
		
		
		
		
		dasm, zv, & src->op1
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("op1"), (sizeof("op1")), zv);
		
	} while (0);
	

	
	

	
	
	
	


	
	
	assert(sizeof(znode) == sizeof(src->op2));
	
	
	
	
	

	
	assert(sizeof(znode) == sizeof((& src->op2)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_znode(
		
		
		
		
		dasm, zv, & src->op2
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("op2"), (sizeof("op2")), zv);
		
	} while (0);
	

	
	

	
	
	
	


#endif
		
	assert(sizeof(ulong) == sizeof(src->extended_value));
		
	
	
		add_assoc_long_ex(dst, ("extended_value"), (sizeof("extended_value")), src->extended_value);
		
	
	
	
	
	


	

		
	assert(sizeof(uint) == sizeof(src->lineno));
		
	
	
		add_assoc_long_ex(dst, ("lineno"), (sizeof("lineno")), src->lineno);
		
	
	
	
	
	


	

#ifdef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uchar) == sizeof(src->op1_type));
		
	
	
		add_assoc_long_ex(dst, ("op1_type"), (sizeof("op1_type")), src->op1_type);
		
	
	
	
	
	


	

		
	assert(sizeof(zend_uchar) == sizeof(src->op2_type));
		
	
	
		add_assoc_long_ex(dst, ("op2_type"), (sizeof("op2_type")), src->op2_type);
		
	
	
	
	
	


	

		
	assert(sizeof(zend_uchar) == sizeof(src->result_type));
		
	
	
		add_assoc_long_ex(dst, ("result_type"), (sizeof("result_type")), src->result_type);
		
	
	
	
	
	


	

#endif
	
#ifdef ZEND_ENGINE_2
		
	assert(sizeof(opcode_handler_t) == sizeof(src->handler));
	/* is copying enough? */
	
	
	

	

#endif

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#ifdef ZEND_ENGINE_2_4

	
	

/* {{{ xc_dasm_zend_literal */
	
 	  	 	static void inline 	xc_dasm_zend_literal( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_literal * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
	
	assert(sizeof(zval) == sizeof(src->constant));
	
	
	
	
	

	
	assert(sizeof(zval) == sizeof((& src->constant)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_zval(
		
		
		
		
		dasm, zv, & src->constant
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("constant"), (sizeof("constant")), zv);
		
	} while (0);
	

	
	

	
	
	
	


		
	assert(sizeof(zend_ulong) == sizeof(src->hash_value));
		
	
	
		add_assoc_long_ex(dst, ("hash_value"), (sizeof("hash_value")), src->hash_value);
		
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->cache_slot));
		
	
	
		add_assoc_long_ex(dst, ("cache_slot"), (sizeof("cache_slot")), src->cache_slot);
		
	
	
	
	
	


	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif

	
	

/* {{{ xc_dasm_zend_op_array */
	
		/* export:  	  	 	void 	xc_dasm_zend_op_array( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_op_array * const src 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dasm_zend_op_array( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const zend_op_array * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
	
		dasm->active_op_array_src = src;
	
	{
	
	do {
	
	/* Common elements */
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
	
		add_assoc_long_ex(dst, ("type"), (sizeof("type")), src->type);
		
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->function_name) == NULL) {
		
		
			add_assoc_null_ex(dst, ("function_name"), (sizeof("function_name")));
		
	}
	else {
		
		
		
		
		
		
			
				add_assoc_unicodel_ex(dst, ("function_name"), (sizeof("function_name")), ZSTR_U(src->function_name), xc_zstrlen_uchar (src->function_name) + 1-1, 1);
				
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->function_name) == NULL) {
		
		
			add_assoc_null_ex(dst, ("function_name"), (sizeof("function_name")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("function_name"), (sizeof("function_name")), (char *) ZSTR_S(src->function_name), xc_zstrlen_char (src->function_name) + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->function_name) == NULL) {
		
		
			add_assoc_null_ex(dst, ("function_name"), (sizeof("function_name")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("function_name"), (sizeof("function_name")), (char *) ZSTR_S(src->function_name), xc_zstrlen_char (src->function_name) + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

#endif
	

#ifdef ZEND_ENGINE_2
		
	assert(sizeof(zend_uint) == sizeof(src->fn_flags));
		
	
	
		add_assoc_long_ex(dst, ("fn_flags"), (sizeof("fn_flags")), src->fn_flags);
		
	
	
	
	
	


	

	
	if (src->arg_info) {
		zend_uint i; 
		
	

		
			zval *arr;
			ALLOC_INIT_ZVAL(arr);
			array_init(arr);

			for (i = 0;
					i < src->num_args;
					++i) {
				zval *zv;

				ALLOC_INIT_ZVAL(zv);
				array_init(zv);
				xc_dasm_zend_arg_info (dasm, zv, &(src->arg_info[i]) TSRMLS_CC);
				add_next_index_zval(arr, zv);
			}
			add_assoc_zval_ex(dst, ("arg_info"), (sizeof("arg_info")), arr);
				
		
	
	
	

		
		
	}
	else {
		
	
	add_assoc_null_ex(dst, ("arg_info"), (sizeof("arg_info")));
	
	assert(src->arg_info == NULL);

	
	
	


	}

		
	assert(sizeof(zend_uint) == sizeof(src->num_args));
		
	
	
		add_assoc_long_ex(dst, ("num_args"), (sizeof("num_args")), src->num_args);
		
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->required_num_args));
		
	
	
		add_assoc_long_ex(dst, ("required_num_args"), (sizeof("required_num_args")), src->required_num_args);
		
	
	
	
	
	


	

#	ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_bool) == sizeof(src->pass_rest_by_reference));
		
	
	
		add_assoc_bool_ex(dst, ("pass_rest_by_reference"), (sizeof("pass_rest_by_reference")), src->pass_rest_by_reference ? 1 : 0);
		
	
	
	
	
	


	

#	endif
#else
	if (src->arg_types) {
		
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	

		
		do {
			int i;
			zval *zv;
			ALLOC_INIT_ZVAL(zv);
			array_init(zv);
			for (i = 0; i < src->arg_types[0]; i ++) {
				add_next_index_long(zv, src->arg_types[i + 1]);
			}
			add_assoc_zval_ex(dst, ZEND_STRS("arg_types"), zv);
		} while (0);
		
	
	
	

	}
	else {
		
	
	add_assoc_null_ex(dst, ("arg_types"), (sizeof("arg_types")));
	
	assert(src->arg_types == NULL);

	
	
	


	}
#endif
#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(unsigned char) == sizeof(src->return_reference));
		
	
	
		add_assoc_long_ex(dst, ("return_reference"), (sizeof("return_reference")), src->return_reference);
		
	
	
	
	
	


	

#endif
	/* END of common elements */
#ifdef IS_UNICODE
	#endif

	
	
	if (src->refcount) {
		
		
	
	
	

	
	assert(sizeof(zend_uint) == sizeof(( src->refcount)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_zend_uint(
		
		
		
		
		dasm, zv,  src->refcount
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("refcount"), (sizeof("refcount")), zv);
		
	} while (0);
	

	
	

	}
	else {
		
		
	add_assoc_null_ex(dst, ("refcount"), (sizeof("refcount")));
	
	assert(src->refcount == NULL);

	}
	
	
	
	


	

#ifdef ZEND_ENGINE_2_4
		
	if (src->literals) {
		int i; 
		
	

		
			zval *arr;
			ALLOC_INIT_ZVAL(arr);
			array_init(arr);

			for (i = 0;
					i < src->last_literal;
					++i) {
				zval *zv;

				ALLOC_INIT_ZVAL(zv);
				array_init(zv);
				xc_dasm_zend_literal (dasm, zv, &(src->literals[i]) TSRMLS_CC);
				add_next_index_zval(arr, zv);
			}
			add_assoc_zval_ex(dst, ("literals"), (sizeof("literals")), arr);
				
		
	
	
	

		
		
	}
	else {
		
	
	add_assoc_null_ex(dst, ("literals"), (sizeof("literals")));
	
	assert(src->literals == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->last_literal));
		
	
	
		add_assoc_long_ex(dst, ("last_literal"), (sizeof("last_literal")), src->last_literal);
		
	
	
	
	
	


	

#endif

		
	if (src->opcodes) {
		zend_uint i; 
		
	

		
			zval *arr;
			ALLOC_INIT_ZVAL(arr);
			array_init(arr);

			for (i = 0;
					i < src->last;
					++i) {
				zval *zv;

				ALLOC_INIT_ZVAL(zv);
				array_init(zv);
				xc_dasm_zend_op (dasm, zv, &(src->opcodes[i]) TSRMLS_CC);
				add_next_index_zval(arr, zv);
			}
			add_assoc_zval_ex(dst, ("opcodes"), (sizeof("opcodes")), arr);
				
		
	
	
	

		
		
	}
	else {
		
	
	add_assoc_null_ex(dst, ("opcodes"), (sizeof("opcodes")));
	
	assert(src->opcodes == NULL);

	
	
	


	}

		
	assert(sizeof(zend_uint) == sizeof(src->last));
		
	
	
		add_assoc_long_ex(dst, ("last"), (sizeof("last")), src->last);
		
	
	
	
	
	


	

#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uint) == sizeof(src->size));
		
	
	
		add_assoc_long_ex(dst, ("size"), (sizeof("size")), src->size);
		
	
	
	
	
	


	

#endif

#ifdef IS_CV
	
	if (src->vars) {
		int i; 
		
	

		
			zval *arr;
			ALLOC_INIT_ZVAL(arr);
			array_init(arr);

			for (i = 0;
					i < src->last_var;
					++i) {
				zval *zv;

				ALLOC_INIT_ZVAL(zv);
				array_init(zv);
				xc_dasm_zend_compiled_variable (dasm, zv, &(src->vars[i]) TSRMLS_CC);
				add_next_index_zval(arr, zv);
			}
			add_assoc_zval_ex(dst, ("vars"), (sizeof("vars")), arr);
				
		
	
	
	

		
		
	}
	else {
		
	
	add_assoc_null_ex(dst, ("vars"), (sizeof("vars")));
	
	assert(src->vars == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->last_var));
		
	
	
		add_assoc_long_ex(dst, ("last_var"), (sizeof("last_var")), src->last_var);
		
	
	
	
	
	


	

#	ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uint) == sizeof(src->size_var));
		
	
	
		add_assoc_long_ex(dst, ("size_var"), (sizeof("size_var")), src->size_var);
		
	
	
	
	
	


	

#	endif
#else
		
		
		
#endif

		
	assert(sizeof(zend_uint) == sizeof(src->T));
		
	
	
		add_assoc_long_ex(dst, ("T"), (sizeof("T")), src->T);
		
	
	
	
	
	


	


#ifdef ZEND_ENGINE_2_5
		
	assert(sizeof(zend_uint) == sizeof(src->nested_calls));
		
	
	
		add_assoc_long_ex(dst, ("nested_calls"), (sizeof("nested_calls")), src->nested_calls);
		
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->used_stack));
		
	
	
		add_assoc_long_ex(dst, ("used_stack"), (sizeof("used_stack")), src->used_stack);
		
	
	
	
	
	


	

#endif

	
	if (src->brk_cont_array) {
		last_brk_cont_t i; 
		
	

		
			zval *arr;
			ALLOC_INIT_ZVAL(arr);
			array_init(arr);

			for (i = 0;
					i < src->last_brk_cont;
					++i) {
				zval *zv;

				ALLOC_INIT_ZVAL(zv);
				array_init(zv);
				xc_dasm_zend_brk_cont_element (dasm, zv, &(src->brk_cont_array[i]) TSRMLS_CC);
				add_next_index_zval(arr, zv);
			}
			add_assoc_zval_ex(dst, ("brk_cont_array"), (sizeof("brk_cont_array")), arr);
				
		
	
	
	

		
		
	}
	else {
		
	
	add_assoc_null_ex(dst, ("brk_cont_array"), (sizeof("brk_cont_array")));
	
	assert(src->brk_cont_array == NULL);

	
	
	


	}

		
	assert(sizeof(last_brk_cont_t) == sizeof(src->last_brk_cont));
		
	
	
		add_assoc_long_ex(dst, ("last_brk_cont"), (sizeof("last_brk_cont")), src->last_brk_cont);
		
	
	
	
	
	



	

#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uint) == sizeof(src->current_brk_cont));
		
	
	
		add_assoc_long_ex(dst, ("current_brk_cont"), (sizeof("current_brk_cont")), src->current_brk_cont);
		
	
	
	
	
	


	

#endif
#ifndef ZEND_ENGINE_2
		
	assert(sizeof(zend_bool) == sizeof(src->uses_globals));
		
	
	
		add_assoc_bool_ex(dst, ("uses_globals"), (sizeof("uses_globals")), src->uses_globals ? 1 : 0);
		
	
	
	
	
	


	

#endif

#ifdef ZEND_ENGINE_2
	
	if (src->try_catch_array) {
		int i; 
		
	

		
			zval *arr;
			ALLOC_INIT_ZVAL(arr);
			array_init(arr);

			for (i = 0;
					i < src->last_try_catch;
					++i) {
				zval *zv;

				ALLOC_INIT_ZVAL(zv);
				array_init(zv);
				xc_dasm_zend_try_catch_element (dasm, zv, &(src->try_catch_array[i]) TSRMLS_CC);
				add_next_index_zval(arr, zv);
			}
			add_assoc_zval_ex(dst, ("try_catch_array"), (sizeof("try_catch_array")), arr);
				
		
	
	
	

		
		
	}
	else {
		
	
	add_assoc_null_ex(dst, ("try_catch_array"), (sizeof("try_catch_array")));
	
	assert(src->try_catch_array == NULL);

	
	
	


	}

		
	assert(sizeof(int) == sizeof(src->last_try_catch));
		
	
	
		add_assoc_long_ex(dst, ("last_try_catch"), (sizeof("last_try_catch")), src->last_try_catch);
		
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_5
		
	assert(sizeof(zend_bool) == sizeof(src->has_finally_block));
		
	
	
		add_assoc_bool_ex(dst, ("has_finally_block"), (sizeof("has_finally_block")), src->has_finally_block ? 1 : 0);
		
	
	
	
	
	


	

#endif

	
	
	if (src->static_variables) {
		
		
	
	
	

	
	assert(sizeof(HashTable) == sizeof(( src->static_variables)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_HashTable_zval_ptr(
		
		
		
		
		dasm, zv,  src->static_variables
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("static_variables"), (sizeof("static_variables")), zv);
		
	} while (0);
	

	
	

	}
	else {
		
		
	add_assoc_null_ex(dst, ("static_variables"), (sizeof("static_variables")));
	
	assert(src->static_variables == NULL);

	}
	
	
	
	



#ifndef ZEND_ENGINE_2_4
	
	
	
	

		
	assert(sizeof(int) == sizeof(src->backpatch_count));
		
	
	
		add_assoc_long_ex(dst, ("backpatch_count"), (sizeof("backpatch_count")), src->backpatch_count);
		
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_3
		
	assert(sizeof(zend_uint) == sizeof(src->this_var));
		
	
	
		add_assoc_long_ex(dst, ("this_var"), (sizeof("this_var")), src->this_var);
		
	
	
	
	
	


	

#endif

#ifndef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_bool) == sizeof(src->done_pass_two));
		
	
	
		add_assoc_bool_ex(dst, ("done_pass_two"), (sizeof("done_pass_two")), src->done_pass_two ? 1 : 0);
		
	
	
	
	
	


	

#endif
	/* 5.0 <= ver < 5.3 */
#if defined(ZEND_ENGINE_2) && !defined(ZEND_ENGINE_2_3)
		
	assert(sizeof(zend_bool) == sizeof(src->uses_this));
		
	
	
		add_assoc_bool_ex(dst, ("uses_this"), (sizeof("uses_this")), src->uses_this ? 1 : 0);
		
	
	
	
	
	


	

#endif

	 
	
	
	

	
	
	
	
	
	
	
	
	if (src->filename == NULL) {
		
		
			add_assoc_null_ex(dst, ("filename"), (sizeof("filename")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("filename"), (sizeof("filename")), (char *) src->filename, strlen(src->filename) + 1-1, 1);
				
			
		
	}
	
	
	
	
	
	
	

#ifdef IS_UNICODE
	
		 
	
	
	

	
	
	
	
	
	
	
	
	if (src->script_encoding == NULL) {
		
		
			add_assoc_null_ex(dst, ("script_encoding"), (sizeof("script_encoding")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("script_encoding"), (sizeof("script_encoding")), (char *) src->script_encoding, strlen(src->script_encoding) + 1-1, 1);
				
			
		
	}
	
	
	
	
	
	
	

	
#endif
#ifdef ZEND_ENGINE_2
		
	assert(sizeof(zend_uint) == sizeof(src->line_start));
		
	
	
		add_assoc_long_ex(dst, ("line_start"), (sizeof("line_start")), src->line_start);
		
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->line_end));
		
	
	
		add_assoc_long_ex(dst, ("line_end"), (sizeof("line_end")), src->line_end);
		
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->doc_comment_len));
		
	
	
		add_assoc_long_ex(dst, ("doc_comment_len"), (sizeof("doc_comment_len")), src->doc_comment_len);
		
	
	
	
	
	


	

	 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (UG(unicode)) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->doc_comment) == NULL) {
		
		
			add_assoc_null_ex(dst, ("doc_comment"), (sizeof("doc_comment")));
		
	}
	else {
		
		
		
		
		
		
			
				add_assoc_unicodel_ex(dst, ("doc_comment"), (sizeof("doc_comment")), ZSTR_U(src->doc_comment), src->doc_comment_len + 1-1, 1);
				
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
			add_assoc_null_ex(dst, ("doc_comment"), (sizeof("doc_comment")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("doc_comment"), (sizeof("doc_comment")), (char *) ZSTR_S(src->doc_comment), src->doc_comment_len + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->doc_comment) == NULL) {
		
		
			add_assoc_null_ex(dst, ("doc_comment"), (sizeof("doc_comment")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("doc_comment"), (sizeof("doc_comment")), (char *) ZSTR_S(src->doc_comment), src->doc_comment_len + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

#endif
	

#endif
#ifdef ZEND_COMPILE_DELAYED_BINDING
		
	assert(sizeof(zend_uint) == sizeof(src->early_binding));
		
	
	
		add_assoc_long_ex(dst, ("early_binding"), (sizeof("early_binding")), src->early_binding);
		
	
	
	
	
	


	
;
#endif

	/* reserved */
	
	
	
	

#if defined(HARDENING_PATCH) && HARDENING_PATCH
		
	assert(sizeof(zend_bool) == sizeof(src->created_by_eval));
		
	
	
		add_assoc_bool_ex(dst, ("created_by_eval"), (sizeof("created_by_eval")), src->created_by_eval ? 1 : 0);
		
	
	
	
	
	


	

#endif
#ifdef ZEND_ENGINE_2_4
	
	
	
	

		
	assert(sizeof(int) == sizeof(src->last_cache_slot));
		
	
	
		add_assoc_long_ex(dst, ("last_cache_slot"), (sizeof("last_cache_slot")), src->last_cache_slot);
		
	
	
	
	
	


	

#endif
	} while (0);
	

#ifdef ZEND_ENGINE_2
		
		
	
	
	
	
	


#endif

#ifdef ZEND_ENGINE_2
	
	if (src->scope) {
		
		
#ifdef IS_UNICODE
		add_assoc_unicodel_ex(dst, ("scope"), (sizeof("scope")), ZSTR_U(src->scope->name), src->scope->name_length, 1);
#else
		add_assoc_stringl_ex(dst, ("scope"), (sizeof("scope")), (char *) src->scope->name, src->scope->name_length, 1);
#endif
	}
	else {
		
	add_assoc_null_ex(dst, ("scope"), (sizeof("scope")));
	
	assert(src->scope == NULL);

	}

	
	
	

	
#endif

	
	}
	
	
		dasm->active_op_array_src = NULL;
	

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


#ifdef HAVE_XCACHE_CONSTANT

	
	

/* {{{ xc_dasm_xc_constinfo_t */
	
 	  	 	static void inline 	xc_dasm_xc_constinfo_t( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const xc_constinfo_t * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(zend_uint) == sizeof(src->key_size));
		
	
	
		add_assoc_long_ex(dst, ("key_size"), (sizeof("key_size")), src->key_size);
		
	
	
	
	
	


	

#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
	
		add_assoc_long_ex(dst, ("type"), (sizeof("type")), src->type);
		
	
	
	
	
	


	

#endif
	
		
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (src->type == IS_UNICODE) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->key) == NULL) {
		
		
			add_assoc_null_ex(dst, ("key"), (sizeof("key")));
		
	}
	else {
		
		
		
		
		
		
			
				add_assoc_unicodel_ex(dst, ("key"), (sizeof("key")), ZSTR_U(src->key), src->key_size-1, 1);
				
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
			add_assoc_null_ex(dst, ("key"), (sizeof("key")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("key"), (sizeof("key")), (char *) ZSTR_S(src->key), src->key_size-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
			add_assoc_null_ex(dst, ("key"), (sizeof("key")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("key"), (sizeof("key")), (char *) ZSTR_S(src->key), src->key_size-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
	
		add_assoc_long_ex(dst, ("h"), (sizeof("h")), src->h);
		
	
	
	
	
	


	

	
	
	assert(sizeof(zend_constant) == sizeof(src->constant));
	
	
	
	
	

	
	assert(sizeof(zend_constant) == sizeof((& src->constant)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_zend_constant(
		
		
		
		
		dasm, zv, & src->constant
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("constant"), (sizeof("constant")), zv);
		
	} while (0);
	

	
	

	
	
	
	



		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif


	
	

/* {{{ xc_dasm_xc_op_array_info_detail_t */
	
 	  	 	static void inline 	xc_dasm_xc_op_array_info_detail_t( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const xc_op_array_info_detail_t * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(zend_uint) == sizeof(src->index));
		
	
	
		add_assoc_long_ex(dst, ("index"), (sizeof("index")), src->index);
		
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->info));
		
	
	
		add_assoc_long_ex(dst, ("info"), (sizeof("info")), src->info);
		
	
	
	
	
	


	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dasm_xc_op_array_info_t */
	
 	  	 	static void inline 	xc_dasm_xc_op_array_info_t( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const xc_op_array_info_t * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
		#ifdef ZEND_ENGINE_2_4
		
	assert(sizeof(zend_uint) == sizeof(src->literalinfo_cnt));
		
	
	
		add_assoc_long_ex(dst, ("literalinfo_cnt"), (sizeof("literalinfo_cnt")), src->literalinfo_cnt);
		
	
	
	
	
	


	

	
	if (src->literalinfos) {
		zend_uint i; 
		
	

		
			zval *arr;
			ALLOC_INIT_ZVAL(arr);
			array_init(arr);

			for (i = 0;
					i < src->literalinfo_cnt;
					++i) {
				zval *zv;

				ALLOC_INIT_ZVAL(zv);
				array_init(zv);
				xc_dasm_xc_op_array_info_detail_t (dasm, zv, &(src->literalinfos[i]) TSRMLS_CC);
				add_next_index_zval(arr, zv);
			}
			add_assoc_zval_ex(dst, ("literalinfos"), (sizeof("literalinfos")), arr);
				
		
	
	
	

		
		
	}
	else {
		
	
	add_assoc_null_ex(dst, ("literalinfos"), (sizeof("literalinfos")));
	
	assert(src->literalinfos == NULL);

	
	
	


	}

#else
		
	assert(sizeof(zend_uint) == sizeof(src->oplineinfo_cnt));
		
	
	
		add_assoc_long_ex(dst, ("oplineinfo_cnt"), (sizeof("oplineinfo_cnt")), src->oplineinfo_cnt);
		
	
	
	
	
	


	

	
	if (src->oplineinfos) {
		zend_uint i; 
		
	

		
			zval *arr;
			ALLOC_INIT_ZVAL(arr);
			array_init(arr);

			for (i = 0;
					i < src->oplineinfo_cnt;
					++i) {
				zval *zv;

				ALLOC_INIT_ZVAL(zv);
				array_init(zv);
				xc_dasm_xc_op_array_info_detail_t (dasm, zv, &(src->oplineinfos[i]) TSRMLS_CC);
				add_next_index_zval(arr, zv);
			}
			add_assoc_zval_ex(dst, ("oplineinfos"), (sizeof("oplineinfos")), arr);
				
		
	
	
	

		
		
	}
	else {
		
	
	add_assoc_null_ex(dst, ("oplineinfos"), (sizeof("oplineinfos")));
	
	assert(src->oplineinfos == NULL);

	
	
	


	}

#endif

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	



	
	

/* {{{ xc_dasm_xc_funcinfo_t */
	
		/* export:  	  	 	void 	xc_dasm_xc_funcinfo_t( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const xc_funcinfo_t * const src 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dasm_xc_funcinfo_t( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const xc_funcinfo_t * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(zend_uint) == sizeof(src->key_size));
		
	
	
		add_assoc_long_ex(dst, ("key_size"), (sizeof("key_size")), src->key_size);
		
	
	
	
	
	


	

#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
	
		add_assoc_long_ex(dst, ("type"), (sizeof("type")), src->type);
		
	
	
	
	
	


	

#endif
	
		
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (src->type == IS_UNICODE) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->key) == NULL) {
		
		
			add_assoc_null_ex(dst, ("key"), (sizeof("key")));
		
	}
	else {
		
		
		
		
		
		
			
				add_assoc_unicodel_ex(dst, ("key"), (sizeof("key")), ZSTR_U(src->key), src->key_size-1, 1);
				
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
			add_assoc_null_ex(dst, ("key"), (sizeof("key")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("key"), (sizeof("key")), (char *) ZSTR_S(src->key), src->key_size-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
			add_assoc_null_ex(dst, ("key"), (sizeof("key")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("key"), (sizeof("key")), (char *) ZSTR_S(src->key), src->key_size-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
	
		add_assoc_long_ex(dst, ("h"), (sizeof("h")), src->h);
		
	
	
	
	
	


	

	
		
	
	assert(sizeof(xc_op_array_info_t) == sizeof(src->op_array_info));
	
	
	
	
	

	
	assert(sizeof(xc_op_array_info_t) == sizeof((& src->op_array_info)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_xc_op_array_info_t(
		
		
		
		
		dasm, zv, & src->op_array_info
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("op_array_info"), (sizeof("op_array_info")), zv);
		
	} while (0);
	

	
	

	
	
	
	


	
	
	
	
	assert(sizeof(zend_function) == sizeof(src->func));
	
	
	
	
	

	
	assert(sizeof(zend_function) == sizeof((& src->func)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_zend_function(
		
		
		
		
		dasm, zv, & src->func
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("func"), (sizeof("func")), zv);
		
	} while (0);
	

	
	

	
	
	
	



		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dasm_xc_classinfo_t */
	
		/* export:  	  	 	void 	xc_dasm_xc_classinfo_t( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const xc_classinfo_t * const src 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dasm_xc_classinfo_t( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const xc_classinfo_t * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(zend_uint) == sizeof(src->key_size));
		
	
	
		add_assoc_long_ex(dst, ("key_size"), (sizeof("key_size")), src->key_size);
		
	
	
	
	
	


	

#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
	
		add_assoc_long_ex(dst, ("type"), (sizeof("type")), src->type);
		
	
	
	
	
	


	

#endif
	
		
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (src->type == IS_UNICODE) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->key) == NULL) {
		
		
			add_assoc_null_ex(dst, ("key"), (sizeof("key")));
		
	}
	else {
		
		
		
		
		
		
			
				add_assoc_unicodel_ex(dst, ("key"), (sizeof("key")), ZSTR_U(src->key), src->key_size-1, 1);
				
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
			add_assoc_null_ex(dst, ("key"), (sizeof("key")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("key"), (sizeof("key")), (char *) ZSTR_S(src->key), src->key_size-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
			add_assoc_null_ex(dst, ("key"), (sizeof("key")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("key"), (sizeof("key")), (char *) ZSTR_S(src->key), src->key_size-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
	
		add_assoc_long_ex(dst, ("h"), (sizeof("h")), src->h);
		
	
	
	
	
	


	

		
	assert(sizeof(zend_uint) == sizeof(src->methodinfo_cnt));
		
	
	
		add_assoc_long_ex(dst, ("methodinfo_cnt"), (sizeof("methodinfo_cnt")), src->methodinfo_cnt);
		
	
	
	
	
	


	

	
		
	if (src->methodinfos) {
		zend_uint i; 
		
	

		
			zval *arr;
			ALLOC_INIT_ZVAL(arr);
			array_init(arr);

			for (i = 0;
					i < src->methodinfo_cnt;
					++i) {
				zval *zv;

				ALLOC_INIT_ZVAL(zv);
				array_init(zv);
				xc_dasm_xc_op_array_info_t (dasm, zv, &(src->methodinfos[i]) TSRMLS_CC);
				add_next_index_zval(arr, zv);
			}
			add_assoc_zval_ex(dst, ("methodinfos"), (sizeof("methodinfos")), arr);
				
		
	
	
	

		
		
	}
	else {
		
	
	add_assoc_null_ex(dst, ("methodinfos"), (sizeof("methodinfos")));
	
	assert(src->methodinfos == NULL);

	
	
	


	}

	
	
#ifdef ZEND_ENGINE_2
	
	
	if (src->cest) {
		
		
	
	
	

	
	assert(sizeof(zend_class_entry) == sizeof(( src->cest)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_zend_class_entry(
		
		
		
		
		dasm, zv,  src->cest
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("cest"), (sizeof("cest")), zv);
		
	} while (0);
	

	
	

	}
	else {
		
		
	add_assoc_null_ex(dst, ("cest"), (sizeof("cest")));
	
	assert(src->cest == NULL);

	}
	
	
	
	


#else
	
	
	assert(sizeof(zend_class_entry) == sizeof(src->cest));
	
	
	
	
	

	
	assert(sizeof(zend_class_entry) == sizeof((& src->cest)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_zend_class_entry(
		
		
		
		
		dasm, zv, & src->cest
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("cest"), (sizeof("cest")), zv);
		
	} while (0);
	

	
	

	
	
	
	


#endif
#ifndef ZEND_COMPILE_DELAYED_BINDING
		
	assert(sizeof(int) == sizeof(src->oplineno));
		
	
	
		add_assoc_long_ex(dst, ("oplineno"), (sizeof("oplineno")), src->oplineno);
		
	
	
	
	
	


	

#endif

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


#ifdef ZEND_ENGINE_2_1

	
	

/* {{{ xc_dasm_xc_autoglobal_t */
	
 	  	 	static void inline 	xc_dasm_xc_autoglobal_t( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const xc_autoglobal_t * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(zend_uint) == sizeof(src->key_len));
		
	
	
		add_assoc_long_ex(dst, ("key_len"), (sizeof("key_len")), src->key_len);
		
	
	
	
	
	


	

#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->type));
		
	
	
		add_assoc_long_ex(dst, ("type"), (sizeof("type")), src->type);
		
	
	
	
	
	


	

#endif
	
		 
	
#ifdef IS_UNICODE
	
	
	
	
	

	
		if (src->type == IS_UNICODE) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->key) == NULL) {
		
		
			add_assoc_null_ex(dst, ("key"), (sizeof("key")));
		
	}
	else {
		
		
		
		
		
		
			
				add_assoc_unicodel_ex(dst, ("key"), (sizeof("key")), ZSTR_U(src->key), src->key_len + 1-1, 1);
				
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
			add_assoc_null_ex(dst, ("key"), (sizeof("key")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("key"), (sizeof("key")), (char *) ZSTR_S(src->key), src->key_len + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	

	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->key) == NULL) {
		
		
			add_assoc_null_ex(dst, ("key"), (sizeof("key")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("key"), (sizeof("key")), (char *) ZSTR_S(src->key), src->key_len + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

#endif
	

	
		
	assert(sizeof(ulong) == sizeof(src->h));
		
	
	
		add_assoc_long_ex(dst, ("h"), (sizeof("h")), src->h);
		
	
	
	
	
	


	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif


#ifdef XCACHE_ERROR_CACHING

	
	

/* {{{ xc_dasm_xc_compilererror_t */
	
 	  	 	static void inline 	xc_dasm_xc_compilererror_t( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const xc_compilererror_t * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
				
	assert(sizeof(int) == sizeof(src->type));
		
	
	
		add_assoc_long_ex(dst, ("type"), (sizeof("type")), src->type);
		
	
	
	
	
	


	

		
	assert(sizeof(uint) == sizeof(src->lineno));
		
	
	
		add_assoc_long_ex(dst, ("lineno"), (sizeof("lineno")), src->lineno);
		
	
	
	
	
	


	

		
	assert(sizeof(int) == sizeof(src->error_len));
		
	
	
		add_assoc_long_ex(dst, ("error_len"), (sizeof("error_len")), src->error_len);
		
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->error == NULL) {
		
		
			add_assoc_null_ex(dst, ("error"), (sizeof("error")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("error"), (sizeof("error")), (char *) src->error, src->error_len + 1-1, 1);
				
			
		
	}
	
	
	
	
	
	
	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	

#endif


	
	

/* {{{ xc_dasm_xc_entry_data_php_t */
	
		/* export:  	  	 	void 	xc_dasm_xc_entry_data_php_t( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const xc_entry_data_php_t * const src 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dasm_xc_entry_data_php_t( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const xc_entry_data_php_t * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			

	/* skip */
	
	
	
	

		
	assert(sizeof(xc_hash_value_t) == sizeof(src->hvalue));
		
	
	
		add_assoc_long_ex(dst, ("hvalue"), (sizeof("hvalue")), src->hvalue);
		
	
	
	
	
	


	

		
	assert(sizeof(xc_md5sum_t) == sizeof(src->md5));
	
	
	
	

	

		
	assert(sizeof(zend_ulong) == sizeof(src->refcount));
		
	
	
		add_assoc_long_ex(dst, ("refcount"), (sizeof("refcount")), src->refcount);
		
	
	
	
	
	


	


		
	assert(sizeof(zend_ulong) == sizeof(src->hits));
		
	
	
		add_assoc_long_ex(dst, ("hits"), (sizeof("hits")), src->hits);
		
	
	
	
	
	


	

		
	assert(sizeof(size_t) == sizeof(src->size));
		
	
	
		add_assoc_long_ex(dst, ("size"), (sizeof("size")), src->size);
		
	
	
	
	
	


	


	
		
	
	assert(sizeof(xc_op_array_info_t) == sizeof(src->op_array_info));
	
	
	
	
	

	
	assert(sizeof(xc_op_array_info_t) == sizeof((& src->op_array_info)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_xc_op_array_info_t(
		
		
		
		
		dasm, zv, & src->op_array_info
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("op_array_info"), (sizeof("op_array_info")), zv);
		
	} while (0);
	

	
	

	
	
	
	


	
	
	
	
	if (src->op_array) {
		
		
	
	
	

	
	assert(sizeof(zend_op_array) == sizeof(( src->op_array)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_zend_op_array(
		
		
		
		
		dasm, zv,  src->op_array
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("op_array"), (sizeof("op_array")), zv);
		
	} while (0);
	

	
	

	}
	else {
		
		
	add_assoc_null_ex(dst, ("op_array"), (sizeof("op_array")));
	
	assert(src->op_array == NULL);

	}
	
	
	
	



#ifdef HAVE_XCACHE_CONSTANT
		
	assert(sizeof(zend_uint) == sizeof(src->constinfo_cnt));
		
	
	
		add_assoc_long_ex(dst, ("constinfo_cnt"), (sizeof("constinfo_cnt")), src->constinfo_cnt);
		
	
	
	
	
	


	

	
	if (src->constinfos) {
		zend_uint i; 
		
	

		
			zval *arr;
			ALLOC_INIT_ZVAL(arr);
			array_init(arr);

			for (i = 0;
					i < src->constinfo_cnt;
					++i) {
				zval *zv;

				ALLOC_INIT_ZVAL(zv);
				array_init(zv);
				xc_dasm_xc_constinfo_t (dasm, zv, &(src->constinfos[i]) TSRMLS_CC);
				add_next_index_zval(arr, zv);
			}
			add_assoc_zval_ex(dst, ("constinfos"), (sizeof("constinfos")), arr);
				
		
	
	
	

		
		
	}
	else {
		
	
	add_assoc_null_ex(dst, ("constinfos"), (sizeof("constinfos")));
	
	assert(src->constinfos == NULL);

	
	
	


	}

#endif

		
	assert(sizeof(zend_uint) == sizeof(src->funcinfo_cnt));
		
	
	
		add_assoc_long_ex(dst, ("funcinfo_cnt"), (sizeof("funcinfo_cnt")), src->funcinfo_cnt);
		
	
	
	
	
	


	

	
	if (src->funcinfos) {
		zend_uint i; 
		
	

		
			zval *arr;
			ALLOC_INIT_ZVAL(arr);
			array_init(arr);

			for (i = 0;
					i < src->funcinfo_cnt;
					++i) {
				zval *zv;

				ALLOC_INIT_ZVAL(zv);
				array_init(zv);
				xc_dasm_xc_funcinfo_t (dasm, zv, &(src->funcinfos[i]) TSRMLS_CC);
				add_next_index_zval(arr, zv);
			}
			add_assoc_zval_ex(dst, ("funcinfos"), (sizeof("funcinfos")), arr);
				
		
	
	
	

		
		
	}
	else {
		
	
	add_assoc_null_ex(dst, ("funcinfos"), (sizeof("funcinfos")));
	
	assert(src->funcinfos == NULL);

	
	
	


	}


		
	assert(sizeof(zend_uint) == sizeof(src->classinfo_cnt));
		
	
	
		add_assoc_long_ex(dst, ("classinfo_cnt"), (sizeof("classinfo_cnt")), src->classinfo_cnt);
		
	
	
	
	
	


	

	
	if (src->classinfos) {
		zend_uint i; 
		
	

		
			zval *arr;
			ALLOC_INIT_ZVAL(arr);
			array_init(arr);

			for (i = 0;
					i < src->classinfo_cnt;
					++i) {
				zval *zv;

				ALLOC_INIT_ZVAL(zv);
				array_init(zv);
				xc_dasm_xc_classinfo_t (dasm, zv, &(src->classinfos[i]) TSRMLS_CC);
				add_next_index_zval(arr, zv);
			}
			add_assoc_zval_ex(dst, ("classinfos"), (sizeof("classinfos")), arr);
				
		
	
	
	

		
		
	}
	else {
		
	
	add_assoc_null_ex(dst, ("classinfos"), (sizeof("classinfos")));
	
	assert(src->classinfos == NULL);

	
	
	


	}

#ifdef ZEND_ENGINE_2_1
		
	assert(sizeof(zend_uint) == sizeof(src->autoglobal_cnt));
		
	
	
		add_assoc_long_ex(dst, ("autoglobal_cnt"), (sizeof("autoglobal_cnt")), src->autoglobal_cnt);
		
	
	
	
	
	


	

	
		
	if (src->autoglobals) {
		zend_uint i; 
		
	

		
			zval *arr;
			ALLOC_INIT_ZVAL(arr);
			array_init(arr);

			for (i = 0;
					i < src->autoglobal_cnt;
					++i) {
				zval *zv;

				ALLOC_INIT_ZVAL(zv);
				array_init(zv);
				xc_dasm_xc_autoglobal_t (dasm, zv, &(src->autoglobals[i]) TSRMLS_CC);
				add_next_index_zval(arr, zv);
			}
			add_assoc_zval_ex(dst, ("autoglobals"), (sizeof("autoglobals")), arr);
				
		
	
	
	

		
		
	}
	else {
		
	
	add_assoc_null_ex(dst, ("autoglobals"), (sizeof("autoglobals")));
	
	assert(src->autoglobals == NULL);

	
	
	


	}

	
#endif
#ifdef XCACHE_ERROR_CACHING
		
	assert(sizeof(zend_uint) == sizeof(src->compilererror_cnt));
		
	
	
		add_assoc_long_ex(dst, ("compilererror_cnt"), (sizeof("compilererror_cnt")), src->compilererror_cnt);
		
	
	
	
	
	


	

	
		
	if (src->compilererrors) {
		zend_uint i; 
		
	

		
			zval *arr;
			ALLOC_INIT_ZVAL(arr);
			array_init(arr);

			for (i = 0;
					i < src->compilererror_cnt;
					++i) {
				zval *zv;

				ALLOC_INIT_ZVAL(zv);
				array_init(zv);
				xc_dasm_xc_compilererror_t (dasm, zv, &(src->compilererrors[i]) TSRMLS_CC);
				add_next_index_zval(arr, zv);
			}
			add_assoc_zval_ex(dst, ("compilererrors"), (sizeof("compilererrors")), arr);
				
		
	
	
	

		
		
	}
	else {
		
	
	add_assoc_null_ex(dst, ("compilererrors"), (sizeof("compilererrors")));
	
	assert(src->compilererrors == NULL);

	
	
	


	}

	
#endif
#ifndef ZEND_COMPILE_DELAYED_BINDING
		
	assert(sizeof(zend_bool) == sizeof(src->have_early_binding));
		
	
	
		add_assoc_bool_ex(dst, ("have_early_binding"), (sizeof("have_early_binding")), src->have_early_binding ? 1 : 0);
		
	
	
	
	
	


	

#endif
		
	assert(sizeof(zend_bool) == sizeof(src->have_references));
		
	
	
		add_assoc_bool_ex(dst, ("have_references"), (sizeof("have_references")), src->have_references ? 1 : 0);
		
	
	
	
	
	


	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dasm_xc_entry_t */
	
 	  	 	static void inline 	xc_dasm_xc_entry_t( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const xc_entry_t * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			/* skip */
	
	
	
	

		
	assert(sizeof(size_t) == sizeof(src->size));
		
	
	
		add_assoc_long_ex(dst, ("size"), (sizeof("size")), src->size);
		
	
	
	
	
	


	


		
	assert(sizeof(time_t) == sizeof(src->ctime));
		
	
	
		add_assoc_long_ex(dst, ("ctime"), (sizeof("ctime")), src->ctime);
		
	
	
	
	
	


	

		
	assert(sizeof(time_t) == sizeof(src->atime));
		
	
	
		add_assoc_long_ex(dst, ("atime"), (sizeof("atime")), src->atime);
		
	
	
	
	
	


	

		
	assert(sizeof(time_t) == sizeof(src->dtime));
		
	
	
		add_assoc_long_ex(dst, ("dtime"), (sizeof("dtime")), src->dtime);
		
	
	
	
	
	


	

		
	assert(sizeof(long) == sizeof(src->ttl));
		
	
	
		add_assoc_long_ex(dst, ("ttl"), (sizeof("ttl")), src->ttl);
		
	
	
	
	
	


	

		
	assert(sizeof(zend_ulong) == sizeof(src->hits));
		
	
	
		add_assoc_long_ex(dst, ("hits"), (sizeof("hits")), src->hits);
		
	
	
	
	
	


	

	
	
	
	
 
		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dasm_xc_entry_php_t */
	
		/* export:  	  	 	void 	xc_dasm_xc_entry_php_t( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const xc_entry_php_t * const src 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dasm_xc_entry_php_t( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const xc_entry_php_t * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
	
	assert(sizeof(xc_entry_t) == sizeof(src->entry));
	
	
	
	
	

	
	assert(sizeof(xc_entry_t) == sizeof((& src->entry)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_xc_entry_t(
		
		
		
		
		dasm, zv, & src->entry
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("entry"), (sizeof("entry")), zv);
		
	} while (0);
	

	
	

	
	
	
	


	
	
	

			
	assert(sizeof(int) == sizeof(src->entry.name.str.len));
		
	
	
		add_assoc_long_ex(dst, ("entry.name.str.len"), (sizeof("entry.name.str.len")), src->entry.name.str.len);
		
	
	

	

		
			  
	
	
	
	
	
	
	
	
	if (src->entry.name.str.val == NULL) {
		
		
			add_assoc_null_ex(dst, ("entry.name.str.val"), (sizeof("entry.name.str.val")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("entry.name.str.val"), (sizeof("entry.name.str.val")), (char *) src->entry.name.str.val, src->entry.name.str.len + 1-1, 1);
				
			
		
	}
	
	
	
	
	
	
	

		
	
	
	


	
	
	if (src->php) {
		
		
	
	
	

	
	assert(sizeof(xc_entry_data_php_t) == sizeof(( src->php)[0]));
	
	
	
		
	
	
	/* allocate */
	
	
	
	
	
	


	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_xc_entry_data_php_t(
		
		
		
		
		dasm, zv,  src->php
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("php"), (sizeof("php")), zv);
		
	} while (0);
	

	
	

	}
	else {
		
		
	add_assoc_null_ex(dst, ("php"), (sizeof("php")));
	
	assert(src->php == NULL);

	}
	
	
	
	



		
	assert(sizeof(long) == sizeof(src->refcount));
		
	
	
		add_assoc_long_ex(dst, ("refcount"), (sizeof("refcount")), src->refcount);
		
	
	
	
	
	


	

		
	assert(sizeof(time_t) == sizeof(src->file_mtime));
		
	
	
		add_assoc_long_ex(dst, ("file_mtime"), (sizeof("file_mtime")), src->file_mtime);
		
	
	
	
	
	


	

		
	assert(sizeof(size_t) == sizeof(src->file_size));
		
	
	
		add_assoc_long_ex(dst, ("file_size"), (sizeof("file_size")), src->file_size);
		
	
	
	
	
	


	

		
	assert(sizeof(size_t) == sizeof(src->file_device));
		
	
	
		add_assoc_long_ex(dst, ("file_device"), (sizeof("file_device")), src->file_device);
		
	
	
	
	
	


	

		
	assert(sizeof(size_t) == sizeof(src->file_inode));
		
	
	
		add_assoc_long_ex(dst, ("file_inode"), (sizeof("file_inode")), src->file_inode);
		
	
	
	
	
	


	


		
	assert(sizeof(size_t) == sizeof(src->filepath_len));
		
	
	
		add_assoc_long_ex(dst, ("filepath_len"), (sizeof("filepath_len")), src->filepath_len);
		
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->filepath == NULL) {
		
		
			add_assoc_null_ex(dst, ("filepath"), (sizeof("filepath")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("filepath"), (sizeof("filepath")), (char *) src->filepath, src->filepath_len + 1-1, 1);
				
			
		
	}
	
	
	
	
	
	
	

		
	assert(sizeof(size_t) == sizeof(src->dirpath_len));
		
	
	
		add_assoc_long_ex(dst, ("dirpath_len"), (sizeof("dirpath_len")), src->dirpath_len);
		
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->dirpath == NULL) {
		
		
			add_assoc_null_ex(dst, ("dirpath"), (sizeof("dirpath")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("dirpath"), (sizeof("dirpath")), (char *) src->dirpath, src->dirpath_len + 1-1, 1);
				
			
		
	}
	
	
	
	
	
	
	

#ifdef IS_UNICODE
		
	assert(sizeof(int) == sizeof(src->ufilepath_len));
		
	
	
		add_assoc_long_ex(dst, ("ufilepath_len"), (sizeof("ufilepath_len")), src->ufilepath_len);
		
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->ufilepath == NULL) {
		
		
			add_assoc_null_ex(dst, ("ufilepath"), (sizeof("ufilepath")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("ufilepath"), (sizeof("ufilepath")), (char *) src->ufilepath, src->ufilepath_len + 1-1, 1);
				
			
		
	}
	
	
	
	
	
	
	

		
	assert(sizeof(int) == sizeof(src->udirpath_len));
		
	
	
		add_assoc_long_ex(dst, ("udirpath_len"), (sizeof("udirpath_len")), src->udirpath_len);
		
	
	
	
	
	


	

	  
	
	
	

	
	
	
	
	
	
	
	
	if (src->udirpath == NULL) {
		
		
			add_assoc_null_ex(dst, ("udirpath"), (sizeof("udirpath")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("udirpath"), (sizeof("udirpath")), (char *) src->udirpath, src->udirpath_len + 1-1, 1);
				
			
		
	}
	
	
	
	
	
	
	

#endif

		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


	
	

/* {{{ xc_dasm_xc_entry_var_t */
	
		/* export:  	  	 	void 	xc_dasm_xc_entry_var_t( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const xc_entry_var_t * const src 		TSRMLS_DC 	); 	 :export */
	
 	  	 	void 	xc_dasm_xc_entry_var_t( 		 		 		 		 		xc_dasm_t *dasm, zval *dst, const xc_entry_var_t * const src 		TSRMLS_DC 	) 	
	{
		
		
		

		

		
			
	
	assert(sizeof(xc_entry_t) == sizeof(src->entry));
	
	
	
	
	

	
	assert(sizeof(xc_entry_t) == sizeof((& src->entry)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_xc_entry_t(
		
		
		
		
		dasm, zv, & src->entry
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("entry"), (sizeof("entry")), zv);
		
	} while (0);
	

	
	

	
	
	
	



#ifdef IS_UNICODE
		
	assert(sizeof(zend_uchar) == sizeof(src->name_type));
		
	
	
		add_assoc_long_ex(dst, ("name_type"), (sizeof("name_type")), src->name_type);
		
	
	
	
	
	


	

#endif
		
	
	

#ifdef IS_UNICODE
		if (src->name_type == IS_UNICODE) {
				
	assert(sizeof(int32_t) == sizeof(src->entry.name.ustr.len));
		
	
	
		add_assoc_long_ex(dst, ("entry.name.ustr.len"), (sizeof("entry.name.ustr.len")), src->entry.name.ustr.len);
		
	
	

	

		}
		else {
				
	assert(sizeof(int) == sizeof(src->entry.name.str.len));
		
	
	
		add_assoc_long_ex(dst, ("entry.name.str.len"), (sizeof("entry.name.str.len")), src->entry.name.str.len);
		
	
	

	

		}
#else
			
	assert(sizeof(int) == sizeof(src->entry.name.str.len));
		
	
	
		add_assoc_long_ex(dst, ("entry.name.str.len"), (sizeof("entry.name.str.len")), src->entry.name.str.len);
		
	
	

	

#endif
		
#ifdef IS_UNICODE
			 
	
#ifdef IS_UNICODE
	
	
	
		if (src->name_type == IS_UNICODE) {
			
	
	
	
	
	
	
	
	
	if (ZSTR_U(src->entry.name.uni.val) == NULL) {
		
		
			add_assoc_null_ex(dst, ("entry.name.uni.val"), (sizeof("entry.name.uni.val")));
		
	}
	else {
		
		
		
		
		
		
			
				add_assoc_unicodel_ex(dst, ("entry.name.uni.val"), (sizeof("entry.name.uni.val")), ZSTR_U(src->entry.name.uni.val), src->entry.name.uni.len + 1-1, 1);
				
		
	}
	
	
	
	
	
	
	

		}
		else {
			
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->entry.name.uni.val) == NULL) {
		
		
			add_assoc_null_ex(dst, ("entry.name.uni.val"), (sizeof("entry.name.uni.val")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("entry.name.uni.val"), (sizeof("entry.name.uni.val")), (char *) ZSTR_S(src->entry.name.uni.val), src->entry.name.uni.len + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

		}
	
#else
	
	
	
	
	
	
	
	
	
	
	if (ZSTR_S(src->entry.name.uni.val) == NULL) {
		
		
			add_assoc_null_ex(dst, ("entry.name.uni.val"), (sizeof("entry.name.uni.val")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("entry.name.uni.val"), (sizeof("entry.name.uni.val")), (char *) ZSTR_S(src->entry.name.uni.val), src->entry.name.uni.len + 1-1, 1);
					
			
		
	}
	
	
	
	
	
	
	

#endif
	

#else
			  
	
	
	
	
	
	
	
	
	if (src->entry.name.str.val == NULL) {
		
		
			add_assoc_null_ex(dst, ("entry.name.str.val"), (sizeof("entry.name.str.val")));
		
	}
	else {
		
		
		
		
		
		
			 				
					add_assoc_stringl_ex(dst, ("entry.name.str.val"), (sizeof("entry.name.str.val")), (char *) src->entry.name.str.val, src->entry.name.str.len + 1-1, 1);
				
			
		
	}
	
	
	
	
	
	
	

#endif
		
	
	
	

	
	
	
	
	
	

	
	assert(sizeof(zval_ptr) == sizeof((& src->value)[0]));
	

	do {
		zval *zv;
		ALLOC_INIT_ZVAL(zv);
		array_init(zv);
	

	xc_dasm_zval_ptr(
		
		
		
		
		dasm, zv, & src->value
		TSRMLS_CC
	);

	
		
			add_assoc_zval_ex(dst, ("value"), (sizeof("value")), zv);
		
	} while (0);
	

	
	

		
	assert(sizeof(zend_bool) == sizeof(src->have_references));
		
	
	
		add_assoc_bool_ex(dst, ("have_references"), (sizeof("have_references")), src->have_references ? 1 : 0);
		
	
	
	
	
	


	

	
	
	
	


		
		
		
			
			
			
		
		
		
		
	}
/* }}} */
	


#endif /* HAVE_XCACHE_DISASSEMBLER */


