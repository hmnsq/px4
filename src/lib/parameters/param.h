/****************************************************************************
 *
 *   Copyright (c) 2012-2015 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file param.h
 *
 * Global parameter store.
 *
 * Note that a number of API members are marked const or pure; these
 * assume that the set of parameters cannot change, or that a parameter
 * cannot change type or size over its lifetime.  If any of these assumptions
 * are invalidated, the attributes should be re-evaluated.
 */

#ifndef _SYSTEMLIB_PARAM_PARAM_H
#define _SYSTEMLIB_PARAM_PARAM_H

#ضمن <stdint.h>
#ضمن <stdbool.h>
#ضمن <sys/types.h>

/** Maximum size of the parameter backing file */
#define PARAM_FILE_MAXSIZE		4096

__BEGIN_DECLS

/**
 * Parameter types.
 */
#define PARAM_TYPE_UNKNOWN		0
#define PARAM_TYPE_INT32		1
#define PARAM_TYPE_FLOAT		2

تعريفنوع uint8_t param_type_t;

/**
 * Parameter handle.
 *
 * Parameters are represented by parameter handles, which can
 * be obtained by looking up parameters. They are an offset into a global
 * constant parameter array.
 */
تعريفنوع uint16_t	param_t;

/**
 * Handle returned when a parameter cannot be found.
 */
#define PARAM_INVALID	((uint16_t)0xffff)

/**
 * Magic handle for hash check param
 */
#define PARAM_HASH      ((uint16_t)INT16_MAX)


/**
 * Initialize the param backend. Call this on startup before calling any other methods.
 */
__EXPORT فراغ		param_init(فراغ);

/**
 * Look up a parameter by name.
 *
 * @param name		The canonical name of the parameter being looked up.
 * @return		A handle to the parameter, or PARAM_INVALID if the parameter does not exist.
 *			This call will also set the parameter as "used" in the system, which is used
 *			to e.g. show the parameter via the RC interface
 */
__EXPORT param_t	param_find(ثابت حرف *name);

/**
 * Look up a parameter by name.
 *
 * @param name		The canonical name of the parameter being looked up.
 * @return		A handle to the parameter, or PARAM_INVALID if the parameter does not exist.
 */
__EXPORT param_t	param_find_no_notification(ثابت حرف *name);

/**
 * Return the total number of parameters.
 *
 * @return		The number of parameters.
 */
__EXPORT unsigned	param_کيراءن(فراغ);

/**
 * Return the actually used number of parameters.
 *
 * @return		The number of parameters.
 */
__EXPORT unsigned	param_کيراءن_used(فراغ);

/**
 * Wether a parameter is in use in the system.
 *
 * @return		True if it has been written or read
 */
__EXPORT bool		param_used(param_t param);

/**
 * Look up a parameter by index.
 *
 * @param index		An index from 0 to n, where n is param_count()-1.
 * @return		A handle to the parameter, or PARAM_INVALID if the index is out of range.
 */
__EXPORT param_t	param_for_index(unsigned index);

/**
 * Look up an used parameter by index.
 *
 * @param index		The parameter to obtain the index for.
 * @return		The index of the parameter in use, or -1 if the parameter does not exist.
 */
__EXPORT param_t	param_for_used_index(unsigned index);

/**
 * Look up the index of a parameter.
 *
 * @param param		The parameter to obtain the index for.
 * @return		The index, or -1 if the parameter does not exist.
 */
__EXPORT صحيح		param_get_index(param_t param);

/**
 * Look up the index of an used parameter.
 *
 * @param param		The parameter to obtain the index for.
 * @return		The index of the parameter in use, or -1 if the parameter does not exist.
 */
__EXPORT صحيح		param_get_used_index(param_t param);

/**
 * Obtain the name of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @return		The name assigned to the parameter, or NULL if the handle is invalid.
 */
__EXPORT ثابت حرف	*param_name(param_t param);

