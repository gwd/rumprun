/*-
 * Copyright (c) 2016 Antti Kantee.  All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>

#include <sys/param.h>
#include <sys/errno.h>
#include <sys/syscall.h>
#include <sys/syscallargs.h>

#include <rump/rumpuser.h>

#include <rump-sys/kern.h>

static int
accurate_nanosleep(struct lwp *l, const struct sys___nanosleep50_args *uap,
    register_t *retval)
{
	struct timespec ts;

	copyin(SCARG(uap, rqtp), &ts, sizeof(struct timespec));
	rumpuser_clock_sleep(RUMPUSER_CLOCK_RELWALL, ts.tv_sec, ts.tv_nsec);
	return 0;
}

/*
 * XXX: undefined if this runs before or after the actual syscalls are
 * established, but since there's no other convenient way to plug things
 * in, we hope and we hope.
 */
RUMP_COMPONENT(RUMP_COMPONENT_SYSCALL)
{
	struct sysent *ent;

	printf("establishing hacky clock syscalls\n");

	ent = rump_sysent + SYS___nanosleep50;
	KASSERT(ent->sy_call != (sy_call_t *)enosys);
	ent->sy_call = (sy_call_t *)accurate_nanosleep;

	ent = rump_sysent + SYS_clock_nanosleep;
	KASSERT(ent->sy_call != (sy_call_t *)enosys);
	ent->sy_call = (sy_call_t *)accurate_nanosleep;
}
