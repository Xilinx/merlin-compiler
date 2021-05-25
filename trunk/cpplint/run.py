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
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/source-opt/tools').glob('**/*.h'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/optimizers/autodse/tools').glob('**/*.cpp'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/optimizers/systolic/tools/systolic_extract/src').glob('*.cpp'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/optimizers/systolic/tools/systolic_frontend/src').glob('*.cpp'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/mars-gen/tools').glob('**/*.cpp'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/optimizers/autodse/tools').glob('**/*.h'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/optimizers/systolic/tools/systolic_extract/src').glob('*.h'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/optimizers/systolic/tools/systolic_frontend/src').glob('*.h'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/mars-gen/tools').glob('**/*.h'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/legacy/tools/parsers').glob('**/*.h'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/legacy/tools/ir').glob('**/*.h'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/legacy/tools/file_compare').glob('**/*.h'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/legacy/tools/file_split').glob('**/*.h'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/legacy/tools/utils').glob('**/*.h'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/legacy/design_space/task_space_gen').glob('**/*.h'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/legacy/design_space/merge_metrics').glob('**/*.h'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/legacy/design_space/modsel2cfg').glob('**/*.h'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/legacy/tools/parsers').glob('**/*.cpp'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/legacy/tools/ir').glob('**/*.cpp'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/legacy/tools/file_compare').glob('**/*.cpp'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/legacy/tools/file_split').glob('**/*.cpp'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/legacy/tools/utils').glob('**/*.cpp'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/legacy/design_space/task_space_gen').glob('**/*.cpp'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/legacy/design_space/merge_metrics').glob('**/*.cpp'))
file_list.extend(Path(os.getenv('MERLIN_COMPILER_HOME', '.') + '/legacy/design_space/modsel2cfg').glob('**/*.cpp'))

cmd="rm -rf *.rpt"                                                                                                           
os.system(cmd);

existing_fail_files = { 
"source-opt_tools_preprocess_src_preprocess.cpp.rpt" : 1,
"optimizers_systolic_tools_systolic_frontend_src_SystolicData.cpp.rpt" : 1,
"legacy_tools_ir_PolyModel.cpp.rpt" : 1, 
"source-opt_tools_reduction_src_reduction.cpp.rpt" : 1,
"source-opt_tools_replace_exp_visitor_src_replace_exp_visitor.cpp.rpt" : 13
}

def is_new_failed_file(filename, num_warning):
  for one_file,one_warning_num in existing_fail_files.items():
      if -1 != filename.find(one_file) and one_warning_num >= num_warning: 
         return False
  return True


for file_name in file_list :
  full_name = str(file_name)
  part_name = full_name.replace(os.getenv('MERLIN_COMPILER_HOME', '.') + '/', "", 1) 
  report_name=re.sub(r"/", "_", part_name) + ".rpt"
  cmd = "cpplint --filter=-build/include_subdir,-legal/copyright "+full_name+" &>"+report_name + " &"
  os.system(cmd)
os.system("sleep 60")
pattern_dict = {}
report_dict = {}
pattern_list = {}
pattern_list = set()
remaining_issue = 0
for file_name in Path('./').glob('**/*.rpt'):
  with open(file_name) as fin:
    full_name = str(file_name)
    idx = full_name.rfind('/')
    report_name=full_name[idx+1:-4]
    report_dict[report_name] = {}
    found = 0
    curr_issue = 0;
    for line in fin:
      pattern = re.search('\[[a-zA-Z0-9_]*[/][a-zA-Z0-9_]*\]', line)
      if pattern:
        remaining_issue = remaining_issue + 1
        curr_issue = curr_issue + 1
        found = 1
        pattern_str = str(pattern.group(0))
        pattern_list.add(pattern_str)
        if pattern_str not in report_dict[report_name]:
          report_dict[report_name][pattern_str] = 1
        else:
          report_dict[report_name][pattern_str] = report_dict[report_name][pattern_str] + 1          
    if found:
      if is_new_failed_file(full_name, curr_issue):
        print(full_name, " contains cpplint warning") 

for pattern in pattern_list:
  pattern_dict[pattern] = 0

for report_file in report_dict.keys():
  for pattern in pattern_list:
    if pattern not in report_dict[report_file]:
      report_dict[report_file][pattern] = 0
    pattern_dict[pattern] = pattern_dict[pattern]+report_dict[report_file][pattern]

#for item in report_dict:  
#  print(item, report_dict[item]) 



with open("lintreport.csv", "w") as csvfile:
  writer = csv.writer(csvfile)
  writer.writerow(["file_name"] + sorted(pattern_list))
  for report_file in sorted(report_dict.keys()):
    report_row = [report_file]
    for pattern in sorted(report_dict[report_file].keys()):
      report_row.append(report_dict[report_file][pattern])
    writer.writerow(report_row)

  for key, value in sorted(pattern_dict.items(), key=lambda item:item[1], reverse=True):
    writer.writerow([key, value])

if remaining_issue > 17:
  sys.exit("New commit breaks coding style. Please rerun cpplint check under directory cpplint:\n" +
           "python3 run.py\n") 
  
