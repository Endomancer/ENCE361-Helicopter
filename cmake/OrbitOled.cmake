include(FetchContent)

FetchContent_Declare(
  OrbitOled
  GIT_REPOSITORY https://eng-git.canterbury.ac.nz/nro55/OrbitOled.git
  GIT_TAG        master
)

FetchContent_MakeAvailable(OrbitOled)