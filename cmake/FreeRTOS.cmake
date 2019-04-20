include(FetchContent)

FetchContent_Declare(
  FreeRTOS
  GIT_REPOSITORY https://eng-git.canterbury.ac.nz/nro55/FreeRTOS.git
  GIT_TAG        master
)

FetchContent_MakeAvailable(FreeRTOS)