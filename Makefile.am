AM_DEFAULT_SOURCE_EXT = .cpp

bin_PROGRAMS = pel_test

nobase_nodist_include_HEADERS = \
            xyz/openbmc_project/CustomErr/Example/error.hpp

BUILT_SOURCES = \
            xyz/openbmc_project/CustomErr/Example/error.cpp \
            xyz/openbmc_project/CustomErr/Example/error.hpp

CLEANFILES = ${BUILT_SOURCES}

pel_test_SOURCES = pel_test.cpp \
                   xyz/openbmc_project/CustomErr/Example/error.cpp

generic_ldflags = \
                 $(PHOSPHOR_LOGGING_LIBS) \
                 $(SDBUSPLUS_LIBS)

generic_cxxflags = \
                  $(PHOSPHOR_LOGGING_CFLAGS) \
                  $(SDBUSPLUS_CFLAGS)

pel_test_LDFLAGS  = ${generic_ldflags}
pel_test_CXXFLAGS  = ${generic_cxxflags}

xyz/openbmc_project/CustomErr/Example/error.hpp: ${top_srcdir}/xyz/openbmc_project/CustomErr/Example.errors.yaml
	@mkdir -p `dirname $@`
	$(SDBUSPLUSPLUS) -r $(srcdir) error exception-header xyz.openbmc_project.CustomErr.Example> $@

xyz/openbmc_project/CustomErr/Example/error.cpp: ${top_srcdir}/xyz/openbmc_project/CustomErr/Example.errors.yaml
	@mkdir -p `dirname $@`
	$(SDBUSPLUSPLUS) -r $(srcdir) error exception-cpp xyz.openbmc_project.CustomErr.Example> $@

# Export error YAML to shared location
#yamldir = ${datadir}/phosphor-dbus-yaml/yaml
#nobase_yaml_DATA = \
#	xyz/openbmc_project/custom_error/Example.errors.yaml \
#   xyz/openbmc_project/custom_error/Example.metadata.yaml


#CALLOUTS_YAML ?= ${abs_srcdir}/callouts/callouts.yaml
#REQ_FILES_TO_GEN ?= ${CALLOUTS_YAML} \
#	${abs_srcdir}/callouts/callouts.py \
#    ${abs_srcdir}/callouts/callouts-gen.mako.hpp
#callouts-gen.hpp: $(REQ_FILES_TO_GEN)
#	$(AM_V_at)$(PYTHON) ${abs_srcdir}/callouts/callouts.py -i ${CALLOUTS_YAML}
