include(GoogleTest)

add_library(test_framehandler STATIC
    $<TARGET_OBJECTS:common>
    $<TARGET_OBJECTS:databasemock>
    $<TARGET_OBJECTS:displaymock>
    $<TARGET_OBJECTS:framehandler>
    $<TARGET_OBJECTS:datahandler>
)

function(GUNITTEST_FRAMEHANDLER)
    set(oneValueArgs NAME WORKING_DIRECTORY)
    set(multiValueArgs LIBS FILES)
    cmake_parse_arguments(GUNITTEST_FRAMEHANDLER "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT GUNITTEST_FRAMEHANDLER_WORKING_DIRECTORY)
        set(GUNITTEST_FRAMEHANDLER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    endif()

    GUNITTEST(NAME "framehandler_${GUNITTEST_FRAMEHANDLER_NAME}"
        LIBS ${GUNITTEST_FRAMEHANDLER_LIBS} test_framehandler pgw pglMock
        WORKING_DIRECTORY ${GUNITTEST_FRAMEHANDLER_WORKING_DIRECTORY}
        FILES ${GUNITTEST_FRAMEHANDLER_FILES}
    )
endfunction()

set(DDHBUILDERS
    ddhbuilders/DdhBuilder.h
    ddhbuilders/DdhBuilder.cpp
    ddhbuilders/DdhPageBuilder.h
    ddhbuilders/DdhPageBuilder.cpp
    ddhbuilders/DdhPanelBuilder.h
    ddhbuilders/DdhPanelBuilder.cpp
    ddhbuilders/DdhReferenceBitmapFieldBuilder.h
    ddhbuilders/DdhReferenceBitmapFieldBuilder.cpp
    ddhbuilders/DdhStaticBitmapFieldBuilder.h
    ddhbuilders/DdhStaticBitmapFieldBuilder.cpp
)

GUNITTEST_FRAMEHANDLER(
    NAME FrameHandlerTest
    FILES FrameHandlerTest.cpp
          FrameHandlerCorrupter.h
          MockWidget.h
          TestCanvas.h
          WidgetTestBase.h
          ${DDHBUILDERS}
)

GUNITTEST_FRAMEHANDLER(
    NAME WidgetsTest
    FILES BitmapFieldTest.cpp
          FieldTest.cpp
          FrameTest.cpp
          MockWidget.h
          PanelTest.cpp
          ReferenceBitmapFieldTest.cpp
          TestCanvas.h
          WidgetPoolHelper.h
          WidgetPoolHelper.cpp
          WidgetTest.cpp
          WidgetTestBase.h
          WindowTest.cpp
          ${DDHBUILDERS}
)

GUNITTEST_FRAMEHANDLER(
    NAME WidgetPoolTest
    FILES WidgetPoolTest.cpp
          WidgetPoolHelper.h
          WidgetPoolHelper.cpp
          WidgetPoolCorrupter.h
          ${DDHBUILDERS}
)
