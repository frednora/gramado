
// File: gcd_lcm.c
// Credits: Android.

/* Copyright (c) 2012, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 *  with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
static void swap(unsigned *x, unsigned *y)
{
	unsigned temp;
	temp = *x;
	*x   = *y;
	*y   = temp;
}



/*
 * Compute the greatest common divisor of a and b.
 * credits: serenity os.
 */
/* 
static int
gcd(int a, int b)
{
	int c;

	c = a % b;
	while (c != 0) {
		a = b;
		b = c;
		c = a % b;
	}

	return (b);
}
*/

/* Function to calculate gcd of two positive numbers. */
unsigned gcd(unsigned m, unsigned n)
{
	unsigned x;
	if (m < n)
		swap(&m, &n);
	while (n != 0)
	{
		x = m % n;
		m = n;
		n = x;
	}
	return m;
}
/* Function to calculate lcm of two positive numbers. */
unsigned lcm(unsigned m, unsigned n)
{
	unsigned lcm;
	lcm = (m * n) / gcd(m, n);
	return lcm;
}

