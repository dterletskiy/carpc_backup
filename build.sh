#!/bin/bash

ENVIRONMENT_SETUP_MIB3=/packages/sdk/mib3h/installed/environment-setup-aarch64-poky-linux
ENVIRONMENT_SETUP_HCP3=/packages/sdk/hcp3sim/installed/environment-setup-core2-64-hcp3-linux-musl
ENVIRONMENT_SETUP_ANDROID=./.configuration/android-env.sh



ANDROID_INSTALL_PATH=/data/local/tmp/tda



declare -A COMMAND=(
   [INFO]="--info"                  [INFO_S]="-i"
   [CLEAN]="--clean"                [CLEAN_S]="-c"
   [CONFIGURE]="--configure"        [CONFIGURE_S]="-s"
   [BUILD]="--build"                [BUILD_S]="-b"
   [CLEAN_BUILD]="--clean_build"    [CLEAN_BUILD_S]="-x"
   [INSTALL]="--install"            [INSTALL_S]="-d"
   [ARCHIVE]="--archive"            [ARCHIVE_S]="-a"
   [START]="--start"                [START_S]="-r"
   [STOP]="--stop"                  [STOP_S]="-f"
   [ADB_INSTALL]="--adb_install"    [ADB_INSTALL_S]="-z"
)

function info( )
{
   echo "   ${COMMAND[INFO]}         | ${COMMAND[INFO_S]}         print info"
   echo "   ${COMMAND[CLEAN]}        | ${COMMAND[CLEAN_S]}        clean product folder"
   echo "   ${COMMAND[CONFIGURE]}    | ${COMMAND[CONFIGURE_S]}    configure build"
   echo "   ${COMMAND[BUILD]}        | ${COMMAND[BUILD_S]}        build project"
   echo "   ${COMMAND[CLEAN_BUILD]}  | ${COMMAND[CLEAN_BUILD_S]}  clean build project"
   echo "   ${COMMAND[INSTALL]}      | ${COMMAND[INSTALL_S]}      install built project"
   echo "   ${COMMAND[ARCHIVE]}      | ${COMMAND[ARCHIVE_S]}      archive project"
   echo "   ${COMMAND[START]}        | ${COMMAND[START_S]}        start installed project"
   echo "   ${COMMAND[STOP]}         | ${COMMAND[STOP_S]}         stop installed project"
   echo "   ${COMMAND[ADB_INSTALL]}  | ${COMMAND[ADB_INSTALL_S]}  install to device via adb"
}

declare -A PROJECT

CORES=$( grep -c ^processor /proc/cpuinfo )



ACTION=${1}
TARGET_OS=${2}

readonly OS_ANDROID="0"
readonly OS_LINUX="1"

OS_TARGET=${OS_LINUX}
case ${TARGET_OS} in
   "linux")
      OS_TARGET=${OS_LINUX}
   ;;
   "android")
      OS_TARGET=${OS_ANDROID}
      ENVIRONMENT_SETUP=${ENVIRONMENT_SETUP_ANDROID}
   ;;
   *)
      OS_TARGET=${OS_LINUX}
   ;;
esac
CMAKE_PARAMETERS_OS="-DOS_ANDROID:STRING=${OS_ANDROID} -DOS_LINUX:STRING=${OS_LINUX} -DOS_TARGET:STRING=${OS_TARGET}"



function setup_sdk( )
{
   local SDK=${1}

   # Check if 'SDK' is set
   # https://stackoverflow.com/questions/3601515/how-to-check-if-a-variable-is-set-in-bash
   if [ -z ${SDK+x} ]; then
      echo "SDK is unset";
   elif [ -z ${SDK} ]; then
      echo "SDK is empty";
   else
      echo "SDK is set to '${SDK}'";
      if [ ! -f "${SDK}" ]; then
         echo "'${SDK}' file does not exist"
         exit
      fi
      source ${SDK}
   fi
}

