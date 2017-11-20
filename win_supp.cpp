#include "win_supp.h"

#ifdef _WIN32

int getline(char** lineptr, size_t* n, FILE* stream) {

	char *bufptr, *p;

	ssize_t size;
	int c;

	// all parameters are required
	if (!lineptr || !stream || !n)
		return -1;

	bufptr	= *lineptr;
	size 	= *n;
	c 		= fgetc(stream);

	if (c == EOF)
		return -1;

	if (bufptr == NULL) {
		bufptr = (char*) malloc(128);
		if (bufptr == NULL)
			return -1;

		size = 128;
	}

	p = bufptr;

	while (c != EOF) {

		if ((p - bufptr) > (size - 2)) {
			size = size + 128;
			bufptr = (char*) realloc(bufptr, size);
			if (bufptr == NULL)
				return -1;
		}

		*p++ = c;
		if (c == '\n' || c == '\0')
			break;

		c = fgetc(stream);

	}

	*p++	 = '\0';
	*lineptr = bufptr;
	*n		 = size;

	return p - bufptr - 1;

}


#ifdef FRONFCJKANCJKADNCJ
/* mkstemp extracted from libc/sysdeps/posix/tempname.c.  Copyright
   (C) 1991-1999, 2000, 2001, 2006 Free Software Foundation, Inc.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.  */

static const char letters[] =
"abcdefghijklmnopqrstuvwxyz0123456789";
static const uint8_t dof = sizeof(letters);

/* Generate a temporary file name based on TMPL.  TMPL must match the
   rules for mk[s]temp (i.e. end in "XXXXXX").  The name constructed
   does not exist at the time of the call to mkstemp.  TMPL is
   overwritten with the result.  */

int
mkstemp (char *tmpl)
{
  int len;
  char *XXXXXX;
  static unsigned long long value;
  unsigned long long random_time_bits;
  unsigned int count;
  int fd = -1;
  int save_errno = errno;

  /* A lower bound on the number of temporary files to attempt to
     generate.  The maximum total number of temporary file names that
     can exist for a given template is dof**6.  It should never be
     necessary to try all these combinations.  Instead if a reasonable
     number of names is tried (we define reasonable as dof**3) fail to
     give the system administrator the chance to remove the problems.  */
#define ATTEMPTS_MIN (dof * dof * dof)

  /* The number of times to attempt to generate a temporary file.  To
     conform to POSIX, this must be no smaller than TMP_MAX.  */
#if ATTEMPTS_MIN < TMP_MAX
  unsigned int attempts = TMP_MAX;
#else
  unsigned int attempts = ATTEMPTS_MIN;
#endif

  len = strlen (tmpl);
  if (len < 6 || strcmp (&tmpl[len - 6], "XXXXXX"))
    {
      errno = EINVAL;
      return -1;
    }

/* This is where the Xs start.  */
  XXXXXX = &tmpl[len - 6];

  /* Get some more or less random data.  */
  {
    SYSTEMTIME      stNow;
    FILETIME ftNow;

    // get system time
    GetSystemTime(&stNow);
    stNow.wMilliseconds = 500;
    if (!SystemTimeToFileTime(&stNow, &ftNow))
    {
        errno = -1;
        return -1;
    }

    random_time_bits = (((unsigned long long)ftNow.dwHighDateTime << 32)
                        | (unsigned long long)ftNow.dwLowDateTime);
  }
  value += random_time_bits ^ (unsigned long long)GetCurrentThreadId ();

  for (count = 0; count < attempts; value += 7777, ++count)
    {
      unsigned long long v = value;

      /* Fill in the random bits.  */
      XXXXXX[0] = letters[v % dof];
      v /= 62;
      XXXXXX[1] = letters[v % dof];
      v /= 62;
      XXXXXX[2] = letters[v % dof];
      v /= 62;
      XXXXXX[3] = letters[v % dof];
      v /= 62;
      XXXXXX[4] = letters[v % dof];
      v /= 62;
      XXXXXX[5] = letters[v % dof];

      fd = open (tmpl, O_RDWR | O_CREAT | O_EXCL, _S_IREAD | _S_IWRITE);
      if (fd >= 0)
    {
      errno = save_errno;
      return fd;
    }
      else if (errno != EEXIST)
    return -1;
    }

  /* We got out of the loop because we ran out of combinations to try.  */
  errno = EEXIST;
  return -1;
}


#endif // comment
#endif // _WIN32

