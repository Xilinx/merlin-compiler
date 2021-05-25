find_package(PythonInterp 2.7 EXACT REQUIRED)
#find_program(VIRTUALENV virtualenv)
#if(NOT VIRTUALENV)
#  message(FATAL_ERROR "Could not find `virtualenv` in PATH")
#endif()

set(DSE_ROOT ${PROJECT_SOURCE_DIR}/optimizers/autodse)
set(REQ ${DSE_ROOT}/libs/opentuner/requirements.txt)
set(CHECK_REQ ${DSE_ROOT}/checks/req_py3.txt)

add_subdirectory(tools/ds_bitwidth_opt)
add_subdirectory(tools/ds_loop_parallel)
add_subdirectory(tools/ds_tiling)
add_subdirectory(tools/ds_scope)
add_subdirectory(tools/ds_resource_eval)
add_subdirectory(tools/ds_skeleton_extract)
add_subdirectory(tools/ds_pipeline)
add_subdirectory(tools/ds_mark)

#add_custom_target(venv
#    ALL
#    DEPENDS venv.o
#    DEPENDS lib.o
#    DEPENDS script.o)

add_custom_command(OUTPUT venv.o
    DEPENDS ${REQ}
    COMMAND virtualenv ${CMAKE_BINARY_DIR}/venv
    COMMAND ${CMAKE_BINARY_DIR}/venv/bin/pip install --upgrade pip  
    COMMAND ${CMAKE_BINARY_DIR}/venv/bin/pip install -r ${REQ}
    COMMAND touch venv.o)

add_custom_command(OUTPUT lib.o
    DEPENDS venv.o
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/api.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/driverbase.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/__init__.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/tuningrunmain.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/measurement/driver.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/measurement/__init__.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/measurement/inputmanager.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/measurement/interface.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/resultsdb/connect.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/resultsdb/__init__.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/resultsdb/models.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/search/bandittechniques.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/search/composableevolutionarytechniques.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/search/differentialevolution.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/search/driver.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/search/evolutionarytechniques.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/search/globalGA.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/search/gradientsearch.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/search/hotspotopt.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/search/exhaustive.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/search/singlesearch.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/search/__init__.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/search/manipulator.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/search/metatechniques.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/search/objective.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/search/patternsearch.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/search/plugin.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/search/pso.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/search/simplextechniques.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/search/simulatedannealing.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/search/technique.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/utils/adddeps.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/utils/compactdb.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/utils/dictconfig.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/utils/__init__.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/utils/stats_matplotlib.py
    DEPENDS ${DSE_ROOT}/libs/opentuner/opentuner/utils/stats.py
    COMMAND ${CMAKE_BINARY_DIR}/venv/bin/python -m compileall ${DSE_ROOT}/libs
    COMMAND touch lib.o)

add_custom_command(OUTPUT script.o
    DEPENDS venv.o
    DEPENDS ${DSE_ROOT}/scripts/dse_engine.py
    DEPENDS ${DSE_ROOT}/scripts/dse_objective.py
    DEPENDS ${DSE_ROOT}/scripts/dse.py
    DEPENDS ${DSE_ROOT}/scripts/dse_run_hls.py
    DEPENDS ${DSE_ROOT}/scripts/dse_midend_checker.py
    DEPENDS ${DSE_ROOT}/scripts/dse_cache.py
    DEPENDS ${DSE_ROOT}/scripts/dse_cache_perf_xilinx.py
    DEPENDS ${DSE_ROOT}/scripts/dse_cache_res_xilinx.py
    DEPENDS ${DSE_ROOT}/scripts/dse_cache_perf_intel.py
    DEPENDS ${DSE_ROOT}/scripts/dse_cache_res_intel.py
    DEPENDS ${DSE_ROOT}/scripts/dse_mock.py
    DEPENDS ${DSE_ROOT}/scripts/dse_proc_eval.py
    DEPENDS ${DSE_ROOT}/scripts/dse_type.py
    DEPENDS ${DSE_ROOT}/scripts/dse_util.py
    DEPENDS ${DSE_ROOT}/scripts/ds_proc_applier.py
    DEPENDS ${DSE_ROOT}/scripts/ds_proc_builder.py
    DEPENDS ${DSE_ROOT}/scripts/ds_proc_profiler.py
    DEPENDS ${DSE_ROOT}/scripts/merlin_logger.py
    DEPENDS ${DSE_ROOT}/scripts/util.py
    COMMAND ${CMAKE_BINARY_DIR}/venv/bin/python -m compileall ${DSE_ROOT}/scripts
    COMMAND touch script.o)

# Code Checking Process
option(DSE_PYTHON_CHECK "Build DSE Python checking environment and run the check (require Python3)" ON)
if(DSE_PYTHON_CHECK)
    message(STATUS "DSE Python checking is enabled (DSE_PYTHON_CHECK=ON)")
    find_program(PYTHON3 python3)
    if(NOT PYTHON3)
        message(WARNING "Could not find `python3` in PATH, dse_check won't work")
    endif()

    add_custom_target(dse_check
        DEPENDS dse_check_venv2.o
        DEPENDS dse_check_venv3.o
        COMMAND sh ${DSE_ROOT}/checks/run_lint.sh ${DSE_ROOT} ${CMAKE_BINARY_DIR}/dse_check_venv2 lint2.rpt 2
        COMMAND sh ${DSE_ROOT}/checks/run_lint.sh ${DSE_ROOT} ${CMAKE_BINARY_DIR}/dse_check_venv3 lint3.rpt 3
        COMMAND sh ${DSE_ROOT}/checks/run_type.sh ${DSE_ROOT} ${CMAKE_BINARY_DIR}/dse_check_venv3 type2.rpt 2)

    add_custom_command(OUTPUT dse_check_venv2.o
        DEPENDS ${REQ}
        COMMAND virtualenv -p python2 ${CMAKE_BINARY_DIR}/dse_check_venv2
        COMMAND ${CMAKE_BINARY_DIR}/dse_check_venv2/bin/pip install --upgrade pip  
        COMMAND ${CMAKE_BINARY_DIR}/dse_check_venv2/bin/pip install -r ${REQ}
        COMMAND ${CMAKE_BINARY_DIR}/dse_check_venv2/bin/pip install pylint
        COMMAND touch dse_check_venv2.o)

    add_custom_command(OUTPUT dse_check_venv3.o
        DEPENDS ${CHECK_REQ}
        COMMAND virtualenv -p python3 ${CMAKE_BINARY_DIR}/dse_check_venv3
        COMMAND ${CMAKE_BINARY_DIR}/dse_check_venv3/bin/pip install --upgrade pip  
        COMMAND ${CMAKE_BINARY_DIR}/dse_check_venv3/bin/pip install -r ${CHECK_REQ}
        COMMAND ${CMAKE_BINARY_DIR}/dse_check_venv3/bin/pip install mypy
        COMMAND ${CMAKE_BINARY_DIR}/dse_check_venv3/bin/pip install pylint
        COMMAND touch dse_check_venv3.o)
endif()
