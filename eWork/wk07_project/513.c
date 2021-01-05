// SPDX-License-Identifier: GPL-2.0-only
/*
 * turbostat -- show CPU frequency and C-state residency
 * on modern Intel and AMD processors.
 *
 * Copyright (c) 2013 Intel Corporation.
 * Len Brown <len.brown@intel.com>
 */

#define _LARGEFILE64_SOURCE 

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "intel-family.h"
#include "msr-index.h"

extern ssize_t __pread64_chk(int, void *, size_t, off64_t, size_t);
extern ssize_t __pread64_chk_warn(int __fd, void *__buf, size_t __nbytes, __off64_t __offset, size_t __bufsize);
extern ssize_t __pread64_alias(int __fd, void *__buf, size_t __nbytes, __off64_t __offset);

#define MDebug printf
#define __cpuid(level, a, b, c, d)			\
  __asm__ ("cpuid\n\t"					\
	   : "=a" (a), "=b" (b), "=c" (c), "=d" (d)	\
	   : "0" (level))
#define __cpuid_count(level, count, a, b, c, d)		\
  __asm__ ("cpuid\n\t"					\
	   : "=a" (a), "=b" (b), "=c" (c), "=d" (d)	\
	   : "0" (level), "2" (count))
int *fd_percpu;
unsigned int do_snb_cstates;
unsigned int do_knl_cstates;
unsigned int do_slm_cstates;
unsigned int use_c1_residency_msr;
unsigned int has_aperf;
unsigned int has_epb;
unsigned int do_irtl_snb;
unsigned int do_irtl_hsw;
unsigned int genuine_intel;
unsigned int authentic_amd;
unsigned int hygon_genuine;
unsigned int max_level, max_extended_level;
unsigned int has_invariant_tsc;
unsigned int do_nhm_platform_info;
unsigned int no_MSR_MISC_PWR_MGMT;
unsigned int aperf_mperf_multiplier = 1;
double bclk;
double base_hz;
double tsc_tweak = 1.0;
unsigned int do_dts;
unsigned int do_ptm;
unsigned int crystal_hz;
unsigned long long tsc_hz;
int base_cpu;
// double discover_bclk(unsigned int family, unsigned int model);
unsigned int has_hwp;	/* IA32_PM_ENABLE, IA32_HWP_CAPABILITIES */
			/* IA32_HWP_REQUEST, IA32_HWP_STATUS */
unsigned int has_hwp_notify;		/* IA32_HWP_INTERRUPT */
unsigned int has_hwp_activity_window;	/* IA32_HWP_REQUEST[bits 41:32] */
unsigned int has_hwp_epp;		/* IA32_HWP_REQUEST[bits 31:24] */
unsigned int has_hwp_pkg;		/* IA32_HWP_REQUEST_PKG */
unsigned int has_misc_feature_control;

#define __bos0(ptr) __builtin_object_size (ptr, 0)

__fortify_function __wur ssize_t
pread (int __fd, void *__buf, size_t __nbytes, __off64_t __offset)
{
  if (__bos0 (__buf) != (size_t) -1)
    {
      if (!__builtin_constant_p (__nbytes))
	return __pread64_chk (__fd, __buf, __nbytes, __offset, __bos0 (__buf));

      if ( __nbytes > __bos0 (__buf))
	return __pread64_chk_warn (__fd, __buf, __nbytes, __offset,
				   __bos0 (__buf));
    }

  return __pread64_alias (__fd, __buf, __nbytes, __offset);
}

int get_msr_fd(int cpu)
{
	char pathname[32];
	int fd;

	fd = fd_percpu[cpu];

	if (fd)
		return fd;

	sprintf(pathname, "/dev/cpu/%d/msr", cpu);
	fd = open(pathname, O_RDONLY);
	if (fd < 0)
		err(-1, "%s open failed, try chown or chmod +r /dev/cpu/*/msr, or run as root", pathname);

	fd_percpu[cpu] = fd;

	return fd;
}

int get_msr(int cpu, off_t offset, unsigned long long *msr)
{
	ssize_t retval;

	retval = pread(get_msr_fd(cpu), msr, sizeof(*msr), offset);

	if (retval != sizeof *msr)
		err(-1, "cpu%d: msr offset 0x%llx read failed", cpu, (unsigned long long)offset);

	return 0;
}



#define MAX_BIC (sizeof(bic) / sizeof(struct msr_counter))
#define	BIC_USEC	(1ULL << 0)
#define	BIC_TOD		(1ULL << 1)
#define	BIC_Package	(1ULL << 2)
#define	BIC_Node	(1ULL << 3)
#define	BIC_Avg_MHz	(1ULL << 4)
#define	BIC_Busy	(1ULL << 5)
#define	BIC_Bzy_MHz	(1ULL << 6)
#define	BIC_TSC_MHz	(1ULL << 7)
#define	BIC_IRQ		(1ULL << 8)
#define	BIC_SMI		(1ULL << 9)
#define	BIC_sysfs	(1ULL << 10)
#define	BIC_CPU_c1	(1ULL << 11)
#define	BIC_CPU_c3	(1ULL << 12)
#define	BIC_CPU_c6	(1ULL << 13)
#define	BIC_CPU_c7	(1ULL << 14)
#define	BIC_ThreadC	(1ULL << 15)
#define	BIC_CoreTmp	(1ULL << 16)
#define	BIC_CoreCnt	(1ULL << 17)
#define	BIC_PkgTmp	(1ULL << 18)
#define	BIC_GFX_rc6	(1ULL << 19)
#define	BIC_GFXMHz	(1ULL << 20)
#define	BIC_Pkgpc2	(1ULL << 21)
#define	BIC_Pkgpc3	(1ULL << 22)
#define	BIC_Pkgpc6	(1ULL << 23)
#define	BIC_Pkgpc7	(1ULL << 24)
#define	BIC_Pkgpc8	(1ULL << 25)
#define	BIC_Pkgpc9	(1ULL << 26)
#define	BIC_Pkgpc10	(1ULL << 27)
#define BIC_CPU_LPI	(1ULL << 28)
#define BIC_SYS_LPI	(1ULL << 29)
#define	BIC_PkgWatt	(1ULL << 30)
#define	BIC_CorWatt	(1ULL << 31)
#define	BIC_GFXWatt	(1ULL << 32)
#define	BIC_PkgCnt	(1ULL << 33)
#define	BIC_RAMWatt	(1ULL << 34)
#define	BIC_PKG__	(1ULL << 35)
#define	BIC_RAM__	(1ULL << 36)
#define	BIC_Pkg_J	(1ULL << 37)
#define	BIC_Cor_J	(1ULL << 38)
#define	BIC_GFX_J	(1ULL << 39)
#define	BIC_RAM_J	(1ULL << 40)
#define	BIC_Mod_c6	(1ULL << 41)
#define	BIC_Totl_c0	(1ULL << 42)
#define	BIC_Any_c0	(1ULL << 43)
#define	BIC_GFX_c0	(1ULL << 44)
#define	BIC_CPUGFX	(1ULL << 45)
#define	BIC_Core	(1ULL << 46)
#define	BIC_CPU		(1ULL << 47)
#define	BIC_APIC	(1ULL << 48)
#define	BIC_X2APIC	(1ULL << 49)
#define	BIC_Die		(1ULL << 50)
#define	BIC_GFXACTMHz	(1ULL << 51)