/**
 * Obtain the volatile state of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @return			true if the parameter is volatile
 */
__EXPORT bool		param_is_volatile(param_t param);

/**
 * Obtain the read-only state of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @return			true if the parameter is read-only
 */
__EXPORT bool		param_is_readonly(param_t param);

/**
 * Lock all read-only parameters.
 *
 * Before this call, read-only parameters can be freely modified (e.g. by
 * startup scripts).  After this call, any attempt to set, set-default, or
 * reset a read-only parameter will be rejected.
 */
__EXPORT فراغ		param_lock_readonly(فراغ);

/**
 * Test whether a parameter's value has changed from the default.
 *
 * @return		If true, the parameter's value has not been changed from the default.
 */
__EXPORT bool		param_نيلاي_is_default(param_t param);

/**
 * Test whether a parameter's value has been changed but not saved.
 *
 * @return		If true, the parameter's value has not been saved.
 */
__EXPORT bool		param_نيلاي_unsaved(param_t param);

/**
 * Obtain the type of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @return		The type assigned to the parameter.
 */
__EXPORT param_type_t	param_type(param_t param);

/**
 * Determine the size of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @return		The size of the parameter's value.
 */
__EXPORT size_t		param_size(param_t param);

/**
 * Copy the value of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @param val		Where to return the value, assumed to point to suitable storage for the parameter type.
 * @return		Zero if the parameter's value could be returned, nonzero otherwise.
 */
__EXPORT صحيح		param_get(param_t param, فراغ *val);

/**
 * Mark a parameter as used and copy its value.
 *
 * Combines param_set_used() and param_get() into a single out-of-line call,
 * saving flash at the many Param<> constructor call sites.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @param val		Where to return the value, assumed to point to suitable storage for the parameter type.
 * @return		Zero if the parameter's value could be returned, nonzero otherwise.
 */
__EXPORT صحيح		param_get_mark_used(param_t param, فراغ *val);

/**
 * Copy the (airframe-specific) default value of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @param default_val		Where to return the value, assumed to point to suitable storage for the parameter type.
 * @return		Zero if the parameter's deafult value could be returned, nonzero otherwise.
 */
__EXPORT صحيح		param_get_default_نيلاي(param_t param, فراغ *default_val);

/**
 * Copy the system-wide default value of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @param default_val		Where to return the value, assumed to point to suitable storage for the parameter type.
 * @return		Zero if the parameter's deafult value could be returned, nonzero otherwise.
 */
__EXPORT صحيح		param_get_system_default_نيلاي(param_t param, فراغ *default_val);

/**
 * Set the value of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @param val		The value to set; assumed to point to a variable of the parameter type.
 * @return		Zero if the parameter's value could be set from a scalar, nonzero otherwise.
 */
__EXPORT صحيح		param_set(param_t param, ثابت فراغ *val);

/**
 * Set the default value of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @param val		The default value to set; assumed to point to a variable of the parameter type.
 * @return		Zero if the parameter's default value could be set from a scalar, nonzero otherwise.
 */
__EXPORT صحيح		param_set_default_نيلاي(param_t param, ثابت فراغ *val);

/**
 * Mark a parameter as used. Only marked parameters will be sent to a GCS.
 * A call to param_find() will mark a param as used as well.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 */
__EXPORT فراغ		param_set_used(param_t param);

/**
 * Set the value of a parameter, but do not notify the system about the change.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @param val		The value to set; assumed to point to a variable of the parameter type.
 * @return		Zero if the parameter's value could be set from a scalar, nonzero otherwise.
 */
__EXPORT صحيح		param_set_no_notification(param_t param, ثابت فراغ *val);

/**
 * Set the value of a parameter, but do not update the remote system. This avoids
 * a set loop between primary and remote.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @param val		The value to set; assumed to point to a variable of the parameter type.
 *			For structures, the pointer is assumed to point to a structure to be copied.
 * @param notify	Set this to true for the primary (to send out a param update) and false on client
 * @return		Zero if the parameter's value could be set from a scalar, nonzero otherwise.
 */
