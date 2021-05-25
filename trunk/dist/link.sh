########################################################################
# check_symbolic() takes a file/folder path and returns a list of folders. 
# These are the smallest folders to allow every sympolic links under the input
# path be valid.
#
# for example:
# dir1-|-dir2 -|-file1
#      |-file5 |-file2
#              |-file3 <----|
# dir3-|-file4              |
#      |-link1 -------------|
#      |-link2 ---------|
# dir4-|-dir5  <--------|
# 1. if the input is file4, the function returns dir3
# 2. if the input is link1, the function returns dir3 and dir2
# 3. if the input is dir3, the function returns dir3, dir2 and dir5
# 4. if the input is dir2, the function returns dir2 
# 5. if the input is file5, the function returns dir1
# 6. if the input is link2, the function returns link2(not dir3) and dir5(not dir4)
########################################################################

# hashmaps
declare -A to_mount
declare -A visited
declare -A checked

function not_mounted {
  local path=$1
  if [ ${checked[$path]} ]; then
    return 1
  fi
  local save=$path
  local flag=0
  while [ "$path" != "/" ]; do
    if [ ${to_mount[$path]} ]; then
      checked[$path]=1
      return 1
      break
    fi
    path=$(dirname $path)
  done
  return 0
}

function add_mount {
  local path=$1
  if not_mounted $path ; then
    to_mount[$path]=1
  fi
}

function valid {
  path=$1
  if [ -f $path ] || [ -d $path ] || [ -L $path ]; then
    return 0
  fi
  return 1
}

function check_symbolic {
  # queue
  in=$(realpath -sq $1)
  arr=("$in")

  # shallow: not expanding directories
  if [ "$2" = "shallow" ]; then
    shallow=1
  fi

  while [ "${#arr[@]}" -gt 0 ]; do
    cur="${arr[0]}"
    arr=( "${arr[@]:1}" )
    
    if [ ${visited[$cur]} ]; then
      continue
    fi
    visited[$cur]=1
    
    if [ -L "$cur" ]; then
      if [ -d "$cur" ]; then
        add_mount $cur
      else
        add_mount $(dirname $cur)
      fi      

      # get link direction
      path=`ls -l $cur 2>/dev/null | awk -F"-> " '{print $2}' 2>/dev/null`
      # see if the path starts with "/"
      if [[ "$path" =~ ^/.* ]]; then
        path=$(realpath -sq $path)
      else
        path=$(realpath -sq $(dirname $cur)/$path)
      fi
      arr=( "${arr[@]}" $path )
    elif [ -f "$cur" ]; then
      add_mount $(dirname $cur)
    elif [ -d "$cur" ]; then
      add_mount $cur
      if [ -z $shallow ]; then
        # get all symbolic links in the directory. 
        # ( if field $13 does not have "/", the link is linking to a file 
        # in the same directory as the link. So no need to check again. )
        for link in `find $cur -type l -ls 2>/dev/null | \
                     awk '{if($13 ~ "/") print $11}' 2>/dev/null`; do
          arr=( "${arr[@]}" $link )
        done
      fi
    fi
  done  
}

function print_mount {
  for path in "${!to_mount[@]}"; do
    dir=$(dirname $path)
    if not_mounted $dir ; then
      echo $path
    fi
  done
}