#define BIC_DISABLED_BY_DEFAULT	(BIC_USEC | BIC_TOD | BIC_APIC | BIC_X2APIC)

#define BIC_PRESENT(COUNTER_BIT) (bic_present |= COUNTER_BIT)
#define BIC_NOT_PRESENT(COUNTER_BIT) (bic_present &= ~COUNTER_BIT)

#define PCLUKN 0 /* Unknown */
#define PCLRSV 1 /* Reserved */
#define PCL__0 2 /* PC0 */
#define PCL__1 3 /* PC1 */
#define PCL__2 4 /* PC2 */
#define PCL__3 5 /* PC3 */
#define PCL__4 6 /* PC4 */
#define PCL__6 7 /* PC6 */
#define PCL_6N 8 /* PC6 No Retention */
#define PCL_6R 9 /* PC6 Retention */
#define PCL__7 10 /* PC7 */
#define PCL_7S 11 /* PC7 Shrink */
#define PCL__8 12 /* PC8 */
#define PCL__9 13 /* PC9 */
#define PCL_10 14 /* PC10 */
#define PCLUNL 15 /* Unlimited */

int pkg_cstate_limit = PCLUKN;

int nhm_pkg_cstate_limits[16] = {PCL__0, PCL__1, PCL__3, PCL__6, PCL__7, PCLRSV, PCLRSV, PCLUNL, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV};
int snb_pkg_cstate_limits[16] = {PCL__0, PCL__2, PCL_6N, PCL_6R, PCL__7, PCL_7S, PCLRSV, PCLUNL, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV};
int hsw_pkg_cstate_limits[16] = {PCL__0, PCL__2, PCL__3, PCL__6, PCL__7, PCL_7S, PCL__8, PCL__9, PCLUNL, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV};
int slv_pkg_cstate_limits[16] = {PCL__0, PCL__1, PCLRSV, PCLRSV, PCL__4, PCLRSV, PCL__6, PCL__7, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCL__6, PCL__7};
int amt_pkg_cstate_limits[16] = {PCLUNL, PCL__1, PCL__2, PCLRSV, PCLRSV, PCLRSV, PCL__6, PCL__7, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV};
int phi_pkg_cstate_limits[16] = {PCL__0, PCL__2, PCL_6N, PCL_6R, PCLRSV, PCLRSV, PCLRSV, PCLUNL, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV};
int glm_pkg_cstate_limits[16] = {PCLUNL, PCL__1, PCL__3, PCL__6, PCL__7, PCL_7S, PCL__8, PCL__9, PCL_10, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV};
int skx_pkg_cstate_limits[16] = {PCL__0, PCL__2, PCL_6N, PCL_6R, PCLRSV, PCLRSV, PCLRSV, PCLUNL, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV, PCLRSV};


static void
calculate_tsc_tweak()
{
	tsc_tweak = base_hz / tsc_hz;
}

void free_fd_percpu(void)
{
	int i;

	for (i = 0; i < topo.max_cpu_num + 1; ++i) {
		if (fd_percpu[i] != 0)
			close(fd_percpu[i]);
	}

	free(fd_percpu);
}

// int probe_nhm_msrs(unsigned int family, unsigned int model)
// {
// 	unsigned long long msr;
// 	unsigned int base_ratio;
// 	int *pkg_cstate_limits;

// 	if (!genuine_intel)
// 		return 0;

// 	if (family != 6)
// 		return 0;

// 	bclk = discover_bclk(family, model);