__EXPORT صحيح		param_set_no_remote_update(param_t param, ثابت فراغ *val, bool notify);

/**
 * Notify the system about parameter changes. Can be used for example after several calls to
 * param_set_no_notification() to avoid unnecessary system notifications.
 */
__EXPORT فراغ		param_notify_changes(فراغ);

/**
 * Reset a parameter to its default value.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @return		Zero on success, nonzero on failure
 */
__EXPORT صحيح		param_reset(param_t param);

/**
 * Reset a parameter to its default value, but do not notify the system about the change.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @return		Zero on success, nonzero on failure
 */
__EXPORT صحيح		param_reset_no_notification(param_t param);

/**
 * Reset all parameters to their default values.
 */
__EXPORT فراغ		param_reset_all(فراغ);

/**
 * Reset all parameters to their default values except for excluded parameters.
 *
 * @param excludes			Array of param names to exclude from resetting. Use a wildcard
 *							at the end to exclude parameters with a certain prefix.
 * @param num_excludes		The number of excludes provided.
 */
__EXPORT فراغ		param_reset_excludes(ثابت حرف *excludes[], صحيح اڠک_excludes);

تعريفنوع bool(*param_filter_func)(param_t handle);

/**
 * Reset only specific parameters to their default values.
 *
 * @param resets Array of param names to reset. Use a wildcard at the end to reset parameters with a certain prefix.
 * @param num_resets The number of passed reset conditions in the resets array.
 */
__EXPORT فراغ		param_reset_specific(ثابت حرف *resets[], صحيح اڠک_resets);

/**
 * Export changed parameters to a file.
 * Note: this method requires a large amount of stack size!
 *
 * @param filename	Path to the default parameter file.
 * @param filter	Filter parameters to be exported. The method should return true if
 * 			the parameter should be exported. No filtering if nullptr is passed.
 * @return		Zero on success, nonzero on failure.
 */
__EXPORT صحيح		param_export(ثابت حرف *filename, param_filter_func filter);

/**
 * Import parameters from a file, discarding any unrecognized parameters.
 *
 * This function merges the imported parameters with the current parameter set.
 *
 * @param fd		File descriptor to import from (-1 selects the FLASH storage).
 * @return		Zero on success, 1 if the source is blank (nothing stored
 *			yet), negative if an error occurred during import. Note that
 *			in the error case, parameters may be inconsistent.
 */
__EXPORT صحيح		param_import(صحيح fd);

/**
 * Load parameters from a file.
 *
 * This function resets all parameters to their default values, then loads new
 * values from a file.
 *
 * @param fd		File descriptor to import from (-1 selects the FLASH storage).
 * @return		Zero on success, 1 if the source is blank (all parameters
 *			were reset to defaults), negative if an error occurred during
 *			import. Note that in the error case, parameters may be
 *			inconsistent.
 */
__EXPORT صحيح		param_load(صحيح fd);

/**
 * Apply a function to each parameter.
 *
 * Note that the parameter set is not locked during the traversal. It also does
 * not hold an internal state, so the callback function can block or sleep between
 * parameter callbacks.
 *
 * @param func		The function to invoke for each parameter.
 * @param arg		Argument passed to the function.
 * @param only_changed	If true, the function is only called for parameters whose values have
 *			been changed from the default.
 * @param only_used	If true, the function is only called for parameters which have been
 *			used in one of the running applications.
 */
__EXPORT فراغ		param_foreach(فراغ (*func)(فراغ *arg, param_t param), فراغ *arg, bool only_changed, bool only_used);

/**
 * Set the default parameter file name.
 * This has no effect if the FLASH-based storage is enabled.
 *
 * @param filename	Path to the default parameter file.  The file is not required to
 *			exist.
 * @return		Zero on success.
 */
