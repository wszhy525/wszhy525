# Configs
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++14

#Libraries
unix: CONFIG += link_pkgconfig

#OpenCV
unix: PKGCONFIG += opencv

#CUDA
unix:!macx: LIBS += -L$$PWD/../../../usr/local/cuda/lib64/-lcudart
unix:!macx: LIBS += -L$$PWD/../../../usr/local/cuda/lib64/-lcuda
unix:!macx: LIBS += -L$$PWD/../../../usr/local/cuda/lib64/-lcublas
unix:!macx: LIBS += -L$$PWD/../../../usr/local/cuda/lib64/-lcurand
INCLUDEPATH += $$PWD/../../../usr/local/cuda/include
DEPENDPATH += $$PWD/../../../usr/local/cuda/include

#V4L2
unix:!macx: LIBS += -lpthread
unix:!macx: LIBS += -lv4l2

#Darknet
unix:!macx: LIBS += -L$$PWD/Darknet/ -ldarknet
INCLUDEPATH += $$PWD/Darknet
DEPENDPATH += $$PWD/Darknet

#Source and header files
HEADERS += \
    General/singleton.hpp \
    General/opencv_extended.h \
    General/numeric_rm.h \
    General/General.h \
    Driver/RMVideoCapture.hpp \
    Serials/Serial.h \
    Main/ImgProdCons.h \
    Pose/AngleSolver.hpp \
    Armor/ArmorDetector.h \
    Rune/Rune.h \
    Strategies/Strategies.h \
    Windmill/Windmill.h \
    Driver/DHCamera/GenICam/CAPI/IntTypes.h \
    Driver/DHCamera/GenICam/CAPI/SDK.h \
    Driver/DHCamera/GenICam/GigE/GigECamera.h \
    Driver/DHCamera/GenICam/GigE/GigEInterface.h \
    Driver/DHCamera/GenICam/Usb/UsbCamera.h \
    Driver/DHCamera/GenICam/Usb/UsbInterface.h \
    Driver/DHCamera/GenICam/AcquisitionControl.h \
    Driver/DHCamera/GenICam/AnalogControl.h \
    Driver/DHCamera/GenICam/Camera.h \
    Driver/DHCamera/GenICam/Defs.h \
    Driver/DHCamera/GenICam/DeviceControl.h \
    Driver/DHCamera/GenICam/DigitalIOControl.h \
    Driver/DHCamera/GenICam/ErrorCode.h \
    Driver/DHCamera/GenICam/EventSubscribe.h \
    Driver/DHCamera/GenICam/Frame.h \
    Driver/DHCamera/GenICam/ImageFormatControl.h \
    Driver/DHCamera/GenICam/IntegratedCamera.h \
    Driver/DHCamera/GenICam/ISPControl.h \
    Driver/DHCamera/GenICam/ParameterNode.h \
    Driver/DHCamera/GenICam/PixelType.h \
    Driver/DHCamera/GenICam/StreamSource.h \
    Driver/DHCamera/GenICam/System.h \
    Driver/DHCamera/GenICam/TransportLayerControl.h \
    Driver/DHCamera/GenICam/UserSetControl.h \
    Driver/DHCamera/Infra/Detail/allocatorstringstorage.h \
    Driver/DHCamera/Infra/Detail/atomic_count.hpp \
    Driver/DHCamera/Infra/Detail/atomic_count_gcc.hpp \
    Driver/DHCamera/Infra/Detail/atomic_count_gcc_x86.hpp \
    Driver/DHCamera/Infra/Detail/atomic_count_pthreads.hpp \
    Driver/DHCamera/Infra/Detail/atomic_count_solaris.hpp \
    Driver/DHCamera/Infra/Detail/atomic_count_sync.hpp \
    Driver/DHCamera/Infra/Detail/atomic_count_win32.hpp \
    Driver/DHCamera/Infra/Detail/atomic_count_win64.hpp \
    Driver/DHCamera/Infra/Detail/construct.h \
    Driver/DHCamera/Infra/Detail/cowstringopt.h \
    Driver/DHCamera/Infra/Detail/flex_string.h \
    Driver/DHCamera/Infra/Detail/flex_string_details.h \
    Driver/DHCamera/Infra/Detail/flex_string_shell.h \
    Driver/DHCamera/Infra/Detail/ministringstorage.h \
    Driver/DHCamera/Infra/Detail/pp.hpp \
    Driver/DHCamera/Infra/Detail/pp_gcc.hpp \
    Driver/DHCamera/Infra/Detail/pp_msvc.hpp \
    Driver/DHCamera/Infra/Detail/simplestringstorage.h \
    Driver/DHCamera/Infra/Detail/smallstringopt.h \
    Driver/DHCamera/Infra/Detail/vectorstringstorage.h \
    Driver/DHCamera/Infra/Types/IntTypes.h \
    Driver/DHCamera/Infra/Types/Types.h \
    Driver/DHCamera/Infra/Allocator.h \
    Driver/DHCamera/Infra/Assert.h \
    Driver/DHCamera/Infra/AtomicCount.h \
    Driver/DHCamera/Infra/CString.h \
    Driver/DHCamera/Infra/Defs.h \
    Driver/DHCamera/Infra/Error.h \
    Driver/DHCamera/Infra/File.h \
    Driver/DHCamera/Infra/Function.h \
    Driver/DHCamera/Infra/FunctionTemplate.h \
    Driver/DHCamera/Infra/Guard.h \
    Driver/DHCamera/Infra/IntTypes.h \
    Driver/DHCamera/Infra/Mutex.h \
    Driver/DHCamera/Infra/PreProcessor.h \
    Driver/DHCamera/Infra/PrintLog.h \
    Driver/DHCamera/Infra/ReadWriteMutex.h \
    Driver/DHCamera/Infra/RecursiveMutex.h \
    Driver/DHCamera/Infra/Semaphore.h \
    Driver/DHCamera/Infra/Signal.h \
    Driver/DHCamera/Infra/SignalTemplate.h \
    Driver/DHCamera/Infra/StaticAssert.h \
    Driver/DHCamera/Infra/String.h \
    Driver/DHCamera/Infra/System.h \
    Driver/DHCamera/Infra/Thread.h \
    Driver/DHCamera/Infra/ThreadSpecific.h \
    Driver/DHCamera/Infra/Time.h \
    Driver/DHCamera/Infra/Timer.h \
    Driver/DHCamera/Infra/Types.h \
    Driver/DHCamera/Infra/Vector.h \
    Driver/DHCamera/Infra/Version.h \
    Driver/DHCamera/Memory/Detail/sp_counted_base/sp_counted_base.h \
    Driver/DHCamera/Memory/Detail/sp_counted_base/sp_counted_base_gcc_ia64.hpp \
    Driver/DHCamera/Memory/Detail/sp_counted_base/sp_counted_base_gcc_x86.hpp \
    Driver/DHCamera/Memory/Detail/sp_counted_base/sp_counted_base_impl.hpp \
    Driver/DHCamera/Memory/Detail/sp_counted_base/sp_counted_base_w32.hpp \
    Driver/DHCamera/Memory/Detail/checked_delete.hpp \
    Driver/DHCamera/Memory/Detail/MemPool.h \
    Driver/DHCamera/Memory/Detail/shared_count.h \
    Driver/DHCamera/Memory/Detail/singleton.h \
    Driver/DHCamera/Memory/Detail/singleton_pool.h \
    Driver/DHCamera/Memory/Detail/smartptr_detail.h \
    Driver/DHCamera/Memory/Detail/sp_counted_base.hpp \
    Driver/DHCamera/Memory/Detail/sp_counted_impl.hpp \
    Driver/DHCamera/Memory/Block.h \
    Driver/DHCamera/Memory/Buffer.h \
    Driver/DHCamera/Memory/Defs.h \
    Driver/DHCamera/Memory/EnableSharedFromThis.h \
    Driver/DHCamera/Memory/Packet.h \
    Driver/DHCamera/Memory/PoolAllocator.h \
    Driver/DHCamera/Memory/RefCount.h \
    Driver/DHCamera/Memory/ScopedPtr.h \
    Driver/DHCamera/Memory/SharedPtr.h \
    Driver/DHCamera/Memory/SharePacketHelper.h \
    Driver/DHCamera/Memory/SmartObjectPtr.h \
    Driver/DHCamera/Memory/WeakPtr.h \
    Driver/DHVideoCapture.hpp



