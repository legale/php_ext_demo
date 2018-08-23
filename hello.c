#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "php.h"
#include "zend_list.h"
#include "php_ini.h"
#include "hello.h"

int le_hello_person;
int le_hello_person_persist;

ZEND_DECLARE_MODULE_GLOBALS(hello)

static zend_function_entry hello_functions[] = {
        PHP_FE(hello_world, NULL)
        PHP_FE(hello_long, NULL)
        PHP_FE(hello_double, NULL)
        PHP_FE(hello_bool, NULL)
        PHP_FE(hello_null, NULL)
        PHP_FE(hello_greetme, NULL)
        PHP_FE(hello_add, NULL)
        PHP_FE(hello_dump, NULL)
        PHP_FE(hello_array, NULL)
        PHP_FE(hello_array_strings, NULL)
        PHP_FE(hello_array_walk, NULL)
        PHP_FE(hello_array_value, NULL)
        PHP_FE(hello_get_global_var, NULL)
        PHP_FE(hello_set_local_var, NULL)
        PHP_FE(hello_person_new, NULL)
        PHP_FE(hello_person_pnew, NULL)
        PHP_FE(hello_person_greet, NULL)
        PHP_FE(hello_person_delete, NULL)
        {NULL, NULL, NULL}
};

zend_module_entry hello_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
        STANDARD_MODULE_HEADER,
#endif
        PHP_HELLO_WORLD_EXTNAME,
        hello_functions,
        PHP_MINIT(hello),
        PHP_MSHUTDOWN(hello),
        PHP_RINIT(hello),
        NULL,
        NULL,
#if ZEND_MODULE_API_NO >= 20010901
        PHP_HELLO_WORLD_VERSION,
#endif
        STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_HELLO
ZEND_GET_MODULE(hello)
#endif

PHP_INI_BEGIN()
                PHP_INI_ENTRY("hello.greeting", "Hello World", PHP_INI_ALL, NULL)
                STD_PHP_INI_ENTRY("hello.direction", "1", PHP_INI_ALL, OnUpdateBool, direction, zend_hello_globals, hello_globals)
PHP_INI_END()

static void php_hello_init_globals(zend_hello_globals *hello_globals)
{
    hello_globals->direction = 1;
}

PHP_RINIT_FUNCTION(hello)
{
    HELLO_G(counter) = 0;

    return SUCCESS;
}

static void php_hello_person_persist_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    php_hello_person *person = (php_hello_person*)rsrc->ptr;

    if (person) {
        if (person->name) {
            pefree(person->name, 1);
        }
        pefree(person, 1);
    }
}

static void php_hello_person_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    php_hello_person *person = (php_hello_person*)rsrc->ptr;

    if (person) {
        if (person->name) {
            efree(person->name);
        }
        efree(person);
    }
}

PHP_MINIT_FUNCTION(hello)
{
    le_hello_person = zend_register_list_destructors_ex(php_hello_person_dtor, NULL, PHP_HELLO_PERSON_RES_NAME, module_number);
    le_hello_person_persist = zend_register_list_destructors_ex (NULL, php_hello_person_persist_dtor, PHP_HELLO_PERSON_RES_NAME, module_number);

    ZEND_INIT_MODULE_GLOBALS(hello, php_hello_init_globals, NULL);

    REGISTER_INI_ENTRIES();

    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(hello)
{
    UNREGISTER_INI_ENTRIES();

    return SUCCESS;
}

PHP_FUNCTION(hello_world)
{
    RETURN_STRING("Hello World", 1);
}

PHP_FUNCTION(hello_long)
{
    if (HELLO_G(direction)) {
        HELLO_G(counter)++;
    } else {
        HELLO_G(counter)--;
    }

    RETURN_LONG(HELLO_G(counter));
}

PHP_FUNCTION(hello_double)
{
    RETURN_DOUBLE(3.1415926535);
}

PHP_FUNCTION(hello_bool)
{
    RETURN_BOOL(1);
}

PHP_FUNCTION(hello_null)
{
    RETURN_NULL();
}

PHP_FUNCTION(hello_greetme)
{
    zval *zname;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zname) == FAILURE) {
        RETURN_NULL();
    }

    convert_to_string(zname);

    php_printf("Hello ");
    PHPWRITE(Z_STRVAL_P(zname), Z_STRLEN_P(zname));
    php_printf("");

    RETURN_TRUE;
}

PHP_FUNCTION(hello_add)
{
    long a;
    double b;
    zend_bool return_long = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ld|b", &a, &b, &return_long) == FAILURE) {
        RETURN_NULL();
    }

    if (return_long) {
        RETURN_LONG(a + b);
    } else {
        RETURN_DOUBLE(a + b);
    }
}