__EXPORT صحيح 		param_set_default_file(ثابت حرف *filename);

/**
 * Get the default parameter file name.
 *
 * @return		The path to the current default parameter file; either as
 *			a result of a call to param_set_default_file, or the
 *			built-in default.
 */
__EXPORT ثابت حرف	*param_get_default_file(فراغ);

/**
 * Set the backup parameter file name.
 *
 * @param filename	Path to the backup parameter file. The file is not required to
 *			exist.
 * @return		Zero on success.
 */
__EXPORT صحيح 		param_set_backup_file(ثابت حرف *filename);

/**
 * Get the backup parameter file name.
 *
 * @return		The path to the backup parameter file
 */
__EXPORT ثابت حرف	*param_get_backup_file(فراغ);

/**
 * Save parameters to the default file.
 *
 * Note: this method requires a large amount of stack size!
 *
 * This function saves all parameters with non-default values.
 *
 * @param blocking	If true, in case the default file is busy, the function blocks
 * 			until the file is available for writing.
 *
 * @return		Zero on success, -EWOULDBLOCK if the file is busy and blocking is false.
 */
__EXPORT صحيح 		param_save_default(bool blocking);

/**
 * Load parameters from the default parameter file.
 *
 * @return		Zero on success.
 */
__EXPORT صحيح 		param_load_default(فراغ);

/**
 * Generate the hash of all parameters and their values
 *
 * @return		CRC32 hash of all param_ids and values
 */
__EXPORT uint32_t	param_hash_check(فراغ);

/**
 * Print the status of the param system
 *
 */
__EXPORT فراغ	param_print_status(فراغ);

/**
 * Enable/disable the param autosaving.
 * Re-enabling with changed params will not cause an autosave.
 * @param enable true: enable autosaving, false: disable autosaving
 */
__EXPORT فراغ	param_control_autosave(bool enable);

/**
 * Parameter value union.
 */
اتحاد param_نيلاي_u {
	int32_t		i;
	عائم		f;
};

/**
 * Static parameter definition structure.
 *
 * This is normally not used by user code; see the PARAM_DEFINE macros
 * instead.
 */
بنية param_info_s {
	ثابت حرف *name;
	اتحاد param_نيلاي_u val;
};

__END_DECLS



#إذا defined(__cplusplus) && !defined(PARAM_IMPLEMENTATION)
#إذا defined(CONFIG_ARCH_BOARD_PX4_SITL) || 0 // set to 1 to debug param type mismatches
#ضمن <cstdio>
#ضمن <px4_platform_common/log.h>

#ifndef MODULE_NAME
#define MODULE_NAME "px4"
#endif

#define CHECK_PARAM_TYPE(param, type) \
	إذا (param_type(param) != type) { \
		/* use printf() to avoid having to use more includes */ \
		PX4_ERR("wrong type passed to param_get() for param %s", param_name(param)); \
	}
#وإلا
#define CHECK_PARAM_TYPE(param, type)
#endif

// param is a C-interface. This means there is no overloading, and thus no type-safety for param_get().
// So for C++ code we redefine param_get() to inlined overloaded versions, which gives us type-safety
// w/o having to use a different interface
ساكن مضمن صحيح param_get_cplusplus(param_t param, عائم *val)
{
	CHECK_PARAM_TYPE(param, PARAM_TYPE_FLOAT);
	أرجع param_get(param, (فراغ *)val);
}
ساكن مضمن صحيح param_get_cplusplus(param_t param, int32_t *val)
{
	CHECK_PARAM_TYPE(param, PARAM_TYPE_INT32);
	أرجع param_get(param, (فراغ *)val);
}
#undef CHECK_PARAM_TYPE

#define param_get(param, val) param_get_cplusplus(param, val)

#endif /* __cplusplus */

#endif
