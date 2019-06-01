include(FetchContent)

FetchContent_Declare(
  OrbitOLED
  GIT_REPOSITORY https://eng-git.canterbury.ac.nz/nro55/tiva-libraries
  GIT_TAG        OrbitOLED
)

FetchContent_MakeAvailable(OrbitOLED)