PHP_FUNCTION(hello_dump)
{
    zval *uservar;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &uservar) == FAILURE) {
        RETURN_NULL();
    }

    switch (Z_TYPE_P(uservar)) {
        case IS_NULL:
            php_printf("NULL
                       ");
            break;
        case IS_BOOL:
            php_printf("Boolean: %s
                       ", Z_LVAL_P(uservar) ? "TRUE" : "FALSE");
            break;
        case IS_LONG:
            php_printf("Long: %ld
                       ", Z_LVAL_P(uservar));
            break;
        case IS_DOUBLE:
            php_printf("Double: %f
                       ", Z_DVAL_P(uservar));
            break;
        case IS_STRING:
            php_printf("String: ");
            PHPWRITE(Z_STRVAL_P(uservar), Z_STRLEN_P(uservar));
            php_printf("
                       ");
            break;
        case IS_RESOURCE:
            php_printf("Resource
                       ");
            break;
        case IS_ARRAY:
            php_printf("Array
                       ");
            break;
        case IS_OBJECT:
            php_printf("Object
                       ");
            break;
        default:
            php_printf("Unknown
                       ");
    }

    RETURN_TRUE;
}

PHP_FUNCTION(hello_array)
{
    char *mystr;
    zval *mysubarray;

    array_init(return_value);

    add_index_long(return_value, 42, 123);
    add_next_index_string(return_value, "I should now be found at index 43", 1);
    add_next_index_stringl(return_value, "I'm at 44!", 10, 1);
    mystr = estrdup("Forty Five");
    add_next_index_string(return_value, mystr, 0);
    add_assoc_double(return_value, "pi", 3.1415926535);

    ALLOC_INIT_ZVAL(mysubarray);
    array_init(mysubarray);

    add_next_index_string(mysubarray, "hello", 1);
    php_printf("mysubarray->refcount = %d
               ", mysubarray->refcount);
    mysubarray->refcount = 2;
    php_printf("mysubarray->refcount = %d
               ", mysubarray->refcount);
    add_assoc_zval(return_value, "subarray", mysubarray);
    php_printf("mysubarray->refcount = %d
               ", mysubarray->refcount);
}

PHP_FUNCTION(hello_array_strings)
{
    zval *arr, **data;
    HashTable *arr_hash;
    HashPosition pointer;
    int array_count;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &arr) == FAILURE) {
        RETURN_NULL();
    }

    arr_hash = Z_ARRVAL_P(arr);
    array_count = zend_hash_num_elements(arr_hash);

    php_printf("The array passed contains %d elements
               ", array_count);

    for(zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); zend_hash_get_current_data_ex(arr_hash, (void**) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(arr_hash, &pointer)) {

        zval temp;
        char *key;
        int key_len;
        long index;

        if (zend_hash_get_current_key_ex(arr_hash, &key, &key_len, &index, 0, &pointer) == HASH_KEY_IS_STRING) {
            PHPWRITE(key, key_len);
        } else {
            php_printf("%ld", index);
        }

        php_printf(" => ");

        temp = **data;
        zval_copy_ctor(&temp);
        convert_to_string(&temp);
        PHPWRITE(Z_STRVAL(temp), Z_STRLEN(temp));
        php_printf("
                   ");
        zval_dtor(&temp);
    }

    RETURN_TRUE;
}

static int php_hello_array_walk(zval **element TSRMLS_DC)
{
    zval temp;
    temp = **element;
    zval_copy_ctor(&temp);
    convert_to_string(&temp);
    PHPWRITE(Z_STRVAL(temp), Z_STRLEN(temp));
    php_printf("
               ");
    zval_dtor(&temp);

    return ZEND_HASH_APPLY_KEEP;
}

static int php_hello_array_walk_arg(zval **element, char *greeting TSRMLS_DC)
{
    php_printf("%s", greeting);
    php_hello_array_walk(element TSRMLS_CC);

    return ZEND_HASH_APPLY_KEEP;
}

static int php_hello_array_walk_args(zval **element, int num_args, va_list args, zend_hash_key *hash_key)
{
    char *prefix = va_arg(args, char*);
    char *suffix = va_arg(args, char*);
    TSRMLS_FETCH();

    php_printf("%s", prefix);
    php_hello_array_walk(element TSRMLS_CC);
    php_printf("%s
               ", suffix);

    return ZEND_HASH_APPLY_KEEP;
}

PHP_FUNCTION(hello_array_walk)
{
    zval *zarray;
    int print_newline = 1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &zarray) == FAILURE) {
        RETURN_NULL();
    }

    zend_hash_apply(Z_ARRVAL_P(zarray), (apply_func_t)php_hello_array_walk TSRMLS_CC);
    zend_hash_internal_pointer_reset(Z_ARRVAL_P(zarray));
    zend_hash_apply_with_argument(Z_ARRVAL_P(zarray), (apply_func_arg_t)php_hello_array_walk_arg, "Hello " TSRMLS_CC);
    zend_hash_apply_with_arguments(Z_ARRVAL_P(zarray), (apply_func_args_t)php_hello_array_walk_args, 2, "Hello ", "Welcome to my extension!");

    RETURN_TRUE;
}

PHP_FUNCTION(hello_array_value)
{
    zval *zarray, *zoffset, **zvalue;
    long index = 0;
    char *key = NULL;
    int key_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "az", &zarray, &zoffset) == FAILURE) {
        RETURN_NULL();
    }

    switch (Z_TYPE_P(zoffset)) {
        case IS_NULL:
            index = 0;
            break;
        case IS_DOUBLE:
            index = (long)Z_DVAL_P(zoffset);
            break;
        case IS_BOOL:
        case IS_LONG:
        case IS_RESOURCE:
            index = Z_LVAL_P(zoffset);
            break;
        case IS_STRING:
            key = Z_STRVAL_P(zoffset);
            key_len = Z_STRLEN_P(zoffset);
            break;
        case IS_ARRAY:
            key = "Array";
            key_len = sizeof("Array") - 1;
            break;
        case IS_OBJECT:
            key = "Object";
            key_len = sizeof("Object") - 1;
            break;
        default:
            key = "Unknown";
            key_len = sizeof("Unknown") - 1;
    }

    if (key && zend_hash_find(Z_ARRVAL_P(zarray), key, key_len + 1, (void**)&zvalue) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Undefined index: %s", key);
        RETURN_NULL();
    } else if (!key && zend_hash_index_find(Z_ARRVAL_P(zarray), index, (void**)&zvalue) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Undefined index: %ld", index);
        RETURN_NULL();
    }

    *return_value = **zvalue;
    zval_copy_ctor(return_value);
}

PHP_FUNCTION(hello_get_global_var)
{
    char *varname;
    int varname_len;
    zval **varvalue;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &varname, &varname_len) == FAILURE) {
        RETURN_NULL();
    }

    if (zend_hash_find(&EG(symbol_table), varname, varname_len + 1, (void**)&varvalue) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Undefined variable: %s", varname);
        RETURN_NULL();
    }

    *return_value = **varvalue;
    zval_copy_ctor(return_value);
}

PHP_FUNCTION(hello_set_local_var)
{
    zval *newvar;
    char *varname;
    int varname_len;
    zval *value;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &varname, &varname_len, &value) == FAILURE) {
        RETURN_NULL();
    }

    ALLOC_INIT_ZVAL(newvar);
    *newvar = *value;
    zval_copy_ctor(newvar);

    zend_hash_add(EG(active_symbol_table), varname, varname_len + 1, &newvar, sizeof(zval*), NULL);

    RETURN_TRUE;
}

PHP_FUNCTION(hello_person_new)
{
    php_hello_person *person;
    char *name;
    int name_len;
    long age;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &name, &name_len, &age) == FAILURE) {
        RETURN_FALSE;
    }

    if (name_len < 1) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "No name given, person resource not created.");
        RETURN_FALSE;
    }

    if (age < 0 || age > 255) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Nonsense age (%d) given, person resource not created.", age);
        RETURN_FALSE;
    }

    person = emalloc(sizeof(php_hello_person));
    person->name = estrndup(name, name_len);
    person->name_len = name_len;
    person->age = age;

    ZEND_REGISTER_RESOURCE(return_value, person, le_hello_person);
}