function print( )
{
   local -n LOCAL_PRINT__PROJECT=${1}

   echo "ROOT_DIR:           ${LOCAL_PRINT__PROJECT[ROOT_DIR]}"
   echo "ROOT_DIR_NAME:      ${LOCAL_PRINT__PROJECT[ROOT_DIR_NAME]}"
   echo "SOURCE_DIR:         ${LOCAL_PRINT__PROJECT[SOURCE_DIR]}"
   echo "PRODUCT_DIR:        ${LOCAL_PRINT__PROJECT[PRODUCT_DIR]}"
   echo "BUILD_DIR:          ${LOCAL_PRINT__PROJECT[BUILD_DIR]}"
   echo "DELIVERY_DIR:       ${LOCAL_PRINT__PROJECT[DELIVERY_DIR]}"
   echo "DOCUMENTATION_DIR:  ${LOCAL_PRINT__PROJECT[DOCUMENTATION_DIR]}"

   echo "CXX:" ${CXX}
   echo "CC:" ${CC}
}

function setup( )
{
   local -n LOCAL_SETUP__PROJECT=${1}

   LOCAL_SETUP__PROJECT[ROOT_DIR]=${PWD}
   LOCAL_SETUP__PROJECT[ROOT_DIR_NAME]=${PWD##*/}
   LOCAL_SETUP__PROJECT[SOURCE_DIR]=${LOCAL_SETUP__PROJECT[ROOT_DIR]}
   LOCAL_SETUP__PROJECT[PRODUCT_DIR]=${LOCAL_SETUP__PROJECT[ROOT_DIR]}/../${LOCAL_SETUP__PROJECT[ROOT_DIR_NAME]}"_product"
   LOCAL_SETUP__PROJECT[BUILD_DIR]=${LOCAL_SETUP__PROJECT[PRODUCT_DIR]}/build
   LOCAL_SETUP__PROJECT[DELIVERY_DIR]=${LOCAL_SETUP__PROJECT[PRODUCT_DIR]}/delivery
   LOCAL_SETUP__PROJECT[DOCUMENTATION_DIR]=${LOCAL_SETUP__PROJECT[PRODUCT_DIR]}/documentation
}

function clean( )
{
   local DIR=${1}

   rm -R ${DIR}
}

function configure( )
{
   local -n LOCAL_CONFIGURE__PROJECT=${1}

   cmake -B ${LOCAL_CONFIGURE__PROJECT[BUILD_DIR]} \
      -DCMAKE_INSTALL_PREFIX=${LOCAL_CONFIGURE__PROJECT[DELIVERY_DIR]} \
      -S ${LOCAL_CONFIGURE__PROJECT[SOURCE_DIR]} \
      --graphviz=${LOCAL_CONFIGURE__PROJECT[DOCUMENTATION_DIR]}/graph \
      ${CMAKE_PARAMETERS_OS}
}

function build( )
{
   local -n LOCAL_BUILD__PROJECT=${1}
   local LOCAL_TARGETS=${2}

   cmake --build ${LOCAL_BUILD__PROJECT[BUILD_DIR]} --target ${LOCAL_TARGETS} -j ${CORES}
}

function install( )
{
   local -n LOCAL_PROJECT=${1}

   cmake --build ${LOCAL_PROJECT[BUILD_DIR]} --target install
   # cmake --install ${LOCAL_PROJECT[BUILD_DIR]} --prefix ${LOCAL_PROJECT[DELIVERY_DIR]}
}

function archive( )
{
   local -n LOCAL_ARCHIVE__PROJECT=${1}

   ARCHIVE_NAME=${LOCAL_ARCHIVE__PROJECT[ROOT_DIR_NAME]}_$(date +'%Y-%m-%d_%H-%M-%S')
   zip -r ../${ARCHIVE_NAME} ../${LOCAL_ARCHIVE__PROJECT[ROOT_DIR_NAME]}
   echo ${ARCHIVE_NAME}
}

function gpb_generate( )
{
   local SOURCE_DIR=${1}
   local DESTINATION_DIR=${2}
   local PROTO_FILE=${3}

   protoc -I=${SOURCE_DIR} --cpp_out=${DESTINATION_DIR} ${SOURCE_DIR}/${PROTO_FILE}
}

function adb_install( )
{
   if [[ -z ${2+x} ]]; then
      echo "Set project and installation path"
      return 255
   fi

   local -n LOCAL_PROJECT=${1}
   local LOCAL_INSTALL_PATH=${2}

   echo ${ABD_TOOL} root
   echo ${ABD_TOOL} shell mkdir -p ${LOCAL_INSTALL_PATH}
   echo ${ABD_TOOL} push ${LOCAL_PROJECT[DELIVERY_DIR]}/bin/* ${LOCAL_INSTALL_PATH}
   echo ${ABD_TOOL} shell chmod +x ${LOCAL_INSTALL_PATH}/*
   echo ${ABD_TOOL} push ${LOCAL_PROJECT[DELIVERY_DIR]}/lib/* ${LOCAL_INSTALL_PATH}
   echo ${ABD_TOOL} push ${LOCAL_PROJECT[DELIVERY_DIR]}/etc/* ${LOCAL_INSTALL_PATH}
   echo ${ABD_TOOL} push ${SYSROOT}/usr/lib/${TARGET}/libc++_shared.so ${LOCAL_INSTALL_PATH}

}



EXPERIMENTAL=experimental
APPLICATION=application
SERVICEBROCKER=servicebrocker
HMI=hmi
CONTROLLER=controller
CORE=core
DLT_DAEMON="dlt-daemon"

function start_process( )
{
   local -n LOCAL_SDP__PROJECT=${1}
   local LOCAL_PROCESS_NAME=${2}
   local LOCAL_TRACE=${3}

   local LOCAL_PROCESS_PID=$(pgrep -x ${LOCAL_PROCESS_NAME})
   if [ -z "${LOCAL_PROCESS_PID}" ]; then
      echo "starting" ${LOCAL_PROCESS_NAME}
      echo ${LOCAL_SDP__PROJECT[DELIVERY_DIR]}/bin/${LOCAL_PROCESS_NAME} config=${LOCAL_SDP__PROJECT[DELIVERY_DIR]}/etc/${LOCAL_PROCESS_NAME}.cfg trace=${LOCAL_TRACE} &
      ${LOCAL_SDP__PROJECT[DELIVERY_DIR]}/bin/${LOCAL_PROCESS_NAME} config=${LOCAL_SDP__PROJECT[DELIVERY_DIR]}/etc/${LOCAL_PROCESS_NAME}.cfg trace=${LOCAL_TRACE} &
      echo ${LOCAL_PROCESS_NAME} "started successfully with PID" $!
   else
      echo ${LOCAL_PROCESS_NAME} "has been started with PID" ${LOCAL_PROCESS_PID}
   fi
}

function stop_process( )
{
   local LOCAL_PROCESS_NAME=${1}

   echo "stopping" ${LOCAL_PROCESS_NAME}
   killall ${LOCAL_PROCESS_NAME}
}

function start_delivery( )
{
   local -n LOCAL_SD__PROJECT=${1}

   start_dlt_daemon LOCAL_SD__PROJECT
   # start_process LOCAL_SD__PROJECT ${SERVICEBROCKER} "DLT"
   # start_process LOCAL_SD__PROJECT ${APPLICATION} "DLT"
   # start_process LOCAL_SD__PROJECT ${HMI} "DLT"
   # start_process LOCAL_SD__PROJECT ${CONTROLLER} "DLT"
   # start_process LOCAL_SD__PROJECT ${CORE} "DLT"
}

function stop_delivery( )
{
   # stop_process ${CORE}
   # stop_process ${CONTROLLER}
   # stop_process ${HMI}
   stop_process ${APPLICATION}
   stop_process ${SERVICEBROCKER}
   stop_dlt_daemon
}

function start_test( )
{
   local -n LOCAL_SD__PROJECT=${1}

   start_process LOCAL_SD__PROJECT ${SERVICEBROCKER} "CONSOLE"
   # start_process LOCAL_SD__PROJECT ${EXPERIMENTAL} "CONSOLE"
}

function stop_test( )
{
   # stop_process ${EXPERIMENTAL}
   stop_process ${SERVICEBROCKER}
}

function start_dlt_daemon( )
{
   local -n LOCAL_SDLT__PROJECT=${1}
   local LOCAL_PROCESS_NAME=${DLT_DAEMON}

   local LOCAL_PROCESS_PID=$(pgrep -x ${LOCAL_PROCESS_NAME})
   if [ -z "${LOCAL_PROCESS_PID}" ]; then
      echo "starting" ${LOCAL_PROCESS_NAME}
      ${LOCAL_PROCESS_NAME} -d -c ${LOCAL_SDLT__PROJECT[DELIVERI_DIR]}/etc/dlt.conf
      echo ${LOCAL_PROCESS_NAME} "started successfully with PID" $!
   else
      echo ${LOCAL_PROCESS_NAME} "has been started with PID" ${LOCAL_PROCESS_PID}
   fi
}

function stop_dlt_daemon( )
{
   local LOCAL_PROCESS_NAME=${DLT_DAEMON}

   stop_process ${LOCAL_PROCESS_NAME}
}

function start( )
{
   local -n LOCAL_START__PROJECT=${1}

   _LD_LIBRARY_PATH_=${LD_LIBRARY_PATH}:${LOCAL_START__PROJECT[DELIVERY_DIR]}/lib:/usr/lib/:/usr/local/lib/
   echo export LD_LIBRARY_PATH=${_LD_LIBRARY_PATH_}
   export LD_LIBRARY_PATH=${_LD_LIBRARY_PATH_}

   # _LD_PRELOAD_=${LD_PRELOAD}:${LOCAL_START__PROJECT[DELIVERY_DIR]}/lib/libhooks.so
   # echo export LD_PRELOAD=${_LD_PRELOAD_}
   # export LD_PRELOAD=${_LD_PRELOAD_}

   start_delivery LOCAL_START__PROJECT
   # start_test LOCAL_START__PROJECT
}

function stop( )
{
   local -n LOCAL_STOP__PROJECT=${1}

   # stop_delivery LOCAL_STOP__PROJECT
   stop_test LOCAL_STOP__PROJECT
}



function main( )
{
   STARTED=$(($(date +%s%N)/1000000))

   setup PROJECT
   setup_sdk ${ENVIRONMENT_SETUP}

   case ${ACTION} in
      ${COMMAND[INFO]} | ${COMMAND[INFO_S]})
         print PROJECT
      ;;
      ${COMMAND[CLEAN]} | ${COMMAND[CLEAN_S]})
         echo "clean"
         clean ${PROJECT[PRODUCT_DIR]}
      ;;
      ${COMMAND[CONFIGURE]} | ${COMMAND[CONFIGURE_S]})
         echo "configure"
         configure PROJECT
      ;;
      ${COMMAND[BUILD]} | ${COMMAND[BUILD_S]})
         echo "build"
         build PROJECT
         install PROJECT
      ;;
      ${COMMAND[CLEAN_BUILD]} | ${COMMAND[CLEAN_BUILD_S]})
         echo "clean build"
         clean ${PROJECT[PRODUCT_DIR]}
         configure PROJECT
         build PROJECT
         install PROJECT
      ;;
      ${COMMAND[INSTALL]} | ${COMMAND[INSTALL_S]})
         echo "install"
         install PROJECT
      ;;
      ${COMMAND[ARCHIVE]} | ${COMMAND[ARCHIVE_S]})
         echo "archive"
         archive PROJECT
      ;;
      ${COMMAND[START]} | ${COMMAND[START_S]})
         echo "start"
         start PROJECT
      ;;
      ${COMMAND[STOP]} | ${COMMAND[STOP_S]})
         echo "stop"
         stop PROJECT
      ;;
      ${COMMAND[ADB_INSTALL]} | ${COMMAND[ADB_INSTALL_S]})
         echo "adb install"
         adb_install PROJECT ${ANDROID_INSTALL_PATH}
      ;;
      *)
         info
      ;;
   esac

   FINISHED=$(($(date +%s%N)/1000000))
   echo "Build time duration:" $((FINISHED - STARTED)) "miliseconds"
}



reset
main
