cmake_minimum_required(VERSION 3.2.2)

project(NativeCord)

FIND_PACKAGE(CURL)
FIND_PACKAGE(CPR)

message("Project name: ${PROJECT_NAME}")
message("Cpr include path: ${CPR_INCLUDE_DIRS}")
message("System arch: ${CMAKE_OSX_ARCHITECTURES}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-conversion-null -Wno-pointer-arith -Wno-deprecated -Wno-return-local-addr -Wno-write-strings")

set(SOURCE_FILES ${SOURCE_FILES} include/json/json.hpp)
set(SOURCE_FILES ${SOURCE_FILES} src/protocoll/Buffers/DataBuffer.cpp src/connection/PlayerConnection.cpp include/connection/PlayerConnection.h src/connection/Socket.cpp include/connection/Socket.h src/protocoll/Buffers/StreamedDataBuffer.cpp include/protocoll/StreamedDataBuffer.h src/protocoll/packet/ClientPacketHandler.cpp include/protocoll/ClientPacketHandler.h include/protocoll/Packets.h src/chat/ChatMessage.cpp include/chat/ChatMessage.h include/chat/ActionEvent.h src/connection/ServerConnection.cpp include/connection/ServerConnection.h src/protocoll/packet/ServerPacketHandler.cpp include/protocoll/ServerPacketHandler.h include/protocoll/PacketHandler.h include/connection/ConnectionState.h include/connection/Connection.h include/utils/TimeUtils.h include/utils/UUIDUtils.h include/utils/SocketUtil.h include/utils/StringUtil.h include/utils/EntityRewrite.h src/server/TabManager.cpp include/server/TabManager.h include/chat/ChatColor.h src/server/ScoreboardManager.cpp include/server/ScoreboardManager.h src/config/Configuration.cpp include/config/Configuration.h NativeCord.cpp src/server/ServerInfo.cpp include/server/ServerInfo.h src/encription/Cipper.cpp include/cryption/Cipper.h include/cryption/RSAUtil.h src/utils/Base64Utils.cpp include/utils/Base64Utils.h include/utils/HTTPUtil.h include/utils/HTTPUtil.h src/utils/UtilImplement.cpp)
set(SOURCE_FILES ${SOURCE_FILES} include/connection/GameProfile.h include/plugin/cxx/CXXPlugin.h include/plugin/Plugin.h src/plugin/java/JavaPluginManagerImpl.cpp include/plugin/java/JavaPluginManagerImpl.h include/plugin/java/JavaPlugin.h include/plugin/PluginManager.h src/plugin/handle/CXXPluginManagerImpl.cpp src/plugin/handle/CXXPluginManagerImpl.h include/log/LogUtils.h src/log/LogUtils.cpp src/plugin/java/jni/DataStorageImpl.cpp include/plugin/java/jni/DataStorageImpl.h src/plugin/java/jni/DataStorage.cpp include/plugin/java/jni/DataStorage.h include/plugin/event/PlayerConnectEvent.h include/plugin/event/Event.h include/plugin/event/EventType.h src/plugin/java/JavaPlugin.cpp src/plugin/java/JavaReflectManager.cpp include/plugin/java/JavaReflectManager.h src/plugin/event/EventType.cpp include/plugin/event/EventHelper.h src/plugin/event/EventHelper.cpp include/NativeCord.h include/utils/VectorUtils.h)

link_libraries(pthread uuid z crypto) #z = zlib
link_libraries(CXXTerminal)

#TODO
############################### YAML ################################
include_directories(libs/yaml-cpp/include/)
link_libraries(yaml-cpp)
#####################################################################

################################ CPR ################################
set(BUILD_CPR_TESTS false)
if(EXISTS lib/ AND EXISTS lib/libcpr.a)
    message("Found builded cpr")
    link_directories(lib)
else()
    message("Building cpr")
    if(NOT EXISTS libs/cpr/CMakeLists.txt)
        if(EXISTS libs/cpr/CMakeFiles AND NOT EXISTS libs/cpr/CMakeLists.txt)
            message(FATAL_ERROR "Missing cpr. Maybe you havnt init all git submudules yet?\nRun command: 'rm -r libs/* && git submodule update --init --recursive'")
        else()
            message(FATAL_ERROR "Missing cpr. Maybe you havnt init all git submudules yet?\nRun command: 'git submodule update --init --recursive'")
        endif()
    endif()
    add_subdirectory(libs/cpr)
endif()

if(NOT ${CPR_INCLUDE_DIRS})
    message("Linking directories: ${CPR_INCLUDE_DIRS}/../lib/")
    link_directories(${CPR_INCLUDE_DIRS}/../lib/)
endif()
include_directories(${CPR_INCLUDE_DIRS})
link_libraries(${CURL_LIBRARIES})
link_libraries(${CPR_LIBRARIES})
#####################################################################

########################### Java ####################################
message("Sarching vor jni")
find_package(JNI)
message("JNI inclide dir: ${JNI_INCLUDE_DIRS} AND ${JAVA_INCLUDE_PATH2}")
message("JNI server so: ${JAVA_JVM_LIBRARY}")

include_directories(${JNI_INCLUDE_DIRS})
link_libraries(${JAVA_JVM_LIBRARY})
#####################################################################

################### 32 Bit [DONT WORK :/] ###########################
#add_definitions(-DCURL_32_BIT)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
#####################################################################

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

add_definitions(-DNATIVECORD_VERSION="0.5 ALPHA")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})