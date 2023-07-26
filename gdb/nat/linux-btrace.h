/* Linux-dependent part of branch trace support for GDB, and GDBserver.

   Copyright (C) 2013-2018 Free Software Foundation, Inc.

   Contributed by Intel Corp. <markus.t.metzger@intel.com>

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef LINUX_BTRACE_H
#define LINUX_BTRACE_H

#include "btrace-common.h"
#include "vec.h"
#if HAVE_LINUX_PERF_EVENT_H
#  include <linux/perf_event.h>
#endif

#ifdef PERF_ATTR_SIZE_VER5_BUNDLE
#ifndef HAVE_LINUX_PERF_EVENT_H
# error "PERF_ATTR_SIZE_VER5_BUNDLE && !HAVE_LINUX_PERF_EVENT_H"
#endif
#ifndef PERF_ATTR_SIZE_VER5
#define PERF_ATTR_SIZE_VER5
#define perf_event_mmap_page perf_event_mmap_page_bundle
// kernel-headers-3.10.0-493.el7.x86_64/usr/include/linux/perf_event.h
/*
 * Structure of the page that can be mapped via mmap
 */
struct perf_event_mmap_page {
	__u32	version;		/* version number of this structure */
	__u32	compat_version;		/* lowest version this is compat with */

	/*
	 * Bits needed to read the hw events in user-space.
	 *
	 *   u32 seq, time_mult, time_shift, index, width;
	 *   u64 count, enabled, running;
	 *   u64 cyc, time_offset;
	 *   s64 pmc = 0;
	 *
	 *   do {
	 *     seq = pc->lock;
	 *     barrier()
	 *
	 *     enabled = pc->time_enabled;
	 *     running = pc->time_running;
	 *
	 *     if (pc->cap_usr_time && enabled != running) {
	 *       cyc = rdtsc();
	 *       time_offset = pc->time_offset;
	 *       time_mult   = pc->time_mult;
	 *       time_shift  = pc->time_shift;
	 *     }
	 *
	 *     index = pc->index;
	 *     count = pc->offset;
	 *     if (pc->cap_user_rdpmc && index) {
	 *       width = pc->pmc_width;
	 *       pmc = rdpmc(index - 1);
	 *     }
	 *
	 *     barrier();
	 *   } while (pc->lock != seq);
	 *
	 * NOTE: for obvious reason this only works on self-monitoring
	 *       processes.
	 */
	__u32	lock;			/* seqlock for synchronization */
	__u32	index;			/* hardware event identifier */
	__s64	offset;			/* add to hardware event value */
	__u64	time_enabled;		/* time event active */
	__u64	time_running;		/* time event on cpu */
	union {
		__u64	capabilities;
		struct {
			__u64	cap_bit0		: 1, /* Always 0, deprecated, see commit 860f085b74e9 */
				cap_bit0_is_deprecated	: 1, /* Always 1, signals that bit 0 is zero */

				cap_user_rdpmc		: 1, /* The RDPMC instruction can be used to read counts */
				cap_user_time		: 1, /* The time_* fields are used */
				cap_user_time_zero	: 1, /* The time_zero field is used */
				cap_____res		: 59;
		};
	};

	/*
	 * If cap_user_rdpmc this field provides the bit-width of the value
	 * read using the rdpmc() or equivalent instruction. This can be used
	 * to sign extend the result like:
	 *
	 *   pmc <<= 64 - width;
	 *   pmc >>= 64 - width; // signed shift right
	 *   count += pmc;
	 */
	__u16	pmc_width;

	/*
	 * If cap_usr_time the below fields can be used to compute the time
	 * delta since time_enabled (in ns) using rdtsc or similar.
	 *
	 *   u64 quot, rem;
	 *   u64 delta;
	 *
	 *   quot = (cyc >> time_shift);
	 *   rem = cyc & (((u64)1 << time_shift) - 1);
	 *   delta = time_offset + quot * time_mult +
	 *              ((rem * time_mult) >> time_shift);
	 *
	 * Where time_offset,time_mult,time_shift and cyc are read in the
	 * seqcount loop described above. This delta can then be added to
	 * enabled and possible running (if index), improving the scaling:
	 *
	 *   enabled += delta;
	 *   if (index)
	 *     running += delta;
	 *
	 *   quot = count / running;
	 *   rem  = count % running;
	 *   count = quot * enabled + (rem * enabled) / running;
	 */
	__u16	time_shift;
	__u32	time_mult;
	__u64	time_offset;
	/*
	 * If cap_usr_time_zero, the hardware clock (e.g. TSC) can be calculated
	 * from sample timestamps.
	 *
	 *   time = timestamp - time_zero;
	 *   quot = time / time_mult;
	 *   rem  = time % time_mult;
	 *   cyc = (quot << time_shift) + (rem << time_shift) / time_mult;
	 *
	 * And vice versa:
	 *
	 *   quot = cyc >> time_shift;
	 *   rem  = cyc & (((u64)1 << time_shift) - 1);
	 *   timestamp = time_zero + quot * time_mult +
	 *               ((rem * time_mult) >> time_shift);
	 */
	__u64	time_zero;
	__u32	size;			/* Header size up to __reserved[] fields. */

