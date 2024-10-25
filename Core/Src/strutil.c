#include "strutil.h"

void *rt_memset(void *s, int c,rt_ubase_t count)
{
#ifdef RT_USING_TINY_SIZE
	char *xs = (char *)s;

	while (count--)
		*xs++ = c;

	return s;
#else
#define LBLOCKSIZE      (sizeof(long))
#define UNALIGNED(X)    ((long)X & (LBLOCKSIZE - 1))
#define TOO_SMALL(LEN)  ((LEN) < LBLOCKSIZE)

	unsigned int i;
	char *m = (char *)s;
	unsigned long buffer;
	unsigned long *aligned_addr;
	unsigned int d = c & 0xff;	/* To avoid sign extension, copy C to an
								unsigned variable.	*/

	if (!TOO_SMALL(count) && !UNALIGNED(s))
	{
		/* If we get this far, we know that n is large and m is word-aligned. */
		aligned_addr = (unsigned long *)s;

		/* Store D into each char sized location in BUFFER so that
		 * we can set large blocks quickly.
		 */
		if (LBLOCKSIZE == 4)
		{
			buffer = (d << 8) | d;
			buffer |= (buffer << 16);
		}
		else
		{
			buffer = 0;
			for (i = 0; i < LBLOCKSIZE; i ++)
				buffer = (buffer << 8) | d;
		}

		while (count >= LBLOCKSIZE * 4)
		{
			*aligned_addr++ = buffer;
			*aligned_addr++ = buffer;
			*aligned_addr++ = buffer;
			*aligned_addr++ = buffer;
			count -= 4 * LBLOCKSIZE;
		}

		while (count >= LBLOCKSIZE)
		{
			*aligned_addr++ = buffer;
			count -= LBLOCKSIZE;
		}

		/* Pick up the remainder with a bytewise loop. */
		m = (char *)aligned_addr;
	}

	while (count--)
	{
		*m++ = (char)d;
	}

	return s;

#undef LBLOCKSIZE
#undef UNALIGNED
#undef TOO_SMALL
#endif
}

void *rt_memcpy(void *dst, const void *src, rt_ubase_t count)
{
#ifdef RT_USING_TINY_SIZE
    char *tmp = (char *)dst, *s = (char *)src;
    rt_ubase_t len;

    if (tmp <= s || tmp > (s + count))
    {
        while (count--)
            *tmp ++ = *s ++;
    }
    else
    {
        for (len = count; len > 0; len --)
            tmp[len - 1] = s[len - 1];
    }

    return dst;
#else

#define UNALIGNED(X, Y) \
    (((long)X & (sizeof (long) - 1)) | ((long)Y & (sizeof (long) - 1)))
#define BIGBLOCKSIZE    (sizeof (long) << 2)
#define LITTLEBLOCKSIZE (sizeof (long))
#define TOO_SMALL(LEN)  ((LEN) < BIGBLOCKSIZE)

    char *dst_ptr = (char *)dst;
    char *src_ptr = (char *)src;
    long *aligned_dst;
    long *aligned_src;
    int len = count;

    /* If the size is small, or either SRC or DST is unaligned,
    then punt into the byte copy loop.  This should be rare. */
    if (!TOO_SMALL(len) && !UNALIGNED(src_ptr, dst_ptr))
    {
        aligned_dst = (long *)dst_ptr;
        aligned_src = (long *)src_ptr;

        /* Copy 4X long words at a time if possible. */
        while (len >= BIGBLOCKSIZE)
        {
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            len -= BIGBLOCKSIZE;
        }

        /* Copy one long word at a time if possible. */
        while (len >= LITTLEBLOCKSIZE)
        {
            *aligned_dst++ = *aligned_src++;
            len -= LITTLEBLOCKSIZE;
        }

        /* Pick up any residual with a byte copier. */
        dst_ptr = (char *)aligned_dst;
        src_ptr = (char *)aligned_src;
    }

    while (len--)
        *dst_ptr++ = *src_ptr++;

    return dst;
#undef UNALIGNED
#undef BIGBLOCKSIZE
#undef LITTLEBLOCKSIZE
#undef TOO_SMALL
#endif
}

