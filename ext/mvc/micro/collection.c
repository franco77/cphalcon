
/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2013 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  +------------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_phalcon.h"
#include "phalcon.h"

#include "Zend/zend_operators.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/string.h"
#include "kernel/array.h"
#include "kernel/object.h"
#include "kernel/fcall.h"

/**
 * Phalcon\Mvc\Micro\Collection
 *
 * Groups Micro-Mvc handlers as controllers
 *
 *<code>
 *
 * $app = new Phalcon\Mvc\Micro();
 *
 * $collection = new Phalcon\Mvc\Micro\Collection();
 *
 * $collection->setHandler(new PostsController());
 *
 * $collection->get('/posts/edit/{id}', 'edit');
 *
 * $app->mount($collection);
 *
 *</code>
 *
 */


/**
 * Phalcon\Mvc\Micro\Collection initializer
 */
PHALCON_INIT_CLASS(Phalcon_Mvc_Micro_Collection){

	PHALCON_REGISTER_CLASS(Phalcon\\Mvc\\Micro, Collection, mvc_micro_collection, phalcon_mvc_micro_collection_method_entry, 0);

	zend_declare_property_null(phalcon_mvc_micro_collection_ce, SL("_prefix"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_mvc_micro_collection_ce, SL("_lazy"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_mvc_micro_collection_ce, SL("_handler"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_mvc_micro_collection_ce, SL("_handlers"), ZEND_ACC_PROTECTED TSRMLS_CC);

	zend_class_implements(phalcon_mvc_micro_collection_ce TSRMLS_CC, 1, phalcon_mvc_micro_collectioninterface_ce);

	return SUCCESS;
}

/**
 * Internal function to add a handler to the group
 *
 * @param string|array $method
 * @param string $routePattern
 * @param mixed $handler
 */
void phalcon_mvc_collection_addmap(zval *this_ptr, zval *method, zval *route_pattern, zval *handler TSRMLS_DC) {

	zval *handler_definition;

	Z_ADDREF_P(method);
	Z_ADDREF_P(route_pattern);
	Z_ADDREF_P(handler);

	MAKE_STD_ZVAL(handler_definition);
	array_init_size(handler_definition, 3);
	add_next_index_zval(handler_definition, method);
	add_next_index_zval(handler_definition, route_pattern);
	add_next_index_zval(handler_definition, handler);
	phalcon_update_property_array_append(this_ptr, SL("_handlers"), handler_definition TSRMLS_CC);
}

/**
 * Sets a prefix for all routes added to the collection
 *
 * @param string $prefix
 * @return Phalcon\Mvc\Micro\CollectionInterface
 */
PHP_METHOD(Phalcon_Mvc_Micro_Collection, setPrefix){

	zval *prefix;

	phalcon_fetch_params(0, 1, 0, &prefix);
	
	phalcon_update_property_this(this_ptr, SL("_prefix"), prefix TSRMLS_CC);
	RETURN_THISW();
}

/**
 * Returns the collection prefix if any
 *
 * @return string
 */
PHP_METHOD(Phalcon_Mvc_Micro_Collection, getPrefix){


	RETURN_MEMBER(this_ptr, "_prefix");
}

/**
 * Returns the registered handlers
 *
 * @return array
 */
PHP_METHOD(Phalcon_Mvc_Micro_Collection, getHandlers){


	RETURN_MEMBER(this_ptr, "_handlers");
}

/**
 * Sets the main handler
 *
 * @param mixed $handler
 * @param boolean $lazy
 * @return Phalcon\Mvc\Micro\CollectionInterface
 */
PHP_METHOD(Phalcon_Mvc_Micro_Collection, setHandler){

	zval *handler, *lazy = NULL;

	PHALCON_MM_GROW();

	phalcon_fetch_params(1, 1, 1, &handler, &lazy);
	
	if (!lazy) {
		PHALCON_INIT_VAR(lazy);
		ZVAL_FALSE(lazy);
	}
	
	phalcon_update_property_this(this_ptr, SL("_handler"), handler TSRMLS_CC);
	phalcon_update_property_this(this_ptr, SL("_lazy"), lazy TSRMLS_CC);
	RETURN_THIS();
}

/**
 * Sets if the main handler must be lazy loaded
 *
 * @param boolean $lazy
 * @return Phalcon\Mvc\Micro\CollectionInterface
 */
PHP_METHOD(Phalcon_Mvc_Micro_Collection, setLazy){

	zval *lazy;

	phalcon_fetch_params(0, 1, 0, &lazy);
	
	phalcon_update_property_this(this_ptr, SL("_lazy"), lazy TSRMLS_CC);
	RETURN_THISW();
}

/**
 * Returns if the main handler must be lazy loaded
 *
 * @return boolean
 */
PHP_METHOD(Phalcon_Mvc_Micro_Collection, isLazy){


	RETURN_MEMBER(this_ptr, "_lazy");
}

/**
 * Returns the main handler
 *
 * @return mixed
 */
PHP_METHOD(Phalcon_Mvc_Micro_Collection, getHandler){


	RETURN_MEMBER(this_ptr, "_handler");
}

/**
 * Maps a route to a handler
 *
 * @param string $routePattern
 * @param callable $handler
 * @return Phalcon\Mvc\Micro\CollectionInterface
 */
PHP_METHOD(Phalcon_Mvc_Micro_Collection, map){

	zval *route_pattern, *handler, *method;

	phalcon_fetch_params(0, 2, 0, &route_pattern, &handler);
	
	ALLOC_INIT_ZVAL(method);
	phalcon_mvc_collection_addmap(getThis(), method, route_pattern, handler TSRMLS_CC);
	zval_ptr_dtor(&method);
	RETURN_ZVAL(getThis(), 1, 0);
}

/**
 * Maps a route to a handler that only matches if the HTTP method is GET
 *
 * @param string $routePattern
 * @param callable $handler
 * @return Phalcon\Mvc\Micro\CollectionInterface
 */
PHP_METHOD(Phalcon_Mvc_Micro_Collection, get){

	zval *route_pattern, *handler, *method;

	phalcon_fetch_params(0, 2, 0, &route_pattern, &handler);
	
	ALLOC_INIT_ZVAL(method);
	PHALCON_ZVAL_MAYBE_INTERNED_STRING(method, phalcon_interned_GET);
	phalcon_mvc_collection_addmap(getThis(), method, route_pattern, handler TSRMLS_CC);
	zval_ptr_dtor(&method);
	RETURN_ZVAL(getThis(), 1, 0);
}

/**
 * Maps a route to a handler that only matches if the HTTP method is POST
 *
 * @param string $routePattern
 * @param callable $handler
 * @return Phalcon\Mvc\Micro\CollectionInterface
 */
PHP_METHOD(Phalcon_Mvc_Micro_Collection, post){

	zval *route_pattern, *handler, *method;

	phalcon_fetch_params(0, 2, 0, &route_pattern, &handler);
	
	ALLOC_INIT_ZVAL(method);
	PHALCON_ZVAL_MAYBE_INTERNED_STRING(method, phalcon_interned_POST);
	phalcon_mvc_collection_addmap(getThis(), method, route_pattern, handler TSRMLS_CC);
	zval_ptr_dtor(&method);
	RETURN_ZVAL(getThis(), 1, 0);
}

/**
 * Maps a route to a handler that only matches if the HTTP method is PUT
 *
 * @param string $routePattern
 * @param callable $handler
 * @return Phalcon\Mvc\Micro\CollectionInterface
 */
PHP_METHOD(Phalcon_Mvc_Micro_Collection, put){

	zval *route_pattern, *handler, *method;

	phalcon_fetch_params(0, 2, 0, &route_pattern, &handler);
	
	ALLOC_INIT_ZVAL(method);
	PHALCON_ZVAL_MAYBE_INTERNED_STRING(method, phalcon_interned_PUT);
	phalcon_mvc_collection_addmap(getThis(), method, route_pattern, handler TSRMLS_CC);
	zval_ptr_dtor(&method);
	RETURN_ZVAL(getThis(), 1, 0);
}

/**
 * Maps a route to a handler that only matches if the HTTP method is PATCH
 *
 * @param string $routePattern
 * @param callable $handler
 * @return Phalcon\Mvc\Micro\CollectionInterface
 */
PHP_METHOD(Phalcon_Mvc_Micro_Collection, patch){

	zval *route_pattern, *handler, *method;

	phalcon_fetch_params(0, 2, 0, &route_pattern, &handler);
	
	ALLOC_INIT_ZVAL(method);
	PHALCON_ZVAL_MAYBE_INTERNED_STRING(method, phalcon_interned_PATCH);
	phalcon_mvc_collection_addmap(getThis(), method, route_pattern, handler TSRMLS_CC);
	zval_ptr_dtor(&method);
	RETURN_ZVAL(getThis(), 1, 0);
}

/**
 * Maps a route to a handler that only matches if the HTTP method is HEAD
 *
 * @param string $routePattern
 * @param callable $handler
 * @return Phalcon\Mvc\Micro\CollectionInterface
 */
PHP_METHOD(Phalcon_Mvc_Micro_Collection, head){

	zval *route_pattern, *handler, *method;

	phalcon_fetch_params(0, 2, 0, &route_pattern, &handler);
	
	ALLOC_INIT_ZVAL(method);
	PHALCON_ZVAL_MAYBE_INTERNED_STRING(method, phalcon_interned_HEAD);
	phalcon_mvc_collection_addmap(getThis(), method, route_pattern, handler TSRMLS_CC);
	zval_ptr_dtor(&method);
	RETURN_ZVAL(getThis(), 1, 0);
}

/**
 * Maps a route to a handler that only matches if the HTTP method is DELETE
 *
 * @param string $routePattern
 * @param callable $handler
 * @return Phalcon\Mvc\Micro\CollectionInterface
 */
PHP_METHOD(Phalcon_Mvc_Micro_Collection, delete){

	zval *route_pattern, *handler, *method;

	phalcon_fetch_params(0, 2, 0, &route_pattern, &handler);
	
	ALLOC_INIT_ZVAL(method);
	PHALCON_ZVAL_MAYBE_INTERNED_STRING(method, phalcon_interned_DELETE);
	phalcon_mvc_collection_addmap(getThis(), method, route_pattern, handler TSRMLS_CC);
	zval_ptr_dtor(&method);
	RETURN_ZVAL(getThis(), 1, 0);
}

/**
 * Maps a route to a handler that only matches if the HTTP method is OPTIONS
 *
 * @param string $routePattern
 * @param callable $handler
 * @return Phalcon\Mvc\Micro\CollectionInterface
 */
PHP_METHOD(Phalcon_Mvc_Micro_Collection, options){

	zval *route_pattern, *handler, *method;

	phalcon_fetch_params(0, 2, 0, &route_pattern, &handler);
	
	ALLOC_INIT_ZVAL(method);
	PHALCON_ZVAL_MAYBE_INTERNED_STRING(method, phalcon_interned_OPTIONS);
	phalcon_mvc_collection_addmap(getThis(), method, route_pattern, handler TSRMLS_CC);
	zval_ptr_dtor(&method);
	RETURN_ZVAL(getThis(), 1, 0);
}
