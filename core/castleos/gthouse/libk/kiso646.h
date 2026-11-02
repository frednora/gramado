// kiso646.h
// Created by Fred Nora.

#ifndef ISO646_H
#define ISO646_H    1

/*
	<iso646.h> -- macros for use in place of certain tokens that are
		not expressible in the invariant subset of ISO 646:1991

	public-domain implementation for all versions of C

	last edit:	03-Apr-1995	Gwyn@ARL.Mil

	complies with the following standard:
		ISO/IEC 9899:1990 (as amended in 1994)

	Usage:	??=include <iso646.h>

	Notes:	The C standard has always required that the following
		trigraphs be supported in translation phase 1:
			??= mapped to #
			??( mapped to [
			??/ mapped to \
			??) mapped to ]
			??' mapped to ^
			??< mapped to {
			??! mapped to |
			??> mapped to }
			??- mapped to ~

		The so-called "normative addendum" to the C standard,
		ISO/IEC 9899:1990/Amendment 1, which requires conforming
		implementations to provide <iso646.h>, also requires
		support in translation phases 3..4 for the following
		preprocessing tokens:
			%: alternate spelling for #
			%:%: alternate spelling for ##
		It also requires support in translation phases 3..7 for
		the following preprocessing tokens:
			<: alternate spelling for [
			:> alternate spelling for ]
			<% alternate spelling for {
			%> alternate spelling for }

		The C standard also requires that the "difficult"
		characters be somehow provided in both the basic source
		and execution character sets, regardless of the
		provision of trigraph, digraph, and macro alternatives
		for these characters.  (Thus, the alternate spellings
		are intended to help programmers, not C implementors.)

		It is suggested that all C programmers avoid use of the
		following identifiers in their programs, in case
		somebody later needs to use <iso646.h> in maintaining
		the program.
 */

#define and     &&
#define and_eq  &=
#define bitand  &
#define bitor   |
#define compl   ~
#define not     !
#define not_eq  !=
#define or      ||
#define or_eq   |=
#define xor     ^
#define xor_eq  ^=    

#endif

