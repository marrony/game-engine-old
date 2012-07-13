/*
 * Exports.h
 *
 *  Created on: 17/04/2012
 *      Author: Marrony
 */

#ifndef EXPORTS_H_
#define EXPORTS_H_

#if defined( MSVC )
	#if defined (PLUGIN_EXPORTS)
		#define EXPIMP __declspec( dllexport )
	#else
		#define EXPIMP __declspec( dllimport )
	#endif
#else
	#if defined( __GNUC__ ) && __GNUC__ >= 4
		#if defined (PLUGIN_EXPORTS)
			#define EXPIMP __attribute__((dllexport))
		#else
			#define EXPIMP __attribute__((dllimport))
		#endif
	#else
		#define EXPIMP
	#endif
#endif

#endif /* EXPORTS_H_ */
