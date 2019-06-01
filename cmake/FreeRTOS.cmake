include(FetchContent)

FetchContent_Declare(
  FreeRTOS
  GIT_REPOSITORY https://eng-git.canterbury.ac.nz/nro55/tiva-libraries
  GIT_TAG        FreeRTOS
)

FetchContent_MakeAvailable(FreeRTOS)