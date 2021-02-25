#!/bin/bash

work_dir=`pwd`
pkg_name=`basename $work_dir`
time=`date +%Y.%m.%d`
src_dir=${work_dir}/src
output_path=${work_dir}/deb/debug
version=0.0.0
deb_name=${pkg_name}_${version}-${time}_amd64.deb

deb_dir=${work_dir}/tmp_${pkg_name}

raw_folder_list=()

root_commands=""

get_version_from_control(){
    pkg_name=`grep Package: ${deb_dir}/DEBIAN/control | head -n1 | awk -F : '{print $2}' | xargs echo`
    version=`grep Version: ${deb_dir}/DEBIAN/control | head -n1 | awk -F : '{print $2}' | xargs echo`
}

generate_deb_name(){
    deb_name=${pkg_name}_${version}-${time}_amd64.deb
}

generate_md5() {
    cd ${deb_dir}
    find . -type f ! -path './DEBIAN/*' -exec md5sum {} \;> DEBIAN/md5sums
    cd ${work_dir}
}

# git can't add raw folder, and sometimes it's not feasible to put a .gitignore there.
# E.g. /var/run
# So mkdir every time.
mk_raw_folder(){
    cd ${deb_dir}
    for folder in "${raw_folder_list[@]}"
    do
    if [ ! -d $folder ];then
        mkdir -p $folder
    fi
    done
    cd ${work_dir}
}

check_output_path(){
    if [ ! -d ${output_path} ];then
        mkdir -p ${output_path}
    fi
}

get_files_to_pack(){
    # Build files, copy files...... From src_dir to deb_dir
    if [ ! -d ${deb_dir} ];then
        mkdir -p ${deb_dir}
    fi
    cp -r ${src_dir}/* ${deb_dir}/
}

prepare_commands() {
    get_files_to_pack
    get_version_from_control
    generate_md5
    mk_raw_folder
    check_output_path
    generate_deb_name


    # The fakeroot commands.
    root_commands="
        dpkg -b ${deb_dir} ${output_path}/${deb_name}
    "
}

finish_commands() {
    :
    #rm -rf ${deb_dir}
}


prepare_commands
fakeroot-tcp -- ${root_commands}
finish_commands
