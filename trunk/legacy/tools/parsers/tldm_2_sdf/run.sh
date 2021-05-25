
# mpeg.tldm -> mpeg.xml
#./tldm_parser.exe mpeg.tldm mpeg.xml

# generate tldm_2_sdf_parser
# make clean
# make

# mpeg.xml -> sdf
  rm -f mpeg.sdf 
  ./xml2sdf two_task_fed.tldm.xml platform_metrics.xml directive.xml simple_med_pipe.sdf
