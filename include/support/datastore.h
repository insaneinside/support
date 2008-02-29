/**
 * \file	support/datastore.h
 *
 * \brief	Data storage abstractions.
 *
 */
#ifndef SUPPORT_DATASTORE_H
#define SUPPORT_DATASTORE_H
#include <support/dllist.h>
#include <support/meta.h>

/** \defgroup datastore Table-model data storage
 */
/*@{*/

/** \name Data Types */
/*@{*/
typedef struct datastore_t;
/*@}*/
/** \name Object management
 */
/*@{*/

/** Create a new data store.
 */
datastore_t*
datastore_create();

/** Destroy a data store.
 */
void
datastore_destroy(datastore_t* datastore);
/*@}*/

/** \name Field definitions */
/*@{ */
/** Fetch the datastore's list of field type definitions.
 *
 * \param	datastore	Target datastore.
 *
 * \returns	a dllist of pointers to field type definitions.
 */
dllist_t*
datastore_get_fields(const datastore_t* datastore);
/*@}*/

/** \name Data records */
/*@{*/
typedef struct datastore_record_t;

const meta_t*
datastore_record_field_by_name(const datastore_record_t* record,
			       const char* fieldName);

const meta_t*
datastore_record_field_by_index(const datastore_record_t* record,
				const unsigned fieldIndex);


/*@}*/


/*@}*/
#endif SUPPORT_DATASTORE_H
