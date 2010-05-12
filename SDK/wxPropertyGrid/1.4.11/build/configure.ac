# ======================================================================================
# Author: Francesco Montorsi
#
# A basic "configure.ac" for a wxCode component.
#
# Customized for propgrid by gen_makefile.py.
#
# See
#    http://www.gnu.org/software/autoconf/manual/html_node/index.html
# for more info about autoconf and the autoconf macros.
# ======================================================================================


# Init this configure script with basic info about the component
# (DON'T PUT ANYTHING BEFORE AC_INIT, JUST COMMENTS)

# NOTE: you'll want to replace the first string in [] brackets with the name of your
#       component, the second string with its version and third one with your mail
#       (so that users will be able to contact you in case they have problems)
AC_INIT([PROPGRID], [1.4.11], [jmsalli@users.sourceforge.net])
AC_CONFIG_AUX_DIR(build)        # this is needed, don't touch it

# ENABLES/DISABLES THE DEBUG MODE FOR THIS CONFIGURE SCRIPT
# (you won't need it unless you get in troubles)
#WX_DEBUG_CONFIGURE=1

# you need the wxCode/build/bakefiles in your local repository to generate a configure
# script from this configure.ac file since wxcode.m4 is required....
m4_include(../../../build/autoconf/wxcode.m4)
WXCODE_INIT




## CONFIGURE OPTIONS
##
## Before starting checks, declare the options of this configure script
## Here you should use the AC_ARG_ENABLE and AC_ARG_WITH macros,
## or the wxCode-specific AM_WXCODE_ARG_ENABLE & AM_WXCODE_ARG_WITH macros,
## to add to this configure script the --enable-XXX and/or --with-XXX
## options required. If you did not use any <option> tag in your
## component's bakefile, then you can leave this section as is
#####################################################################

# this macro will declare various options for this configure script
# looking at the comma-separed list passed as argument:
# --enable-debug      when 'debug' is in the list
# --enable-unicode    when 'unicode' is in the list
# --enable-shared     when 'shared' is in the list
# --with-toolkit      when 'toolkit' is in the list
# --with-wxshared     when 'wxshared' is in the list
# --with-wxversion    when 'wxversion' is in the list
#
# All these options will default to an "auto" value which means that
# wx-config will be used to find a matching wxWidgets build and that
# the project will then be built with the same wxWidgets configuration.
#
# NOTE: you typically won't need to touch anything in this section
WXCODE_OPTIONS([debug,unicode,shared,toolkit,wxshared,wxversion])





## CONFIGURE CHECKS
##
## Here you should use the AC_CHECK_LIB, AC_COMPILE_IFELSE,
## AC_LINK_IFELSE, etc macros to check that the libraries required
## by your component exist on the host machine and match your
## required options (version, build settings, etc)
#####################################################################

# argument 1: here put the minimum required version of wx
# argument 2: here you must put a comma-separed list of all wx libraries required
#             by your project.
#             If your project builds multiple targets the libraries listed here are
#             passed to the linker for *all* targets (thus you'll need to write here
#             the union of all wx-library sets used by your targets).
#             NOTE: the libraries should be listed in the correct order: from
#                   the most-advanced to the basic ones (e.g. 'base' _must_
#                   always be the last listed library).
WXCODE_CHECKS([2.6.4], [xrc,html,xml,adv,core,base])      # e.g. we need at least wxWidgets 2.8 to work and
                                         # we want to link against wxCore and wxBase libs

# Other things which can be useful to add here can be:
# AC_PROG_CC                                              if you want to compile .c sources and not only .cpp ones
# CPPFLAGS="$CPPFLAGS `pkg-config gtk+-2.0 --cflags`"     if you want to use GTK+ 2 headers directly

# end in a nice way the configure script
WXCODE_END
