cmake_minimum_required(VERSION 3.0)

project(MyWxWidgetsClientApp)

# wxWidgets 경로 설정
if (WIN32)
	SET(wxWidgets_ROOT_DIR "D:/sdks/wxWidgets-3.2.6")
	SET(wxWidgets_LIB_DIR "D:/sdks/wxWidgets-3.2.6/lib/gcc_dll")
	set(wxWidgets_CONFIGURATION mswu)
	set(wxWidgets_EXCLUDE_COMMON_LIBRARIES true)
elseif (UNIX)
	set(wxWidgets_CONFIGURATION "")
	set(wxWidgets_EXCLUDE_COMMON_LIBRARIES true)
endif()

# wxWidgets 패키지 찾기
find_package(wxWidgets REQUIRED COMPONENTS gl core base OPTIONAL_COMPONENTS net)

# wxWidgets 사용 설정 파일 포함
include(${wxWidgets_USE_FILE})

# 실행 파일 생성
add_executable(MyWxWidgetsClientApp wxSocket_Client.cpp)

# wxWidgets 라이브러리 링크
target_link_libraries(MyWxWidgetsClientApp ${wxWidgets_LIBRARIES})