// 	switch (model) {
// 	case INTEL_FAM6_NEHALEM:	/* Core i7 and i5 Processor - Clarksfield, Lynnfield, Jasper Forest */
// 	case INTEL_FAM6_NEHALEM_EX:	/* Nehalem-EX Xeon - Beckton */
// 		pkg_cstate_limits = nhm_pkg_cstate_limits;
// 		break;
// 	case INTEL_FAM6_SANDYBRIDGE:	/* SNB */
// 	case INTEL_FAM6_SANDYBRIDGE_X:	/* SNB Xeon */
// 	case INTEL_FAM6_IVYBRIDGE:	/* IVB */
// 	case INTEL_FAM6_IVYBRIDGE_X:	/* IVB Xeon */
// 		pkg_cstate_limits = snb_pkg_cstate_limits;
// 		has_misc_feature_control = 1;
// 		break;
// 	case INTEL_FAM6_HASWELL:	/* HSW */
// 	case INTEL_FAM6_HASWELL_G:	/* HSW */
// 	case INTEL_FAM6_HASWELL_X:	/* HSX */
// 	case INTEL_FAM6_HASWELL_L:	/* HSW */
// 	case INTEL_FAM6_BROADWELL:	/* BDW */
// 	case INTEL_FAM6_BROADWELL_G:	/* BDW */
// 	case INTEL_FAM6_BROADWELL_X:	/* BDX */
// 	case INTEL_FAM6_SKYLAKE_L:	/* SKL */
// 	case INTEL_FAM6_CANNONLAKE_L:	/* CNL */
// 		pkg_cstate_limits = hsw_pkg_cstate_limits;
// 		has_misc_feature_control = 1;
// 		break;
// 	case INTEL_FAM6_SKYLAKE_X:	/* SKX */
// 		pkg_cstate_limits = skx_pkg_cstate_limits;
// 		has_misc_feature_control = 1;
// 		break;
// 	case INTEL_FAM6_ATOM_SILVERMONT:	/* BYT */
// 		no_MSR_MISC_PWR_MGMT = 1;
// 	case INTEL_FAM6_ATOM_SILVERMONT_D:	/* AVN */
// 		pkg_cstate_limits = slv_pkg_cstate_limits;
// 		break;
// 	case INTEL_FAM6_ATOM_AIRMONT:	/* AMT */
// 		pkg_cstate_limits = amt_pkg_cstate_limits;
// 		no_MSR_MISC_PWR_MGMT = 1;
// 		break;
// 	case INTEL_FAM6_XEON_PHI_KNL:	/* PHI */
// 		pkg_cstate_limits = phi_pkg_cstate_limits;
// 		break;
// 	case INTEL_FAM6_ATOM_GOLDMONT:	/* BXT */
// 	case INTEL_FAM6_ATOM_GOLDMONT_PLUS:
// 	case INTEL_FAM6_ATOM_GOLDMONT_D:	/* DNV */
// 	case INTEL_FAM6_ATOM_TREMONT:	/* EHL */
// 	case INTEL_FAM6_ATOM_TREMONT_D: /* JVL */
// 		pkg_cstate_limits = glm_pkg_cstate_limits;
// 		break;
// 	default:
// 		return 0;
// 	}
// 	get_msr(base_cpu, MSR_PKG_CST_CONFIG_CONTROL, &msr);
// 	pkg_cstate_limit = pkg_cstate_limits[msr & 0xF];

// 	get_msr(base_cpu, MSR_PLATFORM_INFO, &msr);
// 	base_ratio = (msr >> 8) & 0xFF;

// 	base_hz = base_ratio * bclk * 1000000;
// 	has_base_hz = 1;
// 	return 1;
// }

int has_slv_msrs(unsigned int family, unsigned int model)
{
	if (!genuine_intel)
		return 0;

	switch (model) {
	case INTEL_FAM6_ATOM_SILVERMONT:
	case INTEL_FAM6_ATOM_SILVERMONT_MID:
	case INTEL_FAM6_ATOM_AIRMONT_MID:
		return 1;
	}
	return 0;
}
int is_dnv(unsigned int family, unsigned int model)
{

	if (!genuine_intel)
		return 0;

	switch (model) {
	case INTEL_FAM6_ATOM_GOLDMONT_D:
		return 1;
	}
	return 0;
}
int is_bdx(unsigned int family, unsigned int model)
{

	if (!genuine_intel)
		return 0;

	switch (model) {
	case INTEL_FAM6_BROADWELL_X:
		return 1;
	}
	return 0;
}
int is_skx(unsigned int family, unsigned int model)
{

	if (!genuine_intel)
		return 0;

	switch (model) {
	case INTEL_FAM6_SKYLAKE_X:
		return 1;
	}
	return 0;
}
int is_ehl(unsigned int family, unsigned int model)
{
	if (!genuine_intel)
		return 0;

	switch (model) {
	case INTEL_FAM6_ATOM_TREMONT:
		return 1;
	}
	return 0;
}
int is_jvl(unsigned int family, unsigned int model)
{
	if (!genuine_intel)
		return 0;

	switch (model) {
	case INTEL_FAM6_ATOM_TREMONT_D:
		return 1;
	}
	return 0;
}
// static void
// dump_cstate_pstate_config_info(unsigned int family, unsigned int model)
// {
// 	if (!do_nhm_platform_info)
// 		return;

// 	dump_nhm_platform_info();

// 	if (has_hsw_turbo_ratio_limit(family, model))
// 		dump_hsw_turbo_ratio_limits();

// 	if (has_ivt_turbo_ratio_limit(family, model))
// 		dump_ivt_turbo_ratio_limits();

// 	if (has_turbo_ratio_limit(family, model))
// 		dump_turbo_ratio_limits(family, model);

// 	if (has_atom_turbo_ratio_limit(family, model))
// 		dump_atom_turbo_ratio_limits();

// 	if (has_knl_turbo_ratio_limit(family, model))
// 		dump_knl_turbo_ratio_limits();

// 	if (has_config_tdp(family, model))
// 		dump_config_tdp();

// 	dump_nhm_cst_cfg();
// }
// static void
// dump_sysfs_cstate_config(void)
// {
// 	char path[64];
// 	char name_buf[16];
// 	char desc[64];
// 	FILE *input;
// 	int state;
// 	char *sp;

// 	if (access("/sys/devices/system/cpu/cpuidle", R_OK)) {
// 		fprintf(outf, "cpuidle not loaded\n");
// 		return;
// 	}

// 	dump_sysfs_file("/sys/devices/system/cpu/cpuidle/current_driver");
// 	dump_sysfs_file("/sys/devices/system/cpu/cpuidle/current_governor");
// 	dump_sysfs_file("/sys/devices/system/cpu/cpuidle/current_governor_ro");

// 	for (state = 0; state < 10; ++state) {

// 		sprintf(path, "/sys/devices/system/cpu/cpu%d/cpuidle/state%d/name",
// 			base_cpu, state);
// 		input = fopen(path, "r");
// 		if (input == NULL)
// 			continue;
// 		if (!fgets(name_buf, sizeof(name_buf), input))
// 			err(1, "%s: failed to read file", path);

