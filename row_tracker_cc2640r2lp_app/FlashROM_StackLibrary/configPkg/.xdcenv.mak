#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /Applications/ti/simplelink_cc2640r2_sdk_2_30_00_28/source;/Applications/ti/simplelink_cc2640r2_sdk_2_30_00_28/kernel/tirtos/packages;/Applications/ti/simplelink_cc2640r2_sdk_2_30_00_28/source/ti/blestack
override XDCROOT = /Applications/ti/xdctools_3_50_08_24_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /Applications/ti/simplelink_cc2640r2_sdk_2_30_00_28/source;/Applications/ti/simplelink_cc2640r2_sdk_2_30_00_28/kernel/tirtos/packages;/Applications/ti/simplelink_cc2640r2_sdk_2_30_00_28/source/ti/blestack;/Applications/ti/xdctools_3_50_08_24_core/packages;..
HOSTOS = MacOS
endif
