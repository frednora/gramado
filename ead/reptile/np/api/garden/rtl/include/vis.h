/*	$NetBSD: vis.h,v 1.25 2017/04/23 01:57:36 christos Exp $	*/

#ifndef _VIS_H_
#define	_VIS_H_

#include <sys/types.h>

/*
 * to select alternate encoding format
 */
#define	VIS_OCTAL	0x0001	/* use octal \ddd format */
#define	VIS_CSTYLE	0x0002	/* use \[nrft0..] where appropiate */

/*
 * to alter set of characters encoded (default is to encode all
 * non-graphic except space, tab, and newline).
 */
#define	VIS_SP		0x0004	/* also encode space */
#define	VIS_TAB		0x0008	/* also encode tab */
#define	VIS_NL		0x0010	/* also encode newline */
#define	VIS_WHITE	(VIS_SP | VIS_TAB | VIS_NL)
#define	VIS_SAFE	0x0020	/* only encode "unsafe" characters */
#define	VIS_DQ		0x8000	/* also encode double quotes */

/*
 * other
 */
#define	VIS_NOSLASH	0x0040	/* inhibit printing '\' */
#define	VIS_HTTP1808	0x0080	/* http-style escape % hex hex */
#define	VIS_HTTPSTYLE	0x0080	/* http-style escape % hex hex */
#define	VIS_MIMESTYLE	0x0100	/* mime-style escape = HEX HEX */
#define	VIS_HTTP1866	0x0200	/* http-style &#num; or &string; */
#define	VIS_NOESCAPE	0x0400	/* don't decode `\' */
#define	_VIS_END	0x0800	/* for unvis */
#define	VIS_GLOB	0x1000	/* encode glob(3) magic characters */
#define	VIS_SHELL	0x2000	/* encode shell special characters [not glob] */
#define	VIS_META	(VIS_WHITE | VIS_GLOB | VIS_SHELL)
#define	VIS_NOLOCALE	0x4000	/* encode using the C locale */

/*
 * unvis return codes
 */
#define	UNVIS_VALID	 1	/* character valid */
#define	UNVIS_VALIDPUSH	 2	/* character valid, push back passed char */
#define	UNVIS_NOCHAR	 3	/* valid sequence, no character produced */
#define	UNVIS_SYNBAD	-1	/* unrecognized escape sequence */
#define	UNVIS_ERROR	-2	/* decoder in unknown state (unrecoverable) */

/*
 * unvis flags
 */
#define	UNVIS_END	_VIS_END	/* no more characters */

#endif /* !_VIS_H_ */

