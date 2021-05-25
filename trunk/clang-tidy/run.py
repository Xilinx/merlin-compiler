from pathlib import Path
import os
import re
import csv
import operator
import sys
if not 'MERLIN_COMPILER_HOME' in os.environ:
  print('Error: MERLIN_COMPILER_HOME is not set')
  sys.exit()
file_list = []
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/source-opt/tools').glob('**/*.cpp'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/optimizers/autodse/tools').glob('**/*.cpp'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/optimizers/systolic/tools/systolic_extract/src').glob('*.cpp'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/optimizers/systolic/tools/systolic_frontend/src').glob('*.cpp'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/mars-gen/tools').glob('**/*.cpp'))
#file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/legacy/tools').glob('**/*.cpp'))
            
#export compile commands
cmd = "cd "  + os.getenv('MERLIN_COMPILER_HOME', '.') + "/build; cmake3 -GNinja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..; ninja"
print(cmd)
os.system(cmd)
for file_name in file_list :
  full_name = str(file_name)
  cmd = "/curr/software/dev-tools/llvm-8.0.1/bin/clang-tidy -p "+ os.getenv('MERLIN_COMPILER_HOME', '.') +  "/build -header-filter=.* "+full_name +  " &>" + full_name + ".log &" 
  os.system(cmd)
os.system("sleep 300")
os.system("rm -rf raw_log exclude_third_party_log uniq_log")
for file_name in file_list :
  full_name = str(file_name)
  cmd = "cat " + full_name + ".log >> raw_log"
  os.system(cmd)
os.system("sed  '/merlin-deps/d' raw_log > exclude_third_party_log")
os.system("cat exclude_third_party_log | sort | uniq > uniq_log")
warning_count = 0
with open("uniq_log") as res:
  for line in res:
    pattern = re.search("warning:", line)
    if pattern:
      warning_count = warning_count + 1
      print(line)
print("current number of warning is ", warning_count)
warning_count_threshold = 0 
if warning_count > warning_count_threshold:
  sys.exit("New commit brings in new static analysis warning because the number of warnings is " + warning_count + " which is more than 25.\n")
elif warning_count < warning_count_threshold:
  print("Please update the above variable \'warning_count_threshold\'")

