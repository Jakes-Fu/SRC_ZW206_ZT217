#!/bin/bash

#git log --oneline              #find final commit id
#git log --oneline --reverse    #find base commit id

base_commit=cf765f1f          #
final_commit=7ca52d12         #

diff_path=diff_dir

if [-d ${diff_path}];then
    echo "diff pathc exist"
else
    mkdir ${diff_path}
fi

cp --parents $(git diff --name-only ${base_commit}..${final_commit}) ${diff_path}/

#rm -rf ${diff_path}/external/oneos
#rm -rf ${diff_path}/Third-party/oneos

tar -czvf diff.tar.gz -C ${diff_path} .

rm -rf ${diff_path}
