#ifndef PHP_HELLO_H
#define PHP_HELLO_H 1
#include "php.h"

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(hello)
long counter;
zend_bool direction;
ZEND_END_MODULE_GLOBALS(hello)

#ifdef ZTS
#define HELLO_G(v) TSRMG(hello_globals_id, zend_hello_globals *, v)
#else
#define HELLO_G(v) (hello_globals.v)
#endif

#define PHP_HELLO_WORLD_VERSION "1.0"
#define PHP_HELLO_WORLD_EXTNAME "hello"

typedef struct _php_hello_person {
    char *name;
    int name_len;
    long age;
} php_hello_person;

#define PHP_HELLO_PERSON_RES_NAME "Person Data"

PHP_MINIT_FUNCTION(hello);
PHP_MSHUTDOWN_FUNCTION(hello);
PHP_RINIT_FUNCTION(hello);

PHP_FUNCTION(hello_world);
PHP_FUNCTION(hello_long);
PHP_FUNCTION(hello_double);
PHP_FUNCTION(hello_bool);
PHP_FUNCTION(hello_null);
PHP_FUNCTION(hello_greetme);
PHP_FUNCTION(hello_add);
PHP_FUNCTION(hello_dump);
PHP_FUNCTION(hello_array);
PHP_FUNCTION(hello_array_strings);
PHP_FUNCTION(hello_array_walk);
PHP_FUNCTION(hello_array_value);
PHP_FUNCTION(hello_get_global_var);
PHP_FUNCTION(hello_set_local_var);
PHP_FUNCTION(hello_person_new);
PHP_FUNCTION(hello_person_pnew);
PHP_FUNCTION(hello_person_greet);
PHP_FUNCTION(hello_person_delete);

extern zend_module_entry hello_module_entry;
#define phpext_hello_ptr &hello_module_entry

#endif