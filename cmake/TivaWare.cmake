include(FetchContent)

FetchContent_Declare(
  TivaWare
  GIT_REPOSITORY https://eng-git.canterbury.ac.nz/nro55/TivaWare.git
  GIT_TAG        master
)

FetchContent_MakeAvailable(TivaWare)