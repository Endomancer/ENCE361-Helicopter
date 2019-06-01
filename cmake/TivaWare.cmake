include(FetchContent)

FetchContent_Declare(
  TivaWare
  GIT_REPOSITORY https://eng-git.canterbury.ac.nz/nro55/tiva-libraries
  GIT_TAG        TivaWare
)

FetchContent_MakeAvailable(TivaWare)