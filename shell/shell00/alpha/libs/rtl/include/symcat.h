// Symbol concatenation utilities.
// #todo:
// It's a test yet.
// #todo: Use bsd-like stuff


#ifndef SYM_CAT_H
#define SYM_CAT_H

#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define CONCAT2(a,b)	 a##b
#define CONCAT3(a,b,c)	 a##b##c
#define CONCAT4(a,b,c,d) a##b##c##d
#define STRINGX(s) #s
#else
/* Note one should never pass extra whitespace to the CONCATn macros,
   e.g. CONCAT2(foo, bar) because traditonal C will keep the space between
   the two labels instead of concatenating them.  Instead, make sure to
   write CONCAT2(foo,bar).  */
#define CONCAT2(a,b)	 a/**/b
#define CONCAT3(a,b,c)	 a/**/b/**/c
#define CONCAT4(a,b,c,d) a/**/b/**/c/**/d
#define STRINGX(s) "s"
#endif

#define XCONCAT2(a,b)     CONCAT2(a,b)
#define XCONCAT3(a,b,c)   CONCAT3(a,b,c)
#define XCONCAT4(a,b,c,d) CONCAT4(a,b,c,d)

/* Note the layer of indirection here is typically used to allow
   stringification of the expansion of macros.  I.e. "#define foo
   bar", "XSTRING(foo)", to yield "bar".  Be aware that this only
   works for __STDC__, not for traditional C which will still resolve
   to "foo".  */
#define XSTRING(s) STRINGX(s) 

#endif /* SYM_CAT_H */