SOURCES += \
    General/opencv_extended.cpp \
    General/numeric_rm.cpp \
    Driver/RMVideoCapture.cpp \
    Serials/Serial.cpp \
    Main/ImgProdCons.cpp \
    Pose/AngleSolver.cpp \
    Armor/ArmorDetector.cpp \
    Rune/Rune.cpp \
#   To test a sigular module, uncomment only one of the following
#    Armor/test_armor.cpp
#    Driver/test_camera.cpp
#    Serials/test_serials.cpp
#    Pose/test_angle_2.cpp
#    Pose/test_angle_1.cpp
#   To test a singular module with multi-thread, uncomment main.cpp
#   and one of the others
    Main/main.cpp \
#    Main/test_armor_temp.cpp
#    Main/test_producer.cpp
#    Main/test_armor.cpp
#    Main/test_armor_solver.cpp
#    Main/test_rune.cpp \
#    Main/test_armor_solver_serial.cpp
#     Main/test_recording_video.cpp
    Main/test_infantry.cpp \
    Strategies/Strategies.cpp \
    Driver/DHVideoCapture.cpp \
    Windmill/Windmill.cpp \



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Driver/DHCamera/lib/release/ -lMVSDK
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Driver/DHCamera/lib/debug/ -lMVSDK
else:unix: LIBS += -L$$PWD/Driver/DHCamera/lib/ -lMVSDK

INCLUDEPATH += $$PWD/Driver/DHCamera
DEPENDPATH += $$PWD/Driver/DHCamera