// 		 /* truncate "C1-HSW\n" to "C1", or truncate "C1\n" to "C1" */
// 		sp = strchr(name_buf, '-');
// 		if (!sp)
// 			sp = strchrnul(name_buf, '\n');
// 		*sp = '\0';
// 		fclose(input);

// 		remove_underbar(name_buf);

// 		sprintf(path, "/sys/devices/system/cpu/cpu%d/cpuidle/state%d/desc",
// 			base_cpu, state);
// 		input = fopen(path, "r");
// 		if (input == NULL)
// 			continue;
// 		if (!fgets(desc, sizeof(desc), input))
// 			err(1, "%s: failed to read file", path);

// 		fprintf(outf, "cpu%d: %s: %s", base_cpu, name_buf, desc);
// 		fclose(input);
// 	}
// }
// static void
// dump_sysfs_pstate_config(void)
// {
// 	char path[64];
// 	char driver_buf[64];
// 	char governor_buf[64];
// 	FILE *input;
// 	int turbo;

// 	sprintf(path, "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_driver",
// 			base_cpu);
// 	input = fopen(path, "r");
// 	if (input == NULL) {
// 		fprintf(outf, "NSFOD %s\n", path);
// 		return;
// 	}
// 	if (!fgets(driver_buf, sizeof(driver_buf), input))
// 		err(1, "%s: failed to read file", path);
// 	fclose(input);

// 	sprintf(path, "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_governor",
// 			base_cpu);
// 	input = fopen(path, "r");
// 	if (input == NULL) {
// 		fprintf(outf, "NSFOD %s\n", path);
// 		return;
// 	}
// 	if (!fgets(governor_buf, sizeof(governor_buf), input))
// 		err(1, "%s: failed to read file", path);
// 	fclose(input);

// 	fprintf(outf, "cpu%d: cpufreq driver: %s", base_cpu, driver_buf);
// 	fprintf(outf, "cpu%d: cpufreq governor: %s", base_cpu, governor_buf);

// 	sprintf(path, "/sys/devices/system/cpu/cpufreq/boost");
// 	input = fopen(path, "r");
// 	if (input != NULL) {
// 		if (fscanf(input, "%d", &turbo) != 1)
// 			err(1, "%s: failed to parse number from file", path);
// 		fprintf(outf, "cpufreq boost: %d\n", turbo);
// 		fclose(input);
// 	}

// 	sprintf(path, "/sys/devices/system/cpu/intel_pstate/no_turbo");
// 	input = fopen(path, "r");
// 	if (input != NULL) {
// 		if (fscanf(input, "%d", &turbo) != 1)
// 			err(1, "%s: failed to parse number from file", path);
// 		fprintf(outf, "cpufreq intel_pstate no_turbo: %d\n", turbo);
// 		fclose(input);
// 	}
// }


// /*
//  * rapl_probe()
//  *
//  * sets do_rapl, rapl_power_units, rapl_energy_units, rapl_time_units
//  */
// void rapl_probe(unsigned int family, unsigned int model)
// {
// 	if (genuine_intel)
// 		rapl_probe_intel(family, model);
// 	if (authentic_amd || hygon_genuine)
// 		rapl_probe_amd(family, model);
// }

// void perf_limit_reasons_probe(unsigned int family, unsigned int model)
// {
// 	if (!genuine_intel)
// 		return;

// 	if (family != 6)
// 		return;

// 	switch (model) {
// 	case INTEL_FAM6_HASWELL:	/* HSW */
// 	case INTEL_FAM6_HASWELL_L:	/* HSW */
// 	case INTEL_FAM6_HASWELL_G:	/* HSW */
// 		do_gfx_perf_limit_reasons = 1;
// 	case INTEL_FAM6_HASWELL_X:	/* HSX */
// 		do_core_perf_limit_reasons = 1;
// 		do_ring_perf_limit_reasons = 1;
// 	default:
// 		return;
// 	}
// }

// void automatic_cstate_conversion_probe(unsigned int family, unsigned int model)
// {
// 	if (is_skx(family, model) || is_bdx(family, model))
// 		has_automatic_cstate_conversion = 1;
// }

int has_snb_msrs(unsigned int family, unsigned int model)
{
	if (!genuine_intel)
		return 0;

	switch (model) {
	case INTEL_FAM6_SANDYBRIDGE:
	case INTEL_FAM6_SANDYBRIDGE_X:
	case INTEL_FAM6_IVYBRIDGE:		/* IVB */
	case INTEL_FAM6_IVYBRIDGE_X:		/* IVB Xeon */
	case INTEL_FAM6_HASWELL:		/* HSW */
	case INTEL_FAM6_HASWELL_X:		/* HSW */
	case INTEL_FAM6_HASWELL_L:		/* HSW */
	case INTEL_FAM6_HASWELL_G:		/* HSW */
	case INTEL_FAM6_BROADWELL:		/* BDW */
	case INTEL_FAM6_BROADWELL_G:		/* BDW */
	case INTEL_FAM6_BROADWELL_X:		/* BDX */
	case INTEL_FAM6_SKYLAKE_L:		/* SKL */
	case INTEL_FAM6_CANNONLAKE_L:		/* CNL */
	case INTEL_FAM6_SKYLAKE_X:		/* SKX */
	case INTEL_FAM6_ATOM_GOLDMONT:		/* BXT */
	case INTEL_FAM6_ATOM_GOLDMONT_PLUS:
	case INTEL_FAM6_ATOM_GOLDMONT_D:	/* DNV */
	case INTEL_FAM6_ATOM_TREMONT:		/* EHL */
	case INTEL_FAM6_ATOM_TREMONT_D:		/* JVL */
		return 1;
	}
	return 0;
}
int has_c8910_msrs(unsigned int family, unsigned int model)
{
	if (!genuine_intel)
		return 0;

	switch (model) {
	case INTEL_FAM6_HASWELL_L:	/* HSW */
	case INTEL_FAM6_BROADWELL:	/* BDW */
	case INTEL_FAM6_SKYLAKE_L:	/* SKL */
	case INTEL_FAM6_CANNONLAKE_L:	/* CNL */
	case INTEL_FAM6_ATOM_GOLDMONT:	/* BXT */
	case INTEL_FAM6_ATOM_GOLDMONT_PLUS:
	case INTEL_FAM6_ATOM_TREMONT:	/* EHL */
		return 1;
	}
	return 0;
}