PHP_FUNCTION(hello_person_pnew)
{
    php_hello_person *person;
    char *name, *key;
    int name_len, key_len;
    long age;
    list_entry *le, new_le;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &name, &name_len, &age) == FAILURE) {
        RETURN_FALSE;
    }

    if (name_len < 1) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "No name given, person resource not created.");
        RETURN_FALSE;
    }

    if (age < 0 || age > 255) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Nonsense age (%d) given, person resource not created.", age);
        RETURN_FALSE;
    }

    /* Look for an established resource */
    key_len = spprintf(&key, 0, "hello_person_%s_%d
                                ", name, age);
    if (zend_hash_find(&EG(persistent_list), key, key_len + 1, &le) == SUCCESS) {
        /* An entry for this person already exists */
        ZEND_REGISTER_RESOURCE(return_value, le->ptr, le_hello_person_persist);
        efree(key);
        return;
    }

    /* New person, allocate a structure */
    person = pemalloc(sizeof(php_hello_person), 1);
    person->name = pemalloc(name_len + 1, 1);
    memcpy(person->name, name, name_len + 1);
    person->name_len = name_len;
    person->age = age;

    ZEND_REGISTER_RESOURCE(return_value, person, le_hello_person_persist);

    /* Store a reference in the persistence list */
    new_le.ptr = person;
    new_le.type = le_hello_person_persist;
    zend_hash_add(&EG(persistent_list), key, key_len + 1, &new_le, sizeof(list_entry), NULL);

    efree(key);
}

PHP_FUNCTION(hello_person_greet)
{
    php_hello_person *person;
    zval *zperson;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zperson) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE2(person, php_hello_person*, &zperson, -1, PHP_HELLO_PERSON_RES_NAME, le_hello_person, le_hello_person_persist);

    php_printf("Hello ");
    PHPWRITE(person->name, person->name_len);
    php_printf("!
    According to my records, you are %d years old.
    ", person->age);

    RETURN_TRUE;
}

PHP_FUNCTION(hello_person_delete)
{
    zval *zperson;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zperson) == FAILURE) {
        RETURN_FALSE;
    }

    zend_list_delete(Z_LVAL_P(zperson));
    RETURN_TRUE;
}