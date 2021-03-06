/*
 * pm_domain.h - Definitions and headers related to device power domains.
 *
 * Copyright (C) 2011 Rafael J. Wysocki <rjw@sisk.pl>, Renesas Electronics Corp.
 *
 * This file is released under the GPLv2.
 */

#ifndef _LINUX_PM_DOMAIN_H
#define _LINUX_PM_DOMAIN_H

#include <linux/device.h>

#define GPD_IN_SUSPEND	1
#define GPD_POWER_OFF	2

struct dev_power_governor {
	bool (*power_down_ok)(struct dev_pm_domain *domain);
};

struct generic_pm_domain {
	struct dev_pm_domain domain;	/* PM domain operations */
	struct list_head sd_node;	/* Node in the parent's subdomain list */
	struct generic_pm_domain *parent;	/* Parent PM domain */
	struct list_head sd_list;	/* List of dubdomains */
	struct list_head dev_list;	/* List of devices */
	struct mutex lock;
	struct dev_power_governor *gov;
	struct work_struct power_off_work;
	unsigned int in_progress;	/* Number of devices being suspended now */
	unsigned int sd_count;	/* Number of subdomains with power "on" */
	bool power_is_off;	/* Whether or not power has been removed */
	unsigned int device_count;	/* Number of devices */
	unsigned int suspended_count;	/* System suspend device counter */
	unsigned int prepared_count;	/* Suspend counter of prepared devices */
	bool suspend_power_off;	/* Power status before system suspend */
	int (*power_off)(struct generic_pm_domain *domain);
	int (*power_on)(struct generic_pm_domain *domain);
	int (*start_device)(struct device *dev);
	int (*stop_device)(struct device *dev);
	bool (*active_wakeup)(struct device *dev);
};

static inline struct generic_pm_domain *pd_to_genpd(struct dev_pm_domain *pd)
{
	return container_of(pd, struct generic_pm_domain, domain);
}

struct dev_list_entry {
	struct list_head node;
	struct device *dev;
	bool need_restore;
};

#ifdef CONFIG_PM_GENERIC_DOMAINS
extern int pm_genpd_add_device(struct generic_pm_domain *genpd,
			       struct device *dev);
extern int pm_genpd_remove_device(struct generic_pm_domain *genpd,
				  struct device *dev);
extern int pm_genpd_add_subdomain(struct generic_pm_domain *genpd,
				  struct generic_pm_domain *new_subdomain);
extern int pm_genpd_remove_subdomain(struct generic_pm_domain *genpd,
				     struct generic_pm_domain *target);
extern void pm_genpd_init(struct generic_pm_domain *genpd,
			  struct dev_power_governor *gov, bool is_off);
#else
static inline int pm_genpd_add_device(struct generic_pm_domain *genpd,
				      struct device *dev)
{
	return -ENOSYS;
}
static inline int pm_genpd_remove_device(struct generic_pm_domain *genpd,
					 struct device *dev)
{
	return -ENOSYS;
}
static inline int pm_genpd_add_subdomain(struct generic_pm_domain *genpd,
					 struct generic_pm_domain *new_sd)
{
	return -ENOSYS;
}
static inline int pm_genpd_remove_subdomain(struct generic_pm_domain *genpd,
					    struct generic_pm_domain *target)
{
	return -ENOSYS;
}
static inline void pm_genpd_init(struct generic_pm_domain *genpd,
				 struct dev_power_governor *gov, bool is_off) {}
#endif

#endif /* _LINUX_PM_DOMAIN_H */