/*
 * SKL adds support for additional MSRS:
 *
 * MSR_PKG_WEIGHTED_CORE_C0_RES    0x00000658
 * MSR_PKG_ANY_CORE_C0_RES         0x00000659
 * MSR_PKG_ANY_GFXE_C0_RES         0x0000065A
 * MSR_PKG_BOTH_CORE_GFXE_C0_RES   0x0000065B
 */
int has_skl_msrs(unsigned int family, unsigned int model)
{
	if (!genuine_intel)
		return 0;

	switch (model) {
	case INTEL_FAM6_SKYLAKE_L:	/* SKL */
	case INTEL_FAM6_CANNONLAKE_L:	/* CNL */
		return 1;
	}
	return 0;
}

int is_slm(unsigned int family, unsigned int model)
{
	if (!genuine_intel)
		return 0;
	switch (model) {
	case INTEL_FAM6_ATOM_SILVERMONT:	/* BYT */
	case INTEL_FAM6_ATOM_SILVERMONT_D:	/* AVN */
		return 1;
	}
	return 0;
}

int is_knl(unsigned int family, unsigned int model)
{
	if (!genuine_intel)
		return 0;
	switch (model) {
	case INTEL_FAM6_XEON_PHI_KNL:	/* KNL */
		return 1;
	}
	return 0;
}

int is_cnl(unsigned int family, unsigned int model)
{
	if (!genuine_intel)
		return 0;

	switch (model) {
	case INTEL_FAM6_CANNONLAKE_L: /* CNL */
		return 1;
	}

	return 0;
}

unsigned int get_aperf_mperf_multiplier(unsigned int family, unsigned int model)
{
	if (is_knl(family, model))
		return 1024;
	return 1;
}

// double discover_bclk(unsigned int family, unsigned int model)
// {
// 	if (has_snb_msrs(family, model) || is_knl(family, model))
// 		return 100.00;
// 	else if (is_slm(family, model))
// 		return slm_bclk();
// 	else
// 		return 133.33;
// }

void decode_feature_control_msr(void)
{
	unsigned long long msr;

	if (!get_msr(base_cpu, MSR_IA32_FEAT_CTL, &msr))
		fprintf(outf, "cpu%d: MSR_IA32_FEATURE_CONTROL: 0x%08llx (%sLocked %s)\n",
			base_cpu, msr,
			msr & FEAT_CTL_LOCKED ? "" : "UN-",
			msr & (1 << 18) ? "SGX" : "");
}

// void decode_misc_enable_msr(void)
// {
// 	unsigned long long msr;

// 	if (!genuine_intel)
// 		return;

// 	if (!get_msr(base_cpu, MSR_IA32_MISC_ENABLE, &msr))
// 		fprintf(outf, "cpu%d: MSR_IA32_MISC_ENABLE: 0x%08llx (%sTCC %sEIST %sMWAIT %sPREFETCH %sTURBO)\n",
// 			base_cpu, msr,
// 			msr & MSR_IA32_MISC_ENABLE_TM1 ? "" : "No-",
// 			msr & MSR_IA32_MISC_ENABLE_ENHANCED_SPEEDSTEP ? "" : "No-",
// 			msr & MSR_IA32_MISC_ENABLE_MWAIT ? "" : "No-",
// 			msr & MSR_IA32_MISC_ENABLE_PREFETCH_DISABLE ? "No-" : "",
// 			msr & MSR_IA32_MISC_ENABLE_TURBO_DISABLE ? "No-" : "");
// }


// void decode_misc_pwr_mgmt_msr(void)
// {
// 	unsigned long long msr;

// 	if (!do_nhm_platform_info)
// 		return;

// 	if (no_MSR_MISC_PWR_MGMT)
// 		return;

// 	if (!get_msr(base_cpu, MSR_MISC_PWR_MGMT, &msr))
// 		fprintf(outf, "cpu%d: MSR_MISC_PWR_MGMT: 0x%08llx (%sable-EIST_Coordination %sable-EPB %sable-OOB)\n",
// 			base_cpu, msr,
// 			msr & (1 << 0) ? "DIS" : "EN",
// 			msr & (1 << 1) ? "EN" : "DIS",
// 			msr & (1 << 8) ? "EN" : "DIS");
// }
/*
 * Decode MSR_CC6_DEMOTION_POLICY_CONFIG, MSR_MC6_DEMOTION_POLICY_CONFIG
 *
 * This MSRs are present on Silvermont processors,
 * Intel Atom processor E3000 series (Baytrail), and friends.
 */
// void decode_c6_demotion_policy_msr(void)
// {
// 	unsigned long long msr;

// 	if (!get_msr(base_cpu, MSR_CC6_DEMOTION_POLICY_CONFIG, &msr))
// 		fprintf(outf, "cpu%d: MSR_CC6_DEMOTION_POLICY_CONFIG: 0x%08llx (%sable-CC6-Demotion)\n",
// 			base_cpu, msr, msr & (1 << 0) ? "EN" : "DIS");

// 	if (!get_msr(base_cpu, MSR_MC6_DEMOTION_POLICY_CONFIG, &msr))
// 		fprintf(outf, "cpu%d: MSR_MC6_DEMOTION_POLICY_CONFIG: 0x%08llx (%sable-MC6-Demotion)\n",
// 			base_cpu, msr, msr & (1 << 0) ? "EN" : "DIS");
// }