		/*
		 * Hole for extension of the self monitor capabilities
		 */

	__u8	__reserved[118*8+4];	/* align to 1k. */

	/*
	 * Control data for the mmap() data buffer.
	 *
	 * User-space reading the @data_head value should issue an smp_rmb(),
	 * after reading this value.
	 *
	 * When the mapping is PROT_WRITE the @data_tail value should be
	 * written by userspace to reflect the last read data, after issueing
	 * an smp_mb() to separate the data read from the ->data_tail store.
	 * In this case the kernel will not over-write unread data.
	 *
	 * See perf_output_put_handle() for the data ordering.
	 *
	 * data_{offset,size} indicate the location and size of the perf record
	 * buffer within the mmapped area.
	 */
	__u64   data_head;		/* head in the data section */
	__u64	data_tail;		/* user-space written tail */
	__u64	data_offset;		/* where the buffer starts */
	__u64	data_size;		/* data buffer size */

	/*
	 * AUX area is defined by aux_{offset,size} fields that should be set
	 * by the userspace, so that
	 *
	 *   aux_offset >= data_offset + data_size
	 *
	 * prior to mmap()ing it. Size of the mmap()ed area should be aux_size.
	 *
	 * Ring buffer pointers aux_{head,tail} have the same semantics as
	 * data_{head,tail} and same ordering rules apply.
	 */
	__u64	aux_head;
	__u64	aux_tail;
	__u64	aux_offset;
	__u64	aux_size;
};
#endif // PERF_ATTR_SIZE_VER5
#endif // PERF_ATTR_SIZE_VER5_BUNDLE

struct target_ops;

#if HAVE_LINUX_PERF_EVENT_H
/* A Linux perf event buffer.  */
struct perf_event_buffer
{
  /* The mapped memory.  */
  const uint8_t *mem;

  /* The size of the mapped memory in bytes.  */
  size_t size;

  /* A pointer to the data_head field for this buffer. */
  volatile __u64 *data_head;

  /* The data_head value from the last read.  */
  __u64 last_head;
};

/* Branch trace target information for BTS tracing.  */
struct btrace_tinfo_bts
{
  /* The Linux perf_event configuration for collecting the branch trace.  */
  struct perf_event_attr attr;

  /* The perf event file.  */
  int file;

  /* The perf event configuration page. */
  volatile struct perf_event_mmap_page *header;

  /* The BTS perf event buffer.  */
  struct perf_event_buffer bts;
};

/* Branch trace target information for Intel Processor Trace
   tracing.  */
struct btrace_tinfo_pt
{
  /* The Linux perf_event configuration for collecting the branch trace.  */
  struct perf_event_attr attr;

  /* The perf event file.  */
  int file;

  /* The perf event configuration page. */
  volatile struct perf_event_mmap_page *header;

  /* The trace perf event buffer.  */
  struct perf_event_buffer pt;
};
#endif /* HAVE_LINUX_PERF_EVENT_H */

/* Branch trace target information per thread.  */
struct btrace_target_info
{
  /* The ptid of this thread.  */
  ptid_t ptid;

  /* The obtained branch trace configuration.  */
  struct btrace_config conf;

#if HAVE_LINUX_PERF_EVENT_H
  /* The branch tracing format specific information.  */
  union
  {
    /* CONF.FORMAT == BTRACE_FORMAT_BTS.  */
    struct btrace_tinfo_bts bts;

    /* CONF.FORMAT == BTRACE_FORMAT_PT.  */
    struct btrace_tinfo_pt pt;
  } variant;
#endif /* HAVE_LINUX_PERF_EVENT_H */
};

/* See to_enable_btrace in target.h.  */
extern struct btrace_target_info *
  linux_enable_btrace (ptid_t ptid, const struct btrace_config *conf);

/* See to_disable_btrace in target.h.  */
extern enum btrace_error linux_disable_btrace (struct btrace_target_info *ti);

/* See to_read_btrace in target.h.  */
extern enum btrace_error linux_read_btrace (struct btrace_data *btrace,
					    struct btrace_target_info *btinfo,
					    enum btrace_read_type type);

/* See to_btrace_conf in target.h.  */
extern const struct btrace_config *
  linux_btrace_conf (const struct btrace_target_info *);

#endif /* LINUX_BTRACE_H */
