AM_DEFAULT_SOURCE_EXT = .cpp

bin_PROGRAMS = pel_test

pel_test_SOURCES = pel_test.cpp

generic_ldflags = \
                 $(PHOSPHOR_LOGGING_LIBS)

generic_cxxflags = \
                  $(PHOSPHOR_LOGGING_CFLAGS)

pel_test_LDFLAGS  = ${generic_ldflags}
pel_test_CXXFLAGS  = ${generic_cxxflags}

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