unsigned int intel_model_duplicates(unsigned int model)
{

	switch(model) {
	case INTEL_FAM6_NEHALEM_EP:	/* Core i7, Xeon 5500 series - Bloomfield, Gainstown NHM-EP */
	case INTEL_FAM6_NEHALEM:	/* Core i7 and i5 Processor - Clarksfield, Lynnfield, Jasper Forest */
	case 0x1F:	/* Core i7 and i5 Processor - Nehalem */
	case INTEL_FAM6_WESTMERE:	/* Westmere Client - Clarkdale, Arrandale */
	case INTEL_FAM6_WESTMERE_EP:	/* Westmere EP - Gulftown */
		return INTEL_FAM6_NEHALEM;

	case INTEL_FAM6_NEHALEM_EX:	/* Nehalem-EX Xeon - Beckton */
	case INTEL_FAM6_WESTMERE_EX:	/* Westmere-EX Xeon - Eagleton */
		return INTEL_FAM6_NEHALEM_EX;

	case INTEL_FAM6_XEON_PHI_KNM:
		return INTEL_FAM6_XEON_PHI_KNL;

	case INTEL_FAM6_BROADWELL_X:
	case INTEL_FAM6_BROADWELL_D:	/* BDX-DE */
		return INTEL_FAM6_BROADWELL_X;

	case INTEL_FAM6_SKYLAKE_L:
	case INTEL_FAM6_SKYLAKE:
	case INTEL_FAM6_KABYLAKE_L:
	case INTEL_FAM6_KABYLAKE:
	case INTEL_FAM6_COMETLAKE_L:
	case INTEL_FAM6_COMETLAKE:
		return INTEL_FAM6_SKYLAKE_L;

	case INTEL_FAM6_ICELAKE_L:
	case INTEL_FAM6_ICELAKE_NNPI:
	case INTEL_FAM6_TIGERLAKE_L:
	case INTEL_FAM6_TIGERLAKE:
	case INTEL_FAM6_ROCKETLAKE:
	case INTEL_FAM6_LAKEFIELD:
	case INTEL_FAM6_ALDERLAKE:
		return INTEL_FAM6_CANNONLAKE_L;

	case INTEL_FAM6_ATOM_TREMONT_L:
		return INTEL_FAM6_ATOM_TREMONT;

	case INTEL_FAM6_ICELAKE_X:
	case INTEL_FAM6_SAPPHIRERAPIDS_X:
		return INTEL_FAM6_SKYLAKE_X;
	}
	return model;
}

// void print_dev_latency(void)
// {
// 	char *path = "/dev/cpu_dma_latency";
// 	int fd;
// 	int value;
// 	int retval;

// 	fd = open(path, O_RDONLY);
// 	if (fd < 0) {
// 		warn("fopen %s\n", path);
// 		return;
// 	}

// 	retval = read(fd, (void *)&value, sizeof(int));
// 	if (retval != sizeof(int)) {
// 		warn("read %s\n", path);
// 		close(fd);
// 		return;
// 	}
// 	fprintf(outf, "/dev/cpu_dma_latency: %d usec (%s)\n",
// 		value, value == 2000000000 ? "default" : "constrained");

// 	close(fd);
// }

void process_cpuid()
{
	unsigned int eax, ebx, ecx, edx;
	unsigned int fms, family, model, stepping, ecx_flags, edx_flags;
	unsigned int has_turbo;

	eax = ebx = ecx = edx = 0;

	MDebug("\nmax_level=%d\n", max_level);
	__cpuid(0, max_level, ebx, ecx, edx);

	if (ebx == 0x756e6547 && ecx == 0x6c65746e && edx == 0x49656e69)
		genuine_intel = 1;
	else if (ebx == 0x68747541 && ecx == 0x444d4163 && edx == 0x69746e65)
		authentic_amd = 1;
	else if (ebx == 0x6f677948 && ecx == 0x656e6975 && edx == 0x6e65476e)
		hygon_genuine = 1;

	// if (!quiet)
		MDebug("CPUID(0): %.4s%.4s%.4s \n",
			(char *)&ebx, (char *)&edx, (char *)&ecx);

	// __cpuid(1, fms, ebx, ecx, edx);
	// family = (fms >> 8) & 0xf;
	// model = (fms >> 4) & 0xf;
	// stepping = fms & 0xf;
	// if (family == 0xf)
	// 	family += (fms >> 20) & 0xff;
	// if (family >= 6)
	// 	model += ((fms >> 16) & 0xf) << 4;
	// ecx_flags = ecx;
	// edx_flags = edx;

	// /*
	//  * check max extended function levels of CPUID.
	//  * This is needed to check for invariant TSC.
	//  * This check is valid for both Intel and AMD.
	//  */
	// ebx = ecx = edx = 0;
	// __cpuid(0x80000000, max_extended_level, ebx, ecx, edx);

	// // if (!quiet) {
	// 	MDebug("0x%x CPUID levels; 0x%x xlevels; family:model:stepping 0x%x:%x:%x (%d:%d:%d)\n",
	// 		max_level, max_extended_level, family, model, stepping, family, model, stepping);
	// 	MDebug("CPUID(1): %s %s %s %s %s %s %s %s %s %s\n",
	// 		ecx_flags & (1 << 0) ? "SSE3" : "-",
	// 		ecx_flags & (1 << 3) ? "MONITOR" : "-",
	// 		ecx_flags & (1 << 6) ? "SMX" : "-",
	// 		ecx_flags & (1 << 7) ? "EIST" : "-",
	// 		ecx_flags & (1 << 8) ? "TM2" : "-",
	// 		edx_flags & (1 << 4) ? "TSC" : "-",
	// 		edx_flags & (1 << 5) ? "MSR" : "-",
	// 		edx_flags & (1 << 22) ? "ACPI-TM" : "-",
	// 		edx_flags & (1 << 28) ? "HT" : "-",
	// 		edx_flags & (1 << 29) ? "TM" : "-");
	// // }
	// if (genuine_intel)
	// 	model = intel_model_duplicates(model);

	// if (!(edx_flags & (1 << 5)))
	// 	printf("Error! CPUID: no MSR");

	// if (max_extended_level >= 0x80000007) {

	// 	/*
	// 	 * Non-Stop TSC is advertised by CPUID.EAX=0x80000007: EDX.bit8
	// 	 * this check is valid for both Intel and AMD
	// 	 */
	// 	__cpuid(0x80000007, eax, ebx, ecx, edx);
	// 	has_invariant_tsc = edx & (1 << 8);
	// }

	// /*
	//  * APERF/MPERF is advertised by CPUID.EAX=0x6: ECX.bit0
	//  * this check is valid for both Intel and AMD
	//  */

	// __cpuid(0x6, eax, ebx, ecx, edx);
	// has_aperf = ecx & (1 << 0);
	// if (has_aperf) {
	// 	BIC_PRESENT(BIC_Avg_MHz);
	// 	BIC_PRESENT(BIC_Busy);
	// 	BIC_PRESENT(BIC_Bzy_MHz);
	// }
	// do_dts = eax & (1 << 0);
	// if (do_dts)
	// 	BIC_PRESENT(BIC_CoreTmp);
	// has_turbo = eax & (1 << 1);
	// do_ptm = eax & (1 << 6);
	// if (do_ptm)
	// 	BIC_PRESENT(BIC_PkgTmp);
	// has_hwp = eax & (1 << 7);
	// has_hwp_notify = eax & (1 << 8);
	// has_hwp_activity_window = eax & (1 << 9);
	// has_hwp_epp = eax & (1 << 10);
	// has_hwp_pkg = eax & (1 << 11);
	// has_epb = ecx & (1 << 3);

	// // if (!quiet)
	// 	MDebug("CPUID(6): %sAPERF, %sTURBO, %sDTS, %sPTM, %sHWP, "
	// 		"%sHWPnotify, %sHWPwindow, %sHWPepp, %sHWPpkg, %sEPB\n",
	// 		has_aperf ? "" : "No-",
	// 		has_turbo ? "" : "No-",
	// 		do_dts ? "" : "No-",
	// 		do_ptm ? "" : "No-",
	// 		has_hwp ? "" : "No-",
	// 		has_hwp_notify ? "" : "No-",
	// 		has_hwp_activity_window ? "" : "No-",
	// 		has_hwp_epp ? "" : "No-",
	// 		has_hwp_pkg ? "" : "No-",
	// 		has_epb ? "" : "No-");

	// // if (!quiet)
	// 	decode_misc_enable_msr();


	if (max_level >= 0x7) { // && !quiet) {
		int has_sgx;

		ecx = 0;

		__cpuid_count(0x7, 0, eax, ebx, ecx, edx);

		has_sgx = ebx & (1 << 2);
		printf("CPUID(7): %sSGX\n", has_sgx ? "" : "No-");

		if (has_sgx)
			decode_feature_control_msr();
	}
	
	max_level = 0x15;
	if (max_level >= 0x15) {
		unsigned int eax_crystal;
		unsigned int ebx_tsc;

		/*
		 * CPUID 15H TSC/Crystal ratio, possibly Crystal Hz
		 */
		eax_crystal = ebx_tsc = crystal_hz = edx = 0;
		__cpuid(0x15, eax_crystal, ebx_tsc, crystal_hz, edx);
		MDebug("%d,%d,%d,%d,model:%x\n", eax_crystal, ebx_tsc, crystal_hz, edx,model);
		if (ebx_tsc != 0) {

			if (/*!quiet &&*/ (ebx != 0))
				MDebug("CPUID(0x15): eax_crystal: %d ebx_tsc: %d ecx_crystal_hz: %d\n",
					eax_crystal, ebx_tsc, crystal_hz);

			if (crystal_hz == 0)
				switch(model) {
				case INTEL_FAM6_SKYLAKE_L:	/* SKL */
					crystal_hz = 24000000;	/* 24.0 MHz */
					break;
				case INTEL_FAM6_ATOM_GOLDMONT_D:	/* DNV */
					crystal_hz = 25000000;	/* 25.0 MHz */
					break;
				case INTEL_FAM6_ATOM_GOLDMONT:	/* BXT */
				case INTEL_FAM6_ATOM_GOLDMONT_PLUS:
					crystal_hz = 19200000;	/* 19.2 MHz */
					break;
				default:
					crystal_hz = 0;
			}

			if (crystal_hz) {
				tsc_hz =  (unsigned long long) crystal_hz * ebx_tsc / eax_crystal;
				// if (!quiet)
					MDebug("TSC: %lld MHz (%d Hz * %d / %d / 1000000)\n",
						tsc_hz / 1000000, crystal_hz, ebx_tsc,  eax_crystal);
			}
		}
	}
	// if (max_level >= 0x16) {
	// 	unsigned int base_mhz, max_mhz, bus_mhz, edx;

	// 	/*
	// 	 * CPUID 16H Base MHz, Max MHz, Bus MHz
	// 	 */
	// 	base_mhz = max_mhz = bus_mhz = edx = 0;

	// 	__cpuid(0x16, base_mhz, max_mhz, bus_mhz, edx);
	// 	// if (!quiet)
	// 		MDebug("CPUID(0x16): base_mhz: %d max_mhz: %d bus_mhz: %d\n",
	// 			base_mhz, max_mhz, bus_mhz);
	// }

	// if (has_aperf)
	// 	aperf_mperf_multiplier = get_aperf_mperf_multiplier(family, model);

	// BIC_PRESENT(BIC_IRQ);
	// BIC_PRESENT(BIC_TSC_MHz);

	// if (probe_nhm_msrs(family, model)) {
	// 	do_nhm_platform_info = 1;
	// 	BIC_PRESENT(BIC_CPU_c1);
	// 	BIC_PRESENT(BIC_CPU_c3);
	// 	BIC_PRESENT(BIC_CPU_c6);
	// 	BIC_PRESENT(BIC_SMI);
	// }
	// do_snb_cstates = has_snb_msrs(family, model);

	// if (do_snb_cstates)
	// 	BIC_PRESENT(BIC_CPU_c7);

	// do_irtl_snb = has_snb_msrs(family, model);
	// if (do_snb_cstates && (pkg_cstate_limit >= PCL__2))
	// 	BIC_PRESENT(BIC_Pkgpc2);
	// if (pkg_cstate_limit >= PCL__3)
	// 	BIC_PRESENT(BIC_Pkgpc3);
	// if (pkg_cstate_limit >= PCL__6)
	// 	BIC_PRESENT(BIC_Pkgpc6);
	// if (do_snb_cstates && (pkg_cstate_limit >= PCL__7))
	// 	BIC_PRESENT(BIC_Pkgpc7);
	// if (has_slv_msrs(family, model)) {
	// 	BIC_NOT_PRESENT(BIC_Pkgpc2);
	// 	BIC_NOT_PRESENT(BIC_Pkgpc3);
	// 	BIC_PRESENT(BIC_Pkgpc6);
	// 	BIC_NOT_PRESENT(BIC_Pkgpc7);
	// 	BIC_PRESENT(BIC_Mod_c6);
	// 	use_c1_residency_msr = 1;
	// }
	// if (is_jvl(family, model)) {
	// 	BIC_NOT_PRESENT(BIC_CPU_c3);
	// 	BIC_NOT_PRESENT(BIC_CPU_c7);
	// 	BIC_NOT_PRESENT(BIC_Pkgpc2);
	// 	BIC_NOT_PRESENT(BIC_Pkgpc3);
	// 	BIC_NOT_PRESENT(BIC_Pkgpc6);
	// 	BIC_NOT_PRESENT(BIC_Pkgpc7);
	// }
	// if (is_dnv(family, model)) {
	// 	BIC_PRESENT(BIC_CPU_c1);
	// 	BIC_NOT_PRESENT(BIC_CPU_c3);
	// 	BIC_NOT_PRESENT(BIC_Pkgpc3);
	// 	BIC_NOT_PRESENT(BIC_CPU_c7);
	// 	BIC_NOT_PRESENT(BIC_Pkgpc7);
	// 	use_c1_residency_msr = 1;
	// }
	// if (is_skx(family, model)) {
	// 	BIC_NOT_PRESENT(BIC_CPU_c3);
	// 	BIC_NOT_PRESENT(BIC_Pkgpc3);
	// 	BIC_NOT_PRESENT(BIC_CPU_c7);
	// 	BIC_NOT_PRESENT(BIC_Pkgpc7);
	// }
	// if (is_bdx(family, model)) {
	// 	BIC_NOT_PRESENT(BIC_CPU_c7);
	// 	BIC_NOT_PRESENT(BIC_Pkgpc7);
	// }
	// if (has_c8910_msrs(family, model)) {
	// 	if (pkg_cstate_limit >= PCL__8)
	// 		BIC_PRESENT(BIC_Pkgpc8);
	// 	if (pkg_cstate_limit >= PCL__9)
	// 		BIC_PRESENT(BIC_Pkgpc9);
	// 	if (pkg_cstate_limit >= PCL_10)
	// 		BIC_PRESENT(BIC_Pkgpc10);
	// }
	// do_irtl_hsw = has_c8910_msrs(family, model);
	// if (has_skl_msrs(family, model)) {
	// 	BIC_PRESENT(BIC_Totl_c0);
	// 	BIC_PRESENT(BIC_Any_c0);
	// 	BIC_PRESENT(BIC_GFX_c0);
	// 	BIC_PRESENT(BIC_CPUGFX);
	// }
	// do_slm_cstates = is_slm(family, model);
	// do_knl_cstates  = is_knl(family, model);

	// if (do_slm_cstates || do_knl_cstates || is_cnl(family, model) ||
	//     is_ehl(family, model))
	// 	BIC_NOT_PRESENT(BIC_CPU_c3);

	// // if (!quiet)
	// 	// decode_misc_pwr_mgmt_msr();

	// // if (/*!quiet && */ has_slv_msrs(family, model))
	// 	// decode_c6_demotion_policy_msr();

	// rapl_probe(family, model);
	// perf_limit_reasons_probe(family, model);
	// automatic_cstate_conversion_probe(family, model);

	// // if (!quiet)
	// 	dump_cstate_pstate_config_info(family, model);

	// // if (!quiet)
	// 	print_dev_latency();
	// // if (!quiet)
	// 	dump_sysfs_cstate_config();
	// // if (!quiet)
	// 	dump_sysfs_pstate_config();

	// if (has_skl_msrs(family, model))
	// 	calculate_tsc_tweak();

	// if (!access("/sys/class/drm/card0/power/rc6_residency_ms", R_OK))
	// 	BIC_PRESENT(BIC_GFX_rc6);

	// if (!access("/sys/class/graphics/fb0/device/drm/card0/gt_cur_freq_mhz", R_OK))
	// 	BIC_PRESENT(BIC_GFXMHz);

	// if (!access("/sys/class/graphics/fb0/device/drm/card0/gt_act_freq_mhz", R_OK))
	// 	BIC_PRESENT(BIC_GFXACTMHz);

	// if (!access("/sys/devices/system/cpu/cpuidle/low_power_idle_cpu_residency_us", R_OK))
	// 	BIC_PRESENT(BIC_CPU_LPI);
	// else
	// 	BIC_NOT_PRESENT(BIC_CPU_LPI);

	// if (!access(sys_lpi_file_sysfs, R_OK)) {
	// 	sys_lpi_file = sys_lpi_file_sysfs;
	// 	BIC_PRESENT(BIC_SYS_LPI);
	// } else if (!access(sys_lpi_file_debugfs, R_OK)) {
	// 	sys_lpi_file = sys_lpi_file_debugfs;
	// 	BIC_PRESENT(BIC_SYS_LPI);
	// } else {
	// 	sys_lpi_file_sysfs = NULL;
	// 	BIC_NOT_PRESENT(BIC_SYS_LPI);
	// }

	// if (!quiet)
	// 	decode_misc_feature_control();

	printf("\n\n");

	return;
}

void allocate_fd_percpu(void)
{
	fd_percpu = calloc(topo.max_cpu_num + 1, sizeof(int));
	if (fd_percpu == NULL)
		err(-1, "calloc fd_percpu");
}

int main(int argc, char **argv)
{
    process_cpuid();
}